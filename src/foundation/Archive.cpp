/*******************************************************************************
**
**  NAME:			Archive.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/08/20
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class Archive
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#include "stdafx.h"
#include "Archive.h"

namespace foundation
{

struct Archive::ArchiveImpl
{
	QString file;
};


Archive::Archive(void)
: impl_(new ArchiveImpl)
{
}


Archive::~Archive(void)
{
}


void 
Archive::open(const QString& file)
{
	Q_UNUSED(file);
}


}



