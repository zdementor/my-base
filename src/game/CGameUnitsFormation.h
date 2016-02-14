//|-------------------------------------------------------------------------
//| File:        CGameUnitsFormation.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameUnitsFormationHPP
#define CGameUnitsFormationHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameUnitsFormation.h>
#include "CGameManager.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameUnitsFormation : public IGameUnitsFormation
{
	friend class CGameManager;

public:

	//! constructor
	CGameUnitsFormation(core::dimension2di dim, s32 formation_id);

	//! destructor
	virtual ~CGameUnitsFormation();

	virtual void moveBy(core::vector3df dir,   f32 speed = 1.0f) {}

	virtual void orientTo(core::vector3df dir, f32 speed = 1.0f) {}

	virtual void orientBy(core::vector3df angles, f32 speed = 1.0f) {}

	core::vector3df getDislocation() 
	{ 
		return Commander ? Commander->getDislocation() : core::vector3df(0,0,0); 
	}

	core::vector3df getOrientation() 
	{ 
		return Commander ? Commander->getOrientation() : core::vector3df(0,0,0); 
	}

	virtual core::matrix4 getTransformation()
	{
		return Commander ? Commander->getTransformation() : core::matrix4(); 
	}

	virtual core::vector3df getLookDir()
	{
		return Commander ? Commander->getLookDir() : core::vector3df(1,0,0); 
	}

	virtual core::vector3df getRightDir()
	{
		return Commander ? Commander->getRightDir() : core::vector3df(0,0,-1); 
	}

	virtual core::vector3df getTopDir()
	{
		return Commander ? Commander->getTopDir() : core::vector3df(0,1,0); 
	}

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);

	//! add unit to formation
	virtual bool addUnit(IGameNodeAI *gnode); 

	//! remove unit from formation
	virtual bool removeUnit(IGameNodeAI *gnode); 

	//! remove all units from formation
	virtual void removeAllUnits(); 

	//! return unit from formation
	virtual IGameNodeAI* getUnit(s32 col_idx, s32 row_idx); 

	//! return units count in formation
	virtual s32 getUnitsCount(); 

	//! return units maximal count in formation
	virtual s32 getUnitsMaxCount(); 

	//! return true if able to add one more unit in this formation
	virtual bool canAddOneMoreUnit(); 

	virtual s32 getFormationID();

	virtual bool setFormationID(s32 new_id);

	virtual IGameNode* getCommander();

	virtual E_FORMATION_STATES getFormationState();	

	virtual core::dimension2di getDimension();

	virtual bool setDimension(core::dimension2di &newdim);

protected:

	virtual bool setFormation(game::IGameUnitsFormation* formation) { return false; }

private:

	struct SUnitEntry
	{
		SUnitEntry() : node(0) {}
		IGameNodeAI* node;
	};

	core::array< core::array<SUnitEntry> > _Units;

	void updateStatus();

	core::dimension2di UnitsDim;
	s32 MaxUnitsCount;

	s32 FormationID;
	
	core::list<IGameNodeAI*> Units;

	E_FORMATION_STATES FormationState;

	IGameNodeAI* Commander;

	core::math::pli2df SpeedInterpolator;

	s32 UnitsInFormationCount;

	bool FirstForming;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameUnitsFormationHPP