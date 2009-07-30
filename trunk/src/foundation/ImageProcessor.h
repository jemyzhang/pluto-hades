/*******************************************************************************
**
**  NAME:			ImageProcessor.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/22
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class ImageProcessor
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/22
**
**
*******************************************************************************/
#ifndef ImageProcessor_h__
#define ImageProcessor_h__
#include "foundation_global.h"

#include <QImage>


namespace foundation {


///< Some useful image process algorithms
class FOUNDATION_EXPORT ImageProcessor
{
public:
	enum MarginSide
	{
		Left,
		Top,
		Right,
		Bottom,
	};
public:
	static int margin(QImage& docImg, 
		MarginSide side,
		QRgb marginColor = qRgba(255, 255, 255, 255),
		int deltaR = 0,
		int deltaG = 0,
		int deltaB = 0,
		int deltaA = 255);

	static double marginRatio(QImage& docImg, 
		MarginSide side,
		QRgb marginColor = qRgba(255, 255, 255, 255),
		int deltaR = 0,
		int deltaG = 0,
		int deltaB = 0,
		int deltaA = 255);
};


#endif // ImageProcessor_h__


}

