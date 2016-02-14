//|-------------------------------------------------------------------------
//| File:        CGameNodeTerrain.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameTerrainHPP
#define CGameTerrainHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeTerrain.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Terrain Class
class CGameNodeTerrain : public IGameNodeTerrain
{

public:

	//! constructor
	CGameNodeTerrain(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeTerrain();

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

#endif // #ifndef CGameTerrainHPP