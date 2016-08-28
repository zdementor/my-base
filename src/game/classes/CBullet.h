//|-------------------------------------------------------------------------
//| File:        CBullet.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <scn/IParticleSystemSceneNode.h>
#include <mm/ISoundDriver.h>
#include <mm/ISound.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {

//----------------------------------------------------------------------------	

struct SSoundEffectParameters
{
private:

	mm::ISound* Sound;

public:

	SSoundEffectParameters() : 
		Sound(0) {}

	SSoundEffectParameters(mm::ISound* snd) : Sound(snd) 
    {
		if (snd)
		{
			Sound = snd;
			SAFE_GRAB(Sound);
		}
	}

    SSoundEffectParameters(const SSoundEffectParameters& p) : Sound(0)  
    {
		if (p.Sound)
		{
			Sound = p.Sound;
			SAFE_GRAB(Sound);
		}

		set(p);
	}

	virtual ~SSoundEffectParameters()
	{
		SAFE_DROP(Sound);
	}

	void set(const SSoundEffectParameters& p)
	{
		
	}

    //! operator =
    inline SSoundEffectParameters& operator=(const SSoundEffectParameters& p)
    {
		if (p.Sound)
		{
			SAFE_DROP(Sound);
			Sound  = p.Sound;
			SAFE_GRAB(Sound);
		}

		set(p);

		return *this;
    } 

	mm::ISound* getSound()
	{
		return Sound;
	}
};

//----------------------------------------------------------------------------	

struct SBoomEffectParameters
{
private:

	scn::ISceneNode* Node;

public:

	SBoomEffectParameters() : 
		Node(0), DelayAfterBoom(0), LifeTime(1000), Offset(core::vector3df(0,0,0)),
		NormalAligned(false), Air(true), Ground(true), Flesh(true), Wall(true) {}

	SBoomEffectParameters(scn::ISceneNode* node) : Node(0) 
    {
		if (node)
		{
			Node = node;
			SAFE_GRAB(Node);
		}
	}

    SBoomEffectParameters(const SBoomEffectParameters& p) : Node(0)  
    {
		if (p.Node)
		{
			Node = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);
	}

	virtual ~SBoomEffectParameters()
	{
		SAFE_DROP(Node);
	}

	void set(const SBoomEffectParameters& p)
	{
		DelayAfterBoom = p.DelayAfterBoom;
		LifeTime       = p.LifeTime;
		NormalAligned  = p.NormalAligned;
		Air            = p.Air;
		Ground         = p.Ground;
		Wall           = p.Wall;
		Flesh          = p.Flesh;
		Offset         = p.Offset;
	}

    //! operator =
    inline SBoomEffectParameters& operator=(const SBoomEffectParameters& p)
    {
		if (p.Node)
		{
			SAFE_DROP(Node);
			Node  = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);

		return *this;
    } 

	scn::ISceneNode* getSceneNode()
	{
		return Node;
	}
			
	s32 DelayAfterBoom;
	s32 LifeTime;
	bool NormalAligned;
	bool Air, Wall, Ground, Flesh;
	core::vector3df Offset;
};

//----------------------------------------------------------------------------

struct SFlyEffectParameters
{

private:

	scn::ISceneNode* Node;

public:

	SFlyEffectParameters() 
		: Node(0), Offset(core::vector3df(0,0,0)), AxisAligned(false), LifeTimeAfterBoom(0) {}

	SFlyEffectParameters(scn::ISceneNode* node) : Node(0) 
    {
		if (node)
		{
			Node = node;
			SAFE_GRAB(Node);
		}
	}

    SFlyEffectParameters(const SFlyEffectParameters& p) : Node(0)  
    {
		if (p.Node)
		{
			Node = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);
	}

	virtual ~SFlyEffectParameters()
	{
		SAFE_DROP(Node);
	}

	void set(const SFlyEffectParameters& p)
	{
		AxisAligned = p.AxisAligned;
		Offset         = p.Offset;
		LifeTimeAfterBoom= p.LifeTimeAfterBoom;
	}

    //! operator =
    inline SFlyEffectParameters& operator=(const SFlyEffectParameters& p)
    {
		if (p.Node)
		{
			SAFE_DROP(Node);
			Node  = p.Node;
			SAFE_GRAB(Node);
		}

		set(p);

		return *this;
    } 

	scn::ISceneNode* getSceneNode()
	{
		return Node;
	}
			
	bool AxisAligned;
	core::vector3df Offset;
	s32 LifeTimeAfterBoom;
};

//----------------------------------------------------------------------------

class CBullet
{

public:

	//! constructor
	CBullet(scn::ISceneNode* n);

	//! destructor
	virtual ~CBullet();

	//! setting bullet parameters
	void setBulletParameters(const SBulletParameters &params);

	//! setting bullet parameters
	const SBulletParameters& getBulletParameters() const;

	//! shoot bullet
	void shoot(
		const core::vector3df& dir, const core::vector3df& shoot_pos, 
		f64 curr_time_sec
		);

	//! binding bullet trace (can be just one)
	void bindTrace(scn::IBillboardSceneNode* trace);

	//! getting trace scene node
	virtual scn::IBillboardSceneNode* getTrace();

	//! binding bullet boom effect (multiple allowed)
	void bindBoomEffect(SBoomEffectParameters &effect);

	//! getting bullet boom effects
	const core::array<SBoomEffectParameters>& getBoomEffects();

	//! binding bullet fly effect (multiple allowed)
	void bindFlyEffect(SFlyEffectParameters &effect);

	//! getting bullet fly effects
	const core::array<SFlyEffectParameters>& getFlyEffects();

	//! binding bullet sound boom effect (multiple allowed)
	void bindSoundBoomEffect(SSoundEffectParameters& effect);

	//! getting bullet sound boom effects
	const core::array<SSoundEffectParameters>& getSoundBoomEffects();

	//! binding bullet sound fly effect (multiple allowed)
	void bindSoundFlyEffect(SSoundEffectParameters& effect);

	//! getting bullet sound fly effects
	const core::array<SSoundEffectParameters>& getSoundFlyEffects();

	//! 
	bool fly(f64 delta_time_sec, f64 curr_time_sec);

	//! getting weapon from which this bullet was shooted
	IGameNodeWeapon* getWeapon();

	//! setting weapon from which this bullet was shooted
	void setWeapon(IGameNodeWeapon* weapon);

	//!
	scn::ISceneNode* getSceneNode();

	//!
	void free();

	//! setting sound effects volume
	void setSoundEffectsVolume(f32 volume)
	{
		CHECK_RANGE(volume, 0.0f, 1.0f);

		for (u32 i=0; i<SoundBoomEffects.size(); i++)
			SoundBoomEffects[i].getSound()->setVolume(volume);
		for (u32 i=0; i<SoundFlyEffects.size(); i++)
			SoundFlyEffects[i].getSound()->setVolume(volume);
	}

	//! 
	void moveSoundEffectsTo(const core::vector3df &pos)
	{
		for (u32 i=0; i<SoundBoomEffects.size(); i++)
			SoundBoomEffects[i].getSound()->moveTo(pos);
		for (u32 i=0; i<SoundFlyEffects.size(); i++)
			SoundFlyEffects[i].getSound()->moveTo(pos);
	}

private:

	IGameNodeWeapon* Weapon;

	SBulletParameters BulletParameters;

	bool Shooted, Boomed, Boom;

	core::vector3df ShootedPos;
	core::line3df FlyingMaxRay;
	f32 FlyingMaxDist;

	scn::IBillboardSceneNode* Trace;

	core::array<SBoomEffectParameters> BoomEffects;
	core::array<SFlyEffectParameters>  FlyEffects;

	core::array<SSoundEffectParameters> SoundBoomEffects;
	core::array<SSoundEffectParameters> SoundFlyEffects;

	core::vector3df ShootedDir;

	bool FirstAxisAlongOffset;

	s32 TraceInitialColorRed;
	s32 TraceInitialColorGreen;
	s32 TraceInitialColorBlue;
	s32 TraceInitialColorAlpha;

	f32 AxisAlongOffset;

	core::vector3df CurrentPos;

	f64 m_BoomTimeSec;

	scn::ISceneNode* m_SceneNode;

	bool m_Freeed;

	dyn::SCollisionPoints m_FlyEndCollisions;
};

//----------------------------------------------------------------------------

//! constructor
CBullet::CBullet(
	scn::ISceneNode* n
	) :
m_SceneNode(n), Shooted(false), Trace(0), FirstAxisAlongOffset(true),
ShootedPos(0.0f, 0.0f, 0.0f), Weapon(0), FlyingMaxDist(0.0f), AxisAlongOffset(0.0f),
Boomed(false), Boom(false), m_BoomTimeSec(0), m_Freeed(false)
{
	if (m_SceneNode)
	{
		m_SceneNode->setVisible(false);
		m_SceneNode->setCullingFilter( scn::ECT_NO_CULL );
		SAFE_GRAB(m_SceneNode);
	}
}

//----------------------------------------------------------------------------

//! destructor
CBullet::~CBullet()
{	
	free();
}

//----------------------------------------------------------------------------

void CBullet::free()
{
	if (m_Freeed)
		return;

	scn::ISceneManager &smgr   = SCENE_MANAGER;

	if (m_SceneNode)
	{
		smgr.addToDeletionQueue(m_SceneNode);
		m_SceneNode->drop();
	}
	if (Trace)
	{
		smgr.addToDeletionQueue(Trace);
		Trace->drop();
	}

	for (u32 fe=0; fe<FlyEffects.size(); fe++)
	{
		const SFlyEffectParameters &fep = FlyEffects[fe];
		scn::ISceneNode *fly_snode = (((SFlyEffectParameters&)fep).getSceneNode());
		if (fly_snode)	
			smgr.addToDeletionQueue(fly_snode, fep.LifeTimeAfterBoom);
	}
	FlyEffects.clear();

	for (u32 be=0; be<BoomEffects.size(); be++)
	{
		const SBoomEffectParameters &bep = BoomEffects[be];
		scn::ISceneNode *boom_snode = (((SBoomEffectParameters&)bep).getSceneNode());
		if (boom_snode)	
			smgr.addToDeletionQueue(boom_snode, bep.DelayAfterBoom+bep.LifeTime);
	}
	BoomEffects.clear();

	SoundBoomEffects.clear();
	SoundFlyEffects.clear();

 	m_Freeed = true;
}

//----------------------------------------------------------------------------

//! 
bool CBullet::fly(f64 delta_time_sec, f64 curr_time_sec)
{
	if (Boom || curr_time_sec>=m_BoomTimeSec)
	{
		return false;
	}

	IGameNodeAI *shooter = Weapon ? Weapon->getOwner() : 0;

	if (shooter && Shooted)
	{
		f32 speed = 1000.0f * BulletParameters.Range / BulletParameters.MaxFlyTime;

		core::vector3df new_pos = 
			CurrentPos + ShootedDir * speed * delta_time_sec;

		f32 dist = core::vector3df(new_pos-ShootedPos).getLength();

		if (FirstAxisAlongOffset && dist>core::math::Abs(AxisAlongOffset))
		{
			new_pos += ShootedDir*AxisAlongOffset;

			FirstAxisAlongOffset = false;
		}

		core::line3df ray(CurrentPos, new_pos);
		core::line3df fly_ray(ShootedPos, new_pos);

		dyn::SCollisionPoints collisions;

		scn::ISceneManager   &smgr   = SCENE_MANAGER;
		dyn::IDynamicManager &dynmgr = DYNAMIC_MANAGER;		
		game::IGameManager   &gmgr   = GAME_MANAGER;	

		bool in_the_end_of_trajectory = fly_ray.getLength()>=FlyingMaxDist;

		static E_GAME_NODE_TYPE test_types[] = 
		{ 
			EGNT_MAIN_PLAYER, EGNT_PERSON, EGNT_DECORATION, EGNT_DOOR, EGNT_PLATFORM
		};

		static const s32 test_count = sizeof(test_types) / sizeof(E_GAME_NODE_TYPE);

		bool collided = false;

		for (s32 t=0; !collided && t<test_count; t++)
		{
			const core::list<IGameNode*> &gnodes = gmgr.getGameNodesByType(test_types[t]);

			core::list<IGameNode*>::iterator it = gnodes.begin();
			for (; it!=gnodes.end(); ++it)
			{
				IGameNode *game_node = *it;

				if (game_node == shooter || !game_node->isGaming())
					continue;

				core::vector3df sorigin       = game_node->getTransformedBBoxCenter();
				f32             sradius       = game_node->getNodeSphereRadius();

				// first test ray vs sphere
				if (ray.intersectsWithSphere(sorigin, sradius))
				{
					// then fire true raytrace test
					collided = dynmgr.findCollisionsWithRay(
						game_node->getDynamicObject(), ray, collisions);	
					if (collided)
						break;
				}
			}
		}

		if (in_the_end_of_trajectory || collided)
		{
			scn::ISceneNode *target_snode = 0;
			game::IGameNode *target_gnode = 0;
			game::IGameNodeAI *target_gnode_ai = 0;

			core::vector3df boom_normal;

			if (collided)
			{	
				const dyn::SCollisionPoint &collision =
					collisions.getPoints()[collisions.getNearestTo(CurrentPos)];

				boom_normal = collision.Normal;

				if (collision.DynObj)
				{
					target_snode = collision.DynObj->getSceneNode();
					if (target_snode)
					{
						target_gnode = target_snode->getGameNode();
						target_gnode_ai = SAFE_CAST_TO_GAME_NODE_AI(target_gnode);
						if (!target_gnode_ai || shooter->isWarWith(target_gnode_ai))
							new_pos = collision.Pos; 	
					}			
				}
			}
			else
			{
				s32 idx = m_FlyEndCollisions.getNearestTo(CurrentPos);
				if (idx != -1)
				{
					const dyn::SCollisionPoint &collision =
						m_FlyEndCollisions.getPoints()[idx];
					boom_normal = collision.Normal;
				}
				else
					boom_normal.set(0.f, 1.f, 0.f);
				new_pos = FlyingMaxRay.end;
			}	

			// checking out bullets state

			f32 height = PATH_FINDER_MANAGER.getHeightOverGround(new_pos);

			bool walkable   = PATH_FINDER_MANAGER.isWalkablePosition(new_pos);			
			bool flesh      = target_gnode_ai!=NULL;
			bool in_the_air = !flesh&&walkable&&height>(BulletParameters.DamageRadius+10);
			bool ground     = walkable&&(height<=BulletParameters.DamageRadius+10);
			bool wall       = !flesh&&!ground;

			// creating victims list   		

			core::array<IGameNodeAI*> victims_ai;
			core::array<IGameNode*>   victims;

			core::vector3df new_pos_offs = new_pos;
			
			if (target_gnode_ai)
			{
				new_pos_offs += ShootedDir*10.0f;
				victims_ai.push_back(target_gnode_ai);
			}
			else
			{
				new_pos_offs -= ShootedDir*10.0f;

				if (target_gnode)
					victims.push_back(target_gnode);
			}

			for (s32 t=0; t<test_count; t++)
			{
				const core::list<IGameNode*>& gnodes = gmgr.getGameNodesByType(test_types[t]);

				core::list<IGameNode*>::iterator it = gnodes.begin();

				for (; it!=gnodes.end(); ++it)
				{
					IGameNode *game_node = *it;

					if (game_node==target_gnode || !game_node->isGaming())
						continue;

					const core::vector3df& node_center = game_node->getTransformedBBoxCenter();
					f32                    node_radius = game_node->getNodeSphereRadius();
					core::vector3df&       boom_vec	   = node_center-new_pos; 

					if (BulletParameters.DamageRadius &&
						boom_vec.getLength()<=(node_radius+BulletParameters.DamageRadius)
						)
					{			
						IGameNodeAI *game_node_ai = SAFE_CAST_TO_GAME_NODE_AI(game_node); 

						if (game_node_ai)
							victims_ai.push_back(game_node_ai);							
						else
							victims.push_back(game_node);
					}
				}
			}

			// showing boom effects

			for (u32 be=0; be<BoomEffects.size(); be++)
			{
				SBoomEffectParameters &bep = BoomEffects[be];
				scn::ISceneNode *boom_snode = (((SBoomEffectParameters&)bep).getSceneNode());

				if (boom_snode && ( 
						(flesh&&bep.Flesh) || 
						(in_the_air&&bep.Air) || 
						(ground&&bep.Ground) ||
						(wall&&bep.Wall)
						)
					)
				{
					if (bep.NormalAligned)
					{
						scn::IParticleSystemSceneNode *ps = 
							SAFE_CAST_TO_PARTICLE_SYSTEM_SCENE_NODE(boom_snode);

						if (ps)
						{
							for (s32 e=0; e<ps->getEmittersCount(); e++)
							{
								scn::IParticleEmitter *emit = ps->getEmitter(e);
								core::vector3df ps_dir = boom_normal * emit->getDirection().getLength();
								emit->setDirection(ps_dir);						
							}	
						}
						else
						{
							scn::IBillboardSceneNode *bill = 
								SAFE_CAST_TO_BILLBOARD_SCENE_NODE(boom_snode);

							if (bill)
							{
								scn::SBillboardParams bill_params = bill->getBillboardParameters();
								
								bill_params.AlignAxis = boom_normal;
								if (boom_normal!=core::vector3df(0,1,0))
									bill_params.UpAxis.set(0,1,0);
								else
									bill_params.UpAxis.set(1,0,0); 
								bill_params.UpAxis = bill_params.AlignAxis.getCrossProduct(bill_params.UpAxis);
								
								bill_params.AxisAligmentType = scn::EAAT_NORMAL_AXIS;

								bill->setBillboardParameters(bill_params); 
							}
						}
					}
					s32 acnt = boom_snode->getAnimationsCount();
					s32 anim = core::math::RangeRandom(0, acnt);
					boom_snode->setCurrentAnimation(anim);

					boom_snode->setPosition(new_pos_offs + bep.Offset);     
					smgr.addToVisibilityChangeQueue(boom_snode, true, bep.DelayAfterBoom); 
				}
				else
				{
					bep.DelayAfterBoom=0;
					bep.LifeTime=0;
				}
			}	

			// hiting victims

			IGameNodeAI *shooter = Weapon ? Weapon->getOwner() : 0;

			s32 victims_ai_size = victims_ai.size();
			s32 victims_size    = victims.size();

			if (!shooter || (!victims_ai_size&&!victims_size))
			{
				Shooted = false;
				Boom = true;
			}
			else
			{
				const SGameNodeAIParams& aipar = shooter->getParametersAI();
				
				game::IGameManager &gmgr = GAME_MANAGER;
				scn::ISceneManager &smgr = SCENE_MANAGER;

				s32 v=0;
				
				for (v=0; v<victims_size; v++)
				{	
					IGameNode* victim = victims[v];

					const core::vector3df& node_center = victim->getTransformedBBoxCenter();
					core::vector3df&       boom_vec	   = node_center-new_pos;  

					dyn::IDynamicObject* dobj = victim->getDynamicObject();
					if (dobj)
					{
						core::vector3df force = boom_vec;
						force.normalize();
						force *= BulletParameters.Mass*0.2f;

						dynmgr.translateObjectThroughTheWorld(dobj, force);
					}

					gmgr.GameNodeHit(victim, shooter, BulletParameters.Damage);

					static core::aabbox3df bb;
					bb.reset(shooter->getDislocation());

					game::SGameEvent *event = new game::SGameEvent(game::EGET_PHYSIC, shooter, victim, bb );
					GAME_EVENTS_DISPATCHER.registerGameEvent(event);

					Shooted = false;
					Boom = true;
				}

				for (v=0; v<victims_ai_size; v++)
				{	
					IGameNodeAI* victim_ai = victims_ai[v];

					if (!shooter->isWarWith(victim_ai))
					{
						gmgr.GameNodeHit(victim_ai, shooter, aipar.FriendlyDamageCoeff*BulletParameters.Damage);				

						if (shooter==victim_ai)
						{
							Shooted = false;
							Boom = true;
						}
					}
					else
					{
						const core::vector3df& node_center = victim_ai->getTransformedBBoxCenter();
						core::vector3df&       boom_vec	   = node_center-new_pos; 

						dyn::IDynamicObject* dobj = victim_ai->getDynamicObject();
						if (dobj)
						{
							core::vector3df force = boom_vec;
							force.normalize();
							force *= BulletParameters.Mass*0.2f;

							dynmgr.translateObjectThroughTheWorld(dobj, force);
						}

						gmgr.GameNodeHit(victim_ai, shooter, BulletParameters.Damage);

						static core::aabbox3df bb;
						bb.reset(shooter->getDislocation());

						game::SGameEvent *event = new game::SGameEvent(game::EGET_PHYSIC, shooter, victim_ai, bb );
						GAME_EVENTS_DISPATCHER.registerGameEvent(event);

						Shooted = false;
						Boom = true;
					}
				}
			}

			if (Boom)
			{
				if (
					ground&&
					BulletParameters.DirtSize.X>0&&
					BulletParameters.DirtSize.Y>0&&
					BulletParameters.DirtSize.Z>0	
					)
				{
					core::vector3df &dirt_pos = new_pos;
	
					core::vector3df dirt_size = BulletParameters.DirtSize;

					f32 kx = core::math::RangeRandom( 750.0f, 1250.0f) / 1000.0f; 
					f32 ky = core::math::RangeRandom( 750.0f, 1250.0f) / 1000.0f; 
					f32 kz = core::math::RangeRandom( 750.0f, 1250.0f) / 1000.0f;  

					dirt_size.X *= kx;
					dirt_size.Y *= ky;
					dirt_size.Z *= kz;
				
					smgr.makeDirtSpot(dirt_pos, dirt_size, BulletParameters.DirtColor, true);					
				}

				for (u32 i=0; i<SoundFlyEffects.size(); i++)	
					SoundFlyEffects[i].getSound()->stop();					
				for (u32 i=0; i<SoundBoomEffects.size(); i++)	
					SoundBoomEffects[i].getSound()->play();					
			}
		}
		
		CurrentPos = new_pos;

		moveSoundEffectsTo(new_pos);

		m_SceneNode->setPosition(new_pos);
		m_SceneNode->updateAbsoluteTransformation();

		scn::IBillboardSceneNode * bill = SAFE_CAST_TO_BILLBOARD_SCENE_NODE(m_SceneNode);

		if (bill)
		{
			scn::SBillboardParams bp = bill->getBillboardParameters();

			bp.AxisAligmentType = scn::EAAT_SHARP_CAM_POS;
			bp.AlignAxis = ShootedDir;

			bill->setBillboardParameters(bp);

			m_SceneNode->setVisible(true);			
		}
		else if (Shooted)  
		{
			m_SceneNode->setVisible(true);
		}

		if (Trace)
		{
			Trace->setVisible(true);

			Trace->setPosition(new_pos);
			Trace->updateAbsoluteTransformation();

			core::vector3df axis = ShootedPos;

			scn::SBillboardParams bp = Trace->getBillboardParameters();

			bp.AxisAligmentType = scn::EAAT_STRETCH_CAM_POS;
			bp.AlignAxis = axis;

			Trace->setBillboardParameters(bp);

			f32 k = 1.0f - 0.75*dist / FlyingMaxDist;

			CHECK_RANGE(k, 0.0f, 1.0f);

			Trace->setColor(
				img::SColor(				
					k * (f32)TraceInitialColorAlpha ,
					k * (f32)TraceInitialColorRed,
					k * (f32)TraceInitialColorGreen,
					k * (f32)TraceInitialColorBlue	
					)
				);			
		}

		for (u32 fe=0; fe<FlyEffects.size(); fe++)
		{
			SFlyEffectParameters &fep = FlyEffects[fe];
			scn::ISceneNode *fly_snode = (((SFlyEffectParameters&)fep).getSceneNode());

			fly_snode->setVisible(true);
			fly_snode->setPosition(new_pos);
			fly_snode->updateAbsoluteTransformation();

			if (fep.AxisAligned)
			{
				scn::IParticleSystemSceneNode *ps = 
					SAFE_CAST_TO_PARTICLE_SYSTEM_SCENE_NODE(fly_snode);
				if (ps)
				{
					for (s32 e=0; e<ps->getEmittersCount(); e++)
					{
						scn::IParticleEmitter *emit = ps->getEmitter(e);
						core::vector3df ps_dir = 
							fly_ray.getVector().normalize()*emit->getDirection().getLength()*(-1.0f);
						emit->setDirection(ps_dir);						
					}
				}
				else
				{
					scn::IBillboardSceneNode * bill = 
						SAFE_CAST_TO_BILLBOARD_SCENE_NODE(fly_snode);

					if (bill)
					{
						scn::SBillboardParams bp = bill->getBillboardParameters();
						bp.AxisAligmentType = scn::EAAT_SHARP_CAM_POS;
						bp.AlignAxis = ShootedDir;

						bill->setBillboardParameters(bp);
					}
				}							
			}
		}

		if (Trace && Trace->getReferenceCounter() <= 1)
		{
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
			_asm { int 3 }
#else
			assert(!"Trace && Trace->getReferenceCounter() <= 1");
#endif
		}

		return true;
	}

	return true;
}

//----------------------------------------------------------------------------

//! setting bullet parameters
void CBullet::setBulletParameters(const SBulletParameters &params)
{
	BulletParameters = params;	

	f32 disp = BulletParameters.Dispersion.getLength();

	AxisAlongOffset = core::math::RangeRandom(-disp/4.0f, disp/4.0f);
}

//----------------------------------------------------------------------------

//! setting bullet parameters
const SBulletParameters& CBullet::getBulletParameters() const
{
	return BulletParameters;
}

//----------------------------------------------------------------------------

//! shoot bullet
void CBullet::shoot(
	const core::vector3df& dir, const core::vector3df& shoot_pos, 
	f64 curr_time_sec)
{
	if (!Shooted)
	{
		Shooted = true;	

		ShootedPos = CurrentPos = shoot_pos;

		m_BoomTimeSec = curr_time_sec + BulletParameters.MaxFlyTime / 1000.0;

		ShootedDir = dir;
		ShootedDir.normalize();

		// setting initial bullet orientation	

		scn::IBillboardSceneNode * bill = SAFE_CAST_TO_BILLBOARD_SCENE_NODE(m_SceneNode);

		if (bill)
		{
			scn::SBillboardParams bp = bill->getBillboardParameters();

			bp.AxisAligmentType = scn::EAAT_SHARP_CAM_POS;
			bp.AlignAxis = ShootedDir;

			bill->setBillboardParameters(bp);
		}	
        	
		// calculation flying max distance

		core::line3df fly_ray(ShootedPos, ShootedPos+ShootedDir*BulletParameters.Range);

		core::array<scn::ISceneNode*> exclude_node_list;

		if (fly_ray.getLength()>0 &&
			DYNAMIC_MANAGER.findCollisionsWithRay(fly_ray,
				dyn::EDCF_MONOLITH_COLLID, m_FlyEndCollisions, &exclude_node_list)
			)
		{
			s32 idx = m_FlyEndCollisions.getNearestTo(ShootedPos);
			const dyn::SCollisionPoint &collision = m_FlyEndCollisions.getPoints()[idx];

			FlyingMaxRay.set(ShootedPos, collision.Pos);

			// correcting a little fire end pos
			FlyingMaxRay.end = FlyingMaxRay.start + FlyingMaxRay.getVector();			 
		}
		else 
		{
			FlyingMaxRay = fly_ray;
		}
      
		FlyingMaxDist = FlyingMaxRay.getLength();

		for (u32 i=0; i<SoundFlyEffects.size(); i++)
			SoundFlyEffects[i].getSound()->play();
	}
}

//----------------------------------------------------------------------------

//! binding bullet trace (can be just one)
void CBullet::bindTrace(scn::IBillboardSceneNode* trace)
{
	SAFE_DROP(Trace);

	Trace = trace;

	if (Trace)
	{
		Trace->setCullingFilter( m_SceneNode->getCullingFilter() );

		Trace->setVisible(false);

		img::SColor col = Trace->getMaterial(0).getPass(0).getDiffuseColor ();

		TraceInitialColorRed   = col.getRed();
		TraceInitialColorGreen = col.getGreen();
		TraceInitialColorBlue  = col.getBlue();
		TraceInitialColorAlpha = col.getAlpha();
	}

	SAFE_GRAB(Trace);

	if (Trace && Trace->getReferenceCounter() <= 1)
	{
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
		_asm { int 3 }
#else
		assert(!"Trace && Trace->getReferenceCounter() <= 1");
#endif
	}
}

//----------------------------------------------------------------------------

//! getting trace scene node
scn::IBillboardSceneNode* CBullet::getTrace()
{
	return Trace;
}
	
//----------------------------------------------------------------------------

//! binding bullet boom effect (multiple allowed)	
void CBullet::bindBoomEffect(SBoomEffectParameters &effect)
{
	if (!effect.getSceneNode())
		return;

	effect.getSceneNode()->setCullingFilter( m_SceneNode->getCullingFilter() );

	effect.getSceneNode()->setVisible(false);

	BoomEffects.push_back(effect);
}

//----------------------------------------------------------------------------

//! getting bullet boom effects
const core::array<SBoomEffectParameters>& CBullet::getBoomEffects()
{
	return BoomEffects;
}

//----------------------------------------------------------------------------

//! binding bullet fly effect (multiple allowed)
void CBullet::bindFlyEffect(SFlyEffectParameters &effect)
{
	if (!effect.getSceneNode())
		return;

	effect.getSceneNode()->setCullingFilter( m_SceneNode->getCullingFilter() );

	effect.getSceneNode()->setVisible(false);

	FlyEffects.push_back(effect);
}

//----------------------------------------------------------------------------

//! getting bullet fly effects
const core::array<SFlyEffectParameters>& CBullet::getFlyEffects()
{
	return FlyEffects;
}

//----------------------------------------------------------------------------

//! binding bullet sound boom effect (multiple allowed)
void CBullet::bindSoundBoomEffect(SSoundEffectParameters& effect)
{
	if (!effect.getSound())
		return;

	effect.getSound()->setVolume(GAME_MANAGER.getSoundEffectsVolume());

	SoundBoomEffects.push_back(effect);
}

//----------------------------------------------------------------------------

//! getting bullet sound boom effects
const core::array<SSoundEffectParameters>& CBullet::getSoundBoomEffects()
{
	return SoundBoomEffects;
}

//----------------------------------------------------------------------------

//! binding bullet sound fly effect (multiple allowed)
void CBullet::bindSoundFlyEffect(SSoundEffectParameters& effect)
{
	if (!effect.getSound())
		return;

	effect.getSound()->setVolume(GAME_MANAGER.getSoundEffectsVolume());

	SoundFlyEffects.push_back(effect);
}

//----------------------------------------------------------------------------

//! getting bullet sound fly effects
const core::array<SSoundEffectParameters>& CBullet::getSoundFlyEffects()
{
	return SoundFlyEffects;
}

//----------------------------------------------------------------------------

//! getting weapon from which this bullet was shooted
IGameNodeWeapon* CBullet::getWeapon()
{
	return Weapon;
}

//----------------------------------------------------------------------------

//! setting weapon from which this bullet was shooted
void CBullet::setWeapon(IGameNodeWeapon* weapon)
{
	Weapon = weapon;
}

//----------------------------------------------------------------------------

//!
scn::ISceneNode* CBullet::getSceneNode()
{
	return m_SceneNode;
}

//----------------------------------------------------------------------------

class CBulletCloner
{
public:

	static CBullet* clone(CBullet* bullet_to_clone, bool log_info)
	{
		if (!bullet_to_clone) return 0;  		

		scn::ISceneManager& smgr = SCENE_MANAGER;
		mm::ISoundDriver& snddrv = SOUND_DRIVER;

		scn::ISceneNode* scene_node_to_clone = bullet_to_clone->getSceneNode();

		static core::stringc Msg;

		if (log_info)
		{
			Msg.sprintf( "Cloning Bullet Node . . ." );         
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);    
		}

		LOGGER.increaseFormatLevel();		

		scn::ISceneNode* scene_node_cloned =
			smgr.cloneSceneNode( scene_node_to_clone, true, log_info );	
		
		CBullet* bullet_cloned = 0;
		
		if (scene_node_cloned)
			bullet_cloned = new CBullet(scene_node_cloned);
             
		LOGGER.decreaseFormatLevel();

		if (!bullet_cloned)                                                                                    
		{            
			core::stringc fname = 
				bullet_to_clone->getSceneNode() ?
				bullet_to_clone->getSceneNode()->getFileName() : "";

			Msg.sprintf("Error cloning Bullet Node %s",	fname.c_str());               
			LOGGER.log(Msg.c_str(), io::ELL_ERROR);                                                   
		}                                                                                             
		else 
		if (log_info)                                                                                         
		{       
			core::stringc fname = 
				bullet_to_clone->getSceneNode() ?
				bullet_to_clone->getSceneNode()->getFileName() : "";

			Msg.sprintf("Bullet Node %s successfully cloned.", fname.c_str());         
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);                                             
		} 

		if (bullet_cloned)
		{	
			// cloning trace

			scn::IBillboardSceneNode* trace_snode = bullet_to_clone->getTrace();						
			if (trace_snode)
			{
				scn::ISceneNode* snode = smgr.cloneSceneNode(trace_snode, true, log_info);

				scn::IBillboardSceneNode* cloned_bill_snode = 
					SAFE_CAST_TO_BILLBOARD_SCENE_NODE(snode);

				if (cloned_bill_snode)
					bullet_cloned->bindTrace(cloned_bill_snode);	
			}	

			// cloning boom effects

			const core::array<SBoomEffectParameters>& boom_effects = bullet_to_clone->getBoomEffects();
			for (u32 be=0; be<boom_effects.size(); be++)
			{
				const SBoomEffectParameters &bep = boom_effects[be];
				scn::ISceneNode *boom_snode = (((SBoomEffectParameters&)bep).getSceneNode());
				if (boom_snode)
				{
					scn::ISceneNode* snode = smgr.cloneSceneNode(boom_snode, true, log_info);
					if (snode)
					{
						SBoomEffectParameters new_effect(snode);
						new_effect.set(bep);

						bullet_cloned->bindBoomEffect(new_effect);
					}
				}
			}

			// cloning fly effects

			const core::array<SFlyEffectParameters>& fly_effects = bullet_to_clone->getFlyEffects();
			for (u32 fe=0; fe<fly_effects.size(); fe++)
			{
				const SFlyEffectParameters &fep = fly_effects[fe];
				scn::ISceneNode *fly_snode = (((SFlyEffectParameters&)fep).getSceneNode());
				if (fly_snode)
				{
					scn::ISceneNode* snode = smgr.cloneSceneNode(fly_snode, true, log_info);
					if (snode)
					{
						SFlyEffectParameters new_effect(snode);
						new_effect.set(fep);

						bullet_cloned->bindFlyEffect(new_effect);
					}
				}
			}			

			// cloning boom sound effects

			const core::array<SSoundEffectParameters>& snd_boom_effects = bullet_to_clone->getSoundBoomEffects();
			for (u32 i=0; i<snd_boom_effects.size(); i++)
			{
				const SSoundEffectParameters &sep = snd_boom_effects[i];
				mm::ISound *snd_to_clone = (((SSoundEffectParameters&)sep).getSound());
				if (snd_to_clone)
				{
					mm::ISound *snd_cloned = snddrv.cloneSound(snd_to_clone);
					if (snd_cloned)
					{
						SSoundEffectParameters new_effect(snd_cloned);
						new_effect.set(sep);

						bullet_cloned->bindSoundBoomEffect(new_effect);
					}
				}				
			}

			// cloning fly sound effects

			const core::array<SSoundEffectParameters>& snd_fly_effects = bullet_to_clone->getSoundFlyEffects();
			for (u32 i=0; i<snd_fly_effects.size(); i++)
			{
				const SSoundEffectParameters &sep = snd_fly_effects[i];
				mm::ISound *snd_to_clone = (((SSoundEffectParameters&)sep).getSound());
				if (snd_to_clone)
				{
					mm::ISound *snd_cloned = snddrv.cloneSound(snd_to_clone);
					if (snd_cloned)
					{
						SSoundEffectParameters new_effect(snd_cloned);
						new_effect.set(sep);

						bullet_cloned->bindSoundFlyEffect(new_effect);
					}
				}				
			}
			
			bullet_cloned->setBulletParameters(bullet_to_clone->getBulletParameters());		
		}

		return bullet_cloned;
	}
};

//----------------------------------------------------------------------------

class CBulletLoader
{

public:

	static CBullet* load(const c8* fname)
	{
		core::stringc XMLFileName;
		core::stringc Msg;
		CBullet* bullet_node;
		io::IXMLReader* XMLFile;
		s32 SceneDataXMLPos, GameDataXMLPos, SoundDataXMLPos, DynamicDataXMLPos;

		XMLFileName = "";
		Msg = "";
		bullet_node =0;
		XMLFile = 0;
		SceneDataXMLPos=GameDataXMLPos=SoundDataXMLPos=0;

		XMLFileName = fname;
		
		XMLFile = FILE_SYSTEM.createXMLReader(XMLFileName.c_str());  
		
		Msg.sprintf( "Loading Bullet Node from %s . . .", XMLFileName.c_str());         
		LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);    

		LOGGER.increaseFormatLevel();

		if (!XMLFile)                                                                                     
		{                                                                                             
			Msg.sprintf("Can't open xml-file %s", XMLFileName.c_str());                               
			LOGGER.log(Msg.c_str(), io::ELL_WARNING);                    
		}
		else 
		{
			XMLFile->rewindToBegin();

			bool scene_data_finded = false, game_data_finded = false; 
			bool sound_data_finded = false, dynamic_data_finded = false;

			SceneDataXMLPos = GameDataXMLPos = SoundDataXMLPos = DynamicDataXMLPos = XMLFile->getPos();	
			
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Scene") == XMLFile->getName())
					{
						scene_data_finded = true;
					}
					else 
					if (core::stringw("Game") == XMLFile->getName())
					{
						game_data_finded = true;		
					}
					else 
					if (core::stringw("Sound") == XMLFile->getName())
					{
						sound_data_finded = true;
					}
					else 
					if (core::stringw("Dynamic") == XMLFile->getName())
					{
						dynamic_data_finded = true;
					}
				}
				
				if (!scene_data_finded)
				{
					SceneDataXMLPos = XMLFile->getPos();
				}

				if (!game_data_finded)
				{
					GameDataXMLPos = XMLFile->getPos();
				}

				if (!sound_data_finded)
				{
					SoundDataXMLPos = XMLFile->getPos();
				}

