/*******************************************************************************
**
**  NAME:			arch.h	
**	VER: 			1.0
**  CREATE DATE: 	06/02/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Type definition of some primitive C/C++ type, as standard for all modules, 
**				every modules must use this type definition in its interface.
**				Provide a hardware architecture abstration.
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#ifndef arch_h__
#define arch_h__

typedef signed char				INT8;  
typedef unsigned char			UINT8; 
typedef unsigned char			BYTE;

typedef signed short			INT16; 
typedef unsigned short			UINT16;

typedef signed int				INT32; 
typedef unsigned int			UINT32;

#if (_MSC_VER >= 1400)
typedef signed long long		INT64;
typedef unsigned long long		UINT64;
#endif

typedef int						BOOL;///< C style BOOL

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/** TRIVALUE for triple options, has additional 'NULL' */
enum TriBool
{
	VALUE_NULL	= -1,//Null or undefined
	VALUE_FALSE = 0	,//False
	VALUE_TRUE	= 1	,//True
};
typedef enum TriBool TriBool;

/** NULL Pointer Definition */
#ifndef NULL
#ifdef __cplusplus
#define NULL        (0L)
#else /* !__cplusplus */
#define NULL        ((void*) 0)
#endif /* !__cplusplus */
#endif

/*******************************************************************************
** 	                                                                
** Define Parameter Type, when the parameter is pointer or reference,
** this is useful to identify what the parameter serve for.                                                               
**                                                             
*******************************************************************************/
#define _IN			///< Input type parameter
#define _OUT		///< Output type, use to transfer output
#define _INOUT		///< Both In&Out
#define _FREE		///< Free the data pointed by pointer

/** safe delete macro */
#define SAFE_FREE(ptr)			if (ptr) {free(ptr); ptr = 0;}

#ifdef __cplusplus
#define SAFE_DELETE(ptr)		if (ptr) {delete ptr; ptr = 0;}
#define SAFE_ARRAY_DELETE(ptr)	if (ptr) {delete[] ptr; ptr = 0;}
#endif

#endif // arch_h__



