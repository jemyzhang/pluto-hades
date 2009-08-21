/*******************************************************************************
**
**  NAME:			PDFThreadReader.h	
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
#ifndef PDFTHREADREADER_H
#define PDFTHREADREADER_H
#include "pdf_global.h"
#include "PDFReader.h"

#include <QThread>

namespace pdf {


#define DEFAULT_CACHE_SIZE				(1024 * 1024 * 10)
#define DEFAULT_RENDER_BUFFER_SIZE		(1024 * 512)


class PDF_EXPORT PDFThreadReader : public QThread, public PDFReader
{
	Q_OBJECT

public:
	PDFThreadReader(QObject *parent = NULL, 
		int cacheSize = DEFAULT_CACHE_SIZE, 
		int gcBufSize = DEFAULT_RENDER_BUFFER_SIZE);

	virtual ~PDFThreadReader();


public://query
	bool hasPage(int pageNo) const;
	QImage pageImage(int pageNo) const;

	bool hasPageThumb(int pageNo) const;
	QImage pageThumb(int pageNo) const;

	ZoomLevel zoomLevel() const;


public://command
	virtual void open(const QString& pdfFile, const QString& password = "");

	virtual void setMargin(double leftPercent = 0.05, 
		double rightPercent = 0.05,
		double topPercent = 0.0,
		double bottomPercent = 0.0,
		int ignorePages = 0);


	void setRenderParams(ZoomLevel level,
		int screenW,
		int screenH,
		int rotation = 0);

	void setThumbSize(QSize size);

	void setConvertTo16Bits(bool convert);

	void setUseFastCompressAlgo(bool use);

	void enableCaching(bool enable);

	void askRender(int pageNo, bool wait = false);


signals:
	void renderError(QString errMsg);
	void rendering(QString msg);

	void rendered(int pageNo, QImage image, QImage thumb);

	void cached(QString msg);

protected:
	virtual void run();


private:
	void prefetch(int pageNo, int count);

	void stop();
	void stopAndClean();

	void renderPage(int pageNo);

	void emitRendered(int pageNo, const QImage& image, const QImage& thumb);

	void cache(int pageNo, const QImage& image);

private:
	Q_DISABLE_COPY(PDFThreadReader)

	struct PDFThreadReaderImpl;
	PDFThreadReaderImpl* impl_;
};

//end namespace
}
#endif // PDFTHREADREADER_H
