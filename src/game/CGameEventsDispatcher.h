//|-------------------------------------------------------------------------
//| File:        CGameEventsDispatcher.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameEventsDispatcherHPP
#define CGameEventsDispatcherHPP
//----------------------------------------------------------------------------

#include <game/IGameEventsDispatcher.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Events Dispatcher
class __MY_GAME_LIB_API__ CGameEventsDispatcher : public IGameEventsDispatcher
{
public:

	//! constructor
	CGameEventsDispatcher();

	//! destructor
	virtual ~CGameEventsDispatcher();
	
	//! add game event
	virtual void registerGameEvent(SGameEvent *event);

	//! dispatching game events
	virtual void dispatchGameEvents();

	//! getting game events count 
	virtual s32 getGameEventsCount();

	virtual void clearGameEvents();

	//! calling on deleting game node
	virtual void onDeleteNode(IGameNode* gnode);

private:

	//! validating game events pool
	void validateGameEvents();

	//! generating game ai events, and extracting them from other events pool
	void generateGameAIEvents();

	//! generating game tasks from events
	void generateGameTasks();	

	core::list<SGameEvent*> NewGameEvents;
	core::list<SGameEvent*> GameEvents;

	s32 GameEventsCount;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameEventsDispatcherHPP