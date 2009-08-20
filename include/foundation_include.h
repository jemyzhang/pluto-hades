/*******************************************************************************
**
**  NAME:			foundation_include.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/11
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Include foundation library header
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/11
**
**
*******************************************************************************/
#ifndef foundation_include_h__
#define foundation_include_h__

#include "../src/foundation/Logger.h"
#include "../src/foundation/Exception.h"
#include "../src/foundation/Assertion.h"
#include "../src/foundation/Enum.h"
#include "../src/foundation/Properties.h"
#include "../src/foundation/Timer.h"
#include "../src/foundation/ImageProcessor.h"

#include "../src/foundation/Compress.h"


typedef foundation::Logger								pltLogger;
typedef foundation::Enum								pltEnum;
typedef foundation::Properties							pltProperties;

typedef foundation::Timer								pltTimer;
typedef foundation::GuardTimer							pltGuardTimer;
typedef foundation::AvgGuardTimer						pltAvgGuardTimer;
typedef foundation::GroupableTimer						pltGroupableTimer;

typedef foundation::Exception							pltException;
typedef foundation::AssertException						pltAssertException;
typedef foundation::ContractException					pltContractException;
typedef foundation::PreconditionException				pltPreconditionException;
typedef foundation::PostconditionException				pltPostconditionException;
typedef foundation::InvariantException					pltInvariantException;
typedef foundation::ImpossibleException					pltImpossibleException;

typedef foundation::ImageProcessor						pltImageProcessor;

typedef foundation::CompressedImage						pltCompressedImage;


#endif // foundation_include_h__