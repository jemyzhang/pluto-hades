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
#ifndef THUMBABLEOBJECT_H
#define THUMBABLEOBJECT_H

#include "foundation_global.h"
#include "CacheableObject.h"


namespace foundation
{

class ThumbableObject;
typedef QSharedPointer<ThumbableObject>	ThumbableObjectPtr;


class ThumbDecorator;



/**
ThumbableObject

ThumbableObjecy inherit to CacheableObject, and it can provide an thumbnail
for display.

The generated thumbnail can be cached inside when the request size less than
640x480, and object can direct return the cached thumbnail 
when the next request size match the size of cached thumbnail.

Also the object can be marked as dirty, 
when the object is dirty, usually the previous generated thumbnail is out of date.
*/
class FOUNDATION_EXPORT ThumbableObject : public CacheableObject 
{
public:
	ThumbableObject(bool cacheThumb = true);
	~ThumbableObject();

public://query
	QPixmap thumbnail(const QSize& size, 
		Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) const;

	bool isDirty() const;

public:
	void markDirty();

public://query
	virtual QPixmap thumbnail_(const QSize& size,
		Qt::AspectRatioMode mode) const = 0;

public: 
	static void addDecorator(const QString& className,
		ThumbDecorator* decorator);

private:
	bool matchCahcedThumb(const QSize& size, Qt::AspectRatioMode mode) const;
	void setCachedThumb(const QPixmap& thumb) const;

	bool cacheThumb_;

	mutable bool dirty_;
	mutable QPixmap cachedThumb_;

private:
	static QMap<QString, ThumbDecorator*> decorators_;
};


class FOUNDATION_EXPORT ThumbDecorator
{
public:
	virtual QPixmap decorate(const QPixmap& thumb, 
		QSize size, 
		const ThumbableObject* object) const = 0;
};

//end of namespace foundation
}


#endif // THUMBABLEOBJECT_H
