//|-------------------------------------------------------------------------
//| File:        CCriticalSection.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CCriticalSection.h"

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! constructor
CCriticalSection::CCriticalSection()
{
#if MY_DEBUG_MODE  
	setClassName("CCriticalSection");
#endif

	InitializeCriticalSection(&CriticalSection);
}

//---------------------------------------------------------------------------

//! destructor
CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(&CriticalSection);
}

//---------------------------------------------------------------------------

//! function waits for ownership of the specified critical section object
void CCriticalSection::enter()
{
	EnterCriticalSection(&CriticalSection);
}

//---------------------------------------------------------------------------

//! function releases ownership of the specified critical section object
void CCriticalSection::leave()
{
	LeaveCriticalSection(&CriticalSection);
}

//---------------------------------------------------------------------------

//! creating critical section object
__MY_EXPORT__ os::ICriticalSection* createCriticalSection()
{
	return new os::CCriticalSection();
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------