//|-------------------------------------------------------------------------
//| File:        CEvent.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CEvent.h"
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! constructor
CEvent::CEvent(bool manualReset)
{
#if MY_DEBUG_MODE  
	setClassName("CEvent");
#endif

	// The CreateEvent function creates a named or unnamed event object. 
	EventHandle = CreateEvent(0, manualReset ? TRUE : FALSE, FALSE, 0);

    if(!EventHandle)
	{
		LOGGER.log("CEvent::CEvent()", "CreateEvent failed", io::ELL_ERROR);
	}
}

//---------------------------------------------------------------------------

//! destructor
CEvent::~CEvent()
{
	if(EventHandle)
    {
		// The CloseHandle function closes an open object handle. 
        CloseHandle(EventHandle);

        EventHandle = 0;
    }
}

//---------------------------------------------------------------------------

//! function waits for ownership of the specified event object
void CEvent::wait()
{
	if(EventHandle)
    {
		WaitForSingleObject(EventHandle, INFINITE);
	}
}

//---------------------------------------------------------------------------

//! The SetEvent function sets the state of the specified event object to signaled. 
void CEvent::set()
{
	if(EventHandle)
    {
		if(!SetEvent(EventHandle))
		{
			LOGGER.log("CEvent::CEvent()", "SetEvent failed", io::ELL_ERROR);
		}
	}
}

//---------------------------------------------------------------------------

//! The ResetEvent function sets the state of the specified event object to nonsignaled.
void CEvent::reset()
{
	if(EventHandle)
    {
		if(!ResetEvent(EventHandle))
		{
			LOGGER.log("CEvent::CEvent()", "ResetEvent failed", io::ELL_ERROR);
		}
	}
}

//---------------------------------------------------------------------------

//! creating os event object
__MY_EXPORT__ os::IEvent* createEvent(bool manualReset)
{
	return new os::CEvent(manualReset);
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------