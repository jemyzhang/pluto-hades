/*******************************************************************************
**
**  NAME:			PDFMenu.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/16
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/16
**
**
*******************************************************************************/
#ifndef PDFMENU_H
#define PDFMENU_H
#include "pdf_global.h"
#include <QWidget>
#include <QLineEdit>

namespace pdf {


class PDFMenu : public QWidget
{
	Q_OBJECT

public:
	enum CutRequest
	{
		CutLeft3,
		CutRight3,
		CutLeft5,
		CutRight5,
		CutAuto,
		CutReset,
	};

	enum SubMenuIndex
	{
		RecentFiles,
		CutMargin,
		Jump,
		Zoom,
		Settings,
		About,
	};

public:
	PDFMenu(QWidget *parent = 0);
	~PDFMenu();


public:
	void setApplicationName(const QString& appName);
	void setCurrentPageNo(int pageNo);
	void setRecentOpenFiles(const QStringList& files);
	void setCurrentSettings(bool showThumb, bool showStatus);

public slots:
	void on_m1Exit_clicked();
	void on_m1Open_clicked();
	void on_m1Recent_clicked();
	void on_m1Cut_clicked();
	void on_m1Jump_clicked();
	void on_m1Zoom_clicked();
	void on_m1Settings_clicked();
	void on_m1About_clicked();

	void on_m2Help_clicked();

	void on_m2CutLeft3_clicked();
	void on_m2CutRight3_clicked();
	void on_m2CutLeft5_clicked();
	void on_m2CutRight5_clicked();
	void on_m2AutoCut_clicked();
	void on_m2CutReset_clicked();

	void on_m2Jump_clicked();
	void on_m2JumpNext10_clicked();
	void on_m2JumpNext100_clicked();
	void on_m2JumpPre10_clicked();
	void on_m2JumpPre100_clicked();

	void on_m2Zoom400_clicked();
	void on_m2Zoom300_clicked();
	void on_m2Zoom200_clicked();
	void on_m2Zoom180_clicked();
	void on_m2Zoom150_clicked();
	void on_m2Zoom120_clicked();
	void on_m2Zoom100_clicked();

	void on_m2Recent1_clicked();
	void on_m2Recent2_clicked();
	void on_m2Recent3_clicked();
	void on_m2Recent4_clicked();
	void on_m2Recent5_clicked();
	void on_m2Recent6_clicked();

	void on_m2ShowThumb_clicked();
	void on_m2ShowStatus_clicked();
	void on_m2Style_clicked();
	void on_m2Rotate180_clicked();

protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);


signals:
	void askExit();
	void askOpen();
	void askCut(int req);
	void askJump(int pageNo);
	void askHelp();
	void askZoom(int zoomLevel);
	void askRecent(const QString& recentFile);
	void askChangeSettings(bool showThumb, bool showStatus);
	void askChangeStyle(const QString& styleFile);
	void askRotate180();

private:
	void showSubMenu(int index);

private:
	struct PDFMenuImpl;
	PDFMenuImpl* impl_;
};

//end namespace
}
#endif // PDFMENU_H
