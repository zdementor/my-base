//|-------------------------------------------------------------------------
//| File:        CParticleEmitter.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CParticleEmitter.h"

#include "CParticleAffectorFadeOut.h"
#include "CParticleAffectorGravity.h"

#include <vid/S3DVertex.h>
#include <vid/IVideoDriver.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

CParticleEmitter::CParticleEmitter(
	core::aabbox3d<f32> volume, core::vector3df direction,  
	u32 minParticlesPerSecond, u32 maxParticlePerSecond, 
	img::SColor minStartColor, img::SColor maxStartColor, 
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees		
	) : 
m_AppearVolume(volume), m_Direction(direction), 
m_MinParticlesPerSecond(minParticlesPerSecond),
m_MaxParticlesPerSecond(maxParticlePerSecond), 
m_MinStartColor(minStartColor), m_MaxStartColor(maxStartColor),
m_MinLifeTime(lifeTimeMin), m_MaxLifeTime(lifeTimeMax), 
m_MaxAngleDegrees(maxAngleDegrees),
m_ParticleSize(core::dimension2d<f32>(5.0f, 5.0f)),
m_Texture(0), m_Time(0), m_Emitted(0), m_Enabled(true),
m_GlobalParticlesTransformation(true),
m_GL(VIDEO_DRIVER.getDriverFamily() == vid::EDF_OPENGL)
{
	const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE_WHITE);

	setTexture(VIDEO_DRIVER.getTexture(tname));
}

//----------------------------------------------------------------------------

CParticleEmitter::~CParticleEmitter() 
{
	SAFE_DROP(m_Texture);

	removeAllAffectors();

	core::list<SParticle*>::iterator it = m_Particles.begin();
	for (; it!=m_Particles.end(); ++it)
		delete *it;
	m_Particles.clear();
}

//---------------------------------------------------------------------------

void CParticleEmitter::reallocateBuffers()
{
    if (m_Particles.size() * 4 > m_Vertices.size() ||
        m_Particles.size() * 6 > m_Indices.size())
    {
        s32 oldSize = m_Vertices.size();
        m_Vertices.set_used(m_Particles.size() * 4);

        u32 i;

        // fill remaining vertices
        for (i=oldSize; i<m_Vertices.size(); i+=4)
        {
            m_Vertices[0+i].TCoords.set(0.0f, 1.0f);
            m_Vertices[1+i].TCoords.set(0.0f, 0.0f);
            m_Vertices[2+i].TCoords.set(1.0f, 0.0f);
            m_Vertices[3+i].TCoords.set(1.0f, 1.0f);
        }

        // fill remaining indices
        s32 oldIdxSize = m_Indices.size();
        s32 oldvertices = oldSize;
        m_Indices.set_used(m_Particles.size() * 6);
        
        for (i=oldIdxSize; i<m_Indices.size(); i+=6)
        {
            m_Indices[0+i] = 0+oldvertices;
            m_Indices[1+i] = 2+oldvertices;
            m_Indices[2+i] = 1+oldvertices;
            m_Indices[3+i] = 0+oldvertices;
            m_Indices[4+i] = 3+oldvertices;
            m_Indices[5+i] = 2+oldvertices;
            oldvertices += 4;
        }
    }
}

//----------------------------------------------------------------------------

s32 CParticleEmitter::emitt(
	u32 now, u32 timediff, const core::matrix4 &transformation, 
	const core::vector3df& horiz_normalized, const core::vector3df& vert_normalized
	)
{
	// animate all particles

    f32 scale = (f32)timediff;

	core::list<SParticle*>::iterator it = m_Particles.begin();
	
	m_BoundingBox.reset(transformation.getTranslation());	

	for (; it!=m_Particles.end(); )
	{
		SParticle *p = *it;

		if (now > p->endTime || p->color.color == 0)
		{
			delete *it;
            it = m_Particles.erase(it);
		}
        else
        {   
			p->pos += (p->vector * scale);
			m_BoundingBox.addInternalPoint(p->pos);			
            ++it;
        }
	}

	f32 m = m_ParticleSize.Width > m_ParticleSize.Height ? m_ParticleSize.Width : m_ParticleSize.Height;
    m *= 0.5f;
    m_BoundingBox.MaxEdge.X += m;
    m_BoundingBox.MaxEdge.Y += m;
    m_BoundingBox.MaxEdge.Z += m;

    m_BoundingBox.MinEdge.X -= m;
    m_BoundingBox.MinEdge.Y -= m;
    m_BoundingBox.MinEdge.Z -= m;

	// apply all affectors

	core::list<IParticleAffector*>::iterator ait = m_AffectorList.begin();
	while (ait != m_AffectorList.end())
	{
		if ((*ait)->isEnabled())
		{
			(*ait)->affect(now, m_Particles);
		}
		++ait;
	}  		

	// reallocate arrays, if they are too small

	reallocateBuffers();
		
	// create particle vertex data	

	core::vector3df horizontal = horiz_normalized * 0.5f * m_ParticleSize.Width;
	core::vector3df vertical   = vert_normalized * 0.5f * m_ParticleSize.Height;

	core::vector3df horiz_plus_vert = horizontal + vertical;
	core::vector3df horiz_minus_vert = horizontal - vertical;

	s32 idx = 0;

	it = m_Particles.begin();

	for (; it!=m_Particles.end(); ++it)
	{
		SParticle* particle = *it;

		m_Vertices[idx  ].Pos    = particle->pos + horiz_plus_vert;
		m_Vertices[idx++].Color  = particle->color;

		m_Vertices[idx  ].Pos    = particle->pos + horiz_minus_vert;
		m_Vertices[idx++].Color  = particle->color;

		m_Vertices[idx  ].Pos    = particle->pos - horiz_plus_vert;
		m_Vertices[idx++].Color  = particle->color;

		m_Vertices[idx  ].Pos    = particle->pos - horiz_minus_vert;
		m_Vertices[idx++].Color  = particle->color;		
	}

	return 0;
}

//----------------------------------------------------------------------------

bool CParticleEmitter::isGlobalParticlesTransformation()
{
	return m_GlobalParticlesTransformation;
}

//----------------------------------------------------------------------------

const core::dimension2d<f32>& CParticleEmitter::getParticleSize()
{
	return m_ParticleSize;
}

//----------------------------------------------------------------------------

const core::vector3df& CParticleEmitter::getDirection()
{
	return m_Direction;
}

//----------------------------------------------------------------------------

u32 CParticleEmitter::getMinParticlePerSecond()
{
	return m_MinParticlesPerSecond;
}

//----------------------------------------------------------------------------

u32 CParticleEmitter::getMaxParticlePerSecond()
{
	return m_MaxParticlesPerSecond;
}

//----------------------------------------------------------------------------

const img::SColor& CParticleEmitter::getMinStartColor()
{
	return m_MinStartColor;
}

//----------------------------------------------------------------------------

const img::SColor& CParticleEmitter::getMaxStartColor()
{
	return m_MaxStartColor;
}

//----------------------------------------------------------------------------

u32 CParticleEmitter::getMinLifeTime()
{
	return m_MinLifeTime;
}

//----------------------------------------------------------------------------

u32 CParticleEmitter::getMaxLifeTime()
{
	return m_MaxLifeTime;
}

//----------------------------------------------------------------------------

s32 CParticleEmitter::getMaxAngleDegrees()
{
	return m_MaxAngleDegrees;
}

//----------------------------------------------------------------------------

const core::aabbox3d<f32>& CParticleEmitter::getAppearVolume()
{
	return m_AppearVolume;
}

//----------------------------------------------------------------------------

vid::ITexture* CParticleEmitter::getTexture()
{
	return m_Texture;	
}

//----------------------------------------------------------------------------

bool CParticleEmitter::isEnabled()
{
	return m_Enabled;
}

void CParticleEmitter::setGlobalParticlesTransformation(bool value)
{
	m_GlobalParticlesTransformation = value;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setParticleSize(const core::dimension2d<f32> &size)
{
	m_ParticleSize = size;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setDirection(const core::vector3df &dir)
{
	m_Direction = dir;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMinParticlePerSecond(u32 pps)
{
	m_MinParticlesPerSecond = pps;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMaxParticlePerSecond(u32 pps)
{
	m_MaxParticlesPerSecond = pps;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMinStartColor(const img::SColor &col)
{
	m_MinStartColor = col;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMaxStartColor(const img::SColor &col)
{
	m_MaxStartColor = col;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMinLifeTime(u32 lt)
{
	m_MinLifeTime = lt;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMaxLifeTime(u32 lt)
{
	m_MaxLifeTime = lt;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setMaxAngleDegrees(s32 ang)
{
	m_MaxAngleDegrees = ang;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setAppearVolume(const core::aabbox3d<f32> &volume)
{
	m_AppearVolume = volume;
}

//----------------------------------------------------------------------------

void CParticleEmitter::setTexture(vid::ITexture *tex)
{
	if (!tex)
	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE_WHITE);
		tex = VIDEO_DRIVER.getTexture(tname);
	}
	SAFE_DROP(m_Texture);
	m_Texture = tex;
	SAFE_GRAB(m_Texture);
}

//----------------------------------------------------------------------------

void CParticleEmitter::setEnabled(bool enabled)
{
	m_Enabled = enabled;
}

//----------------------------------------------------------------------------

bool CParticleEmitter::set(IParticleEmitter* pe)
{
	if (!pe || this == pe)
		return false;

	setParticleSize        (pe->getParticleSize());
	setDirection           (pe->getDirection()); 
	setMinParticlePerSecond(pe->getMinParticlePerSecond());
	setMaxParticlePerSecond(pe->getMaxParticlePerSecond());
	setMinStartColor       (pe->getMinStartColor());
	setMaxStartColor       (pe->getMaxStartColor());
	setMinLifeTime         (pe->getMinLifeTime());
	setMaxLifeTime         (pe->getMaxLifeTime());
	setMaxAngleDegrees     (pe->getMaxAngleDegrees());
	setAppearVolume        (pe->getAppearVolume());
	setTexture             (pe->getTexture());
	setGlobalParticlesTransformation(pe->isGlobalParticlesTransformation());

	removeAllAffectors();

	for (s32 i=0; i<pe->getAffectorsCount(); i++)
	{
		IParticleAffector* affector = pe->getAffector(i);
		addAffector(affector->getType())->set(affector);
	}

	return true;
}

//----------------------------------------------------------------------------

IParticleAffector* CParticleEmitter::addAffector(E_PARTICLE_AFFECTOR_TYPE atype)
{
	IParticleAffector* affector = 0;

	if (atype==EPAT_FADEOUT)
	{
		img::SColor targetColor = img::SColor(0,0,0,0);
		u32 timeNeededToFadeOut = 1000;

		affector = new CParticleFadeOutAffector(targetColor, timeNeededToFadeOut);
	}
	else 
	if (atype==EPAT_GRAVITY)
	{
		core::vector3df gravity = core::vector3df(0.0f,-0.03f,0.0f);
		u32 timeForceLost = 1000;

		affector = new CParticleGravityAffector(gravity, timeForceLost);
	}

	if (affector)
		m_AffectorList.push_back(affector);

	return affector;
}

//----------------------------------------------------------------------------

bool CParticleEmitter::removeAffector(s32 idx)
{
	if (idx < 0 || idx >= (s32)m_AffectorList.size())
		return false;

	core::list<IParticleAffector*>::iterator it = m_AffectorList.begin() + idx;

	(*it)->drop();
	m_AffectorList.erase(it);

	return true;
}

//----------------------------------------------------------------------------

void CParticleEmitter::removeAllAffectors()
{
	core::list<IParticleAffector*>::iterator it = m_AffectorList.begin();
	while (it != m_AffectorList.end())
	{
		(*it)->drop();
		it = m_AffectorList.erase(it);
	}
}

//----------------------------------------------------------------------------

IParticleAffector* CParticleEmitter::getAffector(s32 idx)
{
	return *(m_AffectorList.begin() + idx);    
}

//----------------------------------------------------------------------------

s32 CParticleEmitter::getAffectorsCount()
{
	return m_AffectorList.size();
}

//----------------------------------------------------------------------------

core::list<IParticleAffector*>& CParticleEmitter::getAffectorsList()
{
	return m_AffectorList;
}

//----------------------------------------------------------------------------

//!
vid::S3DVertex1TCoordsColoured* CParticleEmitter::getVertices()
{
	return m_Vertices.pointer();
}

//----------------------------------------------------------------------------

//!
u16* CParticleEmitter::getIndices()
{
	return m_Indices.pointer();
}

//----------------------------------------------------------------------------

//!
s32 CParticleEmitter::getParticlesCount()
{
	return m_Particles.size();
}

//----------------------------------------------------------------------------

const core::aabbox3d<f32>& CParticleEmitter::getBoundingBox()
{
	return m_BoundingBox;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------


