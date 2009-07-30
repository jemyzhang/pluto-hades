/*******************************************************************************
**
**  NAME:			ImageProcessor.cpp	
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
#include "stdafx.h"
#include "ImageProcessor.h"
#include "Assertion.h"

namespace foundation {


int 
ImageProcessor::margin(QImage& docImg,
					   MarginSide side, 
					   QRgb marginColor /*= qRgba(255, 255, 255, 255)*/,
					   int deltaR,
					   int deltaG,
					   int deltaB,
					   int deltaA)
{
	__PRECOND_E(!docImg.isNull(), "Null image");

	if (docImg.depth() != 32)
		docImg = docImg.convertToFormat(QImage::Format_ARGB32);


	int x = 0, y = 0;
	int marginR = qRed(marginColor);
	int marginG = qGreen(marginColor);
	int marginB = qBlue(marginColor);
	int marginA = qAlpha(marginColor);

	if (side == Left)
	{
		for (x = 0; x < docImg.width(); ++x)
		{
			for (y = 0; y < docImg.height(); ++y)
			{
				QRgb color = docImg.pixel(x, y);
				
				bool equal = qAbs(qRed(color) - marginR) <= deltaR &&
					qAbs(qGreen(color) - marginG) <= deltaG &&
					qAbs(qBlue(color) - marginB) <= deltaB &&
					qAbs(qAlpha(color) - marginA) <= deltaA;

				if (!equal)
					return x;
			}
		}
	}
	else if (side == Right)
	{
		for (x = docImg.width() - 1; x >= 0; --x)
		{
			for (y = 0; y < docImg.height(); ++y)
			{
				QRgb color = docImg.pixel(x, y);

				bool equal = qAbs(qRed(color) - marginR) <= deltaR &&
					qAbs(qGreen(color) - marginG) <= deltaG &&
					qAbs(qBlue(color) - marginB) <= deltaB &&
					qAbs(qAlpha(color) - marginA) <= deltaA;

				if (!equal)
					return (docImg.width() - x);
			}
		}
	}
	else if (side == Top)
	{
		for (y = 0; y < docImg.height(); ++y)
		{
			for (x = 0; x < docImg.width(); ++x)
			{
				QRgb color = docImg.pixel(x, y);

				bool equal = qAbs(qRed(color) - marginR) <= deltaR &&
					qAbs(qGreen(color) - marginG) <= deltaG &&
					qAbs(qBlue(color) - marginB) <= deltaB &&
					qAbs(qAlpha(color) - marginA) <= deltaA;

				if (!equal)
					return y;
			}
		}
	}
	else
	{
		for (y = docImg.height() - 1; y >= 0; --y)
		{
			for (x = 0; x < docImg.width(); ++x)
			{
				QRgb color = docImg.pixel(x, y);

				bool equal = qAbs(qRed(color) - marginR) <= deltaR &&
					qAbs(qGreen(color) - marginG) <= deltaG &&
					qAbs(qBlue(color) - marginB) <= deltaB &&
					qAbs(qAlpha(color) - marginA) <= deltaA;

				if (!equal)
					return (docImg.height() - y);
			}
		}
	}

	return 0;
}


double 
ImageProcessor::marginRatio(QImage& docImg, 
							MarginSide side, 
							QRgb marginColor /*= qRgba(255, 255, 255, 255)*/, 
							int deltaR /*= 0*/, 
							int deltaG /*= 0*/, 
							int deltaB /*= 0*/, 
							int deltaA /*= 255*/)
{
	int mg = margin(docImg, side, marginColor, deltaR, deltaG, deltaB, deltaA);

	double ratio = (side == Left || side == Right) ?
		mg * 1.0 / docImg.width() : mg * 1.0 /docImg.height();

	return ratio;
}


}
