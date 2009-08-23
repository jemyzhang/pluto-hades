/*******************************************************************************
**
**  NAME:			PDFMenu.cpp	
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
#include "stdafx.h"
#include "PDFMenu.h"
#include "PDFreader.h"

namespace pdf {
#include "ui_PDFMenu.h"
	
struct PDFMenu::PDFMenuImpl
{
	Ui::PDFMenuClass ui;

	int currPageNo;
	QList<QCommandLinkButton*> recentWidgets;
};


PDFMenu::PDFMenu(QWidget *parent)
	: QWidget(parent)
	, impl_(new PDFMenuImpl)

{
	impl_->ui.setupUi(this);

	impl_->recentWidgets<<impl_->ui.m2Recent1<<impl_->ui.m2Recent2<<impl_->ui.m2Recent3
		<<impl_->ui.m2Recent4<<impl_->ui.m2Recent5<<impl_->ui.m2Recent6;

	//this->setAttribute(Qt::WA_TransparentForMouseEvents);
	//impl_->ui.frameBottom->setAttribute(Qt::WA_TransparentForMouseEvents);
	//impl_->ui.frameRight->setAttribute(Qt::WA_TransparentForMouseEvents);
}


PDFMenu::~PDFMenu()
{
	SAFE_DELETE(impl_);
}


void 
PDFMenu::on_m1Exit_clicked()
{
	emit askExit();
}


void 
PDFMenu::on_m1Open_clicked()
{
	emit askOpen();
}


void 
PDFMenu::on_m1Recent_clicked()
{
	this->showSubMenu(RecentFiles);
}


void 
PDFMenu::on_m1Cut_clicked()
{
	this->showSubMenu(CutMargin);
}


void 
PDFMenu::on_m1Jump_clicked()
{
	this->showSubMenu(Jump);
}


void 
PDFMenu::on_m1Zoom_clicked()
{
	this->showSubMenu(Zoom);
}


void 
PDFMenu::on_m1Settings_clicked()
{
	this->showSubMenu(Settings);
}


void 
PDFMenu::on_m1About_clicked()
{
	this->showSubMenu(About);
}


void 
PDFMenu::on_m2Help_clicked()
{
	emit askHelp();
}


void 
PDFMenu::showEvent(QShowEvent * event)
{
	QWidget::showEvent(event);
}


void 
PDFMenu::hideEvent(QHideEvent * event)
{
	impl_->ui.lePageNo->clearFocus();

	QWidget::hideEvent(event);
}




void 
PDFMenu::on_m2CutLeft3_clicked()
{
	emit askCut(CutLeft3);
}


void 
PDFMenu::on_m2CutRight3_clicked()
{
	emit askCut(CutRight3);
}


void 
PDFMenu::on_m2CutLeft5_clicked()
{
	emit askCut(CutLeft5);
}


void 
PDFMenu::on_m2CutRight5_clicked()
{
	emit askCut(CutRight5);
}


void 
PDFMenu::on_m2AutoCut_clicked()
{
	emit askCut(CutAuto);
}




void 
PDFMenu::on_m2CutReset_clicked()
{
	emit askCut(CutReset);
}


void 
PDFMenu::setCurrentPageNo(int pageNo)
{
	impl_->currPageNo = pageNo;
	impl_->ui.lePageNo->setText(QString::number(pageNo));
}


void 
PDFMenu::setRecentOpenFiles(const QStringList& files)
{
	int i = 0;

	foreach (QCommandLinkButton* button, impl_->recentWidgets)
	{
		QString file = files.value(i++);

		if (!file.isEmpty())
		{
			button->show();
			button->setText(QFileInfo(file).baseName());
			button->setDescription(QDir::toNativeSeparators(file));
		}
		else
		{
			button->hide();
		}
	}
}


void 
PDFMenu::setApplicationName(const QString& appName)
{
	impl_->ui.lbAppName->setText(appName);
}


void 
PDFMenu::setCurrentSettings(bool showThumb, 
							bool showStatus,
							bool useHomeKey,
							bool useCahce,
							bool useAcc)
{
	impl_->ui.m2ShowThumb->setChecked(showThumb);
	impl_->ui.m2ShowStatus->setChecked(showStatus);
	impl_->ui.m2UseHomeKey->setChecked(useHomeKey);
	impl_->ui.m2UseCache->setChecked(useCahce);
	impl_->ui.m2UseAcc->setChecked(useAcc);

	impl_->ui.m2Rotate90->setDisabled(useAcc);
	impl_->ui.m2Rotate180->setDisabled(useAcc);
}


void 
PDFMenu::on_m2Jump_clicked()
{
	emit askJump(impl_->ui.lePageNo->text().toUInt());
}


void 
PDFMenu::on_m2JumpNext10_clicked()
{
	emit askJump(impl_->currPageNo + 10);
}


void 
PDFMenu::on_m2JumpNext100_clicked()
{
	emit askJump(impl_->currPageNo + 100);
}


void 
PDFMenu::on_m2JumpPre10_clicked()
{
	emit askJump(impl_->currPageNo - 10);
}


void 
PDFMenu::on_m2JumpPre100_clicked()
{
	emit askJump(impl_->currPageNo - 100);
}


void 
PDFMenu::on_m2Zoom300_clicked()
{
	emit askZoom(PDFReader::FitWidth300);
}


void 
PDFMenu::on_m2Zoom250_clicked()
{
	emit askZoom(PDFReader::FitWidth250);
}


void 
PDFMenu::on_m2Zoom200_clicked()
{
	emit askZoom(PDFReader::FitWidth200);
}


void 
PDFMenu::on_m2Zoom180_clicked()
{
	emit askZoom(PDFReader::FitWidth180);
}


void 
PDFMenu::on_m2Zoom150_clicked()
{
	emit askZoom(PDFReader::FitWidth150);
}


void 
PDFMenu::on_m2Zoom120_clicked()
{
	emit askZoom(PDFReader::FitWidth120);
}


void 
PDFMenu::on_m2Zoom100_clicked()
{
	emit askZoom(PDFReader::FitWidth);
}


void 
PDFMenu::showSubMenu(int index)
{
	impl_->ui.subMenus->setCurrentIndex(index);
	impl_->ui.subMenus->show();
	impl_->ui.line2->show();
}


void 
PDFMenu::on_m2Recent1_clicked()
{
	emit askRecent(impl_->ui.m2Recent1->description());
}


void 
PDFMenu::on_m2Recent2_clicked()
{
	emit askRecent(impl_->ui.m2Recent2->description());
}


void 
PDFMenu::on_m2Recent3_clicked()
{
	emit askRecent(impl_->ui.m2Recent3->description());
}


void 
PDFMenu::on_m2Recent4_clicked()
{
	emit askRecent(impl_->ui.m2Recent4->description());
}


void 
PDFMenu::on_m2Recent5_clicked()
{
	emit askRecent(impl_->ui.m2Recent5->description());
}


void 
PDFMenu::on_m2Recent6_clicked()
{
	emit askRecent(impl_->ui.m2Recent6->description());
}


void 
PDFMenu::on_m2ShowThumb_clicked()
{
	emit askChangeSettings(impl_->ui.m2ShowThumb->isChecked(),
		impl_->ui.m2ShowStatus->isChecked(),
		impl_->ui.m2UseHomeKey->isChecked(),
		impl_->ui.m2UseCache->isChecked(),
		impl_->ui.m2UseAcc->isChecked());
}


void 
PDFMenu::on_m2ShowStatus_clicked()
{
	emit askChangeSettings(impl_->ui.m2ShowThumb->isChecked(),
		impl_->ui.m2ShowStatus->isChecked(),
		impl_->ui.m2UseHomeKey->isChecked(),
		impl_->ui.m2UseCache->isChecked(),
		impl_->ui.m2UseAcc->isChecked());
}


void 
PDFMenu::on_m2UseHomeKey_clicked()
{
	emit askChangeSettings(impl_->ui.m2ShowThumb->isChecked(),
		impl_->ui.m2ShowStatus->isChecked(),
		impl_->ui.m2UseHomeKey->isChecked(),
		impl_->ui.m2UseCache->isChecked(),
		impl_->ui.m2UseAcc->isChecked());
}


void 
PDFMenu::on_m2UseCache_clicked()
{
	emit askChangeSettings(impl_->ui.m2ShowThumb->isChecked(),
		impl_->ui.m2ShowStatus->isChecked(),
		impl_->ui.m2UseHomeKey->isChecked(),
		impl_->ui.m2UseCache->isChecked(),
		impl_->ui.m2UseAcc->isChecked());
}


void 
PDFMenu::on_m2UseAcc_clicked()
{
	emit askChangeSettings(impl_->ui.m2ShowThumb->isChecked(),
		impl_->ui.m2ShowStatus->isChecked(),
		impl_->ui.m2UseHomeKey->isChecked(),
		impl_->ui.m2UseCache->isChecked(),
		impl_->ui.m2UseAcc->isChecked());

	impl_->ui.m2Rotate90->setDisabled(impl_->ui.m2UseAcc->isChecked());
	impl_->ui.m2Rotate180->setDisabled(impl_->ui.m2UseAcc->isChecked());
}


void 
PDFMenu::on_m2Style_clicked()
{
	QString styleFile = plutoApp->getOpenFileName(this, 
		plutoApp->applicationName(),
		plutoApp->pathRelateToAppDir("style"),
		"*.qss;");

	if (!styleFile.isEmpty())
	{
		emit askChangeStyle(styleFile);
	}
}


void 
PDFMenu::on_m2Rotate90_clicked()
{
	emit askRotate90();
}


void 
PDFMenu::on_m2Rotate180_clicked()
{
	emit askRotate180();
}


}
