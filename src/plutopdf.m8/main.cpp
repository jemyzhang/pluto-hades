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
#include <string>

typedef int (*start_app_func) (int, char**);

static ImagingHelper*  
draw_splash()
{
	ImagingHelper* helper = new ImagingHelper();

	HMODULE hmodule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int i = GetModuleFileNameW(hmodule, path, MAX_PATH);

	std::wstring pathStr(path);
	size_t pos = pathStr.find_last_of(L'\\');
	std::wstring dir = pathStr.substr(0, pos + 1);
	std::wstring splashPath = dir + L"splash_pdf.png";

	helper->LoadImage(splashPath.c_str(), false, true, true);

	HDC desktopDC = ::GetDC(NULL);
	RECT rect = ::MzGetWorkArea();

	rect.left = (RECT_WIDTH(rect) - helper->GetImageWidth()) / 2;
	rect.top = (RECT_HEIGHT(rect) - helper->GetImageHeight()) / 2;
	rect.right = rect.left + helper->GetImageWidth();
	rect.bottom = rect.top + helper->GetImageHeight();

	helper->Draw(desktopDC, &rect);
	
	::ReleaseDC(NULL, desktopDC);

	return helper;
};


#define PLUTO_PDF_M8_EXISTS_MESSAGE (WM_USER + 0x10000)


static bool 
check_instance_exists()
{
	::CreateMutex(NULL, TRUE, L"pluto_pdf_m8_roger_yi");

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//Already exists
		::PostMessage(HWND_BROADCAST, PLUTO_PDF_M8_EXISTS_MESSAGE, NULL, NULL);

		return true;
	}

	return false;
}


int 
main(int argc, char* argv[])
{
	int result = 0;

	if (check_instance_exists())
		return result;

	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ImagingHelper* helper = draw_splash();

	HINSTANCE pdf_dll = NULL;
	//HINSTANCE pdf_engine_dll = NULL;
	
#ifdef _DEBUG
	//pdf_engine_dll = ::LoadLibraryW(L"pdf_wince_debug.dll");
	pdf_dll = ::LoadLibraryW(L"pdf_ui_wince_debug.dll");
#else
	pdf_dll = ::LoadLibraryW(L"pdf_ui_wince.dll");
#endif 

	start_app_func start_func = (start_app_func)
		::GetProcAddressA(pdf_dll, "start_pdf_app");

	if (start_func)
		result = start_func(argc, argv);
	else
		result = -1;

	SAFE_DELETE(helper);

	::exit(result);

	return result;
}


