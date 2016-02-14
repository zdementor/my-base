//|-------------------------------------------------------------------------
//| File:        CGameNodePlatform.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameNodePlatform.h"

#include <io/ILogger.h>
#include <scn/ISceneManager.h>
#include <game/IGameTasksManager.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

s32 PLATFORM_ACTIVATED_SOUND   = 0;
s32 PLATFORM_DEACTIVATED_SOUND = 1;
s32 PLATFORM_MOVING_SOUND      = 2;

//----------------------------------------------------------------------------

//! constructor
CGameNodePlatform::CGameNodePlatform(
	scn::ISceneNode* n, SGameNodeParams &params
	) : 
IGameNodePlatform(n, SGameNodeParams(params, EGNT_PLATFORM)), 
m_PlatformState(EPS_WAITING_AT_START), m_Speed(100), m_FinishWayTime(0)
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodePlatform");	
#endif
	
	setMoveTrajectory(m_Trajectory);
}

//----------------------------------------------------------------------------

//! destructor
CGameNodePlatform::~CGameNodePlatform()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodePlatform::OnPreDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPreDynamic(delta_time_sec);	

	core::vector3df passenger_mov_vec(0,0,0);

	if (m_PlatformState == EPS_MOVING_FORWARD || m_PlatformState == EPS_MOVING_BACKWARD)
	{
		f32 dir = (m_PlatformState == EPS_MOVING_FORWARD) ? 1.0f : -1.0f;

		core::vector3df platf_mov_vec = m_CurrentMoveDir * m_Speed * m_DeltaTimeSec * dir;		

		// setting new platform position 		
		m_SceneNode->setPosition(m_NodePosition + platf_mov_vec);   
		
		passenger_mov_vec =	m_CurrentMoveDir * m_Speed *0.01f * dir;
	}	

	// checking object wich are on the platform	

	s32 obj_cnt = getInventory().getObjectsCount();
	
	for (s32 oi=0; oi<obj_cnt; oi++)
	{
		static core::aabbox3df bb;

		if (oi==0)
		{
			bb = m_NodeTransformedBBox;
			bb.MaxEdge.Y += 10;
		}

		IGameNode* gnode = getInventory().getObject(oi);

		if (gnode)
		{
			if (!bb.intersectsWithBox(gnode->getTransformedBBox()))
			// checking object wich are leaving platform
			{
				// make a passenger get out
				if (getInventory().removeObject(gnode))
				{
					obj_cnt = getInventory().getObjectsCount();

					oi--;

					CHECK_MIN_RANGE(oi, 0);
				}
			}
			else 
			{
				scn::ISceneNode *snode = gnode->getSceneNode();				

				if (snode)
				{
					// moving object with platform
					if (passenger_mov_vec!= core::vector3df(0,0,0))
					{
						if(passenger_mov_vec.Y>0)
						{
							passenger_mov_vec *= 0.1f;
						}

						snode->setPosition(	gnode->getDislocation() + passenger_mov_vec, true ); 
					}
				}
			}
		}
	}

	m_ActivatedFlagOnLastTime = false;
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodePlatform::OnPostDynamic(f64 delta_time_sec)
{
	IGameNodeActivatable::OnPostDynamic(delta_time_sec);	

	s32 now = m_NowTimeMs;

	core::vector3df platf_pos = m_SceneNode->getPosition();	
	core::vector3df new_platf_pos;

	bool platf_just_stopped = false;
	
	if (m_PlatformState == EPS_MOVING_FORWARD)
	{
		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			core::vector3df vec = m_EndPos-platf_pos;

			f32 angle = m_CurrentMoveDir.getAngleDeg(vec);	
			
			if (angle>85)
			{
				m_PlatformState = EPS_WAITING_AT_END;	
				
				new_platf_pos = m_EndPos;

				platf_just_stopped = true;
			}
		}
	}
	else if (m_PlatformState == EPS_MOVING_BACKWARD)
	{
		core::vector3df vec = platf_pos - m_LastNodePosition;
		core::vector3df vec1 = m_StartPos - platf_pos;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			core::vector3df vec = m_StartPos-platf_pos;

			f32 angle = m_CurrentMoveDir.getAngleDeg(vec);	
				
			if (angle<95)
			{
				m_PlatformState = EPS_WAITING_AT_START;					
				
				new_platf_pos = m_StartPos;

				platf_just_stopped = true;
			}
		}
	}

	if (platf_just_stopped)
	{	
		// setting new platform position 
		m_SceneNode->setPosition(new_platf_pos); 
				
		// play sounds
		stopSoundEffect(PLATFORM_ACTIVATED_SOUND);
		playSoundEffect(PLATFORM_DEACTIVATED_SOUND);
				
		// make all passengers get out
		getInventory().removeAllObjects();

		m_FinishWayTime = m_NowTimeMs;

		m_Activated = false;
	}
}

//----------------------------------------------------------------------------

//! return door current state
E_PLATFORM_STATE CGameNodePlatform::getState()
{
	return m_PlatformState;
}

//----------------------------------------------------------------------------

//! setting speed, determines how fast the door moves (default 100). 
void CGameNodePlatform::setMoveSpeed(f32 speed)
{
	m_Speed = speed;
}

//----------------------------------------------------------------------------

//! returning speed, determines how fast the door moves
f32 CGameNodePlatform::getMoveSpeed()
{
	return m_Speed;
}

//----------------------------------------------------------------------------

//! setting moving trajectory
void CGameNodePlatform::setMoveTrajectory(const core::array<core::vector3df> &traj)
{
	m_Trajectory = traj;

	if (m_Trajectory.size()<=1)
	{
		m_CurrentMoveDir.set(0,-1,0);

		core::aabbox3df bbox = m_SceneNode->getBoundingBox();
		f32 diap = bbox.getExtend().Y - m_Lip;
	
		m_StartPos = m_NodePosition;
		m_EndPos = m_StartPos + m_CurrentMoveDir*diap;
	}
	else	
	{

	}
}

//----------------------------------------------------------------------------

//! setting lip value
void CGameNodePlatform::setLip(f32 lip)
{
	IGameNodeActivatable::setLip(lip);

	setMoveTrajectory(m_Trajectory);
}

//----------------------------------------------------------------------------

//! 
bool CGameNodePlatform::activate(IGameNode* activator)
{
	bool activate_platform = false;	
	
	if (!activator || (
			m_TeamNodes.size()==0 && (
				m_NowTimeMs>(m_FinishWayTime + m_WaitTimeSec*1000)
				) && 
			getInventory().addObject(activator)
			)
		)
		activate_platform = true;

	if (m_PlatformState==EPS_WAITING_AT_START &&
		activate_platform
		)
	{
		m_Activated = true;

		m_PlatformState = EPS_MOVING_FORWARD;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			dyn::SDynamicObjectParams platf_dynparams = dynobj->getParameters();
										
			platf_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

			dynobj->setParameters(platf_dynparams);
		}

		stopSoundEffect(PLATFORM_DEACTIVATED_SOUND);
		playSoundEffect(PLATFORM_ACTIVATED_SOUND);

		return true;
	}
	else if (
		m_PlatformState==EPS_WAITING_AT_END &&
		activate_platform
		)
	{
		m_Activated = true;

		m_PlatformState = EPS_MOVING_BACKWARD;

		dyn::IDynamicObject *dynobj = getDynamicObject();

		if (dynobj)
		{
			dyn::SDynamicObjectParams platf_dynparams = dynobj->getParameters();
										
			platf_dynparams.DynamicConstraints = dyn::EDTC_NO_TRANSF;

			dynobj->setParameters(platf_dynparams);
		}

		stopSoundEffect(PLATFORM_DEACTIVATED_SOUND);
		playSoundEffect(PLATFORM_ACTIVATED_SOUND);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! Platform registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardPlatform(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodePlatform(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardPlatformCreator;

IGameNode* Create_StandardPlatform(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardPlatformCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardPlatformCloner;

IGameNode* Clone__StandardPlatform(IGameNode *game_node, bool log_info)
{
	return StandardPlatformCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseLoader StandardPlatformLoader;

IGameNode* Load___StandardPlatform(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardPlatformLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
