//|-------------------------------------------------------------------------
//| File:        CGameNodeItem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeItemHPP
#define CGameNodeItemHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeItem.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Item Class
class CGameNodeItem : public IGameNodeItem
{

public:

	//! constructor
	CGameNodeItem(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeItem();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPreDynamic(f64 delta_time_sec);

    //! calling every time after doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPostDynamic(f64 delta_time_sec);	
		
	//! take this item to inventory
	virtual void collectToInventory(IGameInventory& inventory);
	
	//! throw this item from inventory
	virtual void throwFromInventory();
		
	//! use this item
	virtual bool use();
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeItemHPP