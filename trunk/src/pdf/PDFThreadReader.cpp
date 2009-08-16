/*******************************************************************************
**
**  NAME:			PDFThreadReader.cpp
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class PDFThreadReader
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/08/13
**
**
*******************************************************************************/
#include "stdafx.h"
#include "PDFThreadReader.h"

#include "../src/3rdparty/zlib/zlib.h"

//rewrite qt version for save memory purpose
bool qUncompress_(const uchar* data, int nbytes, uchar* target, ulong trgsz)
{
	if (!data) {
		qWarning("qUncompress: Data is null");
		return false;
	}
	if (nbytes <= 4) {
		if (nbytes < 4 || (data[0]!=0 || data[1]!=0 || data[2]!=0 || data[3]!=0))
			qWarning("qUncompress: Input data is corrupted");
		return false;
	}

	ulong expectedSize = (data[0] << 24) | (data[1] << 16) |
		(data[2] <<  8) | (data[3]      );
	ulong len = qMax(expectedSize, 1ul);


	int res;
	do {
		if (len > trgsz)
			return false;

		res = ::uncompress(target, &len, (uchar*)data+4, nbytes-4);

		switch (res) {
		case Z_OK:
			break;
		case Z_MEM_ERROR:
			qWarning("qUncompress: Z_MEM_ERROR: Not enough memory");
			break;
		case Z_BUF_ERROR:
			len *= 2;
			break;
		case Z_DATA_ERROR:
			qWarning("qUncompress: Z_DATA_ERROR: Input data is corrupted");
			break;
		}
	} while (res == Z_BUF_ERROR);

	return res == Z_OK;
}


namespace pdf
{

static const quint32 MAX_ACCEPT_MEM_USE = 80;

struct CompressedImage
{
	static int compressLevel;

	QByteArray data;
	
	QSize imgsize;
	QImage::Format format;

	bool useDefaultLevel;

	CompressedImage(bool useDefaultLevel_ = false)
		: useDefaultLevel (useDefaultLevel_)
	{
	}


	void compress(const QImage& image)
	{
		pltGuardTimer guard(__FUNCTION__);

		int level = useDefaultLevel ?  -1 : compressLevel;

		data = qCompress(image.bits(), image.numBytes(), level);

		imgsize = image.size();
		format = image.format();

		if (!useDefaultLevel)
		{

		}

		adjustCompressLevel(guard.elapsed());

		__LOG(QString("Compress level %1, ratio %2/%3 - %4")
			.arg(compressLevel)
			.arg(data.size())
			.arg(image.numBytes())
			.arg(data.size() * 1.0 / image.numBytes()));
	}

	void adjustCompressLevel(double used)
	{
		if (used > 1)
		{
			--compressLevel;
		}
		else if (used < 0.5)
		{
			++compressLevel;
		}

		compressLevel = qBound(1, compressLevel, 4);
	}

	QImage uncompress()
	{
		pltGuardTimer guard(__FUNCTION__);

		QImage image(imgsize, format);

		if (!image.isNull())
		{
			bool success = qUncompress_(reinterpret_cast<const uchar*>(data.constData()), 
				data.size(),
				image.bits(),
				image.numBytes());

			if (!success)
				image = QImage();//failed, release
		}

		return image;
	}

	int size()
	{
		return data.size();
	}
};

int CompressedImage::compressLevel = 1;

struct Request
{
	int pageNo;
};

struct PDFThreadReader::PDFThreadReaderImpl
{
	QCache<int, CompressedImage> pages;
	QCache<int, CompressedImage> thumbs;

	QQueue<int>	requests;

	QMutex requestMutex;
	QMutex cacheMutex;

	QSemaphore renderSemaphore;

	volatile int requestPageNo;
	volatile int renderingPageNo;

	PDFThreadReader::ZoomLevel zoomLevel;
	int screenWidth;
	int screenHeight;
	int rotation;

	volatile bool stopFlag;

	bool convert16bits;
	QSize thumbSize;

	int prefetchNum;
	bool enableCaching;

	PDFThreadReaderImpl()
		: requestMutex(QMutex::Recursive)
		, cacheMutex(QMutex::Recursive)
		, renderingPageNo(-1)
		, convert16bits(false)
		, thumbSize (128, 128)
		, prefetchNum (2)
		, enableCaching (true)
	{
	}

	CompressedImage* addIntoCache(int pageNo, const QImage& image)
	{
		CompressedImage* page = new CompressedImage();
		page->compress(image);

		if (page->size() != 0 && page->size() < pages.maxCost())
		{
			QMutexLocker locker(&cacheMutex);
			
			pages.insert(pageNo, page, page->size());
		}
		else
		{
			SAFE_DELETE(page);
		}

		return page;
	}

	void clearCache()
	{
		QMutexLocker locker(&cacheMutex);

		pages.clear();
	}

