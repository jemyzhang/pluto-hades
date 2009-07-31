/*******************************************************************************
**
**  NAME:			Assertion.h	
**	VER: 			1.0
**  CREATE DATE: 	2007/10/18
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**------------------------------------------------------------------------------	
**	PURPOSE:	Assertion, Pre-condition, Post-condition, Invariant support
**	NOTE:		
**
**------------------------------------------------------------------------------
**	HISTORY:	v1.0	2007/10/18	First create
**				v1.1	2009/05/15	Add into Pluto 2
**
**
*******************************************************************************/
#ifndef FOUNDATION_ASSERTION_H
#define FOUNDATION_ASSERTION_H
#include "foundation_global.h"

#include "Exception.h"
#include "Logger.h"

namespace foundation
{
/*******************************************************************************
** 	                                                                
**	Helper Macro Declare an New AssertException as child/grandchild of AssertException
**
**	DECLARE_ASSERT_EXCEPTION,		add Constructor for child of AssertException
**	DECLARE_CHILD_ASSERT_EXCEPTION,	add Constructor for child of provide Exception Type, 
**									may be grandchild of AssertException
**
*******************************************************************************/
#define DECLARE_ASSERT_EXCEPTION(ASSERT_EXCEPTION_TYPE) \
public:\
	ASSERT_EXCEPTION_TYPE(const QString& cond, const QString& msg, const QString& file, const QString& func, int ln)\
	: AssertException(cond, msg, file, func, ln) {}\


	/** A Macro to add suitable constructor in Exception's
	* 	derived grandchild (child of child). */
#define DECLARE_CHILD_ASSERT_EXCEPTION(ASSERT_EXCEPTION_TYPE, PARENT_ASSERT_EXCEPTION_TYPE) \
public:\
	ASSERT_EXCEPTION_TYPE(const QString& cond, const QString& msg, const QString& file, const QString& func, int ln)\
	: PARENT_ASSERT_EXCEPTION_TYPE(cond, msg, file, func, ln) {}\

/*******************************************************************************
** 	                                 
**											<- PreconditionException
**	AssertException <- ContractException	<- PostconditionException
**											<- InvariantException
**					<- ImpossibleException	
**                                                             
*******************************************************************************/
class FOUNDATION_EXPORT AssertException : public Exception
{
public:
	AssertException(const QString& condition,
					const QString& msg, 
					const QString& file, 
					const QString& func,
					int ln) throw();

	virtual QString toString() const;///< detail information

private:
	QString	condition_;//Assert Condition
};

class FOUNDATION_EXPORT ContractException : public AssertException
{
	DECLARE_ASSERT_EXCEPTION(ContractException)
};

class FOUNDATION_EXPORT PreconditionException : public ContractException
{
	DECLARE_CHILD_ASSERT_EXCEPTION(PreconditionException, ContractException)
};

class FOUNDATION_EXPORT PostconditionException : public ContractException
{
	DECLARE_CHILD_ASSERT_EXCEPTION(PostconditionException, ContractException)
};

class FOUNDATION_EXPORT InvariantException : public ContractException
{
	DECLARE_CHILD_ASSERT_EXCEPTION(InvariantException, ContractException)
};

class FOUNDATION_EXPORT ImpossibleException : public AssertException
{
	DECLARE_ASSERT_EXCEPTION(ImpossibleException)
};

/*******************************************************************************
** 	                                                                
** Class Assertion                                                               
**                                                             
*******************************************************************************/
class FOUNDATION_EXPORT Assertion
{
public:
	enum AssertionLevel
	{
		ASSERT_ENABLE,				//Use to enable all assertion
		ASSERT_IN_INTERNAL_LEVEL,	//Use in sub-system internal
		ASSERT_IN_MIDDLE_LEVEL,		//Use in middle
		ASSERT_IN_EXTERNAL_LEVEL,	//Use in sub-system's boundary
		ASSERT_DISABLE,				//Use to disable all assertion
	};

/*******************************************************************************
** 	                                                                
**	Set the assertion throw level, when the assertion fail, 
**	it will throw exception when its assertion level above or equal
**	the throw level.
**                                                             
*******************************************************************************/
	static void setAssertionThrowLevel(AssertionLevel throwLevel);


/*******************************************************************************
** 	                                                                
**	The Assertion Failed...
**
**	assertFail,		the assertion failed
**	precondFail,	the pre condition not fulfill
**	postcondFail,	the post condition not fulfill
**	invariantFail,	the invariant broken
**	impossibleFail,	the impossible turn true
**                                                             
*******************************************************************************/
	static void assertFail(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln);

