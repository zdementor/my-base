//|-------------------------------------------------------------------------
//| File:        IMemStat.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMemStatHPP
#define IMemStatHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

class IMemStatus : public IUnknown, public ISingleton <IMemStatus>
{
public:
	static IMemStatus& getSingleton();
	static IMemStatus* getSingletonPtr();

	virtual u32 getAllocatedBytes() = 0;
	virtual u32 getAllocsPerSec() = 0;
	virtual u32 getDeallocsPerSec() = 0;
	virtual u32 getAllocBytesPerSec() = 0;
	virtual u32 getDeallocBytesPerSec() = 0;
};

//---------------------------------------------------------------------------

__MY_OS_LIB_API__ os::IMemStatus* createMemStatus();

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//-----------------------------------------------------------------------------

#endif