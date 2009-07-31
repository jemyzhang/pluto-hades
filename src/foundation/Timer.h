/*******************************************************************************
**
**  NAME:			Timer.h	
**	VER: 			1.0
**  CREATE DATE: 	06/03/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Provide Timer and GroupableTimer to profile program's performance.
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#ifndef FOUNDATION_TIMER_H
#define FOUNDATION_TIMER_H
#include "foundation_global.h"

namespace foundation
{


/** A step-able timer, can record each step's used time in a
	long processing procedure. */
class FOUNDATION_EXPORT Timer
{
public:
	/** Create a timer with input name. */
	static Timer createTimer(const QString&);

	/** Get timer by name, if the timer is not exist, 
		a new one will be created.
		Reset flag is used to restart the timer or not.*/
	static Timer getTimer(const QString&, bool reset = false);

	/**	Has this timer?  */
	static bool hasTimer(const QString&);

	/**	Remove a timer with input name.  */
	static void removeTimer(const QString&);

public:
	Timer();
	~Timer();

	/*!	Equal? Use Reference Semantic, Whether Hold the Same Object or Not */
	bool operator== (const Timer& other) const;

	/*!	Not Equal? */
	bool operator!= (const Timer& other) const;

	/** Reset. */
	void reset();

	/** Trigger the timer to record current step. */
	void trigger();

	/** Trigger with a name. */
	void trigger(const QString&);

	/** Get timer's name. */
	QString name() const;

	/** Total elapsed time. */
	double total() const;
	
	/** Get step counts. */
	int stepCount() const;

	/** Get step's used time by index. */
	double stepTime(int) const;
	
	/** Get step's name by index. */
	QString stepName(int) const;

	/** Last step's used time. */
	double lastStepTime() const;
	
	/** Last step's name. */
	QString lastStepName() const;

	/** Print out. */
	QTextStream& print(QTextStream& out) const;
	QString toString() const;

private:
	Timer(const QString&);

	struct TimerImpl;
	QSharedPointer<TimerImpl> impl_;
};


inline QTextStream&
operator<<(QTextStream& out, const Timer& timer)
{
	return timer.print(out);
}
/************************************************************************
** 	                                                                
**	Provide for convenient, start when it construct,
**	when it destroy, it calculate the time and output to the logger
**                                                             
************************************************************************/

class FOUNDATION_EXPORT GuardTimer
{
public:
	GuardTimer(const QString& timerName, 
		const QString& loggerName = DEFAULT_LOGGER);
	~GuardTimer() throw();

private:
	static const QString DEFAULT_LOGGER;

private:
	QString		name_;
	QString		logger_;
};


class FOUNDATION_EXPORT AvgGuardTimer
{
public:
	AvgGuardTimer(const QString& timerName, 
		int interval = 30,
		const QString& loggerName = DEFAULT_LOGGER);

	~AvgGuardTimer() throw();

	double avgTime();

private:
	static const QString DEFAULT_LOGGER;

private:
	int			interval_;
	QString		name_;
	QString		logger_;
	QTime		time_;
};


#define __AVG_TIME__ foundation::AvgGuardTimer avgGuardTimer(__FUNCTION__);

/** A groupable timer, can record each step's used time,
	and put them into different group sequential.*/
class FOUNDATION_EXPORT GroupableTimer
{
public:
	/** Create a timer with input name and an array of group names.*/
	static GroupableTimer createTimer(const QString&, const QStringList&);

	/** Get timer by name, if the timer is not exist, 
		a new one will be created.
		Reset flag is used to restart the timer or not.*/
	static GroupableTimer getTimer(const QString&, bool reset = false);

	/**	Has this timer?  */
	static bool hasTimer(const QString&);

	/**	Remove a timer with input name.  */
	static void removeTimer(const QString&);

public:
	GroupableTimer();
	~GroupableTimer();

	/*!	Equal? Use Reference Semantic, Whether Hold the Same Object or Not */
	bool operator== (const GroupableTimer& other) const;

	/*!	Not Equal? */
	bool operator!= (const GroupableTimer& other) const;

	/**	Add group.  */
	void addGroup(const QString&);

	/**	Insert group on selected position.  */
	void insertGroup(int, const QString&);

	/**	Remove group by name or by index.  */
	void removeGroup(const QString&);
	void removeGroup(int);

	/** Reset. */
	void reset();

	/** Trigger the timer to record current step. */
	void trigger();

	/** Get timer's name. */
	QString name() const;

	/** Total elapsed time. */
	double total() const;

	/**	Current active group.  */
	int activeGroup() const;

	/** Get group counts. */
	int groupCount() const;

	/** Get each group's used time by index. */
	double groupTime(int) const;

	/** Get each group's name by index. */
	QString groupName(int) const;

	/** Get each group's step counts by group index. */
	int stepCount(int) const;

	/** Get each step's used time by group index and step index. */
	double stepTime(int, int) const;

	/** To QString and print out. */
	QTextStream& print(QTextStream& out, bool verbose = false) const;
	QString toString(bool verbose = false) const;

private:
	GroupableTimer(const QString&, const QStringList&);

	struct GroupableTimerImpl;
	QSharedPointer<GroupableTimerImpl>	impl_;
};


inline QTextStream&
operator<<(QTextStream& out, const GroupableTimer& gtimer)
{
	return gtimer.print(out);
}

//End foundation
}


#endif
