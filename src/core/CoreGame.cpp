//|-------------------------------------------------------------------------
//| File:        CoreGame.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

#include "CoreSingletonsTable.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Create Game node 
__MY_CORE_LIB_API__ game::IGameNode* createGameNodeForSceneNode(
	scn::ISceneNode *node, game::SGameNodeParams &game_node_params)
{
	if (!node)
		return NULL;
	return core::g_SingletonsTable.Ptrs.GameManager->
		addGameNodeForSceneNode(node, game_node_params);
}

//----------------------------------------------------------------------------

//! Destroy Game node 
__MY_CORE_LIB_API__ bool destroyGameNodeForSceneNode(scn::ISceneNode *node)
{
	if (!node)
		return false;
	return core::g_SingletonsTable.Ptrs.GameManager->
		removeGameNodeForSceneNode(node);
}

//----------------------------------------------------------------------------

//! called each time when IGameNode creating
__MY_CORE_LIB_API__ void onCreateGameNode(game::IGameNode* gnode)
{      
	core::g_SingletonsTable.Ptrs.GameManager->onCreateNode(gnode);
}

//----------------------------------------------------------------------------

//! called each time when IGameNode deleting
__MY_CORE_LIB_API__ void onDeleteGameNode(game::IGameNode* gnode)
{
	core::g_SingletonsTable.Ptrs.GameManager->onDeleteNode(gnode);
}

//----------------------------------------------------------------------------

//! called each time when IGameNode died
__MY_CORE_LIB_API__ void onDieGameNode(game::IGameNode* gnode)
{
	core::g_SingletonsTable.Ptrs.GameManager->onDieNode(gnode);
}

//----------------------------------------------------------------------------

//! creating game inventory for node
__MY_CORE_LIB_API__ game::IGameInventory* createGameInventory(game::IGameNode *owner)
{
	return core::g_SingletonsTable.Ptrs.GameManager->createGameInventory(owner);
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ bool addUnitToFormation(
	game::IGameNodeAI *unit, game::IGameUnitsFormation *formation)
{
	if (!formation)
		return false;
	unit->removeFromFormation();
	if (formation->addUnit(unit))
	{
		unit->m_Formation = formation;		
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void removeUnitFromFormation(game::IGameNodeAI* unit)
{
	if (unit->m_Formation)
		unit->m_Formation->removeUnit(unit);
	unit->m_Formation = NULL;
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void updateGameNode(game::IGameNode *game_node)
{
	game_node->update();
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ const c8* GetGameActionName(game::E_GAME_ACTION action)
{
	return game::GameActionNameStr[action];
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------



