/*******************************************************************************
**
**  NAME:			ComicBook.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/08/21
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class ComicBook
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/08/21
**
**
*******************************************************************************/
#include "stdafx.h"

#include "ComicBook.h"


namespace comic
{


struct ComicBook::ComicBookImpl
{
	bool isArchive;

	QString file;
	QString subFile;

	QStringList fileList;

	int currPageNo;
	PagePart currPart;

	SplitPageStyle splitStyle;

	QSize thumbSize;
	bool convert16bits;
	bool enableCaching;
	int prefetchNum;

	ComicBookImpl()
		: isArchive(false)
		, splitStyle(SplitRight2Left)
		, thumbSize(128, 128)
		, convert16bits(true)
		, enableCaching(true)
		, prefetchNum(2)
	{
	}
};


ComicBook::ComicBook(QObject *parent)
	: QThread(parent)
	, impl_(new ComicBookImpl)
{

}


ComicBook::~ComicBook()
{
	SAFE_DELETE(impl_);
}



ComicBook::operator bool() const
{
	return QFile::exists(impl_->file) && this->pageCount();
}


bool 
ComicBook::isNull() const
{
	return !(QFile::exists(impl_->file) && this->pageCount());
}


QString 
ComicBook::fileName() const
{
	return QFileInfo(impl_->file).fileName();
}


QString 
ComicBook::fileBaseName() const
{
	return QFileInfo(impl_->file).baseName();
}


QString 
ComicBook::filePath() const
{
	return impl_->file;
}


QString 
ComicBook::fileDescription() const
{
	return this->fileName();
}


int 
ComicBook::pageCount() const
{
	return impl_->fileList.size();
}


ComicBook::SplitPageStyle 
ComicBook::splitPageStyle() const
{
	return impl_->splitStyle;
}


QStringList 
ComicBook::supportFileFormats() const
{
	return QStringList()<<"jpg"<<"jpeg";
}


QString 
ComicBook::fileFilter() const
{
	return "*.jpg; *.jpeg;";
}


void 
ComicBook::open(const QString& file)
{
	QFileInfo fi(file);

	if (fi.exists())
	{
		QDir dir = fi.absoluteDir();

		this->open(dir);

		impl_->currPageNo = impl_->fileList.indexOf(file);
	}
}


void 
ComicBook::open(const QDir& dir)
{
	QStringList filter = QStringList() << "*.jpg" << "*.jpeg";
	impl_->fileList = dir.entryList(filter, QDir::Files, QDir::Name);

	impl_->currPageNo = 0;
	impl_->currPart = RightPart;
}


void 
ComicBook::setSplitPageStyle(SplitPageStyle style)
{
	impl_->splitStyle = style;
}


void 
ComicBook::setThumbSize(QSize size)
{
	impl_->thumbSize = size;
}


void 
ComicBook::setConvertTo16Bits(bool convert)
{
	impl_->convert16bits = convert;
}


void 
ComicBook::enableCaching(bool enable)
{
	impl_->enableCaching = enable;
}


}

