//|-------------------------------------------------------------------------
//| File:        CGameNodeWeapon.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeWeaponHPP
#define CGameNodeWeaponHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeWeapon.h>
#include <game/IGameNodeAI.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

class CBullet;

//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodeWeapon : public IGameNodeWeapon
{
	friend class CStandardWeaponCloner;

public:

	//! constructor
	CGameNodeWeapon(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeWeapon();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

	//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPostDynamic(f64 delta_time_sec);

	//! calling every time before game sound playing
    virtual void OnPreSound();

	//! setting firing flash time ms
	virtual void setFlashTimeMs(u32 ms);

	//! getting firing flash time ms
	virtual u32 getFlashTimeMs();

	//! setting firing flash radius
	virtual void setFlashRadius(f32 r);

	//! getting firing flash radius
	virtual f32 getFlashRadius();

	//! setting firing flash color
	virtual void setFlashColor(img::SColorf &c);

	//! getting firing flash color
	virtual img::SColorf getFlashColor();

	//! setting firing point
	virtual void setFirePoint(core::vector3df &pos);

	//! getting firing point
	virtual core::vector3df getFirePoint();

	//! binding bullet node for this weapon (multiple allowed)
	virtual bool bindBullet(const c8* fname, const SWeaponBulletParameters& params);

	//! 
	virtual s32 getBulletsCount();

	//! bindins fire effect (multiple allowed)
	virtual void bindFireEffect(scn::ISceneNode *fireeffect, const core::vector3df &pos);

	//!
	virtual const core::list<scn::ISceneNode*>& getFireEffects() const;

	//! start firing
	virtual bool startFire(bool with_delay=true);

	//! stop firing
	virtual void stopFire();

	//! return true if firing now
	virtual bool isFiring();

	//! firing at dir if ready
	virtual bool fire();

	virtual void aimAtPos( const core::vector3df &pos );

	virtual void aimByDir( const core::vector3df &dir );

	virtual void aimByRay( const core::line3df &ray );

	virtual void setAimedPos( const core::vector3df &aimed_pos );

	virtual const core::vector3df& getAimedPos();

	//! setting weapon parameters
	virtual void setWeaponParameters(const SWeaponParameters &params);

	//! getting weapon parameters
	virtual const SWeaponParameters& getWeaponParameters() const;

	//! getting bullet parameters
	virtual const SBulletParameters& getBulletParameters(s32 bullet_idx) const;

	//! getting weapon bullet parameters
	virtual const SWeaponBulletParameters& getWeaponBulletParameters(s32 bullet_idx) const;

	//! getting owner game node of this weapon
	virtual IGameNodeAI* getOwner();

	//! setting owner game node for this weapon
	virtual void setOwner(IGameNodeAI* owner);

	virtual void select();

	virtual void deSelect();

	virtual s32 getFireCount();

	virtual bool chooseBullet(s32 billet_idx);

	virtual s32 getChoosedBulletIndex();

	virtual s32 getBulletIndexByKeyName(const c8* key_name);

	virtual bool feelBulletAmmo(s32 bullet_idx, s32 ammo);

	virtual s32 getBulletAmmoCount(s32 bullet_idx);

	virtual s32 getBulletMaxAmmoCount(s32 bullet_idx);

	virtual f32 getFireRange();

	virtual const c8* getBindedBoneName();

	virtual void setBindedBoneName(const c8* binded_bone_name);

	virtual bool isGaming()
	{	
		if (m_FiredBullets.size()!=0)
		{
			return true;
		}

		return m_Owner ? m_Owner->isGaming() : IGameNode::isGaming();	
	}

	virtual void setFlashLightPosition(const core::vector3df &lpos);

	virtual const core::vector3df& getFlashLightPosition();

protected:

	//! setting bullet node for this weapon (multiple allowed)
	void _bindBullet(CBullet *bullet, const SWeaponBulletParameters& params);

	//! 
	CBullet* _getBullet(s32 idx);

private:

	s32 m_FireCount;

	IGameNodeAI* m_Owner;

	core::vector3df m_FirePoint, m_FirePointAbs;

	SWeaponParameters m_Params;

	img::SColorf m_FlashColor;

	f32 m_FlashRadius;

	bool m_Firing;

	s32 m_NextFireTime, m_ReloadTime;

	struct SWeaponBulletEntry
	{
		SWeaponBulletEntry() : Bullet(0), AmmoCount(0) {}
		SWeaponBulletEntry(CBullet* bullet, const SWeaponBulletParameters& params)	
			: Bullet(bullet), AmmoCount(0), Parameters(params) {}

		CBullet* Bullet;
		f32 AmmoCount; 
		SWeaponBulletParameters Parameters;
	};

	core::array<SWeaponBulletEntry> m_Bullets;

	SWeaponBulletEntry* m_CurrentBullets;

	s32 m_FireNum;

	core::vector3df m_AimedPos, m_AimDir;

	f32 m_MinShootAngle, m_MaxShootAngle;

	s32 m_ChoosedBulletIdx;

	core::stringc m_BindedBoneName;

	core::list<CBullet*> m_FiredBullets;

	core::vector3df m_FlashLightPosition;

	u32 m_FlashTimeMs;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeWeaponHPP