//|-------------------------------------------------------------------------
//| File:        IGameNodeButton.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeButtonHPP
#define IGameNodeButtonHPP
//----------------------------------------------------------------------------

#include "IGameNodeActivatable.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! GameNode Trigger class
class IGameNodeButton : public IGameNodeActivatable
{

public:

	//! constructor
	IGameNodeButton(scn::ISceneNode* n, SGameNodeParams &params)
		: IGameNodeActivatable(n, params) {}

	//! destructor
	virtual ~IGameNodeButton() {}

	//! return door current state
	virtual E_BUTTON_STATE getState() = 0;

	//! setting angle, determines the opening direction of door (up = -1, down = -2). 
	virtual void setOpenAngle( s32 angle) = 0;

	//! returning angle, determines the opening direction of door
	virtual s32 getOpenAngle() = 0;

	//! setting speed, determines how fast the door moves (default 100). 
	virtual void setMoveSpeed(f32 speed) = 0;

	//! returning speed, determines how fast the door moves
	virtual f32 getMoveSpeed() = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeButtonHPP