//|-------------------------------------------------------------------------
//| File:        CGameManagerFunctions.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <io/ILogger.h>
#include <game/IGameManager.h>
#include <game/IGameNode.h>
#include <scn/ISceneManager.h>
#include <res/IResourceManager.h>
#include <game/IGameNodeDoor.h>
#include <game/IGameNodeTrigger.h>
#include <game/IGameTasksManager.h>
#include <io/IFileSystem.h>
#include <scn/IMaterialsManager.h>

#include "CGameManager.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//////////////////////////////////
//!      game functions
//////////////////////////////////

//----------------------------------------------------------------------------

//! return true if trigger activated
bool CGameManager::IsTriggerActivated(s32 id)
{
	s32 gnodecnt = getGameNodesCount(EGNT_TRIGGER);
	scn::ISceneNode* snode = NULL;
	IGameNodeTrigger* trigger = NULL;

	for (s32 i=0; i<gnodecnt; i++)
	{
		IGameNode *gn = getGameNode(EGNT_TRIGGER, i);

		trigger = SAFE_CAST_TO_GAME_NODE_TRIGGER( gn );

		if (trigger)
		{
			snode = trigger->getSceneNode();

			if (snode && snode->getID() == id)
			{
				if (trigger && trigger->isActivated())
					return true;

				break;
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------------

//! return true if door activated
bool CGameManager::IsDoorActivated(s32 id)
{
	s32 gnodecnt = getGameNodesCount(EGNT_DOOR);
	scn::ISceneNode* snode = NULL;
	IGameNodeDoor* door = NULL;

	for (s32 i=0; i<gnodecnt; i++)
	{
		IGameNode *gn = getGameNode(EGNT_DOOR, i);

		door = SAFE_CAST_TO_GAME_NODE_DOOR( gn );

		if (door)
		{
			snode = door->getSceneNode();

			if (snode && snode->getID() == id)
			{
				if (door && door->isActivated())
					return true;

				break;
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------------

//! one game node hited by another
int CGameManager::GameNodeHit(IGameNode *victim, IGameNode *agressor, s32 hit_power)
{
	if (victim)
    {
		SGameTaskHited *hittask = new SGameTaskHited(agressor, victim);
		hittask->HitPower = hit_power;

		GAME_TASK_MANAGER.registerGameTask( hittask );  

        return 1;
    }

	return 0;
}

//----------------------------------------------------------------------------

//! game node death
int CGameManager::GameNodeDie(IGameNode *victim)
{	
	if (victim)
    {
		SGameTaskDie *dietask = new SGameTaskDie(NULL, victim);

		GAME_TASK_MANAGER.registerGameTask( dietask );  

        return 1;
    }

	return 0;
}

//----------------------------------------------------------------------------

//! game node simple move
void CGameManager::GameNodeMove(IGameNode *moving_node, core::vector3df dest_pos)
{
	if (moving_node)
	{
		SGameTaskFindPathMove *movetask = new SGameTaskFindPathMove(NULL, moving_node);
		movetask->DestPos = dest_pos;

		GAME_TASK_MANAGER.registerGameTask( movetask );  
	}
}

//----------------------------------------------------------------------------

//! game node movewith complex trajectory
void CGameManager::GameNodeMoveTrajectory(
	IGameNode *moving_node, core::array<core::vector3df> &way_points
	)
{
	if (moving_node)
	{
		
	}
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------