	QImage addThumb(int pageNo, const QImage& image)
	{
		QImage thumbImg = QImage(image.scaled(thumbSize, 
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation));

		CompressedImage* thumb = new CompressedImage(true);
		thumb->compress(thumbImg);

		if (thumb->size() != 0 && thumb->size() < thumbs.maxCost())
		{
			thumbs.insert(pageNo, thumb, thumb->size());
		}
		else
		{
			SAFE_DELETE(thumb);
		}

		__LOG(QString("Add thumb, size : %1").arg(thumb->size()));

		return thumbImg;
	}

	void clearThumbs()
	{
		thumbs.clear();
	}

	void enqueueRequest(int pageNo)
	{
		QMutexLocker locker(&requestMutex);

		requests.enqueue(pageNo);
	}

	int dequeueRequest()
	{
		QMutexLocker locker(&requestMutex);

		return requests.dequeue();
	}

	void clearRequest()
	{
		QMutexLocker locker(&requestMutex);

		requests.clear();
	}

	void setRenderingPageNo(int pageNo)
	{
		renderingPageNo = pageNo;
	}

	void resetRenderingPageNo()
	{
		renderingPageNo = -1;
	}

};


PDFThreadReader::PDFThreadReader(QObject *parent /*= NULL*/, 
	int cacheSize /*= 1024 * 1024 * 10*/,
	int gcBufSize /*= 1024 * 512*/)
	: QThread(parent)
	, PDFReader(gcBufSize)
	, impl_ (new PDFThreadReaderImpl)
{
	impl_->pages.setMaxCost(cacheSize);
	impl_->thumbs.setMaxCost(cacheSize / 5);
}



PDFThreadReader::~PDFThreadReader()
{
	this->stop();

	SAFE_DELETE(impl_);
}


bool 
PDFThreadReader::hasPage(int pageNo) const
{
	return impl_->pages.contains(pageNo);
}


QImage 
PDFThreadReader::pageImage(int pageNo) const
{
	QImage image;
	CompressedImage* page = impl_->pages.object(pageNo);

	if (page)
	{
		image = page->uncompress();
	}

	return image;
}


PDFThreadReader::ZoomLevel 
PDFThreadReader::zoomLevel() const
{
	return impl_->zoomLevel;
}


void 
PDFThreadReader::open(const QString& pdfFile, 
					  const QString& password /*= ""*/)
{
	if (QFileInfo(pdfFile) != QFileInfo(this->filePath()))
	{
		this->stopAndClean();

		PDFReader::open(pdfFile, password);
	}
}


void 
PDFThreadReader::setMargin(double leftPercent /*= 0.05*/, 
						   double rightPercent /*= 0.05*/,
						   double topPercent /*= 0.0*/, 
						   double bottomPercent /*= 0.0*/,
						   int ignorePages /*= 0*/)
{
	if (qAbs(this->leftMargin() - leftPercent) > 0.01 ||
		qAbs(this->rightMargin() - rightPercent) > 0.01 ||
		qAbs(this->topMargin() - topPercent) > 0.01 ||
		qAbs(this->bottomMargin() - bottomPercent) > 0.01)
	{
		this->stopAndClean();

		PDFReader::setMargin(leftPercent, 
			rightPercent,
			topPercent, 
			bottomPercent, 
			ignorePages);
	}
}


void 
PDFThreadReader::setRenderParams(ZoomLevel level, 
								 int screenW, 
								 int screenH, 
								 int rotation /*= 0*/)
{
	if (impl_->zoomLevel != level ||
		impl_->screenWidth != screenW ||
		impl_->screenHeight != screenH ||
		impl_->rotation != rotation)
	{
		this->stopAndClean();

		impl_->zoomLevel = level;
		impl_->screenWidth = screenW;
		impl_->screenHeight = screenH;
		impl_->rotation = rotation;

		if (level <= FitWidth250)
		{
			impl_->enableCaching = false;
			impl_->prefetchNum = 0;
		}
		else if (level < FitWidth150)
		{
			impl_->enableCaching = true;
			impl_->prefetchNum = 1;
		}
		else
		{
			impl_->enableCaching = true;
			impl_->prefetchNum = 2;
		}
	}
}


void 
PDFThreadReader::enableCaching(bool enable)
{
	impl_->enableCaching = enable;
}


void 
PDFThreadReader::askRender(int pageNo, bool wait)
{
	impl_->requestPageNo = pageNo;
	impl_->clearRequest();

	if (this->hasPage(pageNo))
	{
		QImage image = this->pageImage(pageNo);
		QImage thumb = this->pageThumb(pageNo);

		if (!image.isNull())
		{
			this->emitRendered(pageNo, image, thumb);
		}
		else
		{
			emit renderError(QString("Render p%1 error - %2")
				.arg(pageNo + 1).arg("Out of memory"));
		}
	}
	else
	{
		if (impl_->renderingPageNo != pageNo)
			impl_->enqueueRequest(pageNo);

		emit rendering(QString("Rendering p%1, please wait").arg(pageNo + 1));

		if (!this->isRunning())
		{
			//start
			impl_->stopFlag = false;
			this->start(QThread::LowPriority);

			if (wait)
			{
				impl_->renderSemaphore.acquire(
					impl_->renderSemaphore.available() + 1);
			}
		}
	}

	this->prefetch(pageNo, impl_->prefetchNum);
}


void 
PDFThreadReader::run()
{
	while (!impl_->stopFlag)
	{
		if (impl_->requests.isEmpty())
		{
			this->msleep(100);
		}
		else
		{
			int pageNo = impl_->dequeueRequest();
			
			if (this->hasPage(pageNo))
			{
				//do not need to render, already has page
				if (pageNo == impl_->requestPageNo)
				{
					this->emitRendered(pageNo, 
						this->pageImage(pageNo),
						this->pageThumb(pageNo));
				}
			}
			else
			{
				//need to render
				this->renderPage(pageNo);
			}
		}
	}
}


void 
PDFThreadReader::stop()
{
	if (this->isRunning())
	{
		impl_->stopFlag = true;

		if (!this->wait(10000))
		{
			this->terminate();
		}
	}
}


void 
PDFThreadReader::stopAndClean()
{
	this->stop();

	impl_->clearCache();
	impl_->clearThumbs();
	impl_->clearRequest();

	impl_->resetRenderingPageNo();

	CompressedImage::compressLevel = 1;
}


void 
PDFThreadReader::prefetch(int pageNo, int count)
{
	int nextPage = pageNo + 1;

	for (int i = 0; i < count; ++i)
	{
		if (!this->hasPage(nextPage) && nextPage < this->pageCount())
		{
			impl_->enqueueRequest(nextPage);
		}

		++nextPage;
	}
}


void 
PDFThreadReader::renderPage(int pageNo)
{
	try
	{
		impl_->setRenderingPageNo(pageNo);

		QImage image = this->render(pageNo, 
			impl_->zoomLevel,
			impl_->screenWidth,
			impl_->screenHeight,
			impl_->rotation);

		//release memory
		this->clearEngineBuffer();

		//check
		if (image.isNull())
		{
			__THROW_L(PDFException, "Out of memory");
		}

		//add thumb
		QImage thumb;
		
		if (this->hasPageThumb(pageNo))
		{
			thumb = this->pageThumb(pageNo);
		}
		{
			thumb = impl_->addThumb(pageNo, image);
		}

		//convert to 16bits
		if (impl_->convert16bits)
		{
			image = image.convertToFormat(QImage::Format_RGB16);
		}

		if (!impl_->stopFlag)
		{
			//cache then emit
			if (impl_->enableCaching)
				this->cache(pageNo, image);

			if (pageNo == impl_->requestPageNo)
			{
				this->emitRendered(pageNo, image, thumb);
			}
		}

		impl_->resetRenderingPageNo();
	}
	catch (PDFException& e)
	{
		emit renderError(QString("Render p%1 error - %2")
			.arg(pageNo + 1).arg(e.what()));
	}
}


void 
PDFThreadReader::emitRendered(int pageNo, 
							  const QImage& image,
							  const QImage& thumb)
{
	impl_->renderSemaphore.release();

	emit rendered(pageNo, image, thumb);
}


void 
PDFThreadReader::cache(int pageNo, const QImage& image)
{
	CompressedImage* page = impl_->addIntoCache(pageNo, image);

	QString cachedMsg = QString("Cached p%1 (%2), cost %3k (%4m / %5m) - [%6]")
		.arg(pageNo + 1)
		.arg(impl_->pages.size())
		.arg(page->size() / 1024)
		.arg(impl_->pages.totalCost() * 1.0 / (1024 * 1024), 0, 'f', 1)
		.arg(impl_->thumbs.totalCost() * 1.0 / (1024 * 1024), 0, 'f', 1)
		.arg(CompressedImage::compressLevel);

	emit cached(cachedMsg); __LOG(cachedMsg);

	if (page == NULL && pageNo == impl_->requestPageNo)
	{
		//too large, can not add into cache
		//remove the later request
		impl_->clearRequest();
	}
}


void 
PDFThreadReader::setConvertTo16Bits(bool convert)
{
	impl_->convert16bits = convert;
}


void 
PDFThreadReader::setThumbSize(QSize size)
{
	impl_->thumbSize = size;
}


bool 
PDFThreadReader::hasPageThumb(int pageNo) const
{
	return impl_->thumbs.contains(pageNo);
}


QImage 
PDFThreadReader::pageThumb(int pageNo) const
{
	QImage thumbImg;
	CompressedImage* thumb = impl_->thumbs.object(pageNo);

	if (thumb)
	{
		thumbImg = thumb->uncompress();
	}

	return thumbImg;
}


//end namespace
}

