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
#include "PDFThreadReader.h"



namespace pdf {



static const double MAX_CUT_MARGIN = 0.3;
static const QString DEFAULT_STYLE = "style/shinynoir.qss";

struct PDFScreen::PDFScreenImpl
{
	PDFScreenImpl()
		: zoomLevel (PDFReader::FitWidth)
		, screenRegion (NULL)
		, scrollDirection (pltScreen::DirectionUpLeftCorner)
	{

	}

	pltM8Platform::ScreenRotateAngle screenAngle;
	pltScreen::ScrollScreenDirection scrollDirection;

	PDFMenu* menu;
	QGraphicsScene* scene;

	//ui layout control
	QRect trCorner;
	QRect brCorner;
	QRect tlCorner;
	QRect blCorner;
	QRect mlCorner;
	QRect mrCorner;
	QRect center;

	QGraphicsRectItem* screenRegion;

	//pdf engine
	PDFThreadReader pdfReader;

	//Settings
	QSettings* settings;
	QString firstPdfBook;
	QStringList recentPdfBooks;

	bool showThumb;
	bool showStatus;
	bool useHomeKey;
	bool useCache;
	bool useAcc;

	QString styleFile;

	//book open parameters
	PDFReader::ZoomLevel zoomLevel;

	double leftMargin;
	double rightMargin;
	double topMargin;
	double bottomMargin;

	int ignoreCutPages;
	int pageNumber;

	void setupHotspots(QWidget* topWin)
	{
		if (screenRegion)
		{
			scene->removeItem(screenRegion);
			SAFE_DELETE(screenRegion);
		}

		int cornerW = 100;
		int cornerH = 100;

		tlCorner = QRect(0, 0, cornerW, cornerH);
		trCorner = QRect(topWin->width() - cornerW,	
			0,			
			cornerW,	
			cornerH);

		mlCorner = QRect(0, 
			(topWin->height() - cornerH) / 2,
			cornerW,
			cornerH);
		mrCorner = QRect(topWin->width() - cornerW,	
			(topWin->height() - cornerH) / 2,			
			cornerW,	
			cornerH);

		blCorner = QRect(0,				
			topWin->height() - cornerH,
			cornerW,	
			cornerH);
		brCorner = QRect(topWin->width() - cornerW,
			topWin->height() - cornerH,	
			cornerW,	
			cornerH);

		center =   QRect((topWin->width() - cornerW) / 2,	
			(topWin->height() - cornerH) / 2,
			cornerW,
			cornerH);


		QList<QRect> rects;
		rects<<tlCorner<<trCorner<<mlCorner<<mrCorner<<center<<blCorner<<brCorner;

		screenRegion = 
			scene->addRect(0, 0, 
			topWin->width(), topWin->height());

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

		writeSettings(false);
	}


	void readSettings()
	{
		settings->beginGroup(plutoApp->applicationName());

		firstPdfBook = settings->value("lastReadingBook").toString();
		recentPdfBooks = settings->value("recentReadingBooks").toStringList();

		showThumb = settings->value("showThumb", true).toBool();
		showStatus = settings->value("showStatus", true).toBool();
		useHomeKey = settings->value("useHomeKey", true).toBool();
		useCache = settings->value("useCache", true).toBool();
		useAcc = settings->value("openAcc", true).toBool();

		styleFile = settings->value("style", DEFAULT_STYLE).toString();

		screenAngle = (pltM8Platform::ScreenRotateAngle)
			settings->value("rotateAngle", plutoApp->realScreenRotateAngle()).toInt();

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

		ignoreCutPages = settings->value("ignoreCutPages", 0).toInt();
		pageNumber = qMax(settings->value("lastReadingPage", 1).toInt(), 1);

		zoomLevel = (PDFReader::ZoomLevel)
			settings->value("zoomLevel", PDFReader::FitWidth).toInt();

		settings->endGroup();
		settings->endGroup();
	}


	void writeSettings(bool writeScreenAngle = true)
	{
		settings->beginGroup(plutoApp->applicationName());

		settings->setValue("lastReadingBook", pdfReader.filePath());
		settings->setValue("recentReadingBooks", recentPdfBooks);

		settings->setValue("showThumb", showThumb);
		settings->setValue("showStatus", showStatus);
		settings->setValue("useHomeKey", useHomeKey);
		settings->setValue("useCache", useCache);
		settings->setValue("openAcc", useAcc);

		settings->setValue("style", styleFile);

		if (writeScreenAngle)
		{
			screenAngle = plutoApp->realScreenRotateAngle();
			settings->setValue("rotateAngle", screenAngle);
		}

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
	plutoApp->setOrganizationName("PlutoWare - Roger.Yi (roger2yi@gmail.com)");

	this->setWindowTitle("Roger's PlutoPDF");


	//scene
	impl_->scene = this->scene();
	__ASSERT_E(impl_->scene, "Scene not created.");

	//menu
	impl_->menu = new PDFMenu(this);
	impl_->menu->move(0, 0);
	impl_->menu->hide();

	//connect signals and slots
	this->connect(impl_->menu, SIGNAL(askExit()), SLOT(exit()));
	this->connect(impl_->menu, SIGNAL(askOpen()), SLOT(getPdfBook()));
	this->connect(impl_->menu, SIGNAL(askHelp()), SLOT(onAskHelp()));
	this->connect(impl_->menu, SIGNAL(askCut(int)), SLOT(onAskCut(int)));
	this->connect(impl_->menu, SIGNAL(askJump(int)), SLOT(onAskJump(int)));
	this->connect(impl_->menu, SIGNAL(askZoom(int)), SLOT(onAskZoom(int)));
	this->connect(impl_->menu, SIGNAL(askRecent(const QString&)), SLOT(onAskRecent(const QString&)));
	this->connect(impl_->menu, 
		SIGNAL(askChangeSettings(bool, bool, bool, bool, bool)), 
		SLOT(onAskChangeSettings(bool, bool, bool, bool, bool)));
	this->connect(impl_->menu, SIGNAL(askChangeStyle(const QString&)), SLOT(onAskChangeStyle(const QString&)));
	this->connect(impl_->menu, SIGNAL(askRotate90()), SLOT(onAskRotate90()));
	this->connect(impl_->menu, SIGNAL(askRotate180()), SLOT(onAskRotate180()));

	this->connect(this, SIGNAL(firstShow()), SLOT(onFirstShown()), Qt::QueuedConnection);

	this->connect(&impl_->pdfReader, SIGNAL(rendered(int, QImage, QImage)), SLOT(onRendered(int, QImage, QImage))/*, Qt::QueuedConnection*/);
	this->connect(&impl_->pdfReader, SIGNAL(renderError(QString)), SLOT(onRenderError(QString)));
	this->connect(&impl_->pdfReader, SIGNAL(rendering(QString)), SLOT(onRendering(QString)));
	this->connect(&impl_->pdfReader, SIGNAL(cached(QString)), SLOT(onCached(QString)));

	impl_->pdfReader.setConvertTo16Bits(true);
	impl_->pdfReader.setUseFastCompressAlgo(true);
	impl_->pdfReader.setThumbSize(this->neededThumbSize());

	//settings
	impl_->settings = new QSettings(plutoApp->pathRelateToAppDir("config/config.ini"),
		QSettings::IniFormat,
		this);

	//system configuration
	impl_->readSettings();

	impl_->menu->setCurrentSettings(impl_->showThumb, 
		impl_->showStatus, 
		impl_->useHomeKey, 
		impl_->useCache,
		impl_->useAcc);

	this->setThumbVisible(impl_->showThumb);
	this->setStatusBarVisible(impl_->showStatus);

	impl_->pdfReader.setUseCache(impl_->useCache);

	//load style
	plutoApp->loadStyle(impl_->styleFile);
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

	if (this->openPdfBook(pdfFile))
	{
		this->renderPage();
	}
}


bool 
PDFScreen::openPdfBook(const QString& pdfFile)
{	
	QFileInfo fi(pdfFile);
	if (!fi.exists() || fi == QFileInfo(impl_->pdfReader.filePath()))
	{
		return false;
	}

	QTime time;
	time.start();

	//read settings
	impl_->readSettings(pdfFile);

	try
	{
		//open
		impl_->pdfReader.open(pdfFile); 
		impl_->addIntoRecentList(pdfFile);

		this->setFileInfo(impl_->pdfReader.fileBaseName());

		this->setMessage(QString("Open %1 successes, used %2ms")
			.arg(impl_->pdfReader.fileName())
			.arg(time.elapsed()));

		return true;
	}
	catch (pltException& e)
	{
		this->setMessage(QString("Can not open %1 - %2")
			.arg(QFileInfo(pdfFile).fileName(), e.toString()));

		return false;
	}
}


void 
PDFScreen::renderPage(int screenWidth, bool wait)
{
	if (impl_->pdfReader.isNull())
		return;

	if (screenWidth <= 0)
		screenWidth = this->width();

	//start progress display
	this->startProgress();

	//render
	this->updateMemoryInfo();

	impl_->pdfReader.setMargin(impl_->leftMargin, 
		impl_->rightMargin, 
		impl_->topMargin, 
		impl_->bottomMargin, 
		impl_->ignoreCutPages);

	impl_->pdfReader.setRenderParams(impl_->zoomLevel, screenWidth, 0);

	impl_->pdfReader.askRender(impl_->pageNumber, wait);
}


void 
PDFScreen::onRendered(int pageNo, QImage image, QImage thumb)
{
	//if necessary, clear store buffer to save memory
	this->updateMemoryInfo();

	//painting
	this->setPageImage(image, 
		QPixmap::fromImage(thumb), 
		impl_->scrollDirection);

	impl_->scrollDirection = DirectionUpLeftCorner;//set back


	//update ui info
	this->setPageIndicator(pageNo, impl_->pdfReader.pageCount());
	this->setZoomInfo(qAbs((int)impl_->zoomLevel));

	impl_->menu->setCurrentPageNo(pageNo);

	this->setMessage(QString("Render p%1 OK, %2ms, mg %3, %4, zm %5")
		.arg(pageNo)
		.arg(this->progressTime())
		.arg(impl_->leftMargin)
		.arg(impl_->rightMargin)
		.arg(qAbs((int)impl_->zoomLevel))
		);


	//end progress display
	this->endProgress();

	this->setFocus();
}


void 
PDFScreen::onRendering(QString msg)
{
	this->setMessage(msg);
}


void 
PDFScreen::onRenderError(QString errMsg)
{
	this->setMessage(errMsg);

	//hide progress display
	this->hideProgress();
}


void 
PDFScreen::onCached(QString cacheMsg)
{
	this->setMessage(cacheMsg);
	this->updateMemoryInfo();
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
		this->scrollSceen(DirectionUpLeftCorner);
	}
	else if (impl_->trCorner.contains(pos))
	{
		this->scrollSceen(DirectionUpRightCorner);
	}
	else if (impl_->mlCorner.contains(pos))
	{
		this->scrollSceen(DirectionLeft);
	}
	else if (impl_->mrCorner.contains(pos))
	{
		this->scrollSceen(DirectionRight);
	}
	else if (impl_->blCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownLeftCorner);
	}
	else if (impl_->brCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownRightCorner);
	}
	else if (pos.y() < this->rect().height() / 3)
	{
		this->scrollUp();
	}
	else if (pos.y() > this->rect().height() * 2 / 3)
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
	else if (impl_->mlCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownLeft);
	}
	else if (impl_->mrCorner.contains(pos))
	{
		this->scrollSceen(DirectionDownRight);
	}
	else if (impl_->center.contains(pos))
	{
		plutoApp->rotateScreen(impl_->screenAngle);
		plutoApp->enterFullScreen(this);

		impl_->setupHotspots(this);
	}
	else
	{
		if (pos.y() < this->rect().height() / 3)
		{
			this->scrollPrePage();
		}
		else if (pos.y() > this->rect().height() * 2 / 3)
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
		pageNo = qBound(1, pageNo, impl_->pdfReader.pageCount());

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
	impl_->scrollDirection = DirectionDownLeftCorner;

	this->scrollPage(-1);
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
	pltScreen::showEvent(event);
	plutoApp->processEvents();

	static bool first = true;
	if (first)
	{
		first = false;
		emit firstShow();
	}
}


void 
PDFScreen::closeEvent(QCloseEvent *event)
{
	//this->setMessage("Close window");
	//event->ignore();

	pltScreen::closeEvent(event);
}


void 
PDFScreen::onFirstShown()
{
	//rotate
	plutoApp->setAccOpen(impl_->useAcc);

	if (impl_->useAcc)
	{
		plutoApp->rotateScreen(plutoApp->realScreenRotateAngle());
	}
	else
	{
		plutoApp->rotateScreen(impl_->screenAngle);
	}
	
	plutoApp->enterFullScreen(this);
	impl_->setupHotspots(this);
}


void 
PDFScreen::setFirstPdfBook(const QString& pdfFile)
{
	impl_->firstPdfBook = pdfFile;
}


void 
PDFScreen::openFirstPdfBook()
{
	QString file =  plutoApp->fileInArgument();;

	if (QFile::exists(file))
	{
		impl_->firstPdfBook = file;
	}

	if (!QFile::exists(impl_->firstPdfBook))
	{
		impl_->firstPdfBook = plutoApp->helpFile();
	}

	if (this->openPdfBook(impl_->firstPdfBook))
	{
		//render page, the needed width will depend on newAngle and current Angle
		pltPlatform::ScreenRotateAngle realAngle = 
			plutoApp->realScreenRotateAngle();

		int deltaAngle = plutoApp->deltaAngle(realAngle, impl_->screenAngle);

		QDesktopWidget dw;

		if (impl_->useAcc)
		{
			this->renderPage(dw.screenGeometry().width());
		}
		else if ((deltaAngle == 90 || deltaAngle == 270))
		{
			this->renderPage(dw.screenGeometry().height());
		}
		else
		{
			this->renderPage(dw.screenGeometry().width());
		}
	}
}


void 
PDFScreen::showMenu()
{
	impl_->menu->setRecentOpenFiles(impl_->recentPdfBooks);
	impl_->menu->show();
}


void 
PDFScreen::onAskHelp()
{
	if (this->openPdfBook(plutoApp->helpFile()))
	{
		this->renderPage();
	}
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

			impl_->scrollDirection = DirectionNone;

			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutRight3)
	{
		if (impl_->rightMargin < MAX_CUT_MARGIN)
		{
			impl_->rightMargin += 0.03;

			impl_->scrollDirection = DirectionNone;

			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutLeft5)
	{
		if (impl_->leftMargin < MAX_CUT_MARGIN)
		{
			impl_->leftMargin += 0.05;

			impl_->scrollDirection = DirectionNone;

			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutRight5)
	{
		if (impl_->rightMargin < MAX_CUT_MARGIN)
		{
			impl_->rightMargin += 0.05;

			impl_->scrollDirection = DirectionNone;

			this->renderPage();
		}
	}
	else if (cutReg == PDFMenu::CutAuto)
	{
		QImage pageImg = this->pageImage();

		int leftMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Left);
		int rightMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Right);
		int topMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Top);
		int bottomMg = pltImageProcessor::margin(pageImg, pltImageProcessor::Bottom);

		double originalW = pageImg.width() * 1.0 / (1.0 - impl_->leftMargin - impl_->rightMargin);
		double originalH = pageImg.height() * 1.0 / (1.0 - impl_->topMargin - impl_->bottomMargin);

		impl_->leftMargin += leftMg / originalW;
		impl_->rightMargin += rightMg / originalW;
		impl_->topMargin += topMg / originalH;
		impl_->bottomMargin += bottomMg / originalH;

		impl_->leftMargin = ((int)(impl_->leftMargin * 100)) / 100.0;
		impl_->rightMargin = ((int)(impl_->rightMargin * 100)) / 100.0;
		
		impl_->topMargin = qMax(((int)(impl_->topMargin * 100)) / 100.0 - 0.02, 0.0);
		impl_->bottomMargin = qMax(((int)(impl_->bottomMargin * 100)) / 100.0 - 0.05, 0.0);

		impl_->scrollDirection = DirectionNone;

		this->renderPage();
	}
	else if (cutReg == PDFMenu::CutReset)
	{
		//reset margin
		impl_->leftMargin = impl_->rightMargin = 0.0;

		impl_->scrollDirection = DirectionNone;

		this->renderPage();
	}

	impl_->writeSettings();
}


void 
PDFScreen::onAskZoom(int zoomLevel)
{
	impl_->zoomLevel = (PDFReader::ZoomLevel)zoomLevel;

	impl_->scrollDirection = DirectionNone;

	this->renderPage();

	impl_->writeSettings();
}


void 
PDFScreen::onAskJump(int pageNo)
{
	this->scrollToPage(pageNo);
}



void 
PDFScreen::onAskRecent(const QString& recentFile)
{
	if (this->openPdfBook(recentFile))
	{
		this->renderPage();
	}
}	


