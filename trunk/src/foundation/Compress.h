/*******************************************************************************
**
**  NAME:			Compress.h	
**	VER: 			1.0
**  CREATE DATE: 	2007/09/13
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Provide some compress ralative helper function and classes
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#ifndef Compress_h__
#define Compress_h__
#include "foundation_global.h"

#include <QImage>

namespace foundation
{

QByteArray FOUNDATION_EXPORT compress(const BYTE* src, int len);
QByteArray FOUNDATION_EXPORT compress(const QByteArray& ba);

int FOUNDATION_EXPORT decompress(const BYTE* src, int src_len, BYTE* trg, int trg_len);
int FOUNDATION_EXPORT decompress(const QByteArray&ba, BYTE* trg, int len);

class FOUNDATION_EXPORT CompressedImage
{

public:
	CompressedImage(bool useDefaultLevel_ = false,
		bool useFastAlgo = false);

	~CompressedImage();

	int size();

	void compress(const QImage& image);
	QImage uncompress();

	static int compressLevel();
	static void resetCompressLevel();
	static void adjustCompressLevel(double usedTime);

private:
	static int compressLevel_;

	QByteArray data_;

	QSize imgsize_;
	QImage::Format format_;

	bool useDefaultLevel_;
	bool useFastAlgo_;
};

}

#endif // Compress_h__

