/*******************************************************************************
**
**  NAME:			CacheableObject.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/05/22
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	
**
**  --------------------------------------------------------------
**
**	HISTORY:	
**
**
*******************************************************************************/
#include "stdafx.h"
#include "CacheableObject.h"
#include "Assertion.h"

namespace foundation
{

CacheableObject::CacheableObject()
	: id_(QUuid::createUuid())
	, squeezeState_(NormalState)
{
}


CacheableObject::~CacheableObject()
{
}


bool
CacheableObject::operator ==(const CacheableObject& other) const
{
	return this->id_ == other.id_;
}


bool
CacheableObject::operator !=(const CacheableObject& other) const
{
	return !(*this == other);
}


QUuid
CacheableObject::id() const
{
	return this->id_;
}


QVariant 
CacheableObject::information(InformationType infoType) const
{
	return this->information_(infoType);
}


bool 
CacheableObject::isNormal() const
{
	return this->squeezeState_ == NormalState;
}



bool
CacheableObject::isSqueezed() const
{
	return this->squeezeState_ == SqueezedState;
}


void
CacheableObject::squeeze()
{
	if (!this->isSqueezed())
	{
		this->squeeze_();
		this->squeezeState_ = SqueezedState;
	}
}


void
CacheableObject::restore()
{
	if (this->isSqueezed())
	{
		this->restore_();
		this->squeezeState_ = NormalState;
	}
}


void
CacheableObject::squeeze_()
{
}


void
CacheableObject::restore_()
{
}


QVariant 
CacheableObject::information_(InformationType /*infoType*/) const
{
	return QVariant();
}


	//end of namespace
}