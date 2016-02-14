//|-------------------------------------------------------------------------
//| File:        CGameNodeMainPlayer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeMainPlayerHPP
#define CGameNodeMainPlayerHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeMainPlayer.h>
#include <game/IGameNodeWeapon.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodeMainPlayer : public IGameNodeMainPlayer
{

public:

	//! constructor
	CGameNodeMainPlayer(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeMainPlayer();

	//! binding new weapon to the bone and index
    virtual bool bindWeapon(s32 number, IGameNodeWeapon* weapon, const c8 *target_bone);

	//! selecting to use weapon, by its index
    virtual bool selectWeapon(s32 number, bool select=true);

	//! selecting to use previous weapon
    virtual bool selectPrevWeapon();

	//! selecting to use next weapon
    virtual bool selectNextWeapon();

	//! fire weapon
	virtual bool fireWeapon(bool fire=true);

	//! return true if firing weapon, or false
	virtual bool isFiring();

protected:

private:

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeMainPlayerHPP