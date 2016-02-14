//|-------------------------------------------------------------------------
//| File:        CParticleEmitterBox.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CParticleBoxEmitterHPP
#define CParticleBoxEmitterHPP
//----------------------------------------------------------------------------

#include "CParticleEmitter.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! A default box emitter
class CParticleBoxEmitter : public CParticleEmitter
{
public:
    CParticleBoxEmitter(
        core::aabbox3df box,
        core::vector3df direction,
        u32 minParticlesPerSecond,
        u32 maxParticlePerSecond,
        img::SColor minStartColor,
        img::SColor maxStartColor,
        u32 lifeTimeMin,
        u32 lifeTimeMax,
        s32 maxAngleDegrees);

    virtual s32 emitt(u32 now, u32 timeSinceLastCall, const core::matrix4 &transformation, 
		const core::vector3df& horiz_normalized, const core::vector3df& vert_normalized);

    virtual E_PARTICLE_EMITTER_TYPE getType()  
	{ return EPET_BOX; }
 
private:
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

