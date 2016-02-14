//|-------------------------------------------------------------------------
//| File:        IGameNodeLevelMap.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeLevelMapHPP
#define IGameNodeLevelMapHPP
//----------------------------------------------------------------------------

#include <game/IGameNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Decoreation Class
class __MY_GAME_LIB_API__ IGameNodeLevelMap : public IGameNode
{   
public:

    //! constructor
    IGameNodeLevelMap(scn::ISceneNode* n, SGameNodeParams &params) 
        : IGameNode(n, params) {}

    //! destructor
    virtual ~IGameNodeLevelMap() {}

	//!
	virtual void initializeLevelParts() = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeLevelMap