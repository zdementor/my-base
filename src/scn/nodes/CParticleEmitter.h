//|-------------------------------------------------------------------------
//| File:        CParticleEmitter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CParticleEmitterHPP
#define CParticleEmitterHPP
//----------------------------------------------------------------------------

#include <scn/IParticleEmitter.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CParticleEmitter : public IParticleEmitter
{
public:

	CParticleEmitter(
		core::aabbox3df volume, core::vector3df direction,  
		u32 minParticlesPerSecond, u32 maxParticlePerSecond, 
		img::SColor minStartColor, img::SColor maxStartColor, 
		u32 lifeTimeMin, u32 lifeTimeMax,
		s32 maxAngleDegrees);
    virtual ~CParticleEmitter();

	virtual s32 emitt(u32 now, u32 timediff,	const core::matrix4 &transformation, 
		const core::vector3df& horiz_normalized, const core::vector3df& vert_normalized);

    ///////////////////////////////////////////////////////////////////////////
    //               Get Functions
    ///////////////////////////////////////////////////////////////////////////

	//! return if transformation of particles exists in global space, otherwise false
	virtual bool isGlobalParticlesTransformation();

    //! Gets the size of all particles.
    virtual const core::dimension2d<f32>& getParticleSize();

    //! return particles moving direction
    virtual const core::vector3df& getDirection();

    //! return min umber of particles appearing per second
    virtual u32 getMinParticlePerSecond();

    //! return max number of particles appearing per second
    virtual u32 getMaxParticlePerSecond();

    //! return min color in the beginig of particle life
    virtual const img::SColor& getMinStartColor();

    //! return max color in the beginig of particle life
    virtual const img::SColor& getMaxStartColor();

    //! return min particle life time
    virtual u32 getMinLifeTime();

    //! return max particle life time
    virtual u32 getMaxLifeTime();

    //! return minimal angle of particle moving
    virtual s32 getMaxAngleDegrees();

    //! return volume, particles appears within
    virtual const core::aabbox3d<f32>& getAppearVolume();

	//! setting volume, particles appears within
    virtual vid::ITexture* getTexture();

	//! return true if enabled, otherwise false
	virtual bool isEnabled();

    ///////////////////////////////////////////////////////////////////////////
    //               Set Functions
    ///////////////////////////////////////////////////////////////////////////

	//! set true if wants to transform particles in global space, otherwise false (default: false)
	virtual void setGlobalParticlesTransformation(bool value);

	//! Sets the size of all particles.
    virtual void setParticleSize( const core::dimension2d<f32> &size = core::dimension2d<f32>(5.0f, 5.0f));

    //! return particles moving direction
    virtual void setDirection(const core::vector3df &dir);

    //! return min umber of particles appearing per second
    virtual void setMinParticlePerSecond(u32 pps);

    //! return max number of particles appearing per second
    virtual void setMaxParticlePerSecond(u32 pps);

    //! return min color in the beginig of particle life
    virtual void setMinStartColor(const img::SColor &col);

    //! return max color in the beginig of particle life
    virtual void setMaxStartColor(const img::SColor &col);

    //! return min particle life time
    virtual void setMinLifeTime(u32 lt);

    //! return max particle life time
    virtual void setMaxLifeTime(u32 lt);

    //! return minimal angle of particle moving
    virtual void setMaxAngleDegrees(s32 ang);

    //! setting volume, particles appears within
    virtual void setAppearVolume(const core::aabbox3d<f32> &volume);

	//! setting volume, particles appears within
    virtual void setTexture(vid::ITexture* tex);

	//! set enabled/disabled
    virtual void setEnabled(bool enabled);

	//!
	virtual bool set(IParticleEmitter* pe);

	//////////////////////////////////////////////////////

	//! Adds new particle effector to the particle system. 
	virtual IParticleAffector* addAffector(E_PARTICLE_AFFECTOR_TYPE atype);

	//! Removes particle affectors ny index. returns true on success
	virtual bool removeAffector(s32 idx);

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors();

	//! return particle affector
	virtual IParticleAffector* getAffector(s32 idx);

	//! return particle affector count
	virtual s32 getAffectorsCount();

	//! return particle affectors list
    virtual core::list<IParticleAffector*>& getAffectorsList();

	//!
	virtual vid::S3DVertex1TCoordsColoured* getVertices();

	//!
	virtual u16* getIndices();

	//!
	virtual s32 getParticlesCount();

	//!
	virtual const core::aabbox3d<f32>& getBoundingBox();
 
protected:

	bool m_Enabled;

	void reallocateBuffers();

	core::aabbox3d<f32> m_BoundingBox;

	core::aabbox3d<f32> m_AppearVolume;
    core::vector3df m_Direction;
    u32 m_MinParticlesPerSecond, m_MaxParticlesPerSecond;
    img::SColor m_MinStartColor, m_MaxStartColor;
    u32 m_MinLifeTime, m_MaxLifeTime;
    s32 m_MaxAngleDegrees;
	core::dimension2d<f32> m_ParticleSize;

	vid::ITexture *m_Texture;

	u32 m_Time;
    u32 m_Emitted;

	core::list<IParticleAffector*> m_AffectorList;

	core::list<SParticle*> m_Particles;

	core::array<vid::S3DVertex1TCoordsColoured> m_Vertices; // 4 vertices per particle
    core::array<u16> m_Indices; // 6 indices per particle

	bool m_GlobalParticlesTransformation;

	bool m_GL;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

