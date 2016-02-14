//|-------------------------------------------------------------------------
//| File:        CDeviceWin32.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDeviceWin32HPP
#define CDeviceWin32HPP

#include <my_inc.h>

//---------------------------------------------------------------------------
#if MY_PLATFORM == MY_PLATFORM_WIN32
//---------------------------------------------------------------------------

#include <dev/IDevice.h>
#include <io/ICursorControl.h>  
#include <vid/IVideoDriver.h>

#include "CDeviceStub.h"
#include <img/IImagePresenter.h>

//---------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN

//---------------------------------------------------------------------------
namespace my {
namespace dev {
//---------------------------------------------------------------------------

class CDeviceWin32 : public CDeviceStub, img::IImagePresenter
{
public:

    CDeviceWin32(SDeviceCreationParameters params);

    virtual ~CDeviceWin32();

	//! IDevice interface implementation

    virtual bool run();

    virtual void setWindowCaption ( const wchar_t* text );
	virtual void setWindowCaption ( const c8* text );

    virtual bool isWindowActive();

	virtual void show();
	virtual void hide();
    virtual void close();

    virtual void OnResize();

    virtual void setResizeAble(bool resize=false);

	virtual void setWindowIcon(const c8 *iconFileName, u32 width, u32 height);

	//! IImagePresenter interface implementation

	virtual void present(img::IImage* surface);

private:

    void getWindowsVersion(core::stringc& out, os::E_OS_VERSION &out_ver);

    void resizeIfNecessary();

    HWND HWnd;
    
    bool FullScreen;
    bool IsNonNTWindows;
	bool ExternalWindow;

	bool m_Quit;
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifdef MY_PLATDORM == MY_PLATFORM_WIN32
#endif