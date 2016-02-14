//|-------------------------------------------------------------------------
//| File:        IGameUnitsFormation.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameUnitsFormationHPP
#define IGameUnitsFormationHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNodeAI.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Formation Class
class __MY_GAME_LIB_API__ IGameUnitsFormation : public IGameNodeAI
{
public:

	//! constructor
	IGameUnitsFormation() 
		: IGameNodeAI(NULL, SGameNodeParams(EGNT_UNITS_FORMATION, "Standard")) {}

	//! destructor
	virtual ~IGameUnitsFormation() {};

	//! add unit to formation
	virtual bool addUnit(IGameNodeAI *gnode) = 0; 

	//! remove unit from formation
	virtual bool removeUnit(IGameNodeAI *gnode) = 0; 

	//! remove all units from formation
	virtual void removeAllUnits() = 0; 

	//! return unit from formation
	virtual IGameNodeAI* getUnit(s32 col_idx, s32 row_idx) = 0; 

	//! return units count in formation
	virtual s32 getUnitsCount() = 0; 

	//! return units maximal count in formation
	virtual s32 getUnitsMaxCount() = 0; 

	//! return true if able to add one more unit in this formation
	virtual bool canAddOneMoreUnit() = 0; 

	virtual s32 getFormationID() = 0;

	virtual bool setFormationID(s32 new_id) = 0;

	virtual IGameNode* getCommander() = 0;

	virtual E_FORMATION_STATES getFormationState() = 0;

	virtual core::dimension2di getDimension() = 0;

	virtual bool setDimension(core::dimension2di &newdim) = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameUnitsFormationHPP