//|-------------------------------------------------------------------------
//| File:        CShadowVolume.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CShadowVolumeHPP
#define CShadowVolumeHPP
//---------------------------------------------------------------------------

#include "CShadowFake.h"
#include <vid/SLight.h>
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Scene node for rendering a shadow volume into a stencil buffer.
class CShadowVolume : public CShadowFake
{
public:

    CShadowVolume(ISceneNode* parent);

    virtual ~CShadowVolume();

    virtual E_SHADOW_TYPE getShadowType()
	{ return ESHT_SHADOW_VOLUME; }

	virtual void registerForRendering(
		bool fake_shadow, u32 nowMs,
		const vid::SLight &light0, u64 enabled_lights,
		IAnimatedMesh *amesh, IMesh *mesh, bool render_debug);

private:

    class CLightShadowVolume* _createShadowVolume(
		SDynamicLightEntry &light, IMesh *mesh, ISceneNode *owner, bool render_debug);

	void _registerGeometryForRendering(
		f32 now, IAnimatedMesh *amesh, IMesh *mesh, bool render_debug);

	u8				*m_MeshIndices;
    u16				*m_VertIndices;
    u16				*m_Edges;
    u16				*m_Adjacency;
    bool			*m_FaceData; 

    s32 m_IndexCount;
    s32 m_VertexCount;
    s32	m_EdgeCount;
    s32 m_IndexCountAllocated;

	bool m_JustCreated;

	bool m_FakeShadow;

	u32 m_StartSwitchingMs;
	f32 m_AnimationTimeSec;

	core::array <class CLightShadowVolume*> m_ShadowVolumeLights;
    s32 m_ShadowVolumeLightsUsed;

	static u32 s_ProfileShadowVolume;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CShadowVolumeHPP

