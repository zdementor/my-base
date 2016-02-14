//|-------------------------------------------------------------------------
//| File:        CParticleAffectorFadeOut.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CParticleFadeOutAffectorHPP
#define CParticleFadeOutAffectorHPP
//---------------------------------------------------------------------------

#include <scn/IParticleAffector.h>
#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Particle Affector for fading out a color
class CParticleFadeOutAffector : public IParticleFadeOutAffector
{
public:

    CParticleFadeOutAffector(img::SColor targetColor, u32 fadeOutTime);

	// IParticleAffector implementation

    virtual void affect(u32 now, core::list<SParticle*>& particlearray);

    virtual E_PARTICLE_AFFECTOR_TYPE getType();

    virtual bool isEnabled();
    virtual void setEnabled(bool e);

	virtual bool set(IParticleAffector* pa);

	// IParticleFadeOutAffector implementation

    virtual img::SColor getTargetColor();
    virtual void setTargetColor(const img::SColor &col);	

private:

    img::SColor TargetColor;
	bool m_GL;
	bool m_Enabled;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CParticleFadeOutAffectorHPP

