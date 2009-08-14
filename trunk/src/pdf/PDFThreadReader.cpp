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

namespace pdf
{

static const quint32 MAX_ACCEPT_MEM_USE = 80;

struct CompressedImage
{
	static int compressLevel;

	QByteArray data;
	
	QSize imgsize;
	QImage::Format format;

	void compress(const QImage& image)
	{
		pltGuardTimer guard(__FUNCTION__);

		data = qCompress(image.bits(), image.numBytes(), compressLevel);

		imgsize = image.size();
		format = image.format();

		double used = guard.elapsed();

		if (used > 4)
		{
			--compressLevel;
		}
		else if (used < 1)
		{
			++compressLevel;
		}

		compressLevel = qBound(1, compressLevel, 9);

		__LOG(QString("Compress level %1, ratio %2/%3 - %4")
			.arg(compressLevel)
			.arg(data.size())
			.arg(image.numBytes())
			.arg(data.size() * 1.0 / image.numBytes()));
	}

	QImage uncompress()
	{
		pltGuardTimer guard(__FUNCTION__);

		QByteArray imgData = qUncompress(data);
		QImage image(imgsize, format);

		if (!image.isNull() && !imgData.isNull())
		{
			qMemCopy(image.bits(), imgData.data(), image.numBytes());
		}

		return image;
	}

	int size()
	{
		return data.size();
	}
};

int CompressedImage::compressLevel = 5;

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

	volatile int requestPageNo;
	volatile int renderingPageNo;

	PDFThreadReader::ZoomLevel zoomLevel;
	int screenWidth;
	int screenHeight;
	int rotation;

	volatile bool stopFlag;

	bool convert16bits;
	QSize thumbSize;

	PDFThreadReaderImpl()
		: requestMutex(QMutex::Recursive)
		, cacheMutex(QMutex::Recursive)
		, renderingPageNo(-1)
		, convert16bits(false)
		, thumbSize (128, 128)
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

		CompressedImage* thumb = new CompressedImage();
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
	}
}


void 
PDFThreadReader::askRender(int pageNo)
{
	impl_->requestPageNo = pageNo;
	impl_->clearRequest();

	if (this->hasPage(pageNo))
	{
		QImage image = this->pageImage(pageNo);
		QImage thumb = this->pageThumb(pageNo);

		if (!image.isNull())
		{
			emit rendered(pageNo, image, thumb);
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
	}

	this->prefetch(pageNo, 2);

	if (!this->isRunning())
	{
		impl_->stopFlag = false;
		this->start(QThread::LowPriority);
	}
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
					emit rendered(pageNo, 
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

		//check
		if (image.isNull())
		{
			__THROW_L(PDFException, "Out of memory");
		}

		//release memory
		if (plutoApp->memoryStatus().memoryLoad > MAX_ACCEPT_MEM_USE)
		{
			this->clearEngineBuffer();
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
			if (pageNo == impl_->requestPageNo)
			{
				//emit directly
				emit rendered(pageNo, image, thumb);

				cache(pageNo, image);
			}
			else
			{
				//cache then emit
				cache(pageNo, image);

				if (pageNo == impl_->requestPageNo)
				{
					emit rendered(pageNo, image, thumb);
				}
			}
		}
	}
	catch (PDFException& e)
	{
		emit renderError(QString("Render p%1 error - %2")
			.arg(pageNo + 1).arg(e.what()));
	}
}


void 
PDFThreadReader::cache(int pageNo, const QImage& image)
{
	CompressedImage* page = impl_->addIntoCache(pageNo, image);

	QString cachedMsg = QString("Cached p%1 (%2), cost %3k (%4m / %5m)")
		.arg(pageNo + 1)
		.arg(impl_->pages.size())
		.arg(page->size() / 1024)
		.arg(impl_->pages.totalCost() * 1.0 / (1024 * 1024), 0, 'f', 2)
		.arg(impl_->thumbs.totalCost() * 1.0 / (1024 * 1024), 0, 'f', 2);

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

