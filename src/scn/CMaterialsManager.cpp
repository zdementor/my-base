//|-------------------------------------------------------------------------
//| File:        CMaterialsManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMaterialsManager.h"

#include <os/ITimer.h>
#include <io/ILogger.h>
#include <io/IFileSystem.h>
#include <io/IFileList.h>
#include <vid/SMaterial.h>
#include <vid/IVideoDriver.h>
#include <vid/ITextureAnim.h>
#include <res/IResourceManager.h>
#include <game/IGameManager.h>
#include <img/IImageLibrary.h>
#include <dev/IDevice.h>
#include <io/IWriteFile.h>
#include <io/IXMLReader.h>
#include <scn/ISceneManager.h>

#include "CQ3ShaderFileParser.h"
#include "CQ3ShaderParser.h"
#include "CQ3LevelMesh.h"
#include "CMaterialFileParser.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CMaterialsManager::CMaterialsManager()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CMaterialsManager");
#endif
}

//---------------------------------------------------------------------------

CMaterialsManager::~CMaterialsManager()
{
	Materials.clear();
	m_MaterialsHash.clear();
}

//---------------------------------------------------------------------------

void CMaterialsManager::loadMaterialsFromDir(
	const c8* dir, bool load_textures, bool reload_if_exists)
{
	c8 ch[255];
	
	LOGGER.logInfo("Loading materials from '%s' . . .", dir);

	io::IFileSystem &fs = FILE_SYSTEM;

	// store initial work dir
	core::stringc old_work_dir = fs.getWorkingDirectory();

	fs.changeWorkingDirectoryTo(dir);

	const io::IFileList *flist = fs.getFileList();

	LOGGER.increaseFormatLevel();

	u32 shfile_cnt = 0;
	u32 shfile_ovr_cnt = 0;

	for (s32 f=0; f<flist->getFileCount(); f++)
	{
		if (flist->isDirectory(f))
			continue;

		core::stringc fname = flist->getFileName(f);
		core::stringc fext;
		fext = core::extractFileExt( fname );
		fext.make_lower();

		if (strstr(fext.c_str(), "shader") || strstr(fext.c_str(), "material"))
			shfile_ovr_cnt++;
	}

	u32 loaded = 0, reloaded = 0;
	u32 errors = 0;

	if (!getMaterialsCount())
		reload_if_exists = false;
		
	for (s32 f=0; f<flist->getFileCount(); f++)
	{
		if (flist->isDirectory(f))
			continue;

		core::stringc fname = flist->getFileName(f);
		core::stringc fext;
		fext = core::extractFileExt(fname);
		fext.make_lower();	

		sprintf(ch, "Materials from file (%d/%d)\n%s",
			shfile_cnt+1, shfile_ovr_cnt, fname.c_str());
		GAME_MANAGER.showMessageBox(
			true, "Please wait, while loading", ch);	

		if (strstr(fext.c_str(), "shader"))
		{
			CQ3ShaderFileParser::loadShaders(flist->getFullFileName(f), Materials,
				load_textures, reload_if_exists, loaded, reloaded, errors);			
		}
		else
		if (strstr(fext.c_str(), "material"))
		{
			CMaterialFileParser::loadMaterials(flist->getFullFileName(f), Materials,
				load_textures, reload_if_exists, loaded, reloaded, errors);
		}
		else
			continue;		

		shfile_cnt++;
	}

	u32 nmapCnt = 0;
	f32 startSec = TIMER.getSystemTimeSec();

	LOGGER.logInfo("Generating normal maps . . .");

	LOGGER.increaseFormatLevel();

	bool use_tbn = DEVICE.getDeviceFlagValue(dev::EDCF_SHADERS_HIGH_QUALITY);

	for (u32 m=0; m < Materials.size(); m++)
	{
		for (u32 p = 0; p < Materials[m].Material.getPassesCount(); p++)
		{
			vid::SRenderPass &pass = Materials[m].Material.getPass(p);
			vid::ITexture *t0 = pass.Layers[0].getTexture();
			if (t0 && t0->getType() == vid::ETT_COLOR_MAP &&
					pass.Layers[0].getType() == vid::ETLT_DIFFUSE_MAP &&
					!pass.Layers[3].getTexture())
			{
				if (use_tbn)
				{				
					const c8 *filename = VIDEO_DRIVER.findTextureName(t0);
					vid::ITexture *nmap = getNormalMapTexture(filename);
					if (nmap)
					{
						nmapCnt++;
						pass.Layers[3].setTexture(nmap, vid::ETLT_NORMAL_MAP);
					}
				}
			}
		}		
	}

	LOGGER.decreaseFormatLevel();

	LOGGER.logInfo(" %d normal maps loaded (elapsed %.3f secs)",
		nmapCnt, TIMER.getSystemTimeSec() - startSec);

	LOGGER.decreaseFormatLevel();

	// updating materials hash
	_updateMaterialsHash();

	// restore initial work dir
	fs.changeWorkingDirectoryTo(old_work_dir.c_str());	

	if (errors == 0)
		LOGGER.logInfo(" %d files loaded (materials %d loaded, %d reloaded)",
			shfile_cnt, loaded, reloaded);
	else
		LOGGER.logWarn(" %d files loaded (materials %d loaded, %d reloaded), with %d errors",
			shfile_cnt, loaded, reloaded, errors);

	LOGGER.logInfo("Materials at all %d", getMaterialsCount());

	GAME_MANAGER.showMessageBox(false);
}

//---------------------------------------------------------------------------

bool CMaterialsManager::loadMaterialFromFile(const c8 *filename, vid::SMaterial &out_material)
{
	u32 errors = 0;
	core::stringc fext = core::extractFileExt(filename);
	fext.make_lower();	

	if (strstr(fext.c_str(), "material"))
	{
		u32 loaded = 0, reloaded = 0; 
		core::array <SMeshBufferMaterial> marr;
		CMaterialFileParser::loadMaterials(filename, marr,
			true, false, loaded, reloaded, errors);			
		if (!errors && loaded > 0)
			out_material = marr[0].Material;
	}
	else
		errors++;

	return (errors == 0);
}

//---------------------------------------------------------------------------

bool CMaterialsManager::saveMaterialToFile(const c8 *filename, const vid::SMaterial &material)
{
	io::IWriteFile *outFile = FILE_SYSTEM.createAndWriteFile(filename);
	if (!outFile)
		return false;

	const c8 header[] = "<?xml version=\"1.0\"?>";
	outFile->write(header, sizeof(header) - 1);
	outFile->write("\n", 1);

	const c8 *fname = core::extractFileName(filename);

	core::stringc buf = "";
	CMaterialFileParser::getMaterialXMLTextTo(buf, material, true, fname);
	outFile->write(buf.c_str(), buf.size());

	outFile->drop();

	return true;
}

//---------------------------------------------------------------------------

bool CMaterialsManager::getMaterialAndVertexType(
	const c8* material_name, vid::ITexture* lmap, 
	vid::SMaterial &out_material, vid::E_VERTEX_TYPE &vtype
	)
{
	// try to fing already loaded material

	s32 idx = _findMaterial(material_name);

	if (idx != -1)
	{
		vid::ITexture* white_tex = CQ3LevelMesh::getQ3WhiteImage();

		out_material = Materials[idx].Material;
		vtype        = Materials[idx].VertexType;

		// setting lightmap (if needed)

		if (lmap)
		{
			for (u32 p=0; p < out_material.getPassesCount(); p++)
			{
				for (u32 l=0; l<vid::MY_MATERIAL_MAX_LAYERS; l++)
				{
					if (out_material.getPass(p).Layers[l].getType() == vid::ETLT_LIGHT_MAP &&
						out_material.getPass(p).Layers[l].getTexture()==white_tex)
						out_material.getPass(p).Layers[l].setTexture(lmap, vid::ETLT_LIGHT_MAP, 1);			
				}				
			}
		}

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------

u32 CMaterialsManager::getMaterialsCount() const
{
	return Materials.size();
}

//---------------------------------------------------------------------------

s32 CMaterialsManager::findMaterialByName(const c8* name) const
{
	return _findMaterial(name);
}

//---------------------------------------------------------------------------

s32 CMaterialsManager::findMaterialByHash(u64 hash) const
{
	s32 idx  = m_MaterialsHash.find(hash, hash);
	if (idx != -1)
		idx = m_MaterialsHash(hash, idx).Index;
	return idx;
}

//---------------------------------------------------------------------------

s32 CMaterialsManager::addMaterial(const vid::SMaterial &m, const c8 *name)
{
	u64 hash = m.getHash();
	if (findMaterialByHash(hash) != -1)
		return -1;
	if (findMaterialByName(name) != -1)
		return -1;
	Materials.push_back(SMeshBufferMaterial(m, (vid::E_VERTEX_TYPE)-1, name));
	_updateMaterialsHash();
	return findMaterialByHash(hash);
}

//---------------------------------------------------------------------------

const vid::SMaterial& CMaterialsManager::getMaterial(u32 idx) const 
{
	if (idx < Materials.size())
		return Materials[idx].Material;
	return m_StubMaterial;
}

//---------------------------------------------------------------------------

const vid::SMaterial& CMaterialsManager::getMaterialByName(const c8 *name) const 
{
	u32 idx = (u32)_findMaterial(name);
	if (idx < Materials.size())
		return Materials[idx].Material;
	else
		LOGGER.logWarn("There is no any material with name '%s'", name);
	return m_StubMaterial;
}

//---------------------------------------------------------------------------

bool CMaterialsManager::setMaterial(u32 idx, const vid::SMaterial &other)
{
	if (idx >= Materials.size())
		return false;
	vid::SMaterial &mat = Materials[idx].Material;
	u64 hash = mat.getHash();
	if (hash != other.getHash())
	{
		s32 hashidx = m_MaterialsHash.find(hash, hash);
		if (hashidx != -1)
			m_MaterialsHash.erase(hash, hashidx);
		mat = other;
		hash = mat.getHash();
		m_MaterialsHash.push_back(hash,
			SMeshBufferMaterialHashEntry(&Materials[idx], (s32)idx));
	}
	return true;
}

//---------------------------------------------------------------------------

bool CMaterialsManager::setMaterialByName(const c8 *name, const vid::SMaterial &other)
{
	u32 idx = (u32)_findMaterial(name);
	if (!setMaterial(idx, other))
	{
		LOGGER.logWarn("There is no any material with name '%s'", name);
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

const c8* CMaterialsManager::getMaterialName(u32 idx) const
{
	if (idx < Materials.size())
		return Materials[idx].Name.c_str();
	return NULL;
}

//---------------------------------------------------------------------------

void CMaterialsManager::correctMaterial(vid::SMaterial &material)
{
	for (u32 p = 0; p < material.getPassesCount(); p++)
	{
		vid::SRenderPass &pass = material.getPass(p);

		s32 last_active_stage = 0;

		for (s32 l=0; l<vid::MY_MATERIAL_MAX_LAYERS; l++)
		{
			if (pass.Layers[l].getTexture())
				last_active_stage = l;
		}

		for ( s32 l = 0; l <= last_active_stage; l++ )
		{
			if (!pass.Layers[l].getTexture())
			{
				pass.Layers[l].setTexture(CQ3LevelMesh::getQ3WhiteImage(),
					pass.Layers[l].getType(), pass.Layers[l].getTexCoordChannel());
			}
		}
	}
}

//---------------------------------------------------------------------------

void CMaterialsManager::_updateMaterialsHash()
{
	Materials.sort();
	m_MaterialsHash.clear();
	for (u32 m = 0; m < Materials.size(); m++)
	{
		u64 hash = Materials[m].Material.getHash();
		m_MaterialsHash.push_back(hash,
			SMeshBufferMaterialHashEntry(&Materials[m], (s32)m));
	}
}

//---------------------------------------------------------------------------

s32 CMaterialsManager::_findMaterial(const c8* material_name) const
{
	core::stringc name = material_name;

	SMeshBufferMaterial m;
	m.Name = material_name;
	m.Name.make_lower();

	return Materials.binary_search(m);
}

//---------------------------------------------------------------------------

vid::ITexture* getNormalMapTexture(const c8 *name)
{
	s32 e=0;
	core::stringc s;

	const s32 NRM_MAP_EXT_CNT = 11;
	const c8 *q3_tex_ext[NRM_MAP_EXT_CNT] =
	{ 
		"_nmap.tga", 		
		"_0_hmap.tga",
		"_1_hmap.tga",
		"_2_hmap.tga",
		"_3_hmap.tga",
		"_4_hmap.tga",
		"_5_hmap.tga",
		"_6_hmap.tga",
		"_7_hmap.tga",
		"_8_hmap.tga",
		"_9_hmap.tga"
	};	
	
	// try to find in temp dir

	const c8 *tmp_dir = 
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEMP_DATA );

	core::stringc fpath = core::extractFilePath(name);
	core::stringc fname = core::extractFileName(name);

	s32 p = fname.find_last('.');
	if (p >= 0)
		fname.set_end_of_line(p);

	s.sprintf("%s%s%s", tmp_dir, fname.c_str(), q3_tex_ext[e]);

	vid::ITexture* tex = VIDEO_DRIVER.findTexture(s.c_str());

	if (tex)
		return tex;

    if (FILE_SYSTEM.existFileOnDisk(s.c_str()))
    {
		tex = VIDEO_DRIVER.getTexture(s.c_str());
		return tex;
    }

	// try to find in zip packages

	core::stringc fulltexname = fpath;
	fulltexname.append(fname);

	for (e = 0; e < NRM_MAP_EXT_CNT; ++e)
	{
		s.sprintf("%s%s", fulltexname.c_str(), q3_tex_ext[e]);

		if (!FILE_SYSTEM.existFileInZip(s.c_str()))
			continue;

		vid::ITexture *tex = VIDEO_DRIVER.findTexture(s.c_str());
		if (tex)
			return tex;

		if (e>0)
		{
			VIDEO_DRIVER.setTextureCreationFlag(
				vid::ETCF_CREATE_NORMAL_MAP, true);				
			VIDEO_DRIVER.setNormalMapCreationAmplitude(e-1);
		}

		tex = VIDEO_DRIVER.getTexture(s.c_str());

		if (e>0)
			VIDEO_DRIVER.setTextureCreationFlag(
				vid::ETCF_CREATE_NORMAL_MAP, false);

		return tex;
    }

	// try to find in default media dir

	fulltexname = RESOURCE_MANAGER.getMediaDirFull(res::EMT_ROOT);
	fulltexname.append(fpath);	
	fulltexname.append(fname);

	for (e=0; e < NRM_MAP_EXT_CNT; ++e)
	{
		s.sprintf("%s%s", fulltexname.c_str(), q3_tex_ext[e]);

		if (!FILE_SYSTEM.existFileOnDisk(s.c_str()))
			continue;

		vid::ITexture *tex = VIDEO_DRIVER.findTexture(s.c_str());
		if (tex)
			return tex;

		img::IImage* image = IMAGE_LIBRARY.getImage(s.c_str());

		if (e>0)
		{
			VIDEO_DRIVER.setTextureCreationFlag(
				vid::ETCF_CREATE_NORMAL_MAP, true);
			VIDEO_DRIVER.setNormalMapCreationAmplitude(e-1);
		}

		tex = VIDEO_DRIVER.getTexture(s.c_str());

		if (e>0)
			VIDEO_DRIVER.setTextureCreationFlag(
				vid::ETCF_CREATE_NORMAL_MAP, false);

		return tex;
    }	

	return 0;
}

//---------------------------------------------------------------------------

IMaterialsManager* createMaterialsManager()
{
	return new CMaterialsManager();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
