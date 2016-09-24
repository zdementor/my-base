//|-------------------------------------------------------------------------
//| File:        CKTreeSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CKTreeSceneNode.h"

#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>
#include <scn/ICameraSceneNode.h>
#include <game/IGameManager.h>
#include <os/ITimer.h>
#include <dev/IDevice.h>
#include <img/IImageLibrary.h>
#include <io/IFileSystem.h>
#include <res/IResourceManager.h>
#include <scn/IBSPLevelMesh.h>
#include <scn/ILightGridManager.h>
#include <io/ILogger.h>

#include "KTree.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

s32 CKTreeSceneNode::m_LastTimeDirtUpdate=0;
core::queue<CKTreeSceneNode*> CKTreeSceneNode::m_DirtTextureUpdateQueue;

//---------------------------------------------------------------------------

CKTreeSceneNode::CKTreeSceneNode(ISceneNode* parent, s32 id) 
	: IKTreeSceneNode(parent, id),
m_KTree(0),	m_KTreeDirt(0), m_MaxTrianglesPerNode(4096), 
m_AnimatedMesh(0), m_PhysicalMesh(NULL), 
m_AnimatedMeshFullFileName(NONAME_FILE_NAME), 
m_PhysicalMeshFullFileName(NONAME_FILE_NAME)
{
#if MY_DEBUG_MODE
	IUnknown::setClassName("CKTreeSceneNode");
#endif
	m_StubRenderBuffer =
		VIDEO_DRIVER.createDynamicRenderBuffer(
			NULL, (vid::E_VERTEX_TYPE)0, 0,
			NULL, (E_INDEX_TYPE)0, 0,
			(vid::E_DRAW_PRIMITIVE_TYPE)0);
}

//---------------------------------------------------------------------------

CKTreeSceneNode::~CKTreeSceneNode()
{
	ILightGridManager::getSingleton().removeLightGrid(this);

	SAFE_DROP(m_AnimatedMesh);
    SAFE_DROP(m_KTree);
	SAFE_DROP(m_KTreeDirt);

	SAFE_DROP(m_PhysicalMesh);

	SAFE_DROP(m_StubRenderBuffer);
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		m_Culled = !IsVisible || CULLING_SYSTEM.isCull(this);
		
		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::OnPreRender(u32 timeMs)
{
	if (!m_KTree || !m_KTree->isOK())
		return;

	_completeDirtTasks(timeMs);

	scn::ISceneManager	&smgr	= SCENE_MANAGER;
	vid::IVideoDriver	&driver	= VIDEO_DRIVER;
	dev::IDevice		&device	= DEVICE;
	
	ICameraSceneNode*  camera = smgr.getActiveCamera();

    if (!camera) return;

	const core::matrix4 & abs_transf = getAbsoluteTransformation();
	core::matrix4 abs_inv = abs_transf;
	abs_inv.makeInversed();

	core::view_frustrum frust = camera->getViewFrustrum();

	E_CULLING_TESTS cull_tests = E_CULLING_TESTS(ECT_VIEW_FRUSTRUM_CULL);
	if (smgr.isOccludersCullingEnabled())
		cull_tests = E_CULLING_TESTS(cull_tests|ECT_OCCLUDERS_FRUSTRUM_CULL);

	// detecting visible geometry
	
	if (smgr.isOctTreeCullingEnabled())
		m_KTree->getVisibleRenderGeometry ( cull_tests, m_ViewedVBOMeshBuffers );

	// register render buffers

	m_SolidMaterials.set_used(0);
	m_TransparentMaterials.set_used(0);
	m_GlobalLightSolidMaterials.set_used(0);

	m_SolidRenderBuffers.set_used(0);
	m_TransparentRenderBuffers.set_used(0);
	m_GlobalLightSolidRenderBuffers.set_used(0);

	core::array<SRenderGeometry> * mb_arr = &m_ViewedVBOMeshBuffers;
	s32 mb_size = (*mb_arr).size();
	for ( s32 i = 0; i < mb_size; ++i )
	{
		IMeshBuffer *			mb	= (*mb_arr)[i].MeshBuffer;
		vid::IRenderBuffer *	rb	= (*mb_arr)[i].RenderBuffer;
		vid::SMaterial &		mat	= mb->getMaterial();

		mat.update(timeMs);

		bool lightmapped = mat.getPass(0).Layers[1].getTexture()
			&& mat.getPass(0).Layers[1].getType() == vid::ETLT_LIGHT_MAP;
		bool lighted = mat.getPass(0).getLightingMode() != vid::ELM_NONE;

		if (mat.isTransparent())
		{
			m_TransparentMaterials.push_back(&mat);
			m_TransparentRenderBuffers.push_back(rb);
		}
		else
		{
			if (!lightmapped && lighted)
			{
				m_GlobalLightSolidMaterials.push_back(&mat);
				m_GlobalLightSolidRenderBuffers.push_back(rb);
			}
			else
			{
				m_SolidMaterials.push_back(&mat);
				m_SolidRenderBuffers.push_back(rb);
			}
		}
	}

	// detecting lighted geometry

	m_DynamicLightRenderData.set_used(0);

	bool medLighing = device.getDeviceFlagValue(dev::EDCF_LIGHTING_MEDIUM_QUALITY);
	bool highLighing = device.getDeviceFlagValue(dev::EDCF_LIGHTING_HIGH_QUALITY);

	if (medLighing || highLighing)
	{
		const core::array < vid::SLight > & dlights = driver.getDynamicLights();
		u32 dl_count = dlights.size();
		u32 lcnt = 0;
		for (u32 i = 0; i < dl_count; i++)
		{
			if (!dlights[i].Enabled ||
				dlights[i].Type != vid::ELT_POINT ||
				(!dlights[i].Dynamic && !highLighing)
				) continue;
			if (lcnt++ >= 32)
				break;

			const core::aabbox3df &light_tbbox = dlights[i].LightedVolume;

			m_DynamicLightRenderData.push_back(SDynamicLightRenderData());
			SDynamicLightRenderData & static_rd =
				m_DynamicLightRenderData[m_DynamicLightRenderData.size()-1];

			static_rd.EnabledLightIdx = i;
			static_rd.RenderBuffers.set_used(0);
			static_rd.Materials.set_used(0);

			u32 vismb_cnt = m_ViewedVBOMeshBuffers.size();
			for (u32 vmb = 0; vmb < vismb_cnt; vmb++)
			{
				IMeshBuffer		*mb = m_ViewedVBOMeshBuffers[vmb].MeshBuffer;
				vid::SMaterial	&mat= mb->getMaterial();

				if (!mat.isTransparent() &&
					light_tbbox.intersectsWithBox(m_ViewedVBOMeshBuffers[vmb].TransformedBBox))
				{
					vid::IRenderBuffer	*rb = m_ViewedVBOMeshBuffers[vmb].RenderBuffer;
					static_rd.RenderBuffers.push_back(rb);
					static_rd.Materials.push_back(&mat);
				}
			}
			if (static_rd.RenderBuffers.size() == 0)
			{
				static_rd.RenderBuffers.push_back(m_StubRenderBuffer);
				static_rd.Materials.push_back(&m_StubMaterial);
			}
		}
	}

	// detecting dirty geometry

	if (smgr.isOctTreeCullingEnabled() &&
			DEVICE.getDeviceFlagValue(dev::EDCF_DIRT_ON_LEVEL))
	{
		// setting more strict culling fristrum for dirt geometry
		f32 z_far = camera->getFarValue();
		camera->setFarValue(z_far * 0.666f);
		core::view_frustrum dirt_cull_frust = camera->getViewFrustrum();
		camera->setFarValue(z_far);

		ICullingSystem &cullsys = ICullingSystem::getSingleton();
		const core::view_frustrum &cull_frust = cullsys.getViewerFrustrum();
		const core::vector3df &cull_lookat = cullsys.getViewerLookAt();
		cullsys.setViewer(dirt_cull_frust, cull_lookat);

		// getting visible dirty geometry
		if (m_KTreeDirt)
			m_KTreeDirt->getVisibleRenderGeometry(cull_tests, m_ViewedDirtMeshBuffers);	

		// restore old culling system cull settings
		cullsys.setViewer(cull_frust, cull_lookat);			
	}

	vid::SRenderPass pass;
	vid::STextureLayer layer_top, layer_front, layer_left;

	pass.setLightingMode(vid::ELM_NONE);
	pass.setFlag(vid::EMF_MIP_MAP_OFF,		false);
	pass.setFlag(vid::EMF_GOURAUD_SHADING,	false);
	pass.setFlag(vid::EMF_BLENDING,			true);
	pass.setFlag(vid::EMF_FOG_ENABLE,		true);
	pass.setFlag(vid::EMF_ZWRITE_ENABLE,	false);
	pass.setBlendFuncs(
		vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);

	layer_top.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);  		
	layer_top.setTexRGBScale(1);
	layer_top.setTexCoordAddr(vid::ETCAM_CLAMP);	

	layer_left.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
	layer_left.setTexRGBScale(1);
	layer_left.setTexCoordAddr(vid::ETCAM_CLAMP);	

	layer_front.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
	layer_front.setTexRGBScale(1);
	layer_front.setTexCoordAddr(vid::ETCAM_CLAMP);

	m_DirtRenderBuffers.set_used(0);
	m_DirtMaterials.set_used(0);

	mb_arr = &m_ViewedDirtMeshBuffers;
	mb_size = (*mb_arr).size();
	for (s32 i = 0; i < mb_size; ++i)
	{
		SRenderGeometry		&rend	= (*mb_arr)[i];
		IMeshBuffer			*mb		= (*mb_arr)[i].MeshBuffer;
		vid::IRenderBuffer	*rb		= (*mb_arr)[i].RenderBuffer;
		vid::SMaterial		&mat	= mb->getMaterial();		
		core::vector3df		center	= rend.TreeNodeTransformedBBoxCenter;

		f32 xext = rend.TreeNodeTransformedBBoxExtend.X;
		f32 yext = rend.TreeNodeTransformedBBoxExtend.Y;
		f32 zext = rend.TreeNodeTransformedBBoxExtend.Z;

		layer_top.setProjectionParams(zext, xext, center,
			core::vector3df(0,-1,0), core::vector3df(-1,0,0));
		layer_left.setProjectionParams(zext, yext, center,
			core::vector3df(1,0,0), core::vector3df(0,-1,0));
		layer_front.setProjectionParams(xext, yext, center,
			core::vector3df(0,0,-1), core::vector3df(0,-1,0));

		layer_top.setTexture(rend.DirtTextureTop);
		layer_left.setTexture(rend.DirtTextureLeft);
		layer_front.setTexture(rend.DirtTextureFront);

		pass.Layers[0] = layer_top;
		pass.Layers[1] = layer_left;
		pass.Layers[2] = layer_front;

		pass.setDepthTest(vid::ECT_LEQUAL);//vid::ECT_GEQUAL);

		mat.getPass(0) = pass;

		m_DirtMaterials.push_back(&mat);
		m_DirtRenderBuffers.push_back(rb);
	}

	if (m_DirtRenderBuffers.size())
		driver.registerPoolForRendering(vid::ERP_3D_DIRT_PASS,
			abs_transf, NULL, NULL, m_DirtMaterials, m_DirtRenderBuffers);
	if (m_TransparentRenderBuffers.size())
		driver.registerPoolForRendering(vid::ERP_3D_TRANSP_1ST_PASS,
			abs_transf, NULL, NULL, m_TransparentMaterials, m_TransparentRenderBuffers);
	if (m_SolidRenderBuffers.size())
		driver.registerPoolForRendering(vid::ERP_3D_SOLID_PASS,
			abs_transf, NULL, NULL, m_SolidMaterials, m_SolidRenderBuffers);

	if (m_GlobalLightSolidRenderBuffers.size())
	{
		const vid::SLight &light0 = driver.getGlobalLightParams();
		if (light0.Enabled)
		{
			driver.registerPoolForRendering(vid::ERP_3D_SOLID_PASS,
				abs_transf, &light0, 1, m_GlobalLightSolidMaterials, m_GlobalLightSolidRenderBuffers);
		}
	}

	if (m_DynamicLightRenderData.size())
	{
		u32 sdata_cnt = m_DynamicLightRenderData.size();
		for (u32 i = 0; i < sdata_cnt; i++)
		{
			SDynamicLightRenderData &sdata = m_DynamicLightRenderData[i];
			if (sdata.RenderBuffers.size())
				driver.registerPoolForRendering(vid::ERP_3D_LIGHTING_PASS,
					abs_transf, sdata.EnabledLightIdx, sdata.Materials,
					sdata.RenderBuffers, -1.0f,
					vid::ERM_RENDER_WITH_STENCIL_TEST_LIGHTING);
		}
	}

	if (isDebugDataVisible())
	{
		driver.register3DBoxForRendering(
			vid::ERP_3D_SOLID_PASS, getAbsoluteTransformation(),
			getBoundingBox(), img::SColor(0xffffffff));
	}

    ISceneNode::OnPreRender(timeMs);
}

//------------------------------------------------------------------------

vid::SMaterial& CKTreeSceneNode::getMaterial(s32 i)
{
	return m_KTree ? m_KTree->getMaterial(i) : *(vid::SMaterial*)NULL;
}

//------------------------------------------------------------------------

s32 CKTreeSceneNode::getMaterialsCount()
{
	return m_KTree ? m_KTree->getMaterialsCount() : 0;
}

//------------------------------------------------------------------------

void CKTreeSceneNode::updateAbsoluteTransformation()
{
	bool transformation_changed = m_TransformationChanged || m_ParentTransformationChanged;

	ISceneNode::updateAbsoluteTransformation();

	if (transformation_changed)
	{
		if (m_KTree)
			m_KTree->setAsoluteTransformation(AbsoluteTransformation);

		if (m_KTreeDirt)
			m_KTreeDirt->setAsoluteTransformation(AbsoluteTransformation);
	}  	
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::setPhysicalMesh(IAnimatedMesh* mesh)
{
	if (!mesh || mesh->getOveralFramesCount()<1)
		return;

	SAFE_DROP(m_PhysicalMesh);
	m_PhysicalMesh = mesh;
	SAFE_GRAB(m_PhysicalMesh);

	Box = m_PhysicalMesh->getMesh(0)->getBoundingBox();

	m_PhysicalMeshFullFileName = SCENE_MANAGER.findMeshFullFileName(m_PhysicalMesh);

	// now we exclusively holding this mesh, until it needed
	ISceneManager::getSingleton().removeMesh(m_PhysicalMesh);
}

//---------------------------------------------------------------------------

IAnimatedMesh* CKTreeSceneNode::getPhysicalMesh() 
{ 
	return m_PhysicalMesh;
}

//---------------------------------------------------------------------------

const c8* CKTreeSceneNode::getPhysicalMeshFullFileName() 
{ 
	return m_PhysicalMeshFullFileName.c_str(); 
}

//---------------------------------------------------------------------------

bool CKTreeSceneNode::rescaleNormals(const core::vector3df& new_scale)
{
	IMesh *mesh = getMesh();

	core::vector3df scale = new_scale;

	if (mesh)	
		mesh->rescaleMeshNormals(scale);	

	return false;
}

//---------------------------------------------------------------------------

bool CKTreeSceneNode::set(ISceneNode *other_scene_node)
{
	IKTreeSceneNode *other =
		SAFE_CAST_TO_KTREE_SCENE_NODE(other_scene_node);
	if (!other || this == other)
		return false;
	setAnimatedMesh(other->getAnimatedMesh());
	return true;
}

//---------------------------------------------------------------------------

//!
const c8* CKTreeSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0));
		xmlstr.append(mat_xml);
	}

	core::stringc relfullfname = "";

	core::stringc meshpath =
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_MESHES );

	core::stringc fullfname = m_AnimatedMeshFullFileName;
    core::stringc fname =  core::extractFileName(fullfname);
    core::stringc fpath =  core::extractFilePath(fullfname);
    core::stringc relpath = core::getRelativePathBetweenDirectories(meshpath, fpath);
    relfullfname.sprintf("%s%s",relpath.c_str(),fname.c_str());

	str.sprintf("<MeshFileName value=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);	

	fullfname = m_PhysicalMeshFullFileName;
    fname =  core::extractFileName(fullfname);
    fpath =  core::extractFilePath(fullfname);
    relpath = core::getRelativePathBetweenDirectories(meshpath, fpath);
    relfullfname.sprintf("%s%s",relpath.c_str(),fname.c_str());

	str.sprintf("<PhysicalMeshFileName value=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);	

	str.sprintf("<MaxTrianglesPerNode value=\"%d\" />\n", m_MaxTrianglesPerNode);
	xmlstr.append(str);	

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------

vid::SLight CKTreeSceneNode::getLightFromPos(core::vector3df pos)
{       
	if (m_AnimatedMesh->getAnimatedMeshType()==scn::EMT_BSP)
	{
		core::matrix4 invabs = getAbsoluteTransformation();
		invabs.makeInversed();	
		invabs.transformVect(pos);

		return ((IBSPLevelMesh*)m_AnimatedMesh)->getLightFromPos(pos);
	}

	vid::SLight light;

	light.DiffuseColor.set(1.0f, 1.0f, 1.0f);
	light.AmbientColor.set(1.0f, 1.0f, 1.0f);
	light.Position.set(0,0,0);

	return light;
}

//---------------------------------------------------------------------------
	
bool CKTreeSceneNode::isPointInside(core::vector3df point)
{
	if (m_AnimatedMesh->getAnimatedMeshType()==scn::EMT_BSP) 
		return m_TransformedBoundingBox.isPointInside(point);

	return false;
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::setAnimatedMesh(IAnimatedMesh* mesh, E_LOD_LEVEL lodlevel) 
{
	if (!mesh || mesh->getOveralFramesCount()<1) 
		return;
	
	SAFE_GRAB(mesh);
	SAFE_DROP(m_AnimatedMesh);	    
	m_AnimatedMesh = mesh;	

	recreateTree();

	m_AnimatedMeshFullFileName = SCENE_MANAGER.findMeshFullFileName(m_AnimatedMesh);

	// now we exclusively holding this mesh, until it needed
	ISceneManager::getSingleton().removeMesh(m_AnimatedMesh);

	ILightGridManager::getSingleton().removeLightGrid(this);

	if (m_AnimatedMesh->getAnimatedMeshType()==scn::EMT_BSP)
	{
		if (((IBSPLevelMesh*)m_AnimatedMesh)->isLightGridOK())
			ILightGridManager::getSingleton().addLightGrid(this);
	}	
}

//---------------------------------------------------------------------------

IMesh* CKTreeSceneNode::getMesh(E_LOD_LEVEL lodlevel)
{
	return m_AnimatedMesh ? m_AnimatedMesh->getMesh(0) : NULL;
}

//---------------------------------------------------------------------------

IMesh* CKTreeSceneNode::getMesh(f32 frame, E_LOD_LEVEL lodlevel)
{
	return getMesh(lodlevel);
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::setMaxTrianglesPerNode(s32 max_triangles_per_node)
{
	m_MaxTrianglesPerNode = max_triangles_per_node;
	CHECK_MIN_RANGE(m_MaxTrianglesPerNode, 1024);
}

//---------------------------------------------------------------------------

//!
s32 CKTreeSceneNode::getMaxTrianglesPerNode()
{
	return m_MaxTrianglesPerNode;
}

//---------------------------------------------------------------------------

bool CKTreeSceneNode::recreateTree()
{
	LOGGER.logInfo("Creating K-Tree . . .");

	if (!m_AnimatedMesh || m_AnimatedMesh->getOveralFramesCount()<1)
	{
		LOGGER.logErr("Can't create K-Tree from this mesh!");
        return false;
	}

	IMesh *mesh = m_AnimatedMesh->getMesh(0);

	if (!mesh)
	{
		LOGGER.logErr("Can't create K-Tree from null mesh!");
		return false;
	}

    u32 beginTime = TIMER.getSystemTime();    

	if (!m_PhysicalMesh)
		Box = mesh->getBoundingBox();

	core::vector3df bext = Box.getExtend();

	LOGGER.logInfo(
		" Bounding box (ext %.2f, %.2f, %.2f), "
		"(min %.2f, %.2f, %.2f) - (max %.2f, %.2f, %.2f)", 
		bext.X, bext.Y, bext.Z, Box.MinEdge.X, Box.MinEdge.Y, Box.MinEdge.Z,
		Box.MaxEdge.X, Box.MaxEdge.Y, Box.MaxEdge.Z);

	s32 TrisCount=0, VertsCount=0;

	s32 mbcnt = mesh->getMeshBuffersCount();
	for (s32 mbi=0; mbi<mbcnt; mbi++)
	{
		TrisCount += mesh->getMeshBuffer(mbi)->getIndexCount()/3; 
		VertsCount += mesh->getMeshBuffer(mbi)->getVertexCount();
	}		
	LOGGER.logInfo(" Mesh (%d tris, %d verts, %d mbufs).",
		TrisCount, VertsCount, mbcnt);

	SAFE_DELETE(m_KTree);	

	m_KTree = new KTree(mesh, m_MaxTrianglesPerNode, false);

	if (!m_KTree || !m_KTree->isOK())
	{
		LOGGER.logErr("Some errors, while creating K-Tree!");
		return false;
	} 

	m_RenderBBox = m_KTree->m_BoundingBox;

    u32 endTime = TIMER.getSystemTime();    

	TrisCount=0; 
	VertsCount=0;

	for (s32 mbi=0; mbi<m_KTree->m_VBOMesh->getMeshBuffersCount(); mbi++)
	{
		TrisCount += m_KTree->m_VBOMesh->getMeshBuffer(mbi)->getIndexCount()/3; 
		VertsCount += m_KTree->m_VBOMesh->getMeshBuffer(mbi)->getVertexCount();
	}	
	
	if (m_KTree->m_BadFacesCount > 0)
	{
		LOGGER.logInfo(
			" %d bad faces dectected, while creating K-tree, skipped them.", 
			m_KTree->m_BadFacesCount);
	}

	vid::IVideoDriver  &driver = VIDEO_DRIVER;

    LOGGER.logInfo(
		" Done in %dms (%d nodes, %d tris, %d verts, %d max tris per node, "
		"%d max depth, mesh buffs %d-init -> %d-subdiv).", 
		endTime - beginTime, m_KTree->m_NodeCount, TrisCount, VertsCount,
		m_MaxTrianglesPerNode, m_KTree->m_MaxDepth, 
		mesh->getMeshBuffersCount(), m_KTree->m_VBOMesh->getMeshBuffersCount());

	if (m_AnimatedMesh->getAnimatedMeshType()==scn::EMT_BSP)
	{
		if (DEVICE.getDeviceFlagValue(dev::EDCF_DIRT_ON_LEVEL)==true)
		{
			LOGGER.logInfo("Creating K-Tree Dirt . . .");

			SMesh dirt_mesh;
			vid::S3DVertexSimple v;
			mesh_buffer <vid::S3DVertexSimple, u32> dirt_mb;

			dirt_mesh.addMeshBuffer(&dirt_mb);	

			s32 mbcnt = mesh->getMeshBuffersCount();
			u32 ioffset = 0;
			for (s32 i=0; i < mbcnt; i++)
			{
				IMeshBuffer *mb = mesh->getMeshBuffer(i);
				if (mb->getMaterial().isTransparent())
					continue;
				u32 vcnt = mb->getVertexCount();
				for (u32 j = 0 ; j < vcnt; j++)
				{
					v.Pos = mb->getVertex(j).Pos; 
					dirt_mb.addVertex(v);
				}
				u32 icnt = mb->getIndexCount();
				for (u32 j = 0 ; j < icnt; j++)
					dirt_mb.addIndex(ioffset + mb->getIndex(j));
				ioffset += vcnt;
			}

			VertsCount = dirt_mb.getVertexCount();
			TrisCount = dirt_mb.getIndexCount()/3;

			LOGGER.logInfo(" Mesh (%d tris, %d verts, %d mbufs).",
				TrisCount, VertsCount, dirt_mesh.getMeshBuffersCount());
			
			beginTime = TIMER.getSystemTime();    

			SAFE_DELETE(m_KTreeDirt);	

			m_KTreeDirt = new KTree(&dirt_mesh, m_MaxTrianglesPerNode, true);

			if (!m_KTreeDirt || !m_KTreeDirt->isOK())
			{
				LOGGER.logErr("Some errors, while creating K-Tree Dirt!");
				return false;
			}

			endTime = TIMER.getSystemTime();    

			TrisCount=0;
			VertsCount=0;

			for (s32 mbi=0; mbi<m_KTreeDirt->m_VBOMesh->getMeshBuffersCount(); mbi++)
			{
				TrisCount += m_KTreeDirt->m_VBOMesh->getMeshBuffer(mbi)->getIndexCount()/3; 
				VertsCount += m_KTreeDirt->m_VBOMesh->getMeshBuffer(mbi)->getVertexCount();
			}	
			
			if (m_KTreeDirt->m_BadFacesCount>0)
			{
				LOGGER.logInfo(
					" %d bad faces dectected, while creating K-tree, skipped them.", 
					m_KTree->m_BadFacesCount);
			}

			LOGGER.logInfo(
				" Done in %dms (%d nodes, %d tris, %d verts, %d max tris per node, "
				"%d max depth, mesh buffs %d-init -> %d-subdiv).", 
				endTime - beginTime, m_KTreeDirt->m_NodeCount, TrisCount, VertsCount,
				m_MaxTrianglesPerNode, m_KTreeDirt->m_MaxDepth, 
				dirt_mesh.getMeshBuffersCount(),
				m_KTreeDirt->m_VBOMesh->getMeshBuffersCount());
		}

		lockMaterials();
	}	

    return true;
}

//---------------------------------------------------------------------------

void CKTreeSceneNode::makeDirtSpot(
	const core::vector3df &abs_pos, const core::vector3df &size,
	const img::SColor &color, bool hipriority)
{	
	if (!m_KTreeDirt || DEVICE.getDeviceFlagValue(dev::EDCF_DIRT_ON_LEVEL)==false)
		return;

	core::matrix4 invabs = getAbsoluteTransformation();
	invabs.makeInversed();

	// transform into local coord system
	core::vector3df abs_pos_inv(abs_pos);
	invabs.transformVect(abs_pos_inv);
	if (!m_RenderBBox.isPointInside(abs_pos_inv))
		return;

	if (hipriority)
		m_DirtTextureUpdateTasks.push_front(SDirtUpdateTask(abs_pos_inv, size, color) ); 
	else
		m_DirtTextureUpdateTasks.push_back(SDirtUpdateTask(abs_pos_inv, size, color) ); 
	
	m_DirtTextureUpdateQueue.push_back(this);  
}      

//---------------------------------------------------------------------------

inline void CKTreeSceneNode::_completeDirtTasks(u32 timeMs)
{	
	if (m_DirtTextureUpdateTasks.empty())
		return;

	if (!m_DirtTextureUpdateQueue.is_ready(this))
		return;

	s32 dirt_cnt = 0;
	core::list<SDirtUpdateTask>::iterator it = m_DirtTextureUpdateTasks.begin();
	for (; it!=m_DirtTextureUpdateTasks.end()&&dirt_cnt<3; dirt_cnt++)
	{		
		SDirtUpdateTask &dtask = *it; 
		core::aabbox3df spot_bb(dtask.Pos-dtask.Size*0.5f, dtask.Pos+dtask.Size*0.5f);

		m_KTreeDirt->makeDirtSpot(spot_bb, dtask.Pos, dtask.Size, dtask.Color); 

		it = m_DirtTextureUpdateTasks.erase(it);  
	}

	m_KTreeDirt->unlockDirtTextures();

	m_LastTimeDirtUpdate = timeMs;

	if (m_DirtTextureUpdateTasks.empty())
		m_DirtTextureUpdateQueue.switch_next();	
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
