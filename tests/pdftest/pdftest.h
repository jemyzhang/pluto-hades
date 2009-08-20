#ifndef PDFTEST_H
#define PDFTEST_H

#include <QObject>

class PDFTest : public QObject
{
	Q_OBJECT

public:
	PDFTest(QObject *parent = NULL);
	virtual ~PDFTest();

private slots:
	void testOpen();
	void testOpen2();
	void testOpen3();

	void testRender();
	void testRender2();
	void testRender3();
	void testRender4();

	void testCompress();
	void testCompress2();
	void testCompress3();

	void testUnCompress();
	void testUnCompress2();

	void testBackground();

	void testImageProcessor();

	void testScreen();
};
#endif // PDFTEST_H
