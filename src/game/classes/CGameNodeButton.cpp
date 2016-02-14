//|-------------------------------------------------------------------------
//| File:        CGameNodeButton.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodeButton.h"

#include <io/ILogger.h>
#include <game/GameTasks.h>
#include <scn/ISceneManager.h>
#include <game/IGameTasksManager.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

s32 BUTTON_ACTIVATED_SOUND   = 0;
s32 BUTTON_DEACTIVATED_SOUND = 1;

//----------------------------------------------------------------------------

//! constructor
CGameNodeButton::CGameNodeButton(
	scn::ISceneNode* n, SGameNodeParams &params
	) : 
IGameNodeButton(n, SGameNodeParams(params, EGNT_BUTTON)), 
State(EBS_UNPUSHED), PushDir(1,0,0), Angle(0), 
Speed(50), LastPos(0,0,0), CurrentPos(0,0,0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeButton");		
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeButton::~CGameNodeButton()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeButton::OnPreDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPreDynamic(delta_time_sec);

	ActivatedFlagOnLastTime = ActivatedFlag;

	ActivatedFlag = false;

	LastPos = CurrentPos;

	CurrentPos = m_NodePosition;

	if (State == EBS_PUSHING || State == EBS_UNPUSHING)
	{
		f32 dir = (State == EBS_PUSHING) ? 1.0f : -1.0f;

		core::vector3df node_new_pos = 
			//CurrentPos + PushDir * Speed * 0.025f * dir;
			CurrentPos + PushDir * Speed * delta_time_sec * dir;

		// setting new position 
		m_SceneNode->setPosition(node_new_pos);   		
	}
	else if (State == EBS_WAITING)
	{
		// setting new position 
		m_SceneNode->setPosition(PushedPos); 
	}
	else if (State == EBS_UNPUSHED)
	{
		// setting new position 
		m_SceneNode->setPosition(UnpushedPos); 
	}
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeButton::OnPostDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPostDynamic(delta_time_sec);

	s32 now = m_NowTimeMs;

	CurrentPos = m_SceneNode->getPosition();	

	if (State == EBS_PUSHING)
	{
		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			core::vector3df vec = PushedPos-CurrentPos;

			f32 angle = PushDir.getAngleDeg(vec);	
			
			if (angle>85)
			{
				State = EBS_WAITING;

				dyn::SDynamicObjectParams Button_dynparams = dynobj->getParameters();

				Button_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

				dynobj->setParameters(Button_dynparams);

				StartUnpushTime = now + m_WaitTimeSec * 1000;

				//stopSoundEffect(BUTTON_ACTIVATED_SOUND);
				playSoundEffect(BUTTON_DEACTIVATED_SOUND);
			}
		}
	}
	else if (State == EBS_UNPUSHING)
	{
		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			core::vector3df vec = UnpushedPos-CurrentPos;
			f32 angle = PushDir.getAngleDeg(vec);	
				
			if (angle<90)
			{
				State = EBS_UNPUSHED;

				dyn::SDynamicObjectParams Button_dynparams = dynobj->getParameters();

				Button_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

				dynobj->setParameters(Button_dynparams);

				m_Activated = false;
			}
		}
	}
	else if (State == EBS_WAITING)
	{
		if (now>StartUnpushTime)
		{
			State = EBS_UNPUSHING;

			dyn::IDynamicObject *dynobj = getDynamicObject();

			if (dynobj)
			{
				dyn::SDynamicObjectParams Button_dynparams = dynobj->getParameters();
						
				Button_dynparams.DynamicConstraints = DynamicConstraints;

				dynobj->setParameters(Button_dynparams);
			}
		}
	} 
}

//----------------------------------------------------------------------------

//! return Button current state
E_BUTTON_STATE CGameNodeButton::getState()
{
	return State;
}

//----------------------------------------------------------------------------

//! setting angle, determines the opening direction of Button (up = -1, down = -2). 
void CGameNodeButton::setOpenAngle( s32 angle)
{
	if (!m_SceneNode)
		return;

	CHECK_RANGE(angle, 0, 360);

	Angle = angle;

	f32 diap = 1.0f;
	core::aabbox3df bbox = m_SceneNode->getBoundingBox();

	if (angle==0)
	{
		PushDir = core::vector3df(1,0,0);

		diap = bbox.getExtend().X;
	}
	else if (angle==90)
	{
		PushDir = core::vector3df(0,0,1);

		diap = bbox.getExtend().Z;
	}
	else if (angle==180)
	{
		PushDir = core::vector3df(-1,0,0);

		diap = bbox.getExtend().X;
	}
	else if (angle==270)
	{
		PushDir = core::vector3df(0,0,-1);

		diap = bbox.getExtend().Z;
	}
	else if (angle==-1)
	{
		PushDir = core::vector3df(0,1,0);

		diap = bbox.getExtend().Y;
	}
	else
	{
		PushDir = core::vector3df(0,-1,0);

		diap = bbox.getExtend().Y;
	}

	if (PushDir.X!=0 && PushDir.Y==0 && PushDir.Z==0)
		DynamicConstraints = dyn::EDTC_MOVX;
	else
	if (PushDir.X==0 && PushDir.Y==0 && PushDir.Z!=0)
		DynamicConstraints = dyn::EDTC_MOVZ; 
	else
		DynamicConstraints = dyn::EDTC_NO_TRANSF;

	UnpushedPos = m_NodePosition;
	PushedPos = UnpushedPos + PushDir*diap*0.8f;
}

//----------------------------------------------------------------------------

//! returning angle, determines the opening direction of Button
s32 CGameNodeButton::getOpenAngle()
{
	return Angle;
}

//----------------------------------------------------------------------------

//! setting speed, determines how fast the Button moves (default 100). 
void CGameNodeButton::setMoveSpeed(f32 speed)
{
	Speed = speed;
}

//----------------------------------------------------------------------------

//! returning speed, determines how fast the Button moves
f32 CGameNodeButton::getMoveSpeed()
{
	return Speed;
}

//----------------------------------------------------------------------------

//! 
bool CGameNodeButton::activate(IGameNode* activator)
{
	if (State == EBS_UNPUSHED)
	{
		m_Activated = true;						

		State = EBS_PUSHING;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			dyn::SDynamicObjectParams Button_dynparams = dynobj->getParameters();
										
			Button_dynparams.DynamicConstraints = DynamicConstraints;

			dynobj->setParameters(Button_dynparams);
		}

		IGameNode *activator = this;

		game::IGameTasksManager &gtmgr = GAME_TASK_MANAGER;

		for (u32 i=0; i<m_TargetNodes.size(); i++)
		{
			// registering task (node activation)

			gtmgr.registerGameTask( new game::SGameTaskActivate(activator, m_TargetNodes[i]) ); 
		}

		//stopSoundEffect(BUTTON_DEACTIVATED_SOUND);
		playSoundEffect(BUTTON_ACTIVATED_SOUND);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! Button registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardButton(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeButton(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardButtonCreator;

IGameNode* Create_StandardButton(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardButtonCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardButtonCloner;

IGameNode* Clone__StandardButton(IGameNode *game_node, bool log_info)
{
	return StandardButtonCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardButtonLoader;

IGameNode* Load___StandardButton(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardButtonLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
