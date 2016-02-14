//|-------------------------------------------------------------------------
//| File:        CGameNodeTerrain.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodeTerrain.h"

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodeTerrain::CGameNodeTerrain(
	scn::ISceneNode* n, SGameNodeParams &params
	) :
IGameNodeTerrain(n, SGameNodeParams(params, EGNT_TERRAIN))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeTerrain");	
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeTerrain::~CGameNodeTerrain()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeTerrain::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeTerrain::OnPostDynamic(f64 delta_time_sec)
{    	
}

//----------------------------------------------------------------------------

//! Terrain registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardTerrain(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeTerrain(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardTerrainCreator;

IGameNode* Create_StandardTerrain(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardTerrainCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardTerrainCloner;

IGameNode* Clone__StandardTerrain(IGameNode *game_node, bool log_info)
{
	return StandardTerrainCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

class CStandardTerrainLoader : public CGameNodeBaseLoader
{
public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);
		_loadGameNode(node_params, false);	
		_postLoadGameNode(node_params);		

		return GameNode;
	}
}
StandardTerrainLoader;

IGameNode* Load___StandardTerrain(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardTerrainLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
