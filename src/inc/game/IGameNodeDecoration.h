//|-------------------------------------------------------------------------
//| File:        IGameNodeDecoration.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeDecorationHPP
#define IGameNodeDecorationHPP
//----------------------------------------------------------------------------

#include <game/IGameNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Decoreation Class
class __MY_GAME_LIB_API__ IGameNodeDecoration : public IGameNode
{   
public:

    //! constructor
    IGameNodeDecoration(scn::ISceneNode* n, SGameNodeParams &params) 
        : IGameNode(n, params) {}

    //! destructor
    virtual ~IGameNodeDecoration() {}

	//! some methods will be added later
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeDecoration