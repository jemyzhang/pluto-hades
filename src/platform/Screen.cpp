/*******************************************************************************
**
**  NAME:			Screen.cpp	
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
#include "stdafx.h"
#include "Screen.h"
#include "Platform.h"

#include "ui_Screen.h"


namespace platform
{


struct Screen::ScreenImpl
{
	ScreenImpl()
		: doubleTouchRange(50, 50)
		, touchRange(10, 10)
		, lastPressedTime(0)
		, lastReleaseTime(0)
		, lastPressedPoint(0, 0)
		, lastReleasePoint(0, 0)
		, freshInfoTimerId (0)
		, pressTimerId (0)
		, thumbVisible (true)
	{
		startTime.start();
	}

	static const int pressTriggerTime = 800;
	static const int thumbBoundary = 4;

	Ui::screen ui;

	QGraphicsScene* scene;
	QGraphicsPixmapItem* pixItem;
	
	QScrollBar* vScrollBar;
	QScrollBar* hScrollBar;
	int pageStep;
	int stepRemain;


	QTime startTime;
	QPoint lastPressedPoint;
	QPoint lastReleasePoint;
	int lastPressedTime;
	int lastReleaseTime;

	QSize touchRange;
	QSize doubleTouchRange;

	int freshInfoTimerId;
	int pressTimerId;

	QTimeLine* thumbTimeLine;
	QTimeLine* progressTimeLine;

	QPixmap thumb;
	bool thumbVisible;
};


Screen::Screen(QWidget *parent)
	: QGraphicsView(parent)
	, impl_(new ScreenImpl)
{
	//ui
	impl_->ui.setupUi(this);
	impl_->ui.verticalLayout->setMargin(0);
	impl_->ui.verticalLayout->setSpacing(0);

	impl_->ui.statusBar->layout()->removeWidget(impl_->ui.progress);
	impl_->ui.progress->move(0, 0);
	impl_->ui.progress->hide();

	impl_->ui.statusBar->setAttribute(Qt::WA_TransparentForMouseEvents);
	impl_->ui.frameThumb->setAttribute(Qt::WA_TransparentForMouseEvents);

	//scene
	impl_->scene = new QGraphicsScene(this);
	this->setScene(impl_->scene);


	impl_->pixItem = impl_->scene->addPixmap(QPixmap());
	impl_->pixItem->setPos(0, 0);
	impl_->pixItem->setZValue(0);
	impl_->pixItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

	//scroll
	impl_->vScrollBar = this->verticalScrollBar();
	impl_->hScrollBar = this->horizontalScrollBar();

	//info
	impl_->freshInfoTimerId = this->startTimer(1000 * 10);
	this->updateTimeInfo();
	this->setBatteryInfo(plutoApp->batteryPercentage());
	this->updateMemoryInfo();

	//thumb
	impl_->ui.frameThumb->layout()->removeWidget(impl_->ui.lbThumbMask);
	impl_->ui.lbThumbMask->setParent(impl_->ui.lbThumb);
	impl_->ui.lbThumbMask->move(impl_->thumbBoundary, impl_->thumbBoundary);
	impl_->ui.lbThumb->hide();

	impl_->thumbTimeLine = new QTimeLine(1000, this);
	impl_->thumbTimeLine->setUpdateInterval(100);
	impl_->thumbTimeLine->setFrameRange(0, 3);
	this->connect(impl_->thumbTimeLine, 
		SIGNAL(frameChanged(int)), 
		SLOT(thumbDisplay(int)));

	impl_->progressTimeLine = new QTimeLine(20000, this);
	impl_->progressTimeLine->setCurveShape(QTimeLine::EaseOutCurve);
	impl_->progressTimeLine->setUpdateInterval(100);
	this->connect(impl_->progressTimeLine, 
		SIGNAL(frameChanged(int)), 
		SLOT(stepProgress(int)));
}


Screen::~Screen()
{
	SAFE_DELETE(impl_);
}


void 
Screen::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);

	impl_->lastPressedTime = impl_->startTime.elapsed();
	impl_->lastPressedPoint = event->pos();

	this->handleTouching(impl_->lastPressedPoint);

	impl_->pressTimerId = this->startTimer(impl_->pressTriggerTime);
}


void 
Screen::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);

	this->killPressTimer();
	this->hideThumb();


	QPoint pos = event->pos();
	QPoint releaseDelta = pos - impl_->lastReleasePoint;
	QPoint pressedDelta = pos - impl_->lastPressedPoint;

	int now = impl_->startTime.elapsed();
	int deltaReleaseTime = now - impl_->lastReleaseTime;
	int deltaPressedTime = now - impl_->lastPressedTime;

	if (deltaReleaseTime < 400
		&& qAbs(releaseDelta.x()) < impl_->doubleTouchRange.width()
		&& qAbs(releaseDelta.y()) < impl_->doubleTouchRange.height())
	{
		this->handleDoubleTouched(pos, deltaReleaseTime);
	}
	else if (deltaPressedTime > 150 && deltaPressedTime < 800
		&& qAbs(pressedDelta.x()) < impl_->touchRange.width()
		&& qAbs(pressedDelta.y()) < impl_->touchRange.height())
	{
		this->handleTouched(pos, deltaPressedTime);
	}


	impl_->lastReleasePoint = pos;
	impl_->lastReleaseTime = now;
}


void 
Screen::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);

	if (event->buttons() & Qt::LeftButton)
	{
		this->showThumb();
		this->killPressTimer();
	}
}


void 
Screen::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == impl_->freshInfoTimerId && this->isVisible()) 
	{
		this->updateTimeInfo();

		this->setBatteryInfo(plutoApp->batteryPercentage());

		//plutoApp->keepScreenLight(this);
	}
	else if (event->timerId() == impl_->pressTimerId)
	{
		this->killPressTimer();

		this->handleLongTouched(impl_->lastPressedPoint, impl_->pressTriggerTime);
	}
}


void 
Screen::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);

	impl_->ui.progress->resize(this->width(), impl_->ui.statusBar->height());

	impl_->stepRemain = 60;
	impl_->pageStep = this->height() - impl_->stepRemain;
}


QImage 
Screen::pageImage() const
{
	QPixmap pagePixmap = impl_->pixItem->pixmap();

	return pagePixmap.toImage();
}


void 
Screen::setBatteryInfo(int percentage)
{
	QString betteryInfo = QString("%1%").arg(percentage);

	impl_->ui.lbBattery->setText(betteryInfo);
}


void 
Screen::updateTimeInfo()
{
	impl_->ui.lbTime->setText(QTime::currentTime().toString("hh:mm"));
}


void 
Screen::setPageIndicator(int curr, int total)
{
	QString pageIndicator = QString(" - %1/%2").arg(curr).arg(total);

	impl_->ui.lbPage->setText(pageIndicator);
}


void 
Screen::setZoomInfo(int zoom)
{
	QString zoomInfo = QString("%1%]").arg(zoom);

	impl_->ui.lbZoom->setText(zoomInfo);
}





void 
Screen::setFileInfo(const QString& fileInfo)
{
	static const int MAX_FILE_LENGTH = 10;

	QString info = fileInfo;

	if (info.size() > MAX_FILE_LENGTH)
	{
		info.truncate(MAX_FILE_LENGTH);
		info = info + QString("...");
	}


	impl_->ui.lbFile->setText(QString("[%1").arg(info));
}


void 
Screen::setMessage(const QString& message)
{
	impl_->ui.lbMessage->setText(message);

	if (impl_->ui.progress->value() == impl_->ui.progress->maximum())
		this->hideProgress();
}


void 
Screen::setPageImage(const QImage& pg, 
					 const QPixmap& thumb,
					 ScrollScreenDirection direction)
{
	QPixmap pixmap = QPixmap::fromImage(pg);
	impl_->pixItem->setPixmap(pixmap);

	impl_->scene->setSceneRect(pixmap.rect());

	impl_->thumb = thumb;

	this->scrollSceen(direction);
	this->startThumbDiaplay();
}


void 
Screen::startThumbDiaplay()
{
	if (impl_->thumbTimeLine->state() != QTimeLine::Running)
	{
		impl_->thumbTimeLine->start();
	}
}


void 
Screen::showThumb()
{
	//update mask's position
	this->updateThumbMask();

	if (impl_->ui.lbThumb->pixmap() 
		&& !impl_->ui.lbThumb->isVisible()
		&& impl_->thumbVisible)
	{
		//show
		impl_->ui.lbThumb->show();

		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
}


void 
Screen::hideThumb()
{
	if (impl_->ui.lbThumb->isVisible())
	{
		impl_->ui.lbThumb->hide();
	}
}


QSize 
Screen::neededThumbSize() const
{
	return impl_->ui.lbThumb->contentsRect().size();
}


void 
Screen::generateThumb(const QPixmap& pixmap)
{
	static QPixmap alpha;

	QPixmap thumb = impl_->thumb;

	if (thumb.isNull())
	{
		thumb = pixmap.scaled(this->neededThumbSize(), 
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation);
	}

	if (alpha.size() != thumb.size())
	{
		alpha = QPixmap(thumb.size());
		alpha.fill(qRgba(200, 200, 200, 255));
	}

	thumb.setAlphaChannel(alpha);

	impl_->ui.lbThumb->setPixmap(thumb);
}


void 
Screen::startProgress(int minmimum, int maximum)
{
	impl_->ui.progress->setRange(minmimum, maximum);
	impl_->ui.progress->setValue(minmimum + (maximum - minmimum) / 3);
	impl_->ui.progress->show();

	impl_->progressTimeLine->setFrameRange(minmimum + (maximum - minmimum) / 3, maximum);
	impl_->progressTimeLine->start();
}


void 
Screen::stepProgress(int step /*= 1*/)
{
	impl_->ui.progress->setValue(step);
}


