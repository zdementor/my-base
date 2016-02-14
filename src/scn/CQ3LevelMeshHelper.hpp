//|-------------------------------------------------------------------------
//| File:        CQ3LevelMeshHelper.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

template <class T> 
void toMyVertex(CQ3LevelMesh::tBSPVertex& bsp_vertex, T& my_vertex)
{
	my_vertex.Pos.X = bsp_vertex.vPosition[0];
	my_vertex.Pos.Y = bsp_vertex.vPosition[2];
	my_vertex.Pos.Z = bsp_vertex.vPosition[1];
	my_vertex.Normal.X = bsp_vertex.vNormal[0];
	my_vertex.Normal.Y = bsp_vertex.vNormal[2];
	my_vertex.Normal.Z = bsp_vertex.vNormal[1];
	my_vertex.TCoords.X = bsp_vertex.vTextureCoord[0];
	my_vertex.TCoords.Y = bsp_vertex.vTextureCoord[1];
}

//---------------------------------------------------------------------------

template <class T> 
void toBSPVertex(T& my_vertex, CQ3LevelMesh::tBSPVertex& bsp_vertex)
{
	bsp_vertex.vPosition[0] = my_vertex.Pos.X;
	bsp_vertex.vPosition[2] = my_vertex.Pos.Y;
	bsp_vertex.vPosition[1] = my_vertex.Pos.Z;
	bsp_vertex.vNormal[0] = my_vertex.Normal.X;
	bsp_vertex.vNormal[2] = my_vertex.Normal.Y;
	bsp_vertex.vNormal[1] = my_vertex.Normal.Z;
	bsp_vertex.vTextureCoord[0] = my_vertex.TCoords.X;
	bsp_vertex.vTextureCoord[1] = my_vertex.TCoords.Y;
	bsp_vertex.vLightmapCoord[0] = my_vertex.TCoords2.X;
	bsp_vertex.vLightmapCoord[1] = my_vertex.TCoords2.Y;
}

//---------------------------------------------------------------------------

s32 findSubDivLevel(core::vector3df &a, core::vector3df &b, core::vector3df &c)
{
	if (b==c || a==b)
		return 1;

	const s32 max_levels = 10;
    const f32 subdiv = 
		DEVICE.getDeviceFlagValue(dev::EDCF_HI_MAPS_DETAIL)==true ? 0.5f : 4.5f ;
    
	s32 level;

    f32 subdiv_v_stepq=subdiv*subdiv;

    core::vector3df s,t,d;

	for(level=0; level<max_levels; level++)
    {
		// Subdivide the 2 lines
        s = a.midPoint(b);
        t = b.midPoint(c);

        // Find the midpoint between the 2 midpoints
        c = s.midPoint(t);

        // Get the vector between this subdivided midpoint and the middle point of the original line
        d = c - b;

        // Find the squared length, and break when small enough
		if (d.getLengthSQ() < subdiv_v_stepq) 
		{
			break;
        }

        b=a; 
	}

	CHECK_RANGE(level, 1, max_levels);

    return level;
}

//---------------------------------------------------------------------------

core::vector3df toVector( CQ3LevelMesh::tBSPVertex& bsp_vertex)
{
	return core::vector3df(
		bsp_vertex.vPosition[0], bsp_vertex.vPosition[2], bsp_vertex.vPosition[1]
		);
}

//---------------------------------------------------------------------------

bool isEqual( CQ3LevelMesh::tBSPVertex &bsp_v1, CQ3LevelMesh::tBSPVertex &bsp_v2)
{
	return 
		core::math::Equals(bsp_v1.vPosition[0], bsp_v2.vPosition[0]) &&
		core::math::Equals(bsp_v1.vPosition[2], bsp_v2.vPosition[2]) &&
		core::math::Equals(bsp_v1.vPosition[1], bsp_v2.vPosition[1]) &&
		core::math::Equals(bsp_v1.vNormal[0], bsp_v2.vNormal[0]) &&
		core::math::Equals(bsp_v1.vNormal[2], bsp_v2.vNormal[2]) &&
		core::math::Equals(bsp_v1.vNormal[1], bsp_v2.vNormal[1]) &&
		core::math::Equals(bsp_v1.vTextureCoord[0], bsp_v2.vTextureCoord[0]) &&
		core::math::Equals(bsp_v1.vTextureCoord[1], bsp_v2.vTextureCoord[1]) &&
		core::math::Equals(bsp_v1.vLightmapCoord[0], bsp_v2.vLightmapCoord[0]) &&
		core::math::Equals(bsp_v1.vLightmapCoord[1], bsp_v2.vLightmapCoord[1]);
} 

//---------------------------------------------------------------------------

struct SBSPFace
{
	//! constructor 1
	SBSPFace() 
	{
		for (s32 i=0; i<3; i++)
			Indices[i]=i;

		LightMapCoordOffset = core::dimension2df(0,0); 
		LightMapCoordScale = 1.0f;
		LightMapID = -1;
	}

	//! constructor 2
    SBSPFace(s32 idx_a, s32 idx_b, s32 idx_c, core::dimension2df& lm_offset, f32 lm_scale, s32 lm_id)
	{
		Indices[0]=idx_a;
		Indices[1]=idx_b;
		Indices[2]=idx_c; 

		LightMapCoordOffset = lm_offset;
		LightMapCoordScale  = lm_scale;
		LightMapID          = lm_id;
	}

	//! constructor 3
    SBSPFace(const SBSPFace& other)
	{
		set(other);
	}

    //! operator =
    inline SBSPFace& operator=(const SBSPFace& other)
    {
		set(other);

		return *this;
    } 

	inline void set(const SBSPFace& other)
	{
		for (s32 i=0; i<3; i++)
			Indices[i]   =other.Indices[i];

		LightMapCoordOffset = other.LightMapCoordOffset;
		LightMapCoordScale  = other.LightMapCoordScale;
		LightMapID          = other.LightMapID;
	}

	s32 Indices[3];

	core::dimension2df LightMapCoordOffset; 
	f32                LightMapCoordScale;
	s32                LightMapID;
};

IMeshBuffer* MeshBufferForAddingSimpleBSPFaces=NULL;
core::array<SBSPFace> BSPFaces;
CQ3LevelMesh *Q3mesh = NULL;
core::array<s32> BSPVertIndices;

//---------------------------------------------------------------------------

void beginCreatingMesh(CQ3LevelMesh *q3mesh)
{
	Q3mesh = q3mesh;

	// pre allocate memory for indices
	BSPVertIndices.set_used(Q3mesh->NumVertices);
	BSPVertIndices.set_used(0);	
}

//---------------------------------------------------------------------------

void endCreatingMesh()
{
	BSPFaces.clear();
	BSPVertIndices.clear();
	Q3mesh = NULL;
}

//---------------------------------------------------------------------------

void beginAddingSimpleBSPFacesToMeshBuffer( IMeshBuffer* mb )
{
	MeshBufferForAddingSimpleBSPFaces=mb;
}

//---------------------------------------------------------------------------

void addSimpleBSPFaceToMeshBuffer(
	CQ3LevelMesh::tBSPFace &face, 	
	core::dimension2df lig_coord_ofs, f32 lig_coord_scl, s32 lig_id)
{
	s32 *meshverts   = Q3mesh->MeshVerts; 

	for (s32 tf=0; tf<face.numMeshVerts; tf+=3)
	{
		// calculating vertices indices

		static s32 indices[3];
		for (s32 i=0; i<3; i++)
		{
			indices[i] = face.vertexIndex + meshverts[face.meshVertIndex + tf + i];
		}
					
		BSPFaces.push_back(
			SBSPFace(
				indices[0], indices[1], indices[2],
				lig_coord_ofs, lig_coord_scl, lig_id
				)
			);
	}	
}

//---------------------------------------------------------------------------

void endAddingSimpleBSPFacesToMeshBuffer()
{
	IMeshBuffer*  mb     = MeshBufferForAddingSimpleBSPFaces;	
	CQ3LevelMesh& q3mesh = *Q3mesh;

	CQ3LevelMesh::tBSPVertex *vertices = q3mesh.Vertices;
	s32 num_verts     = q3mesh.NumVertices; 
	s32 *meshverts    = q3mesh.MeshVerts; 
	s32 num_meshverts = q3mesh.NumMeshVerts;
	s32 num_lmaps     = q3mesh.NumLightMaps;

	vid::SMaterial material = mb->getMaterial();

	img::SColor color = material.getPass(0).getEmissiveColor();
	s32 alpha = material.getPass(0).getDiffuseColor().getAlpha();

	bool alpha_const = material.getPass(0).getAlphaGen() == vid::EAGT_CUSTOM;
	bool rgb_const = material.getPass(0).getColorGen() == vid::ECGT_CUSTOM;

	vid::SLight light;

	s32 idx = mb->getVertexCount();	

	//********************************
	// compressing vertex indices	
	//********************************

	core::array< core::array<s32> > lmap_faces;
	core::array< core::array<s32> > remap_lmap_verts;
	{
		// creating remap arrays

		s32 lmaps_used_cnt = 0;
		core::array<s32> remap_lmaps_ids;
		{
			remap_lmaps_ids.set_used(num_lmaps+1);

			for (u32 l=0; l<remap_lmaps_ids.size(); l++)
			{
				remap_lmaps_ids[l] = -1;
			}
	        
			for (u32 f=0; f<BSPFaces.size(); f++)
			{
				if (BSPFaces[f].LightMapID>=0)
					remap_lmaps_ids[BSPFaces[f].LightMapID] = -2;
				else 
				if (BSPFaces[f].LightMapID==-1)
					remap_lmaps_ids[num_lmaps] = -2;
			}
			
			for (u32 l=0; l<remap_lmaps_ids.size(); l++)
			{
				if (remap_lmaps_ids[l]==-2)
					remap_lmaps_ids[l] = lmaps_used_cnt++;
			}
		}

		lmap_faces.set_used(lmaps_used_cnt);

		for (u32 f=0; f<BSPFaces.size(); f++)
		{
			f32 lig_id = BSPFaces[f].LightMapID;

			if (lig_id == -1)
				lig_id = num_lmaps;		

			s32 remapped_lmap_id = remap_lmaps_ids[lig_id];

			lmap_faces[remapped_lmap_id].push_back(f);
		}
		
		remap_lmap_verts.set_used(lmaps_used_cnt);

		core::array< s32 > lmap_verts_used_cnt;
		lmap_verts_used_cnt.set_used(lmaps_used_cnt);

		for (s32 l=0; l<lmaps_used_cnt; l++)
		{			
			remap_lmap_verts[l].set_used(num_verts);

			for (s32 v=0; v<num_verts; v++)
				remap_lmap_verts[l][v] = -1;			

			for (u32 lf=0; lf<lmap_faces[l].size(); lf++)
			{
				s32 f = lmap_faces[l][lf];

				for (s32 v=0; v<3; v++)
					remap_lmap_verts[l][BSPFaces[f].Indices[v]] = -2;
			}

			s32 verts_used_cnt = 0;

			for (s32 v=0; v<num_verts; v++)
			{
				if (remap_lmap_verts[l][v]==-2)
					remap_lmap_verts[l][v] = verts_used_cnt++;
			}

			lmap_verts_used_cnt[l] = verts_used_cnt;
		}

		// remapping indices

		s32 base_idx = 0;

		for (u32 l=0; l<lmap_faces.size(); l++)
		{
			for (u32 lf=0; lf<lmap_faces[l].size(); lf++)
			{
				s32 f = lmap_faces[l][lf];

				for (s32 v=0; v<3; v++)
				{
					BSPFaces[f].Indices[v] = base_idx + remap_lmap_verts[l][BSPFaces[f].Indices[v]];
				}
			}

			base_idx += lmap_verts_used_cnt[l];
		}
	}

	//********************************
	// creating geometry
	//********************************

	// adding vertices

	bool gl = VIDEO_DRIVER.getDriverFamily() == vid::EDF_OPENGL;

	for (u32 l=0; l<remap_lmap_verts.size(); l++)
	{
		if (lmap_faces[l].size()<=0)
			continue;

		core::dimension2df lig_coord_ofs = BSPFaces[lmap_faces[l][0]].LightMapCoordOffset;
		f32 lig_coord_scl                = BSPFaces[lmap_faces[l][0]].LightMapCoordScale;

		for (u32 lv=0; lv<remap_lmap_verts[l].size(); lv++)
		{
			if (remap_lmap_verts[l][lv]>=0)
			{
				s32 vidx = lv;

				if (mb->getVertexType()==vid::EVT_1TCOORDS)
				{
					vid::S3DVertex1TCoords v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );
			
					mb->addVertex(v, v.Type);			
				}
				else
				if (mb->getVertexType()==vid::EVT_1TCOORDS_COLOURED)
				{
					vid::S3DVertex1TCoordsColoured v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );					

					u8 r = bspv.color[0], g = bspv.color[1], b = bspv.color[2], a = bspv.color[3];

					// get color from light grid
					light = q3mesh.getLightFromPos(core::vector3df(v.Pos));
					img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
					c.setAlpha(alpha_const ? alpha : a);

					v.Color = gl ? c.toOpenGLColor() : c;
				
					mb->addVertex(v, v.Type);			
				}
				else
				if (mb->getVertexType()==vid::EVT_2TCOORDS)
				{
					vid::S3DVertex2TCoords v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );

					v.TCoords2.X = lig_coord_ofs.Width  + bspv.vLightmapCoord[0]*lig_coord_scl;
					v.TCoords2.Y = lig_coord_ofs.Height + bspv.vLightmapCoord[1]*lig_coord_scl;
			
					mb->addVertex(v, v.Type);					
				}
				else
				if (mb->getVertexType()==vid::EVT_2TCOORDS_COLOURED)
				{
					vid::S3DVertex2TCoordsColoured v;
					
					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );

					v.TCoords2.X = lig_coord_ofs.Width  + bspv.vLightmapCoord[0]*lig_coord_scl;
					v.TCoords2.Y = lig_coord_ofs.Height + bspv.vLightmapCoord[1]*lig_coord_scl;

					u8 r = bspv.color[0], g = bspv.color[1], b = bspv.color[2], a = bspv.color[3];

					// get color from light grid
					light = q3mesh.getLightFromPos(core::vector3df(v.Pos));
					img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
					c.setAlpha(alpha_const ? alpha : a);

					v.Color = gl ? c.toOpenGLColor() : c;
			
					mb->addVertex(v, v.Type);	
				}
				else
				if (mb->getVertexType()==vid::EVT_1TCOORDS_TBN)
				{
					vid::S3DVertex1TCoordsTBN v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );
			
					mb->addVertex(v, v.Type);								
				}
				else
				if (mb->getVertexType()==vid::EVT_1TCOORDS_TBN_COLOURED)
				{
					vid::S3DVertex1TCoordsTBNColoured v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );					

					u8 r = bspv.color[0], g = bspv.color[1], b = bspv.color[2], a = bspv.color[3];

					// get color from light grid
					light = q3mesh.getLightFromPos(core::vector3df(v.Pos));
					img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
					c.setAlpha(alpha_const ? alpha : a);

					v.Color = gl ? c.toOpenGLColor() : c;
					
					mb->addVertex(v, v.Type);
				}
				else
				if (mb->getVertexType()==vid::EVT_2TCOORDS_TBN)
				{
					vid::S3DVertex2TCoordsTBN v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );

					v.TCoords2.X = lig_coord_ofs.Width  + bspv.vLightmapCoord[0]*lig_coord_scl;
					v.TCoords2.Y = lig_coord_ofs.Height + bspv.vLightmapCoord[1]*lig_coord_scl;
				
					mb->addVertex(v, v.Type);		
				}
				else
				if (mb->getVertexType()==vid::EVT_2TCOORDS_TBN_COLOURED)
				{
					vid::S3DVertex2TCoordsTBNColoured v;

					CQ3LevelMesh::tBSPVertex &bspv = vertices[vidx];					

					toMyVertex( bspv, v );

					v.TCoords2.X = lig_coord_ofs.Width  + bspv.vLightmapCoord[0]*lig_coord_scl;
					v.TCoords2.Y = lig_coord_ofs.Height + bspv.vLightmapCoord[1]*lig_coord_scl;

					u8 r = bspv.color[0], g = bspv.color[1], b = bspv.color[2], a = bspv.color[3];

					// get color from light grid
					light = q3mesh.getLightFromPos(core::vector3df(v.Pos));
					img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
					c.setAlpha(alpha_const ? alpha : a);

					v.Color = gl ? c.toOpenGLColor() : c;
			
					mb->addVertex(v, v.Type);
				}		
			}
		}
	}

	// adding indices

	for (u32 l=0; l<lmap_faces.size(); l++)
	{
		for (u32 lf=0; lf<lmap_faces[l].size(); lf++)
		{
			s32 f = lmap_faces[l][lf];

			for (s32 v=0; v<3; v++)
			{
				mb->addIndex( BSPFaces[f].Indices[v] );
			}
		}
	}

	MeshBufferForAddingSimpleBSPFaces=NULL;
	BSPFaces.set_used(0);
}

