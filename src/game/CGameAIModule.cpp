//|-------------------------------------------------------------------------
//| File:        CGameAIModule.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameAIModule.h"
#include "CGameManager.h"
 
//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameAIModule::CGameAIModule()
: GameAIEventsCount(0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameAIModule");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameAIModule::~CGameAIModule() 
{
	GameAIEvents.clear();
}

//----------------------------------------------------------------------------

//! add new gane ai event
void CGameAIModule::addGameAIEvent(SGameAIEvent &ai_event)
{
	if (!ai_event.Source || !ai_event.Dest || 
		!ai_event.Source->isLive() || !ai_event.Dest->isLive()
		)
		return;

	GameAIEvents.push_back(ai_event);
}

//----------------------------------------------------------------------------

//! generating game tasks from events
void CGameAIModule::doAI()
{
	core::list<SGameAIEvent>::iterator iter_events = GameAIEvents.begin();

	for (; iter_events != GameAIEvents.end(); ++iter_events )
	{
		SGameAIEvent& event = *iter_events;

		core::vector3df pos = event.Area.getCenter();
		core::vector3df tgt = event.Dest->getDislocation();
		core::vector3df new_dir = pos-tgt;
		f32 dist = new_dir.getLength();
		core::vector3df cur_dir = event.Dest->getLookDir();
		f32 angle = cur_dir.getAngleDeg(new_dir);

		const SGameNodeAIParams& ai_pars = event.Dest->getParametersAI();

		f32 attack_range_2 = 2.0f*ai_pars.MeleAttackRange;
		f32 attack_range_4 = 2.0f*attack_range_2;

		if (event.Type == EGAIET_ENEMY_VISUAL_CONTACT)
		{			
			bool can_mele_attack = false;

			if (event.Dest->getState()==EAIS_IDLE || event.Dest->getState()==EAIS_SEEKING)
			{	
				// attack if can

				if (event.Dest->isReadyToAttack(event.Source, can_mele_attack))
				{	
					SGameTaskAttack *attacktask = new SGameTaskAttack(NULL, event.Dest);
					attacktask->Victim = event.Source;
					attacktask->CanMeleAttack = can_mele_attack;
					attacktask->VectorAttack = new_dir;

					GAME_TASK_MANAGER.registerGameTask( attacktask ); 
				}
				else
				{
					// trying to cross road
				
					core::vector3df dest_pos = 
						pos + event.Source->getMoveDir()*(dist>attack_range_4?attack_range_2:dist);

					if (!PATH_FINDER_MANAGER.isWalkableLine(pos, dest_pos))
					{
						// just folow him
						dest_pos = pos;
					}

					SGameTaskFindPathMove *movetask = new SGameTaskFindPathMove(event.Source, event.Dest);
					movetask->DestPos = dest_pos;

					GAME_TASK_MANAGER.registerGameTask( movetask );  	
				}
			}
			
			event.Dest->OnEnemyDetected();
		}
		else 
		if (event.Type == EGAIET_ENEMY_SOUND_CONTACT)
		{
			if (event.Dest->getState()==EAIS_IDLE &&		
				angle>ai_pars.MeleAttackAngle/2.0f
				)
			{
				f32 r = event.Area.getExtend().getLength() / 2.0f;
				s32 r_d = 10 - 10.0f*dist/r;

				CHECK_RANGE(r_d, 0, 10);

				f32 heared = false;

				for (s32 i=0; i<r_d; i++)
				{
					f32 prob = core::math::UnitRandom();

					if (prob>0.85f)
					{
						heared = true;
						break;
					}
				}	

				if (heared)
				{
					game::SGameTaskOrient *gt = new game::SGameTaskOrient(NULL, event.Dest);
					gt->OrientDir = new_dir;

					GAME_TASK_MANAGER.registerGameTask(gt);  
				}
			}

			event.Dest->OnEnemyDetected();
		}
		else 
		if (event.Type == EGAIET_ENEMY_PHYSIC_CONTACT)
		{
			if (angle>ai_pars.MeleAttackAngle/2.0f)
			{
				game::SGameTaskOrient *gt = new game::SGameTaskOrient(event.Source, event.Dest);
				gt->OrientDir = new_dir;

				GAME_TASK_MANAGER.registerGameTask(gt);   					
			}

			event.Dest->OnEnemyDetected();
		}
		else if (event.Type == EGAIET_FRIEND_VISUAL_CONTACT)
		{	

		}
		else if (event.Type == EGAIET_FRIEND_SOUND_CONTACT)
		{
		}
		else if (event.Type == EGAIET_FRIEND_PHYSIC_CONTACT)
		{

		}
	}

	GameAIEventsCount = GameAIEvents.size();

	GameAIEvents.clear();
}

//----------------------------------------------------------------------------

//! getting game ai events count 
s32 CGameAIModule::getGameAIEventsCount()
{
	return GameAIEventsCount;
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

