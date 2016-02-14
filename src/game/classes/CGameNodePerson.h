//|-------------------------------------------------------------------------
//| File:        CGameNodePerson.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodePersonHPP
#define CGameNodePersonHPP
//----------------------------------------------------------------------------

#include <game/IGameNodePerson.h>
#include <game/IGameUnitsFormation.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodePerson : public IGameNodePerson
{
	friend class CGameManager;

public:

	//! constructor
	CGameNodePerson(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodePerson();

	virtual void moveBy(core::vector3df dir,   f32 speed = 1.0f);

	virtual void orientTo(core::vector3df dir, f32 speed = 1.0f);

	virtual void orientBy(core::vector3df angles, f32 speed = 1.0f) {}

protected:


private:


};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodePersonHPP