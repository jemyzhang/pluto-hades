/*******************************************************************************
**
**  NAME:			stdafx.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Precompile header
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/06
**
**
*******************************************************************************/
#ifndef FOUNDATION_STDAFX_H
#define FOUNDATION_STDAFX_H


#ifdef _WIN32_WCE
//windows.ce

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// 注意 - 这个值与作为目标的 Windows CE OS 版本的关联性并不强
#define WINVER _WIN32_WCE

#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

#include <windows.h>
#include <commctrl.h>



// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
#pragma comment(lib, "ccrtrtti.lib")
#ifdef _X86_	
#if defined(_DEBUG)
#pragma comment(lib, "libcmtx86d.lib")
#else
#pragma comment(lib, "libcmtx86.lib")
#endif
#endif
#endif

#include <altcecrt.h>

#endif // WINCE

#ifdef _MSC_VER
#pragma warning(push, 4)

/** For QT Library */
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4127) 
#pragma warning(disable : 4512)
#pragma warning(disable : 4150)
#endif

#include <QtCore>
#include <QtGui>

#include <cfloat>
#include <typeinfo>

#include "../../../include/arch.h"
#include "../../../include/foundation_include.h"
#include "../../../include/pdf_include.h"
#include "../../../include/platform_include.h"

//Use own logger for logging service or use log4cxx library
#define FOUNDATION_USE_LOG4CXX 0
#define FOUNDATION_LOG_CONFIG_FILE "log4j.properties"

#if FOUNDATION_USE_LOG4CXX
#include "../../include/log4cxx.h"
#endif // FOUNDATION_USE_LOG4CXX

#ifdef  _MSC_VER
#pragma warning(pop)
#pragma warning(disable : 4251)


#ifdef _WIN32_WCE
#pragma warning(disable : 4275) //non-dll interface
#pragma warning(disable : 4244) //int to float
#endif 

#endif  /* _MSC_VER */

#endif
