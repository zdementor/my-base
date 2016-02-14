//|-------------------------------------------------------------------------
//| File:        ConstDevice.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_devHPP
#define my_const_devHPP

//---------------------------------------------------------------------------
namespace my {
namespace dev {
//---------------------------------------------------------------------------

//! my engine device creation flags
enum E_DEVICE_CREATION_FLAG
{
	EDCF_FULLSCREEN					= 1<<0,
	EDCF_ANTIALIASING				= 1<<1,
	EDCF_USE_FFP					= 1<<2, // use FFP or not
	EDCF_USE_SHADERS				= 1<<3, // use shaders or not
	EDCF_SHADERS_HIGH_QUALITY		= 1<<4, // only works with EDCF_USE_SHADERS turned on
	EDCF_CACHE_SHADERS				= 1<<5, // only works with EDCF_USE_SHADERS turned on
	EDCF_VERTICAL_SYNC				= 1<<6,
	EDCF_SHADOWS					= 1<<7,
	EDCF_LIGHTING_MEDIUM_QUALITY	= 1<<8,
	EDCF_LIGHTING_HIGH_QUALITY		= 1<<9,
	EDCF_SOUND_ON					= 1<<10,
	EDCF_HI_MAPS_DETAIL				= 1<<11,
	EDCF_HI_ANIMATION_QUALITY		= 1<<12,
	EDCF_DIRT_ON_LEVEL				= 1<<13,
	EDCF_NO_SYSTEM_MENU				= 1<<14,
	EDCF_NO_TITLE_BAR				= 1<<15,
	EDCF_ALWAYS_ON_TOP				= 1<<16,
	EDCF_SOFTWARE_RASTERIZER		= 1<<17,
	EDCF_ROUNDED_FRAME_RECTANGLE	= 1<<18,
	EDCF_DRAG_USING_TOP_AREA		= 1<<19,

	//! not used
	E_DEVICE_CREATION_FLAG_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_devHPP