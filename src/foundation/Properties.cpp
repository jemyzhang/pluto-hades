/*******************************************************************************
**
**  NAME:			Properties.cpp	
**  VER: 			1.0
**  CREATE DATE: 	02/17/2008
**  AUTHOR:			Roger
**  
**  Copyright (C) 2008 - PlutoWare All Rights Reserved
** 
**------------------------------------------------------------------------------	
**  PURPOSE:		
**  NOTE:		
**
**------------------------------------------------------------------------------
**  HISTORY:	v1.0	02/17/2008	First create
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Properties.h"
#include "Logger.h"


namespace foundation
{


/*!	\brief Implementation of class Properties (as interface) */
struct Properties::PropertiesImpl
{
	QString					sourceFile;///< source file if it has
	QMap<QString, QString>	properties;///< map holds all (key, value) pairs
};


const QString Properties::GROUP_NONE = "";
const QString Properties::SPLITTER_COMMMA = ",";
const QString Properties::SPLITTER_SEMICOLON = ";";
const QString Properties::GROUP_KEY_CONNECTOR = ".";///< group.key connector


Properties::Properties(void)
: impl_(new PropertiesImpl())
{
}


Properties::~Properties(void)
{
}


Properties 
Properties::fromFile(const QString& config)
{
	Properties p;

	QFile f(config);

	if (f.open(QIODevice::ReadOnly))
	{
		QTextStream ts(&f);

		p = Properties::fromStream(ts);

		p.impl_->sourceFile = config;
	}

	return p;
}


Properties 
Properties::fromString(const QString& str)
{
	Properties prop;

	QTextStream ts(str.toLocal8Bit(), QIODevice::ReadOnly);
	
	prop = Properties::fromStream(ts);

	return prop;
}


Properties 
Properties::fromStream(QTextStream& ts)
{
	Properties prop;

	while (!ts.atEnd())
	{
		QString line = ts.readLine();
		line = line.trimmed();

		if (line.isEmpty() || line.startsWith("#"))
			continue;//empty or comment

		QStringList pair = line.split("=", QString::SkipEmptyParts);

		if (!pair.at(0).isEmpty())
			prop.impl_->properties.insert(pair.at(0), pair.at(1));
	}

	return prop;
}


bool 
Properties::isNull() const	
{
	return impl_->properties.empty();
}


Properties::operator bool() const	
{
	return !isNull();
}


bool 
Properties::operator== (const Properties& other) const
{
	if (this->impl_ == other.impl_)
		return true;

	return this->impl_->properties == other.impl_->properties;
}


bool 
Properties::operator!= (const Properties& other) const
{
	return !operator==(other);
}


QTextStream& 
Properties::print(QTextStream& out, bool group) const
{
	out<<this->toString(group);

	return out;
}


QString 
Properties::toString(bool group) const
{
	QString detail;
	QTextStream ts(&detail);

	if (group)
	{
		QString lastG;

		foreach (const QString& key, impl_->properties.keys())
		{
			QString currG = Properties::getGroupNameOfKey(key);

			if (currG != lastG)
			{
				if (lastG.size())
					ts<<"\r\n";

				ts<<QString("#%1\r\n").arg(currG);

				lastG = currG;
			}

			ts<<key<<"="<<impl_->properties.value(key)<<"\r\n";
		}
	}
	else
	{
		foreach (const QString& key, impl_->properties.keys())
		{
			ts<<key<<"="<<impl_->properties.value(key)<<"\r\n";
		}
	}

	return detail;
}


bool 
Properties::save(bool group /*= false*/) const
{
	bool save = !(this->isNull() || this->impl_->sourceFile.isEmpty());
	
	if (save)
	{
		this->saveAs(this->impl_->sourceFile, group);
	}

	return save;
}


bool 
Properties::saveAs(const QString& file, bool group /*= false*/) const
{
	QFile f(file);

	if (f.open(QIODevice::WriteOnly))
	{
		QTextStream ts(&f);
		ts<<this->toString(group);

		return true;
	}
	else
	{
		return false;
	}
}

bool 
Properties::hasKey(const QString& key, const QString& group) const
{
	QString newKey = Properties::combineKeyAndGroup(key, group);

	return impl_->properties.contains(newKey);
}


QStringList 
Properties::getAllKeys() const
{
	return impl_->properties.keys();
}


QStringList 
Properties::getGroupKeys(const QString& group) const
{
	QStringList keys;
	QString prefix = Properties::combineKeyAndGroup("", group);

	foreach(const QString& key, impl_->properties.keys())
	{
		if (key.startsWith(prefix))
			keys.push_back(key);
	}

	return keys;
}


