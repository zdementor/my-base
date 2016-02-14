//|-------------------------------------------------------------------------
//| File:        IParticleEmitter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IParticleEmitterHPP
#define IParticleEmitterHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include "SParticle.h"
#include <vid/ITexture.h>
#include "IParticleAffector.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! A particle emitter for using with particle systems.
//! A Particle emitter emitts new particles into a particle system.
class __MY_SCN_LIB_API__ IParticleEmitter : public IUnknown
{
public:

    //! Prepares an array with new particles to emitt into the system
    //! and returns how much new particles there are.
    //! \param now: Current time.
    //! \param timeSinceLastCall: Time elapsed since last call, in milliseconds.
    //! particles to add into the system.
    //! \return Returns amount of new particles in the array. Can be 0.
    virtual s32 emitt(u32 now, u32 timeSinceLastCall, const core::matrix4 &transformation,
		const core::vector3df& horiz_normalized, const core::vector3df& vert_normalized) = 0;

    ///////////////////////////////////////////////////////////////////////////
    //               Get Functions
    ///////////////////////////////////////////////////////////////////////////

    //! return emitter type
    virtual E_PARTICLE_EMITTER_TYPE getType() =0;

	//! return if transformation of particles exists in global space, otherwise false
	virtual bool isGlobalParticlesTransformation()=0;

    //! Gets the size of all particles.
    virtual const core::dimension2d<f32>& getParticleSize() = 0;

    //! return particles moving direction
    virtual const core::vector3df& getDirection() = 0;

    //! return min umber of particles appearing per second
    virtual u32 getMinParticlePerSecond() = 0;

    //! return max number of particles appearing per second
    virtual u32 getMaxParticlePerSecond() = 0;

    //! return min color in the beginig of particle life
    virtual const img::SColor& getMinStartColor() = 0;

    //! return max color in the beginig of particle life
    virtual const img::SColor& getMaxStartColor() = 0;

    //! return min particle life time
    virtual u32 getMinLifeTime() = 0;

    //! return max particle life time
    virtual u32 getMaxLifeTime() = 0;

    //! return minimal angle of particle moving
    virtual s32 getMaxAngleDegrees() = 0;

    //! return volume, particles appears within
    virtual const core::aabbox3d<f32>& getAppearVolume() = 0;

	//! setting volume, particles appears within
    virtual vid::ITexture* getTexture() = 0;

	//! return true if enabled, otherwise false
    virtual bool isEnabled() = 0;

    ///////////////////////////////////////////////////////////////////////////
    //               Set Functions
    ///////////////////////////////////////////////////////////////////////////

	//! set true if wants to transform particles in global space, otherwise false (default: false)
	virtual void setGlobalParticlesTransformation(bool value)=0;

	//! Sets the size of all particles.
    virtual void setParticleSize(
        const core::dimension2d<f32> &size = core::dimension2d<f32>(5.0f, 5.0f)
		) = 0;

    //! return particles moving direction
    virtual void setDirection(const core::vector3df &dir) = 0;

    //! return min umber of particles appearing per second
    virtual void setMinParticlePerSecond(u32 pps) = 0;

    //! return max number of particles appearing per second
    virtual void setMaxParticlePerSecond(u32 pps) = 0;

    //! return min color in the beginig of particle life
    virtual void setMinStartColor(const img::SColor &col) = 0;

    //! return max color in the beginig of particle life
    virtual void setMaxStartColor(const img::SColor &col) = 0;

    //! return min particle life time
    virtual void setMinLifeTime(u32 lt) = 0;

    //! return max particle life time
    virtual void setMaxLifeTime(u32 lt) = 0;

    //! return minimal angle of particle moving
    virtual void setMaxAngleDegrees(s32 ang) = 0;

    //! setting volume, particles appears within
    virtual void setAppearVolume(const core::aabbox3d<f32> &volume) = 0;

	//! setting volume, particles appears within
    virtual void setTexture(vid::ITexture* tex) = 0;

	//! set enabled/disabled
    virtual void setEnabled(bool enabled) = 0;

	//!
	virtual bool set(IParticleEmitter* pe) = 0;

	//////////////////////////////////////////////////////

	//! Adds new particle effector to the particle system. 
	virtual IParticleAffector* addAffector(E_PARTICLE_AFFECTOR_TYPE atype) = 0;

	//! Removes particle affectors ny index. returns true on success
	virtual bool removeAffector(s32 idx) = 0;

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors() = 0;

	//! return particle affector
	virtual IParticleAffector* getAffector(s32 idx) = 0;

	//! return particle affector count
	virtual s32 getAffectorsCount() = 0;   
	
	//! return particle affectors list
    virtual core::list<IParticleAffector*>& getAffectorsList() = 0;
	
	//!
	virtual vid::S3DVertex1TCoordsColoured* getVertices() = 0;

	//!
	virtual u16* getIndices() = 0;

	//!
	virtual s32 getParticlesCount() = 0;

	//! 
	virtual const core::aabbox3df& getBoundingBox() = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

