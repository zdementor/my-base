//|-------------------------------------------------------------------------
//| File:        IGameNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeHPP
#define IGameNodeHPP
//----------------------------------------------------------------------------
					  
#include <my_inc.h>
#include <IUnknown.h>
#include <vid/SMaterial.h>
#include <dyn/IDynamicObject.h>
#include <scn/ISceneNode.h>
#include <game/GameTasks.h>
#include <game/GameEvents.h>
#include <mm/ISound.h>
#include <game/IGameInventory.h>
#include <game/IGameTasksManager.h>
#include <scn/ISceneManager.h>
#include <game/IGameManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

#define SAFE_CAST_TO_GAME_NODE_AI(game_node) (                    \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_MAIN_PLAYER ||   \
        game_node->getGameNodeType()==game::EGNT_PERSON           \
    )                                                             \
) ? static_cast<game::IGameNodeAI*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_TRIGGER(game_node) (               \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_TRIGGER          \
    )                                                             \
) ? static_cast<game::IGameNodeTrigger*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_DOOR(game_node) (                  \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_DOOR             \
    )                                                             \
) ? static_cast<game::IGameNodeDoor*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_PLATFORM(game_node) (              \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_PLATFORM         \
    )                                                             \
) ? static_cast<game::IGameNodePlatform*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_BUTTON(game_node) (                \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_BUTTON           \
    )                                                             \
) ? static_cast<game::IGameNodeButton*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_MAIN_PLAYER(game_node) (           \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_MAIN_PLAYER      \
    )                                                             \
) ? static_cast<game::IGameNodeMainPlayer*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_WEAPON(game_node) (                \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_WEAPON           \
    )                                                             \
) ? static_cast<game::IGameNodeWeapon*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_BULLET(game_node) (                \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_BULLET           \
    )                                                             \
) ? static_cast<game::IGameNodeBullet*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_ACTIVATABLE(game_node) (           \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_BUTTON ||        \
        game_node->getGameNodeType()==game::EGNT_DOOR ||          \
		game_node->getGameNodeType()==game::EGNT_PLATFORM ||      \
		game_node->getGameNodeType()==game::EGNT_TRIGGER          \
    )                                                             \
) ? static_cast<game::IGameNodeActivatable*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_LEVEL_MAP(game_node) (             \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_LEVEL_MAP        \
    )                                                             \
) ? static_cast<game::IGameNodeLevelMap*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_TERRAIN(game_node) (               \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_TERRAIN          \
    )                                                             \
) ? static_cast<game::IGameNodeTerrain*>(game_node) : NULL

#define SAFE_CAST_TO_GAME_NODE_ITEM(game_node) (                  \
    game_node && (                                                \
        game_node->getGameNodeType()==game::EGNT_ITEM             \
    )                                                             \
) ? static_cast<game::IGameNodeItem*>(game_node) : NULL

//----------------------------------------------------------------------------

//! game node params
struct SGameNodeParams
{
    SGameNodeParams() 
        : Type(EGNT_DECORATION), ClassName("Standard"), 
	MovSpeed(0), RotSpeed(0), Life(0)
    {}  

    SGameNodeParams(E_GAME_NODE_TYPE gntype, c8 *class_name) 
        : Type(gntype), ClassName(class_name), 
	MovSpeed(0), RotSpeed(0), Life(0)
    {}  

    SGameNodeParams(const SGameNodeParams& p)
    {
        set(p);            
    }

	SGameNodeParams(const SGameNodeParams& p, E_GAME_NODE_TYPE gntype)
    {
        set(p); 
		Type = gntype;
    }

    inline SGameNodeParams& operator=(const SGameNodeParams& p)
    {
		set(p);
		return *this;
    } 

	inline void set(const SGameNodeParams& p) 
	{
		ClassName = p.ClassName;
        Type = p.Type;
		MovSpeed = p.MovSpeed;
		RotSpeed = p.RotSpeed;
        Life = p.Life;
	}

    inline bool operator==(const SGameNodeParams& p)
    {
       return !((*this)!=p);
    }

    inline bool operator!=(const SGameNodeParams& p)
    {
        return
			ClassName	!= p.ClassName ||
            Type		!= p.Type || 
			MovSpeed	!= p.MovSpeed ||
			RotSpeed	!= p.RotSpeed ||
            Life	!= p.Life;
    }

