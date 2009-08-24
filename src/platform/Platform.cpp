/*******************************************************************************
**
**  NAME:			Platform.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/11
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class Platform
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/11
**
**
*******************************************************************************/
#include "stdafx.h"
#include "Platform.h"


namespace platform
{



Platform::Platform(int& argc, char ** argv)
	: QApplication(argc, argv)
{
	//load translation
	QTranslator* translator = new QTranslator(this);

	QString translation = this->pathRelateToAppDir("locale/plutopdf_%1.qm")
		.arg(QLocale::system().name());

	translator->load(translation);
	this->installTranslator(translator);
}


Platform::~Platform()
{

}


QString 
Platform::pathRelateToAppDir(const QString& relativePath) const
{
	QFileInfo fi(relativePath);

	if (fi.isRelative())
	{
		return QString("%1\\%2")
			.arg(QDir::toNativeSeparators(this->applicationDirPath()), relativePath);
	}
	else
	{
		return relativePath;
	}
}


QString 
Platform::helpFile() const
{
	QString help = this->pathRelateToAppDir("help/help_%1.pdf")
		.arg(QLocale::system().name());

	if (!QFile::exists(help))
	{
		help = this->pathRelateToAppDir("help/help_en.pdf");
	}

	return help;
}


QString 
Platform::fileInArgument() const
{
	QStringList args = this->arguments();
	QString openfile;

	if (args.size() == 2)
	{
		openfile = args.value(1);
	}
	else if (args.size() > 2)
	{
		args.removeFirst();
		openfile = args.join(" ");
	}

	return openfile;
}


void 
Platform::loadStyle(const QString& styleFile)
{
	QString realStyleFile = QFile::exists(styleFile) ? styleFile :
		this->pathRelateToAppDir(styleFile);

	if (QFile::exists(realStyleFile))
	{
		QFile file(realStyleFile);

		if (file.open(QFile::ReadOnly))
		{
			QTextStream ts(&file);
			QString ss = ts.readAll();

			this->setStyleSheet("");
			this->setStyleSheet(ss);
		}

		this->styleFile_ = realStyleFile;
	}
}


void 
Platform::reloadStyle()
{
	this->loadStyle(this->styleFile_);
}


bool 
Platform::screenCanRotate() const
{
	return this->screenCanRotate_();
}


Platform::ScreenRotateAngle 
Platform::realScreenRotateAngle() const
{
	static bool first = true;

	if (first)
	{
		this->originalAngle_ = this->realScreenRotateAngle_();

		first = false;

		return this->originalAngle_;
	}

	return this->realScreenRotateAngle_();
}


Qt::Orientation 
Platform::screenOrientation() const
{
	return this->screenOrientation_();
}


void 
Platform::rotateScreen(ScreenRotateAngle angle)
{
	this->lastAngle_ = this->realScreenRotateAngle();
	this->currAngle_ = angle;

	this->rotateScreen_(angle);
}


void 
Platform::rotateScreenToOriginal()
{
	this->rotateScreen(this->originalAngle_);
}


void 
Platform::rotateScreenToLast()
{
	this->rotateScreen(this->lastAngle_);
}


void 
Platform::rotateScreenToCurrent()
{
	this->rotateScreen(this->currAngle_);
}


void 
Platform::enterFullScreen(QWidget* mainWin)
{
	this->enterFullScreen_(mainWin);
}


void 
Platform::leaveFullScreen(QWidget* mainWin)
{
	this->leaveFullScreen_(mainWin);
}


void 
Platform::reEnterFullScreen(QWidget* mainWin)
{
	this->leaveFullScreen(mainWin);
	this->enterFullScreen(mainWin);
}


void 
Platform::lockSystem()
{
	this->lockSystem_();
}


int 
Platform::batteryPercentage() const
{
	return this->batteryPercentage_();
}


QString 
Platform::getOpenFileName(QWidget *parent,
						  const QString &caption,
						  const QString &dir,
						  const QString &filter,
						  QString *selectedFilter,
						  QFileDialog::Options options)
{
	QString file =this->getOpenFileName_(parent, 
		caption, 
		dir,
		filter, 
		selectedFilter,
		options);

	return QDir::toNativeSeparators(file);
}


QString 
Platform::getOpenFileName_(QWidget *parent, 
						   const QString &caption, 
						   const QString &dir, 
						   const QString &filter,
						   QString *selectedFilter, 
						   QFileDialog::Options options)
{
	return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
}


quint32 
Platform::getShellEventId() const
{
	return this->getShellEventId_();
}


Platform::ScreenRotateAngle 
Platform::originalRotateAngle() const
{
	return this->originalAngle_;
}


Platform::ScreenRotateAngle 
Platform::lastScreenRotateAngle() const
{
	return this->lastAngle_;
}


Platform::ScreenRotateAngle 
Platform::nextScreenRotateAngle(ScreenRotateAngle angle) const
{
	ScreenRotateAngle nextAngle = angle;

	switch(angle)
	{
	case Angle0:
		nextAngle = Angle90;
		break;
	case Angle90:
		nextAngle = Angle180;
		break;
	case Angle180:
		nextAngle = Angle270;
		break;
	case Angle270:
		nextAngle = Angle0;
		break;
	}

	return nextAngle;
}


Platform::ScreenRotateAngle 
Platform::currentScreenRotateAngle() const
{
	return this->currAngle_;
}


void 
Platform::openSip()
{
	this->openSip_();
}


void 
Platform::closeSip()
{
	this->closeSip_();
}


void 
Platform::holdShellKey(QWidget * mainWin, bool holdHomeKey) const
{
	this->holdShellKey_(mainWin, holdHomeKey);
}


void 
Platform::releaseShellKey(QWidget * mainWin) const
{
	this->releaseShellKey_(mainWin);
}


QMessageBox::StandardButton 
Platform::msgboxQuestion(QWidget* parent, 
						 const QString& title, 
						 const QString& content) const
{
	return this->msgboxQuestion_(parent, title, content);
}


QMessageBox::StandardButton 
Platform::msgboxQuestion_(QWidget* parent,
						  const QString& title,
						  const QString& content) const
{
	return QMessageBox::question(parent, title, content);
}


Platform::MemoryStatus 
Platform::memoryStatus() const
{
	return this->memoryStatus_();
}


void 
Platform::advance90degree()
{
	ScreenRotateAngle nextAngle = this->nextScreenRotateAngle(this->currAngle_);

	this->rotateScreen_(nextAngle);

	this->currAngle_ = nextAngle;
}


void 
Platform::advance180degree()
{
	ScreenRotateAngle nextAngle = this->nextScreenRotateAngle(this->currAngle_);
	nextAngle = this->nextScreenRotateAngle(nextAngle);

	this->rotateScreen_(nextAngle);

	this->currAngle_ = nextAngle;
}


int 
Platform::deltaAngle(ScreenRotateAngle angle1,
					 ScreenRotateAngle angle2) const
{
	switch (angle1)
	{
	case Angle0:
		switch (angle2)
		{
		case Angle0:
			return 0;
		case Angle90:
			return 90;
		case Angle180:
			return 180;
		case Angle270:
			return 270;
		}
	case Angle90:
		switch (angle2)
		{
		case Angle0:
			return 270;
		case Angle90:
			return 0;
		case Angle180:
			return 90;
		case Angle270:
			return 180;
		}
	case Angle180:
		switch (angle2)
		{
		case Angle0:
			return 180;
		case Angle90:
			return 270;
		case Angle180:
			return 0;
		case Angle270:
			return 90;
		}
	case Angle270:
		switch (angle2)
		{
		case Angle0:
			return 90;
		case Angle90:
			return 180;
		case Angle180:
			return 270;
		case Angle270:
			return 0;
		}
	}

	return 0;
}


quint32 
Platform::getAccMessageId() const
{
	return this->getAccMessageId_();
}


void 
Platform::setAccOpen(bool open)
{
	this->setAccOpen_(open);
}


Platform::ScreenRotateAngle 
Platform::convertM8Angle(M8ScreenRotateAngle m8angle) const
{
	Platform::ScreenRotateAngle angle = Angle90;
	switch(m8angle)
	{
	case SCREEN_PORTRAIT_P_:
		angle = Angle90;
		break;
	case SCREEN_PORTRAIT_N_:  
		angle = Angle270;
		break;
	case SCREEN_LANDSCAPE_P_:
		angle = Angle0;
		break;
	case SCREEN_LANDSCAPE_N_:
		angle = Angle180;
		break;
	}

	return angle;
}


void 
Platform::keepScreenLight(QWidget* mainWin)
{
	this->keepScreenLight_(mainWin);
}


/************************************************************************/
/*                                                                      
/*	M8Platform
/*
/************************************************************************/
#ifdef PLATFORM_M8
#include <IMzUnknown_IID.h>
#include <IFileBrowser_GUID.h>

struct COM_INIT_
{
	COM_INIT_()
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}

	~COM_INIT_()
	{
		::CoUninitialize();
	}
};

#define SAFE_RELEASE(itf)		if (itf) {itf->Release(); itf = 0;}

M8Platform::M8Platform(int& argc, char ** argv)
: Platform(argc, argv)
{
	//this->setAutoSipEnabled(true);
}



M8Platform::~M8Platform()
{

}


bool 
M8Platform::winEventFilter(MSG *message, long *result)
{
	if (message->message == WM_QUIT)
	{
		*result = message->wParam;
		return true;
	}

	return false;
}




bool 
M8Platform::screenCanRotate_() const
{
	bool canRotate = false;

	DEVMODE mode;
	ZeroMemory(&mode, sizeof(mode));
	mode.dmSize = sizeof(mode);
	mode.dmFields = DM_DISPLAYQUERYORIENTATION;

	LONG result = ::ChangeDisplaySettingsEx(NULL
		, &mode
		, NULL
		, CDS_TEST
		, NULL);

	if (result == DISP_CHANGE_SUCCESSFUL)
	{
		if (mode.dmDisplayOrientation != DMDO_0)
		{
			canRotate = true;
		}
	}

	return canRotate;
}


M8Platform::ScreenRotateAngle 
M8Platform::realScreenRotateAngle_() const
{
	ScreenRotateAngle angle = Angle0;

	DEVMODE mode;
	ZeroMemory(&mode, sizeof(mode));
	mode.dmSize = sizeof(mode);
	mode.dmFields = DM_DISPLAYORIENTATION;

	LONG result = ::ChangeDisplaySettingsEx(NULL
		, &mode
		, NULL
		, CDS_TEST
		, NULL);

	if(DISP_CHANGE_SUCCESSFUL == result)
	{
		// success
		angle = (ScreenRotateAngle)mode.dmDisplayOrientation;
	}

	__LOG(QString("Real rotate angle %1").arg(angle));

	return angle;
}


