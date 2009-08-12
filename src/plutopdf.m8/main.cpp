/*******************************************************************************
**
**  NAME:			main.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/17
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	main function
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/17
**
**
*******************************************************************************/
#include "stdafx.h"

typedef int (*StartAppFunc)(QSplashScreen*);

int 
main(int argc, char *argv[])
{
	pltM8Platform m8App(argc, argv);

	//splash
	QSplashScreen splash(QPixmap(plutoApp->pathRelateToAppDir("splash.png")));

	splash.showMessage("PlutoPDF for M8 v0.1.4.2 \nRoger (roger2yi@gmail.com)", 
		Qt::AlignBottom|Qt::AlignLeft,
		QColor("white"));

	splash.show();
	plutoApp->processEvents();

#ifdef _DEBUG
		QLibrary pdfLib("pdf_wince_debug");
#else
		QLibrary pdfLib("pdf_wince");
#endif // _DEBUG

		if (pdfLib.load())
		{
			StartAppFunc startAppFunc = (StartAppFunc) pdfLib.resolve("startApp");

			if (startAppFunc)
				return startAppFunc(&splash);
		}

	return -1;
}

//int 
//main(int argc, char *argv[])
//{
//	pltM8Platform m8App(argc, argv);
//
//	//splash
//	QSplashScreen splash(QPixmap(plutoApp->pathRelateToAppDir("splash.png")));
//
//	splash.showMessage("PlutoPDF v0.1.4.1\nRoger (roger2yi@gmail.com)", 
//		Qt::AlignBottom|Qt::AlignLeft,
//		QColor("white"));
//
//	splash.show();
//	plutoApp->processEvents();
//
//
//	//open screen
//	pltPDFScreen screen; 
//
//	screen.openFirstPdfBook();
//	plutoApp->enterFullScreen(&screen);
//
//	//splash.move((screen.width() - splash.width()) / 2,
//	//	(screen.height() - splash.height()) / 2);
//	splash.finish(&screen);
//
//	plutoApp->connect(plutoApp, SIGNAL(lastWindowClosed()), SLOT(quit()));
//
//	return m8App.exec();
//}

