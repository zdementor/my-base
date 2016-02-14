//|-------------------------------------------------------------------------
//| File:        CGameTasksManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameTasksManager.h"
#include "CGameManager.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameTasksManager::CGameTasksManager()
: GameTasksCount(0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameTasksManager");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameTasksManager::~CGameTasksManager() 
{
	clearGameTasks();
}

//---------------------------------------------------------------------------

void deleteGameTask(SGameTask* gtask)
{
	if (!gtask)
		return;

	if (gtask->Sender && gtask->SenderIterator!=0)
	{
		gtask->Sender->getOutputGameTasksIterators().erase(gtask->SenderIterator);
	}

	if (gtask->Receiver && gtask->ReceiverIterator!=0)
	{
		gtask->Receiver->getInputGameTasksIterators().erase(gtask->ReceiverIterator);
	}				
				
	delete gtask;
}

//---------------------------------------------------------------------------

//! register game action in the tasks pool
void CGameTasksManager::registerGameTask(SGameTask *gtask)
{
	if (!gtask) return;

	NewGameTasksPool[gtask->getType()].push_back(gtask);
}

//---------------------------------------------------------------------------

//! validating game tasks pool
void CGameTasksManager::validateGameTasks()
{
	// adding new tasks

	for (s32 gttype=0; gttype<E_GAME_TASK_TYPE_COUNT; gttype++)
	{
		core::list<SGameTask*>::iterator iter_new = NewGameTasksPool[gttype].begin();
	
		for	(; iter_new!= NewGameTasksPool[gttype].end(); ++iter_new) 
		{
			SGameTask* gt_new = (*iter_new);

			if (gt_new->Completed)
				continue;

			bool need_to_add = true;

			if ( gttype==EGTT_FIND_PATH_AND_MOVE || gttype==EGTT_ORIENT )
			{	
				core::list<SGameTask*>::iterator iter = GameTasksPool[gttype].begin();

				for	(; iter!= GameTasksPool[gttype].end(); ++iter) 
				{
					SGameTask* gt = (*iter);

					if (!gt) continue;				

					if (gt->Receiver && gt->Receiver == gt_new->Receiver)
					{
						if (gttype==EGTT_FIND_PATH_AND_MOVE)
						{
							// leave old task	
							
							IGameNodeAI* ai_gnode = SAFE_CAST_TO_GAME_NODE_AI(gt->Receiver);

							SGameTaskFindPathMove* movetask_new = static_cast<SGameTaskFindPathMove*>(gt_new);
							SGameTaskFindPathMove* movetask_old = static_cast<SGameTaskFindPathMove*>(gt);

							// setting new dest pos
							movetask_old->DestPos = movetask_new->DestPos;

							deleteGameTask( gt_new );
						}
						else if (gttype==EGTT_ORIENT)
						{
							// leave old orient task

							deleteGameTask( gt_new );
						}

						need_to_add = false;
						
						break;
					}					
				}				
			}

			if (need_to_add)
			{
				GameTasksPool[gttype].push_back(gt_new);

				if (gt_new->Sender)
				{
					core::list< core::list< SGameTask* >::iterator >& 
						output_tasks = gt_new->Sender->getOutputGameTasksIterators();

					output_tasks.push_back( GameTasksPool[gttype].get_last() );

					gt_new->SenderIterator = output_tasks.get_last();
				}

				if (gt_new->Receiver)
				{
					core::list< core::list< SGameTask* >::iterator >& 
						input_tasks = gt_new->Receiver->getInputGameTasksIterators();

					input_tasks.push_back( GameTasksPool[gttype].get_last() );

					gt_new->ReceiverIterator = input_tasks.get_last();
				}
			}
		}			

		NewGameTasksPool[gttype].clear();

		// make all tasks uncompleted and unhandled

		core::list<SGameTask*>::iterator it = GameTasksPool[gttype].begin();
	
		for	(; it!= GameTasksPool[gttype].end(); ) 
		{
			SGameTask* gtask = (*it);

			if (gtask->Completed)
			{
				deleteGameTask( gtask ); 
				it = GameTasksPool[gttype].erase(it);   				
			}
			else
			{
				++it;
			}

			gtask->Completed = false;
			gtask->Handled   = false;
		}
	}
}

//---------------------------------------------------------------------------

//! executing game tasks pool
void CGameTasksManager::performGameTasks(bool defered)
{
	core::list<SGameTask*>::iterator iter;

	// check load scene tasks

	core::list<SGameTask*>::iterator it = GameTasksPool[EGTT_LOAD_SCENE].begin();

	for (; it!= GameTasksPool[EGTT_LOAD_SCENE].end(); ++it) 
	{
		SGameTask* gtask = *it;

		gtask->Handled = true;

		if (defered!=gtask->Defered)
			continue;

		SGameTaskLoadScene* loadaction = 
			static_cast<SGameTaskLoadScene*>(gtask);

		loadaction->Completed = true; 

		GAME_MANAGER.loadGameScene(loadaction->FileName.c_str());		

		break;
	}		
}

//---------------------------------------------------------------------------

void CGameTasksManager::removeCompletedTasks()
{
	GameTasksCount = 0;

	core::list<SGameTask*>::iterator iter;

	for	( s32 i = 0; i < E_GAME_TASK_TYPE_COUNT; i++ ) 
	{
		GameTasksCount += GameTasksPool[i].size();

		for	(iter = GameTasksPool[i].begin(); iter!= GameTasksPool[i].end();) 
		{			
			SGameTask* gtask = (*iter);

			if (gtask->Completed || !gtask->Handled)
			{				
				deleteGameTask( gtask );

				iter = GameTasksPool[i].erase(iter);
			}
			else
				++iter;
		}
	}	
}

//---------------------------------------------------------------------------

void CGameTasksManager::clearGameTasks()
{
	core::list<SGameTask*>::iterator iter;

	for	( s32 i = 0; i < E_GAME_TASK_TYPE_COUNT; i++ ) 
	{
		for	(iter = GameTasksPool[i].begin(); iter!= GameTasksPool[i].end(); ++iter) 
		{
			deleteGameTask( *iter );
		}

		GameTasksPool[i].clear();

		for	(iter = NewGameTasksPool[i].begin(); iter!= NewGameTasksPool[i].end(); ++iter) 
		{
			deleteGameTask( *iter );	
		}

		NewGameTasksPool[i].clear();
	}
}

//---------------------------------------------------------------------------

void  CGameTasksManager::onDeleteNode(IGameNode* gnode)
{
	core::list<SGameTask*>::iterator iter;
	for	( s32 i = 0; i < E_GAME_TASK_TYPE_COUNT; i++ ) 
	{    		
		for	(iter = NewGameTasksPool[i].begin(); iter!= NewGameTasksPool[i].end(); ++iter) 
		{
			SGameTask* gtask = (*iter); 
			if (gtask->Sender == gnode || gtask->Receiver == gnode)
				gtask->Completed = true;	
		}
	}
	gnode->clearAllTasks();
}

//---------------------------------------------------------------------------

//! returns game tasks count
s32 CGameTasksManager::getGameTasksCount()
{	
	return GameTasksCount;
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

