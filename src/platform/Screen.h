/*******************************************************************************
**
**  NAME:			Screen.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/11
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class Screen
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/11
**
**
*******************************************************************************/
#ifndef SCREEN_H
#define SCREEN_H
#include "platform_global.h"

#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QLineEdit>

namespace platform
{


class PLATFORM_EXPORT Screen : public QGraphicsView
{
	Q_OBJECT

public:
	enum ScrollScreenDirection
	{
		DirectionNone,
		DirectionUp,
		DirectionDown,
		DirectionLeft,
		DirectionRight,
		DirectionUpLeft,
		DirectionDownLeft,
		DirectionUpRight,
		DirectionDownRight,
		DirectionUpLeftCorner,
		DirectionDownLeftCorner,
		DirectionUpRightCorner,
		DirectionDownRightCorner,
	};

public:
	Screen(QWidget *parent = 0);
	virtual ~Screen();

public:
	QSize neededThumbSize() const;

	QImage pageImage() const;
	
	int progressTime() const;

public:
	void setStatusBarVisible(bool visible);
	void setThumbVisible(bool visible);
	void setBatteryInfo(int percentage);
	void updateTimeInfo();
	void updateMemoryInfo();

	void setPageIndicator(int curr, int total);
	void setZoomInfo(int zoom);
	void setFileInfo(const QString& fileInfo);
		
	void setMessage(const QString& message);

	void setPageImage(const QImage& pg,
		const QPixmap& thumb = QPixmap(),
		ScrollScreenDirection direction = DirectionUpLeftCorner);

	void startThumbDiaplay();

public slots:

	void showThumb();
	void hideThumb();

	void updateThumbMask();

	void startProgress(int minmimum = 0, int maximum = 99);
	void stepProgress(int step = 1);
	void endProgress();
	void hideProgress();

	void exit();

private slots:
	void thumbDisplay(int frame);

protected:
	void timerEvent(QTimerEvent * event);

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void resizeEvent(QResizeEvent *event);

	virtual void handleTouching(QPoint pos);
	virtual void handleTouched(QPoint pos, int elapsed);
	virtual void handleDoubleTouched(QPoint pos, int elapsed);
	virtual void handleLongTouched(QPoint pos, int elapsed);

protected:
	void scrollSceen(ScrollScreenDirection direction);

private:
	void killPressTimer();
	void generateThumb(const QPixmap& pixmap);


private:
	struct ScreenImpl;
	ScreenImpl* impl_;
};


class PLATFORM_EXPORT SipLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	SipLineEdit(QWidget *parent = 0);
	virtual ~SipLineEdit();


protected:
	virtual void focusInEvent(QFocusEvent * event);
	virtual void focusOutEvent(QFocusEvent * event);

private slots:
	void onFocusIn();

signals:
	void focusIn();

};
//namespace
}
#endif // SCREEN_H
