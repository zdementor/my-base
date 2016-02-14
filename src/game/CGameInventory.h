//|-------------------------------------------------------------------------
//| File:        CGameInventory.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameInventoryHPP
#define CGameInventoryHPP
//----------------------------------------------------------------------------

#include <game/IGameInventory.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Inventory Class
class __MY_GAME_LIB_API__ CGameInventory : public IGameInventory
{   

public:

	//! constructor
	CGameInventory(IGameNode *owner);

	//! destructor
	virtual ~CGameInventory();
		
	//! add Weapon to inventory
	virtual bool addWeapon(IGameNodeWeapon *weapon, s32 number); 

	//! return Weapon from inventory
	virtual IGameNodeWeapon* getWeapon(s32 number, s32 idx); 

	//! return m_Weapons count in inventory
	virtual s32 getWeaponsCount(); 

	//! return m_Weapons sub count in inventory
	virtual s32 getWeaponsSubCount(s32 number); 

	//! remove Weapon from inventory
	virtual bool removeWeapon(IGameNodeWeapon *weapon); 

	//! add Item to inventory
	virtual bool addItem(IGameNodeItem *item); 

	//! return Item from inventory
	virtual IGameNodeItem* getItem(s32 idx); 

	//! return Item count in inventory
	virtual s32 getItemsCount(); 

	//! remove Item from inventory
	virtual bool removeItem(IGameNodeItem *item); 

	//! add Game Node to inventory
	virtual bool addObject(IGameNode *gnode); 

	//! return Game Node from inventory
	virtual IGameNode* getObject(s32 idx); 

	//! return Game Nodes count in inventory
	virtual s32 getObjectsCount(); 

	//! remove Object from inventory
	virtual bool removeObject(IGameNode *gnode); 

	//! remove all Objects from inventory
	virtual void removeAllObjects(); 

	//!
	virtual IGameNode* getOwner();
    
protected: 

	s32 findObject(IGameNode *gnode);

	core::array< core::array<IGameNodeWeapon*> > m_Weapons;
	core::list<IGameNodeItem*> m_Items;	
	core::list<IGameNode*>     m_Objects;	

	IGameNode* m_Owner;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameInventoryHPP