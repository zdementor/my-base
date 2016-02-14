//|-------------------------------------------------------------------------
//| File:        CGeometryCreator.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGeometryCreator.h"

#include <scn/SAnimatedMesh.h>
#include <scn/SMesh.h>
#include <vid/IVideoDriver.h>
#include <os/ITimer.h>
#include <io/ILogger.h>
#include <scn/mesh_buffer.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
namespace CGeometryCreator {
//----------------------------------------------------------------------------

IAnimatedMesh* createHillPlaneMesh(
	const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tc,
    vid::SMaterial* material, f32 hillHeight, const core::dimension2d<f32>& ch,
    const core::dimension2d<f32>& textureRepeatCount
	)
{
    core::dimension2d<s32> tileCount = tc;
    tileCount.Height += 1;
    tileCount.Width += 1;

    core::dimension2d<f32> countHills = ch;
    
    mesh_buffer< vid::S3DVertex1TCoords >* buffer = new mesh_buffer< vid::S3DVertex1TCoords >();
    SMesh* mesh = new SMesh();
    vid::S3DVertex1TCoords vtx;
    vtx.Normal.set(0,1,0);

    if (countHills.Width < 0.01f) countHills.Width = 1;
    if (countHills.Height < 0.01f) countHills.Height = 1;

    float halfX = (tileSize.Width * tileCount.Width) / 2;
    float halfY = (tileSize.Height * tileCount.Height) / 2;

    // create vertices

    s32 x = 0;
    s32 y = 0;

    core::dimension2d<f32> tx;
    tx.Width = 1.0f / (tileCount.Width / textureRepeatCount.Width);
    tx.Height = 1.0f / (tileCount.Height / textureRepeatCount.Height);
    

    for (x=0; x<tileCount.Width; ++x)
        for (y=0; y<tileCount.Height; ++y)
        {
            vtx.Pos.set(tileSize.Width * x - halfX, 0, tileSize.Height * y - halfY);
            vtx.TCoords.set(-(f32)x * tx.Width, (f32)y * tx.Height);
            
            if (hillHeight)
                vtx.Pos.Y = (f32)(sin(vtx.Pos.X * countHills.Width * core::PI / halfX) *
                    cos(vtx.Pos.Z * countHills.Height * core::PI / halfY))
                    *hillHeight;

            buffer->Vertices.push_back(vtx);
        }

    // create indices

    for (x=0; x<tileCount.Width-1; ++x)
        for (y=0; y<tileCount.Height-1; ++y)
        {
            s32 current = y*tileCount.Width + x;

            buffer->Indices.push_back(current);
            buffer->Indices.push_back(current + 1);
            buffer->Indices.push_back(current + tileCount.Width);

            buffer->Indices.push_back(current + 1);
            buffer->Indices.push_back(current + 1 + tileCount.Width);
            buffer->Indices.push_back(current + tileCount.Width);
        }

    // recalculate normals
    for (s32 i=0; i<(s32)buffer->Indices.size(); i+=3)
    {
        core::plane3d<f32> p(
            buffer->Vertices[buffer->Indices[i+0]].Pos,
            buffer->Vertices[buffer->Indices[i+1]].Pos,
            buffer->Vertices[buffer->Indices[i+2]].Pos);
        p.Normal.normalize();

        buffer->Vertices[buffer->Indices[i+0]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[i+1]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[i+2]].Normal = p.Normal;
    }

    if (material)
        buffer->Material = *material;

    buffer->recalculateBoundingBox();

    SAnimatedMesh* animatedMesh = new SAnimatedMesh();
    mesh->addMeshBuffer(buffer);
    mesh->recalculateBoundingBox();
    animatedMesh->addMesh(mesh);
    animatedMesh->recalculateBoundingBox();

    mesh->drop();
    buffer->drop();

    return animatedMesh;
}

//----------------------------------------------------------------------------

u16* CGeometryCreator::createAdjacencyData(
	core::vector3df *Vertices, u16 *Indices, s32 IndexCount, f32 epsilon)
{
	if (IndexCount > 65535)
	{
		LOGGER.logErr("Can't create adjacency data, to many vertices!");
		return NULL;
	}

    u16* Adjacency = new u16[IndexCount];

    f32 t = 0;
    s32 f = 0;

    for (f=0; f<IndexCount; f++)
        Adjacency[f] = (u16)-1;
	
    s32 no_neighbors_error = 0;
    s32 to_many_neighbors_error = 0;

    // go through all faces and fetch their three neighbours
    for (f=0; f<IndexCount; f+=3)
    {
        for (s32 edge = 0; edge<3; ++edge)
        {
            core::vector3df v1 = Vertices[Indices[f+edge]];
            core::vector3df v2 = Vertices[Indices[f+((edge+1)%3)]];

            // now we search another face with these two 
            // vertices, which is not the current face.

            s32 of;

			s32 cnt1 = 0;
            s32 cnt2 = 0;
            
            for (of=0; of<IndexCount; of+=3)
            {
                if (of != f)
                {   
                    cnt1 = 0;
                    cnt2 = 0;

                    for (s32 e=0; e<3; ++e)
                    {
                        t = v1.getDistanceFromSQ(Vertices[Indices[of+e]]);
                        if (t <= epsilon && t >= -epsilon)
                            ++cnt1;
                        t = v2.getDistanceFromSQ(Vertices[Indices[of+e]]);
                        if (t <= epsilon && t >= -epsilon)
                            ++cnt2;
                    }

                    if (cnt1 == 1 && cnt2 == 1)
                        break;
                }
            }

            if (cnt1 ==0 || cnt2 == 0)
            {
                no_neighbors_error++;
            }
            else 
            {
                if (Adjacency[f + edge] != (u16)-1)
                    to_many_neighbors_error++;

                Adjacency[f + edge] = of / 3;
            }
        }
    }   

	return Adjacency;
}

//----------------------------------------------------------------------------
} // end namespace CGeometryCreator
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

