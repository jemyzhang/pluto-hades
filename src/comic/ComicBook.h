/*******************************************************************************
**
**  NAME:			ComicBook.h	
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
#ifndef COMICBOOK_H
#define COMICBOOK_H
#include "comic_global.h"
#include "../../../include/foundation_include.h"

#include <QThread>

namespace comic
{

class COMIC_EXPORT ComicBook : public QThread
{
	Q_OBJECT

public:
	enum SplitPageStyle
	{
		SplitNone,
		SplitRight2Left,
		SplitLeft2Right,
	};

	enum PagePart
	{
		WholePart,
		LeftPart,
		RightPart,
	};

public:
	ComicBook(QObject *parent);
	~ComicBook();

public:
	operator bool() const;

	bool isNull() const;

	QString fileName() const;
	QString fileBaseName() const;
	QString filePath() const;

	QString fileDescription() const;

	int pageCount() const;

	SplitPageStyle splitPageStyle() const;

	QStringList supportFileFormats() const;
	QString fileFilter() const;

public:
	void open(const QString& file);
	void open(const QDir& dir);

	void setSplitPageStyle(SplitPageStyle style);

	void setThumbSize(QSize size);

	void setConvertTo16Bits(bool convert);

	void enableCaching(bool enable);

	void askRender(int pageNo, PagePart part);
	void askNextPart();
	void askPrePart();

	void emitRendered(int pageNo, 
		PagePart part, 
		const QImage& image, 
		const QImage& thumb);

private:
	void initPagePart();

signals:
	void renderError(QString errMsg);
	void rendering(QString msg);

	void rendered(int pageNo, PagePart part, QImage image, QImage thumb);

	void cached(QString msg);

private:
	struct ComicBookImpl;
	ComicBookImpl* impl_;
};


class COMIC_EXPORT ComicException : public pltException
{
	DECLARE_EXCEPTION(ComicException)
};


}


#endif // COMICBOOK_H
