//|-------------------------------------------------------------------------
//| File:        CDeviceWin32.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CDeviceWin32.h"

#if MY_PLATFORM == MY_PLATFORM_WIN32

#include <game/IGameManager.h> 
#include <io/IInputDispatcher.h>
#include <os/ICriticalSection.h>
#include <io/IEventReceiver.h>
#include <os/IEvent.h>
#include <io/ILogger.h>
#include <res/IResourceManager.h>
#include <dev/SDeviceCreationParameters.h>

struct SEnvMapper
{
    HWND hWnd;
    my::dev::CDeviceWin32* irrDev;
};

//----------------------------------------------------------------------------

my::core::list<SEnvMapper> EnvMap;

//----------------------------------------------------------------------------

SEnvMapper* getEnvMapperFromHWnd(HWND hWnd)
{
    my::core::list<SEnvMapper>::iterator it = EnvMap.begin();
    for (; it!= EnvMap.end(); ++it)
        if ((*it).hWnd == hWnd)
            return &(*it);

    return 0;
}

//----------------------------------------------------------------------------

my::dev::CDeviceWin32* getDeviceFromHWnd(HWND hWnd)
{
    my::core::list<SEnvMapper>::iterator it = EnvMap.begin();
    for (; it!= EnvMap.end(); ++it)
        if ((*it).hWnd == hWnd)
            return (*it).irrDev;

    return 0;
}

//----------------------------------------------------------------------------

//! main window procedure
LRESULT CALLBACK MyDeviceWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    #ifndef WM_MOUSEWHEEL
    #define WM_MOUSEWHEEL 0x020A
    #endif
    #ifndef WHEEL_DELTA
    #define WHEEL_DELTA 120
    #endif

	my::f32 Wheel =0;
    
	my::io::IInputDispatcher *inpd = INPUT_DISPATCHER_PTR;

    switch (message)
    {
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			my::s32 x = ps.rcPaint.left;
			my::s32 y = ps.rcPaint.top;
			my::s32 width = ps.rcPaint.right - x;
			my::s32 height = ps.rcPaint.bottom - y;

			my::io::SEvent paint;
			paint.EventType = my::io::EET_PAINT_EVENT;
			paint.PaintEvent.X = x;
			paint.PaintEvent.Y = y;
			paint.PaintEvent.Width = width;
			paint.PaintEvent.Height = height;

			// immediately handle this event
			((my::dev::CDeviceStub*)DEVICE_PTR)->
				_postEventFromUser(paint);

			EndPaint(hWnd, &ps);

			return 0;
        }
    case WM_ERASEBKGND:
        return 0;

    case WM_SETCURSOR:
		{
			CURSOR_CONTROL.refreshCursor();
			return 0;
		}
    case WM_MOUSEWHEEL:
		{
			Wheel = (float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA;
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseWheel(Wheel);
			return 0;
		}
    case WM_LBUTTONDOWN:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseButtonPress(my::io::EMC_LMOUSE);
			return 0;
		}
    case WM_LBUTTONUP:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseButtonUp(my::io::EMC_LMOUSE);
			return 0;
		}
    case WM_RBUTTONDOWN:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseButtonPress(my::io::EMC_RMOUSE);
			return 0;
		}
    case WM_RBUTTONUP:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseButtonUp(my::io::EMC_RMOUSE);
			return 0;
		}
    case WM_MBUTTONDOWN:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT)
				inpd->collectMouseButtonPress(my::io::EMC_MMOUSE);
			return 0;
		}
    case WM_MBUTTONUP:
		{
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseButtonUp(my::io::EMC_MMOUSE);
			return 0;		
		}
    case WM_MOUSEMOVE:
		{
			my::s32 X = LOWORD(lParam);
			my::s32 Y = HIWORD(lParam);
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectMouseMoveTo(X, Y);
			return 0;
		}
    case WM_KEYDOWN:
        {   
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT)
			{
				wchar_t KeyAsc=0;  
				static BYTE allKeys[256];
				GetKeyboardState(allKeys);
				ToUnicode(wParam, 0, allKeys, &KeyAsc, 1, 0);    
				inpd->collectKeyPress(my::io::win2MyKeyCode((my::u8)wParam), KeyAsc);
			}
			return 0;
        }
    case WM_KEYUP:
        {
			if (inpd && inpd->getInputType() == my::io::EIT_STANDARD_INPUT) 
				inpd->collectKeyUp(my::io::win2MyKeyCode((my::u8)wParam));
			return 0;
        }
    case WM_SIZE:
        {
			// resize
			my::dev::IDevice *dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->OnResize();		
			return 0;
        }  
    case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
    case WM_SYSCOMMAND:
		{
			// prevent screensaver or monitor powersave mode from starting
			if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
				(wParam & 0xFFF0) == SC_MONITORPOWER)
				return 0;
			if (wParam == SC_CLOSE)
			{
				my::dev::IDevice *dev = getDeviceFromHWnd(hWnd);
				if (dev)
					dev->close();
				return 0;
			}
			break;
		}
	case WM_QUIT:
		{
			my::dev::IDevice *dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->close();		
			return 0;
		}
	case WM_NCHITTEST:
		{
			my::dev::IDevice *dev = getDeviceFromHWnd(hWnd);
			if (dev && dev->getDeviceFlagValue(my::dev::EDCF_DRAG_USING_TOP_AREA))
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);

				my::s32 X = LOWORD(lParam);
				my::s32 Y = HIWORD(lParam);
				if (Y < (rect.top + 25))
				{
					UINT uHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
					if(uHitTest == HTCLIENT)
						return HTCAPTION;
					else
						return uHitTest;
				}
			}
		}
	default:
		break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

