//|-------------------------------------------------------------------------
//| File:        IThread.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IThreadHPP
#define IThreadHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with callback Thread
class __MY_OS_LIB_API__ IThread : public IUnknown
{
public:

    //! destructor
    virtual ~IThread() {}

	//! returns current Thread status
	virtual E_THREAD_STATUS getStatus() = 0;

	//! sleep thread (time in miliseconds)
	virtual void sleep(u32 ms) = 0;

	//! run Thread
	virtual bool run() = 0;

	//! stop Thread
	virtual bool stop() = 0;

	//! setting priority of the thread
	virtual void setPriority(E_THREAD_PRIORITY priority) = 0;

	//! returns priority of the thread
	virtual E_THREAD_PRIORITY getPriority() = 0;

	//! setting  thread main func run period
	virtual void setRunPeriod(u32 msec) = 0;

	//! returns  thread main func run period
	virtual u32 getRunPeriod() = 0;
};

//---------------------------------------------------------------------------

//! creating callbackable thread
__MY_OS_LIB_API__ os::IThread* createTimerThread(
	IRunnable* runnable, s32 interval);

//! creating callbackable thread
__MY_OS_LIB_API__ os::IThread* createComputationThread(
	IRunnable* runnable);

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif