//|-------------------------------------------------------------------------
//| File:        CShadowVolume.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CShadowVolume.h"
#include "CGeometryCreator.h"

#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>
#include <dyn/IDynamicManager.h>
#include <scn/IAnimatedMesh.h>
#include <dev/IDevice.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <game/IGameManager.h>
#include <scn/ISceneNode.h>
#include <vid/SFog.h>
#include <scn/IPathFinderManager.h>
#include <dev/IProfiler.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CLightShadowVolume : public CLightShadowGround
{	
public:	

	CLightShadowVolume() 
		: side_vertices(0), cap_vertices(0),
		side_count(0), side_size(0), cap_count(0), cap_size(0), 
		zfail(false), time_sec(0), vcnt(0), icnt(0) {}

	CLightShadowVolume(u32 _side_verts, u32 _cap_verts) 
		: side_vertices(0), cap_vertices(0),
		side_count(0), side_size(0), cap_count(0), cap_size(0), 
		zfail(false), time_sec(0), vcnt(0), icnt(0)
	{
		reallocate(_side_verts, _cap_verts);
	}

	virtual ~CLightShadowVolume()
	{
		SAFE_DELETE_ARRAY(side_vertices);
		SAFE_DELETE_ARRAY(cap_vertices);
	}

	virtual E_SHADOW_TYPE getShadowType() { return ESHT_SHADOW_VOLUME; }

	void reallocate(u32 _side_verts, u32 _cap_verts)
	{
		// reallocating memory for vertices, if allocated arrray is less than needed
		if (side_size<_side_verts)
        {			
            SAFE_DELETE_ARRAY(side_vertices);
			side_size = _side_verts;
            side_vertices = new core::vector3df[side_size];
        }
		if (cap_size<_cap_verts)
        {
			SAFE_DELETE_ARRAY(cap_vertices);
			cap_size = _cap_verts;
            cap_vertices = new core::vector3df[cap_size];
        }
		side_count = 0;
		cap_count  = 0;
	}

	core::vector3df		*side_vertices, *cap_vertices;
    u32					side_count, cap_count;
    u32					side_size, cap_size;
	bool				zfail;
	f32					time_sec;
	s32					vcnt, icnt;
};

//---------------------------------------------------------------------------

u32 CShadowVolume::s_ProfileShadowVolume = 0;

//---------------------------------------------------------------------------

CShadowVolume::CShadowVolume(ISceneNode* parent) 
	: CShadowFake(parent), m_Edges(0), m_EdgeCount(0), m_MeshIndices(0),
	m_VertIndices(0), m_IndexCount(0), m_VertexCount(0),
	m_IndexCountAllocated(0),
	m_Adjacency(0), m_FaceData(0), m_AnimationTimeSec(-1.0f),
	m_ShadowVolumeLightsUsed(0), m_FakeShadow(false)
{
#if MY_DEBUG_MODE 
    setClassName("CShadowVolume");
#endif

    m_JustCreated = true;	
	
	m_AlphaInterpolation = 0.0f;
	m_StartSwitchingMs = 0;

	if (!s_ProfileShadowVolume)
		s_ProfileShadowVolume = PROFILER.allocProfileId("Shadow Volume");

	m_JustCreated = false;
}

//---------------------------------------------------------------------------

CShadowVolume::~CShadowVolume()
{
	SAFE_DELETE_ARRAY(m_Edges);
	SAFE_DELETE_ARRAY(m_VertIndices);
	SAFE_DELETE_ARRAY(m_MeshIndices);
	SAFE_DELETE_ARRAY(m_FaceData);	

	for (u32 i=0; i<m_ShadowVolumeLights.size(); ++i)
		SAFE_DELETE(m_ShadowVolumeLights[i]);
	m_ShadowVolumeLights.clear();
	m_ShadowVolumeLightsUsed =0;
}

//---------------------------------------------------------------------------

void CShadowVolume::_registerGeometryForRendering(
	f32 now, IAnimatedMesh *amesh, IMesh *mesh, bool render_debug)
{
	if (!amesh || !mesh)
		return;	

	s32 bufcnt = mesh->getMeshBuffersCount();
	IMeshBuffer* b;
	s32 i;

	for (i=0; i < bufcnt; i++)
    {
        b = mesh->getMeshBuffer(i);
		if (b->getIndexType()!=EIT_16_BIT)
			return;
	}

	s32 dl_count = m_DLights.size();

	bool need_to_build_shadow_volume = dl_count > 0;

    if (!need_to_build_shadow_volume && !m_JustCreated) 
        return; 

    //------------------------------------------------
    // allocate memory
    //------------------------------------------------

    m_ShadowVolumeLightsUsed = 0;
    
    s32 oldIndexCount = m_IndexCount;
    s32 oldVertexCount = m_VertexCount;

    m_VertexCount = 0;
    m_IndexCount = 0;

    // calculate total amount of vertices and indices     
  
    for (i=0; i<bufcnt; i++)
    {
        b = mesh->getMeshBuffer(i);

		if (b->getMaterial().getPass(0).isTransparent())
			continue;

		m_IndexCount += b->getIndexCount();
		m_VertexCount += b->getVertexCount();
    }

	if (!m_IndexCount || !m_VertexCount)
		return;

	bool need_to_copy_mesh = false;

    // recalculate adjacency if neccessarry

    if (oldVertexCount != m_VertexCount && oldIndexCount != m_IndexCount)
	{
		SAdjacencyData &adj = amesh->getAdjacencyData();
		if (adj.Adjacency == NULL)
			amesh->calculateAdjacency();
		if (m_IndexCount == adj.IndexCount)
			m_Adjacency = adj.Adjacency;
		else
		{
			m_Adjacency = NULL;
			LOGGER.logErr("Wrong adjacency data detected!");
		}
		need_to_copy_mesh = true;
	}

	if (!need_to_build_shadow_volume || !m_Adjacency) 
		return;	

    // allocate memory if nececcary

    if (m_IndexCount > m_IndexCountAllocated)
    {
		SAFE_DELETE_ARRAY(m_VertIndices);
        m_VertIndices = new u16[m_IndexCount];
		SAFE_DELETE_ARRAY(m_MeshIndices);
		m_MeshIndices = new u8[m_IndexCount];
        m_IndexCountAllocated = m_IndexCount;
        SAFE_DELETE_ARRAY(m_FaceData);
        m_FaceData = new bool[m_IndexCount / 3];  
		need_to_copy_mesh = true;
    }

	m_AnimationTimeSec = amesh->getAnimationTimeSec();

    // copy mesh from original if needed
    
    for (s32 vtx=0, idx=0, i=0; i<bufcnt && need_to_copy_mesh; ++i)
    {
		b = mesh->getMeshBuffer(i);
		if (b->getMaterial().getPass(0).isTransparent())
			continue;
		s32 idxcnt = b->getIndexCount();
		s32 vtxcnt = b->getVertexCount();
		const u16* indices = (u16*)b->getIndices();
		for (s32 j = 0; j < idxcnt;)
		{
			m_MeshIndices[idx] = i;
			m_VertIndices[idx++] = indices[j++];
			m_MeshIndices[idx] = i;
			m_VertIndices[idx++] = indices[j++];
			m_MeshIndices[idx] = i;
			m_VertIndices[idx++] = indices[j++];
		}
    }

    //---------------------------------------------------------------------
    // create as much shadow volumes as there are lights but 
    // do not ignore the max light settings.
    //---------------------------------------------------------------------

	dev::IDevice &device = DEVICE;

	s32 lights = m_DLights.size();

	img::SColor shadow_global_color = m_Driver.getShadowColor();
	img::SColor shadow_color = shadow_global_color;	
	f32 shadow_global_alpha = shadow_global_color.getAlpha();
	f32 fog_end = m_Driver.getFog().End;
	f32 fog_beg = m_Driver.getFog().Start;
	f32 fog_mid = (fog_end+fog_beg)/2.0f;

	CHECK_RANGE(shadow_global_alpha, 0, 255);

    for (s32 i=0; i < lights; ++i)
    { 
		SDynamicLightEntry *dl = m_DLights[i];
		if (!dl->Updated || (
				!dl->CastShadowsNow &&
				dl->ActionType == SDynamicLightEntry::laNone))
			continue;

		scn::ISceneNode *owner = NULL;
		if (!dl->Light.Dynamic && (
				dl->ActionType != SDynamicLightEntry::laNone ||
				m_AlphaInterpolation > 0.0f))
			owner = m_TopParent;

		// this stuff is needed to smooth appearing/disappearing of the shadow
		f32 alpha_coeff1 = 1.0f;
		f32 alpha_coeff2 = 1.0f - m_AlphaInterpolation;
		if (dl->ActionType == SDynamicLightEntry::laDisappearing)
			alpha_coeff1 = 1.0f - fabs(now - dl->ActionStartTime) /
				dl->ActionLifeTime;					
		else if (dl->ActionType == SDynamicLightEntry::laAppearing)
			alpha_coeff1 = fabs(now - dl->ActionStartTime) /
				dl->ActionLifeTime;
		CHECK_RANGE(alpha_coeff1, 0.0f, 1.0f);
		CHECK_RANGE(alpha_coeff2, 0.0f, 1.0f);

		shadow_color.setAlpha(shadow_global_alpha *
			alpha_coeff1 * alpha_coeff2);

        // create Shadow Volume for the current light source

		CLightShadowVolume *svp = _createShadowVolume(*dl, mesh, owner, render_debug);
		_updateShadowGround(*dl, svp);
		svp->onLightShadowCasted(
			*dl, scn::ESHT_SHADOW_VOLUME, owner, shadow_color);
    }
}

//---------------------------------------------------------------------------

CLightShadowVolume* CShadowVolume::_createShadowVolume(
	SDynamicLightEntry &light, IMesh *mesh, ISceneNode *owner, bool render_debug)
{
	scn::ICameraSceneNode* cam =
		m_SceneManager.getActiveCamera();
    if (!cam)
		return NULL;

    core::vector3df campos = cam->getAbsolutePosition();
    m_ParentAbsoluteTransformationInv.transformVect(campos);
    
    core::vector3df camtgt = cam->getTarget();
    m_ParentAbsoluteTransformationInv.transformVect(camtgt);

	core::vector3df camdir = camtgt-campos;

	bool point_light = light.Light.Type==vid::ELT_POINT;
	core::vector3df lpos = light.LocalPosition;
	core::vector3df ldir = light.LocalDirection; 
  
    // allocate memory for the shadow volume contour

    s32 faceCount = (int)(m_IndexCount / 3);

    if (faceCount * 6 > m_EdgeCount)
    {
        SAFE_DELETE_ARRAY(m_Edges);
        m_EdgeCount = faceCount * 6;
        m_Edges = new u16[m_EdgeCount];
    }    

    s32 numEdges = 0;

    // Check every face if it is front or back facing the light.

	u32 nBackFaces = 0;
	u8 *meshIndices = m_MeshIndices;
	u16 *indices = m_VertIndices;
	core::vector3df normal;

	if (point_light)
	{
		core::vector3df vec;

		for (s32 i=0; i<faceCount; ++i, meshIndices += 3)
		{
			const scn::IMeshBuffer *mb = mesh->getMeshBuffer(*meshIndices);

			const core::vector3df &v0 = mb->getVertex(*indices++).Pos;
			const core::vector3df &v1 = mb->getVertex(*indices++).Pos;
			const core::vector3df &v2 = mb->getVertex(*indices++).Pos;
			
			normal.set(v2-v1);
			normal.crossProduct(v1-v0);

			vec.set(lpos - v0);

			if(normal.getDotProduct(vec) >= 0.0f )
			{
				// it's a back facing face
				m_FaceData[i] = false;  
				nBackFaces++;
			}
			else
				// it's a front facing face
				m_FaceData[i] = true;   			
		}
	}
	else
	{
		for (s32 i = 0; i < faceCount; ++i, meshIndices += 3)
		{
			const scn::IMeshBuffer *mb = mesh->getMeshBuffer(*meshIndices);

			const core::vector3df &v0 = mb->getVertex(*indices++).Pos;
			const core::vector3df &v1 = mb->getVertex(*indices++).Pos;
			const core::vector3df &v2 = mb->getVertex(*indices++).Pos;

			normal.set(v2-v1);
			normal.crossProduct(v1-v0);

			if (normal.getDotProduct(lpos) >= 0.0f)
			{
				// it's a back facing face
				m_FaceData[i] = false;  
				nBackFaces++;
			}
			else
				// it's a front facing face
				m_FaceData[i] = true;   
		}
	}

	static const u16 minusOne = (u16)-1;

	// creating list of edges

	u16 wFace0, wFace1, wFace2;
	u16 adj0, adj1, adj2;
	u32 numEdges2;		

	for(s32 i = 0; i < faceCount; ++i)
	{
		if (m_FaceData[i] == true)
	    {
		    wFace0 = 3*i;
			wFace1 = wFace0 + 1;
			wFace2 = wFace1 + 1;

	        adj0 = m_Adjacency[wFace0];
		    adj1 = m_Adjacency[wFace1];
			adj2 = m_Adjacency[wFace2];				

			if ((adj0 != minusOne && m_FaceData[adj0] == false) || adj0 == minusOne)
		    {
				numEdges2   = 2*numEdges;
			    //  add edge v0-v1					
				m_Edges[numEdges2] = wFace0;
				m_Edges[numEdges2+1] = wFace1;
	            ++numEdges;
		    }

		    if ((adj1 != minusOne && m_FaceData[adj1] == false) || adj1 == minusOne)
	        {
				numEdges2   = 2*numEdges;
		        //  add edge v1-v2
			    m_Edges[numEdges2] = wFace1;
				m_Edges[numEdges2+1] = wFace2;
				++numEdges;
	        }

		    if ((adj2 != minusOne && m_FaceData[adj2] == false) || adj2 == minusOne)
	        {
				numEdges2   = 2*numEdges;
		        //  add edge v2-v0
			    m_Edges[numEdges2] = wFace2;
				m_Edges[numEdges2+1] = wFace0;
				++numEdges;
	        }
		}    
	}
	
	bool z_fail = false;
	bool need_cap = false;
	{
		core::vector3df avg_pos(0,0,0);
		s32 i2 = 0;

		for (s32 i=0; i<numEdges; ++i)
		{		
			i2 = 2*i;

			const scn::IMeshBuffer *mb = mesh->getMeshBuffer(m_MeshIndices[m_Edges[i2  ]]);

			const core::vector3df &v0 = mb->getVertex(m_VertIndices[i2]).Pos;
			const core::vector3df &v1 = mb->getVertex(m_VertIndices[i2+1]).Pos;

			avg_pos += v0 + v1;
		}
		avg_pos /= 2*numEdges;
		f32 dist  = (campos-lpos).getLength();
		f32 angle = (avg_pos-lpos).getAngleDeg(camdir);
		if (angle > 90)
		{
			need_cap = true;
			//if (point_light)
			//	z_fail = true;
		}
	}

    // allocating memory for shadow volume	

	CLightShadowVolume* svp = 0;

	u32 nNeededSideVertices = 0;//numEdges*6;
	u32 nNeededCapVertices  = 0;//need_cap ? nBackFaces * 6 : 0;

    if (m_ShadowVolumeLights.size() > (u32)m_ShadowVolumeLightsUsed)
    {
        // get the next unused buffer
        svp = m_ShadowVolumeLights[m_ShadowVolumeLightsUsed];
		svp->reallocate(nNeededSideVertices, nNeededCapVertices); 
    }
    else
    {
        // add a new buffer
		svp = new CLightShadowVolume(nNeededSideVertices, nNeededCapVertices);
        m_ShadowVolumeLights.push_back(svp);     		
    }  
	++m_ShadowVolumeLightsUsed;

	svp->setLight(light);

	svp->zfail		= z_fail;
	svp->time_sec	= m_AnimationTimeSec;
	svp->vcnt		= m_VertexCount;
	svp->icnt		= m_IndexCount;

	// This value must be properly calculated before
	// registration geometry for rendering
	svp->m_GroundY = m_GroundPlane.getMemberPoint().Y;
	svp->m_ZOrder = (s32)svp->m_GroundY / 10.0f;

	// creating shadow volume 

	static vid::SMaterial m;
	static vid::SRenderPass &p = m.getPass(0);
	p.setLightingMode(vid::ELM_NONE);
	p.setFlag(vid::EMF_ZWRITE_ENABLE,	false);
	p.setFlag(vid::EMF_GOURAUD_SHADING, false);
	p.setAlphaTest(vid::EAT_ALWAYS);
	p.setDepthTest(vid::ECT_LEQUAL);
	p.setFlag(vid::EMF_BLENDING, false);

	vid::E_RENDER_MODE mode = svp->zfail ?
		vid::ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZFAIL :
		vid::ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZPASS;
	vid::E_RENDER_PASS pass = vid::ERP_3D_LIGHTING_PASS;

	f32 zorder = light.Light.Dynamic ? -1.0f : svp->m_ZOrder;

	s32 i2 = 0;

	u32 nVerts = 0;
	vid::SGeometryBufferEntry geom;

	if (point_light)
	{
		// creating sides of the shadow volume			

		core::vector3df vdir1, vdir2, vdir3;
		f32 vlen=0;
		f32 lradius = light.Light.Radius * 1.1f;

		nVerts = numEdges * 6 + (need_cap ? nBackFaces * 6 : 0);

		geom = m_Driver.registerGeometryBufferForRendering(pass,
			m_ParentAbsoluteTransformation, light.Index, m,
			nVerts, vid::EVT_SIMPLE, 0, (E_INDEX_TYPE)0,
			vid::EDPT_TRIANGLE_LIST, zorder, mode, owner);

		core::vector3df *vertices = (core::vector3df*)geom.Vertices;

		for (s32 i = 0; i < numEdges; ++i)
		{		
			i2 = 2*i;

			const scn::IMeshBuffer *mb = mesh->getMeshBuffer(m_MeshIndices[m_Edges[i2]]);

			const core::vector3df &v1 = mb->getVertex(m_VertIndices[m_Edges[i2  ]]).Pos;
			const core::vector3df &v2 = mb->getVertex(m_VertIndices[m_Edges[i2+1]]).Pos;

			vdir1 = v1-lpos;
			vlen=vdir1.getLength();
			vdir1.normalize();
			vdir1 *= (lradius-vlen);

			vdir2 = v2-lpos;
			vlen=vdir2.getLength();
			vdir2.normalize();
			vdir2 *= (lradius-vlen);

			*vertices++ = v1;
			*vertices++ = v2;
			*vertices++ = v2 + vdir2;

			*vertices++ = v1;
			*vertices++ = *(vertices - 3);
			*vertices++ = v1 + vdir1;
		}

		// creating cap of the shadow volume

		if (need_cap)
		{
			s32 i3 = 0;
			for (s32 i = 0; i < faceCount; ++i)
			{
				if (m_FaceData[i]==false)
				// it's a back facing face
				{
					i3 = 3*i;

					const scn::IMeshBuffer *mb = mesh->getMeshBuffer(m_MeshIndices[i3]);

					const core::vector3df &v1 = mb->getVertex(m_VertIndices[i3]).Pos;
					const core::vector3df &v2 = mb->getVertex(m_VertIndices[i3+1]).Pos;
					const core::vector3df &v3 = mb->getVertex(m_VertIndices[i3+2]).Pos;

					vdir1 = v1-lpos;
					vlen=vdir1.getLength();
					vdir1.normalize();
					vdir1 *= (lradius-vlen);

					vdir2 = v2-lpos;
					vlen=vdir2.getLength();
					vdir2.normalize();
					vdir2 *= (lradius-vlen);

					vdir3 = v3-lpos;
					vlen=vdir3.getLength();
					vdir3.normalize();					
					vdir3 *= (lradius-vlen);
					
					*vertices++ = v3 + vdir3;
					*vertices++ = v2 + vdir2;
					*vertices++ = v1 + vdir1;

					*vertices++ = v1;
					*vertices++ = v2;
					*vertices++ = v3;
				}
			}
		}
	}
	else
	{
		// creating sides of the shadow volume

		nVerts = numEdges * 6 + (need_cap ? nBackFaces * 3 : 0);

		geom = m_Driver.registerGeometryBufferForRendering(pass,
			m_ParentAbsoluteTransformation, light.Index, m,
			nVerts, vid::EVT_SIMPLE, 0, (E_INDEX_TYPE)0,
			vid::EDPT_TRIANGLE_LIST, zorder, mode, owner);

		core::vector3df *vertices = (core::vector3df*)geom.Vertices;

		core::vector3df pos(m_TopParentCenter.X,
			m_TopParentTransformedBBox.MaxEdge.Y,
			m_TopParentCenter.Z);
		f32 height = PATH_FINDER_MANAGER.getHeightOverGround(pos);
		core::vector3df orig(0.0f, 0.0f, 0.0f);
		core::vector3df down_dir(0.0f, -1.0f, 0.0f);
		m_ParentAbsoluteTransformationInv.transformVect(orig);
		m_ParentAbsoluteTransformationInv.transformVect(down_dir);
		down_dir -= orig;
		f32 angle_rad = down_dir.getAngleRad(ldir);
		f32 cos_angle = core::math::Cos(angle_rad);
		cos_angle = (cos_angle != 0.0f) ? cos_angle : ROUNDING_ERROR_32;
		f32 volume_len = 1.05f * (height / cos_angle);
		if (volume_len == 0)
			volume_len = 100.f;

		core::vector3df vdir = ldir * volume_len;

		for (s32 i = 0; i < numEdges; ++i)
		{		
			i2 = 2*i;

			const scn::IMeshBuffer *mb = mesh->getMeshBuffer(m_MeshIndices[m_Edges[i2]]);

			const core::vector3df &v1 = mb->getVertex(m_VertIndices[m_Edges[i2  ]]).Pos;
			const core::vector3df &v2 = mb->getVertex(m_VertIndices[m_Edges[i2+1]]).Pos;

			*vertices++ = v1;
			*vertices++ = v2; 				
			*vertices++ = v2 + vdir;				
			
			*vertices++ = v1;
			*vertices++ = *(vertices - 3);
			*vertices++ = v1 + vdir;
		}

		// creating cap of the shadow volume

		if (need_cap)
		{
			s32 i3 = 0;
			for (s32 i = 0; i < faceCount; ++i)
			{
				if (m_FaceData[i]==false)
				// it's a back facing face
				{
					i3 = 3*i;

					const scn::IMeshBuffer *mb = mesh->getMeshBuffer(m_MeshIndices[i3]);

					const core::vector3df &v1 = mb->getVertex(m_VertIndices[i3]).Pos;
					const core::vector3df &v2 = mb->getVertex(m_VertIndices[i3+1]).Pos;
					const core::vector3df &v3 = mb->getVertex(m_VertIndices[i3+2]).Pos;

					*vertices++ = v3 + vdir;
					*vertices++ = v2 + vdir;
					*vertices++ = v1 + vdir;
				}
			}
		}
	}

	// recalculate bounding box
	svp->BoundingBox.MinEdge.set(FLT_MAX, FLT_MAX, FLT_MAX);
	svp->BoundingBox.MaxEdge.set(FLT_MIN, FLT_MIN, FLT_MIN);
	core::vector3df *vertices = (core::vector3df*)geom.Vertices;
	for (u32 i = 0; i < nVerts; i++)
		svp->BoundingBox.addInternalPoint(*vertices++);
	svp->TransformedBoundingBox = svp->BoundingBox;
	m_ParentAbsoluteTransformation.transformBox(svp->TransformedBoundingBox);

	if (render_debug)
	{
		m_Driver.register3DBoxForRendering(
			core::matrix4(), svp->TransformedBoundingBox,
			0xffffff00);
	}

	return svp;
}

//---------------------------------------------------------------------------

void CShadowVolume::registerForRendering(
	bool fake_shadow, u32 nowMs,
	const vid::SLight & light0, u64 enabled_lights,
	IAnimatedMesh *amesh, IMesh *mesh, bool render_debug)
{
	PROFILER.startProfiling(s_ProfileShadowVolume);

	if (!m_ParentImposter)
		fake_shadow = false;
	if (!m_StartSwitchingMs || m_FakeShadow != fake_shadow)
		m_StartSwitchingMs = nowMs;

	m_FakeShadow = fake_shadow;
	if (m_FakeShadow)
		m_AlphaInterpolation += 0.05f * (nowMs - m_StartSwitchingMs) / 1000.f;
	else
		m_AlphaInterpolation -= 0.05f * (nowMs - m_StartSwitchingMs) / 1000.f;
	CHECK_RANGE(m_AlphaInterpolation, 0.0f, 1.0f);

	PROFILER.stopProfiling(s_ProfileShadowVolume);

	bool need_to_apply_lights = true;
	if (m_AlphaInterpolation > 0.0f)
	{
		CShadowFake::registerForRendering(fake_shadow, nowMs,
			light0, enabled_lights,
			amesh, mesh, render_debug);
		need_to_apply_lights = false;
	}

	PROFILER.startProfiling(s_ProfileShadowVolume);

	if (m_AlphaInterpolation < 1.0f)
	{
		PROFILER.stopProfiling(s_ProfileShadowVolume);
		if (need_to_apply_lights)
			_applyLights(nowMs, light0, enabled_lights);
		PROFILER.startProfiling(s_ProfileShadowVolume);
		
		_registerGeometryForRendering(nowMs, amesh, mesh, render_debug);
		m_ShadowVolumeLightsUsed = 0;
	}

	PROFILER.stopProfiling(s_ProfileShadowVolume);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
