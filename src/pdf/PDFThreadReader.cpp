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

struct PDFPage
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

int PDFPage::compressLevel = 5;

struct Request
{
	int pageNo;
};

struct PDFThreadReader::PDFThreadReaderImpl
{
	QCache<int, PDFPage> pages;
	bool noCacheNext;

	QQueue<int>	requests;
	QMutex requestMutex;

	volatile int requestPageNo;
	volatile int renderingPageNo;

	PDFThreadReader::ZoomLevel zoomLevel;
	int screenWidth;
	int screenHeight;
	int rotation;

	volatile bool stopFlag;

	PDFThreadReaderImpl()
		: requestMutex(QMutex::Recursive)
		, noCacheNext (false)
	{
	}

	PDFPage* addIntoCache(int pageNo, const QImage& image)
	{
		PDFPage* page = NULL;
		
		if (noCacheNext)
		{
			noCacheNext = false;
		}
		else
		{
			page = new PDFPage();

			if (page)
			{
				page->compress(image);

				if (page->size() != 0 && page->size() < pages.maxCost())
				{
					pages.insert(pageNo, page, page->size());
				}
				else
				{
					SAFE_DELETE(page);
				}
			}
		}

		return page;
	}

	void clearCache(bool noCacheNext)
	{
		pages.clear();

		this->noCacheNext = noCacheNext;
	}

	void clearRequest()
	{
		QMutexLocker locker(&requestMutex);

		requests.clear();
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
}



PDFThreadReader::~PDFThreadReader()
{
	this->stop();

	SAFE_DELETE(impl_);
}


bool 
PDFThreadReader::hasPage(int pageNo) const
{
	return impl_->pages.object(pageNo) != NULL;
}


QImage 
PDFThreadReader::getPage(int pageNo) const
{
	QImage image;
	PDFPage* page = impl_->pages.object(pageNo);

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
	impl_->clearCache(this->isRunning());
	impl_->clearRequest();

	this->stop();

	PDFReader::open(pdfFile, password);
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
		impl_->clearCache(this->isRunning());

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
		impl_->clearCache(this->isRunning());

		impl_->zoomLevel = level;
		impl_->screenWidth = screenW;
		impl_->screenHeight = screenH;
		impl_->rotation = rotation;
	}
}


void 
PDFThreadReader::askRender(int pageNo)
{
	QMutexLocker locker(&impl_->requestMutex);

	impl_->requestPageNo = pageNo;
	impl_->clearRequest();

	if (this->hasPage(pageNo))
	{
		QImage image = this->getPage(pageNo);

		if (!image.isNull())
		{
			emit rendered(pageNo, image);
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
			impl_->requests.enqueue(pageNo);

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
			int pageNo = 0;
			
			{
				QMutexLocker locker(&impl_->requestMutex);
				pageNo = impl_->requests.dequeue();
			}
			
			if (this->hasPage(pageNo))
			{
				//do not need to render, already has page
				if (pageNo == impl_->requestPageNo)
				{
					emit rendered(pageNo, this->getPage(pageNo));
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

		if (!this->wait(1000))
		{
			this->terminate();
		}
	}
}


void 
PDFThreadReader::prefetch(int pageNo, int count)
{
	int nextPage = pageNo + 1;

	for (int i = 0; i < count; ++i)
	{
		if (!this->hasPage(nextPage) && nextPage < this->pageCount())
		{
			impl_->requests.enqueue(nextPage);
		}

		++nextPage;
	}
}


void 
PDFThreadReader::renderPage(int pageNo)
{
	try
	{
		impl_->renderingPageNo = pageNo;

		QImage image = this->render(pageNo, 
			impl_->zoomLevel,
			impl_->screenWidth,
			impl_->screenHeight,
			impl_->rotation);

		if (image.isNull())
		{
			__THROW_L(PDFException, "Out of memory");
		}

		if (plutoApp->memoryStatus().memoryLoad > MAX_ACCEPT_MEM_USE)
		{
			this->clearEngineBuffer();
		}

		if (pageNo == impl_->requestPageNo)
		{
			//emit directly
			emit rendered(pageNo, image);

			cache(pageNo, image);
		}
		else
		{
			//cache then emit
			cache(pageNo, image);

			if (pageNo == impl_->requestPageNo)
			{
				emit rendered(pageNo, image);
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
	PDFPage* page = impl_->addIntoCache(pageNo, image);

	QStringList pages;
	foreach(int i, impl_->pages.keys())
	{
		pages<<QString::number(i + 1);
	}

	QString cachedMsg = QString("Cached p%1 (%2), cost %3k, total %4m")
		.arg(pageNo + 1)
		.arg(pages.join(", "))
		.arg(page->size() / 1000)
		.arg(impl_->pages.totalCost() * 1.0 / (1024 * 1024), 0, 'f', 2);

	__LOG(cachedMsg);

	emit cached(cachedMsg);

	if (page == NULL && pageNo == impl_->requestPageNo)
	{
		//too large, can not add into cache
		//remove the later request
		impl_->clearRequest();
	}
}


//end namespace
}