Qt::Orientation 
M8Platform::screenOrientation_() const
{
	ScreenRotateAngle angle = this->realScreenRotateAngle();

	if (angle == Angle0 || angle == Angle180)
	{
		return Qt::Horizontal;
	}
	else
	{
		return Qt::Vertical;
	}
}


void 
M8Platform::rotateScreen_(ScreenRotateAngle angle)
{
	DEVMODE mode;
	ZeroMemory(&mode, sizeof(mode));
	mode.dmSize = sizeof(mode);
	mode.dmFields = DM_DISPLAYORIENTATION;
	mode.dmDisplayOrientation = angle;

	Qt::Orientation oldOrientation = this->screenOrientation();

	QDesktopWidget dw;
	int oldScreenWidth = dw.screenGeometry().width();

	LONG result = ::ChangeDisplaySettingsEx(NULL
		, &mode
		, NULL
		, CDS_RESET
		, NULL);

	if(DISP_CHANGE_SUCCESSFUL == result)
	{
		__LOG(QString("Rotate screen %1 successful.").arg(angle));
	}
	else
	{
		__LOG(QString("Rotate screen %1 failed.").arg(angle));
	}

	//wait screen rotate
	if (oldOrientation != this->screenOrientation())
	{
		int i = 10;

		while (oldScreenWidth == dw.screenGeometry().width() && --i)
		{
			::Sleep(100);

			this->processEvents();
		}
	}
}


void 
M8Platform::enterFullScreen_(QWidget* mainWin)
{
	HideMzTopBar();

	if (mainWin)
	{
		QDesktopWidget dw;

		mainWin->setMinimumSize(dw.size());
		mainWin->setMaximumSize(dw.size());
		mainWin->resize(dw.size());

		mainWin->setWindowState(Qt::WindowFullScreen);
		mainWin->show();
	}

}


void 
M8Platform::leaveFullScreen_(QWidget* mainWin)
{
	ShowMzTopBar();

	if (mainWin)
	{
		mainWin->setWindowState(Qt::WindowFullScreen);
		mainWin->show();
	}
}


void 
M8Platform::keepScreenLight_(QWidget* mainWin)
{
	SetScreenAlwaysOn(mainWin->winId());
	SetScreenAutoOff();
}


int 
M8Platform::batteryPercentage_() const
{
	SYSTEM_POWER_STATUS_EX status;
	
	::GetSystemPowerStatusEx(&status, TRUE);

	__LOG(QString("Battery status - AC ON/OFF %1, Low/High flag %2, "
		"percent %3, life time %4, full life time %5")
		.arg(status.ACLineStatus)
		.arg(status.BatteryFlag)
		.arg(status.BatteryLifePercent)
		.arg(status.BatteryLifeTime)
		.arg(status.BatteryFullLifeTime));

	return status.BatteryLifePercent;
}


