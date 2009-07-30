/*******************************************************************************
**
**  NAME:			Logger.h	
**	VER: 			1.0
**  CREATE DATE: 	06/04/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare, All Rights Reserved
** 
**	
**	PURPOSE:	Logger interface
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.1, 05/15/2009	Add into Pluto 2, use log4cxx 0.10 
**
**
*******************************************************************************/
#ifndef FOUNDATION_LOGGER_H
#define FOUNDATION_LOGGER_H
#include "foundation_global.h"

#include <QHash>
#include <QString>
#include <QSharedPointer>

namespace foundation 
{

class FOUNDATION_EXPORT Logger
{
public:
	enum LogLevel
	{
		LOG_DEBUG_LEVEL,
		LOG_INFO_LEVEL,
		LOG_WARN_LEVEL,
		LOG_ERROR_LEVEL,
		LOG_FATAL_LEVEL,
	};

public:
	Logger();
	~Logger();

public:
	static void enableQDebug(bool enable);
	static int totalLineCount();

private:
	static const int maxLogFileSize_;///< max log file size
	static int totalLineCount_;///< log line count since startup
	static bool enableQDebug_;///< enable log to QDebug?

	static QHash<QString, Logger> loggerStore_;///< store of loggers

	static void startup();
	static void cleanup();

	static Logger createLogger(const QString& name, const QString& file);

	void log2file(LogLevel level,
		const QString& name,
		const QString& msg,
		const char* file, 
		const char* func, 
		int line);

private:
	struct LoggerImpl;
	struct LoggerHelper;

	QSharedPointer<LoggerImpl> impl_;


public:
/*******************************************************************************
** 	                                                                
** log,						log in default system logger
** logWithLogger,			log in selected logger
** logWithLoggerAndLevel,	log in selected logger with level
**                                                             
*******************************************************************************/
	static void log(const QString& msg,
		const char* file = 0,
		const char* func = 0,
		int line = -1);

	static void logWithLogger(const QString& name, 
		const QString& msg, 
		const char* file = 0, 
		const char* func = 0,
		int line = -1);

	static void logWithLoggerAndLevel(const QString& name,
		LogLevel level,
		const QString& msg, 
		const char* file = 0, 
		const char* func = 0,
		int line = -1);
};

//End namespace
}

#define SYSTEM_LOGGER		"SYSTEM"				//System Logger Name
#define ERROR_LOGGER		"SYSTEM.ERROR"			//Error Logger Name
#define ASSERT_LOGGER		"SYSTEM.ERROR.ASSERT"	//Assert Error Logger Name
#define PROFILING_LOGGER	"SYSTEM.PROFILE"		//Performance Profile Logger Name


/*******************************************************************************
** 	                                                                
**	LOG macro
**
**	__LOG,		Log in system logger with debug level
**	__LOG_L,	Log in selected logger with debug level
**	__LOG_LL,	Log in selected logger and with selected level
**	__LOG_P,	Log in profile logger with info level
**	__LOG_W,	Log in error logger with warn level
**	__LOG_E,	Log in error logger with error level
**	__LOG_A,	Log in assert logger with fatal level
**
*******************************************************************************/
#define __LOG(msg) \
	foundation::Logger::log(msg, __FILE__, __FUNCTION__, __LINE__)

#define __LOG_L(logger, msg) \
	foundation::Logger::logWithLogger(logger, msg, __FILE__, __FUNCTION__, __LINE__)

#define __LOG_LL(logger, level, msg) \
	foundation::Logger::logWithLoggerAndLevel(logger, level, msg, __FILE__, __FUNCTION__, __LINE__)

#define __LOG_P(msg) \
	__LOG_LL(PROFILING_LOGGER, foundation::Logger::LOG_INFO_LEVEL, msg)

#define __LOG_W(msg) \
	__LOG_LL(ERROR_LOGGER, foundation::Logger::LOG_WARN_LEVEL, msg)

#define __LOG_E(msg) \
	__LOG_LL(ERROR_LOGGER, foundation::Logger::LOG_ERROR_LEVEL, msg)

#define __LOG_A(msg) \
	__LOG_LL(ASSERT_LOGGER, foundation::Logger::LOG_FATAL_LEVEL, msg)


#endif

