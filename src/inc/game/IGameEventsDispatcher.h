//|-------------------------------------------------------------------------
//| File:        IGameEventsDispatcher.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameEventsDispatcherHPP
#define IGameEventsDispatcherHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>
#include <game/GameEvents.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Events Dispatcher
class __MY_GAME_LIB_API__ IGameEventsDispatcher :
	public IUnknown, public ISingleton<IGameEventsDispatcher>
{
public:

	//! add game event
	virtual void registerGameEvent(SGameEvent *event) = 0;

	//! dispatching game events
	virtual void dispatchGameEvents() = 0;

	//! getting game events count 
	virtual s32 getGameEventsCount() = 0;

	virtual void clearGameEvents() = 0;

	//! calling on deleting game node
	virtual void onDeleteNode(IGameNode* gnode) = 0;

	//!	Return singleton object
	static IGameEventsDispatcher& getSingleton();

	//! Return pointer to singleton object
	static IGameEventsDispatcher* getSingletonPtr();	
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameEventsDispatcherHPP