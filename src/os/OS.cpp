//|-------------------------------------------------------------------------
//| File:        COSOperator.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <os/os.h>
#include <io/ILogger.h>

#include <time.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

static bool GammaStored  = false;
static bool GammaChanged = false;
static float GammaValue   = 1.0f;
static unsigned short s_oldHardwareGamma[3][256];
static bool deviceSupportsGamma = false;
static bool ChangedToFullScreen = false;
static core::stringw OperationSystemName = L"";
static E_OS_VERSION OperationSystemVersion = os::EOV_UNKNOWN_OS;

static void _storeBrightness();
static void _restoreBrightness();

//---------------------------------------------------------------------------

#if MY_PLATFORM == MY_PLATFORM_WIN32

#include <tchar.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

#define PRODUCT_ULTIMATE       0x00000001
#define PRODUCT_HOME_BASIC     0x00000002
#define PRODUCT_HOME_PREMIUM   0x00000003
#define PRODUCT_ENTERPRISE     0x00000004
#define PRODUCT_BUSINESS       0x00000006
#define PRODUCT_PROFESSIONAL   PRODUCT_BUSINESS
#define PRODUCT_STARTER        0x0000000B
#define PRODUCT_STANDARD_SERVER        0x00000007
#define PRODUCT_STANDARD_SERVER_CORE   0x0000000D
#define PRODUCT_WEB_SERVER             0x00000011
#define PRODUCT_CLUSTER_SERVER         0x00000012
#define PRODUCT_DATACENTER_SERVER      0x00000008
#define PRODUCT_DATACENTER_SERVER_CORE 0x0000000C
#define PRODUCT_ENTERPRISE_SERVER      0x0000000A
#define PRODUCT_ENTERPRISE_SERVER_CORE 0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64 0x0000000F
#define PRODUCT_SMALLBUSINESS_SERVER   0x00000009
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM      0x00000019
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE 0x0000003F
#define PRODUCT_CORE_SINGLELANGUAGE               0x00000064

#define SM_SERVERR2 89

#define VER_SUITE_STORAGE_SERVER 0x00002000
#define VER_SUITE_WH_SERVER      0x00008000
#define VER_SUITE_COMPUTE_SERVER 0x00004000

#endif // MY_PLATFORM == MY_PLATFORM_WIN32

static void _getOSInfo()
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	DWORD dwType;
	BOOL bOsVersionInfoEx;

	OperationSystemVersion = os::EOV_UNKNOWN_OS;
	OperationSystemName = L"Unknown OS";

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	if (!bOsVersionInfoEx)
		return;

	OperationSystemName = L"";

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI) GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll")), 
		"GetNativeSystemInfo");
	if (NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
			osvi.dwMajorVersion > 4)
	{
		OperationSystemName.append(L"Microsoft ");

		// Test for the specific product.

		if ( osvi.dwMajorVersion == 6 )
		{
			if ( osvi.dwMinorVersion == 0 )
			{
				if ( osvi.wProductType == VER_NT_WORKSTATION )
				{
					OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_VISTA;
					OperationSystemName.append(L"Windows Vista");
				}
				else
				{
					OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_SERVER_2008;
					OperationSystemName.append(L"Windows Server 2008");
				}
			}

			if ( osvi.dwMinorVersion == 1 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
				{
					OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_7;
					OperationSystemName.append(L"Windows 7");
				}
				else
				{
					OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_SERVER_2008;
					OperationSystemName.append(L"Windows Server 2008 R2");
				}
			}

			if ( osvi.dwMinorVersion == 2 )
			{
				OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_8;
				OperationSystemName.append(L"Windows 8");
			}

			pGPI = (PGPI) GetProcAddress(
				GetModuleHandle(TEXT("kernel32.dll")), 
				"GetProductInfo");

			pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch (dwType)
			{
			case PRODUCT_ULTIMATE:
				OperationSystemName.append(L" Ultimate Edition");
				break;
			case PRODUCT_PROFESSIONAL:
				OperationSystemName.append(L" Professional");
				break;
			case PRODUCT_HOME_PREMIUM:
				OperationSystemName.append(L" Home Premium Edition");
				break;
			case PRODUCT_HOME_BASIC:
				OperationSystemName.append(L" Home Basic Edition");
				break;
			case PRODUCT_ENTERPRISE:
				OperationSystemName.append(L" Enterprise Edition");
				break;
			case PRODUCT_STARTER:
				OperationSystemName.append(L" Starter Edition");
				break;
			case PRODUCT_CLUSTER_SERVER:
				OperationSystemName.append(L" Cluster Server Edition");
				break;
			case PRODUCT_DATACENTER_SERVER:
				OperationSystemName.append(L" Datacenter Edition");
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				OperationSystemName.append(L" Datacenter Edition (core installation)");
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				OperationSystemName.append(L" Enterprise Edition");
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				OperationSystemName.append(L" Enterprise Edition (core installation)");
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				OperationSystemName.append(L" Enterprise Edition for Itanium-based Systems");
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				OperationSystemName.append(L" Small Business Server");
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				OperationSystemName.append(L" Small Business Server Premium Edition");
				break;
			case PRODUCT_STANDARD_SERVER:
				OperationSystemName.append(L" Standard Edition");
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				OperationSystemName.append(L" Standard Edition (core installation)");
				break;
			case PRODUCT_WEB_SERVER:
				OperationSystemName.append(L" Web Server Edition");
				break;
			case PRODUCT_CORE_SINGLELANGUAGE:
				OperationSystemName.append(L" Single Language");
				break;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_SERVER_2003;
			if ( GetSystemMetrics(SM_SERVERR2) )
				OperationSystemName.append(L"Windows Server 2003 R2");
			else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
				OperationSystemName.append(L"Windows Storage Server 2003");
			else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
				OperationSystemName.append(L"Windows Home Server");
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
					si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				OperationSystemName.append(L"Windows XP Professional x64 Edition");
			else
				OperationSystemName.append(L"Windows Server 2003");

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						OperationSystemName.append(L" Datacenter Edition for Itanium-based Systems");
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						OperationSystemName.append(L" Enterprise Edition for Itanium-based Systems");
				}
				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						OperationSystemName.append(L" Datacenter x64 Edition");
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						OperationSystemName.append(L" Enterprise x64 Edition");
					else
						OperationSystemName.append(L" Standard x64 Edition");
				}
				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
						OperationSystemName.append(L" Compute Cluster Edition");
					else if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						OperationSystemName.append(L" Datacenter Edition");
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						OperationSystemName.append(L" Enterprise Edition");
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
						OperationSystemName.append(L" Web Edition");
					else
						OperationSystemName.append(L" Standard Edition");
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_XP;
			OperationSystemName.append(L"Windows XP");

			if ( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				OperationSystemName.append(L" Home Edition");
			else
				OperationSystemName.append(L" Professional");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_2K;
			OperationSystemName.append(L"Windows 2000");

			if ( osvi.wProductType == VER_NT_WORKSTATION )
				OperationSystemName.append(L" Professional");
			else 
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					OperationSystemName.append(L" Datacenter Server");
				else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					OperationSystemName.append(L" Advanced Server");
				else
					OperationSystemName.append(L" Server");
			}
		}

		// Include service pack (if any) and build number.

		if( _tcslen(osvi.szCSDVersion) > 0 )
		{
			OperationSystemName.append(L" ");
			OperationSystemName.append(osvi.szCSDVersion);
		}

		core::stringw buf;
		buf.sprintf(L" (build %d)", osvi.dwBuildNumber);
		OperationSystemName.append(buf.c_str());

		if ( osvi.dwMajorVersion >= 6 )
		{
			if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
				OperationSystemName.append(L", 64-bit");
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
				OperationSystemName.append(L", 32-bit");
		}
	}
	else
	if (osvi.dwMajorVersion <= 4)
	{
		OperationSystemVersion = os::EOV_MICROSOFT_WINDOWS_NT;
		OperationSystemName.append("Microsoft Windows NT");
	}
	else
	{  
		OperationSystemName.append(L"Microsoft Windows Unknown");
	}
#endif // MY_PLATFORM == MY_PLATFORM_WIN32
}

//---------------------------------------------------------------------------

//! returns the current operation system version as string.
__MY_EXPORT__ E_OS_VERSION getOperationSystemVersion()
{
	if (OperationSystemVersion == os::EOV_UNKNOWN_OS)
		_getOSInfo();
    return OperationSystemVersion;
}

//---------------------------------------------------------------------------

//! returns the current operation system version as string.
__MY_EXPORT__ const wchar_t* getOperationSystemName()
{
	if (OperationSystemVersion == os::EOV_UNKNOWN_OS)
		_getOSInfo();
	return OperationSystemName.c_str();
}

//---------------------------------------------------------------------------

//! copies text to the clipboard
__MY_EXPORT__ void copyToClipboard(const c8* text)
{
    if (strlen(text)==0)
        return;

// Windows version
#if MY_PLATFORM == MY_PLATFORM_WIN32

    if (!OpenClipboard(0) || text == 0)
        return;

    EmptyClipboard();

    HGLOBAL clipbuffer;
    char * buffer;

    clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
    buffer = (char*)GlobalLock(clipbuffer);

    strcpy(buffer, text);

    GlobalUnlock(clipbuffer);
    SetClipboardData(CF_TEXT, clipbuffer);
    CloseClipboard();
#endif
}

//---------------------------------------------------------------------------

//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
__MY_EXPORT__ const c8* getTextFromClipboard()
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
    if (!OpenClipboard(NULL))
        return 0;
    
    char * buffer = 0;

    HANDLE hData = GetClipboardData( CF_TEXT );
    buffer = (char*)GlobalLock( hData );
    GlobalUnlock( hData );
    CloseClipboard();
    return buffer;
#else
    return NULL;
#endif
}

//----------------------------------------------------------------------------

//! switchs to fullscreen
__MY_EXPORT__ bool switchMonitorToFullScreen(u32 width, u32 height, u32 bits)
{
#if MY_PLATFORM == MY_PLATFORM_WIN32

	RECT desktopRect;
	if (GetWindowRect(GetDesktopWindow(), &desktopRect))
	{
		if ((desktopRect.right - desktopRect.left) == width
				&& (desktopRect.bottom - desktopRect.top) == height)
			return true;
	}

    DEVMODE dm;
    memset(&dm, 0, sizeof(dm));
    dm.dmSize = sizeof(dm);
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmBitsPerPel = bits;
    dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG ret = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
    
    switch(ret)
    {
    case DISP_CHANGE_SUCCESSFUL:
        ChangedToFullScreen = true;
        return true;
    case DISP_CHANGE_RESTART:
        LOGGER.logErr("Switch to fullscreen: "
			"The computer must be restarted in order for the graphics mode to work.");
        return false;
    case DISP_CHANGE_BADFLAGS:
        LOGGER.logErr("Switch to fullscreen: "
			"An invalid set of flags was passed in.");
        return false;
    case DISP_CHANGE_BADPARAM:
        LOGGER.logErr("Switch to fullscreen: "
			"An invalid parameter was passed in. "
			"This can include an invalid flag or combination of flags.");
        return false;
    case DISP_CHANGE_FAILED:
        LOGGER.logErr("Switch to fullscreen: "
			"The display driver failed the specified graphics mode.");
        return false;
    case DISP_CHANGE_BADMODE:
        LOGGER.logErr("Switch to fullscreen: ""The graphics mode is not supported.");
        return false;
    }
    LOGGER.logErr("An unknown error occured while changing to fullscreen.");
    return false;
#else
    return false;
#endif
}

//---------------------------------------------------------------------------

//! getting monitor brightness
__MY_EXPORT__ f32 getMonitorBrightness()
{
	return GammaValue;
}

//---------------------------------------------------------------------------

