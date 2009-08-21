#include <QtCore>
#include <QtTest>
#include <QImage>
#include <QByteArray>
#include <QDataStream>

#include <QDialog>
#include <QPushButton>
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
	//QFile file("D:\\Noname1.txt");
	//file.open(QFile::ReadOnly);

	//QTextStream ts(&file);
	//ts.setCodec("UTF-16");
	//QString line = ts.readLine();

	//qDebug()<<line;

	QDialog dialog;

	QHBoxLayout* layout = new QHBoxLayout(&dialog);

	pltScreen* screen = new pltPDFScreen(&dialog);

	layout->addWidget(screen);
	layout->setSpacing(0);
	layout->setMargin(0);

	//screen->setMessage(line);

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


void 
PDFTest::testCompress()
{
	QImage image("test/input/oo-pdf-p1.bmp");
	QImage image2("test/input/heavy-test-pdf-p2.jpg");
	QImage image3("test/input/test2-pdf-p145.jpg");

	QBENCHMARK
	{
		QByteArray ba;
		QDataStream ds(&ba, QIODevice::WriteOnly);
		ds<<image;

		__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
			.arg(image.numBytes()).arg(ba.size()).arg(ba.size() * 1.0 / image.numBytes()));

		QByteArray ba2;
		QDataStream ds2(&ba2, QIODevice::WriteOnly);
		ds2<<image2;

		__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
			.arg(image2.numBytes()).arg(ba2.size()).arg(ba2.size() * 1.0 / image2.numBytes()));

		QByteArray ba3;
		QDataStream ds3(&ba3, QIODevice::WriteOnly);
		ds3<<image3;

		__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
			.arg(image3.numBytes()).arg(ba3.size()).arg(ba3.size() * 1.0 / image3.numBytes()));
	}
}


void 
PDFTest::testCompress2()
{
	//QImage image("test/input/oo-pdf-p1.bmp");
	QImage image2("test/input/heavy-test-pdf-p2.bmp");
	//QImage image3("test/input/test2-pdf-p145.jpg");

	QBENCHMARK
	{
		//QByteArray ba = qCompress(image.bits(), image.numBytes());

		//__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
		//	.arg(image.numBytes()).arg(ba.size()).arg(ba.size() * 1.0 / image.numBytes()));

		QByteArray ba2 = qCompress(image2.bits(), image2.numBytes(), 1);

		__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
			.arg(image2.numBytes()).arg(ba2.size()).arg(ba2.size() * 1.0 / image2.numBytes()));

		//QByteArray ba3 = qCompress(image3.bits(), image3.numBytes());

		//__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
		//	.arg(image3.numBytes()).arg(ba3.size()).arg(ba3.size() * 1.0 / image3.numBytes()));
	}
}


void 
PDFTest::testCompress3()
{
	//QImage image("test/input/oo-pdf-p1.bmp");
	QImage image2("test/input/heavy-test-pdf-p2.bmp");
	//QImage image3("test/input/test2-pdf-p145.jpg");

	QBENCHMARK
	{
		//QByteArray ba = foundation::compress(image.bits(), image.numBytes());

		//__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
		//	.arg(image.numBytes()).arg(ba.size()).arg(ba.size() * 1.0 / image.numBytes()));

		QByteArray ba2 = foundation::compress(image2.bits(), image2.numBytes());

		__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
			.arg(image2.numBytes()).arg(ba2.size()).arg(ba2.size() * 1.0 / image2.numBytes()));

		//QByteArray ba3 = foundation::compress(image3.bits(), image3.numBytes());

		//__LOG(QString("Original size %1, compressed size %2, compressed ratio %3")
		//	.arg(image3.numBytes()).arg(ba3.size()).arg(ba3.size() * 1.0 / image3.numBytes()));
	}
}


void 
PDFTest::testUnCompress()
{
	QImage image("test/input/oo-pdf-p1.bmp");
	QImage image2("test/input/heavy-test-pdf-p1.jpg");
	QImage image3("test/input/test2-pdf-p145.jpg");

	QByteArray ba;
	QDataStream ds(&ba, QIODevice::ReadWrite);
	ds<<image;

	QByteArray ba2;
	QDataStream ds2(&ba2, QIODevice::ReadWrite);
	ds2<<image2;

	QByteArray ba3;
	QDataStream ds3(&ba3, QIODevice::ReadWrite);
	ds3<<image3;

	QBENCHMARK
	{
		QDataStream ds(&ba, QIODevice::ReadWrite);
		QDataStream ds2(&ba2, QIODevice::ReadWrite);
		QDataStream ds3(&ba3, QIODevice::ReadWrite);

		ds>>image;
		ds2>>image2;
		ds3>>image3;
	}

	image3.save("test/output/test2-pdf-p145.jpg");
}


void 
PDFTest::testUnCompress2()
{
	QImage image("test/input/oo-pdf-p1.bmp");
	QImage image2("test/input/heavy-test-pdf-p1.jpg");
	QImage image3("test/input/test2-pdf-p145.jpg");

	QByteArray ba = qCompress(image.bits(), image.numBytes());
	QByteArray ba2 = qCompress(image2.bits(), image2.numBytes());
	QByteArray ba3 = qCompress(image3.bits(), image3.numBytes());


	QBENCHMARK
	{
		QByteArray ba4 = qUncompress(ba);
		QImage img(image.size(), image.format());
		qMemCopy(img.bits(), ba4.data(), ba4.size());

		QByteArray ba5 = qUncompress(ba2);
		QImage img2(image2.size(), image2.format());
		qMemCopy(img2.bits(), ba5.data(), ba5.size());

		QByteArray ba6 = qUncompress(ba3);
		QImage img3(image3.size(), image3.format());
		qMemCopy(img3.bits(), ba6.data(), ba6.size());
	}
}

