/*******************************************************************************
**
**  NAME:			PDFScreen.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/12
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class PDFScreen as plutopdf's main screen
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/12
**
**
*******************************************************************************/
#ifndef PDFScreen_h__
#define PDFScreen_h__
#include "pdf_global.h"
#include "../../../include/platform_include.h"


namespace pdf {


class PDF_EXPORT PDFScreen : public pltScreen
{
	Q_OBJECT

public:
	PDFScreen(QWidget *parent = 0);
	virtual ~PDFScreen(void);

	void setFirstPdfBook(const QString& pdfFile);

public slots:
	void getPdfBook();
	void openFirstPdfBook();
	bool openPdfBook(const QString& pdfFile);

	void scrollPrePage();
	void scrollNextPage();
	void scrollPage(int delta);
	void scrollToPage(int pageNo);

	void renderPage(int screenWidth = 0, bool wait = false);

	void onRendered(int pageNo, QImage image, QImage thumb);
	void onRenderError(QString errMsg);
	void onRendering(QString msg);
	void onCached(QString cacheMsg);


private slots:
	void onFirstShown();
	void onAskHelp();
	void onAskCut(int req);
	void onAskJump(int pageNo);
	void onAskRecent(const QString& recentFile);
	void onAskZoom(int zoomLevel);
	void onAskChangeSettings(bool showThumb, 
		bool showStatus, 		
		bool useHomeKey,
		bool useCahce,
		bool useAcc);
	void onAskChangeStyle(const QString& styleFile);
	void onAskRotate90();
	void onAskRotate180();

	void scrollDown();
	void scrollUp();

	void showMenu();

signals:
	void firstShow();

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void closeEvent(QCloseEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	virtual void keyPressEvent(QKeyEvent *event);

	virtual void handleTouching(QPoint pos);
	virtual void handleTouched(QPoint pos, int elapsed);
	virtual void handleDoubleTouched(QPoint pos, int elapsed);
	virtual void handleLongTouched(QPoint pos, int elapsed);

	virtual void openNextOrPrePdfFile(bool next);

#ifdef _WIN32_WCE
	virtual bool winEvent(MSG *message, long *result);
#endif

private:
	struct PDFScreenImpl;
	PDFScreenImpl* impl_;
};

//end namespace
}


extern "C"
{
	PDF_EXPORT int start_pdf_app(int argc, char* argv[]);

}


#endif // PDFScreen