__MY_EXPORT__ bool setMonitorBrightness(f32 r_gamma, u32 bits)
{
	_storeBrightness();

	CHECK_RANGE(r_gamma, 0.5f, 3.0f);

	GammaValue = r_gamma;

	if (!deviceSupportsGamma)
	{
		LOGGER.logWarn("This device didn't support for gamma changing");
		return false;
	}	

#if MY_PLATFORM == MY_PLATFORM_WIN32

	// build brightness translation tables

	int		i, j;
	float	g;
	int		inf;
	int		shift;

	// setup the overbright lighting
	u8 overbrightBits = 0;	

	// allow 2 overbright bits in 24 bit, but only 1 in 16 bit
	if (bits > 16) 
	{
		if ( overbrightBits > 2 ) 
			overbrightBits = 2;
	} 
	else 
	{
		if ( overbrightBits > 1 ) 
			overbrightBits = 1;
	}

	if ( overbrightBits < 0 ) 
		overbrightBits = 0;


	g = r_gamma;

	shift = overbrightBits;

	unsigned char s_gammatable[256];

	for ( i = 0; i < 256; i++ ) 
	{
		if ( g == 1 ) 
			inf = i;
		else 
			inf = 255 * pow ( i/255.0f, 1.0f / g ) + 0.5f;

		inf <<= shift;

		if (inf < 0) 
			inf = 0;
		if (inf > 255) 
			inf = 255;

		s_gammatable[i] = inf;
	}
	
	u8 *red   = s_gammatable;
	u8 *green = s_gammatable;
	u8 *blue  = s_gammatable;		
		
	unsigned short table[3][256];
	int		ret;

	for ( i = 0; i < 256; i++ ) 
	{
		table[0][i] = ( ( ( unsigned short ) red[i] ) << 8 ) | red[i];
		table[1][i] = ( ( ( unsigned short ) green[i] ) << 8 ) | green[i];
		table[2][i] = ( ( ( unsigned short ) blue[i] ) << 8 ) | blue[i];
	}

	// Win2K puts this odd restriction on gamma ramps...

	if ( OperationSystemVersion == os::EOV_MICROSOFT_WINDOWS_2K )
	{
		// performing W2K gamma clamp
		for ( j = 0 ; j < 3 ; j++ ) 
		{
			for ( i = 0 ; i < 128 ; i++ ) 
				if ( table[j][i] > ( (128+i) << 8 ) ) 
					table[j][i] = (128+i) << 8;
			if ( table[j][127] > 254<<8 ) 
				table[j][127] = 254<<8;
		}
	} 
	else 
	{
		// skipping W2K gamma clamp
	}

	// enforce constantly increasing
	for ( j = 0 ; j < 3 ; j++ ) 
		for ( i = 1 ; i < 256 ; i++ ) 
			if ( table[j][i] < table[j][i-1] ) 
				table[j][i] = table[j][i-1];
	HDC hDC = GetDC( GetDesktopWindow() );
	ret = SetDeviceGammaRamp(hDC, table) != 0 ? true : false;
	ReleaseDC( GetDesktopWindow(), hDC );
	if (!ret) 
	{
		LOGGER.logErr("Setting monitor gamma failed");
		return false;
	}
	LOGGER.logInfo("Monitor brightness changed to %f", r_gamma);
	GammaChanged = true;
	return true;
#else
    return false;
#endif
}

//---------------------------------------------------------------------------

static void _restoreDisplaySettings()
{
#ifdef WIN32
	if (ChangedToFullScreen)
        ChangeDisplaySettings(NULL,0);
#endif
}

//---------------------------------------------------------------------------

static void _restoreBrightness()
{
#ifdef WIN32
	if ( deviceSupportsGamma && GammaChanged && GammaStored )
	{
		bool ret = false;

		HDC hDC = GetDC( GetDesktopWindow() );

		ret = SetDeviceGammaRamp( hDC, s_oldHardwareGamma ) != 0 ? true : false;
		
		ReleaseDC( GetDesktopWindow(), hDC );		
		
		if (ret)
		{
			LOGGER.log("Monitor brightness restored");
		}
		else
		{
			LOGGER.log("Failed to restore monitor brightness", io::ELL_WARNING);
		}
	}	
#endif
}

//---------------------------------------------------------------------------

static void _storeBrightness()
{
#ifdef WIN32
	if (!GammaStored)
	{
		HDC hDC = GetDC( GetDesktopWindow() );
		deviceSupportsGamma = GetDeviceGammaRamp( hDC, s_oldHardwareGamma )!=0?true:false;
		ReleaseDC( GetDesktopWindow(), hDC );

		if ( deviceSupportsGamma )
		{
			//
			// do a sanity check on the gamma values
			//
			if ( ( HIBYTE( s_oldHardwareGamma[0][255] ) <= HIBYTE( s_oldHardwareGamma[0][0] ) ) ||
				 ( HIBYTE( s_oldHardwareGamma[1][255] ) <= HIBYTE( s_oldHardwareGamma[1][0] ) ) ||
				 ( HIBYTE( s_oldHardwareGamma[2][255] ) <= HIBYTE( s_oldHardwareGamma[2][0] ) ) )
			{
				deviceSupportsGamma = false;
				LOGGER.log("Device has broken gamma support, generated gamma.dat", io::ELL_WARNING );
			}

			//
			// make sure that we didn't have a prior crash in the game, and if so we need to
			// restore the gamma values to at least a linear value
			//
			if ( ( HIBYTE( s_oldHardwareGamma[0][181] ) == 255 ) )
			{
				int g;

				LOGGER.logWarn("Suspicious gamma tables, using linear ramp for restoration");

				for ( g = 0; g < 255; g++ )
				{
					s_oldHardwareGamma[0][g] = g << 8;
					s_oldHardwareGamma[1][g] = g << 8;
					s_oldHardwareGamma[2][g] = g << 8;
				}
			}
		}

		if (deviceSupportsGamma)
		{
			GammaStored = true;
		}
	}
#endif
}

//---------------------------------------------------------------------------

__MY_EXPORT__ void restoreMonitorSettings()
{
	_restoreBrightness();
	_restoreDisplaySettings();
}

//---------------------------------------------------------------------------

__MY_EXPORT__ const c8* getCurrentProcessName(bool file_name_only)
{
	static core::stringc fname = "";
#if MY_PLATFORM == MY_PLATFORM_WIN32
	TCHAR szPath[MAX_PATH];
    if(!GetModuleFileName(NULL, szPath, MAX_PATH))
        return NULL;
	fname = szPath;
	if (file_name_only)
		fname = core::extractFileName(fname);
#endif
	return fname.c_str();
}

//---------------------------------------------------------------------------

static STimeValue _CurrentTime = {0, 0, 0, 0, 0, 0, 0, 0};
static core::stringc _CurrentTimeString = "";

const STimeValue& getCurrentTime()
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
	SYSTEMTIME tv;

	GetLocalTime(&tv);

	_CurrentTime.Year = tv.wYear;
	_CurrentTime.Month = tv.wMonth;
	_CurrentTime.MonthDay = tv.wDay;
	_CurrentTime.WeekDay = tv.wDayOfWeek;
	_CurrentTime.Hour = tv.wHour;
	_CurrentTime.Minute = tv.wMinute;
	_CurrentTime.Second = tv.wSecond;
	_CurrentTime.Milisecond = tv.wMilliseconds;
#else
	struct tm *tm;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tm = localtime(&tv);

	_CurrentTime.Year = tm->tm_year;
	_CurrentTime.Month = tm->tm_mon;
	_CurrentTime.MonthDay = tm->tm_mday;
	_CurrentTime.WeekDay = tm->tm_wday;
	_CurrentTime.Hour = tm->tm_hour;
	_CurrentTime.Minute = tm->tm_min;
	_CurrentTime.Second = tm->tm_sec;
	_CurrentTime.Milisecond = tv->tv_usec;
#endif
	return _CurrentTime;
}

//---------------------------------------------------------------------------

const c8* getCurrentTimeString(E_TIME_STRING_FORMAT format)
{
	const STimeValue &t = getCurrentTime();

	_CurrentTimeString.sprintf("%04d/%02d/%02d %02d:%02d:%02d",
		t.Year, t.Month, t.MonthDay,
		t.Hour, t.Minute, t.Second);

	return _CurrentTimeString.c_str();
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------
