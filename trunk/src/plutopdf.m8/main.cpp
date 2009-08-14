/*******************************************************************************
**
**  NAME:			main.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/17
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	main function
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/17
**
**
*******************************************************************************/
#include "../../include/arch.h"
#include "../../include/m8_include.h"

#include <Windows.h>

typedef int (*start_app_func) (int, char**);

static ImagingHelper*  
draw_splash()
{
	ImagingHelper* helper = new ImagingHelper();

	HMODULE hmodule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int i = GetModuleFileNameW(hmodule, path, MAX_PATH);

	CMzStringW pathStr(path);
	int pos = pathStr.FindReverse(L'\\');
	CMzStringW dir = pathStr.SubStr(0, pos + 1);
	CMzStringW splashPath = dir + L"splash.png";

	helper->LoadImage(splashPath, false, true, true);

	HDC desktopDC = ::GetDC(NULL);
	RECT rect = ::MzGetWorkArea();

	rect.left = (RECT_WIDTH(rect) - helper->GetImageWidth()) / 2;
	rect.top = (RECT_HEIGHT(rect) - helper->GetImageHeight()) / 2;
	rect.right = rect.left + helper->GetImageWidth();
	rect.bottom = rect.top + helper->GetImageHeight();

	helper->Draw(desktopDC, &rect);

	::DrawText(desktopDC, 
		L"PlutoPDF for M8 v0.1.5.0 \r\nRoger (roger2yi@gmail.com)", 
		-1,
		&rect,
		DT_LEFT | DT_VCENTER);
	
	::ReleaseDC(NULL, desktopDC);

	return helper;
};


int 
main(int argc, char* argv[])
{
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ImagingHelper* helper = draw_splash();

	HINSTANCE pdf_dll = NULL;
	
#ifdef _DEBUG
	pdf_dll = ::LoadLibraryW(L"pdf_wince_debug.dll");
#else
	pdf_dll = ::LoadLibraryW(L"pdf_wince.dll");
#endif 

	start_app_func start_func = (start_app_func)
		::GetProcAddressA(pdf_dll, "start_pdf_app");

	int result = 0;

	if (start_func)
		result = start_func(argc, argv);
	else
		result = -1;

	SAFE_DELETE(helper);

	return result;
}