void 
Screen::endProgress()
{
	impl_->progressTimeLine->stop();
	impl_->progressTimeLine->setCurrentTime(0);

	impl_->ui.progress->setValue(impl_->ui.progress->maximum());
}


void 
Screen::hideProgress()
{
	impl_->ui.progress->hide();
}


int 
Screen::progressTime() const
{
	return impl_->progressTimeLine->currentTime();
}


void
Screen::handleTouching(QPoint pos)
{
	QString message = 
		QString("Touching screen (x %1, y %2)")
		.arg(pos.x()).arg(pos.y());

	this->setMessage(message);
}


void
Screen::handleTouched(QPoint pos, int elapsed)
{
	QString message = 
		QString("Touched screen (x %1, y %2), elapsed %3ms")
		.arg(pos.x()).arg(pos.y()).arg(elapsed);

	this->setMessage(message);
}


void
Screen::handleDoubleTouched(QPoint pos, int elapsed)
{
	QString message = 
		QString("Double touched screen (x %1, y %2), elapsed %3ms")
		.arg(pos.x()).arg(pos.y()).arg(elapsed);

	this->setMessage(message);
}


void 
Screen::handleLongTouched(QPoint pos, int elapsed)
{
	QString message = 
		QString("Long touched screen (x %1, y %2), elapsed %3ms")
		.arg(pos.x()).arg(pos.y()).arg(elapsed);

	this->setMessage(message);
}


