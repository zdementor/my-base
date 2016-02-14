//|-------------------------------------------------------------------------
//| File:        ConstMultimed.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_mmHPP
#define my_const_mmHPP

//----------------------------------------------------------------------------
namespace my {
namespace mm {
//----------------------------------------------------------------------------	

enum E_SOUND_DRIVER_TYPE
{
	ESDT_NULL,
	ESDT_OPENAL,

	E_SOUND_DRIVER_TYPE_COUNT,

	E_SOUND_DRIVER_TYPE_FORCE_32BIT = 0xffffffff
};

//------------------------------------------------------------------------

enum E_SOUND_DATA_TYPE
{
	ESDT_WAV =0,
	ESDT_OGG,

	E_SOUND_DATA_TYPE_COUNT,

	E_SOUND_DATA_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_mmHPP