//|-------------------------------------------------------------------------
//| File:        CGameNodeButton.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeButtonHPP
#define CGameNodeButtonHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeButton.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodeButton : public IGameNodeButton
{

public:

	//! constructor
	CGameNodeButton(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeButton();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);	

	//! return Button current state
	virtual E_BUTTON_STATE getState();

	//! setting angle, determines the opening direction of Button (up = -1, down = -2). 
	virtual void setOpenAngle( s32 angle);

	//! returning angle, determines the opening direction of Button
	virtual s32 getOpenAngle();

	//! setting speed, determines how fast the Button moves (default 100). 
	virtual void setMoveSpeed(f32 speed);

	//! returning speed, determines how fast the Button moves
	virtual f32 getMoveSpeed();

	//! 
	virtual bool activate(IGameNode* activator);

protected:

private:

	bool ActivatedFlag;
	bool ActivatedFlagOnLastTime;

	E_BUTTON_STATE State;

	core::vector3df PushDir;

	f32 Angle;
	f32 Speed;

	core::vector3df UnpushedPos, PushedPos, CurrentPos, LastPos;

	s32 DynamicConstraints;

	s32 StartUnpushTime;	
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeButtonHPP