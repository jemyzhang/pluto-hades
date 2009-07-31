/*******************************************************************************
**
**  NAME:			Exception.cpp	
**	VER: 			1.0
**  CREATE DATE: 	06/03/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Exception implementation
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Exception.h"

namespace foundation
{

Exception::Exception(const QString& msg, 
					 const QString& file, 
					 const QString& func, 
					 int ln) throw()
					 : std::exception(qPrintable(msg))
					 , msg_(msg)
					 , file_(file)
					 , function_(func)
					 , line_(ln)
{
}


Exception::Exception(const QString& msg,
					 const QVariant& param, 
					 const QString& file, 
					 const QString& func, 
					 int ln) throw()
					 : std::exception(qPrintable(msg))
					 , msg_(msg)
					 , param_ (param)
					 , file_(file)
					 , function_(func)
					 , line_(ln)
{

}



Exception::Exception(const Exception& prev,
					 const QString& msg, 
					 const QString& file, 
					 const QString& func, 
					 int ln) throw()
					 : std::exception(qPrintable(msg))
					 , msg_(msg)
					 , history_(prev.toString())
					 , file_(file)
					 , function_(func)
					 , line_(ln)
{
}


Exception::Exception(const std::exception& e) throw()
: std::exception(e.what())
{
}



Exception::Exception() throw()
{

}



Exception::~Exception() throw() {}



QTextStream& 
Exception::print(QTextStream& out) const
{
	out<<this->toString();

	return out;
}


QString 
Exception::toString() const
{
	QString detail = QString("(%1) %2 {%3} [%4 - %5 (%6)]")
		.arg(typeid(*this).name())
		.arg(this->msg_, this->param_.toString(), this->file_, this->function_)
		.arg(line_);

	if (history_.size())
	{
		detail = QString("%1\r\n---------- PREVIOUS EXCEPTION ----------\r\n%2")
			.arg(detail)
			.arg(history_);
	}

	return detail;
}


QString 
Exception::message() const
{
	return this->msg_;
}


QVariant 
Exception::parameter() const
{
	return this->param_;
}


//End namespace
}

