/*******************************************************************************
**
**  NAME:			Archive.h	
**	VER: 			1.0
**  CREATE DATE: 	2009/08/20
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class Archive
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#ifndef Archive_h__
#define Archive_h__
#include "foundation_global.h"
#include "Exception.h"

namespace foundation
{

/**
 *	Provide class to access compressed file (rar, zip, 7z, ...)	
 */
class FOUNDATION_EXPORT Archive
{
public:
	Archive(void);
	~Archive(void);

public:
	QStringList fileList(const QString& filter) const;
	QList<int> fileIdxList(const QString& filter) const;

public:
	void open(const QString& file);
	void close();

	QByteArray decompressFile(const QString& inFile);
	QByteArray decompressFile(int idx);

	void decompressFile(const QString& inFile, const QString& outFile);
	void decompressFile(int idx, const QString& outFile);

private:
	struct ArchiveImpl;
	QSharedPointer<ArchiveImpl> impl_;
};


class FOUNDATION_EXPORT ArchiveException : public Exception
{
	DECLARE_EXCEPTION(ArchiveException)
};

}
#endif // Archive_h__

