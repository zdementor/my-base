//|-------------------------------------------------------------------------
//| File:        CGameInventory.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameInventory.h"

#include <game/IGameNodeWeapon.h>
#include <game/IGameNodeItem.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameInventory::CGameInventory(IGameNode *owner)
: m_Owner(owner)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CGameInventory");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameInventory::~CGameInventory()
{
	removeAllObjects();
}

//----------------------------------------------------------------------------

//! add Weapon to inventory
bool CGameInventory::addWeapon(IGameNodeWeapon *weapon, s32 number)
{
	if (addObject(weapon))
	{
		if (number>=(s32)m_Weapons.size())
			m_Weapons.set_used(number+1);		

		m_Weapons[number].push_back(weapon);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! return Weapon from inventory
IGameNodeWeapon* CGameInventory::getWeapon(s32 number, s32 idx)
{
	if (number>=0 && number<(s32)m_Weapons.size())
	{
		if (idx>=0 && idx<(s32)m_Weapons[number].size())
		{
			return m_Weapons[number][idx];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------

//! return m_Weapons count in inventory
s32 CGameInventory::getWeaponsCount()
{
	return m_Weapons.size();
}

//----------------------------------------------------------------------------

//! return m_Weapons sub count in inventory
s32 CGameInventory::getWeaponsSubCount(s32 number)
{
	if (number>=0 && number<(s32)m_Weapons.size())
	{
		return m_Weapons[number].size();
	}

	return 0;
}

//----------------------------------------------------------------------------

//! remove Weapon from inventory
bool CGameInventory::removeWeapon(IGameNodeWeapon *weapon)
{
	return removeObject(weapon);
}

//----------------------------------------------------------------------------

//! add Item to inventory
bool CGameInventory::addItem(IGameNodeItem *item)
{
	if (addObject(item))
	{
		m_Items.push_back(item);

		// enabling weapons, by key items
		
		s32 weapon_cnt = m_Weapons.size();
		for (s32 w=0; w<weapon_cnt; w++)
		{
			s32 weapon_subcnt = m_Weapons[w].size();
			for (s32 sw=0; sw<weapon_subcnt; sw++)
			{
				game::IGameNodeWeapon *weapon = m_Weapons[w][sw];
				if (core::stringc(weapon->getWeaponItemKeyName())==item->getItemKeyName())
				{
					weapon->setWeaponEnabled(true);

					// to exit cycle
					w=weapon_cnt;
					break;
				}
			}
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! return Item from inventory
IGameNodeItem* CGameInventory::getItem(s32 idx)
{	
	if (idx>=0 && idx<(s32)m_Items.size())
	{
		return *(m_Items.begin()+idx);
	}

	return NULL;
}

//----------------------------------------------------------------------------

//! return Item count in inventory
s32 CGameInventory::getItemsCount()
{
	return m_Items.size();
}

//----------------------------------------------------------------------------

//! remove Item from inventory
bool CGameInventory::removeItem(IGameNodeItem *item)
{
	core::stringc item_key_name = item->getItemKeyName();
	s32 key_tem_cnt = 0;
	core::list<IGameNodeItem*>::iterator it = m_Items.begin();
	for (; it!=m_Items.end(); ++it)
	{
		IGameNodeItem* item = *it;
		if (item_key_name==item->getItemKeyName())
			key_tem_cnt++;
	}

	if (key_tem_cnt==1)
	{
		s32 weapon_cnt = m_Weapons.size();
		for (s32 w=0; w<weapon_cnt; w++)
		{
			s32 weapon_subcnt = m_Weapons[w].size();
			for (s32 sw=0; sw<weapon_subcnt; sw++)
			{
				game::IGameNodeWeapon *weapon = m_Weapons[w][sw];
				if (item_key_name == core::stringc(weapon->getWeaponItemKeyName()))
					weapon->setWeaponEnabled(false);
			}
		}
	}

	return removeObject(item);
}

//----------------------------------------------------------------------------

//! add Game Node to inventory
bool CGameInventory::addObject(IGameNode *gnode)
{
	if (!gnode)
		return false;

	s32 idx = findObject(gnode);

	if (idx==-1)
	{
		if (gnode->m_ParentInventory)
			gnode->m_ParentInventory->removeObject(gnode);
		gnode->m_ParentInventory = this;

		m_Objects.push_back(gnode);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! return Game Node from inventory
IGameNode* CGameInventory::getObject(s32 idx)
{
	if (idx>=0 && idx<(s32)m_Objects.size())
	{
		return *(m_Objects.begin()+idx);
	}

	return NULL;
}

//----------------------------------------------------------------------------

//! return Game Nodes count in inventory
s32 CGameInventory::getObjectsCount()
{
	return m_Objects.size();
}

//----------------------------------------------------------------------------

s32 CGameInventory::findObject(IGameNode *gnode)
{
	s32 idx = -1;

	if (!gnode)
		return false;

	core::list<IGameNode*>::iterator it = m_Objects.begin();

	for (s32 i=0; it!=m_Objects.end(); ++it, i++)
	{
		IGameNode *gn = *it;

		if (gn==gnode)
		{
			idx = i;

			break;
		}
	}

	return idx;
}

//----------------------------------------------------------------------------

//! remove Object from inventory
bool CGameInventory::removeObject(IGameNode *gnode)
{
	if (!gnode)
		return false;

	if (gnode->getGameNodeType()==EGNT_WEAPON)
	{
		s32 weapon_cnt = m_Weapons.size();
		for (s32 i=0; i<weapon_cnt; ++i)
		{
			s32 weapon_subcnt = m_Weapons[i].size();
			for (s32 j=0; j<weapon_subcnt; ++j)
			{
				IGameNode *gn = m_Weapons[i][j];
				if (gn == gnode)
				{
					m_Weapons[i].erase(j);
					break;
				}
			}
		}
	}
	else
	if (gnode->getGameNodeType()==EGNT_ITEM)
	{
		core::list<IGameNodeItem*>::iterator it = m_Items.begin();
		for (; it!=m_Items.end(); ++it)
		{
			IGameNode *gn = *it;
			if (gn == gnode)
			{
				m_Items.erase(it);
				break;
			}
		}
	}

	core::list<IGameNode*>::iterator itobj = m_Objects.begin();
	for (; itobj!=m_Objects.end(); ++itobj)
	{
		IGameNode *gn = *itobj;
		if (gn == gnode)
		{
			gn->m_ParentInventory = NULL;
			m_Objects.erase(itobj);

			return true;
		}
	}

    return false;
}

//----------------------------------------------------------------------------

//! remove all Objects from inventory
void CGameInventory::removeAllObjects()
{
	core::list<IGameNode*>::iterator it = m_Objects.begin();

	for (; it!=m_Objects.end(); ++it)
	{
		IGameNode *gn = *it;

		if (gn)
		{
			gn->m_ParentInventory = NULL;
		}
	}

	m_Weapons.clear();
	m_Items.clear();
	m_Objects.clear();	
}

//----------------------------------------------------------------------------

//!
IGameNode* CGameInventory::getOwner()
{
	return m_Owner;
}
    
//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
