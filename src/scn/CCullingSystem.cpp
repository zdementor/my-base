//|-------------------------------------------------------------------------
//| File:        CCullingSystem.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CCullingSystem.h"

#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>
#include <dyn/IDynamicManager.h>
#include <vid/IHardwareOcclusionQuery.h>
#include <scn/ILightSceneNode.h>
#include <game/IGameNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! constructor
CCullingSystem::CCullingSystem()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CCullingSystem");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CCullingSystem::~CCullingSystem()
{
}

//----------------------------------------------------------------------------

//! return true if this Scene Node is culled
bool CCullingSystem::isCull(ISceneNode* node) const
{
	if (!node)
		return true;

	E_CULLING_TESTS tests = node->getCullingFilter();

	if (tests == ECT_NO_CULL)
		return false;

	static core::aabbox3d<f32> tbox;

	if (node->getSceneNodeType() == ESNT_LIGHT_SCENE_NODE)
	{
		ILightSceneNode *light = static_cast<ILightSceneNode*>(node);

		vid::SLight lparams = light->getLightParameters();

		if (isCull(lparams.Position, lparams.Radius, tests))
			return true;

		tbox = lparams.LightedVolume;
	}
	else
	{
		tbox = node->getTransformedBoundingBox();	
	}

	return isCull(tbox, tests);
}

//----------------------------------------------------------------------------

//! return true if this Bounding box is culled
bool CCullingSystem::isCull(const core::aabbox3df &bb, E_CULLING_TESTS tests) const
{	
	if (tests == ECT_NO_CULL)
		return false;

	const core::vector3df		&cam_pos  = m_ViewerFrustrum.Position;
	const core::view_frustrum	&frustrum = m_ViewerFrustrum;

	// screen bounds culling	

	if (tests&ECT_SCREEN_BOUNDS_CULL)
	{
	}

	// frustrum culling

	if (tests&ECT_VIEW_FRUSTRUM_CULL)
	{	
		if (!bb.isPointInside(cam_pos) &&
			frustrum.classifyBoxRelation(bb)==core::EIREL3D_FRONT) 
				return true; 
	}

	// software occlusion culling

	if (tests&ECT_OCCLUDERS_FRUSTRUM_CULL)
	{	
		for (u32 o = 0; o < m_Occluders.size(); o++)
			if (!m_Occluders[o].isPointInside(cam_pos) &&
				m_Occluders[o].isOccludeByPlanes(bb))
				return true;	
	}

	// hardware occlusion culling

	if (tests&ECT_HARDWARE_OCCLUSION_CULL)
	{	
	}

	// dynamic manager culling

	if (tests&ECT_DYNAMIC_MANAGER_CULL)
	{
	}

	return false;
}

//----------------------------------------------------------------------------

//! return true if this 3D position is culled
bool CCullingSystem::isCull(const core::vector3df &pos, E_CULLING_TESTS tests) const
{
	if (tests == ECT_NO_CULL)
	{
		return false;
	}

	vid::IVideoDriver &driver = VIDEO_DRIVER;

	core::position2d<s32> scr_pos =
		driver.getScreenCoordinatesFrom3DPosition( pos );	

	const core::vector3df		&cam_pos  = m_ViewerFrustrum.Position;
	const core::view_frustrum	&frustrum = m_ViewerFrustrum;

	// screen bounds culling	

	if (tests&ECT_SCREEN_BOUNDS_CULL)
	{
		const core::dimension2d<s32> &screenSize = driver.getScreenSize();
		
		f32 scr_height = (f32)screenSize.Height;
		f32 scr_width  = (f32)screenSize.Width; 
	
		if (scr_pos.X<0 || scr_pos.X>scr_width || scr_pos.Y<0 || scr_pos.Y>scr_height)
		{
			return true;
		}	
	}

	// frustrum culling

	if (tests&ECT_VIEW_FRUSTRUM_CULL)
	{	
		if (frustrum.classifyPointRelation(pos)==core::EIREL3D_FRONT) 
			return true; 
	}

	// occluders bottom volume culling

	if (tests&ECT_OCCLUDERS_BOTTOM_VOLUME_CULL)
	{
		for (u32 o=0; o<m_Occluders.size(); o++)
			if (!m_Occluders[o].isPointInside(cam_pos) &&
				m_Occluders[o].isOccludeByBottomVolume(pos))
				return true;
	}	

	// occluders frustrum culling

	if (tests&ECT_OCCLUDERS_FRUSTRUM_CULL)
	{	
		for (u32 o=0; o<m_Occluders.size(); o++)
			if (!m_Occluders[o].isPointInside(cam_pos) &&
				m_Occluders[o].isOccludeByPlanes(pos))
				return true;
	}

	// hardware occlusion culling

	if (tests&ECT_HARDWARE_OCCLUSION_CULL)
	{	
		if (driver.getDriverFamily() == vid::EDF_OPENGL)
		{			
			core::vector3df pos3d_from_framebuffer = 
				driver.get3DPositionFromScreenCoordinates(scr_pos);

			if (core::vector3df(pos3d_from_framebuffer-cam_pos).getLengthSQ()<
				(core::vector3df(pos-cam_pos).getLengthSQ())
				)
			{
				return true;
			}			
		}
		else
		if (driver.getHardwareOcclusionQuery().isOK())
		{
		}
	}

	// dynamic manager culling

	if (tests&ECT_DYNAMIC_MANAGER_CULL)
	{		
		core::line3d<f32> line(cam_pos, pos);	

		if (m_ViewerFrustrum.isOrthogonalProjection())
			line.start = pos-m_ViewerLookAtDirection;

		// fire dynamic manager culling
		if (DYNAMIC_MANAGER.checkIntersectionWithRay(
				line, 
				dyn::EDCF_MONOLITH_COLLID|dyn::EDCF_DYNAMIC_COLLID|dyn::EDCF_PSEUDO_DYNAMIC_COLLID
				)
			) 
		{
			return true; 
		}
	}

	return false;
}

//----------------------------------------------------------------------------

//! return true if this 3D sphere is culled
bool CCullingSystem::isCull(const core::vector3df &center, f32 radius, E_CULLING_TESTS tests) const
{
	if (tests == ECT_NO_CULL)
	{
		return false;
	}

	const core::vector3df		&cam_pos  = m_ViewerFrustrum.Position;
	const core::view_frustrum	&frustrum = m_ViewerFrustrum;

	// screen bounds culling	

	if (tests&ECT_SCREEN_BOUNDS_CULL) 
	{
	}

	// frustrum culling

	if (tests&ECT_VIEW_FRUSTRUM_CULL)
	{			
		if (frustrum.classifySphereRelation(center, radius)==core::EIREL3D_FRONT) 
			return true; 
	}

	// occluders bottom volume culling

	if (tests&ECT_OCCLUDERS_BOTTOM_VOLUME_CULL) 
	{
	}	

	// occluders frustrum culling

	if (tests&ECT_OCCLUDERS_FRUSTRUM_CULL)
	{	
		for (u32 o=0; o<m_Occluders.size(); o++)
			if (!m_Occluders[o].isPointInside(cam_pos) &&
				m_Occluders[o].isOccludeByPlanes(center, radius))
				return true;
	}

	// hardware occlusion culling

	if (tests&ECT_HARDWARE_OCCLUSION_CULL) 
	{
	}

	// dynamic manager culling

	if (tests&ECT_DYNAMIC_MANAGER_CULL) 
	{
	}

	return false;
}

//----------------------------------------------------------------------------

bool CCullingSystem::addOccluder(ISceneNode* node)
{	
	core::vector3df camera(0,0,0);
	bool orthogonal = m_ViewerFrustrum.isOrthogonalProjection();

	if (orthogonal)
		camera = m_ViewerLookAtDirection;
	else
		camera = m_ViewerFrustrum.Position;
		
	m_Occluders.push_back( SOccluderEntry(node, camera, orthogonal)	);	

	return true;
}

//----------------------------------------------------------------------------

s32 CCullingSystem::getOccludersCount()
{	
	return m_Occluders.size();
}

//----------------------------------------------------------------------------

void CCullingSystem::clearOccluders()
{
	m_Occluders.set_used(0);
}

//----------------------------------------------------------------------------

void CCullingSystem::initializeOccluders()
{
	static core::array<SOccluderEntry> occluders_tmp;
	occluders_tmp.set_used(0);

	m_Occluders.sort();

	for (u32 i=0; i<m_Occluders.size(); i++)
	{
		bool occluded = false;

		for (u32 j=i+1; j<m_Occluders.size(); j++)
		{
			if (m_Occluders[j].isOccludeByPlanes(m_Occluders[i].tbbox))
			{
				occluded = true;
				break;
			}
		}

		if (!occluded)
		{
			occluders_tmp.push_back(m_Occluders[i]);	
		}
	}

	m_Occluders.set_used(0);
	m_Occluders = occluders_tmp;
}

//----------------------------------------------------------------------------

void CCullingSystem::setViewer(const core::view_frustrum &frustrum, const core::vector3df &look_at)
{
	m_ViewerFrustrum = frustrum;
	m_ViewerLookAtDirection = look_at-m_ViewerFrustrum.Position;
}

//----------------------------------------------------------------------------

const core::view_frustrum& CCullingSystem::getViewerFrustrum()
{
	return m_ViewerFrustrum;
}

//----------------------------------------------------------------------------

const core::vector3df& CCullingSystem::getViewerLookAt()
{
	return m_ViewerLookAtDirection;
}

//----------------------------------------------------------------------------

ICullingSystem* createCullingSystem()
{
	return new CCullingSystem;
}

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace my
//----------------------------------------------------------------------------

