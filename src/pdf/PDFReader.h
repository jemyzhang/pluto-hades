/*******************************************************************************
**
**  NAME:			PDFReader.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class PDFReader, PDFException
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/06
**
**
*******************************************************************************/
#ifndef PDFReader_h__
#define PDFReader_h__

#include "pdf_global.h"

#include <QString>
#include <QImage>
#include <QColor>
#include <QSharedPointer>


namespace pdf {


///< PDFException
class PDF_EXPORT PDFException : public pltException
{
	DECLARE_EXCEPTION(PDFException)
};



///< PDFReader
class PDF_EXPORT PDFReader
{
public:
	enum State
	{
		NO_FILE_OPENED, 
		NO_PAGES_DRAWN, 
		DREW_PAGES,
	};

	enum ZoomLevel
	{
		FitWidth400 = -400,
		FitWidth300 = -300,
		FitWidth200 = -200,
		FitWidth180 = -180,
		FitWidth150 = -150,
		FitWidth120 = -120,
		FitWidth = -100,//virtual zoom ratio
		FitHeight200 = -20,
		FitHeight180 = -18,
		FitHeight150 = -15,
		FitHeight120 = -12,
		FitHeight = -10,//virtual zoom ratio
		FitPage = -1,//virtual zoom ratio
		Fit50 = 50,
		Fit100 = 100,
		Fit200 = 200,
	};


public:
	PDFReader(int gcBufSize = 1024 * 512);
	virtual ~PDFReader(void);


public:
	bool isNull() const;

	QString fileName() const;
	QString fileBaseName() const;
	QString filePath() const;

	int pageCount() const;
	QString bookTitle() const;

	//pageNo is zero based
	QSizeF pageSize(int pageNo) const;
	QSizeF pageSizeWithoutMargin(int pageNo) const;

	int pageRotation(int pageNo) const;

	QImage render(int pageNo,
		ZoomLevel level,
		int screenW,
		int screenH,
		int rotation = 0) const;

	QImage renderFitWidth(int pageNo, 
		int width, 
		ZoomLevel level = FitWidth, 
		int rotation = 0) const;

public:
	void open(const QString& pdfFile, const QString& password = "");

	void clearEngineBuffer();

	void setMargin(double leftPercent = 0.05, 
		double rightPercent = 0.05,
		double topPercent = 0.0,
		double bottomPercent = 0.0,
		int ignorePages = 0);

	void setBackground(QColor bk);

private:
	Q_DISABLE_COPY(PDFReader)

	struct PDFReaderImpl;
	PDFReaderImpl* impl_;
};


//namespace pdf
}

#endif // PDFReader_h__

