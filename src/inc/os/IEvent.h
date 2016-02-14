//|-------------------------------------------------------------------------
//| File:        IEvent.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IEventHPP
#define IEventHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with Event class
class __MY_OS_LIB_API__ IEvent : public IUnknown
{
public:	

	//! function waits for ownership of the specified event object
	virtual void wait() = 0;

	//! The SetEvent function sets the state of the specified event object to signaled. 
	virtual void set() = 0;	

	//! The ResetEvent function sets the state of the specified event object to nonsignaled.
	virtual void reset() = 0;
};

//---------------------------------------------------------------------------

__MY_OS_LIB_API__ os::IEvent* createEvent(bool manualReset);

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IEventHPP