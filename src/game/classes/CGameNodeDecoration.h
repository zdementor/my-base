//|-------------------------------------------------------------------------
//| File:        CGameNodeDecoration.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeDecorationHPP
#define CGameNodeDecorationHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeDecoration.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Decoration Class
class CGameNodeDecoration : public IGameNodeDecoration
{

public:

	//! constructor
	CGameNodeDecoration(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeDecoration();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);

protected:

private:

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeDecorationHPP