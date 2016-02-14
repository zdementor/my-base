//|-------------------------------------------------------------------------
//| File:        IGameNodeItem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeItemHPP
#define IGameNodeItemHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNode.h>
#include<game/IGameManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Fromation Class
class __MY_GAME_LIB_API__ IGameNodeItem : public IGameNode
{
public:

	//! constructor
	IGameNodeItem(scn::ISceneNode* n, SGameNodeParams &params)
	: IGameNode(n, params), m_ItemKeyName(""), m_ItemName(""),
	m_ItemDescr(""), m_ItemShortDescr(""), m_ItemImageName("") {}

	//! destructor
	virtual ~IGameNodeItem() 
	{
		if (m_ParentInventory)
			m_ParentInventory->removeItem(this);
	}

	//! take this item to inventory
	virtual void collectToInventory(IGameInventory& inventory) = 0;

	//! throw this item from inventory
	virtual void throwFromInventory() = 0;
	
	//! use this item
	virtual bool use() = 0;

	//! setting item name
	void setItemName(const c8* name)
	{
		m_ItemName = name;
	}

	//! setting item key name
	void setItemKeyName(const c8* name)
	{
		m_ItemKeyName = name;
	}

	//! setting item short description
	void setItemShortDescr(const c8* descr)
	{
		m_ItemShortDescr = descr;
	}

	//! setting item description
	void setItemDescr(const c8* short_descr)
	{
		m_ItemDescr = short_descr;
	}

	//! setting item image name
	void setItemImageName(const c8* image_name)
	{
		m_ItemImageName = image_name;
	}

	//! return item name
	const c8* getItemName()
	{
		return m_ItemName.c_str();
	}

	//! return item key name
	const c8* getItemKeyName()
	{
		return m_ItemKeyName.c_str();
	}

	//! return item short description
	const c8* getItemShortDescr()
	{
		return m_ItemShortDescr.c_str();
	}

	//! return item description
	const c8* getItemDescr()
	{
		return m_ItemDescr.c_str();
	}

	//! return item image name
	const c8* getItemImageName()
	{
		return m_ItemImageName.c_str();
	}

private:

	core::stringc m_ItemKeyName;
	core::stringc m_ItemName;
	core::stringc m_ItemDescr;
	core::stringc m_ItemShortDescr;
	core::stringc m_ItemImageName; 
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeItemHPP