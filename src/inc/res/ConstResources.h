//|-------------------------------------------------------------------------
//| File:        ConstResources.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_resHPP
#define my_const_resHPP

//----------------------------------------------------------------------------
namespace my {
namespace res {
//----------------------------------------------------------------------------

//! enumeration for media types
enum E_MEDIA_TYPE
{
	EMT_ROOT = 0,

	EMT_MESHES,

	EMT_TEXTURES,
	EMT_FONTS,
	EMT_MATERIALS,
	EMT_GPU_PROGRAMS,

	EMT_GAME_SCRIPTS,

	EMT_XML_SCRIPTS,

	EMT_SOUND_TRACKS,
	EMT_SOUND_EFFECTS,

	EMT_TEMP_DATA,

	//! for counting enumeration
	E_MEDIA_TYPE_COUNT,

	//! not used
	E_MEDIA_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace res
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_resHPP