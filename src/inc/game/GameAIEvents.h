//|-------------------------------------------------------------------------
//| File:        GameAIEvents.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SGameAIEventHPP
#define SGameAIEventHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeAI.h>
#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SGameAIEvent
{
	SGameAIEvent() : Source(NULL), Dest(NULL) 
	{
		//setClassName("SGameAIEvent");
	}
	SGameAIEvent(
		E_GAME_AI_EVENT_TYPE type, 
		IGameNodeAI *source, IGameNodeAI *dest, 
		core::aabbox3df &bbox
		)
		: Type(type), Source(source), Dest(dest), Area(bbox) 
	{
		//setClassName("SGameAIEvent");
	}

	E_GAME_AI_EVENT_TYPE Type;
	core::aabbox3df Area;
	IGameNodeAI *Source, *Dest;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef SGameAIEventHPP 