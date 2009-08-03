/*******************************************************************************
**
**  NAME:			Configuration.cpp
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
#include "StdAfx.h"
#include "Configuration.h"
#include "Assertion.h"

namespace foundation {

QString Configuration::defaultConfigFile_;
QString Configuration::defaultSettingsFile_;
QList<Configuration*> Configuration::systemConfigurations_;


Configuration::Configuration()
{
	Configuration::initDefaultFiles();

	this->config_ = QSharedPointer<QSettings>(
		new QSettings(defaultConfigFile_, QSettings::IniFormat));
	this->settings_ = QSharedPointer<QSettings>(
		new QSettings(defaultSettingsFile_, QSettings::IniFormat));

	__POSTCOND_E(config_ && settings_, "Cannot create QSettings");

	Configuration::systemConfigurations_.append(this);
}


Configuration::Configuration(const QString& configFile, 
							 const QString& settingsFile)
{
	this->config_ = QSharedPointer<QSettings>(
		new QSettings(configFile, QSettings::IniFormat));
	this->settings_ = QSharedPointer<QSettings>(
		new QSettings(settingsFile, QSettings::IniFormat));

	__POSTCOND_E(config_ && settings_, "Cannot create QSettings");

	Configuration::systemConfigurations_.append(this);
}


Configuration::~Configuration(void)
{
}


QString 
Configuration::configFile() const
{
	return this->config_->fileName();
}


QString 
Configuration::settingsFile() const
{
	return this->settings_->fileName();
}


QVariant 
Configuration::value(const QString& key, 
					 const QVariant& default /*= QVariant()*/) const
{
	if (this->settings_->contains(key))
	{
		return this->settings_->value(key, default);
	}
	else
	{
		return this->config_->value(key, default);
	}
}


QString 
Configuration::stringValue(const QString& key, 
						   const QString& default /*= QString()*/) const
{
	return this->value(key, default).toString();
}


bool 
Configuration::boolValue(const QString& key, bool default /*= false*/) const
{
	return this->value(key, default).toBool();
}


int 
Configuration::intValue(const QString& key, int default /*= 0*/) const
{
	return this->value(key, default).toInt();
}


double 
Configuration::doubleValue(const QString& key, double default /*= 0.0*/) const
{
	return this->value(key, default).toDouble();
}


QSettings* 
Configuration::configuration()
{
	return this->config_.data();
}


QSettings* 
Configuration::settings()
{
	return this->settings_.data();
}


void 
Configuration::beginGroup(const QString& groupPrefix)
{
	this->settings_->beginGroup(groupPrefix);
	this->config_->beginGroup(groupPrefix);
}


void 
Configuration::endGroup()
{
	this->settings_->endGroup();
	this->config_->endGroup();
}


void 
Configuration::setSettingsValue(const QString& key, const QVariant& value)
{
	this->settings_->setValue(key, value);
	this->settings_->sync();
}


void 
Configuration::setConfigValue(const QString& key, const QVariant& value)
{
	this->config_->setValue(key, value);
}


void 
Configuration::setDefaultFiles(const QString& configFile, 
							   const QString& settingsFile)
{
	Configuration::defaultConfigFile_ = configFile;
	Configuration::defaultSettingsFile_ = settingsFile;
}


bool 
Configuration::hasInstance(const QString& configFile, 
						   const QString& settingsFile)
{
	foreach (Configuration* config, Configuration::systemConfigurations_)
	{
		if (QFileInfo(configFile) == QFileInfo(config->configFile()) &&
			QFileInfo(settingsFile) == QFileInfo(config->settingsFile()))
		{
			return true;
		}
	}

	return false;
}


Configuration* 
Configuration::instance()
{
	if (systemConfigurations_.size())
	{
		return systemConfigurations_.first();
	}
	else
	{
		Configuration::initDefaultFiles();

		return instance(Configuration::defaultConfigFile_, 
			Configuration::defaultSettingsFile_);
	}
}


Configuration* 
Configuration::instance(const QString& configFile,
						const QString& settingsFile)
{
	Configuration* result = NULL;

	foreach (Configuration* config, Configuration::systemConfigurations_)
	{
		if (QFileInfo(configFile) == QFileInfo(config->configFile()) &&
			QFileInfo(settingsFile) == QFileInfo(config->settingsFile()))
		{
			result = config;
		}
	}

	if (!result)
	{
		result = new Configuration(configFile, settingsFile);
	}

	__POSTCOND_E(result, "Cannot create Configuration.");

	return result;
}


void 
Configuration::initDefaultFiles()
{
	//init default files
	if (Configuration::defaultConfigFile_.isEmpty())
	{
		Configuration::defaultConfigFile_ = 
			QCoreApplication::applicationDirPath() + "/config/config.ini";
		Configuration::defaultSettingsFile_ =
			QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/settings.ini";

		__LOG(QString("Config file : %1, settings file : %2")
			.arg(Configuration::defaultConfigFile_, Configuration::defaultSettingsFile_));
	}
}


//end foundation namespace
}
