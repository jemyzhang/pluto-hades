/*******************************************************************************
**
**  NAME:			Platform.h	
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
#ifndef PLATFORM_H
#define PLATFORM_H
#include "platform_global.h"

#include <QString>
#include <QApplication>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

namespace platform
{


class PLATFORM_EXPORT Platform : public QApplication
{
	Q_OBJECT

public:
	enum ScreenRotateAngle
	{
		Angle0 = 0,
		Angle90 = 1,
		Angle180 = 2,
		Angle270 = 4,

		//Angle0 = 3,
		//Angle90 = 0,
		//Angle180 = 2,
		//Angle270 = 1,
	};

	enum M8ScreenRotateAngle
	{
		SCREEN_LANDSCAPE_N_   = 2, 
		SCREEN_LANDSCAPE_P_   = 3,
		SCREEN_PORTRAIT_N_   = 1,
		SCREEN_PORTRAIT_P_   = 0, 
	};

	enum M8Key
	{
		WPARAM_KEY_EVENT_CLICK_VOLUP = 0xB001,
		WPARAM_KEY_EVENT_CLICK_VOLDOWN,
		WPARAM_KEY_EVENT_CLICK_LINE,
		WPARAM_KEY_EVENT_CLICK_PLAY,
		WPARAM_KEY_EVENT_CLICK_HOME,
		WPARAM_KEY_EVENT_CLICK_POWER,
		WPARAM_KEY_EVENT_CLICK_VOLMUTE,

		WPARAM_KEY_EVENT_DBLCLICK_VOLUP = 0xB101,
		WPARAM_KEY_EVENT_DBLCLICK_VOLDOWN,
		WPARAM_KEY_EVENT_DBLCLICK_LINE,
		WPARAM_KEY_EVENT_DBLCLICK_PLAY,
		WPARAM_KEY_EVENT_DBLCLICK_HOME,
		WPARAM_KEY_EVENT_DBLCLICK_POWER,
		WPARAM_KEY_EVENT_DBLCLICK_VOLMUTE,

		WPARAM_KEY_EVENT_LONGCLICK_VOLUP = 0xB201,
		WPARAM_KEY_EVENT_LONGCLICK_VOLDOWN,
		WPARAM_KEY_EVENT_LONGCLICK_LINE,
		WPARAM_KEY_EVENT_LONGCLICK_PLAY,
		WPARAM_KEY_EVENT_LONGCLICK_HOME,
		WPARAM_KEY_EVENT_LONGCLICK_POWER,
		WPARAM_KEY_EVENT_LONGCLICK_VOLMUTE, 
	};

	struct MemoryStatus
	{
		quint32 memoryLoad;
		double totalPhys;//in mega
		double availPhys;
		double usedPhys;
	};


public:
	Platform(int& argc, char ** argv);
	virtual ~Platform();


public://helper functions
	QString pathRelateToAppDir(const QString& relativePath) const;
	QString helpFile() const;
	QString fileInArgument() const;

public://style relative
	void loadStyle(const QString& styleFile);
	void reloadStyle();

public://screen relative helper functions
	bool screenCanRotate() const;

	ScreenRotateAngle convertM8Angle(M8ScreenRotateAngle m8angle) const;

	ScreenRotateAngle realScreenRotateAngle() const;

	ScreenRotateAngle originalRotateAngle() const;
	ScreenRotateAngle lastScreenRotateAngle() const;
	ScreenRotateAngle currentScreenRotateAngle() const;
	ScreenRotateAngle nextScreenRotateAngle(ScreenRotateAngle angle) const;

	int deltaAngle(ScreenRotateAngle angle1, ScreenRotateAngle angle2) const;

	Qt::Orientation screenOrientation() const;

	void rotateScreen(ScreenRotateAngle angle);

	void rotateScreenToOriginal();
	void rotateScreenToLast();
	void rotateScreenToCurrent();

	void advance90degree();
	void advance180degree();

	void enterFullScreen(QWidget* mainWin);
	void leaveFullScreen(QWidget* mainWin);
	void reEnterFullScreen(QWidget* mainWin);

	void keepScreenLight(QWidget* mainWin);

	void lockSystem();


public://system info helper functions
	int batteryPercentage() const;

	MemoryStatus memoryStatus() const;


public://shell helper functions
	QString getOpenFileName(QWidget *parent = 0,
		const QString &caption = QString(),
		const QString &dir = QString(),
		const QString &filter = QString(),
		QString *selectedFilter = 0,
		QFileDialog::Options options = 0);

	void holdShellKey(QWidget * mainWin, bool holdHomeKey = false) const;
	void releaseShellKey(QWidget * mainWin) const;

	quint32 getShellEventId() const;
	quint32 getEntryLockPhoneEventId() const {return this->getEntryLockPhoneEventId_();};
	quint32 getLeaveLockPhoneEventId() const {return this->getLeaveLockPhoneEventId_();};

	QMessageBox::StandardButton msgboxQuestion(QWidget* parent,
		const QString& title,
		const QString& content) const;

public://acc relative
	quint32 getAccEventId() const;
	void setAccOpen(bool open);

public://SIP helper
	void openSip();
	void closeSip();


private:
	virtual bool screenCanRotate_() const {return false;};
	virtual ScreenRotateAngle realScreenRotateAngle_() const {return Angle0;};
	virtual Qt::Orientation screenOrientation_() const {return Qt::Horizontal;};

	virtual void rotateScreen_(ScreenRotateAngle /*angle*/) {};

	virtual void enterFullScreen_(QWidget* /*mainWin*/) {};
	virtual void leaveFullScreen_(QWidget* /*mainWin*/) {};

	virtual void keepScreenLight_(QWidget* /*mainWin*/) {};

	virtual void lockSystem_() {};

	virtual int batteryPercentage_() const {return 100;};

	virtual	MemoryStatus memoryStatus_() const {return MemoryStatus();};
	
	virtual QString getOpenFileName_(QWidget *parent,
		const QString &caption,
		const QString &dir,
		const QString &filter,
		QString *selectedFilter,
		QFileDialog::Options options);

	virtual quint32 getShellEventId_() const {return 0;};
	virtual quint32 getEntryLockPhoneEventId_() const {return 0;};
	virtual quint32 getLeaveLockPhoneEventId_() const {return 0;};

	virtual quint32 getAccEventId_() const {return 0;};
	virtual void setAccOpen_(bool /*open*/) {};

	virtual void holdShellKey_(QWidget * /*mainWin*/, bool /*holdHomeKey*/) const {};
	virtual void releaseShellKey_(QWidget * /*mainWin*/) const {};

	virtual QMessageBox::StandardButton msgboxQuestion_(QWidget* parent,
		const QString& title,
		const QString& content) const;

	virtual void openSip_() {};
	virtual void closeSip_() {};

private:
	mutable ScreenRotateAngle originalAngle_;
	ScreenRotateAngle lastAngle_;
	ScreenRotateAngle currAngle_;

	QString styleFile_;
};


#define plutoApp (static_cast<platform::Platform *>(QCoreApplication::instance()))


/************************************************************************/
/*                                                                      
/*	M8Platform
/*
/************************************************************************/
class PLATFORM_EXPORT M8Platform : public Platform
{
	Q_OBJECT

public:
	M8Platform(int& argc, char ** argv);
	virtual ~M8Platform();

	virtual bool winEventFilter(MSG *message, long *result);

private:

	virtual bool screenCanRotate_() const;
	virtual ScreenRotateAngle realScreenRotateAngle_() const;
	virtual Qt::Orientation screenOrientation_() const;

	virtual void rotateScreen_(ScreenRotateAngle angle);

	virtual void enterFullScreen_(QWidget* mainWin);
	virtual void leaveFullScreen_(QWidget* mainWin);

	virtual void keepScreenLight_(QWidget* mainWin);

	virtual void lockSystem_();

	virtual int batteryPercentage_() const;
	virtual	MemoryStatus memoryStatus_() const;

	virtual QString getOpenFileName_(QWidget *parent,
		const QString &caption,
		const QString &dir,
		const QString &filter,
		QString *selectedFilter,
		QFileDialog::Options options);

	virtual quint32 getShellEventId_() const;
	virtual quint32 getEntryLockPhoneEventId_() const;
	virtual quint32 getLeaveLockPhoneEventId_() const;

	virtual quint32 getAccEventId_() const;
	virtual void setAccOpen_(bool open);

	virtual void holdShellKey_(QWidget * mainWin, bool holdHomeKey) const;
	virtual void releaseShellKey_(QWidget * mainWin) const;

	virtual QMessageBox::StandardButton msgboxQuestion_(QWidget* parent,
		const QString& title,
		const QString& content) const;

	virtual void openSip_();
	virtual void closeSip_();
};


}


#endif // PLATFORM_H
