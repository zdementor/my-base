//|-------------------------------------------------------------------------
//| File:        CParticleAffectorGravity.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CParticleAffectorGravity.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CParticleGravityAffector::CParticleGravityAffector(
    const core::vector3df &gravity, u32 timeForceLost
	) : IParticleGravityAffector(timeForceLost),
m_Gravity(gravity), m_Enabled(true)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CParticleGravityAffector");
#endif
}

//---------------------------------------------------------------------------

void CParticleGravityAffector::affect(u32 now, core::list<SParticle*>& particlearray)
{
    if (!m_Enabled) return;

    f32 d;

	core::list<SParticle*>::iterator it = particlearray.begin();

    for (; it!=particlearray.end(); ++it)
    {
		SParticle* p = *it;

        d = (now - p->startTime) / m_AffectTimeMs;
        if (d > 1.0f) d = 1.0f;
        if (d < 0.0f) d = 0.0f;
        d = 1.0f - d;

        p->vector = p->startVector.getInterpolated(m_Gravity, d);
    }
}
//---------------------------------------------------------------------------

E_PARTICLE_AFFECTOR_TYPE CParticleGravityAffector::getType()
{
    return EPAT_GRAVITY;
}

//---------------------------------------------------------------------------

bool CParticleGravityAffector::isEnabled()
{
    return m_Enabled;
}

//---------------------------------------------------------------------------

core::vector3df CParticleGravityAffector::getGravity()
{
    return m_Gravity;
}

//---------------------------------------------------------------------------

void CParticleGravityAffector::setEnabled(bool e)
{
    m_Enabled = e;
}

//---------------------------------------------------------------------------

void CParticleGravityAffector::setGravity(const core::vector3df &g)
{
    m_Gravity = g;
}

//---------------------------------------------------------------------------

bool CParticleGravityAffector::set(IParticleAffector* pa)
{
	if (!IParticleAffector::set(pa))
		return false;

	CParticleGravityAffector* affector = static_cast<CParticleGravityAffector*>(pa);

	if (!affector)
		return false;

	setGravity(affector->getGravity());

	return true;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
