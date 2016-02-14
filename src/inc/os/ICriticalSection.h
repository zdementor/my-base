//|-------------------------------------------------------------------------
//| File:        ICriticalSection.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ICriticalSectionHPP
#define ICriticalSectionHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with Critical Section class
class __MY_OS_LIB_API__ ICriticalSection : public IUnknown
{
public:

	//! function waits for ownership of the specified critical section object
	virtual void enter() = 0;

	//! function releases ownership of the specified critical section object
	virtual void leave() = 0;	
};

//---------------------------------------------------------------------------

__MY_OS_LIB_API__ os::ICriticalSection* createCriticalSection();

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // ICriticalSectionHPP