CDeviceWin32::CDeviceWin32(SDeviceCreationParameters params)
	: CDeviceStub(params.EventReceiver, params), 
HWnd(0), IsNonNTWindows(false), ExternalWindow(false), m_Quit(false)
{
#if MY_DEBUG_MODE 
	IDevice::setClassName("CDeviceWin32 IDevice");
	IRunnable::setClassName("CDeviceWin32 IRunnable");
#endif

	vid::E_DRIVER_TYPE     driverType    = CreationParams.DriverType;
	core::dimension2d<s32> windowSize    = CreationParams.WindowSize;
	u32                    bits          = CreationParams.Bits;
	vid::E_TEXTURE_FILTER  textureFilter = CreationParams.TextureFilter;

	bool fullscreen	= getDeviceFlagValue(EDCF_FULLSCREEN);
	bool sysmenu	= !getDeviceFlagValue(EDCF_NO_SYSTEM_MENU);
	bool notitlebar	= getDeviceFlagValue(EDCF_NO_TITLE_BAR);
	bool roundrect	= getDeviceFlagValue(EDCF_ROUNDED_FRAME_RECTANGLE);

	FullScreen = fullscreen;

	ExposedDeviceData.OS.Name		= os::getOperationSystemName();
	ExposedDeviceData.OS.Version	= os::getOperationSystemVersion();

	LOGGER.logInfo(L"OS:\n%s", ExposedDeviceData.OS.Name.c_str());

    // create window

    HINSTANCE hInstance = GetModuleHandle(0);

    //if (driverType != vid::EDT_NULL)
    {   
		if (params.WindowHandle == 0)
		{
			// create the window, only if we do not use the null device
			const c8* ClassName = "CDeviceWin32";

			// Register Class
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX); 
			wcex.style          = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc    = (WNDPROC)MyDeviceWindowProc;
			wcex.cbClsExtra     = 0;
			wcex.cbWndExtra     = 0;
			wcex.hInstance      = hInstance;
			wcex.hIcon          = 0;
			wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
			wcex.lpszMenuName   = 0;
			wcex.lpszClassName  = ClassName;
			wcex.hIconSm        = 0;
			RegisterClassEx(&wcex);

			// calculate client size
			RECT clientSize;
			clientSize.top = 0;
			clientSize.left = 0;
			clientSize.right = windowSize.Width;
			clientSize.bottom = windowSize.Height;
        
			DWORD style = WS_POPUP;

			if (!fullscreen)
			{
				if (!notitlebar)
					style = WS_BORDER | WS_CAPTION;
				if (sysmenu)
					style |= WS_SYSMENU;
			}

			style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

			AdjustWindowRect(&clientSize, style, FALSE);

			s32 realWidth = clientSize.right - clientSize.left;
			s32 realHeight = clientSize.bottom - clientSize.top;

			s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
			s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

			if (fullscreen)
			{
				windowLeft = 0;
				windowTop = 0;
			}  

			// Create our main window
			HWnd = CreateWindowEx(
				fullscreen ? WS_EX_TOPMOST : 0, ClassName, "", style, windowLeft, windowTop,
				realWidth, realHeight,  NULL, NULL, hInstance, NULL);

			if (roundrect)
			{
				HRGN hReg = CreateRoundRectRgn(2, 1, realWidth - 1, realHeight - 2, 18, 18);
				SetWindowRgn(HWnd, hReg, true);
				DeleteObject(hReg);
			}

			MoveWindow(HWnd,
				windowLeft, windowTop, realWidth, realHeight, TRUE);
		}
		else
		{

			HWnd = reinterpret_cast<HWND>(params.WindowHandle);

			RECT r;
			GetWindowRect(HWnd, &r);
			windowSize.Width = r.right - r.left;
			windowSize.Height = r.bottom - r.top;
			fullscreen = false;

			ExternalWindow = true;
		}
    }

	bool ok = true;	

    // create driver

	ExposedDeviceData.Video.Win32.HWnd	= reinterpret_cast <void*>(HWnd);
	ExposedDeviceData.Video.WindowSize	= windowSize;
	ExposedDeviceData.Video.FullScreen	= fullscreen;
	ExposedDeviceData.Video.Bits		= bits;
	ExposedDeviceData.Video.DriverType	= driverType;

	LOGGER.logInfo("Video initialization:");

	{
		RECT wrect, crect;
		GetWindowRect(HWnd, &wrect);
		GetClientRect(HWnd, &crect);

		HDC hdc = GetDC(NULL);
		s32 dpiX = 0, dpiY = 0; 
		if (hdc)
		{
			dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
			dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
			ReleaseDC(NULL, hdc);
		}
		LOGGER.logInfo(" Created window rect={%d, %d x %d, %d} size={%d, %d} client_size{%d, %d} dpi={%d, %d}",
			wrect.left, wrect.top, wrect.right, wrect.bottom,
			wrect.right - wrect.left, wrect.bottom - wrect.top,
			crect.right - crect.left, crect.bottom - crect.top,
			dpiX, dpiY);
	}

    VideoDriver = vid::createVideoDriver(ExposedDeviceData.Video, textureFilter);
	
	if (VideoDriver)
		LOGGER.logInfo("Video OK");
	else
	{
		LOGGER.logErr("Error initializing video driver");
		exit(EXIT_FAILURE);	
	}

	// creating input Dispatcher
	InputDispatcher = createInputDispatcher(io::EIT_STANDARD_INPUT);

	if (!InputDispatcher) exit(EXIT_FAILURE);

    // создаем сцену и интерфейс
    createSingletons();

    // register environment
    SEnvMapper em;
    em.irrDev = this;
    em.hWnd = HWnd;
    EnvMap.push_back(em);

	// checking settings
	if (VideoDriver->getMaximalTextureUnitsAmount() < 4)
	{
		LOGGER.logWarn("Your video hardware not support 4 texture units or higher!");
		if (getDeviceFlagValue(EDCF_DIRT_ON_LEVEL))
			LOGGER.logWarn("'Dirt on level' option will be disabled.");
		if (getDeviceFlagValue(EDCF_LIGHTING_MEDIUM_QUALITY) ||
			getDeviceFlagValue(EDCF_LIGHTING_HIGH_QUALITY))
			LOGGER.logWarn("'Dynamic lighting' option will be disabled.");
		if (getDeviceFlagValue(EDCF_SHADERS_HIGH_QUALITY))
			LOGGER.logWarn("'High quality shaders' option will be disabled.");
		CreationParams.Flags &= ~EDCF_DIRT_ON_LEVEL;
		CreationParams.Flags &= ~EDCF_LIGHTING_MEDIUM_QUALITY;
		CreationParams.Flags &= ~EDCF_LIGHTING_HIGH_QUALITY;
		CreationParams.Flags &= ~EDCF_SHADERS_HIGH_QUALITY;
	}

	// default arrow cursor
	CURSOR_CONTROL.setCursor(io::ECT_ARROW);
	CURSOR_CONTROL.setVisible(true);
}

//----------------------------------------------------------------------------

CDeviceWin32::~CDeviceWin32()
{	
	// unregister environment
    core::list <SEnvMapper>::iterator it = EnvMap.begin();
    for (; it!= EnvMap.end(); ++it)
	{
        if ((*it).hWnd == HWnd)
        {
            EnvMap.erase(it);
            break;
        }
	}
	if (!ExternalWindow)
		DestroyWindow(HWnd);  
}

//----------------------------------------------------------------------------

bool CDeviceWin32::run()
{
	CDeviceStub::run();

	// handling all windows messager, wich sended to our main window

    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	
    {
		TranslateMessage(&msg);
		if (ExternalWindow && msg.hwnd == HWnd)
		{
			//MyDeviceWindowProc(HWnd, msg.message, msg.wParam, msg.lParam);
		}
		else
			DispatchMessage(&msg);
    }

    if (!m_Quit)
	{
		// user input handling
		InputDispatcher->captureInput();

		// sending all user input events to all event receivers
		core::array<io::SEvent> EventsQueue =
			InputDispatcher->getInputEventsQueue();
		for (u32 e=0; e<InputDispatcher->getInputEventsQueue().size(); e++)
			_postEventFromUser(EventsQueue[e]);		
	}	

    return !m_Quit;
}

//----------------------------------------------------------------------------

void CDeviceWin32::setWindowCaption(const wchar_t* text)
{
    if (IsNonNTWindows)
    {
        core::stringc s = text;
        SetWindowTextA(HWnd, s.c_str());
    }
    else
        SetWindowTextW(HWnd, text);
}

//----------------------------------------------------------------------------

void CDeviceWin32::setWindowCaption(const c8* text)
{
	core::stringw strw = text;
	
	setWindowCaption ( strw.c_str() );
}

//----------------------------------------------------------------------------

void CDeviceWin32::show()
{
	bool alwaysontop = getDeviceFlagValue(EDCF_ALWAYS_ON_TOP);

	ShowWindow(HWnd, SW_SHOW);
	UpdateWindow(HWnd);

	if (alwaysontop)
		SetWindowPos(HWnd, HWND_TOPMOST,
			0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	CDeviceStub::show();
}

//----------------------------------------------------------------------------

void CDeviceWin32::hide()
{
	ShowWindow(HWnd, SW_HIDE);

	CDeviceStub::hide();
}

//----------------------------------------------------------------------------

void CDeviceWin32::close()
{
	m_Quit = true;
}

//----------------------------------------------------------------------------

bool CDeviceWin32::isWindowActive()
{
    return (GetActiveWindow() == HWnd);
}

//----------------------------------------------------------------------------

//! Notifies the device, that it has been resized
void CDeviceWin32::OnResize()
{
	if (!m_Visible)
		return;

	RECT r;
    GetClientRect(HWnd, &r);

    if (r.right <= 1 || r.bottom <= 1)
    {
        LOGGER.logInfo("Ignoring resize operation to (%d %d)", r.right, r.bottom);
    }
    else
    {
        LOGGER.logInfo("Resizing window (%d %d)", r.right, r.bottom);

        VIDEO_DRIVER.OnResize(my::core::dimension2d<my::s32>(r.right, r.bottom));

		ExposedDeviceData.Video.WindowSize = my::core::dimension2d<my::s32>(r.right, r.bottom);
    }
}

//----------------------------------------------------------------------------

//! Sets if the window should be resizeable in windowed mode.
void CDeviceWin32::setResizeAble(bool resize)
{
    if (FullScreen)
        return;

    LONG style = WS_POPUP;

    if (!resize)
        style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    else
        style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX;

    if (!SetWindowLong(HWnd, GWL_STYLE, style))
        LOGGER.logInfo("Could not change window style.");

    RECT clientSize;
    clientSize.top = 0;
    clientSize.left = 0;
    clientSize.right = VIDEO_DRIVER.getScreenSize().Width;
    clientSize.bottom = VIDEO_DRIVER.getScreenSize().Height;

    AdjustWindowRect(&clientSize, style, FALSE);

    s32 realWidth = clientSize.right - clientSize.left;
    s32 realHeight = clientSize.bottom - clientSize.top;

    s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
    s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

    SetWindowPos(
		HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW
		);
}

//----------------------------------------------------------------------------

void CDeviceWin32::setWindowIcon(const c8 *iconFileName, u32 width, u32 height)
{
	HANDLE hIcon = 0, hIconSm = 0;
	core::stringc fullFileName;
	fullFileName.sprintf("%s%s",
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
		iconFileName);

	hIcon = LoadImage(NULL, fullFileName.c_str(), IMAGE_ICON,
		width, height, LR_LOADFROMFILE);
    if(hIcon)
        SendMessage(HWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    else
		LOGGER.logInfo("Could not load large icon from %s!", fullFileName.c_str());

    hIconSm = LoadImage(NULL, fullFileName.c_str(), IMAGE_ICON,
		width / 2, height / 2, LR_LOADFROMFILE);
    if(hIconSm)
        SendMessage(HWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
    else
        LOGGER.logInfo("Could not load small icon from %s!", fullFileName.c_str());
}

//----------------------------------------------------------------------------

void CDeviceWin32::present(img::IImage* image)
{
    HDC dc = GetDC(HWnd);
    RECT rect;
    GetClientRect(HWnd, &rect);

    if (image->getColorFormat() != img::ECF_A1R5G5B5)
        return;

    s16* memory = (s16*)image->getData();
            
    BITMAPV4HEADER bi;
    ZeroMemory (&bi, sizeof(bi));
    bi.bV4Size          = sizeof(BITMAPINFOHEADER);
    bi.bV4BitCount      = 16;
    bi.bV4Planes        = 1;
    bi.bV4Width         = image->getDimension().Width;
    bi.bV4Height        = -image->getDimension().Height;
    bi.bV4V4Compression = BI_BITFIELDS;
    bi.bV4AlphaMask     = (s16)(0x1<<15);
    bi.bV4RedMask       = 0x1F<<10;
    bi.bV4GreenMask     = 0x1F<<5;
    bi.bV4BlueMask      = 0x1F;

    StretchDIBits(
		dc, 0,0, rect.right, rect.bottom,
        0, 0, image->getDimension().Width, image->getDimension().Height,
        memory, (const BITMAPINFO*)(&bi), DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(HWnd, dc);
}

//----------------------------------------------------------------------------

extern "C" __MY_SCN_LIB_API__ IDevice* createDeviceEx(	
	SDeviceCreationParameters params)
{
    return new CDeviceWin32(params);
}

//----------------------------------------------------------------------------

__MY_DEV_LIB_API__ IDevice* createDevice(	
	SDeviceCreationParameters params)
{
    return new CDeviceWin32(params);
}

//----------------------------------------------------------------------------

__MY_DEV_LIB_API__ IDevice* createDevice(
	vid::E_DRIVER_TYPE driverType,
	u32 windowWidth, u32 windowHeight,
	u32 bits,
	vid::E_TEXTURE_FILTER textureFilter,
	u32 flags)
{
	SDeviceCreationParameters params;
	params.DriverType = driverType;
	params.WindowSize.set(windowWidth, windowHeight);
	params.Bits = bits;
	params.TextureFilter = textureFilter;
	params.Flags = flags;

	return createDevice(params);
}

//----------------------------------------------------------------------------

__MY_DEV_LIB_API__ bool destroyDevice()
{
	if (IDevice::getSingletonPtr())
		return IDevice::getSingleton().drop();
	return true;
}

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32