				if (!dynamic_data_finded)
				{
					DynamicDataXMLPos = XMLFile->getPos();
				}				
			}			
		}		

		if (XMLFile)                                                                                     
		{
			// loading scene data

			XMLFile->setPos(SceneDataXMLPos);

			scn::ISceneNode* scene_node = 
				SCENE_MANAGER.loadSceneNode(XMLFile, true);			

			if (scene_node)                                                                                     
			{    
				bullet_node = new CBullet(scene_node); 
			}

			if (!bullet_node)
			{
				SAFE_DROP(XMLFile);

				return NULL;		
			}
		} 	

		if (XMLFile && bullet_node)
		{
			SBulletParameters params;

			// loading game data

			XMLFile->setPos(GameDataXMLPos);

			bool game_data_finded = false, sound_data_finded = false;	
			
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Game") == XMLFile->getName())	
					{
						game_data_finded = true;
						sound_data_finded = false;
					}					
					else 
					if (core::stringw("Sound") == XMLFile->getName())	
					{
						game_data_finded = false;
						sound_data_finded = true;
					}	
					else 
					if (game_data_finded)
					{
						if (core::stringw("Parameters") == XMLFile->getName())
						{
							params.MaxFlyTime = XMLFile->getAttributeValueAsFloat(L"fly_time");
							params.Mass  = XMLFile->getAttributeValueAsFloat(L"mass");
							params.Damage= XMLFile->getAttributeValueAsFloat(L"damage");
							params.DamageRadius= XMLFile->getAttributeValueAsFloat(L"damage_radius", 0);
							params.Range       = XMLFile->getAttributeValueAsFloat(L"range");
							params.Dispersion= XMLFile->getAttributeValueAsVector(L"dispersion",core::vector3df(0,0,0));	
							params.DirtSize  = XMLFile->getAttributeValueAsVector(L"dirt_size",core::vector3df(0,0,0));
							params.DirtColor = XMLFile->getAttributeValueAsColor(L"dirt_color",img::SColor(255,0,0,0));			
						}					
						else 
						if (core::stringw("BindTrace") == XMLFile->getName())
						{
							core::stringc trace_fname = XMLFile->getAttributeValue(L"filename");

							if (trace_fname.size() >0)
							{			
								LOGGER.log("Binding trace for this bullet . . .");					

								core::stringc full_fname = XMLFile->getFilePath();
								full_fname.append(trace_fname.c_str());					

								scn::ISceneNode *snode = 
									SCENE_MANAGER.loadSceneNode(full_fname.c_str());

								scn::IBillboardSceneNode *trace = 
									SAFE_CAST_TO_BILLBOARD_SCENE_NODE(snode);

								if (trace)
								{
									bullet_node->bindTrace(trace);
								}
							}
						}						
						else 
						if (core::stringw("BindFlyEffect") == XMLFile->getName())
						{
							core::stringc fname = XMLFile->getAttributeValue(L"filename");
							s32 life_time_after_boom = XMLFile->getAttributeValueAsInt(L"life_time_after_boom",0);
							core::vector3df scale=XMLFile->getAttributeValueAsVector(L"scale", core::vector3df(1,1,1));
							core::vector3df offset=XMLFile->getAttributeValueAsVector(L"offset", core::vector3df(0,0,0));
							bool axis_aligned=XMLFile->getAttributeValueAsInt(L"axis_aligned")!=0 ? true : false;

							if (fname.size() >0)
							{			
								LOGGER.log("Binding fly effect for this bullet . . .");					

								core::stringc full_fname = XMLFile->getFilePath();
								full_fname.append(fname.c_str());					

								scn::ISceneNode *snode = 
									SCENE_MANAGER.loadSceneNode(full_fname.c_str());

								if (snode)
								{
									snode->setScale(scale);

									snode->setVisible(false);

									SFlyEffectParameters fly_params(snode);
									fly_params.AxisAligned      = axis_aligned;
									fly_params.LifeTimeAfterBoom= life_time_after_boom;
									fly_params.Offset           = offset;

									bullet_node->bindFlyEffect(fly_params);
								}
							}
						}
						else 
						if (core::stringw("BindBoomEffect") == XMLFile->getName())
						{
							core::stringc smoke_fname = XMLFile->getAttributeValue(L"filename");
							s32 delay_after_boom      = XMLFile->getAttributeValueAsInt(L"delay_after_boom");;
							s32 life_time             = XMLFile->getAttributeValueAsInt(L"life_time");;
							core::vector3df scale=XMLFile->getAttributeValueAsVector(L"scale", core::vector3df(1,1,1));
							core::vector3df offset=XMLFile->getAttributeValueAsVector(L"offset", core::vector3df(0,0,0));
							bool norm_alig  = XMLFile->getAttributeValueAsBool(L"normal_aligned", false);
							bool air        = XMLFile->getAttributeValueAsBool(L"air",    false);
							bool ground     = XMLFile->getAttributeValueAsBool(L"ground", false);
							bool wall       = XMLFile->getAttributeValueAsBool(L"wall",   false);
							bool flesh      = XMLFile->getAttributeValueAsBool(L"flesh",  false);							

							if (smoke_fname.size() >0)
							{			
								LOGGER.log("Binding boom effect for this bullet . . .");					

								core::stringc full_fname = XMLFile->getFilePath();
								full_fname.append(smoke_fname.c_str());					

								scn::ISceneNode *snode = 
									SCENE_MANAGER.loadSceneNode(full_fname.c_str());

								if (snode)
								{
									snode->setScale(scale);

									snode->setVisible(false);

									const core::list<scn::ISceneNodeAnimator*>&ani =  snode->getAnimators();
									core::list<scn::ISceneNodeAnimator*>::iterator ait = ani.begin();
									for (; ait != ani.end(); ++ait)
									{
										scn::ISceneNodeAnimator *a =(*ait);
										a->setAnimationStartTime(0);
									}

									SBoomEffectParameters boom_params(snode);
									boom_params.DelayAfterBoom= delay_after_boom;
									boom_params.LifeTime      = life_time;
									boom_params.NormalAligned = norm_alig;
									boom_params.Air    = air;
									boom_params.Ground = ground;
									boom_params.Wall   = wall;									
									boom_params.Flesh  = flesh;
									boom_params.Offset = offset;

									bullet_node->bindBoomEffect(boom_params);
								}
							}
						}
						else
						{
							break;  
						}
					}
					else 
					if (sound_data_finded)
					{
						if (core::stringw("SoundFlyEffect") == XMLFile->getName())
						{
							core::stringc fname = XMLFile->getAttributeValue(L"filename");
							bool          loop  = XMLFile->getAttributeValueAsBool(L"loop",false);
							
							if (fname.size()>0)
							{			
								LOGGER.log("Binding fly sound effect for this bullet . . .");					

								core::stringc full_fname = 
									RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_EFFECTS );
								full_fname.append(fname.c_str());	

								mm::ISound* snd = 
									SOUND_DRIVER.getSound(full_fname.c_str(), loop, true);
								if (snd)
								{
									SSoundEffectParameters snd_params(snd);
									
									bullet_node->bindSoundFlyEffect(snd_params);
								}
							}
						}
						else
						if (core::stringw("SoundBoomEffect") == XMLFile->getName())
						{
							core::stringc fname = XMLFile->getAttributeValue(L"filename");
							bool          loop  = XMLFile->getAttributeValueAsBool(L"loop",false);
							
							if (fname.size()>0)
							{			
								LOGGER.log("Binding boom sound effect for this bullet . . .");					

								core::stringc full_fname = 
									RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_EFFECTS );
								full_fname.append(fname.c_str());	

								mm::ISound* snd = 
									SOUND_DRIVER.getSound(full_fname.c_str(), loop, true);
								if (snd)
								{	
									SSoundEffectParameters snd_params(snd);

									bullet_node->bindSoundBoomEffect(snd_params);
								}
							}
						}
						else	
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

			bullet_node->setBulletParameters(params);			
		}

		LOGGER.decreaseFormatLevel();

		if (!bullet_node)                                                                                    
		{                                                                                             
			Msg.sprintf("Error loading Bullet Node from %s", XMLFileName.c_str());               
			LOGGER.log(Msg.c_str(), io::ELL_ERROR);                                                   
		}                                                                                             
		else                                                                                          
		{                                                                                             
			Msg.sprintf("Bullet Node successfully loaded from %s", XMLFileName.c_str());         
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);   
		} 

		SAFE_DROP(XMLFile);	

		return bullet_node;
	}
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------