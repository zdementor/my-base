//|-------------------------------------------------------------------------
//| File:        SDeviceCreationParameters.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SDeviceCreationParametersHPP
#define SDeviceCreationParametersHPP

//---------------------------------------------------------------------------
namespace my {
namespace dev {
//---------------------------------------------------------------------------

//! Structure for holding advanced Device creation parameters.
struct SDeviceCreationParameters
{
	//! Constructs a SDeviceCreationParameters structure with default values.
	SDeviceCreationParameters()
	{
		DriverType = vid::EDT_OPENGL21;
		WindowSize = core::dimension2d<s32>(800, 600);
		Bits = 32;
		TextureFilter = vid::ETF_BILINEAR;
		Flags = 0;
		EventReceiver = 0;
		WindowHandle = 0;		
	}

	//! Type of the device. 
	vid::E_DRIVER_TYPE DriverType;

	//! Size of the window or the video mode in fullscreen mode. Default: 800x600
	core::dimension2d<s32> WindowSize;

	//! Bits per pixel in fullscreen mode. Ignored if windowed mode. Default: 16.
	u32 Bits;

	// texture filter
	vid::E_TEXTURE_FILTER TextureFilter;

	// device creation flags
	u32 Flags;

	//! A user created event receiver.
	io::IEventReceiver* EventReceiver;

	//! Window handle.
	void* WindowHandle;
};

//---------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef SDeviceCreationParametersHPP

