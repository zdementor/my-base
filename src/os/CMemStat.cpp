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

#if MY_PLATFORM == MY_PLATFORM_WIN32
#if MY_DEBUG_MODE

#	define _CRTDBG_MAP_ALLOC
#	ifdef _malloca
#		undef _malloca
#	endif
#	include <stdlib.h>
#	include <crtdbg.h>
#	include <time.h>

static my::core::hash_table <void*, size_t> _Allocs;
static my::s32 _AllocBytes = 0;
static bool _AllocSkip = true;

void operator delete(void* ptr)
{
	size_t size = _Allocs.get_value(ptr);
	if (_Allocs.erase_value(ptr))
	{
		_AllocBytes -= size;
	}

	free(ptr);
}

void operator delete[](void* ptr)
{
	return (operator delete(ptr));
}
 
void *__cdecl operator new(size_t size) throw (...)
{
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
	}

	return (ptr);
}

void *__cdecl operator new[](size_t count) throw (...)
{
	return (operator new(count));
}

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

#if MY_PLATFORM == MY_PLATFORM_WIN32
#if MY_DEBUG_MODE
	_Allocs.clear();
	_AllocBytes = 0;
	_AllocSkip = false;
	char *ch = new char[166];
	delete []ch;	
#endif
#endif
}

//-----------------------------------------------------------------------------

CMemStatus::~CMemStatus()
{
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