//|-------------------------------------------------------------------------
//| File:        CGameNodeSkyDome.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeSkyDome.h"

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodeSkyDome::CGameNodeSkyDome(
	scn::ISceneNode* n, SGameNodeParams &params
	) :
IGameNodeSkyDome(n, SGameNodeParams(params, EGNT_SKY_DOME))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeSkyDome");	
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeSkyDome::~CGameNodeSkyDome()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeSkyDome::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeSkyDome::OnPostDynamic(f64 delta_time_sec)
{    	
}

//----------------------------------------------------------------------------

//! SkyDome registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardSkyDome(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeSkyDome(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardSkyDomeCreator;

IGameNode* Create_StandardSkyDome(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardSkyDomeCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardSkyDomeCloner;

IGameNode* Clone__StandardSkyDome(IGameNode *game_node, bool log_info)
{
	return StandardSkyDomeCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardSkyDomeLoader;

IGameNode* Load___StandardSkyDome(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardSkyDomeLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
