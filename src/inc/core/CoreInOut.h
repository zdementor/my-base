//|-------------------------------------------------------------------------
//| File:        CoreInOut.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CoreInOutHPP
#define CoreInOutHPP

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void log(const c8 *text, io::E_LOG_LEVEL ll);

__MY_CORE_LIB_API__ io::E_KEY_CODE win2MyKeyCode(u8 kcode);

__MY_CORE_LIB_API__ const c8* getKeyCodeName(io::E_KEY_CODE kcode);

__MY_CORE_LIB_API__ const c8* getMouseCodeName(io::E_MOUSE_CODE mcode);

__MY_CORE_LIB_API__ bool getActionControlState(
	io::IEventCapturer *capturer, game::E_GAME_ACTION action);

__MY_CORE_LIB_API__ void resetActionControlState(
	io::IEventCapturer *capturer, game::E_GAME_ACTION action);

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CoreInOutHPP