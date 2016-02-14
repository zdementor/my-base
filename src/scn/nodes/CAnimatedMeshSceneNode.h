//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CAnimatedMeshSceneNodeHPP
#define CAnimatedMeshSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/IAnimatedMeshSceneNode.h>
#include <scn/IAnimatedMesh.h>
#include <scn/IAnimatedMeshMD2.h>
#include <scn/IAnimatedMeshCal3d.h>
#include "BillHelper.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class IDummyTransformationSceneNode;

//---------------------------------------------------------------------------

class CAnimatedMeshSceneNode : public IAnimatedMeshSceneNode
{
public:

    CAnimatedMeshSceneNode(ISceneNode* parent, s32 id);
    virtual ~CAnimatedMeshSceneNode();

	//! IAnimatedMeshSceneNode implementation

	virtual const c8* getAnimationFileName(u32 idx)
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationFileName(idx) : 0; }
	virtual u32 getAnimationBeginFrame(u32 idx)
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationBeginFrame(idx) : 0; }
	virtual u32 getAnimationEndFrame(u32 idx)
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationEndFrame(idx) : 0; }
	virtual f32 getAnimationSpeed(u32 idx)
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationSpeed(idx) : 0; }
	virtual bool getAnimationLoopMode(u32 idx)
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationLoopMode(idx) : 0; }

	virtual void setAnimationBeginFrame(u32 idx, u32 frame)
	{ m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->setAnimationBeginFrame(idx, frame) : 0; }
	virtual void setAnimationEndFrame(u32 idx, u32 frame)
	{ m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->setAnimationEndFrame(idx, frame) : 0; }
	virtual void setAnimationSpeed(u32 idx, f32 speed)
	{ m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->setAnimationSpeed(idx, speed) : 0; }
	virtual void setAnimationLoopMode(u32 idx, bool looped)
	{ m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->setAnimationLoopMode(idx, looped) : 0; }

    virtual u32 getAnimationOveralFramesCount()
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getOveralFramesCount() : 0; }

	//! ISceneNode implementation

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);
    virtual void OnPostRender(u32 timeMs);

    virtual const core::aabbox3df& getBoundingBox() const
	{ return m_BoundingBox; }

    virtual u32 getAnimationsCount()
	{ return m_CurrentAnimatedMesh ? m_CurrentAnimatedMesh->getAnimationsCount() : 0; }
    virtual bool setCurrentAnimation(u32 anim, f32 delay=0.0f);
    virtual u32 getCurrentAnimation()
	{ return (u32)m_CurrentAnimation; }
	virtual bool setCurrentAnimationProgress(f32 progress)
	{ return false; }
	virtual f32 getCurrentAnimationProgress();

    virtual void setAnimationOveralSpeed(f32 speed);
    virtual f32 getAnimationOveralSpeed();

	virtual bool isAnimationOveralSmooth();
	virtual void setAnimationOveralSmooth(bool flag);

	virtual void setAnimationCallback(IAnimationCallback *callback);

    virtual IMesh* getMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);
	virtual IMesh* getMesh(f32 frame, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);

	virtual void setAnimatedMesh(IAnimatedMesh* amesh, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);

	virtual IAnimatedMesh* getAnimatedMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);

	virtual void setPhysicalMesh(IAnimatedMesh* mesh);

	virtual IAnimatedMesh* getPhysicalMesh();

	virtual const c8* getPhysicalMeshFullFileName();

	virtual bool rescaleNormals(const core::vector3df& new_scale);   

	virtual bool attachToBone(ISceneNode* node, const c8 *bone_name); 

	virtual ISceneNode* attachImposter(
		const SImposterCreationParameters &creation_params,
		const SImposterParameters &params,
		bool recreate_if_exists = false);

	virtual bool set(ISceneNode* other);

	virtual const c8* getSceneCorePropertiesXMLString();

private:

	core::aabbox3d < f32 > m_BoundingBox;

	IAnimatedMesh * m_PhysicalMesh;
    IAnimatedMesh * m_CurrentAnimatedMesh;

	IAnimatedMesh* m_AnimatedMeshes[E_LOD_LEVEL_COUNT];
    
    s32 m_CurrentAnimation;

    //! last frame time
    u32 m_LastTickMs;	

	SAnimationState	m_AnimationState;

	E_LOD_LEVEL m_CurrentMeshLOD;

	vid::SLight m_PersonalLightForLighting, m_PersonalLightForShadowCalculation;

	class CMeshAnimationCallback : public IMeshAnimationCallback
	{
		IAnimationCallback	*Callback;
		ISceneNode			*SceneNode;

	public:

		CMeshAnimationCallback()
			: Callback(0), SceneNode(0) {}
		CMeshAnimationCallback(ISceneNode *node)
			: Callback(0), SceneNode(node) {}
		CMeshAnimationCallback(IAnimationCallback *callback, ISceneNode *node)
			: Callback(callback), SceneNode(node) {}

		virtual ~CMeshAnimationCallback()
		{ setCallback(NULL); }

		virtual void OnAnimationBegin(u32 animation_idx)
		{
			if (Callback)
				Callback->OnAnimationBegin(SceneNode, animation_idx);
		}

		virtual void OnAnimationEnd(u32 animation_idx)
		{
			if (Callback)
				Callback->OnAnimationEnd(SceneNode, animation_idx);
		}

		void setCallback(IAnimationCallback* callback)
		{
			SAFE_DROP(Callback);
			Callback = callback;
			SAFE_GRAB(Callback);
		}

		void setSceneNode(ISceneNode* node)
		{ SceneNode = node; }
	};

	CMeshAnimationCallback *m_MeshAnimationCallback;

	bool m_ImposterOn;
	bool m_CastShadowFromPersonalLight;
	bool m_SmoothAnimation;	

	struct SAttachedNodeMapping
	{
		SAttachedNodeMapping() : Id(-1), Node(0) {}
		SAttachedNodeMapping(s32 id, IDummyTransformationSceneNode* node) : Id(id), Node(node) {}

		s32 Id;
		IDummyTransformationSceneNode* Node;
	};

	core::array < SAttachedNodeMapping > m_AttachedDummyNodes;
	core::array < IDummyTransformationSceneNode * > m_SkeletonBones;

	core::stringc m_AnimatedMeshFullFileName[E_LOD_LEVEL_COUNT], m_PhysicalMeshFullFileName;

	core::array <vid::IRenderBuffer*> m_SolidRenderBuffers, m_TransparentRenderBuffers;
	core::array <const vid::SMaterial*> m_SolidMaterials, m_TransparentMaterials;

	IMesh* _updateMeshGeometry(E_LOD_LEVEL lodlevel, bool full_update);
	IMesh* _updateGeometry(u32 timeMs);
	void _clearBones();
	void _createBones();

	f32 m_AlphaInterpolation;
	u32 m_StartSwitchingMs;

	static u32 s_ProfilePreRender, s_ProfileAnimation;

	bool m_UseFakeShadow;

	scn::ISceneManager &m_SceneManager;
	vid::IVideoDriver &m_VideoDriver;
	dev::IDevice &m_Device;
	res::IResourceManager &m_ResourceManager;

	u32 m_EnabledLights;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

