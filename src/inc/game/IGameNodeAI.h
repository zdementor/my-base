//|-------------------------------------------------------------------------
//| File:        IGameNodeAI.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeAIHPP
#define IGameNodeAIHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNode.h>	  
#include <game/IGameManager.h>
#include <scn/ISceneNode.h>
#include <dyn/IDynamicManager.h>
#include <scn/IPathFinderManager.h>
#include <game/IPathFindTask.h>
#include <game/IGameNodeWeapon.h>
#include <scn/ILightSceneNode.h>
#include <dev/IDevice.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SHurtEffectParameters
{
private:

	scn::ISceneNode* Node;

public:

	SHurtEffectParameters() : 
		Node(0), DelayAfterHurt(0), LifeTime(1000), Offset(core::vector3df(0,0,0)),
		Hit(true), Death(false) {}

	SHurtEffectParameters(scn::ISceneNode* node) : Node(0) 
    {
		if (node)
		{
			Node = node;
			SAFE_GRAB(Node);
		}
	}

    SHurtEffectParameters(const SHurtEffectParameters& p) : Node(0)  
    {
		if (p.Node)
		{
			Node = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);
	}

	virtual ~SHurtEffectParameters()
	{
		SAFE_DROP(Node);
	}

	void set(const SHurtEffectParameters& p)
	{
		DelayAfterHurt = p.DelayAfterHurt;
		LifeTime       = p.LifeTime;
		Offset         = p.Offset;
		Hit   = p.Hit;
		Death = p.Death;
	}

    //! operator =
    inline SHurtEffectParameters& operator=(const SHurtEffectParameters& p)
    {
		if (p.Node)
		{
			SAFE_DROP(Node);
			Node  = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);

		return *this;
    } 

	scn::ISceneNode* getSceneNode()
	{
		return Node;
	}
			
	s32 DelayAfterHurt;
	s32 LifeTime;
	bool Hit, Death;
	core::vector3df Offset;
};

//----------------------------------------------------------------------------	

//! game node params
struct SGameNodeAIParams
{    
	s32 MeleAttackRange, MeleAttackDamage, MeleAttackAngle;
	s32 FireAttackHorizMaxAngle, FireAttackVertMaxAngle; 
	s32 Armor, Power, Dexterity, Accuracy;
	s32 StandReadyMaxTimeMs;
	s32 ViewDist;
	s32 MemoryTime;
	f32 FriendlyDamageCoeff;
	img::SColor BloodColor;

    SGameNodeAIParams() 
        : 
	MeleAttackRange(90), MeleAttackDamage(20), MeleAttackAngle(90),
	Armor(100), Power(100), Dexterity(100), Accuracy(100),
	StandReadyMaxTimeMs(10000), FireAttackHorizMaxAngle(90), FireAttackVertMaxAngle(25),
	ViewDist(0), MemoryTime(5000), FriendlyDamageCoeff(0.1f), BloodColor(255,200,0,0)
    {}  

    inline bool operator!=(const SGameNodeAIParams& p)
    {
        return 
			MeleAttackRange  != p.MeleAttackRange ||
			MeleAttackDamage != p.MeleAttackDamage ||
			MeleAttackAngle  != p.MeleAttackAngle ||
			FireAttackHorizMaxAngle != p.FireAttackHorizMaxAngle ||
			FireAttackVertMaxAngle  != p.FireAttackVertMaxAngle ||
            Armor != p.Armor ||
            Power != p.Power ||
			Dexterity != p.Dexterity ||
            Accuracy != p.Accuracy ||
			StandReadyMaxTimeMs != p.StandReadyMaxTimeMs ||
			ViewDist != p.ViewDist ||
			MemoryTime != p.MemoryTime ||
			FriendlyDamageCoeff != p.FriendlyDamageCoeff ||
			BloodColor != p.BloodColor;
    }
};

//----------------------------------------------------------------------------

//! Game AI Node Class used as base class for all game nodes
class __MY_GAME_LIB_API__ IGameNodeAI : public IGameNode
{
	friend class CGameUnitsFormation;
	friend bool game::addUnitToFormation(
		game::IGameNodeAI* unit, game::IGameUnitsFormation* formation
		);
	friend void game::removeUnitFromFormation(game::IGameNodeAI* unit);

public:

    //----------------------------------------------------------------------------

	//! constructor
	IGameNodeAI(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNode(n, params),
        m_Formation(0), m_AIState(EAIS_IDLE), 
		m_PathFindTask(0), m_Firing(false), m_PrepareForFiring(false), m_StartFiringTime(0), 
		m_SelectedWeaponNumber(-1), m_FireCount(0), m_NodeLookAt(0,0,0), 
		m_NodeLookDir(1,0,0), m_NodeRightDir(0,0,-1), m_NodeTopDir(0,1,0), 
		m_EndTimeEnemyLookUp(0), m_StartWaitATurnTimeMs(-1.0),
		m_FireLight(0), m_FireMaxRadius(0.0f), m_FirePerSecond(25), 
		m_FireFlashTimeMs(0), m_FireFlashIntensity(0.0f), m_WeaponFired(false)
	{
		m_StandingTimeMs = core::math::RangeRandom(0,5000);

		m_PathFindTask = GAME_MANAGER.createPathFindTask();

		m_FireLight = SCENE_MANAGER.addLightSceneNode(m_SceneNode);

		vid::SLight light = m_FireLight->getLightParameters();

		light.Identificator = this;
		light.Radius = 1500;
		light.Enabled = true;
		light.Dynamic = true;
		light.CastShadows = DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_MEDIUM_QUALITY) ||
			DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_HIGH_QUALITY);

		m_FireLight->setLightParameters(light);

		m_FireLight->setVisible(false);		
	}

	//----------------------------------------------------------------------------

	//! destructor
	virtual ~IGameNodeAI() 
	{
		removeFromFormation();
		for (u32 i=0; i<m_HurtEffects.size(); i++)
			SCENE_MANAGER.addToDeletionQueue(m_HurtEffects[i].getSceneNode());
		m_HurtEffects.clear();
		SAFE_DROP(m_PathFindTask);
	}

	//----------------------------------------------------------------------------

	virtual bool addToFormation(game::IGameUnitsFormation* formation)
	{
		return game::addUnitToFormation(this, formation);
	}

	//----------------------------------------------------------------------------
	
	virtual void removeFromFormation()
	{
		game::removeUnitFromFormation(this);
	}

	//----------------------------------------------------------------------------

	virtual game::IGameUnitsFormation* getFormation()
	{
		return m_Formation; 
	}

	//----------------------------------------------------------------------------

	virtual bool inFormation()
	{
		return m_Formation ? true : false;
	}	

	//----------------------------------------------------------------------------

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPreDynamic(f64 delta_time_sec)
	{
		_updateParams(delta_time_sec);  		

		core::list< core::list< SGameTask* >::iterator >::iterator it = 
			m_InputGameTasksIterators.begin();

		scn::ISceneManager &smgr = SCENE_MANAGER;

		s32 cnt1 = 0;

		E_GAME_TASK_TYPE type = (E_GAME_TASK_TYPE)-1;

		for (; it!=m_InputGameTasksIterators.end(); ++it)
		{
			SGameTask* gtask = **it;
			do
			{
				if (gtask->Completed)
					break;

				if (gtask->getType() == EGTT_DIE)
				{
					setState(EAIS_DYING);
					die();				
					gtask->Completed = true; 

					m_PathFindTask->setPerformed();

					m_IsLive = false;

					const core::aabbox3df& tbb = m_NodeTransformedBBox;
					const core::aabbox3df& bb  = m_NodeBoundingBox;

					core::vector3df dirt_pos = m_TransformedBBoxCenter;
					dirt_pos.Y = tbb.MinEdge.Y;

					core::vector3df dirt_ext(m_NodeSphereRadius*2, m_NodeSphereRadius, m_NodeSphereRadius*2);  
	                                                
					smgr.makeDirtSpot(dirt_pos, dirt_ext, m_ParametersAI.BloodColor); 

					for (u32 i=0; i<m_HurtEffects.size(); i++)
					{
						SHurtEffectParameters &hurt = m_HurtEffects[i];

						if (hurt.Death)
						{
							scn::ISceneNode* sn = hurt.getSceneNode();

							sn->setPosition(m_TransformedBBoxCenter+hurt.Offset);
							sn->updateAbsoluteTransformation();

							smgr.addToVisibilityChangeQueue(sn, true,  hurt.DelayAfterHurt                );
							smgr.addToVisibilityChangeQueue(sn, false, hurt.DelayAfterHurt + hurt.LifeTime);
						}
					}
				}
				else 
				if (gtask->getType() == EGTT_FIND_PATH_AND_MOVE)
				{			
					if ( inFormation() )
					{
						gtask->Completed = true;
						m_PathFindTask->setPerformed();
						break;
					}

					gtask->Handled = true;

					SGameTaskFindPathMove* movetask = 
						static_cast<SGameTaskFindPathMove*>(gtask);

					if (movetask->Sender)
					{
						movetask->DestPos = movetask->Sender->getDislocation(); 
					}
						
					if (getState()!=EAIS_IDLE && 
						getState()!=EAIS_SEEKING &&
						getState()!=EAIS_WAIT_A_TURN)
					{		
						movetask->NeedToRepath = true;
						m_PathFindTask->setPerformed();
						break;
					}
							
					f32 height = m_NodeTransformedBBox.getExtend().Y;

					core::vector3df dist_to_dest = movetask->DestPos - m_NodeAbsPosition;
					dist_to_dest.Y=0;

					if ( dist_to_dest.getLength() <= 0.1f*height)
					{						
						gtask->Completed = true;
						m_PathFindTask->setPerformed();
						break;
					}

					scn::IPathFinderManager& pfmgr = PATH_FINDER_MANAGER;

					pfmgr.unmapUnitPosition(m_NodeAbsPosition);

					const core::array<core::vector3df> &path = 
						findPathTo(movetask->DestPos, movetask->NeedToRepath);

					if (path.size()>=2)
					{
						core::vector3df move_vec;
						move_vec = path[1]-path[0];	
						move_vec.Y = 0;

						orientInHorizDir(move_vec);

						if (!pfmgr.isBusy(m_NodeAbsPosition + move_vec))
						{
							if (m_NowTimeMs>(m_StartWaitATurnTimeMs+500) &&
								m_NodeAbsPosition.Y < (path[0].Y + 0.5f*height))
							{							
								pushInDir(move_vec);
								setState(EAIS_SEEKING);
							}
						}
						else
						{
							setState(EAIS_WAIT_A_TURN);
							movetask->NeedToRepath = true;
							m_PathFindTask->setPerformed();
							m_StartWaitATurnTimeMs = m_NowTimeMs;							
						}						
					}
					pfmgr.mapUnitPosition(m_NodeAbsPosition);
				}
				else if (gtask->getType() == EGTT_ORIENT)
				{
					if ( inFormation() )
					{
						gtask->Completed = true;
						break;
					}	

					if (getState()!=EAIS_IDLE && 
						getState()!=EAIS_SEEKING &&
						getState()!=EAIS_WAIT_A_TURN
						)
					{
						gtask->Completed = true;						
						break;
					}					

					SGameTaskOrient* orienttask = 
						static_cast<SGameTaskOrient*>(gtask);

					core::vector3df new_dir = orienttask->OrientDir;
					core::vector3df cur_dir = getLookDir();

					new_dir.Y = 0;

					if (cur_dir.getAngleDeg(new_dir)<10)
					{
						gtask->Completed = true;

						setState(EAIS_IDLE);
						break;
					}
						
					gtask->Handled = true;

					orientInHorizDir(new_dir);

					setState(EAIS_SEEKING);
				}
				else
				if (gtask->getType() == EGTT_ATTACK)
				{
					SGameTaskAttack* attackaction = 
						static_cast<SGameTaskAttack*>(gtask);

					if (getState()==EAIS_FINISHING_ATTACK)
					{
						gtask->Handled = true; 

						if (finishAttack(attackaction->Victim))
						{
							setState(EAIS_FINISHED_ATTACK);
						}
					}
					else
					if (getState()==EAIS_FINISHED_ATTACK)
					{
						gtask->Completed = true; 

						setState(EAIS_IDLE);
					}
					else						
					{	
						bool ready = 
							isReadyToAttack(attackaction->Victim, attackaction->CanMeleAttack);

						if (getState()==EAIS_ATTACKING)
						{
							orientInHorizDir(attackaction->VectorAttack);
							attack(attackaction->CanMeleAttack, attackaction->Victim);								
										
							if (!ready)
							{
								setState(EAIS_FINISHING_ATTACK);
							}
						}
						else if (ready)
						{
							orientInHorizDir(attackaction->VectorAttack);
							attack(attackaction->CanMeleAttack, attackaction->Victim);	
							
							setState(EAIS_ATTACKING);						
						}
						else
						{
							gtask->Completed = true;
						}

						gtask->Handled = true;
					}
				}
				else
				if (gtask->getType() == EGTT_HITED)
				{
					SGameTaskHited* hitaction = 
						static_cast<SGameTaskHited*>(gtask);

					addLife(-hitaction->HitPower);

					IGameNodeAI *hitman = SAFE_CAST_TO_GAME_NODE_AI(gtask->Sender);

					if (hitman && isWarWith(hitman))
					{
						const core::aabbox3df& tbb = m_NodeTransformedBBox;

						core::vector3df dirt_pos = m_TransformedBBoxCenter;
						dirt_pos.Y = tbb.MinEdge.Y;

						core::vector3df dirt_ext(m_NodeSphereRadius, m_NodeSphereRadius, m_NodeSphereRadius);  

						smgr.makeDirtSpot(dirt_pos, dirt_ext, m_ParametersAI.BloodColor);
					}

					for (u32 i=0; i<m_HurtEffects.size(); i++)
					{
						SHurtEffectParameters &hurt = m_HurtEffects[i];

						if (hurt.Hit)
						{
							scn::ISceneNode* sn = hurt.getSceneNode();

							sn->setPosition(m_TransformedBBoxCenter+hurt.Offset);
							sn->updateAbsoluteTransformation();

							smgr.addToVisibilityChangeQueue(sn, true,  hurt.DelayAfterHurt                );
							smgr.addToVisibilityChangeQueue(sn, false, hurt.DelayAfterHurt + hurt.LifeTime);
						}
					}

					gtask->Completed = true; 
				}
				else
				{
					gtask->Completed = true;
				}
			}
			while (0);
		}		

		//setting up fire light

		if (m_FireFlashIntensity==1)
		{
			m_FireLight->setVisible(true);
		}

		if (m_FireLight->isVisible())
		{	
			m_FireFlashIntensity -= ((m_DeltaTimeSec*1000.0f)/m_FireFlashTimeMs);
			CHECK_RANGE(m_FireFlashIntensity, 0.0f, 1.0f);

			if (m_FireFlashIntensity==0)
			{
				m_FireLight->setVisible(false);
			}
			else
			{
				vid::SLight light = m_FireLight->getLightParameters();
				light.Radius = m_FireMaxRadius*m_FireFlashIntensity;
				m_FireLight->setLightParameters(light);
			}
		}
	}

	//----------------------------------------------------------------------------

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPostDynamic(f64 delta_time_sec)
	{
		if (m_EndTimeEnemyLookUp &&	m_NowTimeMs>m_EndTimeEnemyLookUp)
		{
			m_EndTimeEnemyLookUp = 0;

			setState(EAIS_IDLE);

			core::list< core::list< SGameTask* >::iterator >::iterator it = 
				m_InputGameTasksIterators.begin();

			for (; it!=m_InputGameTasksIterators.end(); ++it)
			{
				SGameTask* gtask = **it;

				gtask->Completed = true;

				if (gtask->getType() == EGTT_FIND_PATH_AND_MOVE)
				{
					m_PathFindTask->setPerformed();
				}
			}
		}  
	}

	//----------------------------------------------------------------------------

    virtual bool isCanSee(IGameNode* gnode)
	{
		if (m_NowTimeMs==0.0 || !gnode)
			return false;

		bool res = false;

		core::aabbox3df bb_own   = getTransformedBBox();
		core::aabbox3df	bb_other = gnode->getTransformedBBox();

		res = m_ViewArea.classifyBoxRelation(bb_other)!=0;

		if (res)
		{
			core::line3d<f32> ray(bb_own.getCenter(), bb_other.getCenter());

			if (DYNAMIC_MANAGER.checkIntersectionWithRay(
					ray, dyn::EDCF_MONOLITH_COLLID
					)
				)
			{
				res = false;
			}		
		}

		return res;
	}

	//----------------------------------------------------------------------------

	//! increase/decrease life
	virtual void addLife(f32 add_val)
	{
		IGameNode::addLife(add_val);
	}

	//----------------------------------------------------------------------------

	virtual bool isReadyToAttack(IGameNode *other, bool& can_mele_attack)
	{
		if (!other || !other->isLive())
		{
			return false;		
		}

		if ((m_EndTimeEnemyLookUp-m_NowTimeMs)<0.9f*m_ParametersAI.MemoryTime)
			return false;

		core::vector3df dir       = other->getDislocation()-getDislocation();		
		core::vector3df look_dir  = getLookDir();

		core::vector3df dir_horiz(dir.X, 0, dir.Z);
		core::vector3df look_dir_horiz(look_dir.X, 0, look_dir.Z);
			
		f32 dist = dir.getLength();
		
		f32 angle_horiz = dir_horiz.getAngleDeg(look_dir_horiz);
		f32 angle_vert  = dir_horiz.getAngleDeg(dir);

		if (angle_horiz<m_ParametersAI.MeleAttackAngle)
		{	
			if (dist<0.8f*m_ParametersAI.MeleAttackRange)
			{
				can_mele_attack = true;
			}
			else if (dist>m_ParametersAI.MeleAttackRange)
			{
				can_mele_attack = false;
			}
		}

		f32 fire_range = 0.0f;

		if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
		{
			s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

			for (s32 i=0; i<1 && i<weapon_subcnt; i++)
			{
				game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

				fire_range = weapon->getFireRange();
			}
		}

		bool can_fire_attack = false;

		if (dist<fire_range && 
			angle_horiz<m_ParametersAI.FireAttackHorizMaxAngle &&
			angle_vert<m_ParametersAI.FireAttackVertMaxAngle
			)
		{
			can_fire_attack = true;
		}

		return can_mele_attack || can_fire_attack;
	}

	//! return true if this ai node dead
	virtual bool isDead()
	{
		return m_AIState==EAIS_DEAD;
	}

	//----------------------------------------------------------------------------

	virtual void setState(E_AI_STATE state)
	{
		if (m_AIState!=EAIS_DEAD)
		{
			m_AIState = state;
		}
	}

	//----------------------------------------------------------------------------

	virtual E_AI_STATE getState()
	{		
		return m_AIState;
	}

	//----------------------------------------------------------------------------

	virtual const SGameNodeAIParams& getParametersAI()
	{
		return m_ParametersAI;
	}

	//----------------------------------------------------------------------------

	virtual void setParametersAI(SGameNodeAIParams &params_ai)
	{
		m_ParametersAI = params_ai;

		m_ParametersAI.MemoryTime = core::math::RangeRandom(
			params_ai.MemoryTime*0.75f, params_ai.MemoryTime*1.25f
			);

		f32 fovy = core::DEG2RAD * 120; // Field of view, in radians. 
		f32 aspect = 0.2f;   // Aspect ratio. 
			
		m_ViewArea.setProjectionPerspective(fovy, aspect, 1.0f, m_ParametersAI.ViewDist);
	}

	//---------------------------------------------------------------------------

	virtual const core::array< core::vector3df >& findPathTo( core::vector3df dest_pos, bool force_recalc=false )
	{	
		core::vector3df node_pos = getDislocation();

		if ( m_PathFinderPosOffset == core::vector3df(0,0,0) )
		{
			m_PathFinderPosOffset = 
				PATH_FINDER_MANAGER.getPositionOffsetFor(node_pos);
			m_PathFinderPosOffset.Y = 0;
		}

		core::vector3df pos_offset(0,0,0);
				
		core::aabbox3df bbox = getBoundingBox();

		if (!m_SceneNode->getDynamicObject())
		{
			if (!inFormation())
				pos_offset += m_PathFinderPosOffset;

			pos_offset -= core::vector3df(0, bbox.MinEdge.Y, 0);	
		}

		core::vector3df start_pos = node_pos - pos_offset;		

		m_PathFindTask->initTaskData(	start_pos, dest_pos	);
		m_PathFindTask->performTask(force_recalc);

		return m_PathFindTask->getFindedPath();
	}

	//---------------------------------------------------------------------------

	virtual const core::array< core::vector3df >& getFindedPath()
	{
		return m_PathFindTask->getFindedPath();
	}

	//---------------------------------------------------------------------------

	//! binding new weapon to the bone and index
    virtual bool bindWeapon(s32 number,
		IGameNodeWeapon* weapon, const c8 *target_bone)
	{
		if (!weapon)
			return false;

		LOGGER.logInfo("Binding weapon %d to the '%s' bone . . .",
			number, target_bone);

		scn::ISceneNode *snode = weapon->getSceneNode();

		scn::ISceneNode *wnode = snode;
		scn::IAnimatedMeshSceneNode *anode =
			SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(m_SceneNode);

		if (wnode && anode)
		{
			bool res = anode->attachToBone(wnode, target_bone);
			if (res && m_Inventory->addWeapon(weapon, number))
			{
				snode->setVisible(false);
				weapon->setBindedBoneName(target_bone);
				weapon->setOwner(this);
				LOGGER.logInfo(" Weapon binded OK");
				return true;
			}
			else
				LOGGER.logErr("Can't bind weapon!");
		}
		else
			LOGGER.logErr("Can't bind weapon to this scene node!");

		return false;
	}

	//---------------------------------------------------------------------------

	//! selecting to use weapon, by its number
    virtual bool selectWeapon(s32 number, bool select=true)
	{
		if (number<0 || number>=m_Inventory->getWeaponsCount())
		{
			return false;	
		}

		if (select && m_SelectedWeaponNumber==number)
		{
			return true;
		}

		fireWeapon(false);	

		bool res = false;

		if (number>=0 && number<m_Inventory->getWeaponsCount())
		{
			s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(number);

			for (s32 i=0; i<weapon_subcnt; i++)
			{
				game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(number, i);

				if ( weapon && weapon->isWeaponEnabled())
				{	
					if (select)
					{
						m_WeaponParameters = weapon->getWeaponParameters();
						
						weapon->select();
					}
					else
					{
						weapon->deSelect();
					}

					res = true;
				}
			}		
		}	

		if (res)		
		{
			if (select)
			{
				// deselecting old weapon
				selectWeapon(m_SelectedWeaponNumber, false);			
			}

			m_SelectedWeaponNumber = number;
		}

		if (m_FireLight->isVisible())
		{
			m_FireLight->setVisible(false);
		}

		return res;
	} 	

	//---------------------------------------------------------------------------

	//! selecting to use previous weapon
    virtual bool selectPrevWeapon()
	{
		s32 sel_num = m_SelectedWeaponNumber-1;

		while (sel_num>=0 && sel_num<m_Inventory->getWeaponsCount())
		{			
			if (selectWeapon(sel_num, true))
				return true;	
			sel_num--;
		}
		
		return false;
	}

	//---------------------------------------------------------------------------

	//! selecting to use next weapon
    virtual bool selectNextWeapon()
	{
		s32 sel_num = m_SelectedWeaponNumber+1;

		while (sel_num>=0 && sel_num<m_Inventory->getWeaponsCount())
		{			
			if (selectWeapon(sel_num, true))
				return true;	
			sel_num++;
		}
		
		return false;
	}

	//----------------------------------------------------------------------------

	//! getting selected weapon number
	virtual s32 getSelectedWeaponNumber()
	{
		return m_SelectedWeaponNumber;
	}

	//---------------------------------------------------------------------------

	//! fire weapon
	virtual bool aimWeaponByRay(const core::line3df& ray)
	{
		if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
		{
			s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

			for (s32 i=0; i<weapon_subcnt; i++)
			{
				game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

				if ( weapon )
				{
					weapon->aimByRay(ray);
				}
			}

			return true;
		}

		return false;
	}

	//---------------------------------------------------------------------------

	//! fire weapon
	virtual bool aimWeaponAtPos(const core::vector3df& pos)
	{
		if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
		{
			s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

			for (s32 i=0; i<weapon_subcnt; i++)
			{
				game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

				if ( weapon )
				{
					weapon->aimAtPos(pos);
				}
			}

			return true;
		}

		return false;
	}

	//---------------------------------------------------------------------------

	//! getting aiming position
	virtual core::vector3df getAimedPos()
	{
		if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
		{
			s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

			for (s32 i=0; i<weapon_subcnt; i++)
			{
				game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

				if ( weapon )
				{
					return weapon->getAimedPos();
				}
			}
		}

		return core::vector3df(0,0,0);
	}

	//---------------------------------------------------------------------------

	//! fire weapon
	virtual bool fireWeapon(bool fire=true, bool with_delay=true)
	{
		bool old_firing_state = m_Firing;
		bool new_firing_state = m_Firing;

		s32 now = m_NowTimeMs;

		if (fire && !m_PrepareForFiring && !m_Firing)
		{
			if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
			{
				s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber); 
				for (s32 i=0; i<weapon_subcnt; i++)
				{
					game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);
					if (weapon->getBulletAmmoCount(weapon->getChoosedBulletIndex())>0)
						m_PrepareForFiring=true;
				}
			}

			if (m_PrepareForFiring)
				m_StartFiringTime = now + m_WeaponParameters.WaitBeforeFire;

		}
		else if (!fire)
		{
			m_PrepareForFiring = false;
			m_WeaponFired = false;
		}

		if (m_PrepareForFiring && m_StartFiringTime<=now)
		{
			new_firing_state = true;
			m_PrepareForFiring= false;
		}
		else if (!fire)
		{
			new_firing_state = false;
		}

		if (m_Firing!=new_firing_state)
		{
			m_Firing=false;

			if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
			{
				s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

				for (s32 i=0; i<weapon_subcnt; i++)
				{
					game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

					if ( weapon )
					{
						if (new_firing_state)
						{
							bool res = weapon->startFire(with_delay);

							if (!m_Firing)
								m_Firing = res;
						}
						else
						{
							weapon->stopFire();
						}
					}
				}		
			} 
		}
		
		bool fired = false;

		if (m_Firing)
		{
			m_Firing=false;
			if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
			{
				s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

				for (s32 i=0; i<weapon_subcnt; i++)
				{
					game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

					if (weapon->fire())
					{
						m_FireCount++;
						fired = true;						
					}
					if (weapon->isFiring())
						m_Firing=true;
				}
			}
		}			

		if (m_Firing!=old_firing_state)
		{
			f32 light_r = 1.0f, light_g = 1.0f, light_b = 1.0f;
			f32 light_radius = 1000.0f;
			f32 light_flick_per_sec = 25;
			u32 light_time_ms = 0;

			if (m_SelectedWeaponNumber>=0 && m_SelectedWeaponNumber<m_Inventory->getWeaponsCount())
			{
				if (m_Firing)
				{
					s32 weapon_subcnt = m_Inventory->getWeaponsSubCount(m_SelectedWeaponNumber);

					core::vector3df light_pos(0,0,0);				

					for (s32 i=0; i<weapon_subcnt; i++)
					{
						game::IGameNodeWeapon *weapon = m_Inventory->getWeapon(m_SelectedWeaponNumber, i);

						if ( weapon )
						{
							img::SColorf fire_color = weapon->getFlashColor();
							s32 fire_per_sec = weapon->getWeaponParameters().FirePerSecond;

							f32 fire_r = fire_color.r;
							f32 fire_g = fire_color.g;
							f32 fire_b = fire_color.b;

							f32 fire_radius = weapon->getFlashRadius();	

							// caculating light params
							if (i==0)
							{
								light_time_ms = weapon->getFlashTimeMs();
								light_pos = weapon->getFlashLightPosition();
								light_r = fire_r;
								light_g = fire_g;
								light_b = fire_b;
								light_radius = fire_radius;
								light_flick_per_sec = fire_per_sec;
							}
							else
							{
								light_time_ms += weapon->getFlashTimeMs();
								light_pos += weapon->getFlashLightPosition();
								light_r += fire_r;
								light_g += fire_g;
								light_b += fire_b;									
								light_radius += fire_radius;
								light_flick_per_sec += fire_per_sec;
							}							
						}
					}
				
					if (weapon_subcnt>0)
					{
						// caculating average light params

						light_time_ms /= f32(weapon_subcnt);
						light_pos /= f32(weapon_subcnt);
						light_r /= f32(weapon_subcnt);
						light_g /= f32(weapon_subcnt);
						light_b /= f32(weapon_subcnt);
						light_radius /= f32(weapon_subcnt);				
					}

					m_FireFlashTimeMs = light_time_ms;
					m_FireMaxRadius = light_radius;
					m_FirePerSecond = light_flick_per_sec;

					vid::SLight light = m_FireLight->getLightParameters();
					light.DiffuseColor.set(light_r, light_g, light_b);
					light.SpecularColor.set(light_r, light_g, light_b);
					light.AmbientColor.set(0,0,0);
					light.Radius=m_FireMaxRadius;

					m_FireLight->setLightParameters(light);

					m_FireLight->setPosition(light_pos);
				}				
			}
		}
				
		if (fired)
		{
			m_FireFlashIntensity = 1.0f;

			m_WeaponFired = true;
		}

		return fired;
	}

	//---------------------------------------------------------------------------

	//! return true if firing weapon, or false
	virtual bool isFiring()
	{
		return m_Firing;
	}

	//---------------------------------------------------------------------------

	//! return true if weapon fired at least ones, or false
	virtual bool isWeaponFired()
	{
		return m_WeaponFired;
	}

	//---------------------------------------------------------------------------

	virtual s32 getFireCount()
	{
		return m_FireCount;
	}

	//----------------------------------------------------------------------------

	//! updating status
	virtual bool update()
	{
		if (IGameNode::update())
		{	
			m_NodeTransformation.transformVect( core::vector3df(1,0,0), m_NodeLookAt);				
			m_NodeLookDir = m_NodeLookAt - m_NodeAbsPosition;

			core::vector3df vec_tmp;

			m_NodeTransformation.transformVect( core::vector3df(0,0,-1), vec_tmp);
			m_NodeRightDir = vec_tmp - m_NodeAbsPosition;

			m_NodeTransformation.transformVect( core::vector3df(0,1,0), vec_tmp);
			m_NodeTopDir = vec_tmp - m_NodeAbsPosition;		
			
			// updating view fristrum of this game node

			core::vector3df &position = m_NodeAbsPosition;
			core::vector3df target = m_NodeAbsPosition + m_NodeLookDir;
			core::vector3df &up = m_NodeTopDir;

			m_ViewArea.setView(position, target, up);				

			return true;
		}

		return false;
	}

	core::vector3df getLookDir()
	{
		return m_NodeLookDir;
	}

	core::vector3df getRightDir()
	{
		return m_NodeRightDir;
	}

	core::vector3df getTopDir()
	{
		return m_NodeTopDir;
	}

	virtual bool moveByVec(const core::vector3df &vec)
	{
		if (!m_SceneNode)
			return false;

		m_SceneNode->setPosition(m_NodeAbsPosition+vec);

		return true;
	}

	virtual bool moveToPos(const core::vector3df &pos)
	{
		if (!m_SceneNode)
			return false;

		m_SceneNode->setPosition(pos);

		return true;
	}

	virtual bool pushByForce(const core::vector3df &force)
	{
		if (!m_SceneNode)
			return false;

		m_SceneNode->setPosition(m_NodeAbsPosition+force, true);

		return true;
	}

	virtual bool pushToPos(const core::vector3df &pos)
	{
		if (!m_SceneNode)
			return false;

		m_SceneNode->setPosition(pos, true);

		return true;
	}
	
	virtual bool pushInDir(core::vector3df dir, f32 speed = 1.0f)
	{
		if (!m_SceneNode)
			return false;

		CHECK_RANGE(speed, 0.0f, 1.0f);

		if (speed > 0)
		{
			dir.normalize();	

			dyn::IDynamicObject *dynobj = m_SceneNode->getDynamicObject();	

			core::vector3df node_new_pos;

			speed *= m_MoveSpeedBreakCoeff;

			if (dynobj)
			{
				dyn::SDynamicObjectParams dpar = dynobj->getParameters();
				if (dpar.Type != dyn::EDOT_NONE)
				{
					node_new_pos = 
						m_NodePosition + dir * 0.5 * dpar.MassValue  * speed;
				}
			}
			else
			{
				node_new_pos = m_NodePosition + 
					dir * 0.5 * m_DeltaTimeSec * 
					m_GameCommonParams.Game.MovSpeed * speed;		
			}

			// setting new position 
			m_SceneNode->setPosition(node_new_pos, true);  			
		}

		return true;
	}

	virtual bool orientInHorizDir(core::vector3df dir, f32 speed = 1.0f)
	{
		if (!m_SceneNode)
			return false;

		CHECK_RANGE(speed, 0.0f, 1.0f);	 	

		core::vector3df node_new_rot = m_NodeRotation, node_dir_tmp(dir.X, 0, dir.Z);

		// detecting rotation angle

		f32 angle    = node_dir_tmp.getAngleDeg(m_NodeLookDir); 	
		f32 rotangle = 
			angle * m_DeltaTimeSec * 0.2f * 
			m_GameCommonParams.Game.RotSpeed * speed;

		if (m_DeltaTimeSec==0)
			rotangle = angle;
		else
			CHECK_RANGE(rotangle, 0.0f, angle);

		// need to detect in which order we must rotate this enemie (CW or CCW)

		core::triangle3df tri( m_NodePosition, m_NodePosition+dir, m_NodeLookAt );   

		if (tri.isFrontFacing(core::vector3df(0,-1,0)))
		// CW 
			node_new_rot.Y = node_new_rot.Y - rotangle;
		else 
		// CCW
			node_new_rot.Y = node_new_rot.Y + rotangle;

		// setting new rotation   
		m_SceneNode->setRotation(node_new_rot, true);

		return true;
	}

	//----------------------------------------------------------------------------

	bool orientByAngles(core::vector3df angles, f32 speed = 1.0f)
	{
		if (!m_SceneNode)
			return false;

		// setting new rotation 
		m_SceneNode->setRotation(	
			m_NodeRotation + angles * m_GameCommonParams.Game.RotSpeed * speed,
			true
			);   

		return true;
	}

	//----------------------------------------------------------------------------

	virtual void OnEnemyDetected()
	{
		m_EndTimeEnemyLookUp = m_NowTimeMs+m_ParametersAI.MemoryTime;
	}

	//----------------------------------------------------------------------------

	virtual E_WAR_SIDE getWarSide()
	{
		return EWS_FRIEND;
	}

	//----------------------------------------------------------------------------

	virtual bool isGaming()
	{
		return	!isDead() && (
			m_NowTimeMs<m_EndTimeEnemyLookUp ||
			isRendered() || 
			getMoveSpeed()>1.0f	||					
			m_InputGameTasksIterators.size()>0 ||
			m_OutputGameTasksIterators.size()>0
			);
	}

	//----------------------------------------------------------------------------

	virtual bool isWarWith(IGameNodeAI* other)
	{
		return
			(getWarSide()^other->getWarSide())!=EWS_NEUTRAL &&
			(other->getWarSide()^getWarSide())!=EWS_NEUTRAL;
	}

	//----------------------------------------------------------------------------

	//! binding bullet boom effect (multiple allowed)
	void bindHurtEffect(SHurtEffectParameters &effect)
	{
		if (!effect.getSceneNode())
			return;

		effect.getSceneNode()->setCullingFilter( m_SceneNode->getCullingFilter() );
		effect.getSceneNode()->setVisible(false);

		m_HurtEffects.push_back(effect);
	}

	//----------------------------------------------------------------------------

	//! getting bullet boom effects
	const core::array<SHurtEffectParameters>& getHurtEffects()
	{
		return m_HurtEffects;
	}

protected:

	s32 m_StartWaitATurnTimeMs;

	virtual bool finishAttack(IGameNode* victim)
	{
		fireWeapon(false);

		return true;
	}

	virtual bool attack(bool can_mele_attack, IGameNode* victim) 
	{
		return false;
	}

	virtual bool die() 
	{
		if (!IGameNode::die()) 
			return false;

		fireWeapon(false);
		
		dyn::IDynamicObject *dobj = m_SceneNode->getDynamicObject();

		if (dobj)
		{
			dyn::SDynamicObjectParams dpar = dobj->getParameters();

			dpar.GeomForm = dyn::EDOGF_BOX;
			dpar.DynamicConstraints = dyn::EDTC_ALL_TRANSF^dyn::EDTC_ROTY;
			dpar.DynamicIgnoreFilter = 0xffffffff^dyn::EDCF_MONOLITH_COLLID;
				
			dobj->setParameters(dpar);

			dobj->getAutoUpdateGeomParams() = true;
		}

		s32 mat_cnt = m_SceneNode->getMaterialsCount();
		for (s32 m=0; m<mat_cnt; m++)
		{
			vid::SMaterial& mat = m_SceneNode->getMaterial(m);
			s32 pass_cnt = mat.getPassesCount();
			for (s32 p=0; p<pass_cnt; p++)
			{
				vid::SRenderPass& pass = mat.getPass(p);

				img::SColor diffc = m_ParametersAI.BloodColor;
				diffc.setAlpha(255);

				img::SColor ambc = pass.getAmbientColor ();
				ambc.setRed		( 0.75f * ambc.getRed()		);
				ambc.setGreen	( 0.75f * ambc.getGreen()	);
				ambc.setBlue	( 0.75f * ambc.getBlue()	);

				pass.setDiffuseColor ( diffc );
				pass.setAmbientColor ( ambc );
			}
		}

		setState(EAIS_DYING);

		return true;
	}

	core::vector3df m_NodeLookAt, m_NodeLookDir, m_NodeRightDir, m_NodeTopDir;

	s32 m_FireCount;

	bool m_Firing, m_PrepareForFiring, m_WeaponFired;
	s32 m_StartFiringTime;

	s32 m_SelectedWeaponNumber;

	SWeaponParameters m_WeaponParameters;

	SGameNodeAIParams m_ParametersAI;
	
	E_AI_STATE m_AIState;
	
	IGameUnitsFormation* m_Formation;

	core::view_frustrum m_ViewArea;

	//! path finder task which helps to find paths
	IPathFindTask* m_PathFindTask;

	core::vector3df m_PathFinderPosOffset;

	s32 m_StandingTimeMs;

	s32 m_EndTimeEnemyLookUp;

	scn::ILightSceneNode *m_FireLight;

	f32 m_FireMaxRadius;
	s32 m_FirePerSecond;

	f32 m_FireFlashTimeMs, m_FireFlashIntensity;

	core::array<SHurtEffectParameters> m_HurtEffects;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeAIHPP