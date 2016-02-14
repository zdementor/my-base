//|-------------------------------------------------------------------------
//| File:        CoreGame.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CoreGameHPP
#define CoreGameHPP

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Create Game node 
__MY_CORE_LIB_API__ game::IGameNode* createGameNodeForSceneNode(
	scn::ISceneNode *node, game::SGameNodeParams &game_node_params
	);
//! Destroy Game node 
__MY_CORE_LIB_API__ bool destroyGameNodeForSceneNode(scn::ISceneNode *node);

//! called each time when IGameNode creating
__MY_CORE_LIB_API__ void onCreateGameNode(game::IGameNode* gnode);

//! called each time when IGameNode deleting
__MY_CORE_LIB_API__ void onDeleteGameNode(game::IGameNode* gnode);

//! called each time when IGameNode died
__MY_CORE_LIB_API__ void onDieGameNode(game::IGameNode* gnode);

//! creating game inventory for node
__MY_CORE_LIB_API__ game::IGameInventory* createGameInventory(game::IGameNode *owner);

__MY_CORE_LIB_API__ bool addUnitToFormation(game::IGameNodeAI* unit, game::IGameUnitsFormation* formation);

__MY_CORE_LIB_API__ void removeUnitFromFormation(game::IGameNodeAI* unit);

__MY_CORE_LIB_API__ void updateGameNode(game::IGameNode* game_node);

__MY_CORE_LIB_API__ const c8* GetGameActionName(game::E_GAME_ACTION action);

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef 