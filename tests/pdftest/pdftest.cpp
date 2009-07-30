#include <QtCore>
#include <QtTest>
#include <QImage>

#include <QDialog>
#include <QHBoxLayout>

#include "pdftest.h"

#include "../../include/foundation_include.h"
#include "../../include/pdf_include.h"
#include "../../include/platform_include.h"

PDFTest::PDFTest(QObject *parent)
	: QObject(parent)
{
}


PDFTest::~PDFTest()
{

}


void 
PDFTest::testOpen()
{
	QBENCHMARK
	{
		pltPDFReader reader;
		reader.open("test/input/test.pdf");
	}
}


void 
PDFTest::testOpen2()
{
	QBENCHMARK
	{
		pltPDFReader reader;
		reader.open("test/input/oo.pdf");
	}
}


void 
PDFTest::testOpen3()
{
	QBENCHMARK
	{
		pltPDFReader reader(1024 * 1024);
		reader.open("test/input/oo.pdf");
	}
}

void 
PDFTest::testRender()
{
	pltPDFReader reader;
	reader.open("test/input/oo.pdf");

	int pageNo = 0;

	QBENCHMARK
	{
		QImage page = reader.renderFitWidth(pageNo++, 720);

		page.save(QString("test/output/oo-pdf-p%1.bmp").arg(pageNo));
		page.save(QString("test/output/oo-pdf-p%1.jpg").arg(pageNo), "jpg", 95);
	}
}


void 
PDFTest::testRender2()
{
	pltPDFReader reader;
	reader.open("test/input/heavy-test.pdf");

	int pageNo = 0;

	QBENCHMARK
	{
		QImage page = reader.renderFitWidth(pageNo++, 720);
		QImage page2 = reader.renderFitWidth(pageNo++, 720, pltPDFReader::FitWidth150);

		page2.save(QString("test/output/heavy-test-pdf-p%1.bmp").arg(pageNo));
	}
}


void 
PDFTest::testRender3()
{
	pltPDFReader reader(1024 * 1024);
	reader.open("test/input/heavy-test.pdf");

	int pageNo = 0;

	QBENCHMARK
	{
		QImage page = reader.renderFitWidth(pageNo++, 720);

		page.save(QString("test/output/heavy-test-pdf-p%1.jpg").arg(pageNo), "jpg", 90);
	}
}


void 
PDFTest::testRender4()
{
	pltPDFReader reader(1024 * 1024);
	reader.open("test/input/test2.pdf");

	for (int pageNo = 0; pageNo < reader.pageCount(); ++pageNo)
	{
		QImage page = reader.renderFitWidth(pageNo++, 480);
		page.save(QString("test/output/test2-pdf-p%1.jpg").arg(pageNo), "jpg", 90);
	}
}


void 
PDFTest::testBackground()
{
	pltPDFReader reader;
	reader.open("test/input/oo.pdf");
	reader.setBackground(QColor("yellow"));

	int pageNo = 0;

	QImage page = reader.renderFitWidth(pageNo++, 720);

	page.save(QString("test/output/oo-pdf-p%1.bmp").arg(pageNo));
}


void 
PDFTest::testScreen()
{
	QDialog dialog;

	QHBoxLayout* layout = new QHBoxLayout(&dialog);

	pltScreen* screen = new pltPDFScreen(&dialog);

	layout->addWidget(screen);
	layout->setSpacing(0);
	layout->setMargin(0);

	dialog.resize(720, 480);
	dialog.exec();
}


void 
PDFTest::testImageProcessor()
{
	QImage image("test/input/oo-pdf-p1.bmp");

	int leftMargin = pltImageProcessor::margin(image, pltImageProcessor::Left);
	double leftRatio = pltImageProcessor::marginRatio(image, pltImageProcessor::Left);

	__LOG(QString("oo-pdf-p1.bmp, left margin : %1, left margin ratio : %2")
		.arg(leftMargin).arg(leftRatio));
}