	static void precondFail(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln);

	static void postcondFail(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln);

	static void invariantFail(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln);

	static void impossibleFail(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln);

private:
	static AssertionLevel	throwLevel_;//Throw Level	

	template<typename E> 
	inline static void baseAssertFail_(AssertionLevel level,
		const QString& cond, 
		const QString& msg,
		const QString& file, 
		const QString& func,
		int ln)
	{
		E e(cond, msg, file, func, ln);
		__LOG_A(e.toString());

		if (level != ASSERT_DISABLE && level >= throwLevel_)
			throw e;
	}
};

/*******************************************************************************
** 	                                                                
**	ASSERT Family Macro 
**
**	__ASSERT,		Assert, internal level
**	__ASSERT_E,		Assert, external level
**	__ASSERT_L,		Assert with selected level
**
**	__PRECOND,		Pre Condition, internal level
**	__PRECOND_E,	Pre Condition, external level
**	__PRECOND_L,	Pre Condition with selected level
**
**	__POSTCOND,		Post Condition, internal level
**	__POSTCOND_E,	Post Condition, external level
**	__POSTCOND_L,	Post Condition with selected level
**
**	__INVARIANT,	Invariant, internal level
**	__INVARIANT_E,	Invariant, external level
**	__INVARIANT_L,	Invariant with selected level
**
**	__IMPOSSIBLE,	Impossible situation assert, internal level
**
**                                                             
*******************************************************************************/
#define __ASSERT(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::assertFail(foundation::Assertion::ASSERT_IN_INTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __ASSERT_E(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::assertFail(foundation::Assertion::ASSERT_IN_EXTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __ASSERT_L(level, cond, msg)\
	if(!(cond)){\
		foundation::Assertion::assertFail(level, #cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __PRECOND(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::precondFail(foundation::Assertion::ASSERT_IN_INTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __PRECOND_E(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::precondFail(foundation::Assertion::ASSERT_IN_EXTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __PRECOND_L(level, cond, msg)\
	if(!(cond)){\
		foundation::Assertion::precondFail(level, #cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __POSTCOND(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::postcondFail(foundation::Assertion::ASSERT_IN_INTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __POSTCOND_E(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::postcondFail(foundation::Assertion::ASSERT_IN_EXTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __POSTCOND_L(level, cond, msg)\
	if(!(cond)){\
		foundation::Assertion::postcondFail(level, #cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __INVARIANT(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::invariantFail(foundation::Assertion::ASSERT_IN_INTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __INVARIANT_E(cond, msg)\
	if(!(cond)){\
		foundation::Assertion::invariantFail(foundation::Assertion::ASSERT_IN_EXTERNAL_LEVEL,\
			#cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __INVARIANT_L(level, cond, msg)\
	if(!(cond)){\
		foundation::Assertion::invariantFail(level, #cond, msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __IMPOSSIBLE(msg)\
	{\
		foundation::Assertion::impossibleFail(foundation::Assertion::ASSERT_IN_INTERNAL_LEVEL,\
			"IMPOSSIBLE SITUATION", msg, __FILE__, __FUNCTION__, __LINE__);\
	}

#define __IMPOSSIBLE_E(msg)\
	{\
	foundation::Assertion::impossibleFail(foundation::Assertion::ASSERT_IN_EXTERNAL_LEVEL,\
	"IMPOSSIBLE SITUATION", msg, __FILE__, __FUNCTION__, __LINE__);\
}

// End Namespace [10/18/2007 Roger]
}

// End File [10/18/2007 Roger]
#endif
