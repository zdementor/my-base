//|-------------------------------------------------------------------------
//| File:        IGameNodeActivatable.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeActivatableHPP
#define IGameNodeActivatableHPP
//----------------------------------------------------------------------------

#include <game/IGameNode.h>
#include <game/IGameTasksManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! GameNode Activatable class
class IGameNodeActivatable : public IGameNode
{

public:

	//! constructor
	IGameNodeActivatable(scn::ISceneNode* n, SGameNodeParams &params)
		: IGameNode(n, params), 
		m_ActivateMode(EOAM_AUTOMATIC), m_Activated(false),
		m_WaitTimeSec(2.0f), m_Lip(8.0f), 
		m_TargetNameKey(NONAME), m_TeamNameKey(NONAME), m_TargetKey(NONAME),
		m_TargetedByNode(NULL), m_ActivatedFlagOnLastTime(false), m_ActivatedFlag(false)
	{
	}

	//! destructor
	virtual ~IGameNodeActivatable() {}

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPreDynamic(f64 delta_time_sec)
	{
		IGameNode::_updateParams(delta_time_sec);

		m_ActivatedFlagOnLastTime = m_ActivatedFlag;

		m_ActivatedFlag = false;		
	}

    //! calling every time after doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPostDynamic(f64 delta_time_sec)
	{
		core::list< core::list <SGameTask*>::iterator >::iterator it = 
			m_InputGameTasksIterators.begin();

		for (; it!=m_InputGameTasksIterators.end(); ++it)
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
				if (gtask->getType() == EGTT_ACTIVATE)
				{
					IGameNode* sender = gtask->Sender;

					if (m_ActivateMode == EOAM_BY_BUTTON &&
						sender &&
						sender->getGameNodeType() != EGNT_BUTTON
						)
					{
						gtask->Completed = true;

						continue;							
					}

					m_ActivatedFlag = true;

					if (m_ActivatedFlag!=m_ActivatedFlagOnLastTime)
					{							
						if (activate(sender))
						{
							for (u32 tn=0; tn<m_TeamNodes.size(); tn++)
								m_TeamNodes[tn]->activate(sender);
						}
					}

					gtask->Completed = true;
				} 
				else
				{
					gtask->Completed = true;
				}
			}
		}

		if (!m_ActivatedFlag && m_ActivatedFlag!=m_ActivatedFlagOnLastTime)
		{
			if (deactivate())
			{
				for (u32 tn=0; tn<m_TeamNodes.size(); tn++)
					m_TeamNodes[tn]->deactivate();
			}			
		}
	}

	//! geturn true if this door activated, otherwise false
	virtual bool isActivated()
	{
		return m_Activated;
	}

	//! setting activate mode
	virtual void setActivateMode(E_OBJECT_ACTIVATE_MODE activate_mode)
	{
		m_ActivateMode = activate_mode;
	}

	//! getting activate mode
	virtual E_OBJECT_ACTIVATE_MODE getActivateMode()
	{
		return m_ActivateMode;
	}

	//! adding target - game node wich is activated by this node
	virtual void addTargetNode(IGameNodeActivatable* target)
	{
		if (target)
		{
			target->setTargetedByNode(this);

			m_TargetNodes.push_back(target);
		}		
	}

	//! setting activatable node target name key
	virtual void setTargetNameKey(const c8 *targetname)
	{
		m_TargetNameKey = targetname;
	}

	//! getting activatable node target name key
	virtual core::stringc& getTargetNameKey()
	{
		return m_TargetNameKey;
	}

	//! setting activatable node target key
	virtual void setTargetKey(const c8 *target)
	{
		m_TargetKey = target;
	}

	//! getting activatable node target key
	virtual core::stringc& getTargetKey()
	{
		return m_TargetKey;
	}

	//! setting wait, number of seconds  returns (default 2, -1 = return immediately) 
	virtual void setWaitTimeSec(s32 wait_sec)
	{
		m_WaitTimeSec = wait_sec;
	}

	//! returning wait, number of seconds before door returns
	virtual f32 getWaitTimeSec()
	{
		return m_WaitTimeSec;
	}

	//! setting lip value
	virtual void setLip(f32 lip)
	{
		m_Lip = lip;
	}

	//! getting lip value
	virtual f32 getLip()
	{
		return m_Lip;
	}
	
	//! 
	virtual void addTeamNode(IGameNodeActivatable* node )
	{
		m_TeamNodes.push_back(node);
	}

	//!
	virtual void setTeamNameKey(const c8 *teamname)
	{
		m_TeamNameKey = teamname;
	}

	//!
	virtual const c8* getTeamNameKey()
	{
		return m_TeamNameKey.c_str();
	}

	//! 
	virtual bool activate(IGameNode* activator=0)
	{
		return false;
	}

	//! 
	virtual bool deactivate()
	{
		return false;
	}

	virtual bool isGaming()
	{	
		bool res = m_Activated;

		for (u32 i=0; !res && i<m_TargetNodes.size(); i++)
		{
			res = m_TargetNodes[i]->isGaming();
		}

		return res ? true : IGameNode::isGaming();	
	}

protected:

	virtual void setTargetedByNode(IGameNodeActivatable* tgtby)
	{
		if (tgtby)
		{
			m_TargetedByNode = tgtby;
		}
	}

	E_OBJECT_ACTIVATE_MODE m_ActivateMode;

	bool m_Activated;

	core::stringc m_TargetNameKey, m_TeamNameKey, m_TargetKey;

	f32 m_WaitTimeSec;
	f32 m_Lip;

	core::array<IGameNodeActivatable*> m_TeamNodes;
	core::array<IGameNodeActivatable*> m_TargetNodes;
	
	IGameNodeActivatable* m_TargetedByNode;

	bool m_ActivatedFlagOnLastTime, m_ActivatedFlag;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IIGameNodeActivatableHPP