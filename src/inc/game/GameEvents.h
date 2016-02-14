//|-------------------------------------------------------------------------
//| File:        GameEvents.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SGameEventHPP
#define SGameEventHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SGameEvent
{
	friend void deleteGameEvent(SGameEvent* ge);

private:

	virtual ~SGameEvent() 
	{
	}

public:

	SGameEvent() 
		: Source(NULL), Dest(NULL),	Handled(false)
	{
	}

	SGameEvent(const SGameEvent *e)
		: Handled(false)
	{
		set(e);
	}

	SGameEvent(
		E_GAME_EVENT_TYPE type, 
		IGameNode *source, IGameNode *dest, 
		const core::aabbox3df& bbox
		)
		: Type(type), Source(source), Dest(dest), Area(bbox), Handled(false)
	{
	}

	E_GAME_EVENT_TYPE Type;
	core::aabbox3df Area;
	IGameNode *Source, *Dest;
	bool Handled;

	void set(const SGameEvent *e)
	{
		if (!e)
			return;

		Type  = e->Type;
		Area  = e->Area;
		Source= e->Source;
		Dest  = e->Dest;
	}

	core::list< core::list< SGameEvent* >::iterator >::iterator SourceIterator;
	core::list< core::list< SGameEvent* >::iterator >::iterator DestIterator;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef SGameEventHPP 