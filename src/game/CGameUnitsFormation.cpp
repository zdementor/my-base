//|-------------------------------------------------------------------------
//| File:        CGameUnitsFormation.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameUnitsFormation.h"

#include <dev/IDevice.h>
#include <game/IGameTasksManager.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameUnitsFormation::CGameUnitsFormation(
	core::dimension2di dim, s32 formation_id
	) :
FormationID(-1), MaxUnitsCount(0), FormationState(EFS_FORMING), Commander(0),
FirstForming(true)
{
#if MY_DEBUG_MODE  
	setClassName("CGameUnitsFormation");
#endif

	setFormationID(formation_id);
	setDimension(dim);	

	core::array< core::vector2df > points;

	points.push_back( core::vector2df(   0.0f,  0.0f ) );
	points.push_back( core::vector2df(  50.0f, 0.25f ) );
	points.push_back( core::vector2df( 100.0f,  1.0f ) );

	SpeedInterpolator.init(points.pointer(), points.size());	
}

//----------------------------------------------------------------------------

//! destructor
CGameUnitsFormation::~CGameUnitsFormation()
{    
	GAME_MANAGER.onDeleteNode(this);

	core::removeUniqueID(core::EUID_UNIT_FORMATION_ID, FormationID);	
	
	core::list<IGameNodeAI*>::iterator it = Units.begin();

	for (; it!=Units.end(); ++it)
	{
		IGameNodeAI *gnode = *it;

		if (gnode)
			gnode->m_Formation = NULL;
	}

	Units.clear();
}

//----------------------------------------------------------------------------

void CGameUnitsFormation::updateStatus()
{
	Commander = (UnitsDim.Width>0 && UnitsDim.Height>0) ? _Units[0][0].node : NULL;

	UnitsInFormationCount = 1;
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameUnitsFormation::OnPreDynamic(f64 delta_time_sec)
{
	IGameNodeAI::OnPreDynamic(delta_time_sec);

	updateStatus();	
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameUnitsFormation::OnPostDynamic(f64 delta_time_sec)
{
	IGameNodeAI::OnPostDynamic(delta_time_sec);

	core::list< core::list< SGameTask* >::iterator >::iterator it = 
		m_InputGameTasksIterators.begin();

	for (; it!=m_InputGameTasksIterators.end(); ++it)
	{
		SGameTask* gtask = **it;

		if (!gtask->Completed)
		{
			if (gtask->getType() == EGTT_FIND_PATH_AND_MOVE)
			{
				SGameTaskFindPathMove* movetask = 
					static_cast<SGameTaskFindPathMove*>(gtask);

				core::list<IGameNodeAI*>::iterator units_iter = Units.begin();
				
				if (!Commander)
					continue;

				core::matrix4 form_transf     = Commander->getTransformation();
				core::vector3df form_pos      = Commander->getDislocation();
				core::vector3df form_dest_pos = movetask->DestPos;										core::vector3df form_dir      = form_dest_pos - form_pos;

				core::vector3df cmd_look_dir  = Commander->getLookDir() * 100;

				s32 idx = 0, col_idx=0, row_idx=0;

				for (s32 w=0; w<UnitsDim.Width; w++)
				{
					for (s32 h=0; h<UnitsDim.Height; h++)
					{
						row_idx = h;
						col_idx = w;

						IGameNodeAI* unit = _Units[col_idx][row_idx].node;
						scn::ISceneNode* Node = 0;

						if (!unit || !(Node=unit->getSceneNode()) )
							continue;

						core::aabbox3df bbox   = Node->getBoundingBox();
						core::vector3df node_pos = unit->getDislocation();	
    
						core::vector3df dist_to_prev(0,0,0);
						core::vector3df dest_pos;
						f32 movspeed = 1.0f, rotspeed = 1.0f;
						IGameNodeAI* prev_unit=0;

						if (unit == Commander)
						{
							movspeed = 0.25f;
							rotspeed = 0.0005f;

							dest_pos = form_dest_pos;
						}
						else	
						{
							if (row_idx==0)
							{
								if (col_idx % 2 == 0)
								{
									dest_pos = 
										Commander->getDislocation() + Commander->getRightDir() * 50 * (col_idx/2);
								}
								else
								{
									dest_pos = 
										Commander->getDislocation() - Commander->getRightDir() * 50 * (col_idx/2 + 1);
								}
							}
							else
							{
								rotspeed = 0.0005f;

								prev_unit = _Units[col_idx][row_idx-1].node; 

								if (!prev_unit)
									continue;

								if (col_idx==0)
									dest_pos = 
										prev_unit->getDislocation() - prev_unit->getLookDir() * 50; 
								else							
								if (col_idx % 2 == 0)
								{
									if (_Units[0][row_idx].node)
									{
										dest_pos = 
											_Units[0][row_idx].node->getDislocation() +_Units[0][row_idx].node->getRightDir() * 50 * (col_idx/2);
									}
									else
									{
										dest_pos = 
											Commander->getDislocation() - Commander->getLookDir() * 50 * row_idx;
									}
								}
								else
								{
									if (_Units[0][row_idx].node)
									{
										dest_pos = 
											_Units[0][row_idx].node->getDislocation() -_Units[0][row_idx].node->getRightDir() * 50 * (col_idx/2 +1);
									}
									else
									{
										dest_pos = 
											Commander->getDislocation() - Commander->getLookDir() * 50 * row_idx;
									}
								}
							}								
						}

						core::vector3df dist_to_dest_vec = dest_pos - node_pos;						
						dist_to_dest_vec.Y=0;

						f32 dist_to_dest = dist_to_dest_vec.getLength();

						bool rich_dest = false;

						if (unit != Commander)
							movspeed = SpeedInterpolator.getValue(dist_to_dest);
						else 
						if (unit == Commander  && FormationState == EFS_FORMING)
							movspeed = 0.0f;
							
						core::vector3df node_dir;

						if ( dist_to_dest <= 0.1f * bbox.getExtend().Y )
						{
							UnitsInFormationCount++;

							movspeed = 0.0f;

							if (unit == Commander)
								gtask->Completed = true;
						}
						else
						{
							const core::array<core::vector3df> &path = 
								unit->findPathTo(dest_pos, movetask->NeedToRepath);
								
							movetask->NeedToRepath = false;								

							if (path.size()>=2)
							{
								node_dir = path[1]-path[0];								
							}
							else
								continue;	
								
							core::vector3df dir;

							if (unit != Commander && dist_to_dest<1000)
							{
								if (row_idx==0)
									dir=cmd_look_dir;
								else
									dir=prev_unit->getLookDir();
							}
							else
								dir=node_dir;

							unit->orientInHorizDir(dir, rotspeed);
						}	
							
						unit->pushInDir(node_dir, movspeed);						
					}
				}
			}
		}
	}

	s32 units_size = Units.size();

	if (UnitsInFormationCount!=MaxUnitsCount)
	{
		if (FirstForming || units_size<(MaxUnitsCount/2))
		{
			FormationState = EFS_FORMING;	
		}
	}
	else
	{
		FormationState = EFS_FORMED;

		FirstForming = false;
	}
}

//----------------------------------------------------------------------------

//! add unit to formation
bool CGameUnitsFormation::addUnit(IGameNodeAI *gnode)
{
	IGameNodeAI *gnodeai = SAFE_CAST_TO_GAME_NODE_AI(gnode);

	if (!gnodeai) return false;

	if ((s32)Units.size() >= UnitsDim.Width * UnitsDim.Height)
		return false;

	core::list<IGameNodeAI*>::iterator it = Units.begin();

	for (; it!=Units.end(); ++it)
	{
		IGameNodeAI *gn = *it;

		if (gn == gnodeai)
			return false;	
	}

	for (s32 w=0; w<UnitsDim.Width; w++)
	{
		for (s32 h=0; h<UnitsDim.Height; h++)
		{
			if (!_Units[w][h].node)
			{
				_Units[w][h].node = gnodeai;

				gnodeai->m_Formation = this;

				Units.push_back(gnodeai);

				return true;
			}
		}
	}

	return false;	
}

//----------------------------------------------------------------------------

//! remove unit from formation
bool CGameUnitsFormation::removeUnit(IGameNodeAI *gnode)
{
	IGameNodeAI *gnodeai = SAFE_CAST_TO_GAME_NODE_AI(gnode);

	if (!gnodeai) return false;

	bool finded = false;

	core::list<IGameNodeAI*>::iterator it = Units.begin();

	if (Commander == gnodeai)
		Commander = 0;

	for (; it!=Units.end(); ++it)
	{
		IGameNodeAI *gn = *it;

		if (gn == gnodeai)
		{
			finded = true;

			gnodeai->m_Formation = NULL;

			Units.erase(it);			

			break;
		}
	}	

	for (s32 w=0; w<UnitsDim.Width; w++)
	{
		for (s32 h=0; h<UnitsDim.Height; h++)
		{
			if (_Units[w][h].node == gnodeai)
			{
				_Units[w][h].node = NULL;

				if (w==0)
				// first column 
				{
					s32 hh;	
								
					for (hh=h; hh<UnitsDim.Height; hh++)
					{
						for (s32 hhh=hh+1; hhh<UnitsDim.Height; hhh++)
						{
							if (!_Units[0][hh].node && _Units[0][hhh].node)
							{
								_Units[0][hh].node = _Units[0][hhh].node;
								_Units[0][hhh].node = NULL;
								break;
							}
						}
					}
					for (hh=h; hh<UnitsDim.Height; hh++)
					{
						if (!_Units[0][hh].node)
						{
							for (s32 ww=1; ww<UnitsDim.Width; ww++)
							{
								if (_Units[ww][hh].node)
								{
									_Units[0][hh].node = _Units[ww][hh].node;
									_Units[ww][hh].node = NULL;
									break;
								}
							}
						}
					}
				}						
				return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------

//! remove all units from formation
void CGameUnitsFormation::removeAllUnits()
{
	core::list<IGameNodeAI*>::iterator it = Units.begin();

	for (; it!=Units.end(); ++it)
	{
		IGameNodeAI *gn = *it;

		if (gn)
			gn->m_Formation = NULL;
	}

	Units.clear();

	for (s32 w=0; w<UnitsDim.Width; w++)
	{
		for (s32 h=0; h<UnitsDim.Height; h++)
			_Units[w][h].node = NULL;
	}
}

//----------------------------------------------------------------------------

//! return unit from formation
IGameNodeAI* CGameUnitsFormation::getUnit(s32 col_idx, s32 row_idx)
{
	if (col_idx>0 && col_idx<UnitsDim.Width && 
		row_idx>0 && row_idx<UnitsDim.Height 
		)
	{
		return _Units[col_idx][row_idx].node;
	}

	return NULL;
}

//----------------------------------------------------------------------------

//! return units count in formation
s32 CGameUnitsFormation::getUnitsCount()
{
	return Units.size();
}

//----------------------------------------------------------------------------

//! return units maximal count in formation
s32 CGameUnitsFormation::getUnitsMaxCount()
{
	return MaxUnitsCount;
}

//----------------------------------------------------------------------------

//! return true if able to add one more unit in this formation
bool CGameUnitsFormation::canAddOneMoreUnit()
{
	bool res = (s32)Units.size()<MaxUnitsCount; 

	return res;
}

//----------------------------------------------------------------------------

s32 CGameUnitsFormation::getFormationID()
{
	return FormationID;
}

//----------------------------------------------------------------------------

bool CGameUnitsFormation::setFormationID(s32 new_id)
{
	if (core::addUniqueID(core::EUID_UNIT_FORMATION_ID, new_id))
	{
		core::removeUniqueID(core::EUID_UNIT_FORMATION_ID, FormationID);
		FormationID = new_id;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------

IGameNode* CGameUnitsFormation::getCommander()
{	
	return Commander;
}

//----------------------------------------------------------------------------

E_FORMATION_STATES CGameUnitsFormation::getFormationState()
{
	return FormationState;
}

//----------------------------------------------------------------------------

core::dimension2di CGameUnitsFormation::getDimension()
{
	return UnitsDim;	
}

//----------------------------------------------------------------------------

bool CGameUnitsFormation::setDimension(core::dimension2di &newdim)
{
	if ( (s32)Units.size() <= newdim.Width*newdim.Height )
	{
		UnitsDim = newdim;
		MaxUnitsCount = UnitsDim.Width*UnitsDim.Height;
		
		_Units.clear();

		_Units.set_used(UnitsDim.Width);

		for (s32 w=0; w<UnitsDim.Width; w++)
		{
			for (s32 h=0; h<UnitsDim.Height; h++)
				_Units[w].push_back(SUnitEntry());
		}

		core::list<IGameNodeAI*>::iterator it = Units.begin();

		for (; it!=Units.end(); ++it)
		{
			IGameNodeAI *gn = *it;

			if (gn)
				addUnit(gn);
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
