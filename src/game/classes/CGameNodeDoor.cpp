//|-------------------------------------------------------------------------
//| File:        CGameNodeDoor.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodeDoor.h"

#include <io/ILogger.h>
#include <scn/ISceneManager.h>
#include <game/IGameTasksManager.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

s32 DOOR_ACTIVATED_SOUND   = 0;
s32 DOOR_DEACTIVATED_SOUND = 1;

//----------------------------------------------------------------------------

//! constructor
CGameNodeDoor::CGameNodeDoor(
	scn::ISceneNode* n, SGameNodeParams &params
	) : 
IGameNodeDoor(n, SGameNodeParams(params, EGNT_DOOR)), 
State(EDS_CLOSED), m_LastState(EDS_CLOSED),
m_OpenDir(1,0,0), Angle(0), Speed(100), Damage(4), Crusher(false),
m_LastPos(0,0,0), m_CurrentPos(0,0,0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeDoor");		
#endif
	//m_SceneNode->setDebugDataVisible(true);
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeDoor::~CGameNodeDoor()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeDoor::OnPreDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPreDynamic(delta_time_sec);	

	m_LastPos = m_CurrentPos;
	m_CurrentPos = m_NodePosition;	

	if (State == EDS_OPENING || State == EDS_CLOSING)
	{
		f32 dir = (State == EDS_OPENING) ? 1.0f : -1.0f;

		core::vector3df node_new_pos = 
			m_CurrentPos + m_OpenDir * Speed * 0.06f * dir;
			//m_CurrentPos + m_OpenDir * Speed * DeltaTimeSec * dir;		

		// setting new position 
		m_SceneNode->setPosition(node_new_pos, true);   		
	}
	else if (m_LastState!=State)
	{		
		if (State == EDS_WAITING)
		{
			// setting new position 
			m_SceneNode->setPosition(m_OpenedPos); 
		}
		else if (State == EDS_CLOSED)
		{
			// setting new position 
			m_SceneNode->setPosition(m_ClosedPos); 

			m_Activated = false;
		}
	}

	m_LastState = State;
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeDoor::OnPostDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPostDynamic(delta_time_sec);			

	s32 now = m_NowTimeMs;

	m_CurrentPos = m_SceneNode->getPosition();	

	if (State == EDS_OPENING)
	{
		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			core::vector3df vec = m_OpenedPos-m_CurrentPos;

			f32 angle = m_OpenDir.getAngleDeg(vec);	
			
			if (angle>85)
			{
				State = EDS_WAITING;

				dyn::SDynamicObjectParams door_dynparams = dynobj->getParameters();

				door_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

				dynobj->setParameters(door_dynparams);

				StartCloseTime = now + m_WaitTimeSec * 1000;

				stopSoundEffect(DOOR_ACTIVATED_SOUND);
				playSoundEffect(DOOR_DEACTIVATED_SOUND);
			}
		}
	}
	else if (State == EDS_CLOSING)
	{		
		core::vector3df vec = m_CurrentPos - m_LastPos;
        core::vector3df vec1 = m_ClosedPos - m_CurrentPos;

        if (vec.getLength()<1 && m_MoveDiap_div2.getLength()>vec1.getLength())
        {
            State = EDS_OPENING;

            stopSoundEffect(DOOR_ACTIVATED_SOUND);
            playSoundEffect(DOOR_ACTIVATED_SOUND);
        }
        else        
        {
            dyn::IDynamicObject *dynobj = getDynamicObject();

            if (dynobj)
            {
                core::vector3df vec = m_ClosedPos-m_CurrentPos;

                f32 angle = m_OpenDir.getAngleDeg(vec);   
                
                if (angle<90)
                {
                    State = EDS_CLOSED;

                    dyn::SDynamicObjectParams door_dynparams = dynobj->getParameters();

                    door_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

                    dynobj->setParameters(door_dynparams);      
                    
                    m_SceneNode->setPosition(m_ClosedPos);                  

                    stopSoundEffect(DOOR_ACTIVATED_SOUND);
                    playSoundEffect(DOOR_DEACTIVATED_SOUND);
                }
            }
        }
	}
	else if (State == EDS_WAITING && m_ActivateMode==EOAM_AUTOMATIC)
	{
		if (now>StartCloseTime)
		{
			State = EDS_CLOSING;

			dyn::IDynamicObject *dynobj = getDynamicObject();

			if (dynobj)
			{
				dyn::SDynamicObjectParams door_dynparams = dynobj->getParameters();
						
				door_dynparams.DynamicConstraints = DynamicConstraints;

				dynobj->setParameters(door_dynparams);
			}

			stopSoundEffect(DOOR_DEACTIVATED_SOUND);
			playSoundEffect(DOOR_ACTIVATED_SOUND);

			for (u32 tn=0; tn<m_TeamNodes.size(); tn++)
			{
				if (m_TeamNodes[tn]->getGameNodeType() != EGNT_DOOR)
					m_TeamNodes[tn]->activate();
			}
		}
	} 
}

//----------------------------------------------------------------------------

