/*******************************************************************************
**
**  NAME:			ppScreen.cpp	
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
#include "stdafx.h"
#include "PDFScreen.h"
#include "PDFMenu.h"
#include "PDFReader.h"


namespace pdf {



static const double MAX_CUT_MARGIN = 0.3;
static const quint32 MAX_ACCEPT_MEM_USE = 85;
static const QString DEFAULT_STYLE = "style/shinynoir.qss";

struct PDFScreen::PDFScreenImpl
{
	PDFScreenImpl()
		: zoomLevel (PDFReader::FitWidth)
	{

	}

	PDFMenu* menu;
	QGraphicsScene* scene;

	//ui layout control
	QRect trCorner;
	QRect brCorner;
	QRect tlCorner;
	QRect blCorner;
	QRect center;

	QGraphicsRectItem* screenRegion;

	//pdf engine
	PDFReader pdfReader;

	//Settings
	QSettings* settings;
	QString firstPdfBook;
	QStringList recentPdfBooks;

	bool showThumb;
	bool showStatus;

	QString styleFile;

	//book open parameters
	PDFReader::ZoomLevel zoomLevel;

	double leftMargin;
	double rightMargin;
	double topMargin;
	double bottomMargin;

	int ignoreCutPages;
	int pageNumber;

	void setupLayout()
	{
		int cornerW = plutoApp->screenResolution().width() / 6;
		int cornerH = plutoApp->screenResolution().height() / 5;

		tlCorner = QRect(0,				0,				cornerW,	cornerH);
		trCorner = QRect(cornerW * 5,	0,				cornerW,	cornerH);

		blCorner = QRect(0,				cornerH * 4,	cornerW,	cornerH);
		brCorner = QRect(cornerW * 5,	cornerH * 4,	cornerW,	cornerH);

		center =   QRect(cornerW * 2.5,	cornerH * 2,	cornerW,	cornerH);


		QList<QRect> rects;
		rects<<tlCorner<<trCorner<<center<<blCorner<<brCorner;

		screenRegion = 
			scene->addRect(0, 0, 
			plutoApp->screenResolution().width(), 
			plutoApp->screenResolution().height());

		screenRegion->setBrush(QColor("transparent"));
		screenRegion->setPen(QPen(QColor("transparent")));

		foreach (QRect rc, rects)
		{
			QGraphicsRectItem* rcItem = scene->addRect(rc);

			rcItem->setZValue(2.0);
			rcItem->setBrush(QColor(0, 0, 0, 150));
			rcItem->setPen(QPen(QColor("transparent")));

			rcItem->setParentItem(screenRegion);
		}

		screenRegion->hide();
	}

	void addIntoRecentList(const QString& file)
	{
		QString nativeFile = QDir::toNativeSeparators(file);

		recentPdfBooks.removeAll(nativeFile);
		recentPdfBooks.prepend(nativeFile);

		writeSettings();
	}


	void readSettings()
	{
		settings->beginGroup(plutoApp->applicationName());

		firstPdfBook = settings->value("lastReadingBook").toString();
		recentPdfBooks = settings->value("recentReadingBooks").toStringList();

		showThumb = settings->value("showThumb", true).toBool();
		showStatus = settings->value("showStatus", true).toBool();

		styleFile = settings->value("style", DEFAULT_STYLE).toString();

		settings->endGroup();
	}


	void readSettings(const QString& pdfFile)
	{
		settings->beginGroup(plutoApp->applicationName());
		settings->beginGroup(QFileInfo(pdfFile).fileName());

		leftMargin = qMin(qMax(settings->value("leftMargin").toDouble(), 0.0), MAX_CUT_MARGIN); 
		rightMargin = qMin(qMax(settings->value("rightMargin").toDouble(), 0.0), MAX_CUT_MARGIN); 
		topMargin = qMin(qMax(settings->value("topMargin").toDouble(), 0.0), MAX_CUT_MARGIN); 
		bottomMargin = qMin(qMax(settings->value("bottomMargin").toDouble(), 0.0), MAX_CUT_MARGIN); 

		ignoreCutPages = settings->value("ignoreCutPages").toUInt();
		pageNumber = settings->value("lastReadingPage").toUInt(); 

		zoomLevel = (PDFReader::ZoomLevel)
			settings->value("zoomLevel", PDFReader::FitWidth).toInt();

		settings->endGroup();
		settings->endGroup();
	}


	void writeSettings()
	{
		settings->beginGroup(plutoApp->applicationName());

		settings->setValue("lastReadingBook", pdfReader.filePath());
		settings->setValue("recentReadingBooks", recentPdfBooks);

		settings->setValue("showThumb", showThumb);
		settings->setValue("showStatus", showStatus);

		settings->setValue("style", styleFile);

		settings->setValue("rotateAngle", plutoApp->realScreenRotateAngle());

		settings->beginGroup(pdfReader.fileName());

		settings->setValue("leftMargin", leftMargin);
		settings->setValue("rightMargin", rightMargin);
		settings->setValue("topMargin", topMargin);
		settings->setValue("bottomMargin", bottomMargin);

		settings->setValue("ignoreCutPages", ignoreCutPages);
		settings->setValue("lastReadingPage", pageNumber); 

		settings->setValue("zoomLevel", (int)zoomLevel);

		settings->endGroup();
		settings->endGroup();

		settings->sync();
	}
};


PDFScreen::PDFScreen(QWidget *parent)
: pltScreen(parent)
, impl_(new PDFScreenImpl)
{
	plutoApp->setApplicationName("PlutoPDF");
	plutoApp->setApplicationVersion("v0.1.3.0");
	plutoApp->setOrganizationName("PlutoWare - Roger.Yi (roger2yi@gmail.com)");


	impl_->scene = this->scene();
	__ASSERT_E(impl_->scene, "Scene not created.");

	impl_->menu = new PDFMenu(this);
	impl_->menu->move(0, 0);
	impl_->menu->hide();

	this->connect(impl_->menu, SIGNAL(askExit()), SLOT(exit()));
	this->connect(impl_->menu, SIGNAL(askOpen()), SLOT(getPdfBook()));
	this->connect(impl_->menu, SIGNAL(askHelp()), SLOT(onAskHelp()));
	this->connect(impl_->menu, SIGNAL(askCut(int)), SLOT(onAskCut(int)));
	this->connect(impl_->menu, SIGNAL(askJump(int)), SLOT(onAskJump(int)));
	this->connect(impl_->menu, SIGNAL(askZoom(int)), SLOT(onAskZoom(int)));
	this->connect(impl_->menu, SIGNAL(askRecent(const QString&)), SLOT(onAskRecent(const QString&)));
	this->connect(impl_->menu, SIGNAL(askChangeSettings(bool, bool)),
		SLOT(onAskChangeSettings(bool, bool)));
	this->connect(impl_->menu, SIGNAL(askChangeStyle(const QString&)),
		SLOT(onAskChangeStyle(const QString&)));
	this->connect(impl_->menu, SIGNAL(askRotate180()), SLOT(onAskRotate180()));

	//layout
	impl_->setupLayout();

	//settings
	impl_->settings = new QSettings(plutoApp->pathRelateToAppDir("config/config.ini"),
		QSettings::IniFormat,
		this);

	impl_->readSettings();
	impl_->menu->setCurrentSettings(impl_->showThumb, impl_->showStatus);

	//load style
	plutoApp->loadStyle(impl_->styleFile);

	this->setThumbVisible(impl_->showThumb);
	this->setStatusBarVisible(impl_->showStatus);
}


PDFScreen::~PDFScreen(void)
{
	SAFE_DELETE(impl_);
}


void 
PDFScreen::getPdfBook()
{
	QString pdfFile = plutoApp->getOpenFileName(this, 
		plutoApp->applicationName(),
		"\\Disk\\EBook", 
		"*.pdf;");

	if (!pdfFile.isEmpty())
	{
		this->openPdfBook(pdfFile);
	}
}


void 
PDFScreen::openPdfBook(const QString& pdfFile)
{
	QTime time; time.start();
	this->startProgress(0, 10); this->stepProgress(3);

	//read settings
	impl_->readSettings(pdfFile);

	try
	{
		//open
		impl_->pdfReader.open(pdfFile); 
		impl_->addIntoRecentList(pdfFile);
		
		this->stepProgress(2);

		//render start page
		this->renderPage(); 

		this->setFileInfo(impl_->pdfReader.fileBaseName());
		this->setMessage(QString("Open %1 successes, used %2ms")
			.arg(impl_->pdfReader.fileName())
			.arg(time.elapsed()));
	}
	catch (pltException& e)
	{
		this->setMessage(QString("ERR: %1").arg(e.toString()));
	}

	this->endProgress();
}


void 
PDFScreen::renderPage()
{
	if (impl_->pdfReader.isNull())
		return;

	QTime time; time.start();
	this->startProgress(0, 10); this->stepProgress(5);

	try
	{
		//render
		impl_->pdfReader.setMargin(impl_->leftMargin, 
			impl_->rightMargin, 
			impl_->topMargin, 
			impl_->bottomMargin, 
			impl_->ignoreCutPages);

		QImage pg = impl_->pdfReader.renderFitWidth(impl_->pageNumber,
			plutoApp->screenResolution().width(),
			impl_->zoomLevel);

		//if necessary, clear store buffer to save memory
		this->updateMemoryInfo();

		if (plutoApp->memoryStatus().memoryLoad > MAX_ACCEPT_MEM_USE)
		{
			impl_->pdfReader.clearEngineBuffer();
			this->updateMemoryInfo();
		}

		int realPageNo = impl_->pageNumber + 1;

		if (pg.isNull())
		{
			__THROW_L(PDFException, QString("Render p%1 failed").arg(realPageNo));
		}

		//painting
		this->setPageImage(pg);

		//update ui info
		this->setPageIndicator(realPageNo, impl_->pdfReader.pageCount());
		this->setZoomInfo(qAbs((int)impl_->zoomLevel));

		impl_->menu->setCurrentPageNo(realPageNo);

		this->setMessage(QString("Render p%1 successes, used %2ms, mg %3, %4, zm %5")
			.arg(realPageNo)
			.arg(time.elapsed())
			.arg(impl_->leftMargin)
			.arg(impl_->rightMargin)
			.arg(qAbs((int)impl_->zoomLevel))
			);

		this->setFocus();
	}
	catch (pltException& e)
	{
		this->setMessage(QString("ERR: %1").arg(e.toString()));
	}

	this->endProgress();
}


void 
PDFScreen::handleTouching(QPoint /*pos*/)
{
	//pltScreen::handleTouching(pos);
}


void 
PDFScreen::handleTouched(QPoint pos, int elapsed)
{
	pltScreen::handleTouched(pos, elapsed);

	if (impl_->tlCorner.contains(pos))
	{
		//top-left, show menu
		this->scrollSceen(DirectionUpLeft);
	}
	else if (impl_->trCorner.contains(pos))
	{
		this->scrollSceen(DirectionUpRight);
	}
	else if (impl_->blCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownLeft);
	}
	else if (impl_->brCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownRight);
	}
	else if (pos.y() < this->rect().height() / 2)
	{
		this->scrollUp();
	}
	else
	{
		scrollDown();

	}
}


void 
PDFScreen::handleDoubleTouched(QPoint pos, int elapsed)
{
	pltScreen::handleDoubleTouched(pos, elapsed);

	if (impl_->tlCorner.contains(pos))
	{
		//top-left, show menu
		this->showMenu();
	}
	else if (impl_->trCorner.contains(pos))
	{
		//top-right, open file
		getPdfBook();
	}
	else if (impl_->blCorner.contains(pos))
	{
		scrollPage(-10);
	}
	else if (impl_->brCorner.contains(pos))
	{
		scrollPage(10);
	}
	else
	{
		if (pos.y() < this->rect().height() / 2)
		{
			this->scrollPrePage();
		}
		else
		{
			this->scrollNextPage();
		}
	}
}


void 
PDFScreen::handleLongTouched(QPoint pos, int elapsed)
{
	pltScreen::handleLongTouched(pos, elapsed);

#ifdef _DEBUG
	plutoApp->reloadStyle();//reload style

	impl_->screenRegion->setVisible(!impl_->screenRegion->isVisible());
#endif // _DEBUG

}


void 
PDFScreen::scrollToPage(int pageNo)
{
	if (!impl_->pdfReader.isNull())
	{
		pageNo = qMin(qMax(pageNo, 0), impl_->pdfReader.pageCount() - 1);

		if (pageNo != impl_->pageNumber)
		{
			impl_->pageNumber = pageNo;

			//render
			this->renderPage();

			//write external settings
			impl_->writeSettings();
		}
	}
}


void 
PDFScreen::scrollPage(int delta)
{
	int nextPage = impl_->pageNumber + delta;

	this->scrollToPage(nextPage);
}


void 
PDFScreen::scrollPrePage()
{
	this->scrollPage(-1);
	this->scrollSceen(DirectionDownLeft);
	this->updateThumbMask();
}


void 
PDFScreen::scrollNextPage()
{
	this->scrollPage(1);
}


void 
PDFScreen::scrollUp()
{
	//previous screen or previous page
	if (this->verticalScrollBar()->value() <= 
		this->verticalScrollBar()->minimum())
	{
		this->scrollPrePage();
	}
	else
	{
		this->scrollSceen(DirectionUp);
	}
}


void 
PDFScreen::scrollDown()
{
	//next screen or next page
	if (this->verticalScrollBar()->value() >=
		this->verticalScrollBar()->maximum())
	{
		this->scrollNextPage();
	}
	else
	{
		this->scrollSceen(DirectionDown);
	}
}


void 
PDFScreen::mousePressEvent(QMouseEvent *event)
{
	impl_->menu->hide();

	pltScreen::mousePressEvent(event);
}


void 
PDFScreen::mouseReleaseEvent(QMouseEvent *event)
{

	pltScreen::mouseReleaseEvent(event);
}


void 
PDFScreen::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Down)
	{
		this->scrollUp();
	}
	else if (event->key() == Qt::Key_Up)
	{
		this->scrollDown();
	}
	else
	{
		pltScreen::keyPressEvent(event);
	}

	this->setMessage(QString("Key pressed").arg(event->key()));
}


void 
PDFScreen::showEvent(QShowEvent *event)
{
	static bool first = true;

	pltScreen::showEvent(event);
	QCoreApplication::processEvents();

	if (first)
	{
		first = false;

		this->connect(this, 
			SIGNAL(firstShow()),
			SLOT(onFirstShown()), 
			Qt::QueuedConnection);

		emit firstShow();
	}
}


void 
PDFScreen::setFirstPdfBook(const QString& pdfFile)
{
	impl_->firstPdfBook = pdfFile;
}


void 
PDFScreen::onFirstShown()
{
	QString file =  plutoApp->fileInArgument();;

	if (QFile::exists(file))
	{
		impl_->firstPdfBook = file;
	}
	//else if (!impl_->firstPdfBook.isEmpty())
	//{
	//	//ask open last reading
	//	QMessageBox::StandardButton result = plutoApp->msgboxQuestion(this, 
	//		plutoApp->applicationName(),
	//		tr("Open last reading book - %1")
	//		.arg(QFileInfo(impl_->firstPdfBook).fileName()));

	//	if (result == QMessageBox::No)
	//	{
	//		impl_->firstPdfBook = "";//clear
	//	}
	//}

	if (!QFile::exists(impl_->firstPdfBook))
	{
		impl_->firstPdfBook = plutoApp->helpFile();
	}

	this->openPdfBook(impl_->firstPdfBook);
}


void 
PDFScreen::showMenu()
{
	//impl_->menu->setApplicationName(QString("%1")
	//	.arg(plutoApp->applicationName()/*, plutoApp->applicationVersion()*/));

	impl_->menu->setRecentOpenFiles(impl_->recentPdfBooks);
	impl_->menu->show();
}


void 
PDFScreen::onAskHelp()
{
	this->openPdfBook(plutoApp->helpFile());
}


void 
PDFScreen::onAskCut(int req)
{
	PDFMenu::CutRequest cutReg = (PDFMenu::CutRequest)req;

	if (cutReg == PDFMenu::CutLeft3)
	{
		if (impl_->leftMargin < MAX_CUT_MARGIN)
		{
			impl_->leftMargin += 0.03;
			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutRight3)
	{
		if (impl_->rightMargin < MAX_CUT_MARGIN)
		{
			impl_->rightMargin += 0.03;
			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutLeft5)
	{
		if (impl_->leftMargin < MAX_CUT_MARGIN)
		{
			impl_->leftMargin += 0.05;
			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutRight5)
	{
		if (impl_->rightMargin < MAX_CUT_MARGIN)
		{
			impl_->rightMargin += 0.05;
			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutAuto)
	{
		QImage pageImg = this->pageImage();

		int leftMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Left);
		int rightMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Right);

		double originalW = pageImg.width() * 1.0 / (1.0 - impl_->leftMargin - impl_->rightMargin);

		impl_->leftMargin += leftMg / originalW;
		impl_->rightMargin += rightMg / originalW;

		impl_->leftMargin = (int)(impl_->leftMargin * 100) / 100.0;
		impl_->rightMargin = (int)(impl_->rightMargin * 100) / 100.0;

		this->renderPage();
	}
	else if (cutReg == PDFMenu::CutReset)
	{
		//reset margin
		impl_->leftMargin = impl_->rightMargin = 0.0;
		this->renderPage();
	}

	impl_->writeSettings();
}


void 
PDFScreen::onAskZoom(int zoomLevel)
{
	impl_->zoomLevel = (PDFReader::ZoomLevel)zoomLevel;

	this->renderPage();

	impl_->writeSettings();
}


void 
PDFScreen::onAskJump(int pageNo)
{
	this->scrollToPage(pageNo - 1);
}



void 
PDFScreen::onAskRecent(const QString& recentFile)
{
	if (QFile::exists(recentFile))
	{
		this->openPdfBook(recentFile);
	}
}	


void 
PDFScreen::onAskChangeSettings(bool showThumb, 
							   bool showStatus)
{
	impl_->showThumb = showThumb;
	impl_->showStatus = showStatus;

	this->setThumbVisible(showThumb);
	this->setStatusBarVisible(showStatus);

	impl_->writeSettings();
}


void 
PDFScreen::onAskChangeStyle(const QString& styleFile)
{
	plutoApp->loadStyle(styleFile);

	impl_->styleFile = styleFile;
	impl_->writeSettings();
}


void 
PDFScreen::onAskRotate180()
{
	plutoApp->advance180degree();

	impl_->writeSettings();
}


#ifdef _WIN32_WCE
bool 
PDFScreen::winEvent(MSG *message, long *result)
{
	if (message->message == WM_ACTIVATE)
	{
		if (message->wParam == WA_ACTIVE || message->wParam == WA_CLICKACTIVE)
		{
			plutoApp->holdShellKey(this);

			pltPlatform::ScreenRotateAngle realAngle = plutoApp->realScreenRotateAngle();
			pltPlatform::ScreenRotateAngle currAngle = plutoApp->currentScreenRotateAngle();

			this->setMessage(QString("AP re-active, real rotate angle %1, curr %2")
				.arg(realAngle).arg(currAngle));

			if (currAngle != realAngle)
			{
				//the screen rotate outside, need rotate back
				this->setMessage(QString("!!!Rotate outside, need rotate back by myself."));

				plutoApp->rotateScreenToCurrent();
				plutoApp->enterFullScreen(this);

				result = 0;
				return true;
			}
		}
		else if (message->wParam == WA_INACTIVE)
		{
			plutoApp->releaseShellKey(this);
			plutoApp->leaveFullScreen(this);
		}
	}
	else if (message->message == plutoApp->getShellEventId())
	{
		quint32 keyid = LOWORD(message->wParam);
		
		if (keyid == pltPlatform::WPARAM_KEY_EVENT_CLICK_VOLDOWN)
		{
			this->scrollUp();
		}
		else if (keyid == pltPlatform::WPARAM_KEY_EVENT_CLICK_VOLUP)
		{
			this->scrollDown();
		}

	}

	return false;
}


#endif

//end namespace
}

