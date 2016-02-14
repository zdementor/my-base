//|-------------------------------------------------------------------------
//| File:        CGameNodeWeapon.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeWeapon.h"

#include <scn/IBillboardSceneNode.h>
#include <io/ILogger.h>
#include <scn/IShadow.h>
#include <dyn/IDynamicManager.h>
#include <game/IGameEventsDispatcher.h>
#include <scn/ILensFlareSceneNode.h>
#include <io/IInputDispatcher.h>
#include <io/ICursorControl.h>
#include <game/GameClassesRegistry.h>

#include "CBullet.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

enum
{
	WEAPON_IDLE = 0,
	WEAPON_SHOOT	
};

//----------------------------------------------------------------------------

//! constructor
CGameNodeWeapon::CGameNodeWeapon( 
	scn::ISceneNode* n, SGameNodeParams &params 
	) : 
IGameNodeWeapon(n, SGameNodeParams(params, EGNT_WEAPON)), 
m_FirePoint(0,0,0), m_FlashRadius(1000.0f), m_FlashTimeMs(100),
m_Firing(false), m_ReloadTime(40), m_FireNum(0), m_Owner(0),
m_FireCount(0),
m_ChoosedBulletIdx(-1), m_CurrentBullets(0), m_BindedBoneName(NONAME), 
m_FlashLightPosition(core::vector3df(0,0,0))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeWeapon");	
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeWeapon::~CGameNodeWeapon()
{
	for (u32 b=0; b<m_Bullets.size(); b++)
	{
		delete m_Bullets[b].Bullet;
	}

	m_Bullets.clear();

	core::list<CBullet*>::iterator it = m_FiredBullets.begin();

	for (; it!=m_FiredBullets.end(); ++it)
	{
		delete *it;
	}

	m_FiredBullets.clear();	
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeWeapon::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);	

	m_FirePointAbs = m_FirePoint;
	m_NodeTransformation.transformVect(m_FirePointAbs);	

	if (m_CurrentBullets&&m_CurrentBullets->Parameters.AutoFeelBulletsPerSecond>0)
	{
		f32& ammo_cnt = m_CurrentBullets->AmmoCount;
		f32  max_ammo_cnt = m_CurrentBullets->Parameters.MaxAmmoCount;
		
		ammo_cnt += delta_time_sec*m_CurrentBullets->Parameters.AutoFeelBulletsPerSecond;
		CHECK_MAX_RANGE(ammo_cnt, max_ammo_cnt);
	}
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeWeapon::OnPostDynamic(f64 delta_time_sec)
{
	core::list<CBullet*>::iterator it;

	for (it=m_FiredBullets.begin(); it!=m_FiredBullets.end();)
	{
		CBullet* bullet = *it;

		if (!bullet->fly(m_DeltaTimeSec, m_NowTimeMs/1000.0f))
		{
			delete *it;
			it = m_FiredBullets.erase(it);
		}
		else
		{
			++it;
		}		
	}
}

//----------------------------------------------------------------------------

//! calling every time before game sound playing
void CGameNodeWeapon::OnPreSound()
{
	IGameNodeAI *shooter = getOwner();

	if (shooter)
	{
		core::vector3df pos = shooter->getTransformedBBoxCenter();

		// positioning all sound effects, attached to this node
		core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

		for (; it!=m_SoundEffects.end(); ++it)
		{
			mm::ISound *sound = (*it);

			if (sound)
			{
				sound->moveTo(pos);
			}
		}
	}
	else
	{
		IGameNode::OnPreSound();
	}	
}

//----------------------------------------------------------------------------

//! setting firing flash time ms
void CGameNodeWeapon::setFlashTimeMs(u32 ms)
{
	m_FlashTimeMs = ms;
}

//----------------------------------------------------------------------------

//! getting firing flash time ms
u32 CGameNodeWeapon::getFlashTimeMs()
{
	return m_FlashTimeMs;
}

//----------------------------------------------------------------------------

//! setting firing flash radius
void CGameNodeWeapon::setFlashRadius(f32 r)
{
	m_FlashRadius = r;
}

//----------------------------------------------------------------------------

//! getting firing flash radius
f32 CGameNodeWeapon::getFlashRadius()
{
	return m_FlashRadius;
}

