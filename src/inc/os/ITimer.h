//|-------------------------------------------------------------------------
//| File:        ITimer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITimerHPP
#define ITimerHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for getting the current time
class __MY_OS_LIB_API__ ITimer : public IUnknown
{
public:
    //! returns current time in milliseconds. This value does not start
    //! with 0 when the application starts. For example in one implementation
    //! the value returne could be the amount of milliseconds which
    //! have elapsed since the system was started.
	//! This value increasing, even if timer stopped.
	virtual u32 getSystemTime() = 0;

    //! returns current time in milliseconds. This value does not start
    //! with 0 when the application starts. For example in one implementation
    //! the value returne could be the amount of milliseconds which
    //! have elapsed since the system was started.
	//! This value does not increasing, when timer stopped.
    virtual u32 getTime() = 0;

	//! returns current time in seconds.
	virtual f64 getSystemTimeSec() = 0;

	//! returns current time in seconds.
	virtual f64 getTimeSec() = 0;

	//! suspending timer
	virtual void suspend() = 0;

	//! resuming timer
	virtual void resume() = 0;

	//! returns true if timer is in suspended mode
	virtual bool isSuspended() = 0;
};

//---------------------------------------------------------------------------

//! Interface for getting the current time (Main timer)
class __MY_OS_LIB_API__ ITimerMain :
	public ITimer, public ISingleton <ITimerMain>
{
public:
 	//!	Return singleton ITimerMain object
	static ITimer& getSingleton();

	//! Return pointer to singleton ITimerMain object
	static ITimer* getSingletonPtr();
};

//---------------------------------------------------------------------------

//! Interface for getting the current time (Additional timer)
class __MY_OS_LIB_API__ ITimerSecond :
	public ITimer, public ISingleton <ITimerSecond>
{
public:
	//!	Return singleton ITimerSecond object
	static ITimer& getSingleton();

	//! Return pointer to singleton ITimerSecond object
	static ITimer* getSingletonPtr();
};

//---------------------------------------------------------------------------

__MY_OS_LIB_API__ os::ITimer* createMainTimer();
__MY_OS_LIB_API__ os::ITimer* createSecondTimer();

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif

