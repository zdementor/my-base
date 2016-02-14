//|-------------------------------------------------------------------------
//| File:        CResourceManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CResourceManager.h"

#include <scn/ISceneManager.h>
#include <game/IGameManager.h>
#include <scr/IScriptManager.h>
#include <dev/IDevice.h>
#include <io/IFileSystem.h>

//----------------------------------------------------------------------------
namespace my {
namespace res {
//----------------------------------------------------------------------------

CResourceManager::CResourceManager()  
{
#if MY_DEBUG_MODE 
    setClassName("CResourceManager");
#endif
}

//----------------------------------------------------------------------------

CResourceManager::~CResourceManager() {}

//----------------------------------------------------------------------------

void CResourceManager::setMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir)
{
	m_MediaDirRel[mt] = dir;
	m_MediaDirFull[mt] = "";

	if ( mt == EMT_ROOT )
	{
		SCRIPT_MANAGER.setScriptsFullDir(
			getMediaDirFull(EMT_GAME_SCRIPTS));
		SCENE_MANAGER.setTexturesPath(
			getMediaDirFull(EMT_TEXTURES ) );
	}
	else
	if (mt == EMT_GAME_SCRIPTS)
		SCRIPT_MANAGER.setScriptsFullDir(
			getMediaDirFull(mt));
	else
	if (mt == EMT_TEXTURES)
		SCENE_MANAGER.setTexturesPath(
			getMediaDirFull(mt));
}

//----------------------------------------------------------------------------

void CResourceManager::setCommonMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir)
{
	m_CommonMediaDirRel[mt] = dir;
	m_CommonMediaDirFull[mt] = "";
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getMediaDirRel(E_MEDIA_TYPE mt)
{ return m_MediaDirRel[mt].c_str(); }

//----------------------------------------------------------------------------

const c8* CResourceManager::getCommonMediaDirRel(E_MEDIA_TYPE mt)
{ return m_CommonMediaDirRel[mt].c_str(); }

//----------------------------------------------------------------------------

const c8* CResourceManager::getMediaDirFull(E_MEDIA_TYPE mt)
{
	if (m_MediaDirRel[mt].size() > 0)
	{
		if (m_MediaDirFull[mt].size() == 0)
		{
			io::IFileSystem &fsys = FILE_SYSTEM;
			core::stringc tmp_str;
			if (mt == EMT_ROOT)
				tmp_str = DEVICE.getStartupDir() +
					"/" + m_MediaDirRel[mt];
			else
				tmp_str = core::stringc(getMediaDirFull(EMT_ROOT)) +
					m_MediaDirRel[mt];
			if (!fsys.existDir(tmp_str.c_str()))
				fsys.makeDir(tmp_str.c_str());
			m_MediaDirFull[mt] = fsys.optimizeDirectoryString(tmp_str.c_str());
		}
	}
	return m_MediaDirFull[mt].c_str();
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getCommonMediaDirFull(E_MEDIA_TYPE mt)
{
	if (m_CommonMediaDirRel[mt].size() > 0)
	{
		if (m_CommonMediaDirFull[mt].size() == 0)
		{
			io::IFileSystem &fsys = FILE_SYSTEM;
			core::stringc tmp_str;
			if (mt == EMT_ROOT)
				tmp_str = DEVICE.getStartupDir() +
					"/" + m_CommonMediaDirRel[mt];
			else
				tmp_str = core::stringc(getCommonMediaDirFull(EMT_ROOT)) +
					m_CommonMediaDirRel[mt];
			if (!fsys.existDir(tmp_str.c_str()))
				fsys.makeDir(tmp_str.c_str());
			m_CommonMediaDirFull[mt] = fsys.optimizeDirectoryString(tmp_str.c_str());
		}
	}
	return m_CommonMediaDirFull[mt].c_str();
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getTextureFileNameRel(vid::ITexture *texture)
{
	core::stringc tex_full_fname =
		VIDEO_DRIVER.findTextureName(texture);
	core::stringc tex_fname = core::extractFileName(tex_full_fname);
	core::stringc tex_rel_path = core::getRelativePathBetweenDirectories(
		getMediaDirFull(res::EMT_TEXTURES),
		core::extractFilePath(tex_full_fname));
	static core::stringc tex_full_fname_ret;
	tex_full_fname_ret = (tex_rel_path + tex_fname).c_str();
	return tex_full_fname_ret.c_str();
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getCommonTextureFileNameRel(vid::ITexture *texture)
{
	core::stringc tex_full_fname =
		VIDEO_DRIVER.findTextureName(texture);
	core::stringc tex_fname = core::extractFileName(tex_full_fname);
	core::stringc tex_rel_path = core::getRelativePathBetweenDirectories(
		getCommonMediaDirFull(res::EMT_TEXTURES),
		core::extractFilePath(tex_full_fname));
	static core::stringc tex_full_fname_ret;
	tex_full_fname_ret = (tex_rel_path + tex_fname).c_str();
	return tex_full_fname_ret.c_str();
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name)
{
	const c8 *tex_fpath = core::extractFilePath(full_file_name);
	const c8 *tex_fname = core::extractFileName(full_file_name);
	const c8 *tex_rel_path = core::getRelativePathBetweenDirectories(
		getMediaDirFull(mt), tex_fpath);
	static core::stringc full_file_name_ret;
	full_file_name_ret.sprintf("%s%s", tex_rel_path, tex_fname);
	return full_file_name_ret.c_str();
}

//----------------------------------------------------------------------------

const c8* CResourceManager::getCommonMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name)
{
	const c8 *tex_fpath = core::extractFilePath(full_file_name);
	const c8 *tex_fname = core::extractFileName(full_file_name);
	const c8 *tex_rel_path = core::getRelativePathBetweenDirectories(
		getCommonMediaDirFull(mt), tex_fpath);
	static core::stringc full_file_name_ret;
	full_file_name_ret.sprintf("%s%s", tex_rel_path, tex_fname);
	return full_file_name_ret.c_str();
}

//----------------------------------------------------------------------------

IResourceManager *createResourceManager()
{ return new CResourceManager; }

//----------------------------------------------------------------------------
} // end namespace res
} // end namespace my
//----------------------------------------------------------------------------
