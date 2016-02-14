//|-------------------------------------------------------------------------
//| File:        CParticleEmitterBox.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CParticleEmitterBox.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

CParticleBoxEmitter::CParticleBoxEmitter(
    core::aabbox3df box,
    core::vector3df direction,  u32 minParticlesPerSecond,
    u32 maxParticlePerSecond,   img::SColor minStartColor,
    img::SColor maxStartColor, u32 lifeTimeMin, u32 lifeTimeMax,
    s32 maxAngleDegrees)
		: CParticleEmitter(
			box, direction, 
			minParticlesPerSecond, maxParticlePerSecond,  
			minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax,
			maxAngleDegrees)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CParticleBoxEmitter");
#endif
}

//----------------------------------------------------------------------------

s32 CParticleBoxEmitter::emitt(
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
		f32 everyWhatMillisecond = 1000.0f / (perSecond==0 ? 0.000001f : perSecond);

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

			amount = (s32)((m_Time / everyWhatMillisecond) + 0.5f);
			m_Time = 0;
	        
			core::vector3df extend = m_AppearVolume.getExtend();
			
			if (amount > (s32)m_MaxParticlesPerSecond*2)
				amount = m_MaxParticlesPerSecond * 2;

			f64 len = m_Direction.getLength();

			for (s32 i = 0; i < amount; ++i)
			{
				SParticle *p = new SParticle();

				p->pos.X = m_AppearVolume.MinEdge.X + fmodf((f32)core::math::Random(), extend.X);
				p->pos.Y = m_AppearVolume.MinEdge.Y + fmodf((f32)core::math::Random(), extend.Y);
				p->pos.Z = m_AppearVolume.MinEdge.Z + fmodf((f32)core::math::Random(), extend.Z);

				transformation.transformVect(p->pos);

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
			}		
		}
	}
	
	CParticleEmitter::emitt( 
		now, timeSinceLastCall, transformation, horiz_normalized, vert_normalized);
		
	return amount;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
