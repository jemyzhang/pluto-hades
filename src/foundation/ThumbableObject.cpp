/*******************************************************************************
**
**  NAME:			ThumbableObject.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/06/10
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class ThumbableObject, ThumbDecorator
**
**  --------------------------------------------------------------
**
**	HISTORY:	
**
**
*******************************************************************************/
#include "stdafx.h"
#include "ThumbableObject.h"

#include "Assertion.h"

namespace foundation
{


QMap<QString, ThumbDecorator*> ThumbableObject::decorators_;


ThumbableObject::ThumbableObject(bool cacheThumb /*= false*/)
								 : CacheableObject()
								 , cacheThumb_ (cacheThumb)
								 , dirty_ (true)
{

}

ThumbableObject::~ThumbableObject()
{

}


QPixmap 
ThumbableObject::thumbnail(const QSize& size,
						   Qt::AspectRatioMode mode /*= Qt::IgnoreAspectRatio*/) const
{
	QPixmap thumb;

	if (this->dirty_ || this->cachedThumb_.isNull())
	{
		//no cache or dirty
		thumb = this->thumbnail_(size, mode);
		this->setCachedThumb(thumb);
	}
	else
	{
		//has cache, check whether matched first
		bool match = this->matchCahcedThumb(size, mode);

		if (match)
		{
			thumb = this->cachedThumb_;
		}
		else
		{
			thumb = this->thumbnail_(size, mode);
			this->setCachedThumb(thumb);
		}
	}

	ThumbDecorator* decorator = ThumbableObject::decorators_.value(
		this->information(CacheableObject::ClassName).toString());

	if (decorator)
	{
		thumb = decorator->decorate(thumb, size, this);
	}

	return thumb;
}


bool 
ThumbableObject::isDirty() const
{
	return this->dirty_;
}

void 
ThumbableObject::markDirty()
{
	this->dirty_ = true;
}


void 
ThumbableObject::setCachedThumb(const QPixmap& thumb) const
{
	static const QSize acceptSize(640, 480);

	if (thumb.width() < acceptSize.width() &&
		thumb.height() < acceptSize.height())
	{
		this->cachedThumb_ = thumb;
		this->dirty_ = false;
	}
}


bool 
ThumbableObject::matchCahcedThumb(const QSize& size,
								  Qt::AspectRatioMode mode) const
{
	QSize cachedSize = this->cachedThumb_.size();

	if (size == cachedSize)
	{
		return true;
	}
	else if (size.width() == cachedSize.width() ||
		size.height() == cachedSize.height())
	{
		QSize needSize = cachedSize;
		needSize.scale(size, mode);

		return needSize  == cachedSize;
	}
	else
	{
		return false;
	}
}


void 
ThumbableObject::addDecorator(const QString& className,
							  ThumbDecorator* decorator)
{
	__PRECOND_E(decorator, "Null decorator.");

	decorators_.insert(className, decorator);
}


}

