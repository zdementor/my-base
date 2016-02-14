//|-------------------------------------------------------------------------
//| File:        IGameNodeDoor.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeDoorHPP
#define IGameNodeDoorHPP
//----------------------------------------------------------------------------

#include "IGameNodeActivatable.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! GameNode Trigger class
class IGameNodeDoor : public IGameNodeActivatable
{

public:

	//! constructor
	IGameNodeDoor(scn::ISceneNode* n, SGameNodeParams &params)
		: IGameNodeActivatable(n, params) {}

	//! destructor
	virtual ~IGameNodeDoor() {}

	//! return door current state
	virtual E_DOOR_STATE getState() = 0;

	//! setting angle, determines the opening direction of door (up = -1, down = -2). 
	virtual void setOpenAngle( s32 angle) = 0;

	//! returning angle, determines the opening direction of door
	virtual s32 getOpenAngle() = 0;

	//! setting speed, determines how fast the door moves (default 100). 
	virtual void setMoveSpeed(f32 speed) = 0;

	//! returning speed, determines how fast the door moves
	virtual f32 getMoveSpeed() = 0;

	//! setting damage to inflict on player when he blocks operation of door (default 4).
	//! Door will reverse direction when blocked unless CRUSHER flag is set.
	virtual void setDamage(f32 dmg) = 0;

	//! returning damage to inflict on player when he blocks operation of door
	virtual f32 getDamage() = 0;

	//! setting CRUSHER flag, if true it's mean that player die immediately when he blocks operation of door
	virtual void setCrusherFlag(bool crusher) = 0;

	//! returning CRUSHER flag
	virtual bool isCrusher() = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeDoorHPP