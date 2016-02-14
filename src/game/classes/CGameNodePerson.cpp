//|-------------------------------------------------------------------------
//| File:        CGameNodePerson.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodePerson.h"

#include <dev/IDevice.h>
#include <game/IGameTasksManager.h>
#include <io/ILogger.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodePerson::CGameNodePerson(
	scn::ISceneNode* n, SGameNodeParams &params
	) :
IGameNodePerson(n, SGameNodeParams(params, EGNT_PERSON))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodePerson");	
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodePerson::~CGameNodePerson()
{
}

//----------------------------------------------------------------------------

void CGameNodePerson::moveBy(core::vector3df dir, f32 speed)
{
}

//----------------------------------------------------------------------------

void CGameNodePerson::orientTo(core::vector3df dir, f32 speed)
{
}

//----------------------------------------------------------------------------

//! Person registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardPerson(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodePerson(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardPersonCreator;

IGameNode* Create_StandardPerson(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardPersonCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardPersonCloner;

IGameNode* Clone__StandardPerson(IGameNode *game_node, bool log_info)
{
	return StandardPersonCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardPersonLoader;

IGameNode* Load___StandardPerson(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardPersonLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
