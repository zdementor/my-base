//|-------------------------------------------------------------------------
//| File:        CCriticalSection.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCriticalSectionHPP
#define CCriticalSectionHPP
//---------------------------------------------------------------------------
#include <os/ICriticalSection.h>
//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------
//! Interface for working with CCriticalSection class
class CCriticalSection : public ICriticalSection
{
public:

	//! constructor
	CCriticalSection();

	//! destructor
	virtual ~CCriticalSection();

	//! function waits for ownership of the specified critical section object
	virtual void enter();

	//! function releases ownership of the specified critical section object
	virtual void leave();	
	
private:

	// critical section object
	CRITICAL_SECTION CriticalSection;
};
//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------
#endif // CCriticalSectionHPP