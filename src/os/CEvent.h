//|-------------------------------------------------------------------------
//| File:        CEvent.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CEventHPP
#define CEventHPP
//---------------------------------------------------------------------------

#include <os/IEvent.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with Event class
class __MY_SCN_LIB_API__ CEvent : public IEvent
{
public:	

	//! constructor
	CEvent(bool manualReset=false);

	//! destructor
	virtual ~CEvent();

	//! function waits for ownership of the specified event object
	virtual void wait();

	//! The SetEvent function sets the state of the specified event object to signaled. 
	virtual void set();	

	//! The ResetEvent function sets the state of the specified event object to nonsignaled.
	virtual void reset();

private:

#if MY_PLATFORM == MY_PLATFORM_WIN32
	HANDLE EventHandle;
#endif

};

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CEventHPP