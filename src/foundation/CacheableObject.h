/*******************************************************************************
**
**  NAME:			CacheableObject.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/05/22
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class CacheableObject
**
**  --------------------------------------------------------------
**
**	HISTORY:	
**
**
*******************************************************************************/
#ifndef CACHEABLEOBJECT_H
#define CACHEABLEOBJECT_H
#include "foundation_global.h"

namespace foundation
{


class CacheableObject;
typedef QSharedPointer<CacheableObject>	CacheableObjectPtr;///< Shared pointer


/**
CacheableObject

An object can be managed by CacheableObjectManager, it has a QUuid as its id,
and can be squeezed to save memory.

An cacheable object may also support to query different information by caller.
*/
class FOUNDATION_EXPORT CacheableObject
{
public:
	enum State
	{
		NormalState,
		SqueezedState,
	};

	enum InformationType
	{
		File,///< path of the file linked to the object (string)
		Files,///< path of all files linked to the object (string list)
		ToolTip,///< tool tip
		Description,///< description
		ClassName,///< name of the class, e.g. CacheableObject
	};

public:
	CacheableObject();
	virtual ~CacheableObject();

public://operator
	bool operator==(const CacheableObject& other) const;
	bool operator!=(const CacheableObject& other) const;

public://query
	QUuid id() const;
	QVariant information(InformationType infoType) const;

	bool isNormal() const;
	bool isSqueezed() const;

public://command
	void squeeze();
	void restore();

private://virtual interface
	Q_DISABLE_COPY(CacheableObject)

	virtual QVariant information_(InformationType infoType) const;

	virtual void squeeze_();
	virtual void restore_();

private:
	QUuid id_;
	State squeezeState_;
};


}



#endif // CACHEABLEOBJECT_H
