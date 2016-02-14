//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshCal3d.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CAnimatedMeshCal3d.h"

#include <scn/mesh_buffer.h>
#include <scn/SMesh.h>
#include <io/IFileSystem.h>
#include <io/ILogger.h>
#include <img/IImageLibrary.h>
#include <dev/IDevice.h>

#include <cal3d/coretrack.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

f32			CAnimatedMeshCal3d::m_Cal3dBonesLines[1024][2][3];
CalIndex	CAnimatedMeshCal3d::m_Cal3dMeshFaces[50000][3];

//----------------------------------------------------------------------------

CAnimatedMeshCal3d::CAnimatedMeshCal3d() 
	: NormalScale(core::vector3df(1,1,1)), m_Cal3dCoreModel(NULL), m_Cal3dModel(NULL),
	m_ExecutingAction(false), m_KeyFrameFPS(15), m_IndicesOK(false),
	m_OveralVertexCount(0), m_OveralFaceCount(0)
{
#if MY_DEBUG_MODE 
    IAnimatedMeshCal3d::setClassName("CAnimatedMeshCal3d IAnimatedMeshCal3d");
#endif 

	m_Scale     = 1.0f;
	m_BlendTime = 0.05f;

	m_bPaused   =false;
	m_Initialized=false;

	m_Cal3dCoreModel = new CalCoreModel("dummy");	

	VIDEO_DRIVER.grab();
	FILE_SYSTEM.grab();

	m_OveralSmooth      = false;
	m_InterpolateAnimation = DEVICE.getDeviceFlagValue(dev::EDCF_HI_ANIMATION_QUALITY);
}

//----------------------------------------------------------------------------

CAnimatedMeshCal3d::~CAnimatedMeshCal3d()
{
	_clearCal3dAnimations();

	SAFE_DELETE(m_Cal3dModel);
	SAFE_DELETE(m_Cal3dCoreModel);

	VIDEO_DRIVER.drop();
	FILE_SYSTEM.drop();
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_clearCal3dAnimations()
{
	u32 cal_animcnt = m_Cal3dCoreModel->getCoreAnimationCount();
	for (u32 i = 0; i < cal_animcnt; i++)
		_clearCal3dAnimation(i);
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_clearCal3dAnimation(u32 idx)
{
	CalCoreAnimation* calanim = m_Cal3dCoreModel->getCoreAnimation(idx);
	std::list<CalCoreTrack *> &listCoreTrack = calanim->getListCoreTrack();
	// destroy all core tracks
	while(!listCoreTrack.empty())
	{
		CalCoreTrack *pCoreTrack = listCoreTrack.front();
		listCoreTrack.pop_front();
		pCoreTrack->destroy();
		delete pCoreTrack;
	}
	m_Cal3dCoreModel->unloadCoreAnimation(idx);
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::loadFromFile(io::IReadFile *file)
{   
	bool res = false;
	if (!file)
		return res;

	m_ConfigFile = file->getFileName();
    m_MediaPath = core::extractFilePath(m_ConfigFile);

	LOGGER.logInfo("Loading Cally3D mesh from file '%s' . . .",
		m_ConfigFile.c_str());
    (res = init()) ? LOGGER.logInfo("Cally3D mesh loaded.") :
		LOGGER.logErr("Loading Cally3D mesh!");

	return res;
}

//------------------------------------------------------------------------------	
	
bool CAnimatedMeshCal3d::init()
{    
	if (!_parseModelConfiguration())
	{
		m_Initialized = false;
		return false;
	}

	// make one material thread for each material

	for(s32 materialId = 0; materialId < m_Cal3dCoreModel->getCoreMaterialCount(); materialId++)
	{
		// create the a material thread
		m_Cal3dCoreModel->createCoreMaterialThread(materialId);

        // initialize the material thread
		m_Cal3dCoreModel->setCoreMaterialId(materialId, 0, materialId);
	}

	// load all textures and store the texture id in the corresponding map in the material

	core::stringc TexturePath = SCENE_MANAGER.getTexturesPath();

	for (s32 materialId = 0;
		materialId < m_Cal3dCoreModel->getCoreMaterialCount(); materialId++)
	{
		// get the core material
		CalCoreMaterial *pCoreMaterial = m_Cal3dCoreModel->getCoreMaterial(materialId);		

		vid::SMaterial material;

		vid::SRenderPass &pass = material.getPass(0);

		pass.setLightingMode(vid::ELM_8_LIGHTS);		
		pass.setFlag(vid::EMF_ZWRITE_ENABLE, true);
		pass.setFlag(vid::EMF_BACK_FACE_CULLING, true);
		
		CalCoreMaterial::Color cal3d_color;

		// set the material ambient color
		cal3d_color = pCoreMaterial->getAmbientColor();
		pass.setAmbientColor(img::SColor(
			cal3d_color.alpha, cal3d_color.red, cal3d_color.green, cal3d_color.blue));
					
	    // set the material diffuse color
		cal3d_color = pCoreMaterial->getDiffuseColor();
		pass.setDiffuseColor(img::SColor(
			cal3d_color.alpha, cal3d_color.red, cal3d_color.green, cal3d_color.blue));
							
		// set the material specular color
		cal3d_color = pCoreMaterial->getSpecularColor();
		pass.setSpecularColor(img::SColor(
			cal3d_color.alpha, cal3d_color.red, cal3d_color.green, cal3d_color.blue));
						
        // set the material shininess factor
		pass.setShininess(pCoreMaterial->getShininess() * 100.0f);
		if (pass.getShininess () == 0)
			pass.setSpecularColor(0x00000000);

		// loop through all maps of the core material
		for (s32 mapId = 0; mapId < pCoreMaterial->getMapCount() &&
			mapId < vid::MY_MATERIAL_MAX_LAYERS; mapId++)
		{
			// get the filename of the texture
			std::string fn = std::string(
				TexturePath.c_str()) + pCoreMaterial->getMapFilename(mapId);
			
			// load software image from the file
			img::IImage* image = IMAGE_LIBRARY.getImage(fn.c_str());
			if (!image)
				continue;

			bool has_alpha_channel = (image->getColorFormat() == img::ECF_A8R8G8B8);

			// load hardware texture by file name

			vid::ITexture* texture =
				VIDEO_DRIVER.getTexture(fn.c_str());

			SAFE_CALL(texture, freeImageData());			

			pass.Layers[mapId].setTexture(texture);

			if (texture && mapId>0)
			{
				pass.Layers[mapId].setTexCoordGen(vid::ETCGT_ENVIRONMENT_MAPPING);
			}

			if (has_alpha_channel)
			{
				pass.setAlphaTest(vid::EAT_GE_80);
				pass.setFlag(vid::EMF_BACK_FACE_CULLING, true);
			}				
		}

		s32 mat_id = -1;

		u32 materialsCount = m_Cal3dMesh.m_Materials.size();
		for (u32 i=0; i < materialsCount; i++)
		{
			if (m_Cal3dMesh.m_Materials[i] == material)
			{
				mat_id = i;
				break;
			}			
		}

		if (mat_id == -1)
		{
			m_Cal3dMesh.m_Materials.push_back ( material );
			mat_id = m_Cal3dMesh.m_Materials.size()-1;
		}

		// store the user data of the material

		pCoreMaterial->setUserData( Cal::UserData((void*)(size_t)mat_id) );
	}

	// creating bones structure

	CalCoreSkeleton* pCalSkeleton = m_Cal3dCoreModel->getCoreSkeleton();

	std::vector<CalCoreBone*> &VectorBone = pCalSkeleton->getVectorCoreBone();

	u32 bones_size = (u32)VectorBone.size();
	m_SkeletonBones.set_used(bones_size);
	for(u32 i=0; i < bones_size; i++)
	{
		CalCoreBone *corebone = VectorBone[i];
		m_SkeletonBones[i].Name         = corebone->getName().c_str();	
		m_SkeletonBones[i].ParentBoneId = corebone->getParentId();
	}
	m_MappedBoneTransformations.set_used(bones_size);

	return true;
}

//------------------------------------------------------------------------------

bool CAnimatedMeshCal3d::_parseModelConfiguration()
{
	std::string fn;

	// open the model configuration file
	std::ifstream file;
	core::stringc &cf = m_ConfigFile;
	file.open(cf.c_str(), std::ios::in | std::ios::binary);
	if(!file)
	{
        LOGGER.logErr("Failed to open model conf file %s!", cf.c_str());
		return false;
	}

	CalLoader::setLoadingMode(LOADER_ROTATE_X_AXIS);

	std::string mediaPath = m_MediaPath.c_str();
    
	// parse all lines from the model configuration file
	s32 line;
	for (line = 1; ; line++)
	{
		// read the next model configuration line
		std::string strBuffer;
		std::getline(file, strBuffer);
		// stop if we reached the end of file
		if(file.eof()) break;

		// check if an error happend while reading from the file
		if(!file)
		{
            LOGGER.logErr("Error while reading from the model conf file %s!",
				cf.c_str());
			return false;
		}

		// find the first non-whitespace character
		std::string::size_type pos;
		pos = strBuffer.find_first_not_of(" \t");

        // check for empty lines
		if (pos == std::string::npos ||
				strBuffer[pos] == '\n' ||
				strBuffer[pos] == '\r' ||
				strBuffer[pos] == 0)
			continue;

		// check for comment lines
		if (strBuffer[pos] == '#')
			continue;

		// get the key
		std::string strKey;
		strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
		pos += strKey.size();

		// get the '=' character
		pos = strBuffer.find_first_not_of(" \t", pos);
		if((pos == std::string::npos) || (strBuffer[pos] != '='))
		{
            LOGGER.logErr("In file %s (line %d) - Invalid syntax!",
				cf.c_str(),line);
			return false;
		}

		// find the first non-whitespace character after the '=' character
		pos = strBuffer.find_first_not_of(" \t", pos + 1);

		// get the data
		std::string strData;
		strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

		// handle the model creation
		if(strKey == "scale")
		{
			// set rendering scale factor
			m_Scale = float(atof(strData.c_str()));
		}
		else if(strKey == "skeleton")
		{			
			fn = mediaPath + strData;
            // load core skeleton
            LOGGER.logInfo(" Loading skeleton: %s . . .", fn.c_str());
			if(!m_Cal3dCoreModel->loadCoreSkeleton(fn))
			{
                LOGGER.logErr("Failed to open skeleton file!");
				return false;
			}
		}
		else if(strKey == "animation")
		{	
			fn = mediaPath + strData;
			//registerAnimation(core::stringc(fn.c_str()), 150.0f);
		}
		else if(strKey == "mesh")
		{
			fn = mediaPath + strData;
			// load core mesh
            LOGGER.logInfo(" Loading mesh: %s . . .",fn.c_str());
			if(m_Cal3dCoreModel->loadCoreMesh(fn) == -1)
			{
				LOGGER.logErr("Failed to open mesh file!");
				return false;
			}
		}
		else if(strKey == "material")
		{
			fn = mediaPath + strData;
			// load core material
            LOGGER.logInfo(" Loading material: %s . . .", fn.c_str());
			if(m_Cal3dCoreModel->loadCoreMaterial(fn) == -1)
			{
                LOGGER.logErr("Failed to open material file!");
				return false;
			}
		}
		else
		{
			// everything else triggers an error message, but is ignored
            LOGGER.logErr("In file %s (line %d) - Invalid syntax!",
				cf.c_str(), line);
		}
	}

	// explicitely close the file
	file.close();

	return true;
}

//------------------------------------------------------------------------------

void CAnimatedMeshCal3d::recalculateBoundingBox() 
{
	if (!m_Initialized)
		return;

	m_InterfaceMesh.recalculateBoundingBox();

	m_BoundingBox = m_InterfaceMesh.getBoundingBox();
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::rescaleAnimatedMeshNormals(const core::vector3df &newScale)
{ NormalScale = newScale; }

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_getAnimatedGeometry(
	core::array <vid::S3DVertex1TCoords*> *vertices,
	core::array <u16*> *indices)
{
	if (m_Initialized && !m_OveralSmooth)
		return;

	u32 materialsCount = m_Cal3dMesh.m_Materials.size();

	for (u32 m = 0; m < materialsCount; m++ )
		m_Cal3dMesh.m_VerticesCount[m] = 0;

	CalRenderer *pCalRenderer = m_Cal3dModel->getRenderer();

	// begin the rendering loop
	if (pCalRenderer->beginRendering())
	{
		// get the number of meshes
		int meshCount = pCalRenderer->getMeshCount();

		// render all meshes of the model

		for(s32 meshId = 0; meshId < meshCount; meshId++)
		{
			// get the number of submeshes
			s32 submeshCount = pCalRenderer->getSubmeshCount(meshId);

			CalMesh * cal_mesh = m_Cal3dModel->getMesh(meshId);
		        
			// render all submeshes of the mesh				
			for(s32 submeshId = 0; submeshId < submeshCount; submeshId++)
			{
				// select mesh and submesh for further data access
				if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
				{ 
					CalSubmesh *cal_submesh = cal_mesh->getSubmesh(submeshId);

					s32 cal_mat_id   = cal_submesh->getCoreMaterialId();

					CalCoreMaterial *pCoreMaterial = m_Cal3dCoreModel->getCoreMaterial(cal_mat_id);

					s32 mat_id = (s32)(size_t)pCoreMaterial->getUserData();

					// vertices

					u32 vCnt = cal_submesh->getVertexCount();

					s32 init_vsize = m_Cal3dMesh.m_VerticesCount[mat_id];

					float *pVert = (float *)((*vertices)[mat_id] + init_vsize);
					pCalRenderer->getVerticesNormalsAndTexCoords(pVert, 1);

					m_Cal3dMesh.m_VerticesCount[mat_id] += vCnt;

					if (!m_IndicesOK)
					{
						u32 fCnt = pCalRenderer->getFaces(&m_Cal3dMeshFaces[0][0]);	

						s32 init_isize = m_Cal3dMesh.m_IndicesCount[mat_id];
						
						for ( u32 f = 0, i = init_isize; f < fCnt; f++, i += 3 )
						{
							m_Cal3dMesh.m_Indices[mat_id][i  ] = init_vsize + m_Cal3dMeshFaces[f][0];
							m_Cal3dMesh.m_Indices[mat_id][i+1] = init_vsize + m_Cal3dMeshFaces[f][1];
							m_Cal3dMesh.m_Indices[mat_id][i+2] = init_vsize + m_Cal3dMeshFaces[f][2];
						}

						m_Cal3dMesh.m_IndicesCount[mat_id] += 3*fCnt;
					}
				}
			}
		}

		m_IndicesOK = true;

		// end the rendering
		pCalRenderer->endRendering();
	}

	if (indices)
	{
		for ( u32 m = 0; m < materialsCount; m++ )
			memcpy((*indices)[m], m_Cal3dMesh.m_Indices[m],
				m_Cal3dMesh.m_MaxIndicesCount[m] * sizeof(u16));
	}
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_getInterpolatedGeometry(
	core::array<vid::S3DVertex1TCoords*> *vertices,
	core::array<u16*> *indices)
{
	bool external_dest = indices != NULL;

	u32 materialsCount = m_Cal3dMesh.m_Materials.size();

	SCal3dSpecialAnimation *spec_anim = NULL;
	SAnimation *anim = NULL;

	if (m_CurrentAnimationIdx >= 0)
	{
		spec_anim = (SCal3dSpecialAnimation*)
			m_Animations[m_CurrentAnimationIdx]->SpecialAnimation;
		anim = m_Animations[m_CurrentAnimationIdx];
	}

	bool looped = anim ? anim->Looping : false;
	f32 dur   = spec_anim ? spec_anim->Duration : 1.f;
	f32 start = anim ? (f32)anim->BeginFrame : 0.f;
	f32 end   = anim ? (f32)anim->EndFrame : 0.f;
	f32 time  = spec_anim ? spec_anim->Time : 0.f;
	f32 end_start = end - start + 1;
	f32 cur_fr = (((end_start - 1) * time )/dur ) + start;
	CHECK_MIN_RANGE(cur_fr, 0);

	s32 next_frame = m_Cal3dMesh.m_CurrentFrame + 1;
	if (looped)
	{				
		if (next_frame >= end)
			next_frame = start;
	}
	else
	{
		if (next_frame >= end)
			next_frame = end;
	}

	f32 interpolation_weight = ((cur_fr - start) / end_start) * end_start -
		(m_Cal3dMesh.m_CurrentFrame - start);		

	f32 interpolation_weight_inv = 1.0f - interpolation_weight;   

	// smooth linear interpolation between  two frames		

	if (!external_dest)
	{
		for (u32 m = 0; m < materialsCount; m++ )
		{
			u32 vert_count = m_Cal3dMesh.m_MaxVerticesCount[m];

			vid::S3DVertex1TCoords *target = (*vertices)[m];		
			vid::S3DVertex1TCoords *first  = m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame][m];
			vid::S3DVertex1TCoords *second = m_Cal3dMesh.m_FrameList[next_frame][m];

			for ( u32 i = 0; i < vert_count; ++i )
			{
				target->Pos = second->Pos*interpolation_weight +
					first->Pos*interpolation_weight_inv;
				target->Normal = first->Normal;
			
				++target, ++first, ++second;
			}
		}
	}
	else
	{
		for (u32 m = 0; m < materialsCount; m++ )
		{
			u32 vert_count = m_Cal3dMesh.m_MaxVerticesCount[m];

			vid::S3DVertex1TCoords *target = (*vertices)[m];		
			vid::S3DVertex1TCoords *first  = m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame][m];
			vid::S3DVertex1TCoords *second = m_Cal3dMesh.m_FrameList[next_frame][m];

			for ( u32 i = 0; i < vert_count; ++i )
			{
				target->Pos = second->Pos*interpolation_weight +
					first->Pos*interpolation_weight_inv;
				target->Normal = first->Normal;
				target->TCoords = first->TCoords;
			
				++target, ++first, ++second;
			}
		}

		for ( u32 m = 0; m < materialsCount; m++ )
			memcpy( (*indices)[m], m_Cal3dMesh.m_Indices[m],
				m_Cal3dMesh.m_MaxIndicesCount[m] * sizeof(u16) );
	}
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_getKeyFrameGeometry(
	core::array <vid::S3DVertex1TCoords*> *&vertices,
	core::array <u16*> *indices )
{
	bool external_dest = indices != NULL;

	if (!external_dest)
	{
		vertices = &m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame];
	}
	else
	{
		u32 materialsCount = m_Cal3dMesh.m_Materials.size();
		for ( u32 m = 0; m < materialsCount; m++ )
		{
			memcpy((*vertices)[m],
				m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame][m],
				m_Cal3dMesh.m_MaxVerticesCount[m] * sizeof(vid::S3DVertex1TCoords));
			memcpy((*indices)[m], m_Cal3dMesh.m_Indices[m],
				m_Cal3dMesh.m_MaxIndicesCount[m] * sizeof(u16));
		}
	}
}

//----------------------------------------------------------------------------

IMesh* CAnimatedMeshCal3d::getMesh()
{
	core::array <vid::S3DVertex1TCoords*> *vertices = NULL;
	core::array <u16*> *indices = &m_Cal3dMesh.m_Indices;

	u32 materialsCount = m_Cal3dMesh.m_Materials.size();

	if (!m_Initialized || m_OveralSmooth)
	{
		for (u32 m = 0; m < materialsCount; m++ )
		{
			if (!m_Cal3dMesh.m_Vertices[m])
				m_Cal3dMesh.m_Vertices[m] =
					new vid::S3DVertex1TCoords[m_Cal3dMesh.m_MaxVerticesCount[m]];
			if (!m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame][m])
				m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame][m] =
					new vid::S3DVertex1TCoords[m_Cal3dMesh.m_MaxVerticesCount[m]];
		}

		if (m_OveralSmooth)
			vertices = &m_Cal3dMesh.m_Vertices;
		else
			vertices = &m_Cal3dMesh.m_FrameList[m_Cal3dMesh.m_CurrentFrame];

		_getAnimatedGeometry(vertices);

		if (!m_Initialized)
		{
			for ( u32 m = 0; m < materialsCount; m++ )
			{
				for ( u32 v = 0; v < m_Cal3dMesh.m_VerticesCount[m]; v++ )
				{
					if (v==0)
						m_Cal3dMesh.m_BoxList[m_Cal3dMesh.m_CurrentFrame][m].
							reset( (*vertices)[m][v].Pos );
					else
						m_Cal3dMesh.m_BoxList[m_Cal3dMesh.m_CurrentFrame][m].
							addInternalPoint( (*vertices)[m][v].Pos );
				}
				m_Cal3dMesh.m_BoxList[m_Cal3dMesh.m_CurrentFrame][m].MinEdge.Y = 0;
			}		
		}
	}
	else
	if (m_InterpolateAnimation)
	{
		vertices = &m_Cal3dMesh.m_Vertices;
		_getInterpolatedGeometry(vertices);
	}
	else
	{
		_getKeyFrameGeometry(vertices);
	}

	if (m_Initialized)
	{
		for (u32 m = 0; m < materialsCount; m++)
		{
			SCal3dMeshBuffer<vid::S3DVertex1TCoords>* mb = 
				(SCal3dMeshBuffer<vid::S3DVertex1TCoords>*)m_InterfaceMesh.getMeshBuffer(m);
			mb->setGeom(&m_Cal3dMesh.m_Materials[m], 
				(*vertices)[m], m_Cal3dMesh.m_VerticesCount[m],
				(*indices)[m], m_Cal3dMesh.m_IndicesCount[m]);
		}
	}

	return &m_InterfaceMesh; 
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::uploadToRenderBuffers(IAnimatedMeshSceneNode *node)
{
	if (!m_Initialized)
		return;

	u32 mbcnt = m_Cal3dMesh.m_Materials.size();
	bool need_recreate = false;
	if (mbcnt != node->getRenderBuffersCount())
		need_recreate = true;
	else
	{
		for (u32 i = 0; i < mbcnt; ++i)
		{
			vid::IRenderBuffer *rb = node->getRenderBuffer(i);
			vid::E_DRAW_PRIMITIVE_TYPE dpt = vid::EDPT_TRIANGLE_LIST;
			u32 vcnt = m_Cal3dMesh.m_MaxVerticesCount[i];
			u32 icnt = m_Cal3dMesh.m_MaxIndicesCount[i];
			vid::E_VERTEX_TYPE vtype = m_Cal3dMesh.m_VertexType;
			E_INDEX_TYPE itype = m_Cal3dMesh.m_IndexType;

			if (dpt != rb->getDrawPrimitiveType()
					|| vcnt > rb->getVertices()->getMaxSize()
					|| icnt > rb->getIndices()->getMaxSize()
					|| (vcnt && vtype != rb->getVertices()->getType())
					|| (icnt && itype != rb->getIndices()->getType()))
			{
				need_recreate = true;
				break;
			}
		}
	}
	if (need_recreate)
	{
		node->clearRenderBuffers();
		node->clearMaterials();

		for (u32 i = 0; i < mbcnt; ++i)
		{
			vid::E_DRAW_PRIMITIVE_TYPE dpt = vid::EDPT_TRIANGLE_LIST;
			u32 vcnt = m_Cal3dMesh.m_MaxVerticesCount[i];
			u32 icnt = m_Cal3dMesh.m_MaxIndicesCount[i];
			vid::E_VERTEX_TYPE vtype = m_Cal3dMesh.m_VertexType;
			E_INDEX_TYPE itype = m_Cal3dMesh.m_IndexType;

			node->addMaterial(m_Cal3dMesh.m_Materials[i]);
			node->addRenderBuffer(
				VIDEO_DRIVER.createRenderBuffer(
					getOveralFramesCount() == 1 ? 
						vid::ERBT_STATIC : vid::ERBT_DYNAMIC,
					vtype, vcnt, itype, icnt, dpt));
		}
	}

	// prepare render buffers for uploading data

	core::array <vid::S3DVertex1TCoords*> *locked_v = &m_Cal3dMesh.m_LockedVertices;
	core::array <u16*> *locked_i = &m_Cal3dMesh.m_LockedIndices;

	(*locked_v).set_used(mbcnt);
	(*locked_i).set_used(mbcnt);

	for (u32 m = 0; m < mbcnt; ++m)
	{
		vid::IRenderBuffer * rb = node->getRenderBuffer(m);

		(*locked_v)[m] = (vid::S3DVertex1TCoords*)rb->getVertices()->lock();
		(*locked_i)[m] = (u16*)rb->getIndices()->lock();

		rb->getVertices()->setSize(m_Cal3dMesh.m_MaxVerticesCount[m]);
		rb->getIndices()->setSize(m_Cal3dMesh.m_MaxIndicesCount[m]);
	}

	// upload animated mesh geometry directly into render buffers

	if (m_OveralSmooth)
		_getAnimatedGeometry(locked_v, locked_i);
	else
	if (m_InterpolateAnimation)
		_getInterpolatedGeometry(locked_v, locked_i);
	else
		_getKeyFrameGeometry(locked_v, locked_i);

	// unlocking render buffers, with updated render data

	for (u32 m = 0; m < mbcnt; ++m)
	{
		vid::IRenderBuffer *rb = node->getRenderBuffer(m);
		rb->getVertices()->unlock();
		rb->getIndices()->unlock();
	}

	(*locked_v).set_used(0);
	(*locked_i).set_used(0);
}

//----------------------------------------------------------------------------

IMesh* CAnimatedMeshCal3d::getMesh(f32 frame) 
{
	u32 acnt = m_Animations.size();
	for (u32 a = 0; a < acnt; a++)
	{
		SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
			m_Animations[a]->SpecialAnimation;
		SAnimation *anim = m_Animations[a];
		s32 start = anim->BeginFrame;
		s32 end   = anim->EndFrame;
		if (start <= frame && frame <= end)
		{
			_setCal3dAnimation_BlendCycleAndUpdate(a, 0.0f);	
			f32 delta_sec = 0.f;
			if (end > start)
				delta_sec = (f32(frame - start) / (f32)(end - start)) * 
					spec_anim->Duration;
			spec_anim->Time = delta_sec;
			m_Cal3dMesh.m_CurrentFrame = frame;	
			m_CurrentAnimationIdx = a;
			if (m_Cal3dMesh.m_CurrentFrame == end)
				delta_sec -= 0.05f;
			m_Cal3dModel->update(delta_sec);
			break;
		}
	}
	return getMesh();
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::executeCal3dAction(u32 anim, f32 delay, bool loop)
{
	if (!m_ExecutingAction
			&& anim < (u32)m_Cal3dCoreModel->getCoreAnimationCount()
			&& m_CurrentAnimationIdx < m_Cal3dCoreModel->getCoreAnimationCount())
	{		
		m_currentBlendedAnimationId = (s32)anim;

		std::list <CalAnimationAction*> &alist = 
			m_Cal3dModel->getMixer()->getAnimationActionList();

		if (alist.size()==0)
			m_Cal3dModel->getMixer()->executeAction(
				m_currentBlendedAnimationId, delay, delay, 0.9f, loop
				);

		m_ExecutingAction = true;

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::removeCal3dAction(f32 delay)
{
	if (m_ExecutingAction && m_Cal3dCoreModel->getCoreAnimationCount() > 0
			&& m_CurrentAnimationIdx < m_Cal3dCoreModel->getCoreAnimationCount())
	{		
		m_Cal3dModel->getMixer()->removeAction(m_currentBlendedAnimationId);
		m_ExecutingAction = false;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::getAnimationState(SAnimationState& ani_state)
{
	if (!m_Initialized)
		return false;	

	if ((u32)m_CurrentAnimationIdx < m_Animations.size())
	{
		SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
			m_Animations[m_CurrentAnimationIdx]->SpecialAnimation;
		SAnimation *anim = m_Animations[m_CurrentAnimationIdx];

		ani_state.Animation				= m_CurrentAnimationIdx;
		ani_state.AnimationTime			= spec_anim->Time;
		ani_state.AnimationTimeSec		= getAnimationTimeSec();
		ani_state.AnimationDurationSec	= getAnimationDurationSec();
		ani_state.Looped				= anim->Looping;	
	}
	else
	{
		ani_state.Animation     = 0;
		ani_state.AnimationTime = 0;
		ani_state.AnimationTimeSec     = 0;
		ani_state.AnimationDurationSec = 0;
		ani_state.Looped = false;
	}

	if (!m_OveralSmooth)
		return true;

	s32 boneId=0;

	// reading skeleton bones from cally3d model

	CalSkeleton* pCalSkeleton = m_Cal3dModel->getSkeleton();

	std::vector<CalBone*> &VectorBone = pCalSkeleton->getVectorBone();

	s32 bones_size = m_SkeletonBones.size();

	ani_state.Bones.set_used(bones_size);

	static CalQuaternion q;
	static CalVector v, p[8];
	static CalBone* bone;

	int nrLines = m_Cal3dModel->getSkeleton()->getBoneLines(&m_Cal3dBonesLines[0][0][0]);

	for(boneId=0; boneId<bones_size; ++boneId)
	{
		bone = VectorBone[boneId];
		if (!bone)
			continue;

		q = bone->getRotation();
		v = bone->getTranslation();

		m_SkeletonBones[boneId].Rotation.set   ( q.x, q.y, q.z, q.w );				
		m_SkeletonBones[boneId].Translation.set( v.x, v.y, v.z );	

		m_SkeletonBones[boneId].Line.set(
			m_Cal3dBonesLines[boneId][0][0], m_Cal3dBonesLines[boneId][0][1], m_Cal3dBonesLines[boneId][0][2],
			m_Cal3dBonesLines[boneId][1][0], m_Cal3dBonesLines[boneId][1][1], m_Cal3dBonesLines[boneId][1][2]
			);

		ani_state.Bones[boneId].Rotation    = m_SkeletonBones[boneId].Rotation;
		ani_state.Bones[boneId].Translation = m_SkeletonBones[boneId].Translation;
		ani_state.Bones[boneId].Line        = m_SkeletonBones[boneId].Line;

		ani_state.Bones[boneId].BBox.reset(m_SkeletonBones[boneId].Line.start);
		ani_state.Bones[boneId].BBox.addInternalPoint(m_SkeletonBones[boneId].Line.end);
	}

	return true;
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::updateMesh(s32 delta_ms)
{
	if ((u32)m_CurrentAnimationIdx >= m_Animations.size())
		return;

	// updating cal3d model

	SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
		m_Animations[m_CurrentAnimationIdx]->SpecialAnimation;
	SAnimation *anim = m_Animations[m_CurrentAnimationIdx];

	f32 delta_sec = f32(delta_ms) * 0.001f * m_OveralSpeed * m_CurrentAnimationSpeed;	
	f32 anim_time = spec_anim->Time + delta_sec;
	bool end_anim = false, on_end_anim = false;

	f32 dur = spec_anim->Duration;
	if (!anim->Looping)
	{
		// XXX This is a bad desision, but don't know how to turn off
		// the interpolation last->first animtion frame for Cal3d mesh
		//  [frame0] ... [frameN]
		//   ^__interpolation__|
		dur *= 0.98f;
	}

	if (anim_time >= dur)
	{
		if (anim->Looping)
		{
			while (anim_time >= dur)
				anim_time -= dur;
			delta_sec = anim_time;
			if (spec_anim->Time < dur)
				on_end_anim = true;
			spec_anim->Time = 0.f;
		}
		else
		{
			anim_time = dur;
			if (spec_anim->Time < anim_time)
				delta_sec = anim_time - spec_anim->Time;
			else
			{
				delta_sec = 0.f;
				spec_anim->Time = dur;
			}
			if (spec_anim->Time < dur)
				on_end_anim = true;
		}
		end_anim = true;
	}
	spec_anim->Time += delta_sec;

	if (end_anim && !anim->Looping)
	{
		m_Cal3dMesh.m_CurrentFrame = anim->EndFrame;
	}
	else
		m_Cal3dMesh.m_CurrentFrame = (f32)anim->BeginFrame +
			f32(anim->EndFrame - anim->BeginFrame) * spec_anim->Time / spec_anim->Duration;

	if (m_OveralSmooth)
	{
		if (delta_sec > 0.f)
			m_Cal3dModel->update(delta_sec);
	}

	_updateBoundingBox();

	if (on_end_anim)
		OnAnimationEnd(m_CurrentAnimationIdx);
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::setAnimationState(const SAnimationState &ani_state)
{
	if (!m_Initialized || (u32)ani_state.Animation >= m_Animations.size())
		return false;	

	SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
		m_Animations[ani_state.Animation]->SpecialAnimation;

	if (m_CurrentAnimationIdx != ani_state.Animation
			|| spec_anim->Time != ani_state.AnimationTime)
	{
		if (!setCurrentAnimation(ani_state.Animation, 0.0f))
			return false;
		spec_anim->Time = ani_state.AnimationTime;
		if (m_OveralSmooth)
			m_Cal3dModel->getMixer()->setAnimationTime(ani_state.AnimationTime);
	}

	if (!m_OveralSmooth)
		return true;

	CalSkeleton* pCalSkeleton = m_Cal3dModel->getSkeleton();
	std::vector<CalBone*> &VectorBone = pCalSkeleton->getVectorBone();
	CalQuaternion q;
	CalVector v;
	CalBone* bone;

	for (u32 boneId = 0; boneId < VectorBone.size() &&
			boneId < ani_state.Bones.size(); ++boneId )
	{
		bone = VectorBone[boneId];
		if (!bone)
			continue;	
		const SSkeletonBone &skelbone = ani_state.Bones[boneId];
		q = CalQuaternion(
			skelbone.Rotation.X, skelbone.Rotation.Y, skelbone.Rotation.Z, skelbone.Rotation.W);
		v = CalVector(
			skelbone.Translation.X, skelbone.Translation.Y, skelbone.Translation.Z);
		bone->setRotation(q);
		bone->setTranslation(v);		
	}
	return true;
}

//----------------------------------------------------------------------------

const core::array<SSkeletonBone>& CAnimatedMeshCal3d::getBones()
{
	return m_SkeletonBones;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::mapBone(s32 bone_idx)
{
	if (bone_idx < 0
			|| bone_idx >= (s32)m_MappedBoneTransformations.size()
			|| bone_idx >= (s32)m_SkeletonBones.size())
		return false;

	if (m_MappedBoneTransformations[bone_idx].size() == 0)
	{
		m_MappedBoneTransformations[bone_idx].set_used(m_Cal3dMesh.m_OveralFramesCount);

		for ( u32 fr = 0; fr < m_Cal3dMesh.m_OveralFramesCount; fr++ )
		{
			getMesh(fr);

			// reading skeleton bones from cally3d model

			CalSkeleton* pCalSkeleton = m_Cal3dModel->getSkeleton();
			CalCoreSkeleton* pCalCoreSkeleton = m_Cal3dModel->getSkeleton()->getCoreSkeleton();

			std::vector<CalBone*>     &VectorBone     = pCalSkeleton->getVectorBone();
			std::vector<CalCoreBone*> &VectorCoreBone = pCalCoreSkeleton->getVectorCoreBone();

			static CalQuaternion q;
			static CalVector v, p[8];
			static core::matrix4 transformation;
			static core::vector3df  mov;
			static core::quaternion rot;
			
			CalBone*     bone     = VectorBone    [bone_idx];
			CalCoreBone* corebone = VectorCoreBone[bone_idx];

			if (!bone || !corebone)
				return false;

			q = bone->getRotation();
			v = bone->getTranslation();

			rot.set( q.x, q.y, q.z, q.w );				
			mov.set( v.x, v.y, v.z );					

			transformation = rot.getMatrix();	
			transformation.setTranslation(mov);	

			m_MappedBoneTransformations[bone_idx][fr] = transformation;
			
			s32 parent_bone_id = corebone->getParentId();

			while (parent_bone_id!=-1)
			{
				bone     = VectorBone    [parent_bone_id];
				corebone = VectorCoreBone[parent_bone_id];

				if (!bone || !corebone)
					break;

				q = bone->getRotation();
				v = bone->getTranslation();

				rot.set( q.x, q.y, q.z, q.w );				
				mov.set( v.x, v.y, v.z );					

				transformation = rot.getMatrix();	
				transformation.setTranslation(mov);	

				m_MappedBoneTransformations[bone_idx][fr] = 
					transformation * m_MappedBoneTransformations[bone_idx][fr];

				parent_bone_id = corebone->getParentId();
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------

core::matrix4 CAnimatedMeshCal3d::getMapedBoneTransformation(s32 bone_idx)
{ return m_MappedBoneTransformations[bone_idx][m_Cal3dMesh.m_CurrentFrame]; }

//----------------------------------------------------------------------------

f32 CAnimatedMeshCal3d::getAnimationTimeSec()
{
	if ((u32)m_CurrentAnimationIdx >= m_Animations.size())
		return 0.f;
	SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
		m_Animations[m_CurrentAnimationIdx]->SpecialAnimation;
	f32 spd = (m_OveralSpeed * m_CurrentAnimationSpeed);
	if (spd < 0.001f) spd = 0.001f;
	return spec_anim->Time * (1.f / spd);
}

//----------------------------------------------------------------------------

f32 CAnimatedMeshCal3d::getAnimationDurationSec()
{
	if ((u32)m_CurrentAnimationIdx >= m_Animations.size())
		return 0.f;
	SCal3dSpecialAnimation *spec_anim = (SCal3dSpecialAnimation*)
		m_Animations[m_CurrentAnimationIdx]->SpecialAnimation;
	f32 dur = spec_anim->Duration;
	if (dur < 0.075f) dur = 0.0f;
	f32 spd = (m_OveralSpeed * m_CurrentAnimationSpeed);
	if (spd < 0.001f) spd = 0.001f;
	return dur * (1.f / spd);
}

//----------------------------------------------------------------------------

s32 CAnimatedMeshCal3d::getAnimationFPS() 
{  return m_KeyFrameFPS; }

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::setAnimationFPS(s32 fps)
{
	if (!m_Initialized)
		m_KeyFrameFPS = fps;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::setCurrentAnimation(u32 idx, f32 delay)
{ 
	if (m_OveralSmooth)
		return _setCal3dAnimation_BlendCycleAndUpdate(idx, delay);
	return _setCal3dAnimation(idx);
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::_setCal3dAnimation(u32 idx)
{
	if (idx >= m_Animations.size()
			|| !m_Animations[idx]->SpecialAnimation
			|| ((SCal3dSpecialAnimation*)m_Animations[idx]->SpecialAnimation)->AnimationId == -1)
		return false;
	m_CurrentAnimationIdx = idx;
	m_CurrentAnimationSpeed = m_Animations[idx]->Speed;
	((SCal3dSpecialAnimation*)m_Animations[idx]->SpecialAnimation)->Time = 0.0f;
	OnAnimationBegin(idx);
	return true;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::_setCal3dAnimation_BlendCycleAndUpdate(u32 idx, f32 delay)
{
	if (!_setCal3dAnimation(idx))
		return false;
	SAnimation *anim = m_Animations[idx];
	SCal3dSpecialAnimation *spec_anim = ((SCal3dSpecialAnimation*)m_Animations[idx]->SpecialAnimation);
	s32 id = spec_anim->AnimationId;
	CalMixer *mixer = m_Cal3dModel->getMixer();

	std::vector<CalAnimation*> anims = mixer->getAnimationVector();
	u32 animcnt = (u32)anims.size();
	for (u32 i = 0; i < animcnt; i++)
	{
		CalAnimation *anim = anims[i];
		if (anim && anim->getType() == CalAnimation::TYPE_CYCLE)
			mixer->clearCycle(i, delay);
	}
	mixer->blendCycle(id, 1.0f, delay);

	m_Cal3dModel->getMixer()->setAnimationTime(0.0f);   	
	m_Cal3dModel->update(0.0f);
	return true;
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::startRegisteringAnimations() {}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::registerAnimation(const c8 *filename, f32 speed, bool looped)
{
	m_Initialized = false;
	return IAnimatedMeshCal3d::registerAnimation(filename, speed, looped);
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::unregisterAnimation(u32 idx)
{
	u32 cal_animcnt = m_Cal3dCoreModel->getCoreAnimationCount();
	if (idx < cal_animcnt)
		_clearCal3dAnimation(idx);
	bool res = IAnimatedMeshCal3d::unregisterAnimation(idx);
	if (!getAnimationsCount())
		m_Initialized = false;
	return res;
}

//----------------------------------------------------------------------------

bool CAnimatedMeshCal3d::swapAnimations(u32 idx1, u32 idx2)
{
	return IAnimatedMeshCal3d::swapAnimations(idx1, idx2);
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::endRegisteringAnimations()
{
	if (m_Initialized)
		return;

	_clearCal3dAnimations();

	// registering animations
	std::string fn = "";
	for (u32 i = 0; i < m_Animations.size(); i++)
	{
		const c8 *filename = m_Animations[i]->FileName.c_str();
		if (filename)
		{
			LOGGER.logInfo(" Registering animation %d from %s . . .",
				i , filename);
			fn = filename;
			s32 id = m_Cal3dCoreModel->loadCoreAnimation(fn);
			SAFE_DELETE(m_Animations[i]->SpecialAnimation);
			m_Animations[i]->SpecialAnimation = new SCal3dSpecialAnimation(id);
		}
	}

	// initialize Cal3d mesh

	SAFE_DELETE(m_Cal3dModel);

	// create the model instance from the loaded core model

	m_Cal3dModel = new CalModel(m_Cal3dCoreModel);

    // attach all meshes to the model
	int meshId;
	for(meshId = 0; meshId < m_Cal3dCoreModel->getCoreMeshCount(); meshId++)
		m_Cal3dModel->attachMesh(meshId);
	// set the material set of the whole model
	m_Cal3dModel->setMaterialSet(0);

	// calculating frames count

	setCurrentAnimation(0);	

	LOGGER.logInfo(" Animations list:");
	m_Cal3dMesh.m_OveralFramesCount = 0;

	u32 acnt = m_Animations.size();
	for (u32 a = 0; a < acnt; a++)
	{
		_setCal3dAnimation_BlendCycleAndUpdate(a, 0.0f);

		f32 duration = m_Cal3dModel->getMixer()->getAnimationDuration();
		f32 spd = (m_OveralSpeed * m_CurrentAnimationSpeed);
		if (spd < 0.001f) spd = 0.001f;
		f32 real_duration = duration * (1.0f/spd);
		s32 frames = real_duration*m_KeyFrameFPS + 1;

		SCal3dSpecialAnimation *spec_anim =
			((SCal3dSpecialAnimation*)m_Animations[a]->SpecialAnimation);
		SAnimation *anim = m_Animations[a];

		anim->BeginFrame = m_Cal3dMesh.m_OveralFramesCount;
		anim->EndFrame   = m_Cal3dMesh.m_OveralFramesCount + (frames ? (frames-1) : 0);
		spec_anim->Duration   = duration;
		spec_anim->Time       = 0.0f;

		m_Cal3dMesh.m_OveralFramesCount += frames;

		LOGGER.logInfo("  %d - %.2f secs, %d frames (start %d, end %d)", 
			a, real_duration, frames, anim->BeginFrame, anim->EndFrame);
	}	
	LOGGER.logInfo(" Overal frames count %d", m_Cal3dMesh.m_OveralFramesCount);

	u32 materialsCount = m_Cal3dMesh.m_Materials.size();
	
	// initializing arrays for further holding geometry

	m_IndicesOK = false;

	m_Cal3dMesh.m_CurrentFrame = 0;

	m_Cal3dMesh.freeData();

	m_Cal3dMesh.m_Vertices.set_used(materialsCount);
	memset(m_Cal3dMesh.m_Vertices.pointer(), 0, materialsCount * sizeof(void*));
	m_Cal3dMesh.m_Indices.set_used(materialsCount);
	memset(m_Cal3dMesh.m_Indices.pointer(), 0, materialsCount * sizeof(void*));
	
	m_Cal3dMesh.m_VerticesCount.set_used(materialsCount);
	memset(m_Cal3dMesh.m_VerticesCount.pointer(), 0, sizeof(u32)*materialsCount);
	m_Cal3dMesh.m_IndicesCount.set_used(materialsCount);
	memset(m_Cal3dMesh.m_IndicesCount.pointer(), 0, sizeof(u32)*materialsCount);

	m_Cal3dMesh.m_MaxVerticesCount.set_used(materialsCount);
	memset(m_Cal3dMesh.m_MaxVerticesCount.pointer(), 0, sizeof(u32)*materialsCount);
	m_Cal3dMesh.m_MaxIndicesCount.set_used(materialsCount);
	memset(m_Cal3dMesh.m_MaxIndicesCount.pointer(), 0, sizeof(u32)*materialsCount);

	m_Cal3dMesh.m_FrameList.set_used(m_Cal3dMesh.m_OveralFramesCount + 1);
	m_Cal3dMesh.m_BoxList.set_used(m_Cal3dMesh.m_OveralFramesCount + 1);
	for ( u32 f = 0; f <= m_Cal3dMesh.m_OveralFramesCount; f++ )
	{
		m_Cal3dMesh.m_FrameList[f].set_used(materialsCount);
		memset(m_Cal3dMesh.m_FrameList[f].pointer(), 0, materialsCount * sizeof(void*));
		m_Cal3dMesh.m_BoxList[f].set_used(materialsCount);
	}

	// calculating mesh parameters

	LOGGER.log(" Meshes list:");

    // set Level Of Detail
	m_Cal3dModel->setLodLevel(1);
	m_Cal3dModel->update(0);

	CalRenderer *pCalRenderer = m_Cal3dModel->getRenderer();

	u32 total_mb_cnt = 0;
	s32 meshCount = pCalRenderer->getMeshCount();
	for (s32 meshId = 0; meshId < meshCount; meshId++)
	{		
		s32 submeshCount = pCalRenderer->getSubmeshCount(meshId);
		for(s32 submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			CalMesh * cal_mesh = m_Cal3dModel->getMesh(meshId);
			if ( pCalRenderer->selectMeshSubmesh(meshId, submeshId) )
			{
				CalSubmesh *cal_submesh = cal_mesh->getSubmesh(submeshId);
				s32 cal_mat_id   = cal_submesh->getCoreMaterialId();

				CalCoreMaterial *pCoreMaterial = m_Cal3dCoreModel->getCoreMaterial(cal_mat_id);

				s32 mat_id = (s32)(size_t)pCoreMaterial->getUserData();

				s32 v_cnt = pCalRenderer->getVertexCount();
				s32 f_cnt = pCalRenderer->getFaceCount();

				m_Cal3dMesh.m_MaxVerticesCount[mat_id] += v_cnt;
				m_Cal3dMesh.m_MaxIndicesCount[mat_id] += f_cnt * 3;

				m_OveralVertexCount += v_cnt;
				m_OveralFaceCount   += f_cnt; 

				LOGGER.logInfo("  %d - %d faces, %d verts", total_mb_cnt++, f_cnt, v_cnt);	
			}
		}
	}	

	LOGGER.logInfo(" Total mesh buffers count %d", total_mb_cnt);
	LOGGER.logInfo(" Downsampled mesh buffers count %d", materialsCount);
	
	m_InterfaceMesh.removeMeshBuffers();
    for ( u32 m = 0; m < materialsCount; ++m)
	{
		m_Cal3dMesh.m_Indices[m] = new u16[m_Cal3dMesh.m_MaxIndicesCount[m]];
		IMeshBuffer *mb = new SCal3dMeshBuffer <vid::S3DVertex1TCoords> ();
		m_InterfaceMesh.addMeshBuffer(mb);
		mb->drop();	
	}

	// caching all animation frames
	for (s32 f = m_Cal3dMesh.m_OveralFramesCount; f >= 0; f--)
		getMesh(f);

	m_Initialized = true;

	_updateBoundingBox();

    LOGGER.logInfo("Cally3D model initialized (%d faces).", m_OveralFaceCount);
}

//----------------------------------------------------------------------------

void CAnimatedMeshCal3d::_updateBoundingBox()
{
	u32 materialsCount = m_Cal3dMesh.m_Materials.size();
	for (u32 m = 0; m < materialsCount; m++)
	{
		SCal3dMeshBuffer<vid::S3DVertex1TCoords>* mb = 
			(SCal3dMeshBuffer<vid::S3DVertex1TCoords>*)m_InterfaceMesh.getMeshBuffer(m);
		mb->setBB(
			&m_Cal3dMesh.m_BoxList[m_Cal3dMesh.m_CurrentFrame][m]);
	}
	recalculateBoundingBox();
}

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace my
//----------------------------------------------------------------------------
