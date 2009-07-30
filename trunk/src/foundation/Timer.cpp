/*******************************************************************************
**
**  NAME:			Timer.cpp	
**	VER: 			1.0
**  CREATE DATE: 	06/03/2007
**  AUTHOR:			Roger
**  
**  Copyright (C) 2007 - PlutoWare, All Rights Reserved
** 
**	
**	PURPOSE:	Timer and GroupableTimer's implementation
**
**  --------------------------------------------------------------
**
**	HISTORY:
**
**
*******************************************************************************/
#include "stdafx.h"

#include "Timer.h"
#include "Logger.h"


namespace foundation
{
/**	Timer and GroupableTimer's store  */
struct Step
{
	Step() : time(0)
	{
	};

	Step(const QString& aName)
		: name(aName)
		, time(0)
	{
	}

	Step(const QString& aName, double aTime)
		: name(aName)
		, time(aTime)
	{
	}

	QString name;
	double time;
};

typedef QVector<Step> StepArray;

struct Group
{
	Group() : total(0)
	{
	};

	Group(const QString& aName)
		: name(aName)
		, total(0)
	{
	}

	QString name;
	StepArray steps;
	double total;
};


typedef QVector<Group> GroupArray;

static QMap<QString, Timer>					TimerStore_;
static QMap<QString, GroupableTimer>		GroupableTimerStore_;


/*******************************************************************************
** 	                                                                
** Timer Methods                                                               
**                                                             
*******************************************************************************/

/*!	\brief Private implementation of Timer */
struct Timer::TimerImpl
{
	QString				name;
	QTime				timer;
	StepArray			record;
	double				total;
};


Timer 
Timer::createTimer(const QString& name)
{
	Timer::removeTimer(name);
	Timer timer(name);

	TimerStore_.insert(name, timer);
	
	return timer;
}


void
Timer::removeTimer(const QString& name)
{
	TimerStore_.remove(name);
}


Timer 
Timer::getTimer(const QString& name, bool reset)
{
	if (TimerStore_.contains(name))
	{
		if (reset)
		{
			TimerStore_[name].reset();
		}

		return TimerStore_[name];
	}
	else
	{
		//Cannot find any exist timer for the given name, create a new one
		return Timer::createTimer(name);
	}		
}



bool 
Timer::hasTimer(const QString& name)
{
	return TimerStore_.contains(name);
}



Timer::Timer(const QString& name)
: impl_(new TimerImpl())
{
	impl_->name = name;
	impl_->timer.start();
	impl_->total = 0;
}



Timer::Timer()
{

}


Timer::~Timer()
{
}


bool 
Timer::operator== (const Timer& other) const
{
	return this->impl_ == other.impl_;
}


bool 
Timer::operator!= (const Timer& other) const
{
	return !operator==(other);
}


void 
Timer::reset()
{
	impl_->timer.restart();
	impl_->record.clear();
	impl_->total = 0;
}


void 
Timer::trigger() 
{
	trigger(QString("S%1").arg(impl_->record.size() + 1));
}


void 
Timer::trigger(const QString& name)
{
	double elapsed = impl_->timer.elapsed() / 1000.0;

	impl_->record.push_back(Step(name, elapsed - impl_->total));
	
	impl_->total = elapsed;
}


QString 
Timer::name() const {return impl_->name;}


double 
Timer::total() const {return impl_->total;}


int 
Timer::stepCount() const 
{
	return impl_->record.size();
}


double 
Timer::stepTime(int idx) const
{
	if (idx < 0 || idx > this->stepCount())
		return DBL_MIN;
	else
		return impl_->record[idx].time;
}


QString 
Timer::stepName(int idx) const
{
	if (idx < 0 || idx > this->stepCount())
		return "";
	else
		return impl_->record[idx].name;
}


double
Timer::lastStepTime() const 
{
	return stepTime(impl_->record.size() - 1);
}


QString 
Timer::lastStepName() const  
{
	return stepName(impl_->record.size() - 1);
}


QString 
Timer::toString() const
{
	QString detail;
	QTextStream ts(&detail);
	
	ts<<QString("[%1 - %2 - %3s] --")
		.arg(this->name())
		.arg(this->stepCount())
		.arg(this->total(), 7, 'f', 3);

	foreach(const Step& step, impl_->record)
	{
		ts<<QString(" %1 - %2s;").arg(step.name).arg(step.time, 7, 'f', 3);
	}

	return detail;
}


QTextStream& 
Timer::print(QTextStream& out) const
{
	out<<this->toString();

	return out;
}

/************************************************************************
** 	                                                                
** Guard Timer's Methods                                                               
**                                                             
************************************************************************/
const QString GuardTimer::DEFAULT_LOGGER = PROFILING_LOGGER;

GuardTimer::GuardTimer(const QString& timerName, const QString& loggerName)
: name_(timerName)
, logger_(loggerName)
{
	Timer::createTimer(name_);
}

GuardTimer::~GuardTimer() throw()
{
	Timer timer = Timer::getTimer(name_);
	timer.trigger();
	__LOG_L(logger_, timer.toString());
	Timer::removeTimer(name_);
}

/************************************************************************
** 	                                                                
** Groupable Timer's Methods                                                               
**                                                             
************************************************************************/
struct GroupableTimer::GroupableTimerImpl
{
	QString				name;
	QTime				timer;
	double				total;

