/*******************************************************************************
**
**  NAME:			Enum.h	
**	VER: 			1.0
**  CREATE DATE: 	2007/09/13
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Provide a Java-like Enum	
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0	2007/09/13	First create
**
**
*******************************************************************************/
#ifndef FOUNDATION_ENUM_H
#define FOUNDATION_ENUM_H
#include "foundation_global.h"

namespace foundation
{

class Enum;

typedef QVector<const Enum*>			ENUM_COLLECTION;/** Enum vector. */
typedef QMap<QString, ENUM_COLLECTION>	ENUM_MAP;/** Enum map. */

/** Simulate JAVA's Enum type, 
	provide a Enumeration system for C++. */
class FOUNDATION_EXPORT Enum
{
public:
	static const Enum* const			NullEnum;/** Null Enum. */

public:
	/** Dump all Enum information out, only for debug. */
	static void dump();
	static void dump(QTextStream& out);
	/** Get the information of the Enum item.*/
	const QString& type() const {return this->type_;}
	const QString& name() const {return this->name_;}
	const int ordinal() const {return this->ordinal_;}

	virtual ~Enum(void);
	/** Print out the status. */
	virtual QString toString() const;
	QTextStream& print(QTextStream& out) const;

protected:
	//Constructor
	Enum(const QString&, const QString&);
	/** A safe storage for store all enumerate items. */
	static ENUM_MAP& EMAP();
	/** Get all Enum items of the given Enum type. */
	static const ENUM_COLLECTION& values(const QString&);
	/** Get the Enum items by it's type and name. */
	static const Enum* valueOf(const QString&, const QString&);

private:
	Enum(const Enum&);//non-copyable
	const Enum& operator=(const Enum&);

	QString		type_;
	QString		name_;
	int			ordinal_;
};

/** Stream output. */
inline QTextStream&
operator<<(QTextStream& out, const Enum* e)
{
	return e->print(out);
}

	/** Declare a new Enum type inside the definition of enum class. 
		The macro will generate a constructor, function - values,
		and function - valueOf. */
#define DECLARE_ENUM(ENUM_TYPE)	\
public:\
	ENUM_TYPE(const QString& name): Enum(#ENUM_TYPE, name) {}\
	static QVector<const ENUM_TYPE *> values(){\
		const QVector<const Enum *>& enums = Enum::values(#ENUM_TYPE);\
		QVector<const Enum *>::const_iterator iter = enums.begin();\
		QVector<const ENUM_TYPE *> ret;\
		for(; iter != enums.end(); ++iter ){\
			const ENUM_TYPE * item = dynamic_cast<const ENUM_TYPE *>(*iter);\
			if (item) ret.push_back(item);\
		}\
		return ret;\
	}\
	static const ENUM_TYPE* valueOf(const QString& name){\
		return  dynamic_cast<const ENUM_TYPE *>(Enum::valueOf(#ENUM_TYPE, name));\
	}

	/** Declare a new Enum item inside the definition of enum item class,
		need the enum item' class name and enum type's class name as input.
		The macro will generate a constructor. */
#define DECLARE_ENUM_ITEM(ENUM_ITEM_TYPE, ENUM_TYPE)\
public:\
	ENUM_ITEM_TYPE(const QString& name) : ENUM_TYPE(name) {}

}

#endif
