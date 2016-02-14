//|-------------------------------------------------------------------------
//| File:        CThread.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CThread.h"
#include <io/ILogger.h>
//---------------------------------------------------------------------------

#include <process.h>
#include <windows.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

CThread::CThread(u32 (__stdcall *thread_proc)(void*), u32 interval)
	: m_ThreadProcPtr(thread_proc),
m_ThreadHandle(NULL), m_Status(ETRDS_STOPPED), m_Priority(ETRDP_NORMAL),
m_Period(interval)
{
#if MY_DEBUG_MODE  
	setClassName("CThread");
#endif
}

//---------------------------------------------------------------------------

CThread::~CThread()
{	
	stop();	
}

//---------------------------------------------------------------------------

void CThread::sleep(u32 ms)
{
	Sleep(ms);
}

//---------------------------------------------------------------------------

E_THREAD_STATUS CThread::getStatus()
{
	return m_Status;
}

//---------------------------------------------------------------------------

bool CThread::run()
{
	if (m_ThreadHandle) return true;

    // create the thread
    unsigned long threadId = 0;
/*
    m_ThreadHandle = reinterpret_cast<HANDLE>(
		_beginthreadex(
			0, 
			0, 
			m_ThreadProcPtr, 
			this, 
			CREATE_SUSPENDED, 
			&threadId
			)
		);*/
	
	m_ThreadHandle = CreateThread(
		0, // SD
		0,                        // initial stack size
		(unsigned long (__stdcall *)(void *))m_ThreadProcPtr,    // thread function
		this,                       // thread argument
		CREATE_SUSPENDED,                    // creation option
		&threadId                        // thread identifier
		);

	bool res = false;

    if(!m_ThreadHandle)
	{
		LOGGER.log("CThread::CThread: _beginthreadex failed");
	}
	else
	{
		setPriority(m_Priority);

		ResumeThread(m_ThreadHandle);

		res = true;
	}

	m_Status = ETRDS_RUNNING;

	return res;
}

//---------------------------------------------------------------------------

bool CThread::stop()
{
	m_Status = ETRDS_STOPPING;

    if(m_ThreadHandle)
    {
        // if the thread is still running then terminate it
        if (WaitForSingleObject(m_ThreadHandle, 5000) == WAIT_TIMEOUT)
		{
			LOGGER.logWarn("Thread %p not exited normally, terminating...", m_ThreadHandle);
            TerminateThread(m_ThreadHandle, 1);
		}

        // close the handle
        CloseHandle(m_ThreadHandle);

        m_ThreadHandle = 0;
    }

	m_Status = ETRDS_STOPPED;

	return true;
}

//---------------------------------------------------------------------------

void CThread::setPriority(E_THREAD_PRIORITY priority)
{
	m_Priority = priority;

	int nm_Priority; 	// thread priority level 

	if (priority == ETRDP_HIGH)
	{
		nm_Priority = THREAD_PRIORITY_TIME_CRITICAL;
	}
	else if (priority == ETRDP_LOW)
	{
		nm_Priority = THREAD_PRIORITY_IDLE;
	}
	else 
	{
		nm_Priority = THREAD_PRIORITY_NORMAL;
	}

	if (m_ThreadHandle)
		SetThreadPriority( m_ThreadHandle, nm_Priority );	
}

//---------------------------------------------------------------------------

E_THREAD_PRIORITY CThread::getPriority()
{
	return m_Priority;
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------
