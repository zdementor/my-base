//|-------------------------------------------------------------------------
//| File:        ConstOS.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_osHPP
#define my_const_osHPP

//---------------------------------------------------------------------------
namespace my {
namespace os {
//----------------------------------------------------------------------------

enum E_OS_VERSION
{	
	EOV_MICROSOFT_WINDOWS_NT = 0,
	EOV_MICROSOFT_WINDOWS_2K,
	EOV_MICROSOFT_WINDOWS_XP,
	EOV_MICROSOFT_WINDOWS_SERVER_2003,
	EOV_MICROSOFT_WINDOWS_VISTA,
	EOV_MICROSOFT_WINDOWS_7,
	EOV_MICROSOFT_WINDOWS_SERVER_2008,
	EOV_MICROSOFT_WINDOWS_8,
	EOV_MICROSOFT_WINDOWS_95,
	EOV_MICROSOFT_WINDOWS_98,
	EOV_MICROSOFT_WINDOWS_ME,
	EOV_MICROSOFT_WIN32S,
	EOV_MICROSOFT_WINDOWS_UNKNOWN, 
	EOV_UNKNOWN_OS,

	E_OS_VERSION_COUNT,

	E_OS_VERSION_COUNT_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! enumeration for the thread states
enum E_THREAD_STATUS
{
	ETRDS_STOPPED =0,
	ETRDS_STOPPING,
	ETRDS_RUNNING,

	E_THREAD_STATUS_COUNT,

	E_THREAD_STATUS_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! enumeration for the thread priorities
enum E_THREAD_PRIORITY
{
	ETRDP_HIGH =0,
	ETRDP_NORMAL,
	ETRDP_LOW,

	E_THREAD_PRIORITY_COUNT,

	E_THREAD_PRIORITY_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! enumeration for the thread types
enum E_THREAD_TYPE
{
	ETRT_TIMER =0,
	ETRT_COMPUTATION,

	E_THREAD_TYPE_COUNT,

	E_THREAD_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_TIME_STRING_FORMAT
{
	ETSF_STANDARD = 0,
	ETSF_YYYY_MM_DD_HH_MM_SS = ETSF_STANDARD, // format "%04d/%02d/%02d %02d:%02d:%02d"
};

//----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_osHPP