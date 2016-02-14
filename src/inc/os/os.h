//|-------------------------------------------------------------------------
//| File:        IOSOperator.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef osHPP
#define osHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace os {
//----------------------------------------------------------------------------

struct STimeValue
{
	u32 Year, Month;
	u32 MonthDay, WeekDay;
	u32 Hour, Minute, Second, Milisecond;
};

//! The Operating system provides operation system specific methods and informations.

//! returns the current operation system version as string.
__MY_OS_LIB_API__ const wchar_t* getOperationSystemName();

//! returns the current operation system version as string.
__MY_OS_LIB_API__ E_OS_VERSION getOperationSystemVersion();

//! copies text to the clipboard
__MY_OS_LIB_API__ void copyToClipboard(const c8* text);

//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
__MY_OS_LIB_API__ const c8* getTextFromClipboard(); 

//! getting monitor brightness
__MY_OS_LIB_API__ f32 getMonitorBrightness();

//! setting monitor brightness
__MY_OS_LIB_API__ bool setMonitorBrightness(f32 r_gamma, u32 bits);
	
//! switchs monitor to fullscreen
__MY_OS_LIB_API__ bool switchMonitorToFullScreen(u32 width, u32 height, u32 bits);

//! restore monotor settings (brighteness/resolution) to initial values
__MY_OS_LIB_API__ void restoreMonitorSettings();

//! return name of the curren running process
__MY_OS_LIB_API__ const c8* getCurrentProcessName(bool file_name_only = true);

//! return time values
__MY_OS_LIB_API__ const STimeValue& getCurrentTime();
__MY_OS_LIB_API__ const c8* getCurrentTimeString(E_TIME_STRING_FORMAT format = ETSF_STANDARD);

//----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//----------------------------------------------------------------------------

#endif

