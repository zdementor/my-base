//|-------------------------------------------------------------------------
//| File:        CGeometryCreator.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGeometryCreatorHPP
#define CGeometryCreatorHPP
//----------------------------------------------------------------------------

#include <scn/IAnimatedMesh.h>
#include <img/IImage.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
namespace CGeometryCreator {
//----------------------------------------------------------------------------

IAnimatedMesh* createHillPlaneMesh(
    const core::dimension2df& tileSize, const core::dimension2di& tileCount,
    vid::SMaterial* material, f32 hillHeight, const core::dimension2df& countHills,
    const core::dimension2df& textureRepeatCount);

//----------------------------------------------------------------------------
//! creating custom geometry ( box, sphere, cylinfer ... )
//----------------------------------------------------------------------------

template <typename TVert>
IMeshBuffer* createBoxGeometry(core::vector3df box_size)
{
	s32 i; // new ISO for scoping problem with different compilers

    mesh_buffer<TVert>* buffer = new mesh_buffer<TVert>();

	// create vertices

	core::vector3df pos[12] =
	{
		core::vector3df(0,0,0), core::vector3df(1,0,0), core::vector3df(1,1,0), core::vector3df(0,1,0),
		core::vector3df(1,0,1), core::vector3df(1,1,1), core::vector3df(0,1,1), core::vector3df(0,0,1),
		core::vector3df(0,1,1), core::vector3df(0,1,0), core::vector3df(1,0,1), core::vector3df(1,0,0)
	};
	core::vector3df norm[12] =
	{
		core::vector3df(-1,-1,-1),
		core::vector3df( 1,-1,-1),
		core::vector3df( 1, 1,-1),
		core::vector3df(-1, 1,-1),
		core::vector3df( 1,-1, 1),
		core::vector3df( 1, 1, 1),
		core::vector3df(-1, 1, 1),
		core::vector3df(-1,-1, 1),
		core::vector3df(-1, 1, 1),
		core::vector3df(-1, 1,-1),
		core::vector3df( 1,-1, 1),
		core::vector3df( 1,-1,-1)
	};

	core::vector2df tcoords[12] =
	{
		core::vector2df(0, 1), core::vector2df(1, 1), core::vector2df(1, 0), core::vector2df(0, 0),
		core::vector2df(0, 1), core::vector2df(0, 0), core::vector2df(1, 0), core::vector2df(1, 1),
		core::vector2df(0, 1), core::vector2df(1, 1), core::vector2df(1, 0), core::vector2df(0, 0),
	};

    TVert Vertex;

	for (i = 0; i < 12; ++i)
    {
        Vertex.Pos = pos[i] - core::vector3df(0.5f, 0.5f, 0.5f);
        Vertex.Pos *= box_size;
		Vertex.TCoords = tcoords[i];
		Vertex.Normal = norm[i];
		buffer->Vertices.push_back(Vertex);
    }

	// create indices

	u16 Indices[36] = 
    {
		0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
		7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7
    };
    
    for (i=0; i<36; ++i)
		 buffer->Indices.push_back(Indices[i]);

	buffer->recalculateBoundingBox();
	mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)buffer);
    return buffer;
}

//----------------------------------------------------------------------------

