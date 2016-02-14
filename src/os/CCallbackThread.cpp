//|-------------------------------------------------------------------------
//| File:        CCallbackThread.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CCallbackThread.h"
#include <io/ILogger.h>
#include <os/ITimer.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

u32 CCallbackThread::TimerThreadProc(void* object) throw()
{
	CCallbackThread* callback_thread = reinterpret_cast<CCallbackThread*>(object);

	if (callback_thread && callback_thread->getRunnable())
	{
		s32 start_time, end_time, sleep_time;
		f64 start_time_sec;

		while (callback_thread->getStatus() != ETRDS_STOPPING)
		{
			start_time     = TIMER_SECOND.getSystemTime();
			start_time_sec = TIMER_SECOND.getSystemTimeSec();

			callback_thread->getRunnable()->run(NULL);	

			end_time = TIMER_SECOND.getSystemTime();  

			sleep_time = callback_thread->getRunPeriod() - (end_time - start_time);
			if (sleep_time > 0)
				callback_thread->sleep(sleep_time);
		}
	}

	return 0;
}

//---------------------------------------------------------------------------

u32 CCallbackThread::ComputationThreadProc(void* object) throw()
{
	CCallbackThread *callback_thread = reinterpret_cast<CCallbackThread*>(object);
	if (callback_thread && callback_thread->getRunnable())
	{
		while(callback_thread->getStatus() != ETRDS_STOPPING)
			callback_thread->getRunnable()->run(NULL);	
	}
	return 0;
}

//---------------------------------------------------------------------------

CCallbackThread::CCallbackThread(IRunnable* runnable, s32 interval)
	: CThread(TimerThreadProc, interval),
Runnable(runnable),ThreadType(ETRT_TIMER)
{
#if MY_DEBUG_MODE  
	setClassName("CCallbackThread");
#endif 
	setPriority(ETRDP_NORMAL);
}

//---------------------------------------------------------------------------

CCallbackThread::CCallbackThread(IRunnable* runnable)
	: CThread(ComputationThreadProc, 0),
Runnable(runnable), ThreadType(ETRT_COMPUTATION)
{
#if MY_DEBUG_MODE  
	setClassName("CCallbackThread");
#endif
	setPriority(ETRDP_NORMAL);
}

//---------------------------------------------------------------------------

CCallbackThread::~CCallbackThread()
{	
	Runnable = NULL;
}

//---------------------------------------------------------------------------

IRunnable* CCallbackThread::getRunnable()
{
	return Runnable;
}

//---------------------------------------------------------------------------

__MY_EXPORT__ os::IThread* createTimerThread(
	IRunnable* runnable, s32 interval)
{
	return new os::CCallbackThread(runnable, interval);
}

//---------------------------------------------------------------------------

__MY_EXPORT__ os::IThread* createComputationThread(IRunnable *runnable)
{
	return new os::CCallbackThread(runnable);
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------
