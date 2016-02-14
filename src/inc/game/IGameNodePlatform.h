//|-------------------------------------------------------------------------
//| File:        IGameNodePlatform.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodePlatformHPP
#define IGameNodePlatformHPP
//----------------------------------------------------------------------------

#include "IGameNodeActivatable.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! GameNode Platform class
class IGameNodePlatform : public IGameNodeActivatable
{

public:

	//! constructor
	IGameNodePlatform(scn::ISceneNode* n, SGameNodeParams &params)
		: IGameNodeActivatable(n, params) {}

	//! destructor
	virtual ~IGameNodePlatform() {}

	//! return door current state
	virtual E_PLATFORM_STATE getState() = 0;

	//! setting speed, determines how fast the door moves (default 100). 
	virtual void setMoveSpeed(f32 speed) = 0;

	//! returning speed, determines how fast the door moves
	virtual f32 getMoveSpeed() = 0;

	//! setting moving trajectory
	virtual void setMoveTrajectory(const core::array<core::vector3df> &traj) = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodePlatformHPP