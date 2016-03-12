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

#if MY_DEBUG_MODE

static my::core::hash_table <void*, size_t> _Allocs;
static size_t _AllocBytes = 0;
my::core::math::event_counter _AllocsCounter, _DeallocsCounter;
my::core::math::event_counter _AllocBytesCounter, _DeallocBytesCounter;
static bool _AllocSkip = true;

#if 0
#if MY_PLATFORM == MY_PLATFORM_WIN32
/*
#	define _CRTDBG_MAP_ALLOC
#	ifdef _malloca
#		undef _malloca
#	endif
#	include <stdlib.h>
#	include <crtdbg.h>
#	include <time.h>
*/
#include <os/ITimer.h>

void operator delete(void* ptr)
{
	my::u32 time = 0;
	size_t size = _Allocs.get_value(ptr);

	if (!_AllocSkip)
		time = TIMER.getTime();

	if (_Allocs.erase_value(ptr))
	{
		_AllocBytes -= size;
		_DeallocsCounter.registerEvent(time);
		_DeallocBytesCounter.registerValue(time, (my::u32)size);
	}

	free(ptr);
}

void operator delete[](void* ptr)
{
	return (operator delete(ptr));
}
 
void *__cdecl operator new(size_t size) throw (...)
{
	my::u32 time = 0;

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
		_AllocBytesCounter.registerValue(time, (my::u32)size);
	}

	return (ptr);
}

void *__cdecl operator new[](size_t count) throw (...)
{
	return (operator new(count));
}

#endif
#endif
#endif

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

CMemStatus::CMemStatus()
{
#if MY_DEBUG_MODE  
	setClassName("CMemStatus");
#endif

#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	_AllocsCounter.registerEvent(0xffffffff);
	_DeallocsCounter.registerEvent(0xffffffff);
	_AllocBytesCounter.registerValue(0xffffffff, 0);
	_DeallocBytesCounter.registerValue(0xffffffff, 0);
	_Allocs.clear();
	_AllocBytes = 0;
	_AllocSkip = false;
#endif
#endif
}

//-----------------------------------------------------------------------------

CMemStatus::~CMemStatus()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	_AllocSkip = true;
#endif
#endif
}

//---------------------------------------------------------------------------

u32 CMemStatus::getAllocatedBytes()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	return (u32)(_AllocBytes > 0 ? _AllocBytes : 0);
#endif
#endif
	return 0;
}

//---------------------------------------------------------------------------

u32 CMemStatus::getAllocsPerSec()
{
#if MY_DEBUG_MODE
#if MY_PLATFORM == MY_PLATFORM_WIN32
	s32 val = _AllocsCounter.getEventPerSecond();
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
	s32 val = _DeallocsCounter.getEventPerSecond();
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
	s32 val = _AllocBytesCounter.getValuePerSecond();
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
	s32 val = _DeallocBytesCounter.getValuePerSecond();
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