//|-------------------------------------------------------------------------
//| File:        CGameAIModule.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameAIModuleHPP
#define CGameAIModuleHPP
//----------------------------------------------------------------------------

#include <game/IGameAIModule.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game AI Module
class __MY_GAME_LIB_API__ CGameAIModule : public IGameAIModule
{
public:

	//! constructor
	CGameAIModule();

	//! destructor
	virtual ~CGameAIModule();	

	//! add new gane ai event
	virtual void addGameAIEvent(SGameAIEvent &ai_event);

	//! generating game tasks from events
	virtual void doAI();

	//! getting game ai events count 
	virtual s32 getGameAIEventsCount();

private:

	core::list<SGameAIEvent> GameAIEvents;

	s32 GameAIEventsCount;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameAIModuleHPP