//|-------------------------------------------------------------------------
//| File:        CTimer.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CTimer.h"

#if MY_PLATFORM == MY_PLATFORM_LINUX
	#include <time.h>
	#include <sys/time.h>
#endif // #if MY_PLATFORM == MY_PLATFORM_LINUX

#include <dev/IDevice.h>
#include <os/ICriticalSection.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <os/ITimer.h>

//-----------------------------------------------------------------------------
namespace my {
namespace os {
//-----------------------------------------------------------------------------

static MY_FORCEINLINE u32 _getSystemTime(bool &hiperf, f64 &hiperftoms)
{	
	u32 ms = 0;
#if MY_PLATFORM == MY_PLATFORM_WIN32

	if (hiperf)
    {
		LARGE_INTEGER nTime;
        QueryPerformanceCounter(&nTime);
        ms = u32((nTime.QuadPart) * hiperftoms);
    }
	else
		ms = GetTickCount();	

#elif  MY_PLATFORM == MY_PLATFORM_LINUX

	static timeval tv;
    gettimeofday(&tv, 0);
    ms = (u32)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);

#endif // #else if  MY_PLATFORM == MY_PLATFORM_LINUX
	return ms;
}

static MY_FORCEINLINE f64 _getSystemTimeSec(bool &hiperf, f64 &hiperftosec)
{
	f64 sec;
#if MY_PLATFORM == MY_PLATFORM_WIN32

	if (hiperf)
    {
		LARGE_INTEGER nTime;
        QueryPerformanceCounter(&nTime);
        sec = f64(nTime.QuadPart * hiperftosec);
    }
	else
	{
		sec = f32(GetTickCount()) / 1000.0f;
	}

#elif  MY_PLATFORM == MY_PLATFORM_LINUX

#endif // #else if  MY_PLATFORM == MY_PLATFORM_LINUX
	return sec;
}

CTimer::CTimer(c8 *TimerCaption)
{
#if MY_DEBUG_MODE  
	setClassName("CTimer");
#endif

	m_HighPerformanceTimerSupport = false;
	m_StopTimeMs =0;
	m_DeltaTimeMs =0;
	m_StopTimeSec =0;
	m_DeltaTimeSec =0;
	m_TimeStopped = false;
	m_PerformanceUnit =0;
	m_TimerStartMs = 0;
	m_TimerStartSec = 0;

	// init timer

#if MY_PLATFORM == MY_PLATFORM_WIN32	

	LARGE_INTEGER HighPerformanceFreq;

	m_HighPerformanceTimerSupport = QueryPerformanceFrequency(&HighPerformanceFreq)==0 ? false : true;
	m_PerformanceUnit = 1.0f / f64(HighPerformanceFreq.QuadPart);
	m_PerformanceCounterToMs = 1000.0f / HighPerformanceFreq.QuadPart;

	LOGGER.logInfo("%s initialization:", TimerCaption);

	if (!m_HighPerformanceTimerSupport || HighPerformanceFreq.QuadPart<1000)
	{	
		LOGGER.logWarn(" Insufficient performance query resolution on this system!");
	}  
	else
	{
		LOGGER.logInfo(" Query resolution 1/%u sec", HighPerformanceFreq.QuadPart);
		LOGGER.logInfo("Timer OK");
	}	
#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32

	m_TimerStartMs =  _getSystemTime(m_HighPerformanceTimerSupport, m_PerformanceCounterToMs);
	m_TimerStartSec = _getSystemTimeSec(m_HighPerformanceTimerSupport, m_PerformanceUnit);
}

//---------------------------------------------------------------------------

CTimer::~CTimer()
{
}

//---------------------------------------------------------------------------

u32 CTimer::getSystemTime()
{	
	return _getSystemTime(m_HighPerformanceTimerSupport, m_PerformanceCounterToMs) - m_TimerStartMs;
}

//---------------------------------------------------------------------------

u32 CTimer::getTime()
{
	if (m_TimeStopped)
		return m_StopTimeMs - m_DeltaTimeMs;
	return getSystemTime() - m_DeltaTimeMs;	
}

//---------------------------------------------------------------------------

f64 CTimer::getSystemTimeSec()
{
	return _getSystemTimeSec(m_HighPerformanceTimerSupport, m_PerformanceUnit) - m_TimerStartSec;
}

//---------------------------------------------------------------------------

f64 CTimer::getTimeSec()
{
	if (m_TimeStopped)
		return m_StopTimeSec - m_DeltaTimeSec;
	return getSystemTimeSec() - m_DeltaTimeSec;
}

//---------------------------------------------------------------------------

void CTimer::suspend()
{
	if (!m_TimeStopped)
	{
		m_StopTimeMs  = getSystemTime();
		m_StopTimeSec = getSystemTimeSec();

		m_TimeStopped = true;
	}
}

//---------------------------------------------------------------------------

void CTimer::resume()
{
	if (m_TimeStopped)
	{	
		m_DeltaTimeMs  += getSystemTime()-m_StopTimeMs;
		m_DeltaTimeSec += getSystemTimeSec()-m_StopTimeSec;

		m_TimeStopped = false;
	}
}

//---------------------------------------------------------------------------

template <class T>
class TimerTemplate : public T
{
public:

	TimerTemplate(c8 *TimerCaption)
	{
#if MY_DEBUG_MODE  
		setClassName(TimerCaption);
#endif
		timer = new CTimer(TimerCaption);
	}

	virtual ~TimerTemplate()
	{ SAFE_DROP(timer); }

	virtual u32 getSystemTime()
	{ return timer->getSystemTime(); }

    virtual u32 getTime()
	{ return timer->getTime(); }

	virtual f64 getSystemTimeSec()
	{ return timer->getSystemTimeSec(); }

	virtual f64 getTimeSec()
	{ return timer->getTimeSec(); }

	virtual void suspend()
	{ timer->suspend(); }

	virtual void resume()
	{ timer->resume(); }

	virtual bool isSuspended()
	{ return timer->isSuspended(); }

private:

	CTimer *timer;
};

//---------------------------------------------------------------------------

__MY_EXPORT__ os::ITimer* createMainTimer()
{
	return new os::TimerTemplate<os::ITimerMain>("Primary Timer");
}

//---------------------------------------------------------------------------

__MY_EXPORT__ os::ITimer* createSecondTimer()
{
	return new os::TimerTemplate<os::ITimerSecond>("Secondary Timer");
}

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//-----------------------------------------------------------------------------

