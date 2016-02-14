//|-------------------------------------------------------------------------
//| File:        IGameNodePerson.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodePersonHPP
#define IGameNodePersonHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNodeAI.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Fromation Class
class __MY_GAME_LIB_API__ IGameNodePerson : public IGameNodeAI
{
public:

	//! constructor
	IGameNodePerson(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNodeAI(n, params) {}

	//! destructor
	virtual ~IGameNodePerson() {};

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodePersonHPP