//|-------------------------------------------------------------------------
//| File:        CParticleSystemSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CParticleSystemSceneNodeHPP
#define CParticleSystemSceneNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IParticleSystemSceneNode.h>
#include <vid/S3DVertex.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! A particle system scene node.
//! A scene node controlling a particle System. The behavior of the particles
//! be controlling by setting the right particle emitters and effectors.
class CParticleSystemSceneNode : public IParticleSystemSceneNode
{
	friend class CSceneManager;

public:

    CParticleSystemSceneNode(ISceneNode* parent, s32 id);
    virtual ~CParticleSystemSceneNode();

	virtual void OnCheckCulling();

    virtual void OnPreRender(u32 timeMs);
	virtual void OnPostRender(u32 timeMs);

    virtual const core::aabbox3d<f32>& getBoundingBox() const
	{ return m_Box; }

	virtual bool set(ISceneNode* other);

    virtual IParticleEmitter* addEmitter(E_PARTICLE_EMITTER_TYPE etype);	

    virtual bool removeEmitter(s32 idx);

    virtual IParticleEmitter* getEmitter(s32 idx);

	virtual s32 getEmittersCount();   

	virtual void setEnabled(bool enabled);

	virtual s32 getParticlesCount();

	virtual const c8* getSceneCorePropertiesXMLString();

private:

    void _removeAllEmitters();    
    void _burn(u32 timeMs, 
		const core::vector3df& horiz_normalized, 
		const core::vector3df& vert_normalized);
    
    core::list<IParticleEmitter*> m_EmitterList;
    
    core::aabbox3df m_Box;
    u32 m_LastEmitTime;      

	bool m_LastVisibility, m_JustShowedUp;

	s32 m_TimeShift;  
	s32 m_ParticlesCount;

	bool m_Enabled;

	vid::SMaterial &m_Material;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

