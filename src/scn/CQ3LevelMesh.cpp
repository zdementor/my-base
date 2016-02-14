//|-------------------------------------------------------------------------
//| File:        CQ3LevelMesh.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CQ3LevelMesh.h"

#include <scn/ISceneManager.h>
#include <scn/mesh_buffer.h>   
#include <scn/ISceneNode.h>
#include <scn/SAnimatedMesh.h>
#include <scn/SMesh.h>
#include <io/IWriteFile.h>
#include <img/IImageLibrary.h>
#include <io/IFileList.h>
#include <dev/IDevice.h>
#include <scn/IMaterialsManager.h>
#include <os/ITimer.h>
#include <res/IResourceManager.h>

#include "CQ3Entity.h"
#include "CQ3EntityParser.h"
#include "CQ3MapFileParser.h"

#include "CQ3LevelMeshHelper.hpp"
#include "CQ3LevelMeshBspTools.hpp"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CQ3LevelMesh::CQ3LevelMesh() 
	: 
Mesh(0), AMeshes(0), LevelPath(""), LevelName(""), LightGridOK(false)
{
#if MY_DEBUG_MODE 
	IAnimatedMesh::setClassName("CQ3LevelMesh IAnimatedMesh");
#endif

	EntitiesSize = 0;
	NumTextures = 0;
	NumPlanes = 0;
	NumNodes = 0;
	NumLeafs = 0;
	LeafFacesSize = 0;
	LeafBrushesSize = 0;
	NumModels = 0;
	NumBrushes = 0;
	NumBrushSides = 0;
	NumVertices = 0;
	NumMeshVerts = 0;
	NumShaders = 0;
	NumFaces = 0;
	NumLightMaps = 0;
	NumLightVols = 0;
	VisData.numOfClusters = 0;
	VisData.bytesPerCluster = 0;

	StrEntities = NULL;
	Textures = NULL;
	Planes = NULL;
	Nodes = NULL;
	Leafs = NULL;
	LeafFaces = NULL;
	LeafBrushes = NULL;
	Models = NULL;
	Brushes = NULL;
	BrushSides = NULL;
	Vertices = NULL;
	MeshVerts = NULL;
	Shaders = NULL;
	Faces = NULL;
	LightMaps = NULL;
	LightVols = NULL;
	VisData.pBitsets = NULL;
}

//---------------------------------------------------------------------------
//! destructor
CQ3LevelMesh::~CQ3LevelMesh()
{
	freeData();	
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::loadData(io::IReadFile* file)
{	
	if (!file)
		return;

	core::stringc msg;
	msg.sprintf("Loading BSP mesh from %s . . .", file->getFileName());
	LOGGER.log(msg);

	LOGGER.increaseFormatLevel();

	const eLumps lumps_to_load[]=
	{
		kEntities,
        kTextures,
        kModels, 
        kVertices, 
        kMeshVerts,
        kFaces,
        kLightmaps,
        kLightVolumes
	};

	const s32 lumps_cnt = sizeof(lumps_to_load)/sizeof(eLumps);  

	for (s32 i=0; i<lumps_cnt; i++)
	{
		eLumps ltype = lumps_to_load[i];

		tBSPLump &l = Lumps[ltype];		

		if (l.length>0)
		{
			file->seek(l.offset);
    
			if (ltype==kEntities)
			{
				EntitiesSize = l.length / sizeof(c8);
				StrEntities = new c8[EntitiesSize];

				file->read(StrEntities, l.length);
			}
			else if (ltype==kTextures)
			{
				NumTextures = l.length / sizeof(tBSPTexture);
				Textures = new tBSPTexture[NumTextures];

				file->read(Textures, l.length);
			}
			else if (ltype==kPlanes)
			{
				NumPlanes = l.length / sizeof(tBSPPlane);
				Planes = new tBSPPlane[NumPlanes];

				file->read(Planes, l.length);
			}
			else if (ltype==kNodes)
			{
				NumNodes = l.length / sizeof(tBSPNode);
				Nodes = new tBSPNode[NumNodes];

				file->read(Nodes, l.length);
			}
			else if (ltype==kLeafs)
			{
				NumLeafs = l.length / sizeof(tBSPLeaf);
				Leafs = new tBSPLeaf[NumLeafs];

				file->read(Leafs, l.length);
			}
			else if (ltype==kLeafFaces)
			{
				LeafFacesSize = l.length / sizeof(s32);
				LeafFaces = new s32[LeafFacesSize];

				file->read(LeafFaces, l.length);
			}
			else if (ltype==kLeafBrushes)
			{
				LeafBrushesSize = l.length / sizeof(s32);
				LeafBrushes = new s32[LeafBrushesSize];

				file->read(LeafBrushes, l.length);
			}
			else if (ltype==kModels)
			{
				NumModels = l.length / sizeof(tBSPModel);
				Models = new tBSPModel[NumModels];

				file->read(Models, l.length);
			}
			else if (ltype==kBrushes)
			{
				NumBrushes = l.length / sizeof(tBSPBrush);
				Brushes = new tBSPBrush[NumBrushes];

				file->read(Brushes, l.length);
			}
			else if (ltype==kBrushSides)
			{
				NumBrushSides = l.length / sizeof(tBSPBrushSide);
				BrushSides = new tBSPBrushSide[NumBrushSides];

				file->read(BrushSides, l.length);
			}
			else if (ltype==kVertices)
			{
				NumVertices = l.length / sizeof(tBSPVertex);
				Vertices = new tBSPVertex[NumVertices];

				file->read(Vertices, l.length);
			}
			else if (ltype==kMeshVerts)
			{
				NumMeshVerts = l.length / sizeof(s32);
				MeshVerts = new s32[NumMeshVerts];

				file->read(MeshVerts, l.length);
			}
			else if (ltype==kShaders)
			{
				NumShaders = l.length / sizeof(tBSPShader);
				Shaders = new tBSPShader[NumShaders];
				
				file->read(Shaders, l.length); 
			}
			else if (ltype==kFaces)
			{
				NumFaces = l.length / sizeof(tBSPFace);
				Faces = new tBSPFace[NumFaces];

				file->read(Faces, l.length);
			}
			else if (ltype==kLightmaps)
			{
				NumLightMaps = l.length / sizeof(tBSPLightmap);
				LightMaps = new tBSPLightmap[NumLightMaps];

				file->read(LightMaps, l.length);
			}
			else if (ltype==kLightVolumes)
			{
				NumLightVols = l.length / sizeof(SLightVolume);
				LightVols = new SLightVolume[NumLightVols];

				file->read(LightVols, l.length);
			}
			else if (ltype==kVisData)
			{
				VisData.pBitsets = new c8[(l.length-2*sizeof(s32))/sizeof(c8)];

				file->seek(l.offset);
				file->read(&VisData, 2*sizeof(s32));
				file->read(VisData.pBitsets, l.length-2*sizeof(s32));
			}		
		}

		printBspInfo(ltype);
	}	
	
	LOGGER.decreaseFormatLevel();
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::freeData()
{
	freeUnusedData();

	SAFE_DROP(Mesh);

	for (s32 m=1; m<NumModels; m++)
	{
		SAnimatedMesh *am = AMeshes[m-1];
		SAFE_DROP(am);
	}

	SAFE_DELETE_ARRAY(AMeshes);

	SAFE_DELETE_ARRAY(LightVols);   	
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::printBspInfo(eLumps ltype)
{
	tBSPLump &l = Lumps[ltype];

	c8 ch[255];		

	if (ltype==kEntities)
	{
		sprintf(ch, "Entities:    offset=0x%X length=0x%X number=%d", l.offset, l.length, EntitiesSize);
	}
	else if (ltype==kTextures)
	{
		sprintf(ch, "Textures:    offset=0x%X length=0x%X number=%d", l.offset, l.length, NumTextures);
	}
	else if (ltype==kPlanes)
	{
		sprintf(ch, "Planes:      offset=0x%X length=0x%X number=%d", l.offset, l.length, NumPlanes);
	}
	else if (ltype==kNodes)
	{
		sprintf(ch, "Nodes:       offset=0x%X length=0x%X number=%d", l.offset, l.length, NumNodes);
	}
	else if (ltype==kLeafs)
	{
		sprintf(ch, "Leafs:       offset=0x%X length=0x%X number=%d", l.offset, l.length, NumLeafs);
	}
	else if (ltype==kLeafFaces)
	{
		sprintf(ch, "LeafFaces:   offset=0x%X length=0x%X number=%d", l.offset, l.length, LeafFacesSize);
	}
	else if (ltype==kLeafBrushes)
	{
		sprintf(ch, "LeafBrushes: offset=0x%X length=0x%X number=%d", l.offset, l.length, LeafBrushesSize);
	}
	else if (ltype==kModels)
	{
		sprintf(ch, "Models:      offset=0x%X length=0x%X number=%d", l.offset, l.length, NumModels);
	}
	else if (ltype==kBrushes)
	{
		sprintf(ch, "Brushes:     offset=0x%X length=0x%X number=%d", l.offset, l.length, NumBrushes);
	}
	else if (ltype==kBrushSides)
	{
		sprintf(ch, "BrushSides:  offset=0x%X length=0x%X number=%d", l.offset, l.length, NumBrushSides);
	}
	else if (ltype==kVertices)
	{
		sprintf(ch, "Vertices:    offset=0x%X length=0x%X number=%d", l.offset, l.length, NumVertices);
	}
	else if (ltype==kMeshVerts)
	{
		sprintf(ch, "MeshVerts:   offset=0x%X length=0x%X number=%d", l.offset, l.length, NumMeshVerts);
	}
	else if (ltype==kShaders)
	{
		sprintf(ch, "Shaders:     offset=0x%X length=0x%X number=%d", l.offset, l.length, NumShaders);
	}
	else if (ltype==kFaces)
	{
		sprintf(ch, "Faces:       offset=0x%X length=0x%X number=%d", l.offset, l.length, NumFaces);
	}
	else if (ltype==kLightmaps)
	{
		sprintf(ch, "LightMaps:   offset=0x%X length=0x%X number=%d", l.offset, l.length, NumLightMaps);
	}
	else if (ltype==kLightVolumes)
	{
		sprintf(ch, "LightVols:   offset=0x%X length=0x%X number=%d", l.offset, l.length, NumLightVols);
	}
	else if (ltype==kVisData)
	{
		sprintf(ch, "VisData:     offset=0x%X length=0x%X number=%d", l.offset, l.length, VisData.numOfClusters*VisData.bytesPerCluster);
	}

	LOGGER.log(ch);	
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::parseEntities()
{
	if (EntitiesSize<=0)
		return;	
	
	c8 *tmp_entity_str = new c8[EntitiesSize];
	memcpy(tmp_entity_str, StrEntities, EntitiesSize*sizeof(c8));

	CQ3EntityParser::parseEntities(tmp_entity_str);

	SAFE_DELETE_ARRAY(tmp_entity_str);

	c8 ch[255];
	u32 e=0;

	ModelEntities.set_used(NumModels);

	for (e=0; e<CQ3EntityParser::getEntities().size(); e++)
	{
		CQ3Entity &q3ent = CQ3EntityParser::getEntities()[e];	

		E_BSP_ENTITY_TYPE etype = q3ent.Type;
		
		Q3Entities[q3ent.Type].push_back( CQ3Entity() );
		CQ3Entity &q3ent_new = 
			Q3Entities[ etype ][ Q3Entities[etype].size()-1 ];
		q3ent_new.set(q3ent);

		s32 modelidx = q3ent.getKeyValueAsInt("model");

		if ((modelidx>=0  && modelidx<NumModels) && 
			!(modelidx>0  && q3ent.Type==EBSPET_WORLDSPAWN) && 
			!(modelidx==0 && q3ent.Type!=EBSPET_WORLDSPAWN)
			)
		{
			ModelEntities[modelidx] = SModelEntityEntry(etype, Q3Entities[etype].size()-1);	
			
			if (q3ent_new.Type==EBSPET_FUNC_DOOR || 
				q3ent_new.Type==EBSPET_FUNC_BUTTON || 
				q3ent_new.Type==EBSPET_FUNC_TRANSPARENT
				)
			{
				if (q3ent_new.Type==EBSPET_FUNC_DOOR)
					sprintf(ch, " Model %d as door %d", modelidx, Q3Entities[q3ent_new.Type].size()-1);
				else
				if (q3ent_new.Type==EBSPET_FUNC_BUTTON)
					sprintf(ch, " Model %d as button %d", modelidx, Q3Entities[q3ent_new.Type].size()-1);
				else
				if (q3ent_new.Type==EBSPET_FUNC_TRANSPARENT)
					sprintf(ch, " Model %d as transp decor %d", modelidx, Q3Entities[q3ent_new.Type].size()-1);
					
				LOGGER.log(ch);
			}	
		}
	}	
	
	CQ3EntityParser::clearEntities();
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::initLightGrid()
{
	LightGridOK = false;

	if (NumModels<=0)
		return;

	f32 lightGridSize[3] = { 64, 64, 128 }; 

	s32 world_count = Q3Entities[EBSPET_WORLDSPAWN].size();

	if (world_count>0)
	{
		core::vector3df light_grid_size = 
			Q3Entities[EBSPET_WORLDSPAWN][0].getKeyValueAsVector("gridsize");

		lightGridSize[0] = light_grid_size.X;
		lightGridSize[1] = light_grid_size.Y;
		lightGridSize[2] = light_grid_size.Z;
	}
	
	s32 lightGridBounds[3];
	f32 *wMins = Models[0].min, *wMaxs = Models[0].max, maxs[3];
	s32 i;

	f32 lightGridOrigin[3];

	for ( i = 0 ; i < 3 ; i++ ) 
	{
        lightGridOrigin[i] = lightGridSize[i] * ceil( wMins[i] / lightGridSize[i] );
        maxs[i] = lightGridSize[i] * floor( wMaxs[i] /lightGridSize[i] );
        lightGridBounds[i] = (maxs[i] - lightGridOrigin[i])/lightGridSize[i] + 1;
    }

    s32 numGridPoints = lightGridBounds[0] * lightGridBounds[1] * lightGridBounds[2];
	
	LightGridData.lightGridData = LightVols;

	LightGridData.lightGridBounds[0] = lightGridBounds[0];
	LightGridData.lightGridBounds[1] = lightGridBounds[1]; 
	LightGridData.lightGridBounds[2] = lightGridBounds[2];

	LightGridData.lightGridOrigin[0] = lightGridOrigin[0];
	LightGridData.lightGridOrigin[1] = lightGridOrigin[1]; 
	LightGridData.lightGridOrigin[2] = lightGridOrigin[2];

	c8 ch[255];
	sprintf(
		ch, "Loaded %d light volumes (%d x %d x %d = %d), gridsize (%.f %.f %.f) ", 
		NumLightVols, 
		LightGridData.lightGridBounds[0], LightGridData.lightGridBounds[2], 
		LightGridData.lightGridBounds[1],
		numGridPoints,
		lightGridSize[0], lightGridSize[1], lightGridSize[2]
		);
	LOGGER.log(ch);	

	if (NumLightVols!=numGridPoints)
	{
		LightGridData.lightGridBounds[0] = 0;
		LightGridData.lightGridBounds[1] = 0; 
		LightGridData.lightGridBounds[2] = 0;
	}
	else
	{
		LightGridOK = true;
	}

	if (!LightGridOK)
	{
		LOGGER.log(" Can't initialize light grid properly!");
	}
	else
	{
		LOGGER.log(" Light grid initialiazed successfully");		
	}
}

//---------------------------------------------------------------------------

bool isNeedToAddToMesh0(E_BSP_ENTITY_TYPE etype)
{
	return (
		etype != EBSPET_FUNC_DOOR &&
		etype != EBSPET_FUNC_BUTTON &&
		etype != EBSPET_FUNC_TRANSPARENT
		);
}

//---------------------------------------------------------------------------

//! constructs a mesh from the quake 3 level file.
void CQ3LevelMesh::constructMesh()
{
	if (NumModels<=0)
		return;

	core::stringc msg;
	c8 ch[255]; 

	LOGGER.log("Loading bsp-map textures . . .", io::ELL_INFORMATION);

	u32 start = TIMER.getSystemTime();

	// load lightmaps.	   
	    
	const s32 lmap_size_arr[]={128,256,512,1024,2048,4096};
	const s32 lmap_orig_size=128;
	const s32 lmap_mix_cnt = sizeof(lmap_size_arr)/sizeof(s32);
	s32 lmap_in_one_arr[lmap_mix_cnt];
	s32 lmap_pitch_arr[lmap_mix_cnt];
	for (s32 i=0; i<lmap_mix_cnt; i++)
	{
		lmap_pitch_arr[i] = lmap_size_arr[i]/lmap_orig_size;
		lmap_in_one_arr[i] = lmap_pitch_arr[i]*lmap_pitch_arr[i];
	}

	vid::IVideoDriver &driver = VIDEO_DRIVER;
	dev::IDevice      &device = DEVICE;

	s32 lmap_size_idx = lmap_mix_cnt-1;

    bool oldMipMapState = driver.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

	driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, false);

	s32 lmap_actual_count=0;

	core::array<vid::ITexture*> lig;
	core::array<img::IImage*> lig_img;
	core::array<core::dimension2df> lig_coord_offset;
	core::array<f32> lig_coord_scale;
	
	core::array<s32> lmap_remap;
	lmap_remap.set_used(NumLightMaps);

	core::dimension2di max_tex_size = driver.getMaximalTextureSize();

	// loading and compressing lightmaps
    for (s32 l=0; l<NumLightMaps; ++l)
    {	
		s32 lmap_size  =lmap_size_arr[lmap_size_idx];
		s32 lmap_in_one=lmap_in_one_arr[lmap_size_idx];
		s32 lmap_pitch =lmap_pitch_arr[lmap_size_idx];

		s32 xoffset = (l%lmap_in_one) % lmap_pitch;
		s32 yoffset = (l%lmap_in_one) / lmap_pitch;

		if (l%lmap_in_one==0)
		{
			s32 lmaps_left = NumLightMaps-l;

			// try to downsample lightmap size
			for (s32 i = lmap_size_idx; i >= 0; i--)
			{				
				if (max_tex_size.Width>=lmap_size_arr[i]&&
					max_tex_size.Height>=lmap_size_arr[i]&&
					lmap_in_one_arr[i]>=0.5f*lmaps_left)
					lmap_size_idx=i;										
				else
					break;
			}

			lmap_size  =lmap_size_arr[lmap_size_idx];
			lmap_in_one=lmap_in_one_arr[lmap_size_idx];
			lmap_pitch =lmap_pitch_arr[lmap_size_idx];
			
			msg.sprintf("Creating lightmap image %d (%d x %d)",
				lmap_actual_count, lmap_size, lmap_size);

			LOGGER.log(msg.c_str());			
		
			lig_img.push_back(
				IMAGE_LIBRARY.createEmptyImage(
					core::dimension2d<s32>(lmap_size, lmap_size), 
					img::ECF_R5G6B5));
			lig_img[lig_img.size()-1]->fill(img::SColor(255,255,255,255));

			lmap_actual_count++;						
		}

		s32 lmap_actual_idx = lmap_actual_count - 1;

		lmap_remap[l] = lmap_actual_idx;	
		
		f32 lmap_coord_scl = 1.0f/(f32)lmap_pitch;

		lig_coord_offset.push_back(
			core::dimension2df(xoffset*lmap_coord_scl, (yoffset)*lmap_coord_scl)
			);
		lig_coord_scale.push_back(lmap_coord_scl);

		void* pp = lig_img[lmap_actual_idx]->getData();				

        if (pp)
        {
			s16* p = (s16*)pp;

			tBSPLightmap* lm;
			lm = &LightMaps[l];

			s32 yoffset_lmap_orig_size = yoffset*lmap_orig_size;
			s32 xoffset_lmap_orig_size = xoffset*lmap_orig_size;

			for (s32 y=0; y<lmap_orig_size; ++y)
			{
				for (s32 x=0; x<lmap_orig_size; ++x)
				{
					f32 r = lm->imageBits[y][x][0];
					f32 g = lm->imageBits[y][x][1];
					f32 b = lm->imageBits[y][x][2];

					p[(yoffset_lmap_orig_size+y)*lmap_size + (xoffset_lmap_orig_size + x)] = 
						img::SColor(255, r, g, b).toR5G6B5();
				}
            } 
			
			msg.sprintf(" Copied lightmap data %d (%d, %d) (%f, %f)", l, xoffset, yoffset, lig_coord_offset[l].Width, lig_coord_offset[l].Height);

			LOGGER.log(msg.c_str());			
        }
		else
		{
			LOGGER.log("Could not lock lightmap!", io::ELL_ERROR);
		}			
    }

	for (s32 l=0; l<(s32)lig_img.size(); ++l)
    {		
		lig.push_back( driver.createTexture(lig_img[l]) );			
		lig[lig.size()-1]->freeImageData();

		SAFE_DROP(lig_img[l]);
	}
	lig_img.clear();

    driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, oldMipMapState);

	// load textures

	bool use_tbn = device.getDeviceFlagValue(dev::EDCF_SHADERS_HIGH_QUALITY);

    core::array<vid::ITexture*> tex, bump;
    tex.set_used(NumTextures);
	bump.set_used(NumTextures);

    for (s32 t=0; t<NumTextures; ++t)
    {
		tex[t] = 0;
		bump[t] = 0;

		if (isRadiantHintTexture(Textures[t].strName))
			continue;

		tex[t] = CQ3LevelMesh::getQ3Texture(Textures[t].strName);
		if (use_tbn)
			bump[t] = getNormalMapTexture(Textures[t].strName);
	}

	u32 end = TIMER.getSystemTime();

	LOGGER.logInfo(" Done in %dms.", end-start);

	/////////////////////////////////////////////////////////////////////////////

	LOGGER.logInfo("Fills geometry and attaching materials . . .");

	start = TIMER.getSystemTime();

	core::array< core::array<SModelMeshBuffer> > models_buffers;
	models_buffers.set_used(NumModels);	

	s32 mb_cnt = (NumTextures+1) * (NumLightMaps+1);

	for (s32 m = 0; m < NumModels; m++)
		models_buffers[m].set_used( mb_cnt );

	// allocation memory for all models meshes

	// main mesh
	Mesh = new SMesh;

	// other models meshes
	if (NumModels>1)
	{
		AMeshes = new SAnimatedMesh* [NumModels-1];

		for (s32 m=0; m<(NumModels-1); m++)
		{
			AMeshes[m] = new SAnimatedMesh;
			SMesh* mesh = new SMesh;
			AMeshes[m]->addMesh( mesh );	
			mesh->drop();
		}
	}

	bool *tex_warns = new bool[NumTextures];	
	memset(tex_warns, 0, NumTextures*sizeof(bool));

	// attaching materials

	for (s32 m=0; m<NumModels; m++)
	{
		// go through all faces and add them to the buffer.

		s32 face_start_idx = Models[m].faceIndex;
		s32 face_end_idx   = Models[m].faceIndex + Models[m].numOfFaces;

		for (s32 i=face_start_idx; i<face_end_idx; ++i)
		{	
			if (Faces[i].lightmapID < -1)
				Faces[i].lightmapID = -1;
			else
			if (Faces[i].lightmapID > NumLightMaps-1)
				Faces[i].lightmapID = -1;

			if (Faces[i].textureID < -1)
				Faces[i].textureID = -1;
			else
			if (Faces[i].textureID > NumTextures-1)
				Faces[i].textureID = -1;

			if (Faces[i].textureID != -1 &&
				isRadiantHintTexture(Textures[Faces[i].textureID].strName)
				)
			{
				continue;				
			}

			vid::SMaterial     mat   = vid::SMaterial();
			vid::E_VERTEX_TYPE vtype = vid::EVT_2TCOORDS;

			core::stringc shader_name = "";
			vid::ITexture *lmap=NULL, *tmap=NULL, *nmap=NULL;
			
			s32 t= Faces[i].textureID;
			s32 l= (Faces[i].lightmapID!=-1) ? (lmap_remap[Faces[i].lightmapID]) : (-1) ;

			if (t!=-1)				
			{
				tmap = tex[t];
				nmap = bump[t];
			}

			if (l!=-1)
			{
				lmap = lig[l];
			}

			if (t!=-1 && !IMaterialsManager::getSingleton().getMaterialAndVertexType(
					Textures[t].strName, lmap, mat, vtype))					
			{		
				if (!tmap && !tex_warns[t])
				{
					LOGGER.logWarn("Can't find Q3 texture %s", Textures[t].strName);
					tex_warns[t]=true;
				}

				// creating default material					
					
				vid::SRenderPass &pass = mat.getPass(0);

				pass.setAmbientColor(0xffffffff);
				pass.setDiffuseColor(0xffffffff);
				pass.setEmissiveColor(0xffffffff);
				pass.setSpecularColor(0x00000000);
				pass.setShininess ( 0.0f );

				pass.setFlag ( vid::EMF_BLENDING,			false	);
				pass.setFlag ( vid::EMF_ZWRITE_ENABLE,		true	);
				pass.setFlag ( vid::EMF_BACK_FACE_CULLING,	true	);
				pass.setDepthTest ( vid::ECT_LESS );
				
				// diffuse map
				pass.Layers[0].setTexture(tmap,	vid::ETLT_DIFFUSE_MAP, 0);
				// lightmap
				pass.Layers[1].setTexture(lmap, vid::ETLT_LIGHT_MAP, 1);	
					
				IMaterialsManager::getSingleton().correctMaterial(mat);
			}

			// setting up material 
			for (u32 p=0; p < mat.getPassesCount(); p++)
			{
				vid::SRenderPass &pass = mat.getPass(p);
				// lightmap scale
				for (s32 li=0; li<vid::MY_MATERIAL_MAX_LAYERS; li++)
					if (mat.getPass(p).Layers[li].getType() == vid::ETLT_LIGHT_MAP)
						mat.getPass(p).Layers[li].setTexRGBScale(2);
				pass.setSpecularColor(0x00000000);
				pass.setShininess ( 0.0f );
			}

			if (m > 0)
			{
				if (ModelEntities[m].Type == EBSPET_FUNC_DOOR)
				{
					vtype = vid::EVT_1TCOORDS_COLOURED;
					for (u32 p=0; p < mat.getPassesCount(); p++)
					{
						vid::SRenderPass &pass = mat.getPass(p);
						if (pass.Layers[1].getType() == vid::ETLT_LIGHT_MAP)
							pass.Layers[1].setTexture(NULL);
					}
				}
				else
				if (ModelEntities[m].Type == EBSPET_FUNC_TRANSPARENT)
				{
					mat = vid::SMaterial(mat.getPass(0));	
					vid::SRenderPass &pass = mat.getPass(0);
					vtype = vid::getVertexTypeFromComponents(
						(~vid::EVC_COL)&vid::VertexComponents[vtype]);
				}
			}

			// there are lightmapsids and textureid with -1

			s32 meshBufferIndex = ((l+1) * (NumTextures+1)) + (t+1);

			IMeshBuffer* mbuf = models_buffers[m][meshBufferIndex].MeshBuffer;

			if (!mbuf)
			{
				if (use_tbn && nmap)
				{	
					vtype = (vid::E_VERTEX_TYPE)(
						vtype + (vid::EVT_1TCOORDS_TBN-vid::EVT_1TCOORDS));
					u32 p = 0;
					for (p = 0; p < mat.getPassesCount(); p++)
						mat.getPass(p).Layers[3].setTexture(nmap, vid::ETLT_NORMAL_MAP);
					for (u32 p=0; p < mat.getPassesCount(); p++)
					{
						vid::SRenderPass &pass = mat.getPass(p);
						pass.setSpecularColor(0xff505050);
						pass.setShininess(100.0f);
					}
				}
				else
				if (use_tbn)
				{
					bool has_nmap = false;
					u32 p = 0;
					for (p = 0; p < mat.getPassesCount(); p++)
					{
						vid::SRenderPass &pass = mat.getPass(p);
						if (pass.Layers[3].getTexture() &&
							pass.Layers[3].getType() == vid::ETLT_NORMAL_MAP)
						{
							has_nmap = true;
							break;
						}
					}

					if (has_nmap)
					{
						vtype = (vid::E_VERTEX_TYPE)(
							vtype + (vid::EVT_1TCOORDS_TBN-vid::EVT_1TCOORDS));
						u32 p = 0;
						for (u32 p=0; p < mat.getPassesCount(); p++)
						{
							vid::SRenderPass &pass = mat.getPass(p);
							pass.setSpecularColor(0xff505050);
							pass.setShininess(100.0f);
						}
					}
				}

				mbuf = mesh_buffer_factory::create(vtype);					

				models_buffers[m][meshBufferIndex].MeshBuffer = mbuf;	

				mbuf->getMaterial() = mat;
			}

			models_buffers[m][meshBufferIndex].BSPFaces.push_back(i);
		} 
	} 

	for (u32 l=0; l<lig.size(); l++ )
		lig[l]->drop();
	lig.clear();

	SAFE_DELETE_ARRAY(tex_warns);

	// fills our meshes with geometry

	beginCreatingMesh(this);

	for (s32 m=0; m<NumModels; m++)
	{
		for (u32 mb=0; mb<models_buffers[m].size(); mb++)
		{
			SModelMeshBuffer& model_mb = models_buffers[m][mb];
			IMeshBuffer*      mbuf     = model_mb.MeshBuffer;

			if (!mbuf)
				continue;

			E_BSP_ENTITY_TYPE etype = ModelEntities[m].Type;

			/////////////////
			// simple faces
			/////////////////

			beginAddingSimpleBSPFacesToMeshBuffer(mbuf);

			for (u32 f=0; f<model_mb.BSPFaces.size(); f++)
			{
				s32 i = model_mb.BSPFaces[f];

				core::dimension2df lig_coord_ofs(0,0);
				f32 lig_coord_scl=1.0f;
				s32 lig_id = Faces[i].lightmapID;

				if (lig_id != -1)
				{
					lig_coord_ofs = lig_coord_offset[lig_id];
					lig_coord_scl = lig_coord_scale[lig_id];
				}

				if (Faces[i].type==1 || Faces[i].type==3)				
					addSimpleBSPFaceToMeshBuffer(
						Faces[i], lig_coord_ofs, lig_coord_scl, lig_id);
			}

			endAddingSimpleBSPFacesToMeshBuffer();

			////////////////////
			// patches 
			////////////////////

			for (u32 f=0; f<model_mb.BSPFaces.size(); f++)
			{
				s32 i = model_mb.BSPFaces[f];

				core::dimension2df lig_coord_ofs(0,0);
				f32 lig_coord_scl=1.0f;
				s32 lig_id = Faces[i].lightmapID;

				if (lig_id != -1)
				{
					lig_coord_ofs = lig_coord_offset[lig_id];
					lig_coord_scl = lig_coord_scale[lig_id];
				}

				if (Faces[i].type == 2)
				// curved face
					addPatchBSPFaceToMeshBuffer(
						mbuf, Faces[i], lig_coord_ofs, lig_coord_scl);
			}            
		}
	}

	endCreatingMesh();

	for (s32 m=0; m<NumModels; m++)
	{
		SMesh *mesh = 0;

		if (m==0)
		{
			mesh = Mesh;
		}
		else
		{
			// we know that all meshes have SMesh type, so use this type conversion
			mesh = static_cast<SMesh*>(AMeshes[m-1]->getMesh(0));
		}

		for (u32 mb=0; mb<models_buffers[m].size(); mb++)
		{
			IMeshBuffer * mbuf = models_buffers[m][mb].MeshBuffer;

			if (!mbuf)
				continue;

			if (mbuf->getVertexCount() > 0 && mbuf->getIndexCount() > 0)			
				// adding this mesh buffer to the mesh
				mesh->addMeshBuffer(mbuf);

			// one grab is much enough
			mbuf->drop();	
		}		
		
		// create bounding boxes

		mesh->getBoundingBox().MinEdge = 
			core::vector3df(Models[m].min[0], Models[m].min[2], Models[m].min[1]);
		mesh->getBoundingBox().MaxEdge = 
			core::vector3df(Models[m].max[0], Models[m].max[2], Models[m].max[1]);	

		if (m>0)
		{
			AMeshes[m-1]->recalculateBoundingBox();
		} 
	}  	

	end = TIMER.getSystemTime();

	sprintf(ch, " Done in %dms.", end-start);
	LOGGER.log(ch, io::ELL_INFORMATION);

	////////////////////////////////////////////////////////////////////////

	// store at q3 entities,
	// to have a fast access to them from q3 entities entries

	for (s32 me=1; me<NumModels; me++)
	{
		E_BSP_ENTITY_TYPE etype = ModelEntities[me].Type;	
		s32              idx   = ModelEntities[me].Index;

		if (idx != -1)
		{
			SAnimatedMesh* amesh = AMeshes[ me-1 ];				
				
			CQ3Entity &q3ent = Q3Entities[etype][idx];

			if (amesh && !isNeedToAddToMesh0(etype))
			{
				IMesh* mesh = amesh->getMesh();
				if (mesh && mesh->getMeshBuffersCount()>0)
					q3ent.ModelMesh = amesh;
				else
				{
					SAFE_DROP(amesh);
					AMeshes[ me-1 ] = 0;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	s32 unkn_count = Q3Entities[EBSPET_UNKNOWN].size();
	for (s32 u = 0; u < unkn_count; u++)
	{
		s32 modelidx = Q3Entities[EBSPET_UNKNOWN][u].getKeyValueAsInt("model");
		if (modelidx > 0 && modelidx < NumModels)
		{
			SAnimatedMesh* amesh = AMeshes[modelidx - 1];
			if (amesh)
			{
				IMesh* mesh = amesh->getMesh();
				if (mesh)
					for (s32 mb = 0; mb < mesh->getMeshBuffersCount(); mb++)
						Mesh->addMeshBuffer(mesh->getMeshBuffer(mb));
				SAFE_DROP(amesh);
				AMeshes[modelidx - 1] = 0;
			}
		}
	}

	Q3Entities[EBSPET_UNKNOWN].clear();

	////////////////////////////////////////////////////////////////////////

	recalculateBoundingBox();
}

//---------------------------------------------------------------------------

//! loads the texture
vid::ITexture* CQ3LevelMesh::getQ3Texture(const c8 *texname)
{
	s32 e=0;
	core::stringc s;
	c8 *q3_tex_ext[2] = { ".jpg", ".tga" };	

	// try to find in zip packages

	for (e=0; e<2; ++e)
	{
		s = texname;
        s.append(q3_tex_ext[e]);

		vid::ITexture* tex = VIDEO_DRIVER.findTexture(s.c_str());
		if (tex)
			return tex;

        if (FILE_SYSTEM.existFile(s.c_str()))
			return VIDEO_DRIVER.getTexture(s.c_str());
    }

	// try to find in default media dir

	core::stringc fulltexname = 
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_ROOT );	
	fulltexname.append(texname);	

	for (e=0; e<2; ++e)
	{
		s = fulltexname;
        s.append(q3_tex_ext[e]);

		vid::ITexture* tex = VIDEO_DRIVER.findTexture(s.c_str());
		if (tex)
			return tex;

        if (FILE_SYSTEM.existFile(s.c_str()))
        {
			img::IImage* image = IMAGE_LIBRARY.getImage(s.c_str());
			if (image)
				return VIDEO_DRIVER.addTexture(s.c_str(), image);
			break;
		}
    }	
	return 0;
}

//---------------------------------------------------------------------------

//! loads the texture
vid::ITexture* CQ3LevelMesh::getQ3TextureWithExt(const c8 *texname)
{
	core::stringc fext = core::extractFileExt(core::stringc(texname));	

	if (fext.size()>0)
	{
		// try to find already loaded texture

		vid::ITexture* tex = VIDEO_DRIVER.findTexture(texname);

		if (tex)
			return tex;

		// try to find in zip packages	

		if (FILE_SYSTEM.existFile(texname))
			tex = VIDEO_DRIVER.getTexture(texname);

		if (tex)
			return tex;

		// try to change ext

		core::stringc fpath = core::extractFilePath(core::stringc(texname));
		core::stringc fname = core::extractFileName(core::stringc(texname));

		core::stringc fpath_fname;

		if (fext==core::stringc("jpg"))
			core::changeFileExt(fname, "tga");
		else
		if (fext==core::stringc("tga"))
			core::changeFileExt(fname, "jpg");
		
		fpath_fname = fpath;
		fpath_fname.append(fname);

		if (FILE_SYSTEM.existFile(fpath_fname.c_str()))
		{
			img::IImage* image = IMAGE_LIBRARY.getImage(fpath_fname.c_str());
			if (image)
				tex = VIDEO_DRIVER.addTexture(texname, image);
		}

		if (tex)
			return tex;

		// try to find in default media dir

		fpath = core::extractFilePath(core::stringc(texname));
		fname = core::extractFileName(core::stringc(texname));

		core::stringc fulltexname = 
			RESOURCE_MANAGER.getMediaDirFull ( res::EMT_ROOT );	
		fulltexname.append(fpath);
		fulltexname.append(fname);
		
		tex = VIDEO_DRIVER.findTexture(fulltexname.c_str());
		if (tex)
			return tex;
		if (FILE_SYSTEM.existFile(fulltexname.c_str()))
		{
			img::IImage* image = IMAGE_LIBRARY.getImage(fulltexname.c_str());		
			if (image)
				tex = VIDEO_DRIVER.addTexture(texname, image);
		}
		if (tex)
			return tex;

		if (fext==core::stringc("jpg"))
			core::changeFileExt(fname, "tga");
		else
		if (fext==core::stringc("tga"))
			core::changeFileExt(fname, "jpg");

		fulltexname = 
			RESOURCE_MANAGER.getMediaDirFull ( res::EMT_ROOT );	
		fulltexname.append(fpath);
		fulltexname.append(fname);
		
		tex = VIDEO_DRIVER.findTexture(fulltexname.c_str());
		if (tex)
			return tex;
		if (FILE_SYSTEM.existFile(fulltexname.c_str()))
		{
			img::IImage* image = IMAGE_LIBRARY.getImage(fulltexname.c_str());		
			if (image)
				tex = VIDEO_DRIVER.addTexture(texname, image);
		}

		return tex;		
	}

	return getQ3Texture(texname);
}

//---------------------------------------------------------------------------

//! returns white texture
vid::ITexture* CQ3LevelMesh::getQ3WhiteImage()
{
	vid::ITexture *white_tex = 
		VIDEO_DRIVER.findTexture("q3.whiteimage");

	if (!white_tex)
	{
		img::IImage *white_img = IMAGE_LIBRARY.createEmptyImage(
			core::dimension2di(32,32), img::ECF_A8R8G8B8
			);
		white_img->fill(img::SColor(255,255,255,255));

		white_tex = VIDEO_DRIVER.addTexture("q3.whiteimage" , white_img);
		SAFE_DROP(white_img);
	}

	return white_tex;
}

//---------------------------------------------------------------------------

//! return true for caulk, hint and so on textures
bool CQ3LevelMesh::isRadiantHintTexture(c8* texname)
{
	core::stringc tex = texname;

	return (
		tex == core::stringc("textures/NULL") || 
		tex == core::stringc("textures/common/caulk") || 
		tex == core::stringc("textures/common/caulk.tga") ||
		tex == core::stringc("textures/notex") ||
		tex == core::stringc("textures/") ||
		tex == core::stringc("noshader")
		);
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::rescaleAnimatedMeshNormals(const core::vector3df &newScale)
{
	if (Mesh && NumModels>0)
		Mesh->rescaleMeshNormals(newScale);
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::recalculateBoundingBox()
{
	if (Mesh && NumModels>0)
	{
		m_BoundingBox = Mesh->getBoundingBox();
	}
	else
	{
		m_BoundingBox.reset(0,0,0);
	}
}

//---------------------------------------------------------------------------

//! return Entities count	
s32 CQ3LevelMesh::getEntitiesCount(E_BSP_ENTITY_TYPE type)
{
	return Q3Entities[type].size();
}

//---------------------------------------------------------------------------

//! return Entity by its type and index
IBSPEntity& CQ3LevelMesh::getEntity(E_BSP_ENTITY_TYPE type, s32 idx)
{
	if (idx>=0 && idx<(s32)Q3Entities[type].size())
		return Q3Entities[type][idx];

	return *((IBSPEntity*)0);
}

//---------------------------------------------------------------------------

//! loads a level from a .bsp-File. Also tries to load all needed textures. 
//! Returns true if successful.
bool CQ3LevelMesh::loadFromFile(io::IReadFile* file)
{
    if (!file)
        return false;
   
    LevelName = file->getFileName();
	LevelPath = file->getFilePath();

	LevelPath = FILE_SYSTEM.optimizeDirectoryString(LevelPath.c_str());

	// read bsp header

    file->read(&Header, sizeof(tBSPHeader));

    if (Header.strID != 0x50534249 || Header.version != 0x2e)
    {
		LOGGER.logErr("Could not load .bsp file, unknown header. :%s",
			file->getFileName());
        return false;
    }

    // now read lumps

    file->read(&Lumps[0], sizeof(tBSPLump)*kMaxLumps);

    // load data

	loadData(file);	

	// initialize data
	
	parseEntities();    
	initLightGrid();		
	constructMesh();

    return true;
}

//---------------------------------------------------------------------------

void CQ3LevelMesh::freeUnusedData()
{
	for (s32 i=0; i<E_BSP_ENTITY_TYPE_COUNT; i++)
	{
		Q3Entities[i].clear();
	}
    
	ModelEntities.clear();

	SAFE_DROP(Mesh);
	SAFE_DELETE_ARRAY(Textures);
    SAFE_DELETE_ARRAY(LightMaps);    
    SAFE_DELETE_ARRAY(Vertices);
    SAFE_DELETE_ARRAY(Faces);
    SAFE_DELETE_ARRAY(Planes);
    SAFE_DELETE_ARRAY(Nodes);
    SAFE_DELETE_ARRAY(Leafs);
    SAFE_DELETE_ARRAY(LeafFaces);    
	SAFE_DELETE_ARRAY(LeafBrushes);
	SAFE_DELETE_ARRAY(Models);  
    SAFE_DELETE_ARRAY(Brushes);
	SAFE_DELETE_ARRAY(BrushSides);
	SAFE_DELETE_ARRAY(MeshVerts);		
	SAFE_DELETE_ARRAY(StrEntities);	
	SAFE_DELETE_ARRAY(Shaders);
	SAFE_DELETE_ARRAY(VisData.pBitsets);
}

//---------------------------------------------------------------------------

//! saving a level to a .bsp-File
bool CQ3LevelMesh::saveToFile(const c8* filename)
{	
	if (Header.strID != 0x50534249 || Header.version != 0x2e)
    {
        LOGGER.logErr("Could not save .bsp file, wrong header.");
        return false;
    }

	LOGGER.log("Saving BSP data:");

	io::IWriteFile* file = 
		FILE_SYSTEM.createAndWriteFile(filename);

	if (!file)
        return false;

	// saving header

	file->seek(0);
	file->write(&Header, sizeof(tBSPHeader));

	// saving lumps
	
	for (s32 l=0; l<kMaxLumps; l++)
	// updating lumps offsets
	{
		if (l==kEntities)
		{
			Lumps[kEntities].offset = sizeof(tBSPHeader)+sizeof(tBSPLump)*kMaxLumps;
		}
		else
		{	
			Lumps[l].offset = Lumps[l-1].offset + Lumps[l-1].length;
		}
	}

	file->seek(sizeof(tBSPHeader));
    file->write(Lumps, sizeof(tBSPLump)*kMaxLumps);

	// saving data

	void *bsp_data[kMaxLumps] = 
	{
		StrEntities,
		Textures,
		Planes,
		Nodes,
		Leafs,
		LeafFaces,
		LeafBrushes,
		Models,
		Brushes,
		BrushSides,
		Vertices,
		MeshVerts,
		Shaders,
		Faces,
		LightMaps,
		LightVols,
		&VisData
	};

	for (s32 l=0; l<kMaxLumps; l++)
	{
		if (Lumps[l].length>0 && bsp_data[l])
		{
			file->seek(Lumps[l].offset);
			file->write(bsp_data[l], Lumps[l].length);
		}		
	}

	file->drop();	

	LOGGER.increaseFormatLevel();
	for (s32 i=0; i<kMaxLumps; i++)
	{
		printBspInfo((eLumps)i);
	}
	LOGGER.decreaseFormatLevel();

	LOGGER.logInfo("BSP saved to: %s", filename);	

	return true;
}

//---------------------------------------------------------------------------

vid::SLight CQ3LevelMesh::getLightFromPos(core::vector3df _pos)
{
	vid::SLight light;

	light.DiffuseColor.set(1.0f, 1.0f, 1.0f);
	light.AmbientColor.set(1.0f, 1.0f, 1.0f);
	light.Position.set(0,0,0);

	if (!LightGridOK)
	{
		return light;
	}

	f32 light_pos[3] = {
		_pos.X - LightGridData.lightGridOrigin[0],
		_pos.Z - LightGridData.lightGridOrigin[1],
		_pos.Y - LightGridData.lightGridOrigin[2],		
		};

    s32 pos[3];
    s32 i, j;
    u8  *grid_data;
    f32 frac[3];
    s32 grid_step[3];
    f32 direction[3] = {0,0,0};
    f32 total_factor;
	u8 ambient_light[3] = {0,0,0};
	u8 directed_light[3] = {0,0,0};	

	s32 light_grid_bounds_min_1[3] = {
		LightGridData.lightGridBounds[0] - 1,
		LightGridData.lightGridBounds[1] - 1,
		LightGridData.lightGridBounds[2] - 1,
		};

    for ( i = 0 ; i < 3 ; i++ ) 
	{
        float   v;

        v = light_pos[i] / (f32)LightGridData.lightGridSize[i];

        pos[i] = floor( v );

        frac[i] = v - pos[i];

        if ( pos[i] < 0 ) 
		{
            pos[i] = 0;
        } 
		else if ( pos[i] >= light_grid_bounds_min_1[i] ) 
		{
            pos[i] = light_grid_bounds_min_1[i];
        }
    }

    // trilerp the light value
    grid_step[0] = 8;
    grid_step[1] = 8 * LightGridData.lightGridBounds[0];
    grid_step[2] = 8 * LightGridData.lightGridBounds[0] * LightGridData.lightGridBounds[1];
    grid_data = 
		(u8*)(LightGridData.lightGridData) + 
		pos[0]*grid_step[0] + pos[1]*grid_step[1] + pos[2]*grid_step[2];

    total_factor = 0;

    for ( i = 0 ; i < 8 ; i++ ) 
	{
        float   factor;
        u8     *data;
		core::vector3df normal;

        factor = 1.0;
        data = grid_data;	

        for ( j = 0 ; j < 3 ; j++ ) 
		{
            if ( i & (1<<j) ) 
			{				
                factor *= frac[j];

				if (pos[j]<(light_grid_bounds_min_1[j]))
				{
					data += grid_step[j];
				}
            } 
			else 
			{
                factor *= (1.0f - frac[j]);
            }
        }

        if ( !(data[0]+data[1]+data[2]) ) 
		{
            continue;   // ignore samples in walls
        }

        total_factor += factor;

        ambient_light[0] += factor * data[0];
        ambient_light[1] += factor * data[1];
        ambient_light[2] += factor * data[2];

        directed_light[0] += factor * data[3];
        directed_light[1] += factor * data[4];
        directed_light[2] += factor * data[5];

        // Converting lattitude and longitude to vector
		normal.setFromLatLong(&data[6]);

		direction[0] = direction[0] + normal.X * factor;
		direction[1] = direction[1] + normal.Y * factor;
		direction[2] = direction[2] + normal.Z * factor;
    }

    if ( total_factor > 0 && total_factor < 0.99 ) 
	{
        total_factor = 1.0f / total_factor;

        ambient_light[0] *= total_factor;
		ambient_light[1] *= total_factor;
		ambient_light[2] *= total_factor;

		directed_light[0] *= total_factor;
		directed_light[1] *= total_factor;
		directed_light[2] *= total_factor;
    }

	core::vector3df light_dir(direction[0], direction[2], direction[1]);
	light_dir.normalize();

	img::SColor DiffuseColor( 255, directed_light[0], directed_light[1], directed_light[2] );
	
	f32 kamb = 1.0f;
	f32 kamb_inv = 1.0f - kamb;

	img::SColor AmbientColor(
		255,
		kamb * ambient_light[0] + kamb_inv * DiffuseColor.getRed(),
		kamb * ambient_light[1] + kamb_inv * DiffuseColor.getGreen(),
		kamb * ambient_light[2] + kamb_inv * DiffuseColor.getBlue()
		);

	// setting light parameters

	light.DiffuseColor  = img::SColorf(DiffuseColor);
	light.AmbientColor  = img::SColorf(AmbientColor);
	light.SpecularColor = light.DiffuseColor;

	light.CastShadows = false;

	light.Position = light_dir;

	light.Type = vid::ELT_DIRECTIONAL;

	light.Radius = 10000;

	light.Enabled = true;

	return light;
}

//---------------------------------------------------------------------------

bool CQ3LevelMesh::isLightGridOK()
{
	return LightGridOK;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