QStringList 
Properties::getAllValues() const
{
	return impl_->properties.values();
}


QStringList 
Properties::getGroupValues(const QString& group) const
{
	QStringList vals;
	QString prefix = Properties::combineKeyAndGroup("", group);

	foreach(const QString& key, impl_->properties.keys())
	{
		if (key.startsWith(prefix))
			vals.push_back(impl_->properties.value(key));
	}

	return vals;
}


bool 
Properties::getVariantValue(const QString& key,
							QVariant& vval,
							const QString& group /*= GROUP_NONE*/) const
{
	if (this->hasKey(key, group))
	{
		QString newKey = Properties::combineKeyAndGroup(key, group);
		vval = QVariant(impl_->properties[newKey]);

		return true;
	}
	else
	{
		return false;
	}
}





bool 
Properties::getStringValue(const QString& key, 
						   QString& sval, 
						   const QString& group) const
{
	if (this->hasKey(key, group))
	{
		QString newKey = Properties::combineKeyAndGroup(key, group);
		sval = impl_->properties[newKey];

		return true;
	}
	else
	{
		return false;
	}
}


bool 
Properties::getIntValue(const QString& key,
						int& ival,
						const QString& group) const
{
	bool rtval = false;
	QString sval;

	if (this->getStringValue(key, sval, group))
	{
		ival = QVariant(sval).toInt();
		rtval = true;
	}

	return rtval;
}


bool 
Properties::getBoolValue(const QString& key,
						 bool& bval, 
						 const QString& group) const
{
	bool rtval = false;
	QString sval;

	if (this->getStringValue(key, sval, group))
	{
		bval = QVariant(sval).toBool();
		rtval = true;
	}

	return rtval;
}


bool 
Properties::getDoubleValue(const QString& key, 
						   double& dval, 
						   const QString& group) const
{
	bool rtval = false;
	QString sval;

	if (this->getStringValue(key, sval, group))
	{
		dval = QVariant(sval).toDouble();
		rtval = true;
	}

	return rtval;
}


bool 
Properties::getVariantArrayValue(const QString& key, 
								 QList<QVariant>& vvals, 
								 const QString& group /*= GROUP_NONE*/, 
								 const QString& splitter /*= SPLITTER_COMMMA*/) const
{
	bool rtval = false;
	QStringList svals;

	if (this->getStringArrayValue(key, svals, group, splitter))
	{
		QList<QVariant> vvalsTemp;

		foreach(const QString& sval, svals)
		{
			vvalsTemp.push_back(QVariant(sval));
		}

		vvals = vvalsTemp;
		rtval = true;//success
	}

	return rtval;
}


bool 
Properties::getStringArrayValue(const QString& key, 
								QStringList& svals, 
								const QString& group,
								const QString& splitter) const
{
	bool rtval = false;
	QString sval;

	if (this->getStringValue(key, sval, group))
	{
		QStringList svalsTemp;
		QStringList svalsTemp2 = sval.split(splitter, QString::SkipEmptyParts);

		foreach(const QString& sval, svalsTemp2)
		{
			svalsTemp.push_back(sval.trimmed());
		}

		svals = svalsTemp;
		rtval = true;//success
	}

	return rtval;
}


bool 
Properties::getIntArrayValue(const QString& key, 
							 QList<int>& ivals, 
							 const QString& group,
							 const QString& splitter) const
{
	bool rtval = false;
	QStringList svals;

	if (this->getStringArrayValue(key, svals, group, splitter))
	{
		QList<int> ivalsTemp;

		foreach(const QString& sval, svals)
		{
			ivalsTemp.push_back(QVariant(sval).toInt());
		}

		ivals = ivalsTemp;
		rtval = true;//success
	}

	return rtval;
}


bool 
Properties::getBoolArrayValue(const QString& key, 
							  QList<bool>& bvals, 
							  const QString& group,
							  const QString& splitter) const
{
	bool rtval = false;
	QStringList svals;

	if (this->getStringArrayValue(key, svals, group, splitter))
	{
		QList<bool> bvalsTemp;

		foreach(const QString& sval, svals)
		{
			bvalsTemp.push_back(QVariant(sval).toBool());
		}

		bvals = bvalsTemp;
		rtval = true;//success
	}

	return rtval;
}


bool 
Properties::getDoubleArrayValue(const QString& key,
								QList<double>& dvals, 
								const QString& group,
								const QString& splitter) const
{
	bool rtval = false;
	QStringList svals;

	if (this->getStringArrayValue(key, svals, group, splitter))
	{
		QList<double> dvalsTemp;

		foreach(const QString& sval, svals)
		{
			dvalsTemp.push_back(QVariant(sval).toDouble());
		}

		dvals = dvalsTemp;
		rtval = true;//success
	}

	return rtval;
}


QString 
Properties::combineKeyAndGroup(const QString& key, 
							   const QString& group)
{
	QString newKey = (group == Properties::GROUP_NONE) ? key :
		(group + Properties::GROUP_KEY_CONNECTOR + key);

	return newKey.trimmed();
}


void 
Properties::setVariantValue(const QString& key, 
							const QVariant& vval, 
							const QString& group /*= GROUP_NONE*/)
{
	QString newKey = Properties::combineKeyAndGroup(key, group);

	if (newKey.size())
		impl_->properties[newKey] = vval.toString();
}


void 
Properties::setStringValue(const QString& key, 
						   const QString& sval, 
						   const QString& group /*= GROUP_NONE*/)
{
	QString newKey = Properties::combineKeyAndGroup(key, group);
	
	if (newKey.size())
		impl_->properties[newKey] = sval;
}


void 
Properties::setIntValue(const QString& key, 
						int ival,
						const QString& group /*= GROUP_NONE*/)
{
	this->setStringValue(key, QVariant(ival).toString(), group);
}


void 
Properties::setBoolValue(const QString& key,
						 bool bval,
						 const QString& group /*= GROUP_NONE*/)
{
	this->setStringValue(key, QVariant(bval).toString(), group);
}


void 
Properties::setDoubleValue(const QString& key, 
						   double dval, 
						   const QString& group /*= GROUP_NONE*/)
{
	this->setStringValue(key, QVariant(dval).toString(), group);
}


void 
Properties::setVariantArrayValue(const QString& key,
								 const QList<QVariant>& vvals, 
								 const QString& group /*= GROUP_NONE*/,
								 const QString& splitter /*= SPLITTER_COMMMA*/)
{
	QStringList svals;

	foreach(const QVariant& vval, vvals)
	{
		svals.push_back(vval.toString());
	}

	this->setStringArrayValue(key, svals, group, splitter);
}


void 
Properties::setStringArrayValue(const QString& key, 
								const QStringList& svals, 
								const QString& group /*= GROUP_NONE*/, 
								const QString& splitter /*= SPLITTER_COMMMA*/)
{
	QString connector = splitter + " ";
	QString sval = svals.join(connector);

	this->setStringValue(key, sval, group);
}


void 
Properties::setIntArrayValue(const QString& key, 
							 const QList<int>& ivals, 
							 const QString& group /*= GROUP_NONE*/, 
							 const QString& splitter /*= SPLITTER_COMMMA*/)
{
	QStringList svals;

	foreach(int ival, ivals)
	{
		svals.push_back(QVariant(ival).toString());
	}

	this->setStringArrayValue(key, svals, group, splitter);
}


void 
Properties::setBoolArrayValue(const QString& key, 
							  const QList<bool>& bvals, 
							  const QString& group /*= GROUP_NONE*/, 
							  const QString& splitter /*= SPLITTER_COMMMA*/)
{
	QStringList svals;

	foreach(bool bval, bvals)
	{
		svals.push_back(QVariant(bval).toString());
	}

	this->setStringArrayValue(key, svals, group, splitter);
}


void 
Properties::setDoubleArrayValue(const QString& key, 
								const QList<double>& dvals,
								const QString& group /*= GROUP_NONE*/,
								const QString& splitter /*= SPLITTER_COMMMA*/)
{
	QStringList svals;

	foreach(double dval, dvals)
	{
		svals.push_back(QVariant(dval).toString());
	}

	this->setStringArrayValue(key, svals, group, splitter);
}


void 
Properties::appendOtherProperties(const Properties& other)
{

	foreach(const QString& key, other.impl_->properties.keys())
	{
		impl_->properties.insert(key, other.impl_->properties.value(key));
	}
}


void 
Properties::deleteValue(const QString& key)
{
	impl_->properties.remove(key);
}


void 
Properties::deleteGroup(const QString& group)
{
	QStringList keys = this->getGroupKeys(group);

	foreach(const QString& key, keys)
	{
		impl_->properties.remove(key);
	}
}


QString 
Properties::getSourceFile() const
{
	return impl_->sourceFile;
}


QString 
Properties::getGroupNameOfKey(const QString& key)
{
	return key.left(key.lastIndexOf(Properties::GROUP_KEY_CONNECTOR));
}

// End Namespace [2/18/2008 Roger.Yi]
}


