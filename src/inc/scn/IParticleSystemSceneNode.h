//|-------------------------------------------------------------------------
//| File:        IParticleSystemSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IParticleSystemSceneNodeHPP
#define IParticleSystemSceneNodeHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

#include <scn/ISceneNode.h>
#include "IParticleEmitter.h"
#include "IParticleAffector.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! A particle system scene node for creating snow, fire, exlosions, smoke...
class __MY_SCN_LIB_API__ IParticleSystemSceneNode : public ISceneNode
{
public:

    //! constructor
    IParticleSystemSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id) 
		: ISceneNode(type, parent, id) {}

     //! Sets the particle emitter type, which creates the particles.    
    virtual IParticleEmitter* addEmitter(E_PARTICLE_EMITTER_TYPE etype) = 0;	

	//! Removes particle affectors ny index. returns true on success
    virtual bool removeEmitter(s32 idx) = 0;

	//! return particle emiter
    virtual IParticleEmitter* getEmitter(s32 idx) = 0;

	//! return particle emitter count
	virtual s32 getEmittersCount() = 0;  

	//!
	virtual void setEnabled(bool enabled) = 0;

	//!
	virtual s32 getParticlesCount() = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

