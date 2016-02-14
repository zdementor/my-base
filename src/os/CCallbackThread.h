//|-------------------------------------------------------------------------
//| File:        CCallbackThread.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCallbackThreadHPP
#define CCallbackThreadHPP
//---------------------------------------------------------------------------

#include "../inc/IRunnable.h"
#include "CThread.h"

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with callback Thread
class CCallbackThread : public CThread
{

private:

	IRunnable *Runnable;

    static u32 __stdcall TimerThreadProc      (void* object);
	static u32 __stdcall ComputationThreadProc(void* object);

	E_THREAD_TYPE ThreadType;

public:

    CCallbackThread(IRunnable* runnable, s32 interval);
    CCallbackThread(IRunnable* runnable);

    virtual ~CCallbackThread();

	IRunnable* getRunnable();
};

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // CCallbackThreadHPP