	core::stringc ClassName;
    E_GAME_NODE_TYPE Type;
	f32 MovSpeed, RotSpeed, Life;
};

//----------------------------------------------------------------------------

//! Game node common params - used scene node, dynamic object and game node
struct SGameNodeCommonParams
{
    SGameNodeCommonParams()
    {     
    }

    SGameNodeCommonParams(const SGameNodeCommonParams& p)
    {
        set(p);            
    }

    inline SGameNodeCommonParams& operator=(const SGameNodeCommonParams& p)
    {
		set(p);
		return *this;
    } 

	inline void set(const SGameNodeCommonParams& p) 
	{
		Scene    = p.Scene;
		Dynamic  = p.Dynamic;
		Game     = p.Game;
	} 

    scn::SSceneNodeCommonParams Scene;
    dyn::SDynamicObjectParams   Dynamic;
    game::SGameNodeParams       Game;
};                                                                               

//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class __MY_GAME_LIB_API__ IGameNode : public scn::IAnimationCallback
{   
    friend class CGameManager;
	friend class CGameInventory;

public:

    IGameNode(scn::ISceneNode* n, SGameNodeParams &params) 
        : m_SceneNode(n), m_DeltaTimeSec(0),m_NowTimeMs(0),
	m_NodeAbsPosition(0,0,0), m_NodePosition(0,0,0), m_NodeRotation(0,0,0), m_AutoCreated(false),
	m_ParentInventory(0), m_Enabled(true), m_MoveSpeedBreakCoeff(1.0f), m_IsLive(true),
	m_GameNodesIterator(0), m_GameNodesByTypeIterator(0), m_GameNodesAIIterator(0)
    {
		m_GameCommonParams.Game.Type		= params.Type;
		m_GameCommonParams.Game.ClassName	= params.ClassName;

        if(m_SceneNode)
        {
            m_SceneNode->GameNode = this;
			m_SceneNode->setAnimationCallback(this);
        }

        setParameters(params);

		m_Inventory = game::createGameInventory(this);

		update();

        game::onCreateGameNode(this);	
    }

    virtual ~IGameNode() 
    {
		game::onDeleteGameNode(this);

        if(m_SceneNode) 
        { 
			m_SceneNode->setAnimationCallback(NULL);
			m_SceneNode->GameNode = NULL; 
			m_SceneNode = NULL; 
        }      

		SAFE_DROP(m_Inventory);

		if (m_ParentInventory)
			m_ParentInventory->removeObject(this);
		
		removeAllSoundEffects();

		// all game tasks/events assotiated with this game node make completed
		clearAllTasks();	
		clearAllEvents();
    }

	virtual void OnAnimationBegin(scn::ISceneNode *node, u32 animation_idx)
	{
		if (node == m_SceneNode)
			scn::onAnimationBegin(node, animation_idx);	
	}

	virtual void OnAnimationEnd(scn::ISceneNode *node, u32 animation_idx)
	{
		if (node == m_SceneNode)
			scn::onAnimationEnd(node, animation_idx);	
	}

	virtual game::IGameInventory& getInventory()
	{
		return *m_Inventory; 
	}

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPreGame(s32 now_time_ms)
	{
		m_NowTimeMs=now_time_ms;
	}

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPostGame(s32 now_time_ms)
	{	
		m_NowTimeMs=now_time_ms;
	}

    //! calling every time before doing dynamic
    virtual void OnPreDynamic(f64 delta_time_sec)
	{
		_updateParams(delta_time_sec);	

		core::list< core::list< SGameTask* >::iterator >::iterator it = 
			m_InputGameTasksIterators.begin();

		scn::ISceneManager &smgr = SCENE_MANAGER;

		for (; it != m_InputGameTasksIterators.end(); ++it)
		{
			SGameTask* gtask = **it;

			if (!gtask->Completed)
			{
				if (gtask->getType() == EGTT_DIE)
				{
					die();					
					gtask->Completed = true;  					
					m_IsLive = false;   					
				} 				
				else
				if (gtask->getType() == EGTT_HITED)
				{
					SGameTaskHited* hitaction = 
						static_cast<SGameTaskHited*>(gtask);
					addLife(-hitaction->HitPower);
					gtask->Completed = true;   					
				}
				else
				{
					gtask->Completed = true;
				}
			}
		}	
	}

    //! calling every time after doing dynamic
    virtual void OnPostDynamic(f64 delta_time_sec) = 0;

    //! calling every time before game sound playing
    virtual void OnPreSound()
	{
		// positioning all sound effects, attached to this node
		core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

		for (; it!=m_SoundEffects.end(); ++it)
		{
			mm::ISound *sound = (*it);

			if (sound)
			{
				sound->moveTo(m_TransformedBBoxCenter);
			}
		}
	}

    //! calling every time after game sound playing
    virtual void OnPostSound()
	{		
	}

	//!
	virtual void setAllTasksCompleted()
	{
		core::list< core::list< SGameTask* >::iterator >::iterator it = 
			m_InputGameTasksIterators.begin();
		for (; it != m_InputGameTasksIterators.end(); ++it)
			(**it)->Completed = true;
		it = m_OutputGameTasksIterators.begin();
		for (; it != m_OutputGameTasksIterators.end(); ++it)
			(**it)->Completed = true;
	}

	//!
	virtual void setAllEventsHandled()
	{
		core::list< core::list< SGameEvent* >::iterator >::iterator it = 
			m_InputGameEventsIterators.begin();
		for (; it != m_InputGameEventsIterators.end(); ++it)
			(**it)->Handled = true;	
		it = m_OutputGameEventsIterators.begin();
		for (; it != m_OutputGameEventsIterators.end(); ++it)
			(**it)->Handled = true;
	}

	virtual void clearAllTasks()
	{
		core::list< core::list< SGameTask* >::iterator >::iterator it = 
			m_InputGameTasksIterators.begin();
		for (; it != m_InputGameTasksIterators.end(); ++it)
		{
			(**it)->Completed = true;
			(**it)->Receiver = NULL;
			(**it)->ReceiverIterator = 0;
		}
		it = m_OutputGameTasksIterators.begin();
		for (; it != m_OutputGameTasksIterators.end(); ++it)
		{
			(**it)->Completed = true;
			(**it)->Sender = NULL;
			(**it)->SenderIterator = 0;
		}
		m_InputGameTasksIterators.clear();
		m_OutputGameTasksIterators.clear();
	}

	//!
	virtual void clearAllEvents()
	{
		core::list< core::list< SGameEvent* >::iterator >::iterator it = 
			m_InputGameEventsIterators.begin();
		for (; it != m_InputGameEventsIterators.end(); ++it)
		{
			(**it)->Handled = true;	
			(**it)->Dest = NULL;
			(**it)->DestIterator = 0;
		}
		it = m_OutputGameEventsIterators.begin();
		for (; it != m_OutputGameEventsIterators.end(); ++it)
		{
			(**it)->Handled = true;
			(**it)->Source = NULL;
			(**it)->SourceIterator = 0;
		}
		m_InputGameEventsIterators.clear();
		m_OutputGameEventsIterators.clear();
	}

    //! return type of game node
    virtual E_GAME_NODE_TYPE getGameNodeType() 
    {
        return m_GameCommonParams.Game.Type;
    }

    //! get game node parameters
    const SGameNodeParams& getParameters()
    {
        return m_GameCommonParams.Game;
    }

    //! setting game node parameters
    void setParameters(SGameNodeParams &game_params)
    {
        m_GameCommonParams.Game.Life     = game_params.Life;
        m_GameCommonParams.Game.MovSpeed = game_params.MovSpeed;
        m_GameCommonParams.Game.RotSpeed  = game_params.RotSpeed;

		setLife(game_params.Life);
    }
    
    //! returns scene node assotiated with this game node
    virtual scn::ISceneNode* getSceneNode()
    {
        return m_SceneNode;
    }

	//! returns dynamic object assotiated with this game node
    virtual dyn::IDynamicObject* getDynamicObject()
    {
        return m_SceneNode ? m_SceneNode->getDynamicObject() : NULL;
    }

    //! return true if this game node attached to main player
    virtual bool isMainPlayer()
    {
        return m_SceneNode ? m_SceneNode->isMainPlayer() : false;
    }

    //! ading sound effect to this game node
    virtual void addSoundEffect(mm::ISound *sound)
    {
		if (!sound)
			return;

        m_SoundEffects.push_back(sound);

		sound->grab();
    }

	//! playing sound effect, attached to this game node
    virtual bool playSoundEffect(s32 idx=0)
    {	
		mm::ISound* sound = getSoundEffect(idx);

		if (sound)
		{
			//if (sound->isPlaying())
			//	sound->stop();
			sound->play();

			return true;
		}

		return false;
    }

	//! playing sound effect, attached to this game node
    virtual bool stopSoundEffect(s32 idx=0)
    {	
		mm::ISound* sound = getSoundEffect(idx);

		if (sound)
		{
			sound->stop();

			return true;
		}

		return false;
    }

	virtual bool isPlayingSoundEffect(s32 idx)
	{
		mm::ISound* snd = getSoundEffect(idx);

		if (snd)
		{
			return snd->isPlaying();
		}

		return false;
	}

    //! return sound effect, attached to this game node
    virtual mm::ISound* getSoundEffect(s32 idx)
    {
		if (idx>=0 && idx<(s32)m_SoundEffects.size())
		{
			return *(m_SoundEffects.begin() + idx);
		}

		return NULL;
    }

	//! return sound effects count
    virtual s32 getSoundEffectsCount()
    {
		return m_SoundEffects.size();
    }

	//! removing all sound effects from this game node
    virtual void removeAllSoundEffects()
    {
		core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

		for (; it!=m_SoundEffects.end(); ++it)
		{
			SAFE_DROP(*it);
		}

		m_SoundEffects.clear();
    }

	//! removing sound effect from this game node
    virtual bool removeSoundEffect(mm::ISound *sound)
    {
		bool res = false;

		core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

		for (; it!=m_SoundEffects.end(); ++it)
		{
			if (sound == (*it))
			{
				SAFE_DROP(*it);
				m_SoundEffects.erase(it);

				res = true;

				break;
			}			
		}

		return res;
    }

	//! setting sound effects volume
	virtual void setSoundEffectsVolume(f32 volume)
	{
		CHECK_RANGE(volume, 0.0f, 1.0f);

		core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

		for (; it!=m_SoundEffects.end(); ++it)
		{
			if (*it)
				(*it)->setVolume(volume);
		}
	}

	//! setting auto created flag
	void setAutoCreatedFlag(bool value)
	{
		m_AutoCreated = value;
	}

	//! getting auto created flag
	bool isAutoCreated()
	{
		return m_AutoCreated;
	}

	const core::vector3df& getLastDislocation() const
	{
		return m_LastNodeAbsPosition;
	}

	const core::vector3df& getDislocation()  const
	{ 
		return m_NodeAbsPosition; 
	}

	const core::vector3df& getLastOrientation() const
	{
		return m_LastNodeRotation;
	}

	const core::vector3df& getOrientation()  const
	{ 
		return m_NodeRotation; 
	}

	const core::matrix4& getTransformation() const
	{
		return m_NodeTransformation;
	}

	const core::vector3df& getMoveDir() const
	{
		return m_NodeMoveDir;
	}

	const core::aabbox3df& getBoundingBox() const
	{
		return m_NodeBoundingBox;
	}

	const core::aabbox3df& getTransformedBBox() const
	{
		return m_NodeTransformedBBox;
	}

	const core::vector3df& getTransformedBBoxCenter() const
	{
		return m_TransformedBBoxCenter;
	}

	f32 getNodeSphereRadius() const
	{
		return m_NodeSphereRadius;
	}

	f32 getMoveSpeed()
	{
		return m_NodeMoveSpeed;
	}

	const c8* getGameClassName()
	{
		return m_GameCommonParams.Game.ClassName.c_str();
	}

	bool isEnabled()
	{
		return m_Enabled;
	}

	void setEnabled(bool enabled)
	{
		if (m_Enabled!=enabled && m_SceneNode)
		{
			if (enabled)
			{
				m_SceneNode->attachDynamicObject(m_GameCommonParams.Dynamic);
			}
			else
			{
				m_SceneNode->detachDynamicObject();
			}			
		}

		m_Enabled = enabled;
	}

	virtual bool isRendered()
	{	
		return m_SceneNode ? m_SceneNode->isRendered() : false;	
	}

	virtual bool isGaming()
	{
		return	!isDead() && m_SceneNode->isVisible() && (			
			isRendered() || getMoveSpeed()>1.0f	||					
			m_InputGameTasksIterators.size()>0 ||
			m_OutputGameTasksIterators.size()>0
			);
	}						

	//! return curent animation played index
	virtual s32 getAnimation() 
	{
		return m_SceneNode->getCurrentAnimation();
	}

	//! return curent animation progress (0.0 - 1.0)
	virtual f32 getAnimationProgress() 
	{
		return m_SceneNode->getCurrentAnimationProgress();
	}

	//! return true if this ai node dead
	virtual bool isDead()
	{
		return !m_IsLive;
	}

	//! return true if this ai node live
	virtual bool isLive()
	{
		return m_IsLive;
	}

	//! increase/decrease life
	virtual void addLife(f32 add_val)
	{
		m_CurLife += add_val;

		CHECK_MAX_RANGE(m_CurLife, m_GameCommonParams.Game.Life);
	}

	//! return life value
	virtual f32 getLife()
	{
		return m_CurLife;
	}

	//! setting life value
	virtual void setLife(f32 val)
	{
		m_CurLife = val;
	}

	//! setting max life value
	virtual void setMaxLife(f32 val)
	{
		m_GameCommonParams.Game.Life = val;
	}

	//! getting max life value
	virtual f32 getMaxLife()
	{
		return m_GameCommonParams.Game.Life;
	}
	
	//----------------------------------------------------------------------------

	//! updating status
	virtual bool update()
	{
		if (m_SceneNode && isLive())
		{	
			m_NodeTransformation = m_SceneNode->getAbsoluteTransformation();
			m_NodePosition       = m_SceneNode->getPosition();
			m_NodeRotation       = m_SceneNode->getRotation(); 
 
			m_NodeBoundingBox       = m_SceneNode->getBoundingBox();
			m_NodeTransformedBBox   = m_SceneNode->getTransformedBoundingBox();
			m_TransformedBBoxCenter = m_NodeTransformedBBox.getCenter();

			m_NodeSphereRadius = m_NodeBoundingBox.getExtend().getLength()/2.0f;

			m_NodeTransformation.transformVect( core::vector3df(0,0,0), m_NodeAbsPosition);
			
			m_NodeMoveDir = m_NodeAbsPosition-m_LastNodeAbsPosition;

			if (m_NodeMoveDir.getLength()>5)
			{
				m_NodeMoveDir.normalize();
			}
			else
			{
				m_NodeMoveDir.set(0,0,0);
			}			

			return true;
		}

		return false;
	}

	//----------------------------------------------------------------------------

	//! gets game common parameters
	const SGameNodeCommonParams& getGameCommonParams() const
	{
		return m_GameCommonParams;
	}

	//----------------------------------------------------------------------------

	//! gets game common parameters
	void updateGameCommonParams()
	{
		if (!m_SceneNode) 
			return;		

		// Scene

		m_GameCommonParams.Scene = m_SceneNode->getParameters();

		// Dynamic	
		
		dyn::IDynamicObject *ode_obj = m_SceneNode->getDynamicObject();	

		m_GameCommonParams.Dynamic = 
			ode_obj ? ode_obj->getParameters() : dyn::SDynamicObjectParams();
	}

	//! return game input tasks pool
	virtual core::list< core::list< SGameTask* >::iterator >& getInputGameTasksIterators()
	{
		return m_InputGameTasksIterators;
	}

	//! return game output tasks pool
	virtual core::list< core::list< SGameTask* >::iterator >& getOutputGameTasksIterators()
	{
		return m_OutputGameTasksIterators;
	}

	//! return game input events pool
	virtual core::list< core::list< SGameEvent* >::iterator >& getInputGameEventsIterators()
	{
		return m_InputGameEventsIterators;
	}

	//! return game output events pool
	virtual core::list< core::list< SGameEvent* >::iterator >& getOutputGameEventsIterators()
	{
		return m_OutputGameEventsIterators;
	}

	virtual E_WAR_SIDE getWarSide()
	{
		return EWS_NEUTRAL;
	}

	//---------------------------------------------------------------------------

	//! return string with scene node's core properties in xml format
	const c8* getSceneCorePropertiesXMLString()
	{
		return !m_SceneNode ? NULL :
			m_SceneNode->getSceneCorePropertiesXMLString();
	}

	//---------------------------------------------------------------------------

	//! return string with dynamic object's core properties in xml format
	const c8* getDynamicCorePropertiesXMLString()
	{
		static core::stringc xmlstr;
		xmlstr.sprintf(
			"<DynamicParameters dynamic_type=\"%s\" geom_form=\"%s\" mass_value=\"%.2f\"\n"
			"surf_mu=\"%.6f\" surf_bounce=\"%.6f\" surf_bounce_vel=\"%.6f\" surf_soft=\"%.6f\"\n"
			"dynamic_constraints=\"%d\" />\n",
			dyn::DynamicObjectTypeStr[m_GameCommonParams.Dynamic.Type],
			dyn::GeomFormStr[m_GameCommonParams.Dynamic.GeomForm],
			m_GameCommonParams.Dynamic.MassValue,							
			m_GameCommonParams.Dynamic.SurfMu,
			m_GameCommonParams.Dynamic.SurfBounce,
			m_GameCommonParams.Dynamic.SurfBounceVel,
			m_GameCommonParams.Dynamic.SurfSoft,
			m_GameCommonParams.Dynamic.DynamicConstraints);
		return xmlstr.c_str();
	}

	//---------------------------------------------------------------------------

	//! return string with game node's core properties in xml format
	virtual const c8* getGameCorePropertiesXMLString()
	{
		static core::stringc xmlstr;
		xmlstr.sprintf(
			"<GameParameters mov_speed=\"%.2f\" rot_speed=\"%.2f\" life=\"%.2f\" />\n",
			m_GameCommonParams.Game.MovSpeed,
			m_GameCommonParams.Game.RotSpeed,
			m_GameCommonParams.Game.Life);
		return xmlstr.c_str();
	} 	
	
protected: 

	virtual bool die() 
	{
		if (!m_SceneNode) 
			return false;
		
		setAllTasksCompleted(); 

		game::onDieGameNode(this);

		return true;
	}

	void _updateParams(f64 delta_time_sec)
	{
		m_DeltaTimeSec = delta_time_sec;

		if (m_DeltaTimeSec>0)
			m_NodeMoveSpeed = (m_LastNodePosition-m_NodePosition).getLength() / m_DeltaTimeSec;				
		else
			m_NodeMoveSpeed=0;
				
		m_MoveSpeedBreakCoeff *= 
			m_GameCommonParams.Game.MovSpeed / m_NodeMoveSpeed;
				
		CHECK_RANGE(m_MoveSpeedBreakCoeff, 0.0f, 1.0f);

		m_LastNodePosition       = m_NodePosition;
		m_LastNodeRotation       = m_NodeRotation;
		m_LastNodeTransformation = m_NodeTransformation;
		m_LastNodeAbsPosition    = m_NodeAbsPosition;
	}
    
	f32             m_NodeMoveSpeed;
	core::vector3df m_LastNodeAbsPosition,m_NodeAbsPosition;
	core::vector3df m_LastNodePosition,   m_NodePosition;
	core::vector3df m_LastNodeRotation,   m_NodeRotation;
	core::matrix4   m_NodeTransformation, m_LastNodeTransformation;
	core::aabbox3df m_NodeBoundingBox, m_NodeTransformedBBox;	
	core::vector3df m_TransformedBBoxCenter;
	core::vector3df m_NodeMoveDir;

	f32 m_MoveSpeedBreakCoeff;

	SGameNodeCommonParams m_GameCommonParams;

    f32 m_CurLife;

    scn::ISceneNode *m_SceneNode;     

	core::list<mm::ISound*> m_SoundEffects;	

	f64 m_DeltaTimeSec;
	s32 m_NowTimeMs;

	bool m_AutoCreated, m_Enabled;

	IGameInventory *m_Inventory, *m_ParentInventory;

	bool m_IsLive;

	core::list<IGameNode*>::iterator   m_GameNodesIterator;
	core::list<IGameNode*>::iterator   m_GameNodesByTypeIterator;
	core::list<IGameNodeAI*>::iterator m_GameNodesAIIterator;

	f32 m_NodeSphereRadius;

	core::list< core::list< SGameTask* >::iterator > m_InputGameTasksIterators;
	core::list< core::list< SGameTask* >::iterator > m_OutputGameTasksIterators;

	core::list< core::list< SGameEvent* >::iterator > m_InputGameEventsIterators;
	core::list< core::list< SGameEvent* >::iterator > m_OutputGameEventsIterators;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeHPP