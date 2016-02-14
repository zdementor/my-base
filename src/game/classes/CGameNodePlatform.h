//|-------------------------------------------------------------------------
//| File:        CGameNodePlatform.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodePlatformHPP
#define CGameNodePlatformHPP
//----------------------------------------------------------------------------

#include <game/IGameNodePlatform.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodePlatform : public IGameNodePlatform
{

public:

	//! constructor
	CGameNodePlatform(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodePlatform();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);	

	//! return Platform current state
	virtual E_PLATFORM_STATE getState();

	//! setting speed, determines how fast the Platform moves (default 100). 
	virtual void setMoveSpeed(f32 speed);

	//! returning speed, determines how fast the Platform moves
	virtual f32 getMoveSpeed();

	//! setting moving trajectory
	virtual void setMoveTrajectory(const core::array<core::vector3df> &traj);
	
	//! setting lip value
	virtual void setLip(f32 lip);

	//! 
	virtual bool activate(IGameNode* activator);

protected:

private:	

	E_PLATFORM_STATE m_PlatformState;

	core::vector3df m_CurrentMoveDir;

	f32 m_Speed;

	core::vector3df m_EndPos, m_StartPos;

	core::array<core::vector3df> m_Trajectory;

	s32 m_FinishWayTime;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodePlatformHPP