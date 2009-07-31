/*******************************************************************************
**
**  NAME:			Exception.h	
**	VER: 			1.1
**  CREATE DATE: 	06/03/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Exception, provide more debug information than std::exception
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.1, 10/12/2007,	Add history tracing ability.
**				v1.2, 05/15/2009,	Add into Pluto 2, fixed null msg error
**
**
*******************************************************************************/
#pragma once
#ifndef FOUNDATION_EXCEPTION_H
#define FOUNDATION_EXCEPTION_H
#include "foundation_global.h"
#include "Logger.h"

namespace foundation
{

/*******************************************************************************
** 	                                                                
**	Class Exception inherit from std::exception,
**	provide file name, function name and line number, also history traceable                                                             
**                                                             
*******************************************************************************/
class FOUNDATION_EXPORT Exception : public std::exception
{
public:
	Exception(const QString& msg, 
		const QString& file, 
		const QString& func, 
		int ln) throw();

	Exception(const QString& msg, 
		const QVariant& param,
		const QString& file, 
		const QString& func, 
		int ln) throw();


	Exception(const Exception& prev, 
		const QString& msg,
		const QString& file,
		const QString& func, 
		int ln) throw();

	Exception(const std::exception& e) throw();

	Exception() throw();

	virtual ~Exception() throw();

	QString message() const;///< error message
	QVariant parameter() const;///< parameter

	virtual QString toString() const;///< detail information
	QTextStream& print(QTextStream& out) const;///< print to stream

protected:
	QString		history_;//< History tracing information

	QString		msg_;///< Message
	QString		file_;///< File
	QString		function_;///< Function
	int			line_;///< Line number

	QVariant	param_;///< Additional parameter
};

/** Stream output. */
inline QTextStream&  
operator<<(QTextStream& out, const Exception& ke)
{
	return ke.print(out);
}


/*******************************************************************************
** 	                                                                
**	Helper Macro Declare an New Exception as child/grandchild of Exception
**
**	DECLARE_EXCEPTION,			add Constructor for child of Exception
**	DECLARE_CHILD_EXCEPTION,	add Constructor for child of provide Exception Type, 
**								may be grandchild of Exception
**
*******************************************************************************/
#define DECLARE_EXCEPTION(EXCEPTION_TYPE) \
public:\
	EXCEPTION_TYPE(const QString& msg, const QString& file, const QString& func, int ln)\
	: Exception(msg, file, func, ln) {}\
	EXCEPTION_TYPE(const QString& msg, const QVariant& param, const QString& file, const QString& func, int ln)\
	: Exception(msg, param, file, func, ln) {}\
	EXCEPTION_TYPE(const foundation::Exception& prev, const QString& msg, const QString& file, const QString& func, int ln)\
	: Exception(prev, msg, file, func, ln) {}\
	EXCEPTION_TYPE()\
	: Exception() {}\


/** A Macro to add suitable constructor in Exception's
* 	derived grandchild (child of child). */
#define DECLARE_CHILD_EXCEPTION(EXCEPTION_TYPE, PARENT_EXCEPTION_TYPE) \
public:\
	EXCEPTION_TYPE(const QString& msg, const QString& file, const QString& func, int ln)\
	: PARENT_EXCEPTION_TYPE(msg, file, func, ln) {}\
	EXCEPTION_TYPE(const QString& msg, const QVariant& param, const QString& file, const QString& func, int ln)\
	: PARENT_EXCEPTION_TYPE(msg, param, file, func, ln) {}\
	EXCEPTION_TYPE(const foundation::Exception& prev, const QString& msg, const QString& file, const QString& func, int ln)\
	: PARENT_EXCEPTION_TYPE(prev, msg, file, func, ln) {}\
	EXCEPTION_TYPE()\
	: PARENT_EXCEPTION_TYPE() {}\

/*******************************************************************************
** 	                                                                
**	Helper Macro To Throw Exception
**
**	__THROW,		throw exception with exception type
**	__THROW_L,		throw exception with exception type and log
**	__RETHROW,		re-throw exception with new type
**	__RETHROW_L,	re-throw exception with new type and log
**
*******************************************************************************/
#define __THROW(EXCEPTION_TYPE, msg) \
	throw EXCEPTION_TYPE(msg, __FILE__, __FUNCTION__, __LINE__)

#define __THROW_L(EXCEPTION_TYPE, msg) \
	{\
		__LOG_E(msg);\
		throw EXCEPTION_TYPE(msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __THROW_P(EXCEPTION_TYPE, msg, param) \
	throw EXCEPTION_TYPE(msg, param, __FILE__, __FUNCTION__, __LINE__)

#define __THROW_PL(EXCEPTION_TYPE, msg, param) \
	{\
	__LOG_E(msg);\
	throw EXCEPTION_TYPE(msg, param, __FILE__, __FUNCTION__, __LINE__);\
}

#define __RETHROW(NEW_EXCEPTION_TYPE, e, msg) \
	throw NEW_EXCEPTION_TYPE(e, msg, __FILE__, __FUNCTION__, __LINE__)

#define __RETHROW_L(NEW_EXCEPTION_TYPE, e, msg) \
	{\
		__LOG_E(e.what());\
		__LOG_E(msg);\
		throw NEW_EXCEPTION_TYPE(e, msg, __FILE__, __FUNCTION__, __LINE__);\
	}


/************************************************************************/
/*
/*	IOException <-	FileReadException
/*				<-	FileWriteException
/*
/************************************************************************/

class FOUNDATION_EXPORT FileIOException : public Exception
{
	DECLARE_EXCEPTION(FileIOException)
};


class FOUNDATION_EXPORT FileReadException : public FileIOException
{
	DECLARE_CHILD_EXCEPTION(FileReadException, FileIOException)
};


class FOUNDATION_EXPORT FileWriteException : public FileIOException
{
	DECLARE_CHILD_EXCEPTION(FileWriteException, FileIOException)
};
#endif

//End namespace foundation
}
