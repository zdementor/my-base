//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshMD2.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CAnimatedMeshMD2.h"

#include <io/IReadFile.h>
#include <img/SColor.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <vid/IRenderBuffer.h>
#include <vid/IVideoDriver.h>
#include <dev/IDevice.h>

#include "MD2Helper.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

using namespace md2;

//---------------------------------------------------------------------------

CAnimatedMeshMD2::CAnimatedMeshMD2()
	: m_BeginFrame(0), m_EndFrame(0),
m_CurrentFrameFloat(0.0f), m_PreCurrentFrame(0),
m_OveralFramesCount(0), m_CurrentFrame(0), m_VerticesCount(0)
{
#if MY_DEBUG_MODE 
	CAnimatedMeshMD2::setClassName("CAnimatedMeshMD2 IAnimatedMesh");
#endif  

	m_InterfaceMeshBuffer = new SMD2MeshBuffer<vid::S3DVertex1TCoords>();

	m_InterfaceMesh.addMeshBuffer(m_InterfaceMeshBuffer);

	m_InterfaceMeshBuffer->drop();

	m_InterpolateAnimation = DEVICE.getDeviceFlagValue(dev::EDCF_HI_ANIMATION_QUALITY);
}

//---------------------------------------------------------------------------

CAnimatedMeshMD2::~CAnimatedMeshMD2()
{
}

//---------------------------------------------------------------------------

bool CAnimatedMeshMD2::loadFromFile(io::IReadFile* file)
{
    if (!file)
        return false;

    SMD2Header header;
    s8* frames = 0;

    file->read(&header, sizeof(SMD2Header));

    if (header.magic != MD2_MAGIC_NUMBER || header.version != MD2_VERSION)
    {
        LOGGER.log("MD2 Loader: Wrong file header", file->getFileName(), io::ELL_WARNING);
        return false;
    }

    // create Memory for indices and frames

    m_Indices.reallocate(header.numTriangles);

	m_OveralFramesCount = header.numFrames;

    m_FrameList.set_used(m_OveralFramesCount);
	
    s32 i;
    
    for (i=0; i<(s32)m_OveralFramesCount; ++i)
        m_FrameList[i].reallocate(header.numVertices);

    // read TextureCoords

    file->seek(header.offsetTexcoords, false);
    SMD2TextureCoordinate* textureCoords = new SMD2TextureCoordinate[header.numTexcoords];

    if (!file->read(textureCoords, sizeof(SMD2TextureCoordinate)*header.numTexcoords))
    {
        LOGGER.log("MD2 Loader: Error reading TextureCoords.", file->getFileName(), io::ELL_ERROR);
        return false;
    }

    // read Triangles

    file->seek(header.offsetTriangles, false);

    SMD2Triangle *triangles = new SMD2Triangle[header.numTriangles];
    if (!file->read(triangles, header.numTriangles *sizeof(SMD2Triangle)))
    {
        LOGGER.log("MD2 Loader: Error reading triangles.", file->getFileName(), io::ELL_ERROR);
        return false;
    }

    // read Vertices

    s8 buffer[MD2_MAX_VERTS*4+128];
    SMD2Frame* frame = (SMD2Frame*)buffer;

    core::array< core::vector3df >* vertices = new core::array< core::vector3df >[header.numFrames];
    core::array< core::vector3df >* normals = new core::array< core::vector3df >[header.numFrames];

    file->seek(header.offsetFrames, false);

	m_VerticesCount = header.numTriangles * 3;

    for (i = 0; i<header.numFrames; ++i)
    {
        // read vertices

        vertices[i].reallocate(header.numVertices);
        file->read(frame, header.frameSize);

        // store frame data

        SFrameData fdata;
        fdata.begin = i;
        fdata.end = i;

        if (frame->name[0])
        {
            for (s32 s = 0; frame->name[s]!=0 && (frame->name[s] < '0' ||
                frame->name[s] > '9'); ++s)
                fdata.name += frame->name[s];

            if (!m_FrameData.empty() && m_FrameData[m_FrameData.size()-1].name == fdata.name)
                ++m_FrameData[m_FrameData.size()-1].end;
            else
                m_FrameData.push_back(fdata);
        }

        // add vertices

        for (s32 j=0; j<header.numVertices; ++j)
        {
            core::vector3df v;
            v.X = (f32)frame->vertices[j].vertex[0] * frame->scale[0] + frame->translate[0];
            v.Z = (f32)frame->vertices[j].vertex[1] * frame->scale[1] + frame->translate[1];
            v.Y = (f32)frame->vertices[j].vertex[2] * frame->scale[2] + frame->translate[2];

            vertices[i].push_back(v);

            s32 normalidx = frame->vertices[j].lightNormalIndex;
			
            if (normalidx >= 0 && normalidx < Q2_VERTEX_NORMAL_TABLE_SIZE)
            {
                v.X =  Q2_VERTEX_NORMAL_TABLE[normalidx][0];
                v.Z =  Q2_VERTEX_NORMAL_TABLE[normalidx][1];
                v.Y =  Q2_VERTEX_NORMAL_TABLE[normalidx][2];
            }  

            normals[i].push_back(v);
        }

        // calculate bounding boxes
        if (header.numVertices)
        {
            core::aabbox3d<f32> box;
            box.reset(vertices[i][0]);

            for (s32 j=1; j<header.numVertices; ++j)
                box.addInternalPoint(vertices[i][j]);

            m_BoxList.push_back(box);
        }
    }

    // put triangles into frame list
    
    f32 dmaxs = 1.0f/(header.skinWidth);
    f32 dmaxt = 1.0f/(header.skinHeight);

    vid::S3DVertex1TCoords vtx;
    
    for ( s32 f = 0; f < header.numFrames; ++f )
    {
        core::array< core::vector3df >& vert = vertices[f];

        for (s32 t=0; t<header.numTriangles; ++t)
        {
            for (s32 n=0; n<3; ++n)
            {
                vtx.Pos = vert[triangles[t].vertexIndices[n]];
                vtx.TCoords.X = (textureCoords[triangles[t].textureIndices[n]].s + 0.5f) * dmaxs;
                vtx.TCoords.Y = (textureCoords[triangles[t].textureIndices[n]].t + 0.5f) * dmaxt;
                vtx.Normal = normals[f][triangles[t].vertexIndices[n]];				

                m_FrameList[f].push_back(vtx);
            }
        }
    }

    // create indices

	m_Indices.reallocate(m_VerticesCount);
    for (u32 i = 0; i < m_VerticesCount; i++)
        m_Indices.push_back(i);

	if ( m_InterpolateAnimation )
	// allocate interpolate buffer, if needed
		m_InterpolateBuffer.set_used(m_VerticesCount);		

	core::stringc msg;
	msg.sprintf("Md2 mesh initialized verts=%d faces=%d", m_VerticesCount, m_VerticesCount/3);
	LOGGER.log(msg, io::ELL_INFORMATION);

    // clean up

    delete [] normals;
    delete [] vertices;
    delete [] triangles;
    delete [] textureCoords;

    return true;
}

//---------------------------------------------------------------------------

bool CAnimatedMeshMD2::getAnimationState(SAnimationState& ani_state)
{
	ani_state.Animation      = m_CurrentAnimationIdx;
	ani_state.AnimationTime  = m_CurrentFrameFloat;

	ani_state.AnimationTimeSec     = getAnimationTimeSec();
	ani_state.AnimationDurationSec = getAnimationDurationSec();

	ani_state.Looped         = m_Looping;	

	return true;
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::updateMesh(s32 delta_ms)
{
	if (m_CurrentAnimationIdx == -1)
		return;

    if (m_Looping)
    {
        // play animation looped
        m_CurrentFrameFloat = f32(
			m_CurrentFrameFloat + 
			f32(delta_ms) * m_OveralSpeed * m_CurrentAnimationSpeed
			);

		m_CurrentFrame = m_CurrentFrameFloat;

		if (m_CurrentFrameFloat > m_EndFrame)
        {
            m_CurrentFrame       = m_BeginFrame;
			m_CurrentFrameFloat -= (m_EndFrame-m_BeginFrame);		
			
			OnAnimationEnd(m_CurrentAnimationIdx);
        }		
    }
    else
	{
        // play animation non looped
        m_CurrentFrameFloat =	f32(
			m_CurrentFrameFloat + 
			f32(delta_ms) * m_OveralSpeed * m_CurrentAnimationSpeed
			);

		m_CurrentFrame = m_CurrentFrameFloat;

        if (m_CurrentFrame > m_EndFrame)
        {
			m_BeginFrame = m_CurrentFrame = m_EndFrame;
			m_CurrentFrameFloat = m_EndFrame;

			OnAnimationEnd(m_CurrentAnimationIdx);
        }		
    }

	u32 frame_count_shift = m_OveralFramesCount<<MD2_FRAME_SHIFT;

    if (m_CurrentFrame > frame_count_shift)
        m_CurrentFrame = (m_CurrentFrame % frame_count_shift);

	u32 frame_shift          = m_CurrentFrame>>MD2_FRAME_SHIFT;
	u32 startFrameLoop_shift = m_BeginFrame>>MD2_FRAME_SHIFT;
	u32 endFrameLoop_shift   = m_EndFrame>>MD2_FRAME_SHIFT;

	m_PreCurrentFrame = m_CurrentFrame = 0;

    if (m_EndFrame - m_BeginFrame == 0)
    {
        m_PreCurrentFrame  = frame_shift;
        m_CurrentFrame = frame_shift;
    }
    else
    {
		if (!m_Looping)
		{
			m_PreCurrentFrame = frame_shift;
			m_CurrentFrame = frame_shift + 1;

			CHECK_RANGE(m_PreCurrentFrame, startFrameLoop_shift, endFrameLoop_shift);
			CHECK_RANGE(m_CurrentFrame,    startFrameLoop_shift, endFrameLoop_shift);
		}
		else
		{
			m_PreCurrentFrame = frame_shift;

			if (!((endFrameLoop_shift) - (startFrameLoop_shift)))
				m_CurrentFrame = m_PreCurrentFrame;
			else
				m_CurrentFrame = (startFrameLoop_shift) + (
					((frame_shift)+1 - (startFrameLoop_shift)) % 
					((endFrameLoop_shift) - (startFrameLoop_shift))
					);
		}	

    }

	CHECK_RANGE(m_PreCurrentFrame,  0, m_OveralFramesCount-1);
	CHECK_RANGE(m_CurrentFrame,     0, m_OveralFramesCount-1);
}

//---------------------------------------------------------------------------

bool CAnimatedMeshMD2::setAnimationState(const SAnimationState &ani_state)
{
	if (setCurrentAnimation(ani_state.Animation, 0.0f))
	{
		m_CurrentFrame        = ani_state.AnimationTime;
		m_CurrentFrameFloat   = ani_state.AnimationTime;
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::recalculateBoundingBox()
{
	// update bbox
	if (m_OveralFramesCount>0)
		m_BoundingBox = m_BoxList[m_CurrentFrame];
	else
		m_BoundingBox.reset(0,0,0);
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshMD2::getMesh()
{
	vid::S3DVertex1TCoords * vertices = m_InterpolateBuffer.pointer();

	_getMeshGeometry(vertices);

	// init interface mesh buffer

	((SMD2MeshBuffer < vid::S3DVertex1TCoords > *)m_InterfaceMeshBuffer)->set(
		&m_Material,
		vertices, m_VerticesCount,
		m_Indices.pointer(), m_VerticesCount,
		&m_BoundingBox );

    return &m_InterfaceMesh;
}

//---------------------------------------------------------------------------

IMesh* CAnimatedMeshMD2::getMesh(f32 frame)
{
	m_BeginFrame = 0;
	m_EndFrame = m_OveralFramesCount<<MD2_FRAME_SHIFT;

	m_CurrentFrameFloat = m_EndFrame * (frame / (m_EndFrame - m_BeginFrame));
	m_CurrentFrame = m_CurrentFrameFloat + 0.5f;

	return getMesh();
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::uploadToRenderBuffers(IAnimatedMeshSceneNode *node)
{
	bool need_recreate = false;
	if (1 != node->getRenderBuffersCount())
		need_recreate = true;
	else
	{
		vid::IRenderBuffer *rb = node->getRenderBuffer(0);
		vid::E_DRAW_PRIMITIVE_TYPE dpt = vid::EDPT_TRIANGLE_LIST ;
		u32 vcnt = m_VerticesCount;
		u32 icnt = m_VerticesCount;
		vid::E_VERTEX_TYPE vtype = vid::EVT_1TCOORDS;
		E_INDEX_TYPE itype = EIT_16_BIT;

		if (dpt != rb->getDrawPrimitiveType()
				|| vcnt > rb->getVertices()->getMaxSize()
				|| icnt > rb->getIndices()->getMaxSize()
				|| (vcnt && vtype != rb->getVertices()->getType())
				|| (icnt && itype != rb->getIndices()->getType()))
			need_recreate = true;
	}
	if (need_recreate)
	{
		node->clearRenderBuffers();
		node->clearMaterials();

		vid::E_DRAW_PRIMITIVE_TYPE dpt = vid::EDPT_TRIANGLE_LIST ;
		u32 vcnt = m_VerticesCount;
		u32 icnt = m_VerticesCount;
		vid::E_VERTEX_TYPE vtype = vid::EVT_1TCOORDS;
		E_INDEX_TYPE itype = EIT_16_BIT;

		node->addMaterial(m_Material);
		node->addRenderBuffer(
			VIDEO_DRIVER.createRenderBuffer(
				getOveralFramesCount() == 1 ? 
					vid::ERBT_STATIC : vid::ERBT_DYNAMIC,
				vtype, vcnt, itype, icnt, dpt));
	}

	// prepare render buffers for uploading data

	vid::IRenderBuffer * rb = node->getRenderBuffer(0);

	vid::S3DVertex1TCoords * locked_v =
		(vid::S3DVertex1TCoords*)rb->getVertices()->lock();
	u16 * locked_i = (u16*)rb->getIndices()->lock();

	rb->getVertices()->setSize(m_VerticesCount);
	rb->getIndices()->setSize(m_VerticesCount);

	// upload animated mesh geometry directly into render buffers

	_getMeshGeometry(locked_v, locked_i);

	// unlocking render buffers, with updated render data

	rb->getVertices()->unlock();
	rb->getIndices()->unlock();
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::_getMeshGeometry(
	vid::S3DVertex1TCoords * & vertices, u16 * indices )
{
	bool external_dest = indices != NULL;

	if (!m_InterpolateAnimation)
	// not very smooth animation, but very fast
	{
		if (!external_dest)
			vertices = m_FrameList[m_CurrentFrame].pointer();
		else
			memcpy( vertices, m_FrameList[m_CurrentFrame].pointer(),
				m_VerticesCount * sizeof(vid::S3DVertex1TCoords) );
	}
	else
	// smooth linear interpolation between  two frames
	{
		static u32 one_shift = 1 << MD2_FRAME_SHIFT;

		f32 interpolation_weight = (m_CurrentFrame % (one_shift)) / (f32)(one_shift);
		f32 interpolation_weight_inv = 1.0f - interpolation_weight;		

		vid::S3DVertex1TCoords *target = vertices;		
		vid::S3DVertex1TCoords *first  = m_FrameList[m_PreCurrentFrame].pointer();
		vid::S3DVertex1TCoords *second = m_FrameList[m_CurrentFrame].pointer();

		for ( u32 i = 0; i < m_VerticesCount; ++i )
		{
			target->Pos    =
				second->Pos * interpolation_weight + first->Pos * interpolation_weight_inv;
			target->Normal = second->Normal;
			target->TCoords = first->TCoords;
		
			++target, ++first, ++second;
		}
	}

	if (external_dest)
		memcpy( indices, m_Indices.pointer(), m_VerticesCount * sizeof(u16) );
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::_calculateNormals()
{
    for ( u32 i = 0; i < m_OveralFramesCount; ++i )
    {
        vid::S3DVertex1TCoords* vtx = m_FrameList[i].pointer();

        for ( u32 j = 0; j < m_VerticesCount; j += 3 )
        {
            core::plane3d<f32> plane(
                vtx[m_Indices[j]].Pos, vtx[m_Indices[j+1]].Pos, vtx[m_Indices[j+2]].Pos);

            vtx[m_Indices[j  ]].Normal = plane.Normal;
            vtx[m_Indices[j+1]].Normal = plane.Normal;
            vtx[m_Indices[j+2]].Normal = plane.Normal;
        }
    }
}

//----------------------------------------------------------------------------

void CAnimatedMeshMD2::rescaleAnimatedMeshNormals(const core::vector3df &newScale) 
{
	m_NormalScale = newScale;
	for ( u32 f = 0; f < m_OveralFramesCount; ++f )
    {
		for ( u32 v = 0; v < m_VerticesCount; v++ )
		{
			m_FrameList[f][v].Normal.normalize();
			m_FrameList[f][v].Normal *= m_NormalScale;
		}
	}
}

//----------------------------------------------------------------------------

bool CAnimatedMeshMD2::setCurrentAnimation(u32 idx, f32 delay)
{ 
	if (idx >= m_Animations.size())
		return false; 

	u32 start = m_Animations[idx]->BeginFrame;
	u32 end   = m_Animations[idx]->EndFrame;

    if (start > end
			|| start >= m_OveralFramesCount
			|| end >= m_OveralFramesCount)
        return false;

	m_CurrentAnimationIdx = idx;

	m_BeginFrame            = start << MD2_FRAME_SHIFT;
	m_EndFrame              = end << MD2_FRAME_SHIFT;
	m_CurrentAnimationSpeed = m_Animations[idx]->Speed;
	m_Looping               = m_Animations[idx]->Looping;

	m_CurrentFrame     = m_BeginFrame;
	m_CurrentFrameFloat= m_BeginFrame;

	OnAnimationBegin(idx);

	return true;
}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::startRegisteringAnimations()
{}

//---------------------------------------------------------------------------

void CAnimatedMeshMD2::endRegisteringAnimations()
{
	// registering animations
	for (u32 i = 0; i < m_Animations.size(); i++)
	{
		const c8 *filename = m_Animations[i]->FileName.c_str();
		if (!filename)
		{
			LOGGER.logInfo(" Registering MD2 animation %d from %d to %d frames . . .", 
				m_Animations.size()+1, m_Animations[i]->BeginFrame, m_Animations[i]->EndFrame);
		}
	}
}

//---------------------------------------------------------------------------

f32 CAnimatedMeshMD2::getAnimationTimeSec()
{
	return (m_CurrentFrame - m_BeginFrame) /
		(1000.0f * m_OveralSpeed * m_CurrentAnimationSpeed);
}

//---------------------------------------------------------------------------

f32 CAnimatedMeshMD2::getAnimationDurationSec()
{
	return (m_EndFrame - m_BeginFrame) /
		(1000.0f * m_OveralSpeed * m_CurrentAnimationSpeed);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

