//|-------------------------------------------------------------------------
//| File:        SKeyMap.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SKeyMapHPP
#define SKeyMapHPP

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Struct storing which key belongs to which action.
struct SKeyMap
{
	game::E_GAME_ACTION Action;
    E_KEY_CODE KeyCode;
};

//---------------------------------------------------------------------------

//! Struct storing which mouse3 button belongs to which action.
struct SMouseMap
{
	game::E_GAME_ACTION Action;
    E_MOUSE_CODE MouseCode;
};

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------

#endif

