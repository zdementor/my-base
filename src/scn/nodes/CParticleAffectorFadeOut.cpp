//|-------------------------------------------------------------------------
//| File:        CParticleAffectorFadeOut.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CParticleAffectorFadeOut.h"

#include <vid/IVideoDriver.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CParticleFadeOutAffector::CParticleFadeOutAffector(
    img::SColor targetColor, u32 fadeOutTime
	) : IParticleFadeOutAffector(fadeOutTime),
TargetColor(targetColor),
m_GL(VIDEO_DRIVER.getDriverFamily() == vid::EDF_OPENGL),
m_Enabled(true)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CParticleFadeOutAffector");
#endif    
}

//---------------------------------------------------------------------------

void CParticleFadeOutAffector::affect(u32 now, core::list<SParticle*>& particlearray)
{
	if (!m_Enabled) return;

	img::SColor target_color;
	if (m_GL)
		target_color = TargetColor.toOpenGLColor();
	else
		target_color = TargetColor;

	core::list<SParticle*>::iterator it = particlearray.begin();
    for (; it != particlearray.end(); ++it)
    {
		SParticle* p = *it;

        if (p->endTime - now < m_AffectTimeMs)
        {
            f32 d = (p->endTime - now) / m_AffectTimeMs;
            p->color = p->startColor.getInterpolated(target_color, d);
        }
	}
}

//---------------------------------------------------------------------------

E_PARTICLE_AFFECTOR_TYPE CParticleFadeOutAffector::getType()
{
    return EPAT_FADEOUT;
}
//---------------------------------------------------------------------------

bool CParticleFadeOutAffector::isEnabled()
{
    return m_Enabled;
}
//---------------------------------------------------------------------------

img::SColor CParticleFadeOutAffector::getTargetColor()
{
    return TargetColor;
}

//---------------------------------------------------------------------------

void CParticleFadeOutAffector::setEnabled(bool e)
{
    m_Enabled = e;
}

//---------------------------------------------------------------------------

void CParticleFadeOutAffector::setTargetColor(const img::SColor &col)
{
    TargetColor = col;
}

//---------------------------------------------------------------------------

bool CParticleFadeOutAffector::set(IParticleAffector* pa)
{
	if (!IParticleAffector::set(pa))
		return false;

	CParticleFadeOutAffector* affector = static_cast<CParticleFadeOutAffector*>(pa);

	if (!affector)
		return false;
	
	setTargetColor(affector->getTargetColor());
	
	return true;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
