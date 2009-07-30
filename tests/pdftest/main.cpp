#include <QtTest>

#include "../../../include/platform_include.h"
#include "pdftest.h"

int 
main(int argc, char *argv[])
{
	pltPlatform app(argc, argv);
	PDFTest tc;
	return QTest::qExec(&tc, argc, argv);
}