//----------------------------------------------------------------------------

//! setting firing flash color
void CGameNodeWeapon::setFlashColor(img::SColorf &c)
{
	m_FlashColor = c;
}

//----------------------------------------------------------------------------

//! getting firing flash color
img::SColorf CGameNodeWeapon::getFlashColor()
{
	return m_FlashColor;
}

//----------------------------------------------------------------------------

//! setting firing point
void CGameNodeWeapon::setFirePoint(core::vector3df &pos)
{
	m_FirePoint = pos;
}

//----------------------------------------------------------------------------

//! getting firing point
core::vector3df CGameNodeWeapon::getFirePoint()
{
	return m_FirePoint;
}

//----------------------------------------------------------------------------

//! binding bullet node for this weapon (only one per weapon allowed)
void CGameNodeWeapon::_bindBullet(CBullet *bullet, const SWeaponBulletParameters& params)
{
	if (!bullet)
		return;	

	scn::ISceneNode *snode = bullet->getSceneNode();

	if (!snode)
		return;

	bullet->setWeapon(NULL);
	snode->setVisible(false);			

	m_Bullets.push_back( SWeaponBulletEntry(bullet, params) );
}

//----------------------------------------------------------------------------

//! binding bullet node for this weapon (only one per weapon allowed)
bool CGameNodeWeapon::bindBullet(const c8* fname, const SWeaponBulletParameters& params)
{
	CBullet* bullet = CBulletLoader::load(fname);  
					
	if (bullet)
	{
		scn::ISceneNode* bullet_sn = bullet->getSceneNode();

		if (bullet_sn)
		{
			bullet_sn->setVisible(false);
		}

		_bindBullet(bullet, params);	
		
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! 
s32 CGameNodeWeapon::getBulletsCount()
{
	return m_Bullets.size();
}

//----------------------------------------------------------------------------

//! 
CBullet* CGameNodeWeapon::_getBullet(s32 idx)
{
	return m_Bullets[idx].Bullet;
}

//----------------------------------------------------------------------------

//! bindins fire effect (multiple allowed)
void CGameNodeWeapon::bindFireEffect(scn::ISceneNode *fireeffect, const core::vector3df &pos)
{
	if (!fireeffect || !m_SceneNode)
		return;

	m_SceneNode->addChild(fireeffect);

	fireeffect->setPosition(pos);
	fireeffect->setVisible(false);
}

//----------------------------------------------------------------------------

//!
const core::list<scn::ISceneNode*>& CGameNodeWeapon::getFireEffects() const
{
	return m_SceneNode->getChildren();
}

//----------------------------------------------------------------------------

//! start firing
bool CGameNodeWeapon::startFire(bool with_delay)
{		 
	if (!m_SceneNode || m_Firing || !m_CurrentBullets || m_CurrentBullets->AmmoCount<=0)
		return false;

	m_NextFireTime = m_NowTimeMs + (with_delay?m_Params.WaitBeforeFire:0);	

	m_Firing = true;

	return true;
}

//----------------------------------------------------------------------------

//! stop firing
void CGameNodeWeapon::stopFire()
{
	if (!m_SceneNode || !m_Firing)
		return;     
	
	const core::list<scn::ISceneNode*> &child = m_SceneNode->getChildren();

	core::list<scn::ISceneNode*>::iterator it = child.begin();

	// assume that all weapons childrens - firing effects (except shadows)
	for (; it!=child.end(); ++it)
	{
		scn::ISceneNode *snode = *it;
		if (snode)
			snode->setVisible(false);			
	}	

	m_SceneNode->setCurrentAnimation(WEAPON_IDLE);

	m_FireNum = 0;
	m_Firing = false;
}

//----------------------------------------------------------------------------

//! return true if firing now
bool CGameNodeWeapon::isFiring()
{
	return m_Firing;
}

//----------------------------------------------------------------------------

//! firing at dir if ready
bool CGameNodeWeapon::fire()
{	
	if (!m_SceneNode||!m_CurrentBullets||m_CurrentBullets->AmmoCount<=0)
		return false;

	bool ready = (m_NowTimeMs>=m_NextFireTime) ? true : false;

	dyn::IDynamicManager &dynmgr = DYNAMIC_MANAGER;

	if (!ready)
		return false;

	if (m_FireNum == 0)
	{
		m_SceneNode->setCurrentAnimation(WEAPON_SHOOT);

		const core::list<scn::ISceneNode*> &child = m_SceneNode->getChildren();

		core::list<scn::ISceneNode*>::iterator it = child.begin();

		// assume that all weapons childrens - firing effects (except shadows)
		for (; it!=child.end(); ++it)
		{
			scn::ISceneNode *snode = *it;
			if (snode)
				snode->setVisible(true);			
		}
	}

	while (m_NowTimeMs>=m_NextFireTime)
	{
		m_NextFireTime += m_ReloadTime;

		if (m_CurrentBullets->AmmoCount<=0)
			continue;

		// firing bullet here	

		if (m_CurrentBullets)
		{	
			CBullet *bullet = m_CurrentBullets->Bullet;

			SBulletParameters params = bullet->getBulletParameters();			
			
			CBullet *fired_bullet = CBulletCloner::clone(bullet, false);

			if (fired_bullet)
			{
				m_FiredBullets.push_back(fired_bullet);
			}
			
			scn::ISceneNode *snode_fired_bullet = 0;			

			if (fired_bullet && (snode_fired_bullet=fired_bullet->getSceneNode()))
			{	
				fired_bullet->setWeapon(this);
				
				f32 disp_x = params.Dispersion.X/2.0f;
				f32 disp_y = params.Dispersion.Y/2.0f;
				f32 disp_z = params.Dispersion.Z/2.0f;

				core::vector3df offset(
					core::math::RangeRandom(-disp_x, disp_x),
					core::math::RangeRandom(-disp_y, disp_y),
					core::math::RangeRandom(-disp_z, disp_z)
					);
			
				core::vector3df dir(m_AimDir);

				IGameNodeAI* shooter = m_Owner;
				if (shooter)
				{				
					const SGameNodeAIParams& aipar = shooter->getParametersAI();

					core::vector3df look = shooter->getLookDir();						

					core::vector3df dir_horiz_prj(dir.X, 0.0f, dir.Z);
					dir_horiz_prj.normalize();
					dir.normalize();

					f32 angle_vert = dir_horiz_prj.getAngleDeg(dir);

					if (core::math::Abs(angle_vert)>aipar.FireAttackVertMaxAngle)
					{
						f32 d = core::math::Abs(aipar.FireAttackVertMaxAngle/angle_vert);
						dir.Y *= d;
					}
								
					f32 angle_horiz = look.getAngleDeg(dir_horiz_prj);

					if (core::math::Abs(angle_horiz)>aipar.FireAttackHorizMaxAngle)
					{
						f32 d = core::math::Abs(aipar.FireAttackHorizMaxAngle/angle_horiz);
						dir_horiz_prj = dir.getInterpolated(look, d);	
						dir.X = dir_horiz_prj.X;
						dir.Z = dir_horiz_prj.Z;
					}					
				}

				dir.normalize();

				core::line3df ray(m_FirePointAbs, m_FirePointAbs+dir*params.Range+offset);

				fired_bullet->shoot(ray.getVector(), m_FirePointAbs, m_NowTimeMs/1000.0f);

				core::aabbox3df bb(
					m_FirePointAbs-core::vector3df(params.Range, params.Range, params.Range),
					m_FirePointAbs+core::vector3df(params.Range, params.Range, params.Range)
					);

				game::SGameEvent *event = new game::SGameEvent(game::EGET_SOUND, getOwner(), NULL, bb );
				GAME_EVENTS_DISPATCHER.registerGameEvent(event);
			}
		}	

		s32 sound_effects_count = getSoundEffectsCount();		

		m_FireNum++;

		if (sound_effects_count>0)
		{
			playSoundEffect(m_FireNum%sound_effects_count);			
		}
		
		m_FireCount++;

		m_CurrentBullets->AmmoCount--;
	}

	// cant fire, so lets stop
	if (m_CurrentBullets->Parameters.MaxAmmoCount>0 && m_CurrentBullets->AmmoCount<=0)
	{
		m_Firing=true;
		stopFire();

		return false;
	}

	return true;
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::aimAtPos( const core::vector3df &pos )
{
	m_AimDir = pos - m_FirePointAbs;
	m_AimDir.normalize();

	aimByDir(m_AimDir);
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::aimByDir( const core::vector3df &dir )
{
	aimByRay( core::line3df(m_FirePointAbs, m_FirePointAbs+dir*1000.0f) );
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::aimByRay( const core::line3df &ray )
{	
	m_AimedPos = ray.end;
	m_AimDir   = core::line3df(m_FirePointAbs, ray.end).getVector();

	IGameNodeAI *shooter = getOwner();

	if (!shooter)
		return;

	dyn::SCollisionPoints collisions;

	dyn::IDynamicManager &dynmgr = DYNAMIC_MANAGER;

	core::array<scn::ISceneNode*> exclude_node_list;
	exclude_node_list.push_back(shooter->getSceneNode());
	exclude_node_list.push_back(m_SceneNode);	

	if (ray.getLength() > 0 && (
			dynmgr.findCollisionsWithRay(ray,
				dyn::EDCF_MONOLITH_COLLID | dyn::EDCF_DYNAMIC_COLLID,
				collisions, &exclude_node_list)))
	{
		s32 idx = collisions.getNearestTo(m_FirePointAbs);
		const dyn::SCollisionPoint &collision = collisions.getPoints()[idx];

		m_AimedPos = collision.Pos;	
		m_AimDir   = core::line3df(m_FirePointAbs, m_AimedPos).getVector();
	}	
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::setAimedPos( const core::vector3df &aimed_pos )
{
	m_AimedPos = aimed_pos;
	m_AimDir = aimed_pos - m_FirePointAbs;
}

//----------------------------------------------------------------------------

const core::vector3df& CGameNodeWeapon::getAimedPos()
{
	return m_AimedPos;
}

//----------------------------------------------------------------------------

//! setting weapon parameters
void CGameNodeWeapon::setWeaponParameters(const SWeaponParameters &params)
{
	m_Params = params;

	m_ReloadTime = 1000.0f / f32(m_Params.FirePerSecond);
}

//----------------------------------------------------------------------------

//! setting weapon parameters
const SWeaponParameters& CGameNodeWeapon::getWeaponParameters() const
{
	return m_Params;
}

//----------------------------------------------------------------------------

//! getting weapon bullet parameters
const SBulletParameters& CGameNodeWeapon::getBulletParameters(s32 bullet_idx) const
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
		return *((SBulletParameters*)NULL);

	return m_Bullets[bullet_idx].Bullet->getBulletParameters();
}

//----------------------------------------------------------------------------

//! getting weapon bullet parameters
const SWeaponBulletParameters& CGameNodeWeapon::getWeaponBulletParameters(s32 bullet_idx) const
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
		return *((SWeaponBulletParameters*)NULL);

	return m_Bullets[bullet_idx].Parameters;
}

//----------------------------------------------------------------------------

//! getting owner game node of this weapon
IGameNodeAI* CGameNodeWeapon::getOwner()
{
	return m_Owner;
}

//----------------------------------------------------------------------------

//! setting owner game node for this weapon
void CGameNodeWeapon::setOwner(IGameNodeAI* owner)
{
	m_Owner = owner;
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::select()
{
	if (m_SceneNode)
	{
		m_SceneNode->setVisible(true);
	}
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::deSelect()
{
	if (m_SceneNode)
	{
		m_SceneNode->setVisible(false);
	}
}

//----------------------------------------------------------------------------

s32 CGameNodeWeapon::getFireCount()
{
	return m_FireCount;
}

//----------------------------------------------------------------------------

bool CGameNodeWeapon::chooseBullet(s32 bullet_idx)
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
	{
		m_ChoosedBulletIdx = -1;
		m_CurrentBullets = 0;

		return false;
	}

	m_ChoosedBulletIdx = bullet_idx;
	m_CurrentBullets   = &m_Bullets[m_ChoosedBulletIdx];

	return true;
}

//----------------------------------------------------------------------------

s32 CGameNodeWeapon::getChoosedBulletIndex()
{
	return m_ChoosedBulletIdx;
}

//----------------------------------------------------------------------------

s32 CGameNodeWeapon::getBulletIndexByKeyName(const c8* key_name)
{
	core::stringc key_name_str(key_name);

	s32 bullets_cnt = m_Bullets.size();
	for (s32 i=0; i<bullets_cnt; i++)
	{
		const core::stringc &bullet_key_name_str = m_Bullets[i].Parameters.KeyName;
		s32 str_size = bullet_key_name_str.size();

		c8* bullet_key_name = new c8[str_size+1];
		memcpy(bullet_key_name, bullet_key_name_str.c_str(), str_size*sizeof(c8));
		bullet_key_name[str_size]=0;
		
		bool finded=false;	

		c8* token = strtok(bullet_key_name,",");
		while(token && !finded)
		{
			if (key_name_str==token)
				finded=true;
			else
				token = strtok(NULL,",");
		}

		SAFE_DELETE_ARRAY(bullet_key_name);

		if (finded)
			return i;
	}

	return -1;
}

//----------------------------------------------------------------------------

bool CGameNodeWeapon::feelBulletAmmo(s32 bullet_idx, s32 ammo)
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
		return false;

	f32& ammo_cnt     = m_Bullets[bullet_idx].AmmoCount;
	f32  ammo_max_cnt = m_Bullets[bullet_idx].Parameters.MaxAmmoCount;
	
	ammo_cnt += ammo;
	CHECK_MAX_RANGE(ammo_cnt, ammo_max_cnt);

	return true;
}

//----------------------------------------------------------------------------

s32 CGameNodeWeapon::getBulletAmmoCount(s32 bullet_idx)
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
		return 0; 
	return m_Bullets[bullet_idx].AmmoCount;
}


//----------------------------------------------------------------------------

s32 CGameNodeWeapon::getBulletMaxAmmoCount(s32 bullet_idx)
{
	if (bullet_idx<0 || bullet_idx>=(s32)m_Bullets.size())
		return 0;    
	return m_Bullets[bullet_idx].Parameters.MaxAmmoCount;
}

//----------------------------------------------------------------------------

f32 CGameNodeWeapon::getFireRange()
{
	if (!m_CurrentBullets)
		return 0.0f;   
	return m_CurrentBullets->Bullet->getBulletParameters().Range;
}

//----------------------------------------------------------------------------

const c8* CGameNodeWeapon::getBindedBoneName()
{
	return m_BindedBoneName.c_str();
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::setBindedBoneName(const c8* binded_bone_name)
{
	m_BindedBoneName = binded_bone_name;
}

//----------------------------------------------------------------------------

void CGameNodeWeapon::setFlashLightPosition(const core::vector3df &lpos)
{
	m_FlashLightPosition = lpos;
}

//----------------------------------------------------------------------------

const core::vector3df& CGameNodeWeapon::getFlashLightPosition()
{
	return m_FlashLightPosition;
}

//----------------------------------------------------------------------------

//! Weapon registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardWeapon(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeWeapon(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardWeaponCreator;

IGameNode* Create_StandardWeapon(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardWeaponCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

class CStandardWeaponCloner : public CGameNodeBaseCloner
{
public:
	virtual IGameNode* cloneGameNode(IGameNode* game_node_to_clone, bool log_info)
	{
		IGameNode* gnode_cloned = 
			CGameNodeBaseCloner::cloneGameNode(game_node_to_clone, log_info);

		game::IGameNodeWeapon* weapon_cloned   = SAFE_CAST_TO_GAME_NODE_WEAPON(gnode_cloned);
		game::IGameNodeWeapon* weapon_to_clone = SAFE_CAST_TO_GAME_NODE_WEAPON(game_node_to_clone);

		if (weapon_cloned && weapon_to_clone)
		{
			// setting weapon parameters

			weapon_cloned->setWeaponParameters(weapon_to_clone->getWeaponParameters());

			weapon_cloned->setFlashLightPosition     (weapon_to_clone->getFlashLightPosition());
			weapon_cloned->setFlashTimeMs            (weapon_to_clone->getFlashTimeMs());
			weapon_cloned->setFlashRadius            (weapon_to_clone->getFlashRadius());
			weapon_cloned->setFlashColor             (weapon_to_clone->getFlashColor());
			weapon_cloned->setFirePoint              (weapon_to_clone->getFirePoint());
			weapon_cloned->setWeaponImageName        (weapon_to_clone->getWeaponImageName());
			weapon_cloned->setWeaponSelectedImageName(weapon_to_clone->getWeaponSelectedImageName());
			weapon_cloned->setWeaponItemKeyName      (weapon_to_clone->getWeaponItemKeyName());

			// binding fire effects

			const core::list<scn::ISceneNode*>& fire = weapon_to_clone->getFireEffects();
			core::list<scn::ISceneNode*>::iterator it = fire.begin();

			for (; it!=fire.end(); ++it)
			{
				scn::ISceneNode* sn_to_clone = (*it);

				// assume that all weapons childrens - firing effects

				if (sn_to_clone)
				{
					scn::ISceneNode* sn_cloned = SCENE_MANAGER.cloneSceneNode(sn_to_clone, log_info);

					if (sn_cloned)
					{
						weapon_cloned->bindFireEffect(sn_cloned, sn_to_clone->getPosition());
					}								
				}
			}

			s32 bullets_cnt = weapon_to_clone->getBulletsCount();

			CGameNodeWeapon* cweapon_cloned   = static_cast<CGameNodeWeapon*>(weapon_cloned);
			CGameNodeWeapon* cweapon_to_clone = static_cast<CGameNodeWeapon*>(weapon_to_clone);

			for (s32 i=0; i<bullets_cnt; i++)
			{
				CBullet* bullet_to_clone = cweapon_to_clone->_getBullet(i);				
				CBullet* bullet_cloned   = CBulletCloner::clone(bullet_to_clone, log_info);

				if (bullet_cloned)
				{
					const SWeaponBulletParameters& bullet_params = cweapon_to_clone->getWeaponBulletParameters(i);
	
					cweapon_cloned->_bindBullet(bullet_cloned, bullet_params);					
				}
			}		
			
			weapon_cloned->chooseBullet(0);

			weapon_cloned->setWeaponEnabled(weapon_to_clone->isWeaponEnabled());
		}

		return gnode_cloned;
	}
}
StandardWeaponCloner;

IGameNode* Clone__StandardWeapon(IGameNode *game_node, bool log_info)
{
	return StandardWeaponCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

class CStandardWeaponLoader : public CGameNodeBaseLoader
{
	struct SFireEntry
	{
		SFireEntry() 
			: FileName(""), Position(0,0,0) {}
		SFireEntry(core::stringc fname, core::vector3df pos) 
			: FileName(fname), Position(pos) {}

		core::stringc   FileName;
		core::vector3df Position;
	};

public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);
		_loadGameNode(node_params);

		game::IGameNodeWeapon* weapon = SAFE_CAST_TO_GAME_NODE_WEAPON(GameNode);

		if (XMLFile && GameNode && weapon)
		{
			core::vector3df fire_point(0,0,0), fire_target(0,0,1);
			core::array<SFireEntry> fire_effects;
			img::SColor fire_color;
			u32 flash_time_ms=100;
			f32 fire_radius = 1000;
			core::stringc bullet_fname, weapon_image_name, weapon_sel_image_name, weapon_item_key_name;
	
			SWeaponParameters params;
			SWeaponBulletParameters bullet_params;

			// loading game data

			XMLFile->setPos(GameDataXMLPos);

			bool game_data_finded = false;	
			
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Game") == XMLFile->getName())	
					{
						game_data_finded = true;
					}					
					else
					if (game_data_finded)
					{
						if (core::stringw("FlashColor") == XMLFile->getName())
						{
							fire_color.set(
								XMLFile->getAttributeValueAsInt(L"alpha"),
								XMLFile->getAttributeValueAsInt(L"red"),
								XMLFile->getAttributeValueAsInt(L"green"),
								XMLFile->getAttributeValueAsInt(L"blue")							
								);
						}
						else 
						if (core::stringw("FlashRadius") == XMLFile->getName())
						{
							fire_radius = XMLFile->getAttributeValueAsFloat(L"value");
						}
						else 
						if (core::stringw("FlashTime") == XMLFile->getName())
						{
							flash_time_ms = XMLFile->getAttributeValueAsInt(L"value");
						}
						else 
						if (core::stringw("FirePoint") == XMLFile->getName())
						{
							fire_point.set(
								XMLFile->getAttributeValueAsFloat(L"x"),
								XMLFile->getAttributeValueAsFloat(L"y"),
								XMLFile->getAttributeValueAsFloat(L"z")							
								);
						}										
						else 
						if (core::stringw("BindFireEffect") == XMLFile->getName())
						{
							core::stringc fire_fname = XMLFile->getAttributeValue(L"filename");

							core::vector3df fire_pos (
								XMLFile->getAttributeValueAsFloat(L"x"),
								XMLFile->getAttributeValueAsFloat(L"y"),
								XMLFile->getAttributeValueAsFloat(L"z")							
								);
							fire_effects.push_back( SFireEntry(fire_fname, fire_pos) );
						}
						else
						if (core::stringw("BindBullet") == XMLFile->getName())
						{
							bullet_fname = XMLFile->getAttributeValue(L"filename");							
							bullet_params.MaxAmmoCount = XMLFile->getAttributeValueAsFloat(L"max_ammo_count");
							bullet_params.KeyName      = XMLFile->getAttributeValue(L"key_name");
							bullet_params.AutoFeelBulletsPerSecond = XMLFile->getAttributeValueAsFloat(L"auto_feel_per_second");
						}									
						else 
						if (core::stringw("Parameters") == XMLFile->getName())
						{
							params.FirePerSecond = XMLFile->getAttributeValueAsFloat(L"fire_per_second");
							params.Mass          = XMLFile->getAttributeValueAsFloat(L"mass");
							params.SelectSpeed   = XMLFile->getAttributeValueAsFloat(L"select_speed");
							params.WaitBeforeFire= XMLFile->getAttributeValueAsFloat(L"wait_before_fire");				
						}						
						else
						if (core::stringw("WeaponImageName") == XMLFile->getName())
						{
							weapon_image_name = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("WeaponSelectedImageName") == XMLFile->getName())
						{
							weapon_sel_image_name = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("WeaponItemKeyName") == XMLFile->getName())
						{
							weapon_item_key_name = XMLFile->getAttributeValue(L"value");							
						}						
						else
						if (notGameSector(XMLFile))	
						{
							break;
						}
					} 					
					else
					{
						break;  
					}
				}
			}

			weapon->setWeaponParameters(params);
			
			weapon->setFlashColor(img::SColorf(fire_color));
			weapon->setFlashRadius(fire_radius);
			weapon->setFlashTimeMs(flash_time_ms);
			weapon->setFirePoint(fire_point);
			weapon->setWeaponImageName        (weapon_image_name.c_str());
			weapon->setWeaponSelectedImageName(weapon_sel_image_name.c_str());
			weapon->setWeaponItemKeyName      (weapon_item_key_name.c_str());

			core::stringc old_tex_path = SCENE_MANAGER.getTexturesPath();			

			for (u32 i=0; i<fire_effects.size(); i++)
			{
				if (fire_effects[i].FileName.size() >0)
				{			
					LOGGER.log("Binding fire effect for this weapon . . .");					

					core::stringc full_fire_fname = XMLFile->getFilePath();
					full_fire_fname.append(fire_effects[i].FileName.c_str());	

					scn::ISceneNode *fire = 
						SCENE_MANAGER.loadSceneNode(full_fire_fname.c_str());

					if (fire)
					{
						weapon->bindFireEffect(fire, fire_effects[i].Position);
					}									
				}
			}

			if (bullet_fname.size() > 0)
			{			
				LOGGER.log("Binding bullet for this weapon . . .");				

				core::stringc full_bullet_fname = XMLFile->getFilePath();
				full_bullet_fname.append(bullet_fname.c_str());	

				weapon->bindBullet(full_bullet_fname.c_str(), bullet_params);	
			}

			weapon->chooseBullet(0);			

			SCENE_MANAGER.setTexturesPath(old_tex_path);
		}

		_postLoadGameNode(node_params);		

		if (GameNode)
		{
			GameNode->setSoundEffectsVolume(0.375f);
		}

		return GameNode;
	}
}
StandardWeaponLoader;

IGameNode* Load___StandardWeapon(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardWeaponLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
