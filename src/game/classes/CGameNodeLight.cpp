//|-------------------------------------------------------------------------
//| File:        CGameNodeLight.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeLight.h"

#include <dev/IDevice.h>
#include <game/IGameManager.h>
#include <io/ILogger.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodeLight::CGameNodeLight(
	scn::ISceneNode* n, SGameNodeParams &params
	) :
IGameNodeLight(n, SGameNodeParams(params, EGNT_LIGHT))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeLight");	
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeLight::~CGameNodeLight()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeLight::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeLight::OnPostDynamic(f64 delta_time_sec)
{    	
}

//----------------------------------------------------------------------------

//! Light registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardLight(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeLight(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardLightCreator;

IGameNode* Create_StandardLight(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardLightCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardLightCloner;

IGameNode* Clone__StandardLight(IGameNode *game_node, bool log_info)
{
	return StandardLightCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardLightLoader;

IGameNode* Load___StandardLight(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardLightLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
