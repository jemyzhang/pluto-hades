/*******************************************************************************
**
**  NAME:			Enum.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2007/09/13
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare, All Rights Reserved
** 
**	
**	PURPOSE:	
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0	2007/09/13	First create
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Enum.h"


namespace foundation
{

const Enum* const	Enum::NullEnum = NULL;

Enum::Enum(const QString& type, const QString& name)
	: type_(type)
	, name_(name)
{
	ENUM_MAP& emap = EMAP();
	if (emap.find(type) == emap.end())
	{
		//Insert a new vector for the new Enum type
		ENUM_COLLECTION newEnumType;
		emap.insert(type, newEnumType);	
	}

	ENUM_COLLECTION& enums = emap[type];
	enums.push_back(this);
	this->ordinal_ = static_cast<int>(enums.size() - 1);
}


Enum::~Enum(void)
{
}


ENUM_MAP& 
Enum::EMAP()
{
	static ENUM_MAP emap;
	return emap;
}


const ENUM_COLLECTION& 
Enum::values(const QString& type)
{
	return EMAP()[type];
}


const Enum*
Enum::valueOf(const QString& type, const QString& name)
{
	const ENUM_COLLECTION& enums = Enum::values(type);
	ENUM_COLLECTION::const_iterator iter = enums.begin();

	for (; iter != enums.end(); ++iter)
	{
		if (name == (*iter)->name())
			return *iter;
	}

	return NullEnum;
}


QString 
Enum::toString() const
{
	//[Type #1, Type ID] Name
	QString info = QString("[%1 #%2, %3] %4").arg(this->type_)
		.arg(this->ordinal_)
		.arg(typeid(*this).name())
		.arg(this->name_);

	return info;
}


QTextStream& 
Enum::print(QTextStream& out) const
{
	out<<this->toString();

	return out;
}


void 
Enum::dump(QTextStream& out)
{
	ENUM_MAP& emap = EMAP();

	foreach(const ENUM_COLLECTION& enums, emap)
	{
		foreach(const Enum* em, enums)
		{
			out<<em<<"\r\n";
		};
	}
}


void 
Enum::dump()
{
	QTextStream ts(stdout);
	dump(ts);
}


// End Namespace [9/13/2007 Roger]
}