void 
PDFScreen::onAskChangeSettings(bool showThumb, 
							   bool showStatus,
							   bool useHomeKey,
							   bool useCahce,
							   bool useAcc)
{
	impl_->showThumb = showThumb;
	impl_->showStatus = showStatus;
	impl_->useHomeKey = useHomeKey;
	impl_->useCache = useCahce;
	impl_->useAcc = useAcc;

	this->setThumbVisible(showThumb);
	this->setStatusBarVisible(showStatus);
			
	impl_->pdfReader.setUseCache(impl_->useCache);

	plutoApp->holdShellKey(this, impl_->useHomeKey);
	plutoApp->setAccOpen(useAcc);

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
PDFScreen::onAskRotate90()
{
	plutoApp->advance90degree();
	plutoApp->enterFullScreen(this);
	
	this->renderPage();

	impl_->setupHotspots(this);
	impl_->writeSettings();
}


void 
PDFScreen::onAskRotate180()
{
	plutoApp->advance180degree();
	plutoApp->enterFullScreen(this);

	impl_->writeSettings();
}


#ifdef _WIN32_WCE
#define PLUTO_PDF_M8_EXISTS_MESSAGE (WM_USER + 0x10000)

bool 
PDFScreen::winEvent(MSG *message, long *result)
{
	if (message->message == WM_ACTIVATE 
		|| message->message == PLUTO_PDF_M8_EXISTS_MESSAGE)
	{
		if (message->wParam == WA_ACTIVE 
			|| message->wParam == WA_CLICKACTIVE
			|| message->message == PLUTO_PDF_M8_EXISTS_MESSAGE)
		{
			static bool first = true;

			plutoApp->holdShellKey(this, impl_->useHomeKey);

			pltPlatform::ScreenRotateAngle realAngle = 
				plutoApp->realScreenRotateAngle();

			this->setMessage(QString("AP re-active, real ANG %1, screen %2")
				.arg(realAngle).arg(impl_->screenAngle));

			if (impl_->screenAngle != realAngle && !first && !impl_->useAcc)
			{
				//the screen rotate outside, need rotate back
				this->setMessage(QString("!!!Rotate outside, need rotate back by AP."));
				plutoApp->rotateScreen(impl_->screenAngle);
			}

			plutoApp->enterFullScreen(this);

			impl_->setupHotspots(this);

			::BringWindowToTop(this->winId());
			::SetForegroundWindow(this->winId());
			::SetActiveWindow(this->winId());

			first = false;

			*result = 0;
			return true;
		}
		else if (message->wParam == WA_INACTIVE)
		{
			plutoApp->releaseShellKey(this);
			plutoApp->leaveFullScreen(this);

			*result = 0;
			return true;
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
		else if (keyid == pltPlatform::WPARAM_KEY_EVENT_CLICK_HOME)
		{
			this->scrollDown();
		}
		else if (keyid == pltPlatform::WPARAM_KEY_EVENT_DBLCLICK_HOME)
		{
			this->showMenu();
		}
		else if (keyid == pltPlatform::WPARAM_KEY_EVENT_LONGCLICK_HOME)
		{
			plutoApp->releaseShellKey(this);
			plutoApp->rotateScreenToOriginal();
			plutoApp->leaveFullScreen(this);

			::BringWindowToTop(GetDesktopWindow());
			::SetForegroundWindow(GetDesktopWindow());
			::SetActiveWindow(GetDesktopWindow());
		}

		*result = 0;
		return true;
	}
	else if (message->message == plutoApp->getAccEventId())
	{
		pltPlatform::ScreenRotateAngle angle = 
			plutoApp->convertM8Angle((pltPlatform::M8ScreenRotateAngle)message->wParam);

		plutoApp->rotateScreen(angle);
		plutoApp->enterFullScreen(this);

		impl_->setupHotspots(this);

		this->setMessage(QString("Auto rotate : wParam : %1 -> angle : %2")
			.arg(message->wParam).arg((int)angle));
	}


	return false;
}


#endif

//end namespace
}


PDF_EXPORT int 
start_pdf_app(int argc, char* argv[])
{
#ifdef _WIN32_WCE
	pltM8Platform m8App(argc, argv);
#else
	pltPlatform m8App(argc, argv);
#endif

	//open screen
	pdf::PDFScreen screen; 
	screen.openFirstPdfBook();

	plutoApp->enterFullScreen(&screen);
	plutoApp->connect(plutoApp, SIGNAL(lastWindowClosed()), SLOT(quit()));

	int result = plutoApp->exec();

	plutoApp->leaveFullScreen(NULL);
	plutoApp->rotateScreenToOriginal();

	return result;
}


