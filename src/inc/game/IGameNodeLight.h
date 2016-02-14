//|-------------------------------------------------------------------------
//| File:        IGameNodeLight.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeLightHPP
#define IGameNodeLightHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Fromation Class
class __MY_GAME_LIB_API__ IGameNodeLight : public IGameNode
{
public:

	//! constructor
	IGameNodeLight(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNode(n, params) 
	{
	}

	//! destructor
	virtual ~IGameNodeLight() {}

private:

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeLightHPP