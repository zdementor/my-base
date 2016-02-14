//|-------------------------------------------------------------------------
//| File:        CGameNodeMainPlayer.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodeMainPlayer.h"

#include <dev/IDevice.h>
#include <game/IGameManager.h>
#include <io/ILogger.h>
#include <io/IInputDispatcher.h>
#include <mm/ISoundDriver.h>
#include <mm/ISound.h>
#include <game/IGameNodeWeapon.h>
#include <scn/ISceneManager.h>
#include <scn/ILightSceneNode.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodeMainPlayer::CGameNodeMainPlayer( 
	scn::ISceneNode* n, SGameNodeParams &params 
	) : 
IGameNodeMainPlayer(n, SGameNodeParams(params, EGNT_MAIN_PLAYER))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeMainPlayer");	
#endif

	if (m_SceneNode)
		m_SceneNode->setCurrentAnimation(0);
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeMainPlayer::~CGameNodeMainPlayer()
{
}

//----------------------------------------------------------------------------

//! binding new weapon to the bone and index
bool CGameNodeMainPlayer::bindWeapon(
	s32 number, IGameNodeWeapon* weapon, const c8 *target_bone
	)
{
	return false;
}

//----------------------------------------------------------------------------

//! selecting to use weapon, by its index
bool CGameNodeMainPlayer::selectWeapon(s32 number, bool select)
{
	return false;
}

//----------------------------------------------------------------------------

//! selecting to use previous weapon
bool CGameNodeMainPlayer::selectPrevWeapon()
{
	return false;
}

//----------------------------------------------------------------------------

//! selecting to use next weapon
bool CGameNodeMainPlayer::selectNextWeapon()
{
	return false;
}

//----------------------------------------------------------------------------

//! fire weapon
bool CGameNodeMainPlayer::fireWeapon(bool fire)
{
	return false;
}

//----------------------------------------------------------------------------

//! return true if firing weapon, or false
bool CGameNodeMainPlayer::isFiring()
{
	return false;
}

//----------------------------------------------------------------------------

//! Main Player registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardMainPlayer(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeMainPlayer(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardMainPlayerCreator;

IGameNode* Create_StandardMainPlayer(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardMainPlayerCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardMainPlayerCloner;

IGameNode* Clone__StandardMainPlayer(IGameNode *game_node, bool log_info)
{
	return StandardMainPlayerCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardMainPlayerLoader;

IGameNode* Load___StandardMainPlayer(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardMainPlayerLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
