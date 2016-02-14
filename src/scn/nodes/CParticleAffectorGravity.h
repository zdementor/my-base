//|-------------------------------------------------------------------------
//| File:        CParticleAffectorGravity.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CParticleGravityAffectorHPP
#define CParticleGravityAffectorHPP
//---------------------------------------------------------------------------

#include <scn/IParticleAffector.h>
#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Particle Affector for affecting direction of particle
class CParticleGravityAffector : public IParticleGravityAffector
{
public:

    CParticleGravityAffector(
        const core::vector3df &gravity = core::vector3df(0.0f,-0.03f,0.0f),
        u32 timeForceLost = 1000);

	// IParticleAffector implementation

    virtual void affect(u32 now, core::list<SParticle*>& particlearray);

    virtual E_PARTICLE_AFFECTOR_TYPE getType();    

    virtual bool isEnabled();
    virtual void setEnabled(bool e);

	virtual bool set(IParticleAffector* pa);

	// IParticleGravityAffector implementation

    virtual core::vector3df getGravity();
    virtual void setGravity(const core::vector3df &g);

private:

    core::vector3df m_Gravity;

    bool m_Enabled;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif //#ifndef CParticleGravityAffectorHPP

