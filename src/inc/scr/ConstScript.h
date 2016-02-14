//|-------------------------------------------------------------------------
//| File:        ConstScript.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_scrHPP
#define my_const_scrHPP

//---------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

enum E_SCRIPT_CALLBACK_TYPE
{
	// basic MyEngine events
	ESCT_ON_IO_EVENT = 0,
	ESCT_ON_BEFORE_LOAD_SCENE,
	ESCT_ON_AFTER_LOAD_SCENE,
	ESCT_ON_BEFORE_SAVE_SCENE,
	ESCT_ON_AFTER_SAVE_SCENE,
	ESCT_ON_DELETE_GAME_NODE,
	ESCT_ON_CREATE_GAME_NODE,
	ESCT_ON_DIE_GAME_NODE,
	ESCT_ON_COLLECT_GAME_ITEM,
	ESCT_ON_THROW_GAME_ITEM,
	ESCT_ON_USE_GAME_ITEM,
	ESCT_ON_GAME_STARTED,
	ESCT_ON_GAME_STOPPED,
	ESCT_ON_ANIMATION_BEGIN,
	ESCT_ON_ANIMATION_END,

	// Scene Editor calbacks
	ESCT_SCENE_EDITOR_ON_DELETE_SCENE_NODE,
	ESCT_SCENE_EDITOR_ON_CREATE_SCENE_NODE,
	ESCT_SCENE_EDITOR_ON_SELECT_SCENE_NODE,
	ESCT_SCENE_EDITOR_ON_DESELECT_SCENE_NODE,
	ESCT_SCENE_EDITOR_ON_BEFORE_LOAD_SCENE,
	ESCT_SCENE_EDITOR_ON_AFTER_LOAD_SCENE,
	ESCT_SCENE_EDITOR_ON_BEFORE_SAVE_SCENE,
	ESCT_SCENE_EDITOR_ON_AFTER_SAVE_SCENE,
	ESCT_SCENE_EDITOR_ON_GAME_STARTED,
	ESCT_SCENE_EDITOR_ON_GAME_STOPPED,
	ESCT_SCENE_EDITOR_ON_ANIMATION_BEGIN,
	ESCT_SCENE_EDITOR_ON_ANIMATION_END,

	// Action callbacks
	ESCT_SHOW_MESSAGE,
	ESCT_GEN_GPU_PROGRAM,
	ESCT_GET_GEN_GPU_PROGRAM_MAX_LIGHTS,
	ESCT_GET_GEN_GPU_PROGRAM_TAG,

	ESCT_SCRIPT_CALLBACK_TYPE_COUNT,

	ESCT_SCRIPT_CALLBACK_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_scrHPP