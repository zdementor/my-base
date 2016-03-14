//|-------------------------------------------------------------------------
//| File:        CMemStat.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMemStat.h"
#include <os/ITimer.h>

//---------------------------------------------------------------------------
namespace my {
namespace core {
//---------------------------------------------------------------------------

static my::core::hash_table <void*, size_t> _Allocs;
static size_t _AllocBytes = 0;
my::core::math::event_counter _AllocsCounter, _DeallocsCounter;
my::core::math::event_counter _AllocBytesCounter, _DeallocBytesCounter;
static bool _AllocSkip = true;

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void dealloc(void* ptr)
{
	u32 time = 0;
	size_t size = _Allocs.get_value(ptr);

	if (!_AllocSkip)
		time = TIMER.getTime();

	if (_Allocs.erase_value(ptr))
	{
		_AllocBytes -= size;
		_DeallocsCounter.registerEvent(time);
		_DeallocBytesCounter.registerValue(time, (u32)size);
	}

	free(ptr);
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void * alloc(size_t size) throw (...)
{
	u32 time = 0;

	if (!_AllocSkip)
		time = TIMER.getTime();

	void *ptr;
	if ((ptr = malloc(size)) == 0)
	{
		static const std::bad_alloc nomem;
		throw nomem;
	}

	if (!_AllocSkip)
	{
		_AllocSkip = true;
		_Allocs.set_value(ptr, size);
		_AllocSkip = false;

		_AllocBytes += size;

		_AllocsCounter.registerEvent(time);
		_AllocBytesCounter.registerValue(time, (u32)size);
	}

	return (ptr);
}

//-----------------------------------------------------------------------------
} // end namespace core
namespace os {
//-----------------------------------------------------------------------------

CMemStatus::CMemStatus()
{
#if MY_DEBUG_MODE  
	setClassName("CMemStatus");
#endif

#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	my::core::_AllocsCounter.registerEvent(0xffffffff);
	my::core::_DeallocsCounter.registerEvent(0xffffffff);
	my::core::_AllocBytesCounter.registerValue(0xffffffff, 0);
	my::core::_DeallocBytesCounter.registerValue(0xffffffff, 0);
	my::core::_Allocs.clear();
	my::core::_AllocBytes = 0;
	my::core::_AllocSkip = false;
#endif
#endif
}

//-----------------------------------------------------------------------------

CMemStatus::~CMemStatus()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	my::core::_AllocSkip = true;
#endif
#endif
}

//---------------------------------------------------------------------------

u32 CMemStatus::getAllocatedBytes()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	return (u32)(my::core::_AllocBytes > 0 ? my::core::_AllocBytes : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

u32 CMemStatus::getAllocsPerSec()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	s32 val = my::core::_AllocsCounter.getEventPerSecond();
	return (val > 0 ? val : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

u32 CMemStatus::getDeallocsPerSec()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	s32 val = my::core::_DeallocsCounter.getEventPerSecond();
	return (val > 0 ? val : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

u32 CMemStatus::getAllocBytesPerSec()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	s32 val = my::core::_AllocBytesCounter.getValuePerSecond();
	return (val > 0 ? val : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

u32 CMemStatus::getDeallocBytesPerSec()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	s32 val = my::core::_DeallocBytesCounter.getValuePerSecond();
	return (val > 0 ? val : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

__MY_EXPORT__ IMemStatus* createMemStatus()
{
	return new CMemStatus();
}

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//-----------------------------------------------------------------------------
