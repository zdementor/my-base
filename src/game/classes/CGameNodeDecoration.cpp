//|-------------------------------------------------------------------------
//| File:        CGameNodeDecoration.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodeDecoration.h"

#include <game/IGameNodeDoor.h>
#include <game/IGameNodePlatform.h>
#include <game/IGameNodeTrigger.h>
#include <game/IGameNodeButton.h>
#include <scn/ILightSceneNode.h>
#include <img/IImageLibrary.h>
#include <dyn/IDynamicManager.h>
#include <scn/IPathFinderManager.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameNodeDecoration::CGameNodeDecoration(
	scn::ISceneNode* n, SGameNodeParams &params
	) : 
IGameNodeDecoration(n, SGameNodeParams(params, EGNT_DECORATION))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeDecoration");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeDecoration::~CGameNodeDecoration()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeDecoration::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeDecoration::OnPostDynamic(f64 delta_time_sec)
{    	
}

//----------------------------------------------------------------------------

//! Decoration registry funcs

//----------------------------------------------------------------------------

//! Decoration registry funcs
IGameNode* Alloc__StandardDecoration(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeDecoration(n,params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardDecorationCreator;

IGameNode* Create_StandardDecoration(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardDecorationCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardDecorationCloner;

IGameNode* Clone__StandardDecoration(IGameNode *game_node, bool log_info)
{
	return StandardDecorationCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

class CStandardDecorationLoader : public CGameNodeBaseLoader
{
public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);
		_loadGameNode(node_params);	
		_postLoadGameNode(node_params);		

		return GameNode;
	}
}
StandardDecorationLoader;

IGameNode* Load___StandardDecoration(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardDecorationLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