//---------------------------------------------------------------------------

void addPatchBSPFaceToMeshBuffer(
	IMeshBuffer* mb, CQ3LevelMesh::tBSPFace &face, 
	core::dimension2df lig_coord_ofs = core::dimension2df(0,0), 
	f32 lig_coord_scl=1.0f)
{
	CQ3LevelMesh& q3mesh = *Q3mesh;

	vid::SMaterial material = mb->getMaterial();

	img::SColor color = material.getPass(0).getEmissiveColor();
	s32 alpha = material.getPass(0).getDiffuseColor().getAlpha();

	bool alpha_const = material.getPass(0).getAlphaGen() == vid::EAGT_CUSTOM;
	bool rgb_const = material.getPass(0).getColorGen() == vid::ECGT_CUSTOM;

	vid::SLight light;

	CQ3LevelMesh::tBSPVertex *vertices = q3mesh.Vertices;
	s32 num_verts     = q3mesh.NumVertices; 
	s32 *meshverts    = q3mesh.MeshVerts; 
	s32 num_meshverts = q3mesh.NumMeshVerts;

	s32 &width  = face.patchWidth;
	s32 &height = face.patchHeight;

	bool res = true;

	bool gl = VIDEO_DRIVER.getDriverFamily() == vid::EDF_OPENGL;

	s32 *vertidxs = new s32[width*height];

	s32 h=0, w=0;

	// control vertices indexes

	for( h=0; h<height-2; h+=2 )
	{
		for( w=0; w<width-2; w+=2 )
		{
			s32 row_width_col     = (h  ) * width + w;
			s32 row_width_col_1   = (h  ) * width + w + 1;
			s32 row_width_col_2   = (h  ) * width + w + 2;

			s32 row_1_width_col   = (h+1) * width + w;
			s32 row_1_width_col_1 = (h+1) * width + w + 1;
			s32 row_1_width_col_2 = (h+1) * width + w + 2;

			s32 row_2_width_col   = (h+2) * width + w;
			s32 row_2_width_col_1 = (h+2) * width + w + 1;
			s32 row_2_width_col_2 = (h+2) * width + w + 2;

			vertidxs[row_width_col    ] = row_width_col   + face.vertexIndex;
			vertidxs[row_width_col_1  ] = row_width_col_1 + face.vertexIndex;
			vertidxs[row_width_col_2  ] = row_width_col_2 + face.vertexIndex;

			vertidxs[row_1_width_col  ] = row_1_width_col   + face.vertexIndex;
			vertidxs[row_1_width_col_1] = row_1_width_col_1 + face.vertexIndex;
			vertidxs[row_1_width_col_2] = row_1_width_col_2 + face.vertexIndex;

			vertidxs[row_2_width_col  ] = row_2_width_col   + face.vertexIndex;
			vertidxs[row_2_width_col_1] = row_2_width_col_1 + face.vertexIndex;
			vertidxs[row_2_width_col_2] = row_2_width_col_2 + face.vertexIndex;
		}
	}

	// determine how much subdivision levels are required (based on the curvature of the surface)

	core::vector3df a,b,c;
	s32 u, v;
	bool found=false;

	// Find u level

	s32 subdiv_u = 1;
 
	for(v = 0; v < height; v++) 
	{
		for (u = 0; u < width-1; u += 2)
		{
			s32 v_width_u = v * width + u;

			a = toVector( vertices[ vertidxs[v_width_u  ] ] );
			b = toVector( vertices[ vertidxs[v_width_u+1] ] );
			c = toVector( vertices[ vertidxs[v_width_u+2] ] );
					
			if(a!=c) 
			{
				found=true;
				break;
			}
		}

		if(found) break;
	}

	if(!found) 
	{
		LOGGER.log(
			"Can't find suitable control points for determining U subdivision level",
			io::ELL_ERROR
			);
	}
	else
	{
		if (core::vector3df(a-b).getLengthSQ()<core::vector3df(b-c).getLengthSQ())
			subdiv_u = findSubDivLevel(a,b,c);
		else
			subdiv_u = findSubDivLevel(c,b,a);
	}

	found=false;

	// Find v level

	s32 subdiv_v = 1;

	for(u = 0; u < width; u++) 
	{
		for (v = 0; v < height-1; v += 2)
		{
			a = toVector( vertices[ vertidxs[(v  ) * width + u] ] );
			b = toVector( vertices[ vertidxs[(v+1) * width + u] ] );
			c = toVector( vertices[ vertidxs[(v+2) * width + u] ] );
				
			if(a!=c) 
			{
				found=true;
				break;
			}
		}

		if(found) break;
	}	
	
	if(!found) 
	{
		LOGGER.log(
			"Can't find suitable control points for determining V subdivision level",            
			io::ELL_ERROR
			);
	}
	else
	{
		if (core::vector3df(a-b).getLengthSQ()<core::vector3df(b-c).getLengthSQ())
			subdiv_v = findSubDivLevel(a,b,c);
		else
			subdiv_v = findSubDivLevel(c,b,a);
	}

	// making surface subdivision and creating vertices/indices

	f32 subdiv_v_step = 1.0f / (f32)subdiv_v;
	f32 subdiv_u_step = 1.0f / (f32)subdiv_u;

	f32 s_curr[3], t_curr[3], s_next[3], t_next[3];

	for( h=0; h<height-2&&res; h+=2 )
	{
		for( w=0; w<width-2&&res; w+=2 )
		{
			s32 row_width_col     = (h  ) * width + w;
			s32 row_1_width_col   = (h+1) * width + w;
			s32 row_2_width_col   = (h+2) * width + w;

			// 3x3 2D grid of control points for interpolation
			CQ3LevelMesh::tBSPVertex *bsp_verts[9] = 
			{
				&vertices[ vertidxs[row_width_col      ] ],
				&vertices[ vertidxs[row_width_col   + 1] ],
				&vertices[ vertidxs[row_width_col   + 2] ],
				&vertices[ vertidxs[row_1_width_col    ] ],
				&vertices[ vertidxs[row_1_width_col + 1] ],
				&vertices[ vertidxs[row_1_width_col + 2] ],
				&vertices[ vertidxs[row_2_width_col    ] ],
				&vertices[ vertidxs[row_2_width_col + 1] ],
				&vertices[ vertidxs[row_2_width_col + 2] ]
			}; 
			
			// Bezier spline ( quardratic ) polynomial interpolation 
			// uses 3 control points p0, p1, p2 and  parameter t=[0,1]
			// P(t) = (1-t)*(1-t)*p0 + 2.0f*(1-t)*t*p1 + t*t*p2

			for( s32 vi=0; vi<subdiv_v&&res; vi++ )
			{
				// calculating  bezier spline ( quardratic ) polynomial coefficients, along V-direction

				f32 v = vi*subdiv_v_step;

				s_curr[0] = (1.0f-v)*(1.0f-v);
				s_curr[1] = 2.0f * (1.0f - v) * v;
				s_curr[2] = v * v;

				v += subdiv_v_step;

				s_next[0] = (1.0f-v)*(1.0f-v);
				s_next[1] = 2.0f * (1.0f - v) * v;
				s_next[2] = v * v;

				for( s32 ui=0; ui<subdiv_u&&res; ui++ )
				{
					// calculating  bezier spline ( quardratic ) polynomial coefficients, along U-direction

					f32 u = ui*subdiv_u_step;

					t_curr[0] = (1.0f - u) * (1.0f - u);
					t_curr[1] = 2.0f * (1.0f - u) * u;
					t_curr[2] = u * u;

					u += subdiv_u_step;

					t_next[0] = (1.0f-u)*(1.0f-u);
					t_next[1] = 2.0f * (1.0f - u) * u;
					t_next[2] = u * u;

					// calculating interpolated vertices

					vid::S3DBaseVertex *tmp_verts[4] = { 0, 0, 0, 0 };

					for (s32 vidx=0; vidx<4; vidx++)
					{
						if (mb->getVertexType()==vid::EVT_1TCOORDS)
						{						
							vid::S3DVertex1TCoords &my_vert = *(new vid::S3DVertex1TCoords());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }							
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	

							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
								}
							}							
						}
						else
						if (mb->getVertexType()==vid::EVT_1TCOORDS_COLOURED)
						{						
							vid::S3DVertex1TCoordsColoured &my_vert = *(new vid::S3DVertex1TCoordsColoured());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }							
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);								

							u32 r=0, g=0, b=0, a=0;								
							
							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									r += s_t * bsp_vert->color[0];
									g += s_t * bsp_vert->color[1];
									b += s_t * bsp_vert->color[2];
									a += s_t * bsp_vert->color[3];
								}
							}

							// get color from light grid
							light = q3mesh.getLightFromPos(my_vert.Pos);
							img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
							c.setAlpha(alpha_const ? alpha : a);

							my_vert.Color = gl ? c.toOpenGLColor() : c;
						}
						else
						if (mb->getVertexType()==vid::EVT_2TCOORDS)
						{						
							vid::S3DVertex2TCoords &my_vert = *(new vid::S3DVertex2TCoords());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }	

							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	
							my_vert.TCoords2.set(0, 0);						

							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									my_vert.TCoords2.X += s_t * bsp_vert->vLightmapCoord[0];
									my_vert.TCoords2.Y += s_t * bsp_vert->vLightmapCoord[1];																
								}
							}

							my_vert.TCoords2.X = lig_coord_ofs.Width  + my_vert.TCoords2.X*lig_coord_scl;
							my_vert.TCoords2.Y = lig_coord_ofs.Height + my_vert.TCoords2.Y*lig_coord_scl;						
						}
						else
						if (mb->getVertexType()==vid::EVT_2TCOORDS_COLOURED)
						{						
							vid::S3DVertex2TCoordsColoured &my_vert = 
								*(new vid::S3DVertex2TCoordsColoured());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }						
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	
							my_vert.TCoords2.set(0, 0);	
							
							u32 r=0, g=0, b=0, a=0;	

							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									my_vert.TCoords2.X += s_t * bsp_vert->vLightmapCoord[0];
									my_vert.TCoords2.Y += s_t * bsp_vert->vLightmapCoord[1];
									r += s_t * bsp_vert->color[0];
									g += s_t * bsp_vert->color[1];
									b += s_t * bsp_vert->color[2];
									a += s_t * bsp_vert->color[3];
								}
							}

							// get color from light grid
							light = q3mesh.getLightFromPos(my_vert.Pos);
							img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
							c.setAlpha(alpha_const ? alpha : a);

							my_vert.Color = gl ? c.toOpenGLColor() : c;

							my_vert.TCoords2.X = lig_coord_ofs.Width  + my_vert.TCoords2.X*lig_coord_scl;
							my_vert.TCoords2.Y = lig_coord_ofs.Height + my_vert.TCoords2.Y*lig_coord_scl;												
						}
						else
						if (mb->getVertexType()==vid::EVT_1TCOORDS_TBN)
						{						
							vid::S3DVertex1TCoordsTBN &my_vert = 
								*(new vid::S3DVertex1TCoordsTBN());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }						
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	

							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
								}
							}							
						}
						else
						if (mb->getVertexType()==vid::EVT_1TCOORDS_TBN_COLOURED)
						{						
							vid::S3DVertex1TCoordsTBNColoured &my_vert = 
								*(new vid::S3DVertex1TCoordsTBNColoured());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }								
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	

							u32 r=0, g=0, b=0, a=0;								
							
							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									r += s_t * bsp_vert->color[0];
									g += s_t * bsp_vert->color[1];
									b += s_t * bsp_vert->color[2];
									a += s_t * bsp_vert->color[3];
								}
							}

							// get color from light grid
							light = q3mesh.getLightFromPos(my_vert.Pos);
							img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
							c.setAlpha(alpha_const ? alpha : a);

							my_vert.Color = gl ? c.toOpenGLColor() : c;				
						}
						else
						if (mb->getVertexType()==vid::EVT_2TCOORDS_TBN)
						{						
							vid::S3DVertex2TCoordsTBN &my_vert = 
								*(new vid::S3DVertex2TCoordsTBN());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }								
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	
							my_vert.TCoords2.set(0, 0);	
							
							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];
									
									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									my_vert.TCoords2.X += s_t * bsp_vert->vLightmapCoord[0];
									my_vert.TCoords2.Y += s_t * bsp_vert->vLightmapCoord[1];
								}
							}

							my_vert.TCoords2.X = lig_coord_ofs.Width  + my_vert.TCoords2.X*lig_coord_scl;
							my_vert.TCoords2.Y = lig_coord_ofs.Height + my_vert.TCoords2.Y*lig_coord_scl;						
						}
						else
						if (mb->getVertexType()==vid::EVT_2TCOORDS_TBN_COLOURED)
						{						
							vid::S3DVertex2TCoordsTBNColoured &my_vert = 
								*(new vid::S3DVertex2TCoordsTBNColoured());

							tmp_verts[vidx] = &my_vert;

							f32 *s, *t;
							s32 offset = 0;

							if      (vidx==0) { s=s_curr; t=t_curr; }
							else if (vidx==1) { s=s_curr; t=t_next; }
							else if (vidx==2) { s=s_next; t=t_curr; }
							else              { s=s_next; t=t_next; }							
								
							my_vert.Pos.set(0, 0, 0);
							my_vert.Normal.set(0, 0, 0);
							my_vert.TCoords.set(0, 0);	
							my_vert.TCoords2.set(0, 0);	
							
							u32 r=0, g=0, b=0, a=0;	

							for( s32 i=0; i<3; i++ )
							{
								for( s32 j=0; j<3; j++ )
								{
									f32 s_t = s[i] * t[j];

									CQ3LevelMesh::tBSPVertex* bsp_vert = bsp_verts[i*3+j];

									my_vert.Pos.X += s_t * bsp_vert->vPosition[0];
									my_vert.Pos.Y += s_t * bsp_vert->vPosition[2];
									my_vert.Pos.Z += s_t * bsp_vert->vPosition[1];
									my_vert.Normal.X += s_t * bsp_vert->vNormal[0];
									my_vert.Normal.Y += s_t * bsp_vert->vNormal[2];
									my_vert.Normal.Z += s_t * bsp_vert->vNormal[1];
									my_vert.TCoords.X += s_t * bsp_vert->vTextureCoord[0];
									my_vert.TCoords.Y += s_t * bsp_vert->vTextureCoord[1];
									my_vert.TCoords2.X += s_t * bsp_vert->vLightmapCoord[0];
									my_vert.TCoords2.Y += s_t * bsp_vert->vLightmapCoord[1];
									r += s_t * bsp_vert->color[0];
									g += s_t * bsp_vert->color[1];
									b += s_t * bsp_vert->color[2];
									a += s_t * bsp_vert->color[3];
								}
							}

							// get color from light grid
							light = q3mesh.getLightFromPos(my_vert.Pos);
							img::SColor c = rgb_const ? color : light.DiffuseColor.toSColor();
							c.setAlpha(alpha_const ? alpha : a);

							my_vert.Color = gl ? c.toOpenGLColor() : c;

							my_vert.TCoords2.X = lig_coord_ofs.Width  + my_vert.TCoords2.X*lig_coord_scl;
							my_vert.TCoords2.Y = lig_coord_ofs.Height + my_vert.TCoords2.Y*lig_coord_scl;												
						}
						else
						{
							res = false;
						}	
					}
					
					if (res)
					{
						// Put the vertices/indices in the mesh buffer

						s32 idx_cnt  = mb->getIndexCount();

						static s32 indices[4];

						for (s32 v=0; v<4; v++)
						{			
							if (vi==0 && ui==0)
							{
								mb->addVertex(tmp_verts[v], mb->getVertexType());
								indices[v] = mb->getVertexCount()-1;
							}
							else
							if (vi==0)
							{
								if (v==1||v==3)
								{
									mb->addVertex(tmp_verts[v], mb->getVertexType());
									indices[v] = mb->getVertexCount()-1;
								}
								else if (v==0) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -3);
								}
								else if (v==2) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -1);	
								}
							}
							else
							if (ui==0)
							{
								if (v==2||v==3)
								{
									mb->addVertex(tmp_verts[v], mb->getVertexType());
									indices[v] = mb->getVertexCount()-1;
								}
								else if (v==0) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -6*subdiv_u+1);	
								}
								else if (v==1) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -6*subdiv_u+5);
								}
							}
							else
							{
								if (v==3)
								{
									mb->addVertex(tmp_verts[v], mb->getVertexType());
									indices[v] = mb->getVertexCount()-1;
								}
								else if (v==0) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -3);
								}
								else if (v==2) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -1);
								}
								else if (v==1) 
								{
									indices[v]  =      mb->getIndex(idx_cnt -6*subdiv_u+5);	
								}
							}
						}						
						
						mb->addIndex(indices[0]);
						mb->addIndex(indices[2]);
						mb->addIndex(indices[1]);
						mb->addIndex(indices[1]);
						mb->addIndex(indices[2]);
						mb->addIndex(indices[3]);	
					}					

					// delete temp verices
					for (s32 vidx=0; vidx<4; vidx++)
						SAFE_DELETE(tmp_verts[vidx]);						
				}
			}			
		}
	}

	SAFE_DELETE_ARRAY(vertidxs);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------