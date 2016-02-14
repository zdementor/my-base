//|-------------------------------------------------------------------------
//| File:        SExposedDeviceData.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SExposedDeviceData_h
#define SExposedDeviceData_h
//---------------------------------------------------------------------------

// forward declarations for internal pointers

struct IDirect3D9;
struct IDirect3DDevice9;

//---------------------------------------------------------------------------
namespace my {
//---------------------------------------------------------------------------

namespace vid {

//! structure for holding data describing a driver and operating system specific data.
struct SExposedVideoData
{
	union
	{
		struct
		{
			union
			{
				struct
				{
					//! Pointer to the IDirect3D9 interface
					IDirect3D9 *D3D;
					//! Pointer to the IDirect3DDevice9 interface
					IDirect3DDevice9 *D3DDevice;
				} 
				D3D9;

				struct 
				{
					//! Private GDI Device Context. Get if for example with:
					//! HDC h = reinterpret_cast<HDC>(exposedData.Win32.OGL.HDc)
					void *HDc; 
					//! Permanent Rendering Context. Get if for example with:
					//! HGLRC h = reinterpret_cast<HGLRC>(exposedData.Win32.OGL.HRc)
					void *HRc; 		
				} 
				OGL;
			};

			//! Window handle. Get with for example 
			//! HWND h = reinterpret_cast<HWND>(exposedData.Win32.HWnd)
			void *HWnd;
		} 
		Win32;
		struct 
		{
			// TODO
		}
		Linux;
	};

	core::dimension2d<s32> WindowSize;
	bool                   FullScreen;
	u32                    Bits;

	vid::E_DRIVER_TYPE     DriverType;
};

} // end namespace vid

//---------------------------------------------------------------------------

namespace os {

struct SExposedOSData
{
	core::stringw Name;
	E_OS_VERSION  Version;
};

} // end namespace os

//---------------------------------------------------------------------------

namespace dev {

struct SExposedDeviceData
{
	vid::SExposedVideoData Video;
	os::SExposedOSData     OS;
};

} // end namespace dev

//---------------------------------------------------------------------------
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef SExposedDeviceData_h
