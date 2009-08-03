/*******************************************************************************
**
**  NAME:			Configuration.h	
**	VER: 			1.0
**  CREATE DATE: 	06/08/2009
**  AUTHOR:			Roger
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class Configuration, provide configuration information of system
**
**  --------------------------------------------------------------
**
**	HISTORY:	
**
**
*******************************************************************************/
#ifndef Configuration_h__
#define Configuration_h__
#include "foundation_global.h"


namespace foundation {


/**\brief
Configuration information

An configuration object contains two parts:

1, The configuration of the system (read-only)
2, The user settings value of the system (read/write)

Usually, a system only contains one configuration object,
and this object can be got where it needed.

When user read an value from the configuration, it will find the user settings
first then the read-only configuration.

*/
class FOUNDATION_EXPORT Configuration
{
public:
	Configuration();
	Configuration(const QString& configFile, const QString& settingsFile);

	virtual ~Configuration(void);

public://query
	QString configFile() const;
	QString settingsFile() const;

	QVariant value(const QString& key, const QVariant& default = QVariant()) const;
	QString stringValue(const QString& key, const QString& default = QString()) const;
	bool boolValue(const QString& key, bool default = false) const;
	int intValue(const QString& key, int default = 0) const;
	double doubleValue(const QString& key, double default = 0.0) const;

	QSettings* configuration();
	QSettings* settings();

public://command
	void beginGroup(const QString& group);
	void endGroup();

	void setSettingsValue(const QString& key, const QVariant& value);
	void setConfigValue(const QString& key, const QVariant& value);

private:
	QSharedPointer<QSettings> config_;
	QSharedPointer<QSettings> settings_;

public:
	static void setDefaultFiles(const QString& configFile,
		const QString& settingsFile);

	static bool hasInstance(const QString& configFile,
		const QString& settingsFile);

	///<return the first instance found or create a default configuration
	static Configuration* instance();

	static Configuration* instance(const QString& configFile,
		const QString& settingsFile);

private:
	static void initDefaultFiles();

	static QString defaultConfigFile_;
	static QString defaultSettingsFile_;

	static QList<Configuration*> systemConfigurations_;
};

}

#endif // Configuration_h__


