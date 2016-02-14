//|-------------------------------------------------------------------------
//| File:        CQ3LevelMeshBspTools.hpp
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

void CQ3LevelMesh::removePatches()
{	
	LOGGER.log("Removing patches from bsp file . . .");	

	// checking unused vertices and faces

	c8 *vert_used = new c8[NumVertices];
	c8 *face_used = new c8[NumFaces];

	memset(vert_used, 0x00, NumVertices*sizeof(c8));
	memset(face_used, 0x00, NumFaces*sizeof(c8));
	
	s32 vert_used_cnt=0, face_used_cnt=0;

	for (s32 m=0; m<NumModels; m++)
	{
		s32 face_start_idx = Models[m].faceIndex;
		s32 face_end_idx   = Models[m].faceIndex + Models[m].numOfFaces;

		for (s32 i=face_start_idx; i<face_end_idx; ++i)
		{	
			if (Faces[i].type==1 || Faces[i].type==3)
			{
				for (s32 tf=0; tf<Faces[i].numMeshVerts; tf++)
				{
					// calculating vertex index
					s32 vidx = 
						Faces[i].vertexIndex +                   // The index into this face's first vertex 
						MeshVerts[Faces[i].meshVertIndex + tf];  // The vertex offset for a mesh

					if (!vert_used[vidx])
					{
						vert_used[vidx] = 1;	
						
						vert_used_cnt++;
					}
				}

				if (!face_used[i])
				{
					face_used[i] = 1;

					face_used_cnt++;
				}
			}
		}
	}

	s32 vert_not_used_cnt = NumVertices - vert_used_cnt;
	s32 face_not_used_cnt = NumFaces    - face_used_cnt;

	core::list<s32> verts_idxs_not_used;
	core::list<s32> faces_idxs_not_used;

	for (s32 v=0; v<NumVertices; v++)
	{
		if (!vert_used[v])
		{
			verts_idxs_not_used.push_back(v);
		}
	}

	for (s32 f=0; f<NumFaces; f++)
	{
		if (!face_used[f])
		{
			faces_idxs_not_used.push_back(f);
		}
	}

	// checking new verts and faces

	core::list<tBSPVertex> new_verts;
	core::list<tBSPFace>   new_faces;

	s32 mesh_vert_first_idx = NumMeshVerts;

	beginCreatingMesh(this);

	for (s32 f=0; f<NumFaces; ++f)
	{	
		if (!face_used[f]&& Faces[f].type==2)
		{
			mesh_buffer< vid::S3DVertex2TCoords > mb;

			addPatchBSPFaceToMeshBuffer(&mb, Faces[f]);

			s32 isize = mb.Indices.size();

			Faces[f].type = 1;				
			Faces[f].numOfVerts    = isize;
			Faces[f].meshVertIndex = mesh_vert_first_idx;
			Faces[f].numMeshVerts  = isize;

			mesh_vert_first_idx += isize;			

			for (s32 i=0; i<isize; i+=3)
			{
				tBSPVertex bsp_v1, bsp_v2, bsp_v3;

				vid::S3DVertex2TCoords my_v1 = mb.Vertices[mb.Indices[i  ]];
				vid::S3DVertex2TCoords my_v2 = mb.Vertices[mb.Indices[i+1]];
				vid::S3DVertex2TCoords my_v3 = mb.Vertices[mb.Indices[i+2]];

				toBSPVertex( my_v1, bsp_v1 );
				toBSPVertex( my_v2, bsp_v2 );
				toBSPVertex( my_v3, bsp_v3 );

				new_verts.push_back( bsp_v1 );	
				new_verts.push_back( bsp_v2 );	
				new_verts.push_back( bsp_v3 );	
			}
		}		
	}

	endCreatingMesh();

	// adding new vertices

	core::array<s32> new_verts_idxs;

	while (verts_idxs_not_used.size()>0 && new_verts.size()>0)
	{
		s32 vidx       = *verts_idxs_not_used.begin();
		Vertices[vidx] = *new_verts.begin();
		
		new_verts_idxs.push_back(vidx);

		verts_idxs_not_used.erase(verts_idxs_not_used.begin());
		new_verts.erase(new_verts.begin());
	}	

	Lumps[kVertices].length = (NumVertices + new_verts.size())*sizeof(tBSPVertex);

	tBSPVertex *vertices = new tBSPVertex[NumVertices + new_verts.size()];

	memcpy(vertices, Vertices, NumVertices*sizeof(tBSPVertex)); 

	core::list<tBSPVertex>::iterator iter = new_verts.begin();

	s32 cnt = 0;

	for (; iter!=new_verts.end(); ++iter, cnt++)
	{
		s32 vidx = NumVertices+cnt;

		vertices[vidx] = *iter;

		new_verts_idxs.push_back(vidx);
	}

	NumVertices += new_verts.size();

	SAFE_DELETE_ARRAY(Vertices);

	Vertices = vertices;	

	// adding new mesh vertices

	Lumps[kMeshVerts].length = (NumMeshVerts+new_verts_idxs.size())*sizeof(s32);

	s32 *mesh_verts = new s32[NumMeshVerts+new_verts_idxs.size()];

	memcpy(mesh_verts, MeshVerts, NumMeshVerts*sizeof(s32)); 

	// updating faces and mesh vertices arrays

	s32 face_first_vert_idx = 0;
	s32 mv_count = NumMeshVerts;

	for (s32 f=0; f<NumFaces; ++f)
	{	
		if (!face_used[f])
		{	
			Faces[f].vertexIndex = new_verts_idxs[face_first_vert_idx];			

			for (s32 mv=0; mv<Faces[f].numMeshVerts; mv++, mv_count++)
			{
				if (mv==0)
				{
					mesh_verts[mv_count] = 0;
				}
				else
				{
					mesh_verts[mv_count] = 
						new_verts_idxs[face_first_vert_idx+mv]-Faces[f].vertexIndex;
				}
			}

			face_first_vert_idx += Faces[f].numOfVerts;
		}
	}

	NumMeshVerts += new_verts_idxs.size();

	SAFE_DELETE_ARRAY(MeshVerts);

	MeshVerts = mesh_verts;
	
	// free mem

	SAFE_DELETE_ARRAY(vert_used);
	SAFE_DELETE_ARRAY(face_used);

	LOGGER.increaseFormatLevel();
	for (s32 i=0; i<kMaxLumps; i++)
	{
		printBspInfo((eLumps)i);
	}
	LOGGER.decreaseFormatLevel();
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::collapseVertices()
{
	LOGGER.log("Collapsing BSP vertices . . .");

	s32 initial_verts_num = NumVertices;

	s32 i;

	s32 *remap_idxs = new s32[NumVertices];
	s32 *offset_idxs = new s32[NumVertices];
	bool *rem_flags = new bool[NumVertices];

	for (i=0; i<NumVertices; i++)
	{
		remap_idxs[i] = -1;
		offset_idxs[i] = 0;
		rem_flags[i] = false;
	}

	// checking doublicate verts

	for (i=0; i<NumVertices; i++)
	{	
		if (remap_idxs[i]!=-1)
			continue;

		for (s32 j=i+1; j<NumVertices; j++)
		{
			if (remap_idxs[j]==-1 && isEqual(Vertices[i], Vertices[j]))
			{
				remap_idxs[j] = i;				
			}
		}
	}

	s32 new_verts_num = 0;

	for (i=0; i<NumVertices; i++)
	{	
		if (remap_idxs[i]==-1)
		{
			new_verts_num++;			
		}
		else
		{
			rem_flags[i] = true;
		}
	}

	s32 offset_remap_idx=0;;

	for (i=0; i<NumVertices; i++)
	{	
		offset_idxs[i] = offset_remap_idx;

		if (remap_idxs[i]!=-1)	
		{
			offset_remap_idx++;		
		}						
	}

	for (i=0; i<NumVertices; i++)
	{	
		if (remap_idxs[i]!=-1)	
		{
			remap_idxs[i] = remap_idxs[i] - offset_idxs[remap_idxs[i]];	
		}
		else
		{
			remap_idxs[i] = i - offset_idxs[i];	
		}		
	}

	// updating faces (remaping vertex indices)

	s32 new_mesh_verts_size = 0;

	for (i=0; i<NumFaces; i++)
	{
		new_mesh_verts_size += Faces[i].numMeshVerts;
	}

	s32 *old_mesh_verts = MeshVerts;	

	MeshVerts = new s32[new_mesh_verts_size];
	NumMeshVerts = new_mesh_verts_size;

	Lumps[kMeshVerts].length = NumMeshVerts*sizeof(s32);

	for (s32 m=0; m<NumModels; m++)
	{	
		s32 face_start_idx = Models[m].faceIndex;
		s32 face_end_idx   = Models[m].faceIndex + Models[m].numOfFaces;

		for (s32 i=face_start_idx; i<face_end_idx; i++)
		{	
			s32 old_mesh_vert_index = Faces[i].meshVertIndex;

			Faces[i].meshVertIndex = 0;

			for (s32 f=0; f<i; f++)
			{	
				Faces[i].meshVertIndex += Faces[f].numMeshVerts;
			}
				
			if (Faces[i].type==1||Faces[i].type==3)
			{
				for (s32 tf=0; tf<Faces[i].numMeshVerts; tf++)
				{	
					s32 vidx = Faces[i].vertexIndex + old_mesh_verts[old_mesh_vert_index + tf];					

					MeshVerts[Faces[i].meshVertIndex + tf] = remap_idxs[vidx] - Faces[i].vertexIndex;					
				}
			}
		}
	}

	SAFE_DELETE_ARRAY(old_mesh_verts);	

	// removing unused verts

	tBSPVertex *vertices = new tBSPVertex[new_verts_num];

	s32 v=0;	

	for (i=0, v=0; i<NumVertices && v<new_verts_num; i++)
	{	
		if (rem_flags[i]==false)
		{
			vertices[v] = Vertices[i];
			v++;
		}
	}	

	SAFE_DELETE_ARRAY(Vertices);

	NumVertices = new_verts_num;
	Vertices = vertices;

	Lumps[kVertices].length = NumVertices*sizeof(tBSPVertex);

	SAFE_DELETE_ARRAY(offset_idxs);
	SAFE_DELETE_ARRAY(remap_idxs);
	SAFE_DELETE_ARRAY(rem_flags);	

	LOGGER.increaseFormatLevel();
	for (i=0; i<kMaxLumps; i++)
	{
		printBspInfo((eLumps)i);
	}
	LOGGER.decreaseFormatLevel();
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::generateMaxImportScript(const c8* filename, f32 light_intens_mul, f32 light_radius_mul)
{
	LOGGER.log("Generating 3DSMax import script . . .");

	io::IWriteFile *scr = FILE_SYSTEM.createAndWriteFile(filename);

	if (!scr)
	{
		LOGGER.log("Can't write", filename, io::ELL_ERROR);

		return;
	}

	core::stringc map_fname = core::extractFileName(LevelName);
	core::changeFileExt(map_fname, "map");

	core::stringc map_fullfname = LevelPath;
	map_fullfname.append(map_fname);

	core::stringc tex_path = LevelPath;
	tex_path.append("../");
	tex_path.replace_char('\\','/');

	io::IReadFile *map = 
		FILE_SYSTEM.createAndOpenFile(map_fullfname.c_str());

	if (!map)
	{
		SAFE_DROP(scr);

		LOGGER.log("Can't read:", map_fname.c_str(), io::ELL_ERROR);

		return;
	}

	core::stringc ent_str = CQ3MapFileParser::extractEntitiesString(map);

	c8 *tmp_entity_str = new c8[ent_str.size()];
	memcpy(tmp_entity_str, ent_str.c_str(), ent_str.size()*sizeof(c8));

	CQ3EntityParser::parseEntities(tmp_entity_str);

	SAFE_DELETE_ARRAY(tmp_entity_str);

	c8 ch[1024];
	s32 f=0;

	///////////////////////////////////////////////////
	// groupping faces by lightmaps and textures
	///////////////////////////////////////////////////
	
	// checking out what faces are in use

	bool *face_used = new bool[NumFaces];		
	memset(face_used, 0x00, NumFaces);

	s32 *face_model = new s32[NumFaces];
	for (f=0; f<NumFaces; f++)
	{
		face_model[f] = -1;
	}

	core::stringc str;

	for (s32 m=0; m<NumModels; m++)
	{	
		s32 face_start_idx = Models[m].faceIndex;
		s32 face_end_idx   = Models[m].faceIndex + Models[m].numOfFaces - 1;		

		for (s32 f=face_start_idx; f<=face_end_idx; ++f)
		{
			if ((	Faces[f].textureID != -1 &&
					core::stringc(Textures[Faces[f].textureID].strName) == core::stringc("textures/common/caulk")	
				) ||
				(Faces[f].type!=1&&Faces[f].type!=3)
				)
			{
				continue;
			}

			face_used[f] = true;
			face_model[f] = m;
		}

		str.sprintf("Model %d constists from %d - %d faces", m, face_start_idx, face_end_idx);	
		LOGGER.log(str.c_str());
	}

	// grouping faces by lightmaps

	core::array<s32> *lmap_face = new core::array<s32>[NumLightMaps+1];

	s32 skip_faces = 0;;

	for (f=0; f<NumFaces; f++)
	{
		if (Faces[f].textureID != -1 &&
			core::stringc(Textures[Faces[f].textureID].strName) == core::stringc("textures/common/caulk")
			)
		{
			skip_faces++;

			continue;				
		}

		if (Faces[f].type!=1&&Faces[f].type!=3)
		{
			skip_faces++;

			continue;
		}

		if (Faces[f].lightmapID>=0 && Faces[f].lightmapID<NumLightMaps)
		{
			lmap_face[Faces[f].lightmapID].push_back(f);
		}
		else
		{
			lmap_face[NumLightMaps].push_back(f);
		}
	}

	// groupping faces by textures inside lightmaps groups	
	
	core::array< core::array< core::array<s32> > > lmap_tex_face;
	lmap_tex_face.set_used(NumLightMaps+1);

	for (s32 l=0; l<(NumLightMaps+1); l++)
	{
		core::array<s32> face_materials;
		core::array<core::stringc> textures;	

		for (u32 f=0; f<lmap_face[l].size(); f++)
		{
			core::stringc newtex = 
				core::stringc( Textures[ Faces[lmap_face[l][f] ].textureID].strName );

			vid::SMaterial material;
			vid::E_VERTEX_TYPE vtype;

			IMaterialsManager::getSingleton().getMaterialAndVertexType(
				Textures[ Faces[lmap_face[l][f] ].textureID].strName, 0, material, vtype
				);

			if (material.getPass(0).isTransparent())
				continue;

			bool already_exists = false;

			for (u32 t=0; t<textures.size(); t++)
			{
				if ( newtex==textures[t] && face_used[lmap_face[l][f]] )
				{
					lmap_tex_face[l][t].push_back(lmap_face[l][f]);
					already_exists = true;
					break;
				}
			}

			if ( newtex.size()>0 && !already_exists && face_used[lmap_face[l][f]] )
			{
				textures.push_back(newtex);
				lmap_tex_face[l].push_back( core::array<s32>() );
				lmap_tex_face[l][lmap_tex_face[l].size()-1].push_back( lmap_face[l][f] );
			}		
		}
	}


	SAFE_DELETE_ARRAY(face_used);

	core::array< core::array<s32> > additional_lmap_meshes_idxs; 
	additional_lmap_meshes_idxs.set_used(NumLightMaps+1);	

	core::array< core::array< core::array<s32> > > used_lmap_tex_faces_idxs;
	used_lmap_tex_faces_idxs.set_used(NumLightMaps+1);

	for (s32 l=0; l<(NumLightMaps+1); l++)
	{
		used_lmap_tex_faces_idxs[l].set_used(NumTextures);
	}

	////////////////////////////////////////////////
	// importing geometry
	////////////////////////////////////////////////	

	s32 *remap_idxs = new s32[NumVertices];

	sprintf(
		ch, 
		"max select all\n"
		"max delete\n\n"
		);
	scr->write(ch, (u32)strlen(ch));

	sprintf(
		ch, 
		"progressstart \"Step 1 of 3 . . . \"\n\n"
		);
	scr->write(ch, (u32)strlen(ch));	

	s32 mesh_cnt=0, mesh_idx=0;

	for (s32 l=0; l<(s32)(NumLightMaps+1); l++)
	{
		for ( u32 t=0; t<lmap_tex_face[l].size(); t++)
		{
			if (lmap_tex_face[l][t].size()<=0)
				continue;

			mesh_cnt++;

			for (u32 f=0; f<lmap_tex_face[l][t].size(); f++)
			{
				if (face_model[lmap_tex_face[l][t][f]]>0)
				{
					mesh_cnt++;
					break;
				}
			}
		}
	}		

	for (s32 l=0, mesh_idx=0; l<(s32)(NumLightMaps+1); l++)
	{
		for ( u32 t=0; t<lmap_tex_face[l].size(); t++)
		{
			if (lmap_tex_face[l][t].size()<=0)
				continue;			

			core::stringc texture_tga = tex_path;
			texture_tga.append( Textures[Faces[lmap_tex_face[l][t][0]].textureID].strName );
			texture_tga.append(".tga");
				
			core::stringc texture_jpg  = tex_path;
			texture_jpg.append( Textures[Faces[lmap_tex_face[l][t][0]].textureID].strName );
			texture_jpg.append(".jpg");

			core::stringc tex_fname = "";

			if (FILE_SYSTEM.existFile(texture_tga.c_str()))
			{
				tex_fname = texture_tga;
			}
			else
			if (FILE_SYSTEM.existFile(texture_jpg.c_str()))
			{
				tex_fname = texture_jpg;
			}	
			
			vid::SMaterial material;
			vid::E_VERTEX_TYPE vtype=vid::EVT_2TCOORDS;

			IMaterialsManager::getSingleton().getMaterialAndVertexType(
				Textures[Faces[lmap_tex_face[l][t][0]].textureID].strName, 0, material, vtype
				);						

			for (s32 model_group=0; model_group<2; model_group++)
			{
				core::array<s32> used_idxs, remaped_idxs;

				for ( u32 face=0; face<lmap_tex_face[l][t].size(); face++)
				{
					s32 f = lmap_tex_face[l][t][face];	
					
					if ((model_group==0 && face_model[f]>0) || 
						(model_group==1 && face_model[f]<1) ||
						ModelEntities[face_model[f]].Type==EBSPET_FUNC_DOOR						
						)
					{
						continue;						
					}					

					for (s32 v=0; v<NumVertices; v++)		
					{
						remap_idxs[v] = -1;
					}									

					for (s32 tf=0; tf<Faces[f].numMeshVerts; tf++)
					{		
						s32 vidx = Faces[f].vertexIndex + MeshVerts[Faces[f].meshVertIndex + tf];
						
						if (remap_idxs[vidx]==-1)	
						{
							remap_idxs[vidx] = used_idxs.size();

							used_idxs.push_back(vidx);
						}
						
						remaped_idxs.push_back(remap_idxs[vidx]);
					}
				}

				if (used_idxs.size()==0)
					continue;

				bool full_transparent = false;

				s32 fidx = lmap_tex_face[l][t][0];				

				s32 f = ( model_group==0 ) ? ( fidx ) : ( fidx+NumFaces );

				if (model_group==0)
				{
					used_lmap_tex_faces_idxs[l][t].push_back(f);
				}
				else
				if (model_group==1)
				{
					additional_lmap_meshes_idxs[l].push_back(f);					
				}

				// vertex list
					
				for (u32 v=0; v<used_idxs.size(); v++)
				{
					if (v==0)
					{
						sprintf(ch, "Vertices%d=\n#(\n", f);		
						scr->write(ch, (u32)strlen(ch));
					}

					s32 vidx = used_idxs[v];					
							
					if (v==(used_idxs.size()-1))
					{
						sprintf(ch, "[%.f,%.f,%.f]\n", Vertices[vidx].vPosition[0], Vertices[vidx].vPosition[1], Vertices[vidx].vPosition[2]);
						scr->write(ch, (u32)strlen(ch));

						sprintf(ch, ")\n");		
						scr->write(ch, (u32)strlen(ch));
					}
					else
					{
						sprintf(ch, "[%.f,%.f,%.f],\n", Vertices[vidx].vPosition[0], Vertices[vidx].vPosition[1], Vertices[vidx].vPosition[2]);
						scr->write(ch, (u32)strlen(ch));
					}
				}

				for (u32 v=0; v<used_idxs.size(); v++)
				{
					if (v==0)
					{
						sprintf(ch, "Normals%d=\n#(\n", f);		
						scr->write(ch, (u32)strlen(ch));
					}

					s32 vidx = used_idxs[v];					
							
					if (v==(used_idxs.size()-1))
					{
						sprintf(ch, "[%.1f,%.1f,%.1f]\n", Vertices[vidx].vNormal[0], Vertices[vidx].vNormal[1], Vertices[vidx].vNormal[2]);
						scr->write(ch, (u32)strlen(ch));

						sprintf(ch, ")\n");		
						scr->write(ch, (u32)strlen(ch));
					}
					else
					{
						sprintf(ch, "[%.1f,%.1f,%.1f],\n", Vertices[vidx].vNormal[0], Vertices[vidx].vNormal[1], Vertices[vidx].vNormal[2]);
						scr->write(ch, (u32)strlen(ch));
					}
				}

				// tcoords list

				for (u32 v=0; v<used_idxs.size(); v++)
				{
					if (v==0)
					{
						sprintf(ch, "TextureCoords%d=\n#(\n", f);		
						scr->write(ch, (u32)strlen(ch));
					}
					
					s32 vidx = used_idxs[v];					
							
					if (v==(used_idxs.size()-1))
					{
						sprintf(ch, "[%.2f,%.2f,0]\n", Vertices[vidx].vTextureCoord[0], 1-Vertices[vidx].vTextureCoord[1]);
						scr->write(ch, (u32)strlen(ch));

						sprintf(ch, ")\n");		
						scr->write(ch, (u32)strlen(ch));
					}
					else
					{
						sprintf(ch, "[%.2f,%.2f,0],\n", Vertices[vidx].vTextureCoord[0], 1-Vertices[vidx].vTextureCoord[1]);
						scr->write(ch, (u32)strlen(ch));
					}
				}

				// tcoords2 list

				for (u32 v=0; v<used_idxs.size(); v++)
				{
					if (v==0)
					{
						sprintf(ch, "LightmapCoords%d=\n#(\n", f);		
						scr->write(ch, (u32)strlen(ch));
					}
					
					s32 vidx = used_idxs[v];	

					f32 tx = 0;
					f32 ty = 0;

					if (vtype==vid::EVT_2TCOORDS||
						vtype==vid::EVT_2TCOORDS_COLOURED||
						vtype==vid::EVT_2TCOORDS_TBN||
						vtype==vid::EVT_2TCOORDS_TBN_COLOURED
						)
					{
						tx = Vertices[vidx].vLightmapCoord[1];
						ty = 1-Vertices[vidx].vLightmapCoord[0];
					}
							
					if (v==(used_idxs.size()-1))
					{
						sprintf(ch, "[%.6f,%.6f,0]\n", tx, ty);
						scr->write(ch, (u32)strlen(ch));

						sprintf(ch, ")\n");		
						scr->write(ch, (u32)strlen(ch));
					}
					else
					{
						sprintf(ch, "[%.6f,%.6f,0],\n", tx, ty);
						scr->write(ch, (u32)strlen(ch));
					}
				}

				// index list

				for (u32 v=0; v<remaped_idxs.size(); v+=3)
				{
					if (v==0)
					{
						sprintf(ch, "Faces%d=\n#(\n", f);		
						scr->write(ch, (u32)strlen(ch));
					}

					s32 vidx1 = remaped_idxs[v  ] + 1;
					s32 vidx2 = remaped_idxs[v+1] + 1;
					s32 vidx3 = remaped_idxs[v+2] + 1;

					if (v==(remaped_idxs.size()-3))
					{
						sprintf(ch, "[%d,%d,%d]\n", vidx3, vidx2, vidx1);
						scr->write(ch, (u32)strlen(ch));

						sprintf(ch, ")\n");		
						scr->write(ch, (u32)strlen(ch));
					}
					else
					{
						sprintf(ch, "[%d,%d,%d],\n", vidx3, vidx2, vidx1);
						scr->write(ch, (u32)strlen(ch));
					}
				}	

				sprintf(
					ch, 
					"BSPFaceMesh%d = mesh name:(\"BSPFaceMesh%d\") vertices:Vertices%d faces:Faces%d normals:Normals%d\n\n",
					f, f, f, f, f
					);
				scr->write(ch, (u32)strlen(ch));				

				sprintf(
					ch, 
					"BSPFaceMaterial%d = standardmaterial()\n\n",				
					f
					);
				scr->write(ch, (u32)strlen(ch));

				sprintf(
					ch, 
					"tex = bitmaptexture()\n"
					"tex.filename = \"%s\"\n"
					"tex.reload()\n\n",
					tex_fname.c_str()
					);
				scr->write(ch, (u32)strlen(ch));

				sprintf(
					ch, 
					"BSPFaceMaterial%d.diffuseMap=tex\n"
					"BSPFaceMaterial%d.diffuseMap.alphaSource = 2\n"
					"BSPFaceMaterial%d.showInViewport = true\n",
					f, f, f
					);
				scr->write(ch, (u32)strlen(ch));

				if (full_transparent)
				{
					sprintf(
						ch, 
						"BSPFaceMaterial%d.opacity=0\n",
						f
						);
					scr->write(ch, (u32)strlen(ch));
				}

				if (!full_transparent && material.getPass(0).getAlphaTest () != vid::EAT_ALWAYS )
				{
					sprintf(
						ch, 
						"BSPFaceMaterial%d.opacityMap=tex\n"
						"BSPFaceMaterial%d.opacityMap.monoOutput = 1\n\n",
						f, f
						);
					scr->write(ch, (u32)strlen(ch));
				}

				sprintf(
					ch, 
					"BSPFaceMesh%d.material = BSPFaceMaterial%d\n\n",
					f, f
					);
				scr->write(ch, (u32)strlen(ch));

				sprintf(
					ch,
					"meshop.setNumMaps BSPFaceMesh%d 3\n\n",
					f
					);
				scr->write(ch, (u32)strlen(ch));
		
				sprintf(
					ch,
					"meshop.setMapSupport  BSPFaceMesh%d 1 true\n"
					"meshop.setNumMapVerts BSPFaceMesh%d 1 %d\n"
					"meshop.setNumMapFaces BSPFaceMesh%d 1 %d\n\n",
					f, f, used_idxs.size(), f, remaped_idxs.size()/3
					);
				scr->write(ch, (u32)strlen(ch));

				sprintf(
					ch,
					"meshop.setMapSupport  BSPFaceMesh%d 2 true\n"
					"meshop.setNumMapVerts BSPFaceMesh%d 2 %d\n"
					"meshop.setNumMapFaces BSPFaceMesh%d 2 %d\n\n",
					f, f, used_idxs.size(), f, remaped_idxs.size()/3
					);
				scr->write(ch, (u32)strlen(ch));		
				
				sprintf(
					ch,
					"for i = 1 to %d do\n"
					"(\n"
					"	meshop.setMapVert BSPFaceMesh%d 1 i TextureCoords%d[i]\n"
					"	meshop.setMapVert BSPFaceMesh%d 2 i LightmapCoords%d[i]\n"
					")\n\n",
					used_idxs.size(), f, f, f, f
					);
				scr->write(ch, (u32)strlen(ch));	

				sprintf(
					ch,
					"for i = 1 to %d do\n"
					"(\n"
					"	meshop.setMapFace BSPFaceMesh%d 1 i Faces%d[i]\n"
					"	meshop.setMapFace BSPFaceMesh%d 2 i Faces%d[i]\n"
					")\n\n",
					remaped_idxs.size()/3, f, f, f, f
					);
				scr->write(ch, (u32)strlen(ch));	

				sprintf(ch, "progressUpdate (%d*100 / %d)\n\n", mesh_idx++, mesh_cnt);
				scr->write(ch, (u32)strlen(ch));
			}			
		}
	}

	SAFE_DELETE_ARRAY(remap_idxs);
	SAFE_DELETE_ARRAY(face_model);

	sprintf(ch, "progressEnd()\n\n");
	scr->write(ch, (u32)strlen(ch));

	///////////////////////////////////////////////////
	// creating lights
	///////////////////////////////////////////////////

	sprintf(ch, "progressstart \"Step 2 of 3 . . . \"\n\n");
	scr->write(ch, (u32)strlen(ch));

	sprintf(ch, "progressUpdate (0)\n\n");
	scr->write(ch, (u32)strlen(ch));	

	u32 light_count = 0, e, lcnt=0;
	s32 bsp_ambient = 0;

	for (e=0; e<CQ3EntityParser::getEntities().size(); e++)
	{
		CQ3Entity &q3ent = CQ3EntityParser::getEntities()[e];		

		if (q3ent.Type==EBSPET_LIGHT)
		{	
			light_count++;
		}
		else if (q3ent.Type==EBSPET_WORLDSPAWN)
		{
			bsp_ambient = q3ent.getKeyValueAsInt("ambient");
		}
	}	

	core::array<core::vector2df> points;
	points.push_back(core::vector2df(      0.0f, 0.0f));
	points.push_back(core::vector2df(    200.0f, 0.0f));
	points.push_back(core::vector2df(   3000.0f, 1.0f));
	points.push_back(core::vector2df(1000000.0f, 1.0f));

	core::math::pli2df light_ampl(points.pointer(), points.size());

	for (e=0; e<CQ3EntityParser::getEntities().size(); e++)
	{
		CQ3Entity &q3ent = CQ3EntityParser::getEntities()[e];		

		if (q3ent.Type==EBSPET_LIGHT)
		{	
			core::vector3df origin = q3ent.getKeyValueAsVector("origin");
			s32 radius = q3ent.getKeyValueAsFloat("light");
			img::SColor color = q3ent.getKeyValueAsColor("color");

			sprintf(
				ch, 
				"Light %d ('%s', '%s', '%s') -> origin=(%.2f %.2f %.2f), color=0x%08X, radius=%d",
				lcnt, 
				q3ent.getKeyValue("origin"), q3ent.getKeyValue("color"), q3ent.getKeyValue("light"),
				origin.X, origin.Y, origin.Z, color.color, radius
				);
			LOGGER.log(ch);

			f32 l_intens_mul= 1 + (light_intens_mul-1.0f) * light_ampl.getValue(radius);
			f32 l_radius_mul= 1 + (light_radius_mul-1.0f) * light_ampl.getValue(radius);

			sprintf(
				ch, 
				"Omnilight rgb:(color %d %d %d) shadowColor:(color 0 0 0) multiplier:%.2f contrast:0 softenDiffuseEdge:0 "
				"nearAttenStart:0 nearAttenEnd:%d farAttenStart:0 farAttenEnd:%d decayRadius:40 atmosOpacity:100 "
				"atmosColorAmt:100 shadowMultiplier:1 pos:[%f,%f,%f] castShadows:on raytracedShadows:on "
				"useFarAtten:on useNearAtten:off\n\n",
				color.getRed(), color.getGreen(), color.getBlue(), 
				l_intens_mul, 
				0,
				s32(     l_radius_mul*radius),
				origin.X, origin.Y, origin.Z
				);
			scr->write(ch, (u32)strlen(ch));	
			
			sprintf(ch, "progressUpdate (%d*100 / %d)\n\n", lcnt, light_count);
			scr->write(ch, (u32)strlen(ch));

			lcnt++;
		}		
	}		

	sprintf(
		ch,
		"TargetDirectionallight rgb:(color %d %d %d) shadowColor:(color 0 0 0) multiplier:1 contrast:0 "
		"softenDiffuseEdge:0 nearAttenStart:0 nearAttenEnd:40 farAttenStart:80 farAttenEnd:200 decayRadius:40 "
		"atmosOpacity:100 atmosColorAmt:100 shadowMultiplier:1 hotspot:100000 falloff:100000 aspect:1 "
		"pos:[0,0,100000] isSelected:on target:(Targetobject transform:(matrix3 [1,0,0] [0,1,0] [0,0,1] [0,0,-100000])) "
		"ambientonly:true\n\n",
		bsp_ambient, bsp_ambient, bsp_ambient
		);
	scr->write(ch, (u32)strlen(ch));	
	
	CQ3EntityParser::clearEntities();

	sprintf(ch, "progressEnd()\n\n");
	scr->write(ch, (u32)strlen(ch));


	sprintf(ch, "progressstart \"Step 3 of 3 . . . \"\n\n");
	scr->write(ch, (u32)strlen(ch));

	sprintf(ch,	"\nclearSelection()\n\n");
	scr->write(ch, (u32)strlen(ch));		

	sprintf(ch, "progressUpdate (0)\n\n");
	scr->write(ch, (u32)strlen(ch));	

	///////////////////////////////////////////////////
	// attaching faces together using its groups
	///////////////////////////////////////////////////	

	core::array<s32>lightmap_meshes;
	s32 nolightmap_mesh=-1;

	for (s32 l=0, mesh_idx=0; l<NumLightMaps+1; l++)
	{
		// attaching meshes to root faces		

		sprintf(ch, "progressUpdate (%d*100 / %d)\n\n", mesh_idx, mesh_cnt);
		scr->write(ch, (u32)strlen(ch));

		if (used_lmap_tex_faces_idxs[l].size()>0)
		{
			mesh_idx++;
		}

		for(u32 t=1; 
			t<used_lmap_tex_faces_idxs[l].size()&&
			used_lmap_tex_faces_idxs[l][t].size()>0; 
			t++
			)
		{
			sprintf(
				ch, 				
				"meshop.attach BSPFaceMesh%d BSPFaceMesh%d attachMat:#MatToID condenseMat:false\n",			
				used_lmap_tex_faces_idxs[l][0][0], used_lmap_tex_faces_idxs[l][t][0]
				);
			scr->write(ch, (u32)strlen(ch));				

			sprintf(ch, "progressUpdate (%d*100 / %d)\n\n", mesh_idx, mesh_cnt);
			scr->write(ch, (u32)strlen(ch));

			mesh_idx++;
		}

		if (additional_lmap_meshes_idxs[l].size()>0)
		{
			mesh_idx++;
		}

		if (used_lmap_tex_faces_idxs[l].size()>0 &&	used_lmap_tex_faces_idxs[l][0].size()>0)
		{
			for( u32 am=0;am<additional_lmap_meshes_idxs[l].size(); am++)
			{		
				sprintf(
					ch, 				
					" meshop.attach BSPFaceMesh%d BSPFaceMesh%d attachMat:#MatToID condenseMat:false\n",			
					used_lmap_tex_faces_idxs[l][0][0], additional_lmap_meshes_idxs[l][am]
					);
				scr->write(ch, (u32)strlen(ch));				

				sprintf(ch, " progressUpdate (%d*100 / %d)\n\n", mesh_idx, mesh_cnt);
				scr->write(ch, (u32)strlen(ch));

				mesh_idx++;
			}	
		}
		else
		{
			for( u32 am=1; am<additional_lmap_meshes_idxs[l].size(); am++)
			{		
				sprintf(
					ch, 				
					" meshop.attach BSPFaceMesh%d BSPFaceMesh%d attachMat:#MatToID condenseMat:false\n",			
					additional_lmap_meshes_idxs[l][0], additional_lmap_meshes_idxs[l][am]
					);
				scr->write(ch, (u32)strlen(ch));				

				sprintf(ch, " progressUpdate (%d*100 / %d)\n\n", mesh_idx, mesh_cnt);
				scr->write(ch, (u32)strlen(ch));

				mesh_idx++;
			}
		}

		//lightmap_meshes

		if (l==NumLightMaps)
		{
			if (used_lmap_tex_faces_idxs[l].size()>0 && used_lmap_tex_faces_idxs[l][0].size()>0)
			{
				nolightmap_mesh = used_lmap_tex_faces_idxs[l][0][0];
			}
			else
			if (additional_lmap_meshes_idxs[l].size()>0)		
			{
				nolightmap_mesh = additional_lmap_meshes_idxs[l][0];
			}
		}
		else
		{
			if (used_lmap_tex_faces_idxs[l].size()>0 && used_lmap_tex_faces_idxs[l][0].size()>0)
			{
				lightmap_meshes.push_back(used_lmap_tex_faces_idxs[l][0][0]);
			}
			else
			if (additional_lmap_meshes_idxs[l].size()>0)		
			{
				lightmap_meshes.push_back(additional_lmap_meshes_idxs[l][0]);
			}
			else
			{
				lightmap_meshes.push_back(-1);		
			}
		}
	}

	for (u32 lm=0; lm<lightmap_meshes.size(); lm++)
	{
		if (lightmap_meshes[lm]!=-1)
		{
			sprintf(ch,	"BSPFaceMesh%d.name = \"%04d.LightMapMesh\"\n", lightmap_meshes[lm], lm);
			scr->write(ch, (u32)strlen(ch));	
		}
		else
		{
			sprintf(
				ch, 
				"BSPDummieMesh = mesh name:(\"%04d.LightMapMesh\") vertices:#([0,0,0]) faces:#([1,1,1])\n\n",
				lm
				);
			scr->write(ch, (u32)strlen(ch));	

			sprintf(
				ch,
				"meshop.setNumMaps BSPDummieMesh 3\n"				
				);
				scr->write(ch, (u32)strlen(ch));
	
			sprintf(
				ch,
				"meshop.setMapSupport  BSPDummieMesh 1 true\n"
				"meshop.setNumMapVerts BSPDummieMesh 1 1\n"
				"meshop.setNumMapFaces BSPDummieMesh 1 1\n"
				);
			scr->write(ch, (u32)strlen(ch));

			sprintf(
				ch,
				"meshop.setMapSupport  BSPDummieMesh 2 true\n"
				"meshop.setNumMapVerts BSPDummieMesh 2 1\n"
				"meshop.setNumMapFaces BSPDummieMesh 2 1\n\n"
				);
			scr->write(ch, (u32)strlen(ch));				
		}
	}

	if (nolightmap_mesh!=-1)
	{
		sprintf(ch,	"BSPFaceMesh%d.name = \"MeshWithoutLightMap\"\n", nolightmap_mesh);
		scr->write(ch, (u32)strlen(ch));	
	}
	
	sprintf(ch,	"\nselect $*LightMapMesh*\n\n");
	scr->write(ch, (u32)strlen(ch));	

	sprintf(ch, "\nprogressEnd()\n\n");
	scr->write(ch, (u32)strlen(ch));	

	sprintf(ch,	"macros.run \"Render\" \"BakeDialog\"\n\n");
	scr->write(ch, (u32)strlen(ch));

	SAFE_DELETE_ARRAY(lmap_face);	
	
	SAFE_DROP(scr);
	SAFE_DROP(map);
}

//---------------------------------------------------------------------------

struct SBadTexel
{
	SBadTexel() {}
	SBadTexel(img::SColor &c, img::SColor &lgc, f32 u, f32 v) 
		: U(u), V(v), Color(c), LightGridColor(lgc) {}

	img::SColor Color, LightGridColor;
	f32 U, V;
};
//---------------------------------------------------------------------------

void CQ3LevelMesh::rewriteLightMapsFrom(const c8* lightmaps_path)
{
	LOGGER.log("Updating lightmaps in BSP file . . .");

	core::stringc initial_work_dir =
		FILE_SYSTEM.getWorkingDirectory();

	if (!FILE_SYSTEM.changeWorkingDirectoryTo(lightmaps_path))
	{
		LOGGER.log("Can't find lightmap directory :", lightmaps_path, io::ELL_ERROR);

		return;
	}

	const io::IFileList *files = FILE_SYSTEM.getFileList();	

	c8 ch[255];

	s32 bsp_ambient = 10;

	if (Q3Entities[EBSPET_WORLDSPAWN].size()>0)
	{
		bsp_ambient = Q3Entities[EBSPET_WORLDSPAWN][0].getKeyValueAsInt("ambient");
	}	

	if (files)
	{
		// loading new lightmaps

		core::array<img::IImage*> lightmaps;

		for (s32 f=0; f<files->getFileCount(); f++)
		{
			if (files->isDirectory(f))
				continue;

			core::stringc fname = files->getFullFileName(f);			

			img::IImage* img = IMAGE_LIBRARY.createImageFromFile(fname.c_str());
				
			if (img)
			{
				lightmaps.push_back(img);
			}	
			else
			{
				LOGGER.log("Can't load image :", fname.c_str(), io::ELL_ERROR);
			}
		}

		if (lightmaps.size() == NumLightMaps)
		{
			// checking bat texels in lightmaps

			core::array<SBadTexel> *bad_lightmaps_texels = new core::array<SBadTexel>[NumLightMaps];

			for (s32 m=0; m<NumModels; m++)
			{	
				s32 face_start_idx = Models[m].faceIndex;
				s32 face_end_idx   = Models[m].faceIndex + Models[m].numOfFaces;		

				for (s32 i=face_start_idx; i<face_end_idx; ++i)
				{	
					for (s32 tf=0; tf<Faces[i].numMeshVerts; tf+=3)
					{	
						s32 lidx = Faces[i].lightmapID;

						if (lidx==-1)
							continue;

						tBSPLightmap* lm = &LightMaps[lidx];

						s32 vidx1 = Faces[i].vertexIndex + MeshVerts[Faces[i].meshVertIndex + tf    ];
						s32 vidx2 = Faces[i].vertexIndex + MeshVerts[Faces[i].meshVertIndex + tf + 1];
						s32 vidx3 = Faces[i].vertexIndex + MeshVerts[Faces[i].meshVertIndex + tf + 2];

						tBSPVertex &v1 = Vertices[vidx1];
						tBSPVertex &v2 = Vertices[vidx2];
						tBSPVertex &v3 = Vertices[vidx3];

						//if (v1.vLightmapCoord[0]==v2.vLightmapCoord[0] && v1.vLightmapCoord[0]==v3.vLightmapCoord[0] &&
						//	v1.vLightmapCoord[1]==v2.vLightmapCoord[1] && v1.vLightmapCoord[1]==v3.vLightmapCoord[1]
						//	)
						if ((v1.vLightmapCoord[0]==v2.vLightmapCoord[0] && v1.vLightmapCoord[1]==v2.vLightmapCoord[1]) || 
							(v1.vLightmapCoord[0]==v3.vLightmapCoord[0] && v1.vLightmapCoord[1]==v3.vLightmapCoord[1]) ||
							(v2.vLightmapCoord[0]==v3.vLightmapCoord[0] && v2.vLightmapCoord[1]==v3.vLightmapCoord[1])
							)
						// itsa bad texel, so lets remember it
						{
							f32 u = v1.vLightmapCoord[0];
							f32 v = v1.vLightmapCoord[1];

							s32 x = v*128.0f;
							s32 y = u*128.0f;

							core::vector3df vpos1(v1.vPosition[0], v1.vPosition[2], v1.vPosition[1]);
							core::vector3df vpos2(v1.vPosition[0], v1.vPosition[2], v1.vPosition[1]);
							core::vector3df vpos3(v1.vPosition[0], v1.vPosition[2], v1.vPosition[1]);

							vid::SLight light1 = getLightFromPos(vpos1);
							vid::SLight light2 = getLightFromPos(vpos2);
							vid::SLight light3 = getLightFromPos(vpos3);

							img::SColor c(
								255,
								lm->imageBits[x][y][0],
								lm->imageBits[x][y][1],
								lm->imageBits[x][y][2]
								);  

							img::SColor lgc1 = light1.DiffuseColor.toSColor();
							img::SColor lgc2 = light2.DiffuseColor.toSColor();
							img::SColor lgc3 = light3.DiffuseColor.toSColor();

							f32 r = (lgc1.getRed()  +lgc2.getRed()  +lgc3.getRed()  )/3.0f;
							f32 g = (lgc1.getGreen()+lgc2.getGreen()+lgc3.getGreen())/3.0f;
							f32 b = (lgc1.getBlue() +lgc2.getBlue() +lgc3.getBlue() )/3.0f;
							f32 a = (lgc1.getAlpha()+lgc2.getAlpha()+lgc3.getAlpha())/3.0f;

							img::SColor lgc(a,r,g,b);  

							bad_lightmaps_texels[lidx].push_back( SBadTexel(c, lgc, u, v) );
						}
					}
				}
			}			
		
			for (s32 l=0; l<NumLightMaps; l++)
			{
				tBSPLightmap* lm = &LightMaps[l];

				for (s32 x=0; x<128; ++x)
				{
					for (s32 y=0; y<128; ++y)
					{
						img::SColor c = lightmaps[l]->getPixel(x,y);

						lm->imageBits[x][y][0] = (u8)c.getRed();
						lm->imageBits[x][y][1] = (u8)c.getGreen();
						lm->imageBits[x][y][2] = (u8)c.getBlue();												
					}				
				}  				

				// restoring bad texels in lightmap

				for (u32 t=0; t<bad_lightmaps_texels[l].size(); t++)
				{
					f32 &u = bad_lightmaps_texels[l][t].U;
					f32 &v = bad_lightmaps_texels[l][t].V;
					img::SColor &c   = bad_lightmaps_texels[l][t].Color;
					img::SColor &lgc = bad_lightmaps_texels[l][t].LightGridColor;

					s32 x = v*128.0f;
					s32 y = u*128.0f;

					s32 r=0.75f*lgc.getRed()  +0.25f*c.getRed(); 
					s32 g=0.75f*lgc.getGreen()+0.25f*c.getGreen();
					s32 b=0.75f*lgc.getBlue() +0.25f*c.getBlue();		

					CHECK_RANGE(r, 0, 255);
					CHECK_RANGE(g, 0, 255);
					CHECK_RANGE(b, 0, 255);

					lm->imageBits[x][y][0] = (u8)r;
					lm->imageBits[x][y][1] = (u8)g;
					lm->imageBits[x][y][2] = (u8)b;
				}

				sprintf( ch, " LightMap.%d updated", l );
				LOGGER.log(ch);
			}
			
			SAFE_DELETE_ARRAY(bad_lightmaps_texels);
		}
		else
		{
			sprintf(
				ch, 
				"Can't find needed lightmaps number (%d finded, %d must be), aborting!", 
				lightmaps.size(), NumLightMaps
				);
			LOGGER.log(ch);
		}

		// deleting lightmaps

		for (u32 i=0; i<lightmaps.size(); i++)
		{
			SAFE_DROP(lightmaps[i]);
		}
	}	

	FILE_SYSTEM.changeWorkingDirectoryTo(initial_work_dir.c_str());
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------





