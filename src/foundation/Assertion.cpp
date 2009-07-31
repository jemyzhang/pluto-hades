/*******************************************************************************
**
**  NAME:			Assertion.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2007/10/18
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**------------------------------------------------------------------------------	
**	PURPOSE:	
**	NOTE:		
**
**------------------------------------------------------------------------------
**	HISTORY:	v1.0	2007/10/18	First create
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Assertion.h"

namespace foundation
{


/*******************************************************************************
** 	                                                                
** Exception                                                               
**                                                             
*******************************************************************************/
AssertException::AssertException(const QString& condition,
	const QString& msg, 
	const QString& file, 
	const QString& func,
	int ln) throw()
	: Exception(msg, file, func, ln)
	, condition_(condition)
{
}


QString 
AssertException::toString() const
{
	QString detail = QString("(%1, %2) %3 [%4 - %5 (%6)]")
		.arg(typeid(*this).name())
		.arg(this->condition_, this->msg_, this->file_, this->function_)
		.arg(this->line_);

	return detail;
}

/*******************************************************************************
** 	                                                                
** Assertion                                                               
**                                                             
*******************************************************************************/
//Default is internal level in debug mode, middle level in release mode
#ifdef _DEBUG
Assertion::AssertionLevel Assertion::throwLevel_ = Assertion::ASSERT_IN_INTERNAL_LEVEL;
#else
Assertion::AssertionLevel Assertion::throwLevel_ = Assertion::ASSERT_IN_MIDDLE_LEVEL;
#endif


/** setAssertionThrowLevel */
void 
Assertion::setAssertionThrowLevel(AssertionLevel throwLevel)
{
	Assertion::throwLevel_ = throwLevel;
}


/** assertFail */
void 
Assertion::assertFail(AssertionLevel level,
					  const QString& cond, 
					  const QString& msg,
					  const QString& file, 
					  const QString& func,
					  int ln)
{
	Assertion::baseAssertFail_<AssertException>(level, cond, msg, file, func, ln);
}


/** precondFail */
void 
Assertion::precondFail(AssertionLevel level,
					   const QString& cond, 
					   const QString& msg,
					   const QString& file, 
					   const QString& func,
					   int ln)
{
	Assertion::baseAssertFail_<PreconditionException>(level, cond, msg, file, func, ln);
}


/** postcondFail */
void 
Assertion::postcondFail(AssertionLevel level,
						const QString& cond, 
						const QString& msg,
						const QString& file, 
						const QString& func,
						int ln)
{
	Assertion::baseAssertFail_<PostconditionException>(level, cond, msg, file, func, ln);
}


/** invariantFail */
void 
Assertion::invariantFail(AssertionLevel level,
						 const QString& cond, 
						 const QString& msg,
						 const QString& file, 
						 const QString& func,
						 int ln)
{
	Assertion::baseAssertFail_<InvariantException>(level, cond, msg, file, func, ln);
}


/** impossibleFail */
void 
Assertion::impossibleFail(AssertionLevel level,
						  const QString& cond, 
						  const QString& msg,
						  const QString& file, 
						  const QString& func,
						  int ln)
{
	Assertion::baseAssertFail_<ImpossibleException>(level, cond, msg, file, func, ln);
}

// End Namespace [10/18/2007 Roger]
}
