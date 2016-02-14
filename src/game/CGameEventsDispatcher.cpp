//|-------------------------------------------------------------------------
//| File:        CGameEventsDispatcher.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameEventsDispatcher.h"
#include "CGameManager.h"
#include <game/IGameAIModule.h>
#include <game/IGameTasksManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameEventsDispatcher::CGameEventsDispatcher()
: GameEventsCount(0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameEventsDispatcher");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameEventsDispatcher::~CGameEventsDispatcher() 
{	
	clearGameEvents();
}

//---------------------------------------------------------------------------

void deleteGameEvent(SGameEvent* gevent)
{
	if (!gevent)
		return;

	if (gevent->Source && gevent->SourceIterator!=0)
	{
		gevent->Source->getOutputGameEventsIterators().erase(gevent->SourceIterator);
	}

	if (gevent->Dest && gevent->DestIterator!=0)
	{
		gevent->Dest->getInputGameEventsIterators().erase(gevent->DestIterator);
	}				
				
	delete gevent;
}

//----------------------------------------------------------------------------

//! add game event
void CGameEventsDispatcher::registerGameEvent(SGameEvent *event)
{
	if (!event)
		return;

	NewGameEvents.push_back(event);
}

//----------------------------------------------------------------------------

//! dispatching game events
void CGameEventsDispatcher::dispatchGameEvents()
{
	validateGameEvents();
	generateGameAIEvents();
	generateGameTasks();

	core::list<SGameEvent*>::iterator it = GameEvents.begin();

	for (; it!=GameEvents.end(); ++it )
	{
		deleteGameEvent(*it);
	}

	GameEvents.clear();
}

//----------------------------------------------------------------------------

void CGameEventsDispatcher::validateGameEvents()
{
	core::list<SGameEvent*>::iterator it = NewGameEvents.begin();
	
	for	(; it!= NewGameEvents.end(); ++it) 
	{
		SGameEvent* event = (*it);

		if (event->Handled)
		{
			deleteGameEvent(event);
		}
		else
		if (!event->Source)
		{
			deleteGameEvent(event);
		}
		else
		if (!event->Dest)
		{
			core::list<IGameNode*>::iterator iter_gnode = CGameManager::GameNodes.begin();

			for(; iter_gnode!= CGameManager::GameNodes.end(); ++iter_gnode)
			{
				IGameNode* gnode = *iter_gnode;
 
				if (gnode) 
				{
					const core::aabbox3df &tbb = gnode->getTransformedBBox();

					if (event->Source != gnode && tbb.intersectsWithBox(event->Area)) 
					{
						SGameEvent *e = new SGameEvent(event);
						e->Dest = gnode;

						registerGameEvent(e);
					}				
				}
			}

			deleteGameEvent(event);
		}
		else 
		if (event->Dest->isLive() && (
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_PERSON) ||
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_TRIGGER) ||
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_BUTTON) ||
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_DOOR) ||
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_PLATFORM) ||
				(event->Source->getGameNodeType()==EGNT_MAIN_PLAYER && event->Dest->getGameNodeType()==EGNT_ITEM) ||

				(event->Source->getGameNodeType()==EGNT_PERSON      && event->Dest->getGameNodeType()==EGNT_MAIN_PLAYER) ||
				(event->Source->getGameNodeType()==EGNT_PERSON      && event->Dest->getGameNodeType()==EGNT_TRIGGER) ||
				(event->Source->getGameNodeType()==EGNT_PERSON      && event->Dest->getGameNodeType()==EGNT_DOOR) ||
				(event->Source->getGameNodeType()==EGNT_PERSON      && event->Dest->getGameNodeType()==EGNT_PLATFORM) ||
				(event->Source->getGameNodeType()==EGNT_PERSON      && event->Dest->getGameNodeType()==EGNT_ITEM) ||
				
				(event->Source->getGameNodeType()==EGNT_TRIGGER     && event->Dest->getGameNodeType()==EGNT_MAIN_PLAYER) ||			
				(event->Source->getGameNodeType()==EGNT_TRIGGER     && event->Dest->getGameNodeType()==EGNT_PERSON) ||						
				
				(event->Source->getGameNodeType()==EGNT_DOOR        && event->Dest->getGameNodeType()==EGNT_MAIN_PLAYER) ||			
				(event->Source->getGameNodeType()==EGNT_DOOR        && event->Dest->getGameNodeType()==EGNT_PERSON) ||

				(event->Source->getGameNodeType()==EGNT_BUTTON      && event->Dest->getGameNodeType()==EGNT_MAIN_PLAYER) ||

				(event->Source->getGameNodeType()==EGNT_PLATFORM    && event->Dest->getGameNodeType()==EGNT_MAIN_PLAYER) 
				)
			)
		{
			GameEvents.push_back(event);

			core::list< core::list< SGameEvent* >::iterator >& 
				output_events = event->Source->getOutputGameEventsIterators();
			output_events.push_back( GameEvents.get_last() );
			event->SourceIterator = output_events.get_last();
			
			core::list< core::list< SGameEvent* >::iterator >& 
				input_events = event->Dest->getInputGameEventsIterators();
			input_events.push_back( GameEvents.get_last() );
			event->DestIterator = input_events.get_last();
		}
		else
		{
			deleteGameEvent(event);
		}
	}

	NewGameEvents.clear();
}

//----------------------------------------------------------------------------

//! generating game ai events, and extracting them from other events pool
void CGameEventsDispatcher::generateGameAIEvents()
{
	core::list<SGameEvent*>::iterator iter_events = GameEvents.begin();

	for (; iter_events != GameEvents.end(); ++iter_events)
	{
		SGameEvent* event = *iter_events;

		IGameNodeAI *source = SAFE_CAST_TO_GAME_NODE_AI(event->Source);
		IGameNodeAI *dest   = SAFE_CAST_TO_GAME_NODE_AI(event->Dest);		

		if (!source || !dest || source==dest || dest->isMainPlayer()) 
			continue;				

		bool enemies = source->isWarWith(dest);			

		SGameAIEvent ai_event(EGAIET_ENEMY_VISUAL_CONTACT, source, dest, event->Area);

		if (event->Type == EGET_SOUND)
		{
			if (enemies)
			{
				ai_event.Type = EGAIET_ENEMY_SOUND_CONTACT;
			}
			else
			{
				ai_event.Type = EGAIET_FRIEND_SOUND_CONTACT;
			}
		}
		else if (event->Type == EGET_PHYSIC)
		{
			if (enemies)
			{
				ai_event.Type = EGAIET_ENEMY_PHYSIC_CONTACT;
			}
			else
			{
				ai_event.Type = EGAIET_FRIEND_PHYSIC_CONTACT;
			}
		}

		IGameAIModule::getSingleton().addGameAIEvent(ai_event);

		event->Handled = true;	
	}

	// sending main player appearence event

	core::list<IGameNode*>::iterator player_iter = 
		CGameManager::GameNodesByType[EGNT_MAIN_PLAYER].begin();

	for (; player_iter!= CGameManager::GameNodesByType[EGNT_MAIN_PLAYER].end(); ++player_iter)
	{
		IGameNode *gnode = (*player_iter);

		if (!gnode)
			continue;	

		IGameNodeAI *player = SAFE_CAST_TO_GAME_NODE_AI(gnode);	

		if (!player)
			continue;
		
		core::aabbox3df bb = gnode->getTransformedBBox();

		core::list<IGameNode*>::iterator person_iter = 
			CGameManager::GameNodesByType[EGNT_PERSON].begin();

		for	(; person_iter!= CGameManager::GameNodesByType[EGNT_PERSON].end(); ++person_iter) 
		{
			gnode = (*person_iter);

			IGameNodeAI *person = SAFE_CAST_TO_GAME_NODE_AI(gnode);

			if (person && person->isCanSee(player))
			{
				SGameAIEvent ai_event(EGAIET_ENEMY_VISUAL_CONTACT, player, person, bb);	

				IGameAIModule::getSingleton().addGameAIEvent(ai_event);				
			}			
		}
	}
}

//----------------------------------------------------------------------------

//! generating game tasks from events
void CGameEventsDispatcher::generateGameTasks()
{
	// generationg activate task

	core::list<SGameEvent*>::iterator iter_events = GameEvents.begin();

	for (; iter_events != GameEvents.end(); ++iter_events )
	{
		SGameEvent* event = *iter_events;

		if (event->Handled)
			continue;
			
		if (event->Type == EGET_SOUND)
		{
		}
		else if (event->Type == EGET_PHYSIC)
		{
			// registering task activation
			GAME_TASK_MANAGER.registerGameTask(
				new game::SGameTaskActivate(event->Source, event->Dest)
				); 
		}

		event->Handled = true;
	}

	GameEventsCount = GameEvents.size();
	  
	// generating die tasks

	core::list<IGameNode*>::iterator iter = CGameManager::GameNodes.begin();  
	for (; iter!= CGameManager::GameNodes.end(); ++iter)
	{
		IGameNode* gnode = *iter;    
		if (gnode->isLive() && gnode->getLife()<=0 && gnode->getMaxLife()>0)
		{
			SGameTaskDie *dietask = new SGameTaskDie(NULL, gnode);  
			GAME_TASK_MANAGER.registerGameTask( dietask );  
		}
	}  
}

//----------------------------------------------------------------------------

//! getting game ai events count 
s32 CGameEventsDispatcher::getGameEventsCount()
{
	return GameEventsCount;
}

//----------------------------------------------------------------------------

void CGameEventsDispatcher::clearGameEvents()
{
	core::list<SGameEvent*>::iterator it = GameEvents.begin();

	for (; it!=GameEvents.end(); ++it )
	{
		deleteGameEvent(*it);
	}

	GameEvents.clear();

	for	(it = NewGameEvents.begin(); it!= NewGameEvents.end(); ++it) 
	{
		deleteGameEvent(*it);		
	}

	NewGameEvents.clear();
}

//---------------------------------------------------------------------------

void  CGameEventsDispatcher::onDeleteNode(IGameNode* gnode)
{
	core::list<SGameEvent*>::iterator it = NewGameEvents.begin();  
	for	(; it!= NewGameEvents.end(); ++it) 
	{
		SGameEvent* event = *it;
		if (event->Dest == gnode || event->Source == gnode)
			event->Handled = true;
	}
	gnode->clearAllEvents();
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

