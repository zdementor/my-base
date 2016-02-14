//|-------------------------------------------------------------------------
//| File:        CGameNodeDoor.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeDoorHPP
#define CGameNodeDoorHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeDoor.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodeDoor : public IGameNodeDoor
{
public:

	CGameNodeDoor(scn::ISceneNode* n, SGameNodeParams &params);
	virtual ~CGameNodeDoor();

	virtual void OnPreDynamic(f64 delta_time_sec);
	virtual void OnPostDynamic(f64 delta_time_sec);	

	virtual E_DOOR_STATE getState();

	virtual void setOpenAngle( s32 angle);
	virtual s32 getOpenAngle();

	virtual void setMoveSpeed(f32 speed);
	virtual f32 getMoveSpeed();

	virtual void setDamage(f32 dmg);
	virtual f32 getDamage();

	virtual void setCrusherFlag(bool crusher);
	virtual bool isCrusher();

	virtual void setLip(f32 lip);

	virtual bool activate(IGameNode* activator);

protected:

private: 	

	E_DOOR_STATE State, m_LastState;

	core::vector3df m_OpenDir; 

	f32 Angle, Speed, Damage;	

	bool Crusher;

	core::vector3df m_MoveDiap_div2, m_ClosedPos, m_OpenedPos, m_CurrentPos, m_LastPos;

	s32 DynamicConstraints;

	s32 StartCloseTime;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeDoorHPP