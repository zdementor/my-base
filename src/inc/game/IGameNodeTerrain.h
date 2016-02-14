//|-------------------------------------------------------------------------
//| File:        IGameNodeTerrain.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeTerrainHPP
#define IGameNodeTerrainHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Fromation Class
class __MY_GAME_LIB_API__ IGameNodeTerrain : public IGameNode
{
public:

	//! constructor
	IGameNodeTerrain(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNode(n, params) 
	{
	}

	//! destructor
	virtual ~IGameNodeTerrain() {}

private:

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeTerrainHPP