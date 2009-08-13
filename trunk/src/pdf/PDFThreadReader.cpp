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


struct PDFPage
{
	QByteArray data;
	
	QSize imgsize;
	QImage::Format format;

	void compress(const QImage& image)
	{
		data = qCompress(image.bits(), image.numBytes());

		imgsize = image.size();
		format = image.format();
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


struct Request
{
	int pageNo;
};

struct PDFThreadReader::PDFThreadReaderImpl
{
	QCache<int, PDFPage> pages;

	QQueue<int>	requests;
	QMutex requestMutex;
	volatile int requestPageNo;

	PDFThreadReader::ZoomLevel zoomLevel;
	int screenWidth;
	int screenHeight;
	int rotation;

	volatile bool stopFlag;

	bool addIntoCache(int pageNo, const QImage& image)
	{
		PDFPage* page = new PDFPage();
		page->compress(image);

		if (page->size() != 0 && page->size() < pages.maxCost())
		{
			pages.insert(pageNo, page, page->size());

			qDebug()<<"Cached : "<<pages.keys()
				<<", cost (k)"<< (page->size() / 1000)
				<<", total cost (m)"<< (pages.totalCost() * 1.0 / (1024 * 1024));

			return true;
		}
		else
		{
			SAFE_DELETE(page);
			return false;
		}
	}

	void clearCache()
	{
		pages.clear();
	}

	void clearRequest()
	{
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
	impl_->clearCache();
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
		impl_->clearCache();

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
		impl_->clearCache();

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
		emit rendered(pageNo, this->getPage(pageNo));
	}
	else
	{
		impl_->requests.enqueue(pageNo);
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
			int pageNo = impl_->requests.dequeue();

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
		QImage image = this->render(pageNo, 
			impl_->zoomLevel,
			impl_->screenWidth,
			impl_->screenHeight,
			impl_->rotation);

		if (image.isNull())
		{
			__THROW_L(PDFException, "Out of memory");
		}

		if (pageNo == impl_->requestPageNo)
		{
			emit rendered(pageNo, image);
		}

		bool success = impl_->addIntoCache(pageNo, image);

		if (!success && pageNo == impl_->requestPageNo)
		{
			//too large, can not add into cache
			//remove the later request
			impl_->clearRequest();
		}
	}
	catch (PDFException& e)
	{
		emit renderError(QString("Render p%1 error - %2")
			.arg(pageNo + 1).arg(e.what()));
	}
}


//end namespace
}

