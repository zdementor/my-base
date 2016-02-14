//|-------------------------------------------------------------------------
//| File:        CThread.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CThreadHPP
#define CThreadHPP
//---------------------------------------------------------------------------

#include <os/IThread.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with callback Thread
class CThread : public IThread
{
protected:

    CThread(u32 (__stdcall *thread_proc)(void*), u32 interval);
    virtual ~CThread();

private:

	E_THREAD_STATUS m_Status;
	E_THREAD_PRIORITY m_Priority;

	u32 m_Period;

	HANDLE m_ThreadHandle;

	u32 (__stdcall *m_ThreadProcPtr) (void *);

public:

	//! sleep thread (time in miliseconds)
	virtual void sleep(u32 ms);

	//! returns current Thread status
	virtual E_THREAD_STATUS getStatus();

	//! run Thread
	virtual bool run();

	//! stop Thread
	virtual bool stop();	

	//! setting priority of the thread
	virtual void setPriority(E_THREAD_PRIORITY priority);

	//! returns priority of the thread
	virtual E_THREAD_PRIORITY getPriority();

	//! setting  thread main func run period
	virtual void setRunPeriod(u32 msec)
	{ m_Period = msec; }

	//! returns  thread main func run period
	virtual u32 getRunPeriod()
	{ return m_Period; }

};

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // CThreadHPP