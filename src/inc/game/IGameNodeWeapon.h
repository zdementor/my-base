//|-------------------------------------------------------------------------
//| File:        IGameNodeWeapon.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeWeaponHPP
#define IGameNodeWeaponHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNode.h>
#include <game/IGameNodeItem.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SWeaponParameters
{
	SWeaponParameters() 
		: FirePerSecond(10.0f), Mass(100.0f), SelectSpeed(100.0f), WaitBeforeFire(0.0f)
		{}

	f32 FirePerSecond;
	f32 Mass;
	f32 SelectSpeed;
	f32 WaitBeforeFire;
};

//--------------------------------------------------------------------------

struct SWeaponBulletParameters
{
	SWeaponBulletParameters() 
		: MaxAmmoCount(0), KeyName(""), AutoFeelBulletsPerSecond(0) {}

	f32 MaxAmmoCount, AutoFeelBulletsPerSecond;
	core::stringc KeyName;
};

//--------------------------------------------------------------------------

struct SBulletParameters
{
	SBulletParameters() 
	: MaxFlyTime(1000.0f), Damage(10.0f), DamageRadius(0.0f), Mass(10.0f), Range(1000.0f), 
	Dispersion(25.0f,25.0f,25.0f), DirtSize(0.0f,0.0f,0.0f), DirtColor(255,0,0,0)
	{}

	f32 MaxFlyTime;
	f32 Damage;
	f32 DamageRadius;
	f32 Mass;
	f32 Range;
	core::vector3df Dispersion;
	core::vector3df DirtSize;
	img::SColor DirtColor;
};

//----------------------------------------------------------------------------

//! Game Weapon Class
class __MY_GAME_LIB_API__ IGameNodeWeapon : public IGameNode
{
public:

	//! constructor
	IGameNodeWeapon(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNode(n, params), m_WeaponEnabled(false), 
		m_WeaponImageName(""), m_WeaponSelectedImageName(""), m_WeaponItemKeyName("") {}

	//! destructor
	virtual ~IGameNodeWeapon() 
	{
		if (m_ParentInventory)
			m_ParentInventory->removeWeapon(this);
	}

	//! setting firing flash time ms
	virtual void setFlashTimeMs(u32 ms) = 0;

	//! getting firing flash time ms
	virtual u32 getFlashTimeMs() = 0;

	//! setting firing flash radius
	virtual void setFlashRadius(f32 r) = 0;

	//! getting firing flash radius
	virtual f32 getFlashRadius() = 0;

	//! setting firing flash color
	virtual void setFlashColor(img::SColorf &c) = 0;

	//! getting firing flash color
	virtual img::SColorf getFlashColor() = 0;

	//! setting firing point
	virtual void setFirePoint(core::vector3df &pos) = 0;

	//! getting firing point
	virtual core::vector3df getFirePoint() = 0;

	//! binding bullet node for this weapon (multiple allowed)
	virtual bool bindBullet(const c8* fname, const SWeaponBulletParameters& params) = 0;

	//! 
	virtual s32 getBulletsCount() = 0;

	//! bindins fire effect (multiple allowed)
	virtual void bindFireEffect(scn::ISceneNode *fireeffect, const core::vector3df &pos) = 0;

	//!
	virtual const core::list<scn::ISceneNode*>& getFireEffects() const = 0;

	//! start firing
	virtual bool startFire(bool with_delay=true) = 0;

	//! stop firing
	virtual void stopFire() = 0;

	//! return true if firing now
	virtual bool isFiring() = 0;

	//! firing at dir
	virtual bool fire() = 0;

	virtual void aimAtPos(const core::vector3df &pos) = 0;

	virtual void aimByDir(const core::vector3df &dir) = 0;

	virtual void aimByRay(const core::line3df &ray) = 0;

	virtual void setAimedPos(const core::vector3df &aimed_pos) = 0;

	virtual const core::vector3df& getAimedPos() = 0;
	
	//! setting weapon parameters
	virtual void setWeaponParameters(const SWeaponParameters &params) = 0;

	//! getting weapon parameters
	virtual const SWeaponParameters& getWeaponParameters() const = 0;

	//! getting bullet parameters
	virtual const SBulletParameters& getBulletParameters(s32 bullet_idx) const = 0;

	//! getting weapon bullet parameters
	virtual const SWeaponBulletParameters& getWeaponBulletParameters(s32 bullet_idx) const = 0;

	//! getting owner game node of this weapon
	virtual IGameNodeAI* getOwner() = 0;

	//! setting owner game node for this weapon
	virtual void setOwner(IGameNodeAI* owner) = 0;	

	virtual void select() = 0;

	virtual void deSelect() = 0;

	virtual s32 getFireCount() = 0;

	virtual bool chooseBullet(s32 bullet_idx) = 0;

	virtual s32 getChoosedBulletIndex() = 0;

	virtual s32 getBulletIndexByKeyName(const c8* key_name) = 0;

	virtual bool feelBulletAmmo(s32 bullet_idx, s32 ammo) = 0;

	virtual s32 getBulletAmmoCount(s32 bullet_idx) = 0;

	virtual s32 getBulletMaxAmmoCount(s32 bullet_idx) = 0;

	virtual f32 getFireRange() = 0;

	virtual const c8* getBindedBoneName() = 0;

	virtual void setBindedBoneName(const c8* binded_bone_name) = 0;

	virtual void setFlashLightPosition(const core::vector3df &lpos) = 0;

	virtual const core::vector3df& getFlashLightPosition() = 0;

	virtual void setWeaponEnabled(bool enabled)
	{
		m_WeaponEnabled=enabled;
	}

	virtual bool isWeaponEnabled()
	{
		return m_WeaponEnabled;
	}

	//! setting weapon image name
	void setWeaponImageName(const c8* image_name)
	{
		m_WeaponImageName = image_name;
	}

	//! return weapon image name
	const c8* getWeaponImageName()
	{
		return m_WeaponImageName.c_str();
	}

	//! setting weapon selected image name
	void setWeaponSelectedImageName(const c8* image_name)
	{
		m_WeaponSelectedImageName = image_name;
	}

	//! return weapon selected image name
	const c8* getWeaponSelectedImageName()
	{
		return m_WeaponSelectedImageName.c_str();
	}

	//! setting weapon item key name
	void setWeaponItemKeyName(const c8* item_key_name)
	{
		m_WeaponItemKeyName = item_key_name;
	}

	//! return weapon item key name
	const c8* getWeaponItemKeyName()
	{
		return m_WeaponItemKeyName.c_str();
	}

private:

	bool m_WeaponEnabled;

	core::stringc m_WeaponImageName, m_WeaponSelectedImageName, m_WeaponItemKeyName; 
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeWeaponHPP