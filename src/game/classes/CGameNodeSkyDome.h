//|-------------------------------------------------------------------------
//| File:        CGameNodeSkyDome.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameSkyDomeHPP
#define CGameSkyDomeHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeSkyDome.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node SkyDome Class
class CGameNodeSkyDome : public IGameNodeSkyDome
{

public:

	//! constructor
	CGameNodeSkyDome(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeSkyDome();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);

private:

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameSkyDomeHPP