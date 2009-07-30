/*******************************************************************************
**
**  NAME:			pdf_include.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Include pdf module
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/06
**
**
*******************************************************************************/
#ifndef pdf_include_h__
#define pdf_include_h__

#include "../src/pdf/PDFReader.h"
#include "../src/pdf/PDFScreen.h"

typedef pdf::PDFReader			pltPDFReader;
typedef pdf::PDFException		pltPDFException;

typedef pdf::PDFScreen			pltPDFScreen;

#endif // pdf_include_h__