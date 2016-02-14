//|-------------------------------------------------------------------------
//| File:        IGameAIModule.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameAIModuleHPP
#define IGameAIModuleHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>
#include <game/IGameNodeAI.h>
#include "GameAIEvents.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game AI Module
class __MY_GAME_LIB_API__ IGameAIModule : public IUnknown, public ISingleton<IGameAIModule>
{
public:

	//! add new gane ai event
	virtual void addGameAIEvent(SGameAIEvent &ai_event) = 0;

	//! generating game tasks from events
	virtual void doAI() = 0;

	//! getting game ai events count 
	virtual s32 getGameAIEventsCount() = 0;

	//!	Return singleton object
	static IGameAIModule& getSingleton();

	//! Return pointer to singleton object
	static IGameAIModule* getSingletonPtr();	
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameAIModuleHPP