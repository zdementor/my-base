//|-------------------------------------------------------------------------
//| File:        IGameInventory.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameInventoryHPP
#define IGameInventoryHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Inventory Class
class __MY_GAME_LIB_API__ IGameInventory : public IUnknown
{   

public:

    //! destructor
    virtual ~IGameInventory() {}

	//! add Weapon to inventory
	virtual bool addWeapon(IGameNodeWeapon *weapon, s32 number) = 0; 

	//! return Weapon from inventory
	virtual IGameNodeWeapon* getWeapon(s32 number, s32 idx) = 0; 

	//! return Weapons count in inventory
	virtual s32 getWeaponsCount() = 0; 

	//! return Weapons sub count in inventory
	virtual s32 getWeaponsSubCount(s32 number) = 0; 

	//! remove Weapon from inventory
	virtual bool removeWeapon(IGameNodeWeapon *weapon) = 0; 

	//! add Item to inventory
	virtual bool addItem(IGameNodeItem *item) = 0; 

	//! return Item from inventory
	virtual IGameNodeItem* getItem(s32 idx) = 0; 

	//! return Item count in inventory
	virtual s32 getItemsCount() = 0; 

	//! remove Item from inventory
	virtual bool removeItem(IGameNodeItem *item) = 0; 

	//! add Game Node to inventory
	virtual bool addObject(IGameNode *gnode) = 0; 

	//! return Game Node from inventory
	virtual IGameNode* getObject(s32 idx) = 0; 

	//! return Game Nodes count in inventory
	virtual s32 getObjectsCount() = 0; 

	//! remove Object from inventory
	virtual bool removeObject(IGameNode *gnode) = 0; 

	//! remove all Objects from inventory
	virtual void removeAllObjects() = 0; 

	//!
	virtual IGameNode* getOwner() = 0;
    
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameInventoryHPP