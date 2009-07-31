/*******************************************************************************
**
**  NAME:			Properties.h	
**  VER: 			1.0
**  CREATE DATE: 	02/17/2008
**  AUTHOR:			Roger
**  
**  Copyright (C) 2008 - PlutoWare All Rights Reserved
** 
**------------------------------------------------------------------------------	
**  PURPOSE:	Class Properties	
**  NOTE:		
**
**------------------------------------------------------------------------------
**  HISTORY:	v1.0	02/17/2008	First create
**
**
*******************************************************************************/
#ifndef Properties_h__
#define Properties_h__
#include "foundation_global.h"

namespace foundation
{

	/*!	\brief Properties from config file, stream, string...
	*/
	class FOUNDATION_EXPORT Properties
	{
	public:
		static const QString GROUP_NONE;///< just null string, means no us group
		static const QString 
			SPLITTER_COMMMA;///< use to split a string to an array, ","
		static const QString
			SPLITTER_SEMICOLON;///< use to split a string to an array, ";"
		static const QString GROUP_KEY_CONNECTOR;///< group.key connector, "."

	public:
		/** Create properties from file, string, stream, 
		if failed return null properties object*/
		static Properties fromFile(const QString& config);
		static Properties fromString(const QString& str);
		static Properties fromStream(QTextStream& ts);


	public:
		/** Default constructor */
		Properties(void);
		~Properties(void);



		/*!	In Null Object? */
		bool isNull() const;
		operator bool() const;



		/*!	Equal? Use Value Semantic. */
		bool operator== (const Properties& other) const;

		/*!	Not Equal? */
		bool operator!= (const Properties& other) const;



		/** Save to file */
		bool save(bool group = false) const;
		bool saveAs(const QString& file, bool group = false) const;



		/** Print out and get string */
		QTextStream& print(QTextStream& out, bool group = false) const;
		QString toString(bool group = false) const;



		/** Has this key inside? */
		bool hasKey(const QString& key, const QString& group = GROUP_NONE) const;



		/** Get all keys */
		QStringList getAllKeys() const;

		/** Get keys of group */
		QStringList getGroupKeys(const QString& group) const;

		/** Get all values in string form */
		QStringList getAllValues() const;

		/** Get group values in string form */
		QStringList getGroupValues(const QString& group) const;



		/** Get value methods, failed if key not exist or type covert error */
		bool getVariantValue(const QString& key,
			QVariant& vval,
			const QString& group = GROUP_NONE) const;

		bool getStringValue(const QString& key, 
			QString& sval, 
			const QString& group = GROUP_NONE) const;

		bool getIntValue(const QString& key, 
			int& ival,
			const QString& group = GROUP_NONE) const;

		bool getBoolValue(const QString& key, 
			bool& bval,
			const QString& group = GROUP_NONE) const;

		bool getDoubleValue(const QString& key, 
			double& dval,
			const QString& group = GROUP_NONE) const;


		/** Get value methods, split the original value into an array */
		bool getVariantArrayValue(const QString& key, 
			QList<QVariant>& vvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA) const;

		bool getStringArrayValue(const QString& key, 
			QStringList& svals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA) const;

		bool getIntArrayValue(const QString& key, 
			QList<int>& ivals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA) const;

		bool getBoolArrayValue(const QString& key, 
			QList<bool>& bvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA) const;

		bool getDoubleArrayValue(const QString& key, 
			QList<double>& dvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA) const;



		/** Set value methods, add if key not exist */
		void setVariantValue(const QString& key, 
			const QVariant& vval,
			const QString& group = GROUP_NONE);

		void setStringValue(const QString& key, 
			const QString& sval,
			const QString& group = GROUP_NONE);

		void setIntValue(const QString& key, 
			int ival,
			const QString& group = GROUP_NONE);

		void setBoolValue(const QString& key, 
			bool bval,
			const QString& group = GROUP_NONE);

		void setDoubleValue(const QString& key, 
			double dval,
			const QString& group = GROUP_NONE);


		/** Set value methods, use splitter to connect an array to a string */
		void setVariantArrayValue(const QString& key, 
			const QList<QVariant>& vvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA);

		void setStringArrayValue(const QString& key, 
			const QStringList& svals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA);

		void setIntArrayValue(const QString& key, 
			const QList<int>& ivals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA);

		void setBoolArrayValue(const QString& key, 
			const QList<bool>& bvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA);

		void setDoubleArrayValue(const QString& key, 
			const QList<double>& dvals, 
			const QString& group = GROUP_NONE, 
			const QString& splitter = SPLITTER_COMMMA);


	public://some helper functions

		void appendOtherProperties(const Properties& other);

		void deleteValue(const QString& key);

		void deleteGroup(const QString& group);

		QString getSourceFile() const;

	private:
		static QString getGroupNameOfKey(const QString& key);

	private:
		static QString combineKeyAndGroup(const QString& key, 
			const QString& group);


	private:
		struct PropertiesImpl;
		QSharedPointer<PropertiesImpl> impl_;
	};


	inline QTextStream&
	operator<<(QTextStream& out, const Properties& props)
	{
		return props.print(out);
	}
}

#endif // Properties_h__