void 
Screen::thumbDisplay(int frame)
{
	if (frame == impl_->thumbTimeLine->startFrame() ||
		!impl_->ui.lbThumb->pixmap())
	{
		this->generateThumb(impl_->pixItem->pixmap());
		this->showThumb();

	}
	else if (frame == impl_->thumbTimeLine->endFrame())
	{
		this->hideThumb();
	}
}


void 
Screen::killPressTimer()
{
	if (impl_->pressTimerId)
	{
		this->killTimer(impl_->pressTimerId);
		impl_->pressTimerId = 0;
	}
}


void 
Screen::exit()
{
	plutoApp->leaveFullScreen(this);
	plutoApp->rotateScreenToLast();

	this->close();
}


void 
Screen::scrollSceen(ScrollScreenDirection direction)
{
	if (direction == DirectionDown)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			+ impl_->pageStep);
	}
	else if (direction == DirectionUp)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			- impl_->pageStep);
	}
	else if (direction == DirectionLeft)
	{
		impl_->hScrollBar->setValue(0);
	}
	else if (direction == DirectionRight)
	{
		impl_->hScrollBar->setValue(impl_->hScrollBar->maximum());
	}
	else if (direction == DirectionUpLeft)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			- impl_->pageStep);
		impl_->hScrollBar->setValue(0);
	}
	else if (direction == DirectionUpRight)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			- impl_->pageStep);
		impl_->hScrollBar->setValue(impl_->hScrollBar->maximum());
	}
	else if (direction == DirectionDownLeft)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			+ impl_->pageStep);
		impl_->hScrollBar->setValue(0);
	}
	else if (direction == DirectionDownRight)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->value() 
			+ impl_->pageStep);
		impl_->hScrollBar->setValue(impl_->hScrollBar->maximum());
	}
	else if (direction == DirectionUpLeftCorner)
	{
		impl_->vScrollBar->setValue(0);
		impl_->hScrollBar->setValue(0);
	}
	else if (direction == DirectionUpRightCorner)
	{
		impl_->vScrollBar->setValue(0);
		impl_->hScrollBar->setValue(impl_->hScrollBar->maximum());
	}
	else if (direction == DirectionDownLeftCorner)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->maximum());
		impl_->hScrollBar->setValue(0);
	}
	else if (direction == DirectionDownRightCorner)
	{
		impl_->vScrollBar->setValue(impl_->vScrollBar->maximum());
		impl_->hScrollBar->setValue(impl_->hScrollBar->maximum());
	}

	this->startThumbDiaplay();
}


