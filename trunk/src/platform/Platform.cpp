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
Platform::rotateScreenBack()
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


QSize 
Platform::screenResolution() const
{
#ifdef _WIN32_WCE
	static QDesktopWidget* dw = NULL;

	if (!dw)
	{
		dw = new QDesktopWidget();
	}
	
	QRect rect = dw->screenGeometry();
	return rect.size();
#else
	return QSize(720, 480);
#endif
}


Platform::ScreenRotateAngle 
Platform::lastScreenRotateAngle() const
{
	return this->lastAngle_;
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
Platform::holdShellKey(QWidget * mainWin) const
{
	this->holdShellKey_(mainWin);
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
	ScreenRotateAngle nextAngle = (ScreenRotateAngle)(
		(this->currentScreenRotateAngle() + 1) % 4);

	this->rotateScreen_(nextAngle);

	this->currAngle_ = nextAngle;
}


void 
Platform::advance180degree()
{
	ScreenRotateAngle nextAngle = (ScreenRotateAngle)(
		(this->currentScreenRotateAngle() + 2) % 4);

	this->rotateScreen_(nextAngle);

	this->currAngle_ = nextAngle;
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

	LONG result = ::ChangeDisplaySettingsEx(NULL
		, &mode
		, NULL
		, CDS_RESET
		, NULL);

	if(DISP_CHANGE_SUCCESSFUL == result)
	{
		__LOG(QString("Rotate screeen %1 successed.").arg(angle));
	}
	else
	{
		__LOG(QString("Rotate screeen %1 failed.").arg(angle));
	}
}


void 
M8Platform::enterFullScreen_(QWidget* mainWin)
{
	HideMzTopBar();

	mainWin->setWindowState(Qt::WindowFullScreen);
	mainWin->show();
}


void 
M8Platform::leaveFullScreen_(QWidget* mainWin)
{
	ShowMzTopBar();

	mainWin->hide();

	mainWin->setWindowState(Qt::WindowFullScreen);
	mainWin->show();
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


	this->rotateScreenBack();//need rotate back first
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
	this->rotateScreenBack();//rotate back

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


void 
M8Platform::holdShellKey_(QWidget * mainWin) const
{

	HoldShellUsingSomeKeyFunction(mainWin->winId(), 
		MZ_HARDKEY_VOLUME_DOWN  | MZ_HARDKEY_VOLUME_UP);
}


void 
M8Platform::releaseShellKey_(QWidget * mainWin) const
{
	UnHoldShellUsingSomeKeyFunction(mainWin->winId(), 
		MZ_HARDKEY_VOLUME_DOWN  | MZ_HARDKEY_VOLUME_UP);
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




#endif // PLATFORM_M8

}

