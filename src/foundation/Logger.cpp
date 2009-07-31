/*******************************************************************************
**
**  NAME:			Logger.cpp	
**	VER: 			1.0
**  CREATE DATE: 	06/04/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Logger's method implementation
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Logger.h"
#include "Properties.h"


namespace foundation
{

const int Logger::maxLogFileSize_ = 20 * 1024 * 1024;//20M
int Logger::totalLineCount_ = 0;

#ifdef _DEBUG
bool Logger::enableQDebug_ = true;
#else
bool Logger::enableQDebug_ = false;
#endif // _DEBUG

QHash<QString, Logger> Logger::loggerStore_;///< Store of loggers

struct Logger::LoggerImpl
{
	LoggerImpl()
		: lineCount (0)
		, file (NULL)
	{
	}

	~LoggerImpl()
	{
		SAFE_DELETE(file);
	}

	QString name;
	int lineCount;

	QFile* file;
	QTextStream ts;
};


struct Logger::LoggerHelper
{
	LoggerHelper()
	{
		Logger::startup();
	}

	~LoggerHelper()
	{
		Logger::cleanup();
	}
};


Logger::Logger()
: impl_ (new LoggerImpl)
{

}

Logger::~Logger()
{
}


void 
Logger::log2file(LogLevel level, 
				 const QString& name,
				 const QString& msg, 
				 const char* file, 
				 const char* func,
				 int line)
{
	static QStringList LevelNames = QStringList()<<"DEBUG"<<"INFO "<<"WARN "<<"ERROR"<<"FETAL";
	static int LineCount = 0;

	if (impl_->file)
	{
		QString datetime = QDateTime::currentDateTime().toString(Qt::ISODate);
		size_t threadId = (size_t)QThread::currentThreadId();

		QString content = 			
			QString("[0x%1, %2] %3 (%4, %5, %6 - %7 (%8))").arg((uint)threadId, 4, 16, QChar('0')).arg(datetime)
			.arg(msg, -100).arg(name, LevelNames.value(level), file, func).arg(line);
		
		impl_->ts<<content<<"\r\n";

		if (++impl_->lineCount % 20 == 0)
			impl_->ts.flush();

		if (Logger::enableQDebug_)
			qDebug()<<content;

		++Logger::totalLineCount_;
	}
}


void 
Logger::enableQDebug(bool enable)
{
	Logger::enableQDebug_ = enable;
}


int 
Logger::totalLineCount()
{
	return Logger::totalLineCount_;
}


void 
Logger::startup()
{
	Properties prop = Properties::fromFile(FOUNDATION_LOG_CONFIG_FILE);

	QStringList loggerKeys = prop.getGroupKeys("log4j.logger");

	QStringList loggers;
	QStringList files;

	foreach(const QString& key, loggerKeys)
	{
		QString logger = key;
		logger.remove(QString("log4j.logger."));
		loggers<<logger;

		QStringList appenderValues;
		prop.getStringArrayValue(key, appenderValues);
		QString appender = appenderValues.value(1);

		if (!appender.isEmpty())
		{
			QString fileKey = QString("log4j.appender.%1.File").arg(appender);
			QString file;
			prop.getStringValue(fileKey, file);

			files<<file;
		}
	}

	if (loggers.size() == files.size())
	{
		for (int i = 0; i < loggers.size(); ++i)
		{
			Logger::loggerStore_.insert(loggers[i], 
				Logger::createLogger(loggers[i], files[i]));
		}
	}
}


void 
Logger::cleanup()
{
	foreach (Logger logger, Logger::loggerStore_.values())
	{
		logger.impl_->ts.flush();
	}
}


foundation::Logger 
Logger::createLogger(const QString& name, const QString& file)
{
	Logger logger;

	logger.impl_->name = name;
	logger.impl_->file = new QFile(file);

	QIODevice::OpenMode mode;

	if (logger.impl_->file->size() > maxLogFileSize_)
	{
		mode = QFile::WriteOnly | QFile::Truncate | QFile::Text;
	}
	else
	{
		mode = QFile::WriteOnly | QFile::Append | QFile::Text;
	}

	if (logger.impl_->file->open(mode))
	{
		logger.impl_->ts.setDevice(logger.impl_->file);

		return logger;
	}
	else
	{
		return Logger();
	}
}


#if FOUNDATION_USE_LOG4CXX

void
Logger::log(const QString& msg, const char* file, const char* func, int line)
{
	log4cxx::spi::LocationInfo li(file, func, line);
	log4cxx::Logger::getLogger(SYSTEM_LOGGER)->debug(qPrintable(msg), li);
}


void
Logger::logWithLogger(const QString& name,
					  const QString& msg,
					  const char* file, 
					  const char* func,
					  int line)
{
	log4cxx::spi::LocationInfo li(file, func, line);
	log4cxx::Logger::getLogger(qPrintable(name))->debug(qPrintable(msg), li);
}


void 
Logger::logWithLoggerAndLevel(const QString& name,
							  LogLevel level,
							  const QString& msg, 
							  const char* file, 
							  const char* func,
							  int line)
{
	log4cxx::spi::LocationInfo li(file, func, line);

	switch(level)
	{
	case LOG_DEBUG_LEVEL:
		log4cxx::Logger::getLogger(qPrintable(name))->debug(qPrintable(msg), li);
		break;
	case LOG_INFO_LEVEL:
		log4cxx::Logger::getLogger(qPrintable(name))->info(qPrintable(msg), li);
		break;
	case LOG_WARN_LEVEL:
		log4cxx::Logger::getLogger(qPrintable(name))->warn(qPrintable(msg), li);
		break;
	case LOG_ERROR_LEVEL:
		log4cxx::Logger::getLogger(qPrintable(name))->error(qPrintable(msg), li);
		break;
	case LOG_FATAL_LEVEL:
		log4cxx::Logger::getLogger(qPrintable(name))->fatal(qPrintable(msg), li);
		break;
	}
}

#else

void
Logger::log(const QString& msg, const char* file, const char* func, int line)
{
	Logger::logWithLoggerAndLevel(
		SYSTEM_LOGGER, LOG_DEBUG_LEVEL, msg, file, func, line);
}


void
Logger::logWithLogger(const QString& name,
					  const QString& msg,
					  const char* file, 
					  const char* func,
					  int line)
{
	Logger::logWithLoggerAndLevel(
		name, LOG_DEBUG_LEVEL, msg, file, func, line);
}


void 
Logger::logWithLoggerAndLevel(const QString& name,
							  LogLevel level,
							  const QString& msg, 
							  const char* file, 
							  const char* func,
							  int line)
{
	static LoggerHelper helper;
	//if (!Logger::startup_)
	//	Logger::startup();

	Logger lg = Logger::loggerStore_.value(name);
	lg.log2file(level, name, msg, file, func, line);

	if (name != SYSTEM_LOGGER)
	{
		lg = Logger::loggerStore_.value(SYSTEM_LOGGER);
		lg.log2file(level, name, msg, file, func, line);
	}
}


#endif


}

