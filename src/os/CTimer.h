//|-------------------------------------------------------------------------
//| File:        CTimer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTimerHPP
#define CTimerHPP
//-----------------------------------------------------------------------------

#include <os/ITimer.h>

//-----------------------------------------------------------------------------
namespace my {
namespace os {
//-----------------------------------------------------------------------------

//! Device independent implementation of the timer
class CTimer : public ITimer
{
public:

    CTimer(c8 *TimerCaption="");
	virtual ~CTimer();

	virtual u32 getSystemTime();
    virtual u32 getTime();

	virtual f64 getSystemTimeSec();
	virtual f64 getTimeSec();

	virtual void suspend();
	virtual void resume();

	virtual bool isSuspended()
	{ return m_TimeStopped; }

public:		

	bool m_HighPerformanceTimerSupport;
	u32 m_StopTimeMs, m_DeltaTimeMs;
	f64 m_StopTimeSec, m_DeltaTimeSec;
	bool m_TimeStopped;
	f64 m_PerformanceUnit;
	f64 m_PerformanceCounterToMs;

	u32 m_TimerStartMs;
	f32 m_TimerStartSec;
};

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//-----------------------------------------------------------------------------

#endif
