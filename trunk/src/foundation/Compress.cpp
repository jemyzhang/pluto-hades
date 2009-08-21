/*******************************************************************************
**
**  NAME:			Compress.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/08/20
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
#include "stdafx.h"
#include "Compress.h"

#include "3rd/minilzo.h"
#include "Logger.h"
#include "Timer.h"

#include "../src/3rdparty/zlib/zlib.h"

//rewrite qt version for save memory purpose
bool qUncompress_(const uchar* data, int nbytes, uchar* target, ulong trgsz)
{
	if (!data) {
		qWarning("qUncompress: Data is null");
		return false;
	}
	if (nbytes <= 4) {
		if (nbytes < 4 || (data[0]!=0 || data[1]!=0 || data[2]!=0 || data[3]!=0))
			qWarning("qUncompress: Input data is corrupted");
		return false;
	}

	ulong expectedSize = (data[0] << 24) | (data[1] << 16) |
		(data[2] <<  8) | (data[3]      );
	ulong len = qMax(expectedSize, 1ul);


	int res;
	do {
		if (len > trgsz)
			return false;

		res = ::uncompress(target, &len, (uchar*)data+4, nbytes-4);

		switch (res) {
		case Z_OK:
			break;
		case Z_MEM_ERROR:
			qWarning("qUncompress: Z_MEM_ERROR: Not enough memory");
			break;
		case Z_BUF_ERROR:
			len *= 2;
			break;
		case Z_DATA_ERROR:
			qWarning("qUncompress: Z_DATA_ERROR: Input data is corrupted");
			break;
		}
	} while (res == Z_BUF_ERROR);

	return res == Z_OK;
}

static bool init_flag = false;
static BYTE wrkmem[LZO1X_1_MEM_COMPRESS];

static bool 
init_minilzo()
{
	if (!init_flag)
	{
		if (lzo_init() != LZO_E_OK)
		{
			__LOG_E("internal error - lzo_init() failed !!!\n");
			__LOG_E("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable `-DLZO_DEBUG' for diagnostics)\n");
		}
		else
		{
			init_flag = true;
		}
	}

	return init_flag;
}


namespace foundation
{


QByteArray 
compress(const BYTE* src, int len)
{
	QByteArray ba;

	if (init_minilzo())
	{
		lzo_uint trgLen = len + len / 16 + 64 + 3;
		ba.resize(trgLen);

		if (!ba.isNull())
		{
			int result = 
				lzo1x_1_compress(src, len, (uchar *)ba.data(), &trgLen, wrkmem);

			if (result == LZO_E_OK)
			{
				ba.resize(trgLen);
			}
			else
			{
				ba.resize(0);
			}
		}
	}

	return ba;
}


QByteArray 
compress(const QByteArray& data)
{
	return compress(reinterpret_cast<const uchar*>(data.constData()),
		data.size());
}


int 
decompress(const BYTE* src, int src_len, BYTE* trg, int trg_len)
{
	if (init_minilzo())
	{
		lzo_uint trg_len_2 = trg_len;

		int result = 
			lzo1x_decompress(src, src_len, trg, &trg_len_2, wrkmem);

		if (result == LZO_E_OK)
			return trg_len_2;
		else
			return 0;
	}

	return 0;
}


int 
decompress(const QByteArray& data, BYTE* trg, int len)
{
	return decompress(reinterpret_cast<const uchar*>(data.constData()),
		data.size(), trg, len);
}


int CompressedImage::compressLevel_ = 1;


CompressedImage::CompressedImage(bool useDefaultLevel_ /*= false*/,
								 bool useFastAlgo /*= false*/)
	: useDefaultLevel_ (useDefaultLevel_)
	, useFastAlgo_ (useFastAlgo)
{

}



CompressedImage::~CompressedImage()
{

}


int 
CompressedImage::size()
{
	return data_.size();
}


void 
CompressedImage::compress(const QImage& image)
{
	GuardTimer guard(__FUNCTION__);

	int level = useDefaultLevel_ ?  -1 : compressLevel_;

	if (useFastAlgo_)
	{
		data_ = foundation::compress(image.bits(), image.numBytes());
	}
	else
	{
		data_ = qCompress(image.bits(), image.numBytes(), level);
	}

	imgsize_ = image.size();
	format_ = image.format();

	if (!useDefaultLevel_)
	{
		CompressedImage::adjustCompressLevel(guard.elapsed());
	}

	__LOG(QString("Compress level %1, ratio %2/%3 - %4")
		.arg(compressLevel_)
		.arg(data_.size())
		.arg(image.numBytes())
		.arg(data_.size() * 1.0 / image.numBytes()));
}


QImage 
CompressedImage::uncompress()
{
	GuardTimer guard(__FUNCTION__);

	QImage image(imgsize_, format_);

	if (!image.isNull())
	{
		bool success;

		if (useFastAlgo_)
		{
			success = decompress(reinterpret_cast<const uchar*>(data_.constData()), 
				data_.size(),
				image.bits(),
				image.numBytes()) > 0;
		}
		else
		{
			success = qUncompress_(reinterpret_cast<const uchar*>(data_.constData()), 
					data_.size(),
					image.bits(),
					image.numBytes());
		}

		if (!success)
			image = QImage();//failed, release
	}

	return image;
}


int 
CompressedImage::compressLevel()
{
	return compressLevel_;
}


void 
CompressedImage::resetCompressLevel()
{
	compressLevel_ = 1;
}


void 
CompressedImage::adjustCompressLevel(double used)
{
	if (used > 1)
	{
		--compressLevel_;
	}
	else if (used < 0.5)
	{
		++compressLevel_;
	}

	compressLevel_ = qBound(1, compressLevel_, 9);
}


}