template <typename TVert>
IMeshBuffer* createSphereGeometry(f32 radius, u32 segments)
{	
	mesh_buffer <TVert>* buffer = new mesh_buffer <TVert>();

	u32 ring   = segments/2;
	u32 sector = segments;

	u32	r;
	u32	s;
	f32	y;
	f32	prjRadius;    
    
	// vertices
    TVert Vertex;

	for( r = 0; r <= ring; ++r )
	{
		y = -cos( core::PI / f32( ring ) * f32( r ) );
		prjRadius = sqrt( 1.0f - y * y );
		
		for( s = 0; s <= sector; ++s )
		{
			core::vector3df n( 
				sin(2.0f * core::PI / f32(sector) * f32(s)) * prjRadius * radius, 
				y * radius, 
				cos(2.0f * core::PI / f32(sector) * f32(s)) * prjRadius * radius);
			Vertex.Pos = n;
			n.normalize();
			Vertex.Normal  = n;
			Vertex.TCoords = core::vector2df(
				y,
				(r == 0 || r == ring) ?
					(f32((2 * s + 1)) / (2.0f * sector)) : (f32(s) / f32(sector)));
			Vertex.TCoords.Y *= 4.0f;
			buffer->Vertices.push_back(Vertex);
		}
	}

	sector++;

	// indices
	for( r = 0; r < ring; ++r )
	{
		for( s = 0; s < sector; ++s )
		{		
			buffer->Indices.push_back( r * sector + s );
			buffer->Indices.push_back( ( r + 1 ) * sector + ( ( s + 1 ) % sector ) );
			buffer->Indices.push_back( ( r + 1 ) * sector + s );

			buffer->Indices.push_back( ( r + 1 ) * sector + ( ( s + 1 ) % sector ) );
			buffer->Indices.push_back( r * sector + s );
			buffer->Indices.push_back( r * sector + ( ( s + 1 ) % sector ) );
		}
	}

	buffer->recalculateBoundingBox();
	mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)buffer);
	return buffer;
}

//----------------------------------------------------------------------------

template <typename TVert>
IMeshBuffer* createCylinderGeometry( 
	f32 radius, f32 height, u32 segments)
{
	mesh_buffer<TVert>* buffer = new mesh_buffer<TVert>();

	u32 sector = segments;

	u32	s;

    u32 lowerRingStartVtx;
    u32 lowerVtx;
    u32 upperRingStartVtx;
    u32 upperVtx;
    
	// create vertices

    TVert Vertex;

	// lower cape
    // ring vertices

	lowerRingStartVtx = buffer->Vertices.size();
     
    for( s = 0; s < sector; ++s )
    {
		core::vector3df p( 
			sin( 2.0f * core::PI / f32( sector ) * f32( s ) ) * radius, 
			0.0f, 
            cos( 2.0f * core::PI / f32( sector ) * f32( s ) ) * radius );
        core::vector3df n = p;
            
		p.Y -= height / 2.0f;

		Vertex.Pos = p;
		n.normalize();
		Vertex.Normal  = n;
		Vertex.TCoords = core::vector2d<f32>(
			f32( s ) / f32( sector ), (height / 2.0f) / ( height + 2.0f * radius ) + 0.5f 
			);

		buffer->Vertices.push_back(Vertex);
    }

	// lower vertex

	lowerVtx = buffer->Vertices.size();

    Vertex.Pos = core::vector3df( 0.0f, -height / 2.0f, 0.0f );    
	Vertex.Normal = core::vector3df( 0.0f, -1.0f, 0.0f );
    Vertex.TCoords = core::vector2d<f32>( 0.0f, 0.0f );

    buffer->Vertices.push_back(Vertex);

    // upper cape
    // ring vertices

	upperRingStartVtx = buffer->Vertices.size();

    for( s = 0; s < sector; ++s )
    {
		core::vector3df p( 
			sin( 2.0f * core::PI / f32( sector ) * f32( s ) ) * radius, 
            0.0f, 
            cos( 2.0f * core::PI / f32( sector ) * f32( s ) ) * radius );
        core::vector3df n = p;
        p.Y += height / 2.0f;

		Vertex.Pos = p;
		n.normalize();
		Vertex.Normal  = n;
		Vertex.TCoords = core::vector2d<f32>(
			f32( s ) / f32( sector ), (height / 2.0f) / ( height + 2.0f * radius ) + 0.5f 
			);

		buffer->Vertices.push_back(Vertex);
        
    }

    // upper vertex

	upperVtx = buffer->Vertices.size();

	Vertex.Pos = core::vector3df( 0.0f, height / 2.0f, 0.0f );    
	Vertex.Normal = core::vector3df( 0.0f, -1.0f, 0.0f );
    Vertex.TCoords = core::vector2d<f32>( 0.0f, 1.0f );

    buffer->Vertices.push_back(Vertex);

    // create indices
    for( s = 0; s < sector; ++s )
    {
        // lower
        buffer->Indices.push_back( lowerVtx );
        buffer->Indices.push_back( lowerRingStartVtx + ( s + 1 ) % sector );
        buffer->Indices.push_back( lowerRingStartVtx + s );

        // upper
        buffer->Indices.push_back( upperVtx );
        buffer->Indices.push_back( upperRingStartVtx + s );
        buffer->Indices.push_back( upperRingStartVtx + ( s + 1 ) % sector );
    }
    
    // cylinder
    for( s = 0; s < sector; ++s )
    {
        buffer->Indices.push_back( lowerRingStartVtx + s );
        buffer->Indices.push_back( upperRingStartVtx + ( ( s + 1 ) % sector ) );
        buffer->Indices.push_back( upperRingStartVtx + s );

        buffer->Indices.push_back( upperRingStartVtx + ( ( s + 1 ) % sector ) );
        buffer->Indices.push_back( lowerRingStartVtx + s );
        buffer->Indices.push_back( lowerRingStartVtx + ( ( s + 1 ) % sector ) );
    }

	buffer->recalculateBoundingBox();
	mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)buffer);
	return buffer;
}

//----------------------------------------------------------------------------

template <typename TVert>
IMeshBuffer* createCapedCylinderGeometry( 
	f32 radius, f32 height, u32 segments)
{
	height -= 2*radius;

	CHECK_MIN_RANGE(height, 0);

	mesh_buffer<TVert> *buffer = new mesh_buffer<TVert>();

	u32 ring   = segments/4;
	u32 sector = segments;

	u32	r;
	u32	s;
	f32	y;
	f32	prjRadius;    

    u32 lowerRingStartVtx;
    u32 lowerVtx;
    u32 upperRingStartVtx;
    u32 upperVtx;
    
	// create vertices

    TVert Vertex;

	// lower cape
    // ring vertices

	lowerRingStartVtx = buffer->Vertices.size();

    for( r = 1; r <= ring; ++r )
    {
        y = -cos( core::PI / f32( ring ) * f32( r ) / 2.0f );
        prjRadius = sqrt( 1.0f - y * y );
        
        for( s = 0; s < sector; ++s )
        {
            core::vector3df p( 
                sin( 2.0f * core::PI / f32( sector ) * f32( s ) ) * prjRadius * radius, 
                y * radius, 
                cos( 2.0f * core::PI / f32( sector ) * f32( s ) ) * prjRadius * radius );
            core::vector3df n = p;
            p.Y -= height / 2.0f;

			Vertex.Pos = p;
			n.normalize();
			Vertex.Normal  = n;
			Vertex.TCoords = core::vector2d<f32>(
				f32( s ) / f32( sector ), y / ( height + 2.0f * radius ) + 0.5f);
			buffer->Vertices.push_back(Vertex);
		}
    }

	// lower vertex

	lowerVtx = buffer->Vertices.size();

    Vertex.Pos = core::vector3df( 0.0f, -height / 2.0f - radius, 0.0f );    
	Vertex.Normal = core::vector3df( 0.0f, -1.0f, 0.0f );
    Vertex.TCoords = core::vector2d<f32>( 0.0f, 0.0f );

    buffer->Vertices.push_back(Vertex);

    // upper cape
    // ring vertices

	upperRingStartVtx = buffer->Vertices.size();

    for( r = 1; r <= ring; ++r )
    {
        y = cos( core::PI / f32( ring ) * f32( r ) / 2.0f );
        prjRadius = sqrt( 1.0f - y * y );
        
        for( s = 0; s < sector; ++s )
        {
            core::vector3df p( 
                sin(2.0f * core::PI / f32(sector) * f32(s)) * prjRadius * radius, 
                y * radius, 
                cos(2.0f * core::PI / f32(sector) * f32(s)) * prjRadius * radius);
            core::vector3df n = p;
            p.Y += height / 2.0f;
			Vertex.Pos = p;
			n.normalize();
			Vertex.Normal  = n;
			Vertex.TCoords = core::vector2d<f32>(
				f32(s) / f32(sector), y / (height + 2.0f * radius) + 0.5f);
			buffer->Vertices.push_back(Vertex);
        }
    }

    // upper vertex

	upperVtx = buffer->Vertices.size();

	Vertex.Pos = core::vector3df( 0.0f, height / 2.0f + radius, 0.0f );    
	Vertex.Normal = core::vector3df( 0.0f, -1.0f, 0.0f );
    Vertex.TCoords = core::vector2d<f32>( 0.0f, 1.0f );

    buffer->Vertices.push_back(Vertex);

    // create indices
    for( s = 0; s < sector; ++s )
    {
        // lower
        buffer->Indices.push_back( lowerVtx );
        buffer->Indices.push_back( lowerRingStartVtx + ( s + 1 ) % sector );
        buffer->Indices.push_back( lowerRingStartVtx + s );

        // upper
        buffer->Indices.push_back( upperVtx );
        buffer->Indices.push_back( upperRingStartVtx + s );
        buffer->Indices.push_back( upperRingStartVtx + ( s + 1 ) % sector );
    }

    // ring
    for( r = 0; r < ring - 1; ++r )
    {
        for( s = 0; s < sector; ++s )
        {
            // lower
            buffer->Indices.push_back( lowerRingStartVtx + r * sector + s );
            buffer->Indices.push_back( lowerRingStartVtx + ( r + 1 ) * sector + ( ( s + 1 ) % sector ) );
            buffer->Indices.push_back( lowerRingStartVtx + ( r + 1 ) * sector + s );

            buffer->Indices.push_back( lowerRingStartVtx + ( r + 1 ) * sector + ( ( s + 1 ) % sector ) );
            buffer->Indices.push_back( lowerRingStartVtx + r * sector + s );
            buffer->Indices.push_back( lowerRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );

            // upper
            buffer->Indices.push_back( upperRingStartVtx + r * sector + s );
            buffer->Indices.push_back( upperRingStartVtx + ( r + 1 ) * sector + s );
            buffer->Indices.push_back( upperRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );

            buffer->Indices.push_back( upperRingStartVtx + ( r + 1 ) * sector + ( ( s + 1 ) % sector ) );
            buffer->Indices.push_back( upperRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );
            buffer->Indices.push_back( upperRingStartVtx + ( r + 1 ) * sector + s );
        }
    }
    
    // cylinder
    for( s = 0; s < sector; ++s )
    {
        r = ring - 1;
        buffer->Indices.push_back( lowerRingStartVtx + r * sector + s );
        buffer->Indices.push_back( upperRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );
        buffer->Indices.push_back( upperRingStartVtx + r * sector + s );

        buffer->Indices.push_back( upperRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );
        buffer->Indices.push_back( lowerRingStartVtx + r * sector + s );
        buffer->Indices.push_back( lowerRingStartVtx + r * sector + ( ( s + 1 ) % sector ) );
    }

	buffer->recalculateBoundingBox();
	mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)buffer);
	return buffer;
}

//----------------------------------------------------------------------------

template <typename TVert>
IMeshBuffer* createLimitedPlaneGeometry( 
	core::dimension2df dim, core::dimension2di tiles)
{
	mesh_buffer<TVert> *buffer = new mesh_buffer<TVert>();
	// TO DO
	buffer->recalculateBoundingBox();
	mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)buffer);
	return buffer;
}

//----------------------------------------------------------------------------

//! Generates adjacency information based on mesh indices.

u16* createAdjacencyData(
	core::vector3df* Vertices, u16* Indices, s32 IndexCount, f32 epsilon = 0.0001f);

//----------------------------------------------------------------------------
} // end namespace CGeometryCreator
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

