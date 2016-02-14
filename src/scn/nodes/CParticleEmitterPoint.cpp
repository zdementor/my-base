//|-------------------------------------------------------------------------
//| File:        CParticleEmitterPoint.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CParticleEmitterPoint.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CParticlePointEmitter::CParticlePointEmitter(
    core::vector3df direction,  u32 minParticlesPerSecond,
    u32 maxParticlePerSecond,   img::SColor minStartColor,
    img::SColor maxStartColor, u32 lifeTimeMin, u32 lifeTimeMax,
    s32 maxAngleDegrees)
		: CParticleEmitter(
			core::aabbox3df(), direction, 
			minParticlesPerSecond, maxParticlePerSecond,  
			minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax,
			maxAngleDegrees)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CParticlePointEmitter");
#endif
}

//---------------------------------------------------------------------------

s32 CParticlePointEmitter::emitt(
	u32 now, u32 timeSinceLastCall, const core::matrix4 &transformation, 
	const core::vector3df& horiz_normalized, const core::vector3df& vert_normalized)
{
	if (m_Particles.size() > 16386)
		return 0;

	s32 amount = 0;

	if (m_Enabled)
	{
		m_Time += timeSinceLastCall;

		u32 pps = (m_MaxParticlesPerSecond - m_MinParticlesPerSecond);
		f32 perSecond = pps ?
			(f32)m_MinParticlesPerSecond + (core::math::Random() % pps) :
			m_MinParticlesPerSecond;
		f32 everyWhatMillisecond = 1000.0f / perSecond;

		core::vector3df pos = getAppearVolume().getCenter();
		transformation.transformVect(pos);

		if (m_Time > everyWhatMillisecond)
		{
			img::SColor min_start_solor, max_start_solor;
			if (m_GL)
			{
				min_start_solor = m_MinStartColor.toOpenGLColor();
				max_start_solor = m_MaxStartColor.toOpenGLColor();
			}
			else
			{
				min_start_solor = m_MinStartColor;
				max_start_solor = m_MaxStartColor;
			}

			m_Time = 0;

			SParticle *p = new SParticle();

			p->pos = pos;

			p->startTime = now;
			p->vector = m_Direction;

			if (m_MaxAngleDegrees)
			{
				core::vector3df tgt = m_Direction;
				tgt.rotateXYByDegrees(
					(core::math::Random() % (m_MaxAngleDegrees*2)) - m_MaxAngleDegrees,
					core::vector3df(0,0,0));
				tgt.rotateYZByDegrees(
					(core::math::Random()%(m_MaxAngleDegrees*2)) - m_MaxAngleDegrees,
					core::vector3df(0,0,0));
				p->vector = tgt;
			}

			if (m_MaxLifeTime - m_MinLifeTime == 0)
				p->endTime = now + m_MinLifeTime;
			else
				p->endTime = now + m_MinLifeTime + (core::math::Random() % (m_MaxLifeTime - m_MinLifeTime));

			p->color = min_start_solor.getInterpolated(
				max_start_solor, (core::math::Random() % 100) / 100.0f);

			p->startColor  = p->color;
			p->startVector = p->vector;

			m_Particles.push_back(p);

			amount = 1;
		}
	}

	CParticleEmitter::emitt( 
		now, timeSinceLastCall, transformation, horiz_normalized, vert_normalized);

    return amount;
}

//---------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//---------------------------------------------------------------------------
