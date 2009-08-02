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


int 
main(int argc, char *argv[])
{
	pltM8Platform m8App(argc, argv);

	//rotate
	QSettings settings(plutoApp->pathRelateToAppDir("config/config.ini"), QSettings::IniFormat);

	settings.beginGroup("PlutoPDF");

	pltM8Platform::ScreenRotateAngle realAngle = plutoApp->realScreenRotateAngle();
	pltM8Platform::ScreenRotateAngle newAngle = (pltM8Platform::ScreenRotateAngle)
		settings.value("rotateAngle", realAngle + 1).toInt();

	settings.endGroup();

	plutoApp->rotateScreen(newAngle);

	if (newAngle != realAngle)
	{
		plutoApp->enterFullScreen(NULL);
	}

	//splash
	QSplashScreen splash;

	splash.setPixmap(QPixmap(plutoApp->pathRelateToAppDir("splash.png")));
	splash.showMessage("PlutoPDF v0.1.4.1\nRoger (roger2yi@gmail.com)", 
		Qt::AlignBottom|Qt::AlignLeft,
		QColor("white"));
	splash.show();
	plutoApp->processEvents();


	//open screen
	pltPDFScreen screen; 
	screen.openFirstPdfBook();

	plutoApp->connect(plutoApp, SIGNAL(lastWindowClosed()), SLOT(quit()));
	plutoApp->enterFullScreen(&screen);

	splash.finish(&screen);
	splash.setPixmap(QPixmap());//release

	return m8App.exec();
}