QString 
M8Platform::getOpenFileName_(QWidget *parent,
							 const QString &caption, 
							 const QString &dir, 
							 const QString &filter, 
							 QString *selectedFilter, 
							 QFileDialog::Options options)
{
	Q_UNUSED(selectedFilter);
	Q_UNUSED(options);

	COM_INIT_ com_init;


	this->rotateScreenToOriginal();//rotate to original vertical screen
	ShowMzTopBar();

	QString file;

	IFileBrowser *browser = NULL;
	IMzSelect *selector = NULL;        

	::CoCreateInstance(CLSID_FileBrowser 
		, NULL 
		, CLSCTX_INPROC 
		, IID_MZ_FileBrowser
		, (void**)&browser);

	if(browser)
	{

		browser->QueryInterface(IID_MZ_Select , (void**)&selector);

		if(selector)
		{
			browser->SetParentWnd(parent->winId());
			browser->SetTitle((wchar_t*)caption.utf16());

			if (!dir.isEmpty())
			{
				browser->SetOpenDirectoryPath((wchar_t*)dir.utf16());
			}

			browser->SetExtFilter((wchar_t*)filter.utf16());

			browser->SetOpenDocumentType(DOCUMENT_SELECT_SINGLE_FILE);

			if(selector->Invoke())
			{
				file = QString::fromUtf16((ushort*)browser->GetSelectedFileName());
			}
		}
	}

	SAFE_RELEASE(selector);
	SAFE_RELEASE(browser);

	HideMzTopBar();

	if (this->currentScreenRotateAngle() != this->realScreenRotateAngle())
	{
		this->rotateScreenToLast();//rotate back
	}

	return file;
}


void 
M8Platform::openSip_()
{
	MzOpenSip(IM_SIP_MODE_KEEP, 0);
}


void 
M8Platform::closeSip_()
{
	MzCloseSip();
}


quint32 
M8Platform::getShellEventId_() const
{
	static quint32 eventId = 0;

	if (!eventId)
	{
		eventId = GetShellNotifyMsg_AllKeyEvent();
	}

	return eventId;
}

quint32 
M8Platform::getAccMessageId_() const
{
	return MzAccGetMessage();
}


void 
M8Platform::holdShellKey_(QWidget * mainWin, bool holdHomeKey) const
{
	this->releaseShellKey(mainWin);//release first

	if (holdHomeKey)
	{
		HoldShellUsingSomeKeyFunction(mainWin->winId(), 
			MZ_HARDKEY_VOLUME_DOWN  | MZ_HARDKEY_VOLUME_UP | MZ_HARDKEY_POWER | MZ_HARDKEY_HOME);
	}
	else
	{
		HoldShellUsingSomeKeyFunction(mainWin->winId(), 
			MZ_HARDKEY_VOLUME_DOWN  | MZ_HARDKEY_VOLUME_UP | MZ_HARDKEY_POWER);
	}

	SetScreenAlwaysOn(mainWin->winId());

	//RegisterShellMessage(mainWin->winId(), 
	//	WM_MZSH_ENTRY_LOCKPHONE | WM_MZSH_LEAVE_LOCKPHONE |
	//	WM_MZSH_ENTRY_SHUTDOWN| WM_MZSH_LEAVE_SHUTDOWN);

}


void 
M8Platform::releaseShellKey_(QWidget * mainWin) const
{
	UnHoldShellUsingSomeKeyFunction(mainWin->winId(), 
		MZ_HARDKEY_VOLUME_DOWN  | MZ_HARDKEY_VOLUME_UP | MZ_HARDKEY_POWER | MZ_HARDKEY_HOME);

	SetScreenAutoOff();

	//UnRegisterShellMessage(mainWin->winId(), 
	//	WM_MZSH_ENTRY_LOCKPHONE | WM_MZSH_LEAVE_LOCKPHONE |
	//	WM_MZSH_ENTRY_SHUTDOWN| WM_MZSH_LEAVE_SHUTDOWN);
}


QMessageBox::StandardButton 
M8Platform::msgboxQuestion_(QWidget* parent, 
							const QString& title, 
							const QString& content) const
{
	int result = MzMessageBoxEx(NULL,
		(wchar_t*)content.utf16(), 
		(wchar_t*)title.utf16(), 
		MZ_YESNO,
		false);

	if (result == -1)
	{
		return QMessageBox::Yes;
	}
	else
	{
		return QMessageBox::No;
	}
}


M8Platform::MemoryStatus 
M8Platform::memoryStatus_() const
{
	MEMORYSTATUS mem;
	::GlobalMemoryStatus(&mem);

	MemoryStatus ms;
	ms.memoryLoad = mem.dwMemoryLoad ;
	ms.totalPhys = mem.dwTotalPhys * 1.0 / (1024 * 1024);
	ms.availPhys = mem.dwAvailPhys * 1.0 / (1024 * 1024);
	ms.usedPhys = ms.totalPhys - ms.availPhys;

	return ms;
}


void 
M8Platform::lockSystem_()
{
	SetScreenAutoOff();
	SetBackLightState(FALSE);
	MzEntryLockPhone();
}


void 
M8Platform::setAccOpen_(bool open)
{
	if (open)
		MzAccOpen();
	else
		MzAccClose();
}


#endif // PLATFORM_M8

}