	int					activeGroup;
	int					groupCount;

	GroupArray			group;
};


GroupableTimer 
GroupableTimer::createTimer(const QString& name, 
							const QStringList& groupNames)
{
	GroupableTimer::removeTimer(name);

	GroupableTimer gtimer(name, groupNames);
	GroupableTimerStore_.insert(name, gtimer);

	return gtimer;
}


void
GroupableTimer::removeTimer(const QString& name)
{
	GroupableTimerStore_.remove(name);
}


GroupableTimer 
GroupableTimer::getTimer(const QString& name, bool reset)
{
	if (GroupableTimerStore_.contains(name))
	{
		if (reset)
		{
			GroupableTimerStore_[name].reset();
		}

		return GroupableTimerStore_[name];
	}
	else
	{
		//Cann't find any exist timer for the given name, 
		//create a new empty one
		return GroupableTimer::createTimer(name, QStringList());
	}
}


bool 
GroupableTimer::hasTimer(const QString& name)
{
	return GroupableTimerStore_.contains(name);
}


GroupableTimer::GroupableTimer(const QString& name, 
							   const QStringList& groupNames)
							   : impl_(new GroupableTimerImpl())
{
	impl_->name = name;
	impl_->timer.start();
	impl_->total = 0.0;

	impl_->groupCount = static_cast<int>(groupNames.size());
	impl_->activeGroup = (impl_->groupCount ? 0 : -1);

	for (int i = 0; i < impl_->groupCount; ++i)
	{
		Group g(groupNames.at(i));
		impl_->group.push_back(g);
	}
}


GroupableTimer::GroupableTimer()
{

}


GroupableTimer::~GroupableTimer()
{
}


bool 
GroupableTimer::operator== (const GroupableTimer& other) const
{
	return this->impl_ == other.impl_;
}


bool 
GroupableTimer::operator!= (const GroupableTimer& other) const
{
	return !operator==(other);
}


void
GroupableTimer::addGroup(const QString& groupName)
{
	impl_->group.push_back(Group(groupName));
	++impl_->groupCount;
}


void
GroupableTimer::insertGroup(int gidx, const QString& groupName)
{
	if (gidx < impl_->groupCount)
	{
		impl_->group.insert(impl_->group.begin() + gidx, Group(groupName));
		++impl_->groupCount;
	}
}


void 
GroupableTimer::removeGroup(int gidx)
{
	if (gidx < impl_->groupCount)
	{
		impl_->group.erase(impl_->group.begin() + gidx);
		--impl_->groupCount;
	}
}


void 
GroupableTimer::removeGroup(const QString& groupName)
{
	GroupArray::iterator iter = impl_->group.begin();

	for (; iter != impl_->group.end(); ++iter)
	{
		if (iter->name == groupName)
		{
			impl_->group.erase(iter);
			--impl_->groupCount;
			break;
		}
	}
}


void
GroupableTimer::reset()
{
	impl_->total = 0;
	impl_->group.clear();
	impl_->timer.restart();
	impl_->groupCount = 0;
	impl_->activeGroup = -1;
}


void
GroupableTimer::trigger()
{
	if (impl_->groupCount > 0)
	{
		Q_ASSERT(impl_->activeGroup >= 0 && "Active group index wrong!");
	
		double elapsed = impl_->timer.elapsed() / 1000.0;		
		double step = elapsed - impl_->total;

		Group& g = impl_->group[impl_->activeGroup];

		QString name = QString("S%1").arg(g.steps.size() + 1);

		g.steps.push_back(Step(name, step));
		g.total += step;

		++impl_->activeGroup %= impl_->groupCount;
		
		impl_->total = elapsed;
	}

}


QString
GroupableTimer::name() const {return impl_->name;}


double 
GroupableTimer::total() const {return impl_->total;}


int 
GroupableTimer::activeGroup() const {return impl_->activeGroup;}


int
GroupableTimer::groupCount() const {return impl_->groupCount;}


double
GroupableTimer::groupTime(int gidx) const
{
	if (impl_->groupCount > 0 && gidx >= 0 && gidx < impl_->groupCount)
		return impl_->group[gidx].total;
	else
		return 0;
}


QString
GroupableTimer::groupName(int gidx) const
{
	if (impl_->groupCount > 0 && gidx >= 0 && gidx < impl_->groupCount)
		return impl_->group[gidx].name;
	else
		return QString();
}


int 
GroupableTimer::stepCount(int gidx) const
{
	if (impl_->groupCount > 0 && gidx >= 0 && gidx < impl_->groupCount)
		return impl_->group[gidx].steps.size();
	else
		return 0;
}


double
GroupableTimer::stepTime(int gidx, int sidx) const
{
	if (impl_->groupCount > 0 && gidx >= 0 && gidx < impl_->groupCount)
	{
		const Group& group = impl_->group[gidx];
		
		if (sidx >= 0 && sidx < group.steps.size())
		{
			return group.steps[sidx].time;;
		}
	}
		
	return 0;
}


QString 
GroupableTimer::toString(bool verbose /*= false */) const
{
	QString detail;
	QTextStream ts(&detail);
	verbose;

	ts<<QString("[%1 - G%2, S%3 - %4] --")
		.arg(this->name())
		.arg(this->groupCount())
		.arg(this->stepCount(impl_->activeGroup))
		.arg(this->total());

	foreach(const Group& g, impl_->group)
	{
		ts<<QString(" %1 - %2s;").arg(g.name).arg(g.total, 7, 'f', 3);
	}

	if (verbose)
	{
		foreach(const Group& g, impl_->group)
		{
			ts<<QString("\r\nGROUP %1 :\t").arg(g.name);

			int count = 0;
			foreach (const Step& step, g.steps)
			{
				ts<<QString(" %1, %2s;").arg(count++).arg(step.time);
			}
		}
	}

	return detail;
}


QTextStream& 
GroupableTimer::print(QTextStream& out, bool verbose) const
{
	out<<this->toString(verbose);

	return out;
}

static QMap<QString, QPair<double, int>> AvgGuardTimerRecord_;
const QString AvgGuardTimer::DEFAULT_LOGGER = PROFILING_LOGGER;

AvgGuardTimer::AvgGuardTimer(const QString& timerName, 
							 int interval /*= 30*/, 
							 const QString& loggerName /*= DEFAULT_LOGGER*/)
							 : name_(timerName)
							 , logger_(loggerName)
							 , interval_(interval)
{
	time_.start();
}



AvgGuardTimer::~AvgGuardTimer() throw()
{						  
	QPair<double, int>& value = AvgGuardTimerRecord_[this->name_];

	value.first += time_.elapsed();
	++value.second;

	if (value.second % this->interval_ == 0)
	{
		double totalSecs = 1.0 * value.first / 1000;

		__LOG_L(this->logger_, 
			QString("%1 - T: %2s, C: %3, Avg: %4s;")
			.arg(this->name_)
			.arg(totalSecs, 7, 'f', 3)
			.arg(value.second)
			.arg(totalSecs / value.second, 8, 'f', 4));
	}
}


double 
AvgGuardTimer::avgTime()
{
	QPair<double, int>& value = AvgGuardTimerRecord_[this->name_];

	return 1.0 * value.first / (1000 * value.second);

}

}

