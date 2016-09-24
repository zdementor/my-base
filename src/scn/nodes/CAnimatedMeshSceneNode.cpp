//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CAnimatedMeshSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <vid/S3DVertex.h>
#include <scn/ICameraSceneNode.h>
#include <scn/IAnimatedMeshCal3d.h>
#include <scn/IDummyTransformationSceneNode.h>
#include <game/IGameManager.h>
#include <os/ITimer.h>
#include <dev/IDevice.h>
#include <scn/SMesh.h>
#include <scn/ILightGridManager.h>
#include <game/IGameNodeAI.h>
#include <scn/IBillboardSceneNode.h>
#include <scn/ICullingSystem.h>
#include <io/IFileSystem.h>
#include <res/IResourceManager.h>
#include <img/IImageLibrary.h>
#include <io/IXMLWriter.h>
#include <dev/IProfiler.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

u32 CAnimatedMeshSceneNode::s_ProfilePreRender = 0;
u32 CAnimatedMeshSceneNode::s_ProfileAnimation = 0;

//---------------------------------------------------------------------------

CAnimatedMeshSceneNode::CAnimatedMeshSceneNode( ISceneNode* parent, s32 id )
	: IAnimatedMeshSceneNode(ESNT_ANIMATED_MESH_SCENE_NODE, parent, id), 
	m_CurrentAnimatedMesh(0), m_LastTickMs(0), m_CurrentAnimation(0), 
	m_CurrentMeshLOD(ELL_LOD_NEAREST), m_ImposterOn(false),
	m_CastShadowFromPersonalLight(true), m_SmoothAnimation(true), m_PhysicalMesh(0),
	m_PhysicalMeshFullFileName(NONAME_FILE_NAME),
	m_AlphaInterpolation(0), m_StartSwitchingMs(0), m_UseFakeShadow(false),
	m_SceneManager(SCENE_MANAGER), m_VideoDriver(VIDEO_DRIVER), m_Device(DEVICE),
	m_ResourceManager(RESOURCE_MANAGER),
	m_EnabledLights(0)
{  
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CAnimatedMeshSceneNode");
#endif

	IsVisible = true;

	m_MeshAnimationCallback = new CMeshAnimationCallback(this);

	for (s32 ll=0; ll<E_LOD_LEVEL_COUNT; ll++)
		m_AnimatedMeshFullFileName[ll] = NONAME_FILE_NAME;

	memset(m_AnimatedMeshes, 0, sizeof(m_AnimatedMeshes));

	if (!s_ProfilePreRender)
		s_ProfilePreRender = PROFILER.allocProfileId("Pre Render animated mesh");
	if (!s_ProfileAnimation)
		s_ProfileAnimation = PROFILER.allocProfileId("Animation mesh");
}

//---------------------------------------------------------------------------

CAnimatedMeshSceneNode::~CAnimatedMeshSceneNode()
{
	setAnimationCallback(NULL);

	SAFE_DROP(m_MeshAnimationCallback);
    
	m_CurrentAnimatedMesh = 0;	

	for (u32 am = 0; am < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); am++)
		SAFE_DROP(m_AnimatedMeshes[am]);

	for (u32 an=0; an < m_AttachedDummyNodes.size(); an++)
		SAFE_DROP(m_AttachedDummyNodes[an].Node);
	m_AttachedDummyNodes.clear();

	_clearBones();
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		m_Culled = !IsVisible || CULLING_SYSTEM.isCull(this);

		if (!m_Culled && isOccluder())
			ICullingSystem::getSingleton().addOccluder(this);

		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::OnPreRender(u32 timeMs)
{
	ICameraSceneNode		*camera	= m_SceneManager.getActiveCamera();
	const core::vector3df	&campos	= m_SceneManager.getActiveCameraPosition();

	if (!camera) return;

	PROFILER.startProfiling(s_ProfilePreRender);

	// store initial 0 light params

	vid::SLight old_light_shvolume = m_VideoDriver.getDynamicLight(0);

	m_PersonalLightForShadowCalculation = old_light_shvolume;

	// update geometry from current animation

	PROFILER.startProfiling(s_ProfileAnimation);

	E_LOD_LEVEL old_lodlevel = m_CurrentMeshLOD;

	IMesh *mesh = _updateGeometry(timeMs);

	PROFILER.stopProfiling(s_ProfileAnimation);

	if (!m_CurrentAnimatedMesh)
		return;

	if (m_AlphaInterpolation > 0.0f && m_Imposter)
	{
		m_BoundingBox = m_CurrentAnimatedMesh->getBoundingBox();
		core::vector3df bbox_center = m_BoundingBox.getCenter();	

		ICameraSceneNode *cam = m_SceneManager.getActiveCamera();
		if (cam)
		{		
			core::vector3df vec = cam->getAbsolutePosition();
			core::matrix4 inv_abs = getAbsoluteTransformation();
			inv_abs.makeInversed();					
			inv_abs.transformVect(vec);	
			vec.normalize();
			vec *= m_ImposterParameters->ViewOffset;

			core::vector3df shift = m_ImposterParameters->ShiftCustom + vec;
			if (m_ImposterParameters->Centered)
				shift += bbox_center;				
			else
				shift += m_ImposterParameters->ShiftBBCenter;

			m_Imposter->setPosition(shift);
			m_Imposter->setScale(getScale());
			m_Imposter->updateAbsoluteTransformation();
		}
		m_Imposter->OnPreRender(timeMs);
	}

	if (m_AlphaInterpolation < 1.0f)
	{
		s32 curr_time_ms = DEVICE.getDeviceTime();

		m_BoundingBox = m_CurrentAnimatedMesh->getBoundingBox();

		// update geometry
		if (m_CurrentAnimatedMesh->getOveralFramesCount() > 1 ||
				old_lodlevel != m_CurrentMeshLOD)
			m_CurrentAnimatedMesh->uploadToRenderBuffers(this);

		// register geometry for rendering
		m_SolidMaterials.set_used(0);
		m_TransparentMaterials.set_used(0);
		m_SolidRenderBuffers.set_used(0);
		m_TransparentRenderBuffers.set_used(0);
		u32 rbc = getRenderBuffersCount();
		ISceneNode * parent = getParent();
		game::IGameNode * parent_gnode = parent ? parent->getGameNode() : NULL;
		dyn::IDynamicObject * dobj = getDynamicObject();

		for (u32 i = 0; i < rbc; ++i)
		{
			vid::IRenderBuffer * rb = getRenderBuffer(i);
			vid::SMaterial & mat = getMaterial(i);
			
			mat.update(curr_time_ms);

			if (mat.isTransparent())
			{
				m_TransparentMaterials.push_back(&mat);
				m_TransparentRenderBuffers.push_back(rb);
			}
			else
			{
				m_SolidMaterials.push_back(&mat);
				m_SolidRenderBuffers.push_back(rb);
			}
		}

		f32 distance = (f32)(getTransformedBoundingBox().getCenter().getDistanceFrom(campos));

		if (m_TransparentRenderBuffers.size())
		{
			u32 rbcnt = m_TransparentRenderBuffers.size();
			for (u32 i = 0; i < rbcnt; i++)
			{
				m_VideoDriver.registerPoolForRendering(
					isPotencialOccluder() ?
						vid::ERP_3D_TRANSP_3RD_PASS : vid::ERP_3D_TRANSP_2ND_PASS,
					AbsoluteTransformation,	&m_PersonalLightForLighting, m_EnabledLights,
					m_TransparentMaterials,	m_TransparentRenderBuffers, distance);
			}
		}

		if (m_SolidRenderBuffers.size())
			m_VideoDriver.registerPoolForRendering(
				vid::ERP_3D_SOLID_PASS,
				AbsoluteTransformation, &m_PersonalLightForLighting, m_EnabledLights,
				m_SolidMaterials, m_SolidRenderBuffers);
	}

	bool debug = isDebugDataVisible();

	if (debug)
	{
		m_VideoDriver.register3DBoxForRendering(
			vid::ERP_3D_SOLID_PASS, getAbsoluteTransformation(),
			getBoundingBox(), img::SColor(0xffffffff));
	}

	if (m_Shadow)
	{
		m_PersonalLightForShadowCalculation =
			m_PersonalLightForLighting;
		m_PersonalLightForShadowCalculation.CastShadows =
			m_CastShadowFromPersonalLight;
		m_Shadow->registerForRendering(m_UseFakeShadow, timeMs,
			m_PersonalLightForShadowCalculation, m_EnabledLights,
			m_CurrentAnimatedMesh, mesh, debug);
	}

	ISceneNode::OnPreRender(timeMs);

	PROFILER.stopProfiling(s_ProfilePreRender);
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::OnPostRender(u32 timeMs)
{ 
	if (m_AlphaInterpolation > 0.0f && m_Imposter)
		m_Imposter->OnPostRender(timeMs);

	ISceneNode::OnPostRender(timeMs);

	m_TransformedBoundingBox = getBoundingBox();
	AbsoluteTransformation.transformBox(m_TransformedBoundingBox);
}

//---------------------------------------------------------------------------

bool CAnimatedMeshSceneNode::setCurrentAnimation(u32 anim, f32 delay)
{
	if (m_CurrentAnimatedMesh)
		m_CurrentAnimatedMesh->setMeshAnimationCallback(m_MeshAnimationCallback);

    m_CurrentAnimation = anim;
	for (u32 am = 0; am < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); am++)
	{
		IAnimatedMesh *amesh = m_AnimatedMeshes[am];
		if (!amesh)
			continue;
		amesh->setAnimationOveralSmooth(m_SmoothAnimation);
		amesh->setCurrentAnimation(anim, delay);
		amesh->getAnimationState(m_AnimationState);
	}
	IBillboardSceneNode *bill = SAFE_CAST_TO_BILLBOARD_SCENE_NODE(m_Imposter);
	if (bill)
		bill->setCurrentAnimation(anim);

	if (m_CurrentAnimatedMesh)
		m_CurrentAnimatedMesh->setMeshAnimationCallback(NULL);
    return true;
}

//---------------------------------------------------------------------------

f32 CAnimatedMeshSceneNode::getCurrentAnimationProgress()
{
	f32 progress = 0.0f;
	if (m_AnimationState.AnimationDurationSec > 0)
	{
		progress = m_AnimationState.AnimationTimeSec /
			m_AnimationState.AnimationDurationSec;
		CHECK_RANGE(progress, 0.0f, 1.0f);
	}
	return progress;
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::setAnimationOveralSpeed(f32 speed)
{    
	for (u32 am = 0; am < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); am++)
	{
		IAnimatedMesh *amesh = m_AnimatedMeshes[am];
		if (!amesh)
			continue;
		amesh->setAnimationOveralSpeed(speed);
	}
}

//---------------------------------------------------------------------------

f32 CAnimatedMeshSceneNode::getAnimationOveralSpeed()
{
	for (u32 am = 0; am < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); am++)
	{
		IAnimatedMesh *amesh = m_AnimatedMeshes[am];
		if (!amesh)
			continue;
		return amesh->getAnimationOveralSpeed();
	}
	return 0.0f;
}

//---------------------------------------------------------------------------

bool CAnimatedMeshSceneNode::isAnimationOveralSmooth() 
{ return m_SmoothAnimation; }

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::setAnimationOveralSmooth(bool flag) 
{ m_SmoothAnimation = flag; }

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::setAnimationCallback(IAnimationCallback* callback)
{
	m_MeshAnimationCallback->setCallback(callback);
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshSceneNode::getMesh(E_LOD_LEVEL lodlevel)
{
	return _updateMeshGeometry(lodlevel, true);
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshSceneNode::_updateGeometry(u32 timeMs)
{
	// for personal lighting from lightgrid position		

	ISceneNode* top_parent = this;
	ISceneNode* root = m_SceneManager.getRootSceneNode();
	while (top_parent->getParent() != root)
		top_parent = top_parent->getParent();

	core::aabbox3df top_parent_absbb = top_parent->getTransformedBoundingBox();
	core::vector3df top_parent_absbb_center = top_parent_absbb.getCenter();

	m_PersonalLightForLighting = 
		LIGHT_GRID_MANAGER.getLightFromPos(top_parent_absbb_center);

	m_PersonalLightForLighting.Identificator = this;	
	m_PersonalLightForLighting.CastShadows = false;

	core::vector3df globalPosition = m_VideoDriver.getGlobalLightParams().Position;
	globalPosition.normalize();

	CHECK_MIN_RANGE(m_PersonalLightForLighting.Position.Y, 0.75);
	m_PersonalLightForLighting.Position.normalize();
	m_PersonalLightForLighting.Position =
		m_PersonalLightForLighting.Position.getInterpolated(globalPosition, 0.5f);

	if (m_PersonalLightForLighting.DiffuseColor.r>0.06f ||
		m_PersonalLightForLighting.DiffuseColor.g>0.06f ||
		m_PersonalLightForLighting.DiffuseColor.b>0.06f)
	{
		m_CastShadowFromPersonalLight = true;
	}
	else if (
		m_PersonalLightForLighting.DiffuseColor.r<0.05f &&
		m_PersonalLightForLighting.DiffuseColor.g<0.05f &&
		m_PersonalLightForLighting.DiffuseColor.b<0.05f)
	{
		m_CastShadowFromPersonalLight = false;
	}

	// Lighting paramaeters

	bool lighted = false;
	const core::array < vid::SLight > & dlights = m_VideoDriver.getDynamicLights();
	u32 dl_count = dlights.size();

	img::SColorf diffMatIntegral(0.0f, 0.0f, 0.0f);
	img::SColorf ambMatIntegral(0.0f, 0.0f, 0.0f);
	img::SColorf specMatIntegral(0.0f, 0.0f, 0.0f);

	u32 mcnt = getMaterialsCount();
	for (u32 i = 0; i < mcnt; i++)
	{
		const vid::SRenderPass &pass = getMaterial(i).getPass(0);
		const img::SColor &amb = pass.getAmbientColor();
		const img::SColor &diff = pass.getDiffuseColor();
		const img::SColor &spec = pass.getSpecularColor();

		ambMatIntegral.r += amb.getRed() / 255.0f;
		ambMatIntegral.g += amb.getGreen() / 255.0f;
		ambMatIntegral.b += amb.getBlue() / 255.0f;

		diffMatIntegral.r += diff.getRed() / 255.0f;
		diffMatIntegral.g += diff.getGreen() / 255.0f;
		diffMatIntegral.b += diff.getBlue() / 255.0f;

		specMatIntegral.r += spec.getRed() / 255.0f;
		specMatIntegral.g += spec.getGreen() / 255.0f;
		specMatIntegral.b += spec.getBlue() / 255.0f;
	}
	ambMatIntegral.r /= mcnt;
	ambMatIntegral.g /= mcnt;
	ambMatIntegral.b /= mcnt;

	diffMatIntegral.r /= mcnt;
	diffMatIntegral.g /= mcnt;
	diffMatIntegral.b /= mcnt;

	specMatIntegral.r /= mcnt;
	specMatIntegral.g /= mcnt;
	specMatIntegral.b /= mcnt;

	img::SColorf diffIntegral(0.0f, 0.0f, 0.0f);
	img::SColorf ambIntegral(0.0f, 0.0f, 0.0f);
	img::SColorf specIntegral(0.0f, 0.0f, 0.0f);

	m_EnabledLights = 0;

	for (u32 i = 0; i < dl_count && i < 32; i++)
	{
		f32 klight = 0.0f;
		const vid::SLight *light =
			(i == 0) ? &m_PersonalLightForLighting : &dlights[i];
		if (i == 0 && !light->Enabled)
			continue;
		else 
		if (light->Type == vid::ELT_POINT)
		{
			bool enabled = dlights[i].LightedVolume.intersectsWithBox(top_parent_absbb);
			if (enabled)
				m_EnabledLights |= (1 << i);
			if (enabled && !lighted)
			{
				lighted = true;
				klight = (top_parent_absbb_center - light->Position).getLengthSQ() /
					(light->Radius * light->Radius);
			}
		}
		else
		{
			klight = 1.0f;
			m_EnabledLights |= (1 << i);
		}

		diffIntegral.r += diffMatIntegral.r * light->DiffuseColor.r * klight;
		diffIntegral.g += diffMatIntegral.g * light->DiffuseColor.g * klight;
		diffIntegral.b += diffMatIntegral.b * light->DiffuseColor.b * klight;

		ambIntegral.r += ambMatIntegral.r * light->AmbientColor.r * klight;
		ambIntegral.g += ambMatIntegral.g * light->AmbientColor.g * klight;
		ambIntegral.b += ambMatIntegral.b * light->AmbientColor.b * klight;

		specIntegral.r += specMatIntegral.r * light->SpecularColor.r * klight;
		specIntegral.g += specMatIntegral.g * light->SpecularColor.g * klight;
		specIntegral.b += specMatIntegral.b * light->SpecularColor.b * klight;
	}

	CHECK_MAX_RANGE(diffIntegral.r, 1.0f);
	CHECK_MAX_RANGE(diffIntegral.g, 1.0f);
	CHECK_MAX_RANGE(diffIntegral.b, 1.0f);

	CHECK_MAX_RANGE(ambIntegral.r, 1.0f);
	CHECK_MAX_RANGE(ambIntegral.g, 1.0f);
	CHECK_MAX_RANGE(ambIntegral.b, 1.0f);

	CHECK_MAX_RANGE(specIntegral.r, 1.0f);
	CHECK_MAX_RANGE(specIntegral.g, 1.0f);
	CHECK_MAX_RANGE(specIntegral.b, 1.0f);

	// Adaptive detail degradation

	m_UseFakeShadow = false;
	E_LOD_LEVEL lodlevel = ELL_LOD_NEAREST;
	if (!IsMainPlayer)
	{
		ICameraSceneNode *camera = m_SceneManager.getActiveCamera();
		const core::vector3df &posNode = getAbsolutePosition();
		const core::vector3df &posCam  = camera->getPosition();

		core::line3d<f32> line(posCam, posNode); 
		f32 distance = (f32)line.getLength();

		lodlevel = m_SceneManager.getLODFromDistance(distance, m_CurrentMeshLOD);

		f32 fake_shadow_dist = (m_SceneManager.getLODDistance(ELL_LOD_NEAR) +
			m_SceneManager.getLODDistance(ELL_LOD_FAR)) / 2;
		if (distance > fake_shadow_dist)
			m_UseFakeShadow = true;
	}

	// m_Imposter update

	bool imposterOn = m_ImposterOn;
	if (!m_Imposter ||
		lodlevel <= ELL_LOD_NEAR || (
			lodlevel <= ELL_LOD_FAR &&
			m_VideoDriver.getFrameFilteredRenderTimeSec() < 0.04)) // 25 fps
		imposterOn = false;
	else
	if (lodlevel > ELL_LOD_FAR || (
			lodlevel > ELL_LOD_NEAR &&
			m_VideoDriver.getFrameFilteredRenderTimeSec() > 0.0666)) // 15 fps
		imposterOn = true;
	if (m_ImposterOn != imposterOn)
	{
		m_StartSwitchingMs = timeMs;
		m_ImposterOn = imposterOn;
	}

	if (m_ImposterOn)
		m_AlphaInterpolation += 0.05f * (timeMs - m_StartSwitchingMs) / 1000.f;
	else
		m_AlphaInterpolation -= 0.05f * (timeMs - m_StartSwitchingMs) / 1000.f;
	CHECK_RANGE(m_AlphaInterpolation, 0.0f, 1.0f);

	if (m_ImposterOn || (m_Imposter && m_Shadow))
		m_Imposter->setCurrentAnimationProgress(getCurrentAnimationProgress());

	if (m_AlphaInterpolation > 0.0f)
	{
		IBillboardSceneNode* bill = SAFE_CAST_TO_BILLBOARD_SCENE_NODE(m_Imposter);
		if (bill)
		{
			s32 r = (diffIntegral.r + ambIntegral.r) * 255.0f;
			s32 g = (diffIntegral.g + ambIntegral.g) * 255.0f;
			s32 b = (diffIntegral.b + ambIntegral.b) * 255.0f;

			CHECK_MAX_RANGE(r, 255.0f);
			CHECK_MAX_RANGE(g, 255.0f);
			CHECK_MAX_RANGE(b, 255.0f);

			img::SColor bill_color(
				m_AlphaInterpolation * 255.0f + 0.5f,r,g,b);					
			
			bill->setColor(bill_color);
		}
	}

	// Animated mesh update

	return _updateMeshGeometry(lodlevel, m_AlphaInterpolation < 1.0f);
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshSceneNode::_updateMeshGeometry(E_LOD_LEVEL lodlevel, bool full_update)
{
	m_CurrentAnimatedMesh = m_AnimatedMeshes[lodlevel];
	if (!m_CurrentAnimatedMesh)
	{
		E_LOD_LEVEL ll = lodlevel;
		while (!m_CurrentAnimatedMesh && ll > ELL_LOD_NEAREST)
		{
			ll = (E_LOD_LEVEL)((s32)ll - 1);
			m_CurrentAnimatedMesh = m_AnimatedMeshes[ll];
		}
		while (!m_CurrentAnimatedMesh && ll < ELL_LOD_FARTHEST)
		{
			ll = (E_LOD_LEVEL)((s32)ll + 1);
			m_CurrentAnimatedMesh = m_AnimatedMeshes[ll];
		}
		lodlevel = ll; 
	}

	if (!m_CurrentAnimatedMesh)
		return NULL;

	m_CurrentAnimatedMesh->setMeshAnimationCallback(m_MeshAnimationCallback);

	s32 now = TIMER.getTime();

	if (m_LastTickMs == 0)
		m_LastTickMs = now;		

	m_CurrentAnimatedMesh->setAnimationOveralSmooth(m_SmoothAnimation);
	m_CurrentAnimatedMesh->setAnimationState(m_AnimationState);
	m_CurrentAnimatedMesh->updateMesh(now - m_LastTickMs);
	m_CurrentAnimatedMesh->getAnimationState(m_AnimationState);

	IMesh *mesh = NULL;

	if (full_update)		
		mesh = m_CurrentAnimatedMesh->getMesh();

	if (m_CurrentAnimatedMesh->getAnimatedMeshType() == EMT_CAL3D)
	// .cal3d
	{    		
		// update all skeleton childs

		u32 bone_size = m_SkeletonBones.size();

		static core::matrix4 transformation;

		for (u32 i = 0; i < bone_size; ++i)
		{
			transformation = m_AnimationState.Bones[i].Rotation.getMatrix();	
			transformation.setTranslation(m_AnimationState.Bones[i].Translation);

			m_SkeletonBones[i]->setRelativeTransformation(transformation);
			m_SkeletonBones[i]->getBoundingBox() = m_AnimationState.Bones[i].BBox;
		}

		for (u32 an=0; an<m_AttachedDummyNodes.size(); an++)
		{
			m_AttachedDummyNodes[an].Node->setRelativeTransformation(
				m_CurrentAnimatedMesh->getMapedBoneTransformation(
					m_AttachedDummyNodes[an].Id));
		}
	}	

	m_CurrentAnimatedMesh->setMeshAnimationCallback(NULL);

	m_CurrentMeshLOD = lodlevel;
	
	m_LastTickMs = now;

	return mesh;
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshSceneNode::getMesh(f32 frame, E_LOD_LEVEL lodlevel)
{
	m_CurrentAnimatedMesh = m_AnimatedMeshes[lodlevel];
	if (!m_CurrentAnimatedMesh)
		return NULL;
	m_CurrentMeshLOD = lodlevel;
	return m_CurrentAnimatedMesh->getMesh(frame);
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::setAnimatedMesh(IAnimatedMesh* amesh, E_LOD_LEVEL lodlevel) 
{
	if (!amesh)
	{
		if (m_CurrentAnimatedMesh == m_AnimatedMeshes[lodlevel])
			m_CurrentAnimatedMesh = NULL;
		SAFE_DROP(m_AnimatedMeshes[lodlevel]);
		m_AnimatedMeshes[lodlevel] = NULL;
		return;
	}
	else
	{
		SAFE_DROP(m_AnimatedMeshes[lodlevel]);
		m_AnimatedMeshes[lodlevel] = m_CurrentAnimatedMesh = amesh;	
		m_AnimatedMeshFullFileName[lodlevel] = m_SceneManager.findMeshFullFileName(amesh);
		bool use_tbn = DEVICE.getDeviceFlagValue(dev::EDCF_SHADERS_HIGH_QUALITY);
		if (use_tbn)
			amesh->calculateTangents();
	}
	SAFE_GRAB(m_AnimatedMeshes[lodlevel]);

	if (!m_CurrentAnimatedMesh)
		return;
	
	// setting initial animation

	setCurrentAnimation(0);
	m_CurrentAnimatedMesh->updateMesh(0);
	m_CurrentAnimatedMesh->getMesh(0);
	m_CurrentAnimatedMesh->getAnimationState(m_AnimationState);

	// updating dynamic object parameters

	m_CurrentAnimatedMesh->setCurrentAnimation(0);
	m_CurrentAnimatedMesh->recalculateBoundingBox();

	m_BoundingBox = m_CurrentAnimatedMesh->getBoundingBox();

	if (DynObject)
		DynObject->updateGeomData();

	if (lodlevel == scn::ELL_LOD_NEAREST)
	{
		clearRenderBuffers();
		clearMaterials();
		m_CurrentAnimatedMesh->uploadToRenderBuffers(this);
	}
}

//---------------------------------------------------------------------------

IAnimatedMesh* CAnimatedMeshSceneNode::getAnimatedMesh(E_LOD_LEVEL lodlevel)
{
	return m_AnimatedMeshes[lodlevel];
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::setPhysicalMesh(IAnimatedMesh* mesh)
{
	if (!mesh || mesh->getOveralFramesCount()<1)
		return;

	SAFE_DROP(m_PhysicalMesh);
	m_PhysicalMesh = mesh;
	SAFE_GRAB(m_PhysicalMesh);

	m_BoundingBox = m_PhysicalMesh->getMesh(0)->getBoundingBox();

	m_PhysicalMeshFullFileName = m_SceneManager.findMeshFullFileName(m_PhysicalMesh);

	// now we exclusively holding this mesh, until it needed
	ISceneManager::getSingleton().removeMesh(m_PhysicalMesh);
}

//---------------------------------------------------------------------------

IAnimatedMesh* CAnimatedMeshSceneNode::getPhysicalMesh()
{ return m_PhysicalMesh; }

//---------------------------------------------------------------------------

const c8* CAnimatedMeshSceneNode::getPhysicalMeshFullFileName()
{ return m_PhysicalMeshFullFileName.c_str(); }

//---------------------------------------------------------------------------

bool CAnimatedMeshSceneNode::rescaleNormals(const core::vector3df& new_scale)
{
	for (u32 i = 0; i < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); i++)
	{
		if (m_AnimatedMeshes[i])
			m_AnimatedMeshes[i]->rescaleAnimatedMeshNormals(new_scale);	
	}
	return true;
}

//---------------------------------------------------------------------------

bool CAnimatedMeshSceneNode::attachToBone(ISceneNode* node, const c8 *bone_name)
{
	if (!node || !m_CurrentAnimatedMesh)
	{
		LOGGER.log("Can't attach NULL node to the bone!", io::ELL_ERROR);
		return false;
	}

	if (m_SmoothAnimation)
	{
		// creating skeleton bone nodes
		_createBones();	
		for ( u32 b=0; b < m_SkeletonBones.size(); b++ )
		{
			if (core::stringw(m_SkeletonBones[b]->getName()) == core::stringw(bone_name))
			{
				node->setParent(m_SkeletonBones[b]);

				ISceneNode* parent = node->getParent();
				while (parent && parent!=this)
				{
					parent->setVisible(true);		
					parent = parent->getParent();
				}
				return true;
			}
		}
	}
	else
	{
		s32 bone_idx = -1;
		u32 amcnt = sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes);
		for (u32 am = 0; am < amcnt; am++)
		{
			if (!m_AnimatedMeshes[am])
				continue;
			const core::array<SSkeletonBone> &skel_bones = m_AnimatedMeshes[am]->getBones();
			u32 bonecnt = skel_bones.size();
			for (u32 i=0; i < bonecnt; i++)
			{
				if (skel_bones[i].Name == bone_name)
				{
					if (bone_idx == -1)
						bone_idx = i;
					else
					if (bone_idx != i)
					{
						bone_idx = -1;
						am = amcnt;
					}
					break;
				}
			}
		}
		if (bone_idx!=-1)
		{
			for (u32 am = 0; am < sizeof(m_AnimatedMeshes)/sizeof(*m_AnimatedMeshes); am++)
			{
				if (!m_AnimatedMeshes[am])
					continue;
				if (!m_AnimatedMeshes[am]->mapBone(bone_idx))
					bone_idx = -1;
			}
		}
		if (bone_idx!=-1)
		{
			IDummyTransformationSceneNode* dummy = 
				m_SceneManager.addDummyTransformationSceneNode(this);
			if (dummy)
			{
				SAFE_GRAB(dummy);
				node->setParent(dummy);
				m_AttachedDummyNodes.push_back(SAttachedNodeMapping(bone_idx, dummy));

				return true;
			}
		}
	}

	core::stringc msg;
	msg.sprintf("Can't find '%s' bone, to attach node to it!", bone_name);

	LOGGER.log(msg.c_str(), io::ELL_ERROR);

	return false;
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::_clearBones() 
{
	for (u32 i=0; i<m_SkeletonBones.size(); ++i)
		SAFE_DROP(m_SkeletonBones[i]);
	m_SkeletonBones.clear();
}

//---------------------------------------------------------------------------

void CAnimatedMeshSceneNode::_createBones()
{
	if (m_SkeletonBones.size()==0 &&
		m_CurrentAnimatedMesh->getAnimatedMeshType() == EMT_CAL3D )
	{		
		const core::array<SSkeletonBone> &skel_bones = m_CurrentAnimatedMesh->getBones();

		s32 bones_size = skel_bones.size();

		m_SkeletonBones.set_used(bones_size);
		
		for (s32 i=0; i<bones_size; ++i)
		{
			m_SkeletonBones[i] = 
				m_SceneManager.addDummyTransformationSceneNode(this);
			m_SkeletonBones[i]->setName(skel_bones[i].Name.c_str());
			m_SkeletonBones[i]->grab();
			m_SkeletonBones[i]->setVisible(false);
		}

		// creating bone nodes structure

		for (s32 i=0; i<bones_size; ++i)
		{
			const SSkeletonBone &skel_bone = skel_bones[i];
			if (skel_bone.ParentBoneId >=0 && skel_bone.ParentBoneId<bones_size)
			{
				IDummyTransformationSceneNode* parent = m_SkeletonBones[skel_bone.ParentBoneId];
				if (parent && m_SkeletonBones[i])
					m_SkeletonBones[i]->setParent(parent);
			}
			else
				m_SkeletonBones[i]->setParent(this);
		}
	} 
}

//---------------------------------------------------------------------------

ISceneNode* CAnimatedMeshSceneNode::attachImposter(
	const SImposterCreationParameters &creation_params,
	const SImposterParameters &params,
	bool recreate_if_exists) 
{
	// set animation	
			
	IAnimatedMesh *amesh = NULL;

	E_LOD_LEVEL lowest_lod = (E_LOD_LEVEL)(E_LOD_LEVEL_COUNT-1);

	for (s32 am=(E_LOD_LEVEL_COUNT-1); am>=0; am--)
	{
		lowest_lod = (E_LOD_LEVEL)am;
		amesh = getAnimatedMesh(lowest_lod);
		if (amesh)
			break;
	}

	if (!amesh)
		return 0;

	s32 prev_anim = getCurrentAnimation();

	setCurrentAnimation(0);

	SImposterParameters imposter_params = params;

	const s32 imp_views_cnt = creation_params.ViewsCount;	
	const core::vector2df imp_cam_view_scale = creation_params.CameraViewScale;
	const core::vector2df imp_size_scale = creation_params.SizeScale;
	const f32 imp_tex_res_frame = creation_params.TextureResolution;
	const f32 imp_frame_step = creation_params.AnimationFrameStep;
	
	scn::IBillboardSceneNode *imposter = NULL;

	ICameraSceneNode* old_act_camera = m_SceneManager.getActiveCamera();

	LOGGER.logInfo("Binding imposter for this scene node . . .");	
	
	core::stringc fname = "", xml_fname = "";
	const c8 *tmppath = m_ResourceManager.getMediaDirFull(res::EMT_TEMP_DATA);

	if (!params.getFileName())
	{
		fname = core::extractFileName(core::stringc(getFileName()));
		xml_fname = core::stringc().sprintf("%simposter_%s", tmppath, fname.c_str());
	}
	else
	{
		fname = core::extractFileName(core::stringc(params.getFileName()));
		xml_fname = params.getFileName();
	}

	if (!recreate_if_exists && FILE_SYSTEM.existFile(xml_fname.c_str()))
		imposter = (IBillboardSceneNode*)m_SceneManager.loadSceneNode(
			xml_fname.c_str(), false);

	core::aabbox3df bb        = getBoundingBox();
	core::vector3df bb_extend = bb.getExtend();
	core::vector3df bb_center = bb.getCenter();
	core::vector3df target    = bb_center;
	core::vector3df up(0, 1, 0);

	if (!imposter)
	{
		fname.replace_char('.', '_');
		core::stringc base_tex_fname = 
			core::stringc().sprintf("%simposter_%s", tmppath, fname.c_str());

		u32 color_mask = m_VideoDriver.getColorMask();

		m_VideoDriver.setColorMask(true, true, true, true);

		// creating imposter
		imposter = m_SceneManager.addBillboardSceneNode(0);

		img::SColor frame_bk_color = m_VideoDriver.getBackgroundColor();
		core::rect<s32> frame_view_port = m_VideoDriver.getViewPort();
		img::SColorf glob_amb_color = m_VideoDriver.getGlobalAmbientColor();

		f32 imp_cam_dist = 1000.0f;

		core::vector3df imp_cam_pos = bb_center+core::vector3df(1,0,0)*imp_cam_dist;
		imp_cam_pos.rotateXYByDegrees(creation_params.OptimalViewAngle, bb_center);
				
		f32 imp_ang_step = 360.0f / (f32)imp_views_cnt;

		core::matrix4 proj;			
						
		f32 w = core::math::Sqrt(bb_extend.X*bb_extend.X + bb_extend.Z*bb_extend.Z);
		f32 h = core::math::Sqrt(w * w + bb_extend.Y*bb_extend.Y);

		proj.buildProjectionMatrixOrthoLH(
			w*imp_cam_view_scale.X, h*imp_cam_view_scale.Y, 1.0f, 5000.0f);	
		
		img::SColor imp_bk_color( 0, 0, 0, 0 );

		core::dimension2di max_tex_size = m_VideoDriver.getMaximalTextureSize();

		const s32 imp_tex_res_arr[]={256,512,1024,2048,4096};	
		const s32 imp_tex_res_cnt = sizeof(imp_tex_res_arr)/sizeof(*imp_tex_res_arr);
		s32 imp_tex_pitch_arr[imp_tex_res_cnt];
		s32 imp_tex_in_one_arr[imp_tex_res_cnt];
		f32 imp_tex_tcwidth_arr[imp_tex_res_cnt];
		for (s32 i=0; i<imp_tex_res_cnt; i++)
		{
			imp_tex_pitch_arr[i]  = (f32)imp_tex_res_arr[i]/imp_tex_res_frame;
			imp_tex_in_one_arr[i] = imp_tex_pitch_arr[i]*imp_tex_pitch_arr[i];
			imp_tex_tcwidth_arr[i]  = 1.0f / ((f32)imp_tex_res_arr[i]/imp_tex_res_frame);
		}
		s32 imp_tex_size_idx = imp_tex_res_cnt-1;
		for (s32 i=imp_tex_size_idx; i>=0; i--)
		{
			if ((imp_tex_res_arr[i] < imp_tex_res_frame
					&& max_tex_size.Width >= imp_tex_res_arr[i]
					&& max_tex_size.Height >= imp_tex_res_arr[i])
				|| imp_tex_res_arr[imp_tex_size_idx]>m_VideoDriver.getScreenSize().Width
				|| imp_tex_res_arr[imp_tex_size_idx]>m_VideoDriver.getScreenSize().Height)
				imp_tex_size_idx=i;
			else
				break;
		}
		s32 imp_tex_res     = imp_tex_res_arr[imp_tex_size_idx];
		s32 imp_tex_in_one  = imp_tex_in_one_arr[imp_tex_size_idx];
		s32 imp_tex_pitch   = imp_tex_pitch_arr[imp_tex_size_idx];
		f32 imp_tex_tcwidth = imp_tex_tcwidth_arr[imp_tex_size_idx];
		s32 imp_tex_in_one_cnt = 0;

		LOGGER.logInfo("Choosed Render Target size={%dx%d}",
			imp_tex_res, imp_tex_res);

		scn::SBillboardParams billparams;
		billparams.Size.set(
			w * imp_size_scale.X * imp_cam_view_scale.X,
			h * imp_size_scale.Y * imp_cam_view_scale.Y);
		billparams.FrameResolution.set(
			creation_params.TextureResolution,
			creation_params.TextureResolution);
		billparams.AxisAligmentType = EAAT_BLEND_CAM_POS;
		billparams.AlignAxis.set(0,1,0);
		billparams.FlipWidth  = false;
		billparams.FlipHeight = false;

		vid::SMaterial imp_mat;
		imp_mat.getPass(0).setAlphaTest ( vid::EAT_GE_80 );
		imp_mat.getPass(0).Layers[0].setTexCoordAddr ( vid::ETCAM_CLAMP );
		imp_mat.getPass(0).setBlendFuncs(vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
		imp_mat.setFlag(vid::EMF_FOG_ENABLE, true);   	
		imp_mat.setFlag(vid::EMF_BLENDING, true);

		imposter->setAnimationFramesDirectionOrderCCW(true);
		imposter->setBillboardParameters(billparams);	
		imposter->setMaterial(imp_mat);				
		
		m_VideoDriver.setBackgroundColor(imp_bk_color);		

		vid::SLight light0;

		light0.DiffuseColor.set ( 1.0f,  1.0f,  1.0f  );
		light0.AmbientColor.set ( 0.3f, 0.3f, 0.3f );
		light0.SpecularColor.set( 0.75f,  0.75f,  0.75f  );

		light0.Position.set(1.0f, 1000.0f, 1.0f);
		light0.Type = vid::ELT_DIRECTIONAL;
		light0.Enabled = true;

		m_VideoDriver.deleteAllDynamicLights();
		m_VideoDriver.addDynamicLight(light0);
		m_VideoDriver.setDynamicLightEnabled(0, true);

		m_VideoDriver.setGlobalAmbientColor( img::SColorf(0.5f, 0.5f, 0.5f) );
				
		s32 anim_cnt = getAnimationsCount();		
					
		s32 imp_tex_cnt = 0;
		s32 imp_frames_cnt = 0;
		s32 imp_frames_ovr_num = 0;	

		f32 ani_ovr_spd = amesh->getAnimationOveralSpeed();

		s32 a=0;

		if (anim_cnt==0)	a=-1;
		else				a= 0;

		for (; a < anim_cnt; a++)
		{
			s32 start = 0, end = 0;
			if (a >= 0)
			{
				setCurrentAnimation(a);
				start = getCurrentAnimationBeginFrame();
				end   = getCurrentAnimationEndFrame();
			}
			for (s32 v = 0; v < imp_views_cnt; v++)
			{
				for (f32 f = start; s32(f * 1000.0) <= end * 1000;
					f += imp_frame_step, imp_frames_ovr_num++) {}
			}
		}

		vid::ITexture *rtt = 0;
		u32 rtt_idx = 0;
		core::stringc rtt_name;

		if (anim_cnt==0)	a=-1;
		else				a= 0;

		u32 rbcnt = getRenderBuffersCount();
		bool rtt_loaded = false;

		core::array < SBillTexture > bill_textures;

		// creating bill frames
		for (; a < anim_cnt; a++)
		{	
			s32 start = 0, end = 0;
			if (a>=0)
			{
				setCurrentAnimation(a);
				start	= getCurrentAnimationBeginFrame();
				end		= getCurrentAnimationEndFrame();
			}
			else
				a=0;

			f32 fps = f32(end - start + 1) / f32(imp_frame_step);
			f32 time_per_frame = 1000.0f *
				(m_AnimationState.AnimationDurationSec / (fps > 0 ? fps : 0.001f));

			bool looped = m_AnimationState.Looped;					

			SAnimationFrameParameters params;
			params.TimePerFrame = time_per_frame;
			params.Loop = looped;

			for (s32 v = 0; v < imp_views_cnt; v++)
			{
				s32 imp_tex_num = 0;						

				f32 f = start;
				for (; f * 1000.0f <= (f32)(end * 1000);
					f += imp_frame_step, imp_tex_num++);
				f = start;

				for (s32 it = 0; it < imp_tex_num; f += imp_frame_step, it++)
				{	
					IMesh * mesh = getMesh(f, lowest_lod);
					if (!mesh)
						continue;	

					s32 xoffset = (imp_tex_in_one_cnt%imp_tex_in_one) % imp_tex_pitch;
					s32 yoffset = (imp_tex_in_one_cnt%imp_tex_in_one) / imp_tex_pitch;

					if (imp_tex_in_one_cnt % imp_tex_in_one == 0)
					{
						imp_tex_in_one_cnt = 0;

						s32 imp_tex_left = imp_frames_ovr_num-imp_frames_cnt;
						imp_tex_res		= imp_tex_res_arr		[imp_tex_size_idx];
						imp_tex_in_one	= imp_tex_in_one_arr	[imp_tex_size_idx];
						imp_tex_pitch	= imp_tex_pitch_arr		[imp_tex_size_idx];	
						imp_tex_tcwidth	= imp_tex_tcwidth_arr	[imp_tex_size_idx];									

						rtt_name.sprintf("%simposter_%s_%03d_%03d.tga",
							tmppath, fname.c_str(), v, a);

						rtt = m_VideoDriver.findTexture(rtt_name.c_str());

						if (rtt)
						{
							rtt_loaded = true;
							rtt->grab();
							LOGGER.logInfo("Use render target texture '%s'", rtt_name.c_str());
						}
						else
						{
							rtt_loaded = false;
							rtt = m_VideoDriver.createRenderTargetTexture(
								core::dimension2di(imp_tex_res, imp_tex_res),
								m_VideoDriver.getBackColorFormat());	
							m_VideoDriver.setColorRenderTarget(NULL);
							m_VideoDriver.setColorRenderTarget(rtt, true, true, imp_bk_color);
						}
						imp_tex_cnt++;

						bill_textures.push_back(SBillTexture(rtt_name.c_str(), rtt, rtt_loaded));
					}

					s32 row = imp_tex_in_one_cnt/imp_tex_pitch;
					s32 col = imp_tex_in_one_cnt%imp_tex_pitch;							
					
					core::rectf tcrect(0,0, imp_tex_tcwidth,imp_tex_tcwidth);
					core::recti frrect(0,0,imp_tex_res_frame-1, imp_tex_res_frame-1);
					core::recti frrect_rend = frrect;

					if (m_VideoDriver.getDriverFamily() == vid::EDF_OPENGL)
					// This vertical inversion is needed due to special order of pixel data in
					// OpenGL frame buffer we reading from
					{
						frrect += core::position2di(imp_tex_res_frame*col,
							imp_tex_res - imp_tex_res_frame*(row+1));
						tcrect += core::position2df(imp_tex_tcwidth*col,
							1.0f - imp_tex_tcwidth*(row+1));
					}
					else
					{
						frrect += core::position2di(imp_tex_res_frame*col,
							imp_tex_res_frame*row);
						tcrect += core::position2df(imp_tex_tcwidth*col,
							imp_tex_tcwidth*row   );
					}

					frrect_rend += core::position2di(imp_tex_res_frame*col,
						imp_tex_res_frame*row);

					if (!rtt_loaded)
					{
						core::matrix4 view;
						core::vector3df pos = imp_cam_pos;		
						
						pos.rotateXZByDegrees(v*imp_ang_step, target);

						view.buildViewMatrixLH(pos, target, up);

						m_VideoDriver.setViewPort(frrect_rend);

						m_VideoDriver.setTransform(vid::ETS_PROJ,	core::matrix4());
						m_VideoDriver.setTransform(vid::ETS_VIEW,	core::matrix4());
						m_VideoDriver.setTransform(vid::ETS_MODEL,	core::matrix4());

						m_VideoDriver.applyDynamicLights();

						m_VideoDriver.setTransform(vid::ETS_PROJ,	proj);
						m_VideoDriver.setTransform(vid::ETS_VIEW,  view);

						mesh->uploadToRenderBuffers(this);
						
						for (u32 i = 0; i < rbcnt; ++i)
						{
							vid::IRenderBuffer * rb = getRenderBuffer(i);
							vid::SMaterial & mat = getMaterial(i);

							u32 passcnt = mat.getPassesCount();
							for ( u32 p = 0; p < passcnt; p++ )
							{
								vid::SRenderPass pass = mat.getPass(p);

								pass.setAmbientColor (creation_params.AmbientColor);
								pass.setDiffuseColor (creation_params.DiffuseColor);
								pass.setSpecularColor(creation_params.SpecularColor);

								m_VideoDriver.setRenderPass(pass);
								m_VideoDriver.draw(rb);
							}
						}			
						LOGGER.logInfo(" Rendered %d frame [%d, %d] "
							"( anim %d, view %d, subframe %d, "
							"tc={%.4f,%.4f,%.4f,%.4f}, vp={%d,%d,%d,%d} )", 
							imp_frames_cnt, row, col, a, v, it,
							tcrect.Left, tcrect.Top, tcrect.Right, tcrect.Bottom,
							frrect.Left, frrect.Top, frrect.Right, frrect.Bottom);
					}

					bill_textures.get_last().frames.push_back(
						SBillAnimationFrame(a, v, it, params, NULL, tcrect, frrect));

					imp_frames_cnt++;
					imp_tex_in_one_cnt++;

					s32 df = end-f;

					if (df > 0 && df < imp_frame_step)
						f = end - imp_frame_step;
				}
			}
		}
		m_VideoDriver.setColorRenderTarget(NULL);

		u32 tcnt = bill_textures.size();
		for (u32 t = 0; t < tcnt; t++)
			bill_textures[t].createImageFromRTT();

		// recalculating bound rects for all bill frames and
		// saving billboard animation textures

		tcnt = bill_textures.size();
		for (u32 t = 0; t < tcnt; t++)
		{
			if (bill_textures[t].loaded || !bill_textures[t].frames.size())
				continue;
			img::IImage * image = bill_textures[t].image;
			if (!image)
				continue;
			u32 frcnt =  bill_textures[t].frames.size();
			for (u32 f = 0; f < frcnt; f++)
			{
				img::SColor transp_col;
				core::recti fr_rect = bill_textures[t].frames[f].TCoordsPix;
				core::recti bound_rect = fr_rect;
				bool first = true;

				bool clear_line = true;
				for (s32 i = fr_rect.UpperLeftCorner.X;
					clear_line && i <= fr_rect.LowerRightCorner.X; i++)
				{
					for (s32 j = fr_rect.UpperLeftCorner.Y;
						j <= fr_rect.LowerRightCorner.Y; j++)
					{
						if (first)
						{
							transp_col = image->getPixel(i, j);
							first = false;
						}
						if (transp_col != image->getPixel(i, j))
						{
							clear_line = false;
							break;
						}
					}
					if (clear_line)
						bound_rect.UpperLeftCorner.X++;
				}
				clear_line = true;
				for (s32 i = fr_rect.LowerRightCorner.X;
					clear_line && i >= fr_rect.UpperLeftCorner.X; i--)
				{
					for (s32 j = fr_rect.UpperLeftCorner.Y;
						j <= fr_rect.LowerRightCorner.Y; j++)
					{
						if (transp_col != image->getPixel(i, j))
						{
							clear_line = false;
							break;
						}
					}
					if (clear_line)
						bound_rect.LowerRightCorner.X--;
				}
				clear_line = true;
				for (s32 j = fr_rect.UpperLeftCorner.Y;
					clear_line && j <= fr_rect.LowerRightCorner.Y; j++)
				{
					for (s32 i = bound_rect.UpperLeftCorner.X;
						i <= bound_rect.LowerRightCorner.X; i++)
					{
						if (transp_col != image->getPixel(i, j))
						{
							clear_line = false;
							break;
						}
					}
					if (clear_line)
						bound_rect.UpperLeftCorner.Y++;
				}
				clear_line = true;
				for (s32 j = fr_rect.LowerRightCorner.Y;
					clear_line && j >= fr_rect.UpperLeftCorner.Y; j--)
				{
					for (s32 i = bound_rect.UpperLeftCorner.X;
						i <= bound_rect.LowerRightCorner.X; i++)
					{
						if (transp_col != image->getPixel(i, j))
						{
							clear_line = false;
							break;
						}
					}
					if (clear_line)
						bound_rect.LowerRightCorner.Y--;
				}
				bound_rect.UpperLeftCorner.X--;
				bound_rect.UpperLeftCorner.Y--;
				bound_rect.LowerRightCorner.X++;
				bound_rect.LowerRightCorner.Y++;

				CHECK_RANGE(bound_rect.UpperLeftCorner.X,
					fr_rect.UpperLeftCorner.X, fr_rect.LowerRightCorner.X);
				CHECK_RANGE(bound_rect.UpperLeftCorner.Y,
					fr_rect.UpperLeftCorner.Y, fr_rect.LowerRightCorner.Y);
				CHECK_RANGE(bound_rect.LowerRightCorner.X,
					bound_rect.UpperLeftCorner.X, fr_rect.LowerRightCorner.X);
				CHECK_RANGE(bound_rect.LowerRightCorner.Y,
					bound_rect.UpperLeftCorner.Y, fr_rect.LowerRightCorner.Y);

				bill_textures[t].frames[f].TOffsetPix.set(
					bound_rect.UpperLeftCorner.X - fr_rect.UpperLeftCorner.X,
					bound_rect.UpperLeftCorner.Y - fr_rect.UpperLeftCorner.Y);
				bill_textures[t].frames[f].TCoordsPix = bound_rect;
			}
		}

		// compressing billboard animation textures

		core::array < SBillTexture > bill_textures_compr;
		core::array < SBillAnimationFrame > bill_frames_compr;
		core::dimension2di bill_tex_max_size(m_VideoDriver.getMaximalTextureSize().Width/2,
			m_VideoDriver.getMaximalTextureSize().Height/2);

		compressBillfoardsTextures(creation_params.Compress,
			base_tex_fname.c_str(), bill_tex_max_size,
			bill_textures, &bill_textures_compr, &bill_frames_compr);

		// creating textures

		tcnt = bill_textures_compr.size();
		for (u32 t = 0; t < tcnt; t++)
		{
			img::IImage * image = bill_textures_compr[t].image;
			u32 frcnt =  bill_textures_compr[t].frames.size();
			if (!image || !frcnt)
				continue;
			image->smoothingFilter(false, false, false, true);
			IMAGE_LIBRARY.saveImageToFile(image, bill_textures_compr[t].fname.c_str());
		}

		// attaching animation textures to the billboard

		SAnimationFrameParameters params;
		core::array < SAnimationFrame > dir_frames;
		core::array < SBillAnimationFrame > & frames = bill_frames_compr;
		u32 frcnt = frames.size();
		u32 anim = 0, view = 0;

		for (u32 f = 0; f < frcnt; f++)
		{
			if (anim != frames[f].animation ||
				view != frames[f].direction)
			{
				if (dir_frames.size())
					imposter->setAnimationDirectionSetFrames(
						anim, view, dir_frames, params);
				dir_frames.set_used(0);
			}
			anim	= frames[f].animation;
			view	= frames[f].direction;
			params	= frames[f].params;

			dir_frames.push_back(*((SAnimationFrame*)&frames[f]));
		}	
		if (dir_frames.size())
			imposter->setAnimationDirectionSetFrames(
				anim, view, dir_frames, params);
		dir_frames.clear();

		m_VideoDriver.setGlobalAmbientColor(glob_amb_color);
		m_VideoDriver.setBackgroundColor(frame_bk_color);
		m_VideoDriver.setViewPort(frame_view_port);

		m_VideoDriver.setColorMask(color_mask);

		LOGGER.logInfo(" Created %d imposter textures (%d frames, %d views).",
			bill_textures_compr.size(), imp_frames_cnt, imp_views_cnt);

		m_SceneManager.saveSceneNode(xml_fname.c_str(), imposter);
	}

	imposter_params.ShiftBBCenter = bb_center;

	ISceneNode::attachImposter(imposter, creation_params, imposter_params);

	setCurrentAnimation(prev_anim);

	m_SceneManager.setActiveCamera(old_act_camera);

	return getImposter(); 
}

//---------------------------------------------------------------------------

bool CAnimatedMeshSceneNode::set(ISceneNode *other_scene_node)
{
	IAnimatedMeshSceneNode *other =
		SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(other_scene_node);
	if (!other || this == other)
		return false;

	for (s32 am=0; am<E_LOD_LEVEL_COUNT; am++)
		setAnimatedMesh(
			other->getAnimatedMesh((E_LOD_LEVEL)am), (E_LOD_LEVEL)am);

	other->getAnimatedMesh(ELL_LOD_NEAREST); 

	setAnimationOveralSmooth(other->isAnimationOveralSmooth());
	setAnimationOveralSpeed(other->getAnimationOveralSpeed());

	ISceneNode *imposter = other->getImposter();
	if (imposter)
	{
		ISceneNode* cloned_imposter = 
			m_SceneManager.cloneSceneNode(imposter);
		if (cloned_imposter)
		{
			const SImposterParameters *imp_params = 
				other->getImposterParameters();
			const SImposterCreationParameters *imp_creation_params = 
				other->getImposterCreationParameters();

			ISceneNode::attachImposter(
				cloned_imposter, *imp_creation_params, *imp_params);
		}
	}

	IShadow *shadow = other->getShadow();
	if (shadow)
		attachShadow(shadow->getShadowType());

	return true;
}

//---------------------------------------------------------------------------

const c8* CAnimatedMeshSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	IAnimatedMesh *amesh2 = getAnimatedMesh(scn::ELL_LOD_FAR);
	IAnimatedMesh *amesh1 = getAnimatedMesh(scn::ELL_LOD_NEAR);
	IAnimatedMesh *amesh0 = getAnimatedMesh(scn::ELL_LOD_NEAREST);	

	if (getMaterialsCount() == 1 && (
			amesh0 && amesh0->getAnimatedMeshType() != scn::EMT_CAL3D))
	{
		bool with_textures = true;
		const core::list<scn::ISceneNodeAnimator*> &animators = getAnimators();
		core::list<scn::ISceneNodeAnimator*>::iterator it = animators.begin();
		for (; it != animators.end(); ++it)
		{
			scn::ISceneNodeAnimator *a = *it;
			if (a->getAnimatorType() == scn::ESNAT_TEXTURE_ANIMATOR)
			{
				with_textures = false;
				break;
			}
		}

		const c8* mat_xml = m_SceneManager.getMaterialXMLText(getMaterial(0), with_textures);
		xmlstr.append(mat_xml);
	}

	for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
	{
		IAnimatedMesh *amesh = getAnimatedMesh((E_LOD_LEVEL)i);
		if (amesh)
		{
			const c8 *full_fname = m_SceneManager.findMeshFullFileName(amesh);
			const c8 *fpath = core::extractFilePath(full_fname);
			const c8 *fname = core::extractFileName(full_fname);
			const c8 *fpathrel = core::getRelativePathBetweenDirectories(
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES), fpath);
			str.sprintf("<Mesh filename=\"%s%s\" animation_fps=\"%d\" lod=\"%d\" />\n",
				fpathrel, fname, amesh->getAnimationFPS(), i);
			xmlstr.append(str);
		}
	}

	IAnimatedMesh *amesh = NULL;
	E_LOD_LEVEL lod = ELL_LOD_NEAREST;
	while (!amesh && lod < E_LOD_LEVEL_COUNT)
	{
		amesh = getAnimatedMesh(lod);
		lod = (E_LOD_LEVEL)(lod + 1);
	}
	if (amesh)
	{
		u32 acnt = amesh->getAnimationsCount();
		if (acnt > 0)
		{
			str.sprintf("<Animation speed=\"%.2f\" smooth=\"%d\" >\n",
				getAnimationOveralSpeed(), isAnimationOveralSmooth());
			xmlstr.append(str);
			for (u32 i = 0; i < acnt; i++)
			{
				const c8* fullfname = amesh->getAnimationFileName(i);
				if (fullfname)
				{
					const c8* basepath = RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES);
					const c8* fpath = core::extractFilePath(fullfname);
					const c8* fname = core::extractFileName(fullfname);
					const c8* fpathrel = core::getRelativePathBetweenDirectories(basepath, fpath);
					str.sprintf("<Register filename=\"%s%s\" speed=\"%.2f\" loop=\"%d\" />\n",
						fpathrel, fname,
						amesh->getAnimationSpeed(i), amesh->getAnimationLoopMode(i));
				}
				else
				{
					str.sprintf("<Register start_frame=\"%d\" end_frame=\"%d\" "
						"speed=\"%.2f\" loop=\"%d\" />\n",
						amesh->getAnimationBeginFrame(i), amesh->getAnimationEndFrame(i),
						amesh->getAnimationSpeed(i), amesh->getAnimationLoopMode(i));
				}
				xmlstr.append(str);
			}
			str.sprintf("</Animation>\n");
			xmlstr.append(str);
		}
	}

	IShadow* shadow = getShadow();
	s32 shadow_on = shadow?1:0;
	E_SHADOW_TYPE shadow_type = shadow ? 
		shadow->getShadowType() : ESHT_SHADOW_FAKE;

	str.sprintf("<Shadow on=\"%d\" type=\"%s\" />\n",
		shadow_on, ShadowTypeStr[shadow_type]);
	xmlstr.append(str);

	if (getImposter())
	{
		str.sprintf(
			"<BindImposter shift_x=\"%.2f\" shift_y=\"%.2f\" shift_z=\"%.2f\" "
			"view_offset=\"%.2f\" centered=\"%d\" >\n", 
			m_ImposterParameters->ShiftCustom.X, 
			m_ImposterParameters->ShiftCustom.Y,
			m_ImposterParameters->ShiftCustom.Z,
			m_ImposterParameters->ViewOffset,
			m_ImposterParameters->Centered ? 1 : 0);
		xmlstr.append(str);

		if (m_ImposterParameters->getFileName())
		{
			const c8 *tex_fname_rel = RESOURCE_MANAGER.getMediaFileNameRel(
				res::EMT_XML_SCRIPTS, m_ImposterParameters->getFileName());
			str.sprintf("<FileName value=\"%s\" >\n", tex_fname_rel);
			xmlstr.append(str);
		}

		str.sprintf(
			"<CreationParams view_count=\"%d\" view_angle=\"%.f\" "
			"view_scale_width=\"%.2f\" view_scale_height=\"%.2f\"\n"
			"size_scale_width=\"%.2f\" size_scale_height=\"%.2f\" "
			"tex_res=\"%.2f\" frame_step=\"%.2f\"\n"
			"ambient_color=\"%d,%d,%d\" diffuse_color=\"%d,%d,%d\" "
			"specular_color=\"%d,%d,%d\"\n"
			"compress=\"%d\" />\n",
			m_ImposterCreationParameters->ViewsCount, 
			m_ImposterCreationParameters->OptimalViewAngle,
			m_ImposterCreationParameters->CameraViewScale.X,
			m_ImposterCreationParameters->CameraViewScale.Y,
			m_ImposterCreationParameters->SizeScale.X,
			m_ImposterCreationParameters->SizeScale.Y,
			m_ImposterCreationParameters->TextureResolution,
			m_ImposterCreationParameters->AnimationFrameStep,
			m_ImposterCreationParameters->AmbientColor.getRed(), 
			m_ImposterCreationParameters->AmbientColor.getGreen(), 
			m_ImposterCreationParameters->AmbientColor.getBlue(), 
			m_ImposterCreationParameters->DiffuseColor.getRed(), 
			m_ImposterCreationParameters->DiffuseColor.getGreen(), 
			m_ImposterCreationParameters->DiffuseColor.getBlue(), 
			m_ImposterCreationParameters->SpecularColor.getRed(), 
			m_ImposterCreationParameters->SpecularColor.getGreen(), 
			m_ImposterCreationParameters->SpecularColor.getBlue(),
			m_ImposterCreationParameters->Compress ? 1 : 0);
		xmlstr.append(str);

		str.sprintf("</BindImposter>\n");
		xmlstr.append(str);
	}

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

