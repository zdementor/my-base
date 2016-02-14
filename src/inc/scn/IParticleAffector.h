//|-------------------------------------------------------------------------
//| File:        IParticleAffector.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IParticleAffectorHPP
#define IParticleAffectorHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include "SParticle.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! A particle affector modifies particles.
class __MY_SCN_LIB_API__ IParticleAffector : public IUnknown
{
public:

	IParticleAffector() : m_AffectTimeMs(0.0f) {}	
	IParticleAffector(u32 affect_time_ms) : m_AffectTimeMs(affect_time_ms) {}

    //! Affects an array of particles.
    //! \param now: Current time. (Same as ITimer::getTime() would return)
    //! \param particlearray: Array of particles.
    //! \param count: Amount of particles in array.
    virtual void affect(u32 now, core::list<SParticle*>& particlearray) = 0;

    //! return affector type
    virtual E_PARTICLE_AFFECTOR_TYPE getType() =0;

    //! set enabled
    virtual void setEnabled(bool e) =0;

    //! return true if enabled, otherwise false
    virtual bool isEnabled() =0;

	//!
	virtual bool set(IParticleAffector* pa)
	{
		if (!pa || pa->getType()!=getType())
		{
			return false;
		}		

		m_AffectTimeMs = pa->getAffectTimeMs();

		setEnabled(pa->isEnabled());

		return true;
	}

	//!
	virtual u32 getAffectTimeMs()
	{
		return (u32)m_AffectTimeMs;
	}

	//!
	virtual void setAffectTimeMs(u32 time_ms)
	{
		m_AffectTimeMs = time_ms;
	}

protected:

	f32 m_AffectTimeMs;
};

//----------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IParticleFadeOutAffector : public IParticleAffector
{
public:

	IParticleFadeOutAffector(u32 affect_time_ms)
		: IParticleAffector(affect_time_ms) {}

    //! return target color
    virtual img::SColor getTargetColor() = 0;

    //! set target color
    virtual void setTargetColor(const img::SColor &col) = 0;
};

//----------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IParticleGravityAffector : public IParticleAffector
{
public:

	IParticleGravityAffector(u32 affect_time_ms)
		: IParticleAffector(affect_time_ms) {}

    //! return Gravity
    virtual core::vector3df getGravity() = 0;

    //! set Gravity
    virtual void setGravity(const core::vector3df &g) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

