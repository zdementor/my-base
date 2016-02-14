//|-------------------------------------------------------------------------
//| File:        strfunc.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace core { 
//---------------------------------------------------------------------------

static core::array<c8> _TmpBufUtf8ToAnsi;
static core::array<c8> _TmpBufAnsiToUtf8;
static core::array<wchar_t> _TmpBufWide;
static core::stringc _TmpHexStr;
static core::stringc _TmpHexStr64;

//---------------------------------------------------------------------------

const c8* Utf8ToAnsi(const c8* utf8)
{
#if MY_PLATFORM == MY_PLATFORM_WIN32

	s32		len			= strlen(utf8) + 1;
	s32		length		= len;
	c8		*ansistr	= NULL;
	wchar_t	*widestr	= NULL;

	// calculate needed buffer size only
	length = MultiByteToWideChar(CP_UTF8, 0, utf8, length, NULL, 0);

	_TmpBufWide.set_used(length);
	_TmpBufUtf8ToAnsi.set_used(length);
	widestr = _TmpBufWide.pointer();
	ansistr = _TmpBufUtf8ToAnsi.pointer();

	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, widestr, length); // UTF8 to UNICODE
	WideCharToMultiByte(CP_ACP, 0, widestr, -1, ansistr, length, NULL, NULL); // UNICODE to ANSI

	return ansistr;
#else
	#error "Unknown platform"
#endif
}

//---------------------------------------------------------------------------

const c8* AnsiToUtf8(const c8* ansi)
{
#if MY_PLATFORM == MY_PLATFORM_WIN32

	s32		len			= strlen(ansi) + 1;
	s32		length		= len;
	c8		*utf8str	= NULL;
	wchar_t	*widestr	= NULL;

	_TmpBufWide.set_used(length);
	widestr = _TmpBufWide.pointer();

	MultiByteToWideChar(CP_ACP, 0, ansi, -1, widestr, length); // ANSI to UNICODE

	// calculate needed buffer size only
	length = WideCharToMultiByte(CP_UTF8, 0, widestr, length, NULL, 0, 0, 0);

	_TmpBufAnsiToUtf8.set_used(length);
	utf8str = _TmpBufAnsiToUtf8.pointer();

	WideCharToMultiByte(CP_UTF8, 0, widestr, -1, utf8str, length, 0, 0); // UNICODE to UTF-8

	return utf8str;
#else
	#error "Unknown platform"
#endif
}

//---------------------------------------------------------------------------

void CP866ToCP1251(c8 *s)
{
    for (c8 *c = s; *c; ++c)
	{
        if (*c > -129 && *c < -80)
            *c += 64;
        else if (*c > -33 && *c < -16)
            *c += 16;
    }
}

//---------------------------------------------------------------------------

const c8* ToHexStr(u32 v)
{
	return _TmpHexStr.sprintf("%08X", v).c_str();
}

//---------------------------------------------------------------------------

const c8* ToHexStr64(u64 v)
{
	return _TmpHexStr64.sprintf("%016I64X", v).c_str();
}

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------