void 
Screen::setStatusBarVisible(bool visible)
{
	impl_->ui.statusBar->setVisible(visible);
}


void 
Screen::setThumbVisible(bool visible)
{
	impl_->thumbVisible = visible;
}


void 
Screen::updateThumbMask()
{
	double ratio = impl_->ui.lbThumb->contentsRect().height() * 1.0 /
		this->sceneRect().height();

	int startY = qRound(impl_->vScrollBar->value() * ratio + impl_->thumbBoundary);
	int height = qRound(this->height() * ratio);

	impl_->ui.lbThumbMask->setGeometry(impl_->thumbBoundary, startY,
		impl_->ui.lbThumb->contentsRect().width(), height);
}


void 
Screen::updateMemoryInfo()
{
	Platform::MemoryStatus ms = plutoApp->memoryStatus();

	QString meminfo = QString("%1/%2M")
		.arg((int)ms.usedPhys).arg((int)ms.totalPhys);

	impl_->ui.lbMem->setText(meminfo);
}


//void 
//Screen::switchToGl()
//{
//	//use opengl
//	this->setViewport(new QGLWidget(QGL::AlphaChannel | 
//		QGL::SampleBuffers /*| QGL::HasOverlay*/)
//		);
//
//	this->setMessage(QString("Switch viewport to OpenGl"));
//}


SipLineEdit::SipLineEdit(QWidget *parent /*= 0*/)
: QLineEdit(parent)
{
	this->connect(this, SIGNAL(focusIn()), SLOT(onFocusIn()), Qt::QueuedConnection);
}



SipLineEdit::~SipLineEdit()
{

}


void 
SipLineEdit::focusInEvent(QFocusEvent * event)
{
	QLineEdit::focusInEvent(event);
	plutoApp->openSip();


	emit focusIn();
	//this->home(true);
}


void 
SipLineEdit::focusOutEvent(QFocusEvent * event)
{
	QLineEdit::focusOutEvent(event);

	plutoApp->closeSip();
}


void 
SipLineEdit::onFocusIn()
{
	this->home(true);
}

}