//! return door current state
E_DOOR_STATE CGameNodeDoor::getState()
{
	return State;
}

//----------------------------------------------------------------------------

//! setting angle, determines the opening direction of door (up = -1, down = -2). 
void CGameNodeDoor::setOpenAngle( s32 angle)
{
	if (!m_SceneNode)
		return;

	Angle = angle;

	f32 diap = 1.0f;

	core::aabbox3df bbox = m_SceneNode->getBoundingBox();

	if (angle==0)
	{
		m_OpenDir = core::vector3df(1,0,0);

		diap = bbox.getExtend().X;
	}
	else if (angle==90)
	{
		m_OpenDir = core::vector3df(0,0,1);

		diap = bbox.getExtend().Z;
	}
	else if (angle==180)
	{
		m_OpenDir = core::vector3df(-1,0,0);

		diap = bbox.getExtend().X;
	}
	else if (angle==270)
	{
		m_OpenDir = core::vector3df(0,0,-1);

		diap = bbox.getExtend().Z;
	}
	else if (angle==-1)
	{
		m_OpenDir = core::vector3df(0,1,0);

		diap = bbox.getExtend().Y;
	}
	else
	{
		m_OpenDir = core::vector3df(0,-1,0);

		diap = bbox.getExtend().Y;
	}

	diap -= m_Lip;

	if (m_OpenDir.X!=0 && m_OpenDir.Y==0 && m_OpenDir.Z==0)
		DynamicConstraints = dyn::EDTC_MOVX;
	else
	if (m_OpenDir.X==0 && m_OpenDir.Y==0 && m_OpenDir.Z!=0)
		DynamicConstraints = dyn::EDTC_MOVZ; 
	else
		DynamicConstraints = dyn::EDTC_NO_TRANSF;

	m_ClosedPos = m_NodePosition;
	m_OpenedPos = m_ClosedPos + m_OpenDir*diap;

	m_MoveDiap_div2 = (m_OpenedPos - m_ClosedPos) / 2.0f;
}

//----------------------------------------------------------------------------

//! returning angle, determines the opening direction of door
s32 CGameNodeDoor::getOpenAngle()
{
	return Angle;
}

//----------------------------------------------------------------------------

//! setting speed, determines how fast the door moves (default 100). 
void CGameNodeDoor::setMoveSpeed(f32 speed)
{
	Speed = speed;
}

//----------------------------------------------------------------------------

//! returning speed, determines how fast the door moves
f32 CGameNodeDoor::getMoveSpeed()
{
	return Speed;
}

//----------------------------------------------------------------------------

//! setting damage to inflict on player when he blocks operation of door (default 4).
//! Door will reverse direction when blocked unless CRUSHER flag is set.
void CGameNodeDoor::setDamage(f32 dmg)
{
	Damage = dmg;
}

//----------------------------------------------------------------------------

//! returning damage to inflict on player when he blocks operation of door
f32 CGameNodeDoor::getDamage()
{
	return Damage;
}

//----------------------------------------------------------------------------

//! setting CRUSHER flag, if true it's mean that player die immediately when he blocks operation of door
void CGameNodeDoor::setCrusherFlag(bool crusher)
{
	Crusher = crusher;
}

//----------------------------------------------------------------------------

//! returning CRUSHER flag
bool CGameNodeDoor::isCrusher()
{
	return Crusher;
}

//----------------------------------------------------------------------------

//! setting lip value
void CGameNodeDoor::setLip(f32 lip)
{
	IGameNodeActivatable::setLip(lip);

	setOpenAngle(Angle);
}

//----------------------------------------------------------------------------

//! 
bool CGameNodeDoor::activate(IGameNode* activator)
{
	if (State == EDS_CLOSED)
	{
		m_Activated = true;

		State = EDS_OPENING;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			dyn::SDynamicObjectParams door_dynparams = dynobj->getParameters();
										
			door_dynparams.DynamicConstraints = DynamicConstraints;

			dynobj->setParameters(door_dynparams);
		}

		stopSoundEffect(DOOR_DEACTIVATED_SOUND);
		playSoundEffect(DOOR_ACTIVATED_SOUND);

		return true;
	}
	else if (State == EDS_WAITING && m_ActivateMode == EOAM_BY_BUTTON)
	{
		m_Activated = true;

		State = EDS_CLOSING;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			dyn::SDynamicObjectParams door_dynparams = dynobj->getParameters();
										
			door_dynparams.DynamicConstraints = DynamicConstraints;

			dynobj->setParameters(door_dynparams);
		}

		stopSoundEffect(DOOR_DEACTIVATED_SOUND);
		playSoundEffect(DOOR_ACTIVATED_SOUND);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! Door registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardDoor(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeDoor(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardDoorCreator;

IGameNode* Create_StandardDoor(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardDoorCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardDoorCloner;

IGameNode* Clone__StandardDoor(IGameNode *game_node, bool log_info)
{
	return StandardDoorCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardDoorLoader;

IGameNode* Load___StandardDoor(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardDoorLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
