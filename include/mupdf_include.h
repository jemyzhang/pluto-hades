/*******************************************************************************
**
**  NAME:			mupdf_include.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Include mupdf library
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/06
**
**
*******************************************************************************/
#ifndef mupdf_include_h__
#define mupdf_include_h__

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef _WIN32_WCE
//wince
#include "wince/mupdf/fitz.h"
#include "wince/mupdf/mupdf.h"

#else
//win32
#include "win32/mupdf/fitz.h"
#include "win32/mupdf/mupdf.h"
#endif


#ifdef  __cplusplus
}
#endif

#ifdef _WIN32_WCE
//wince
#ifdef _DEBUG
#pragma comment(lib, "mupdf-wince-vc9-debug")
#else
#pragma comment(lib, "mupdf-wince-vc9")
#endif

#else

//win32
#if _MSC_VER > 1400
//vc9
#ifdef ARCH_X86
//x86 version
#ifdef _DEBUG
#pragma comment(lib, "mupdf-win32-vc9-x86-debug")
#else
#pragma comment(lib, "mupdf-win32-vc9-x86")
#endif
#else
//general version
#ifdef _DEBUG
#pragma comment(lib, "mupdf-win32-vc9-debug")
#else
#pragma comment(lib, "mupdf-win32-vc9")
#endif
#endif
#endif


#endif


#endif // mupdf_include_h__