/*******************************************************************************
**
**  NAME:			platform_include.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/11
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Include platform library
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/11
**
**
*******************************************************************************/
#ifndef platform_include_h__
#define platform_include_h__

#include "../src/platform/Platform.h"
#include "../src/platform/Screen.h"


typedef platform::Platform			pltPlatform;
typedef platform::M8Platform		pltM8Platform;

typedef platform::Screen			pltScreen;
typedef platform::SipLineEdit		pltSipLineEdit;
#endif // platform_include_h__