//|-------------------------------------------------------------------------
//| File:        CNullDriverGPUCache.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2017 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CNullDriver.h"

#include <io/IFileList.h>
#include <io/IWriteFile.h>
#include <io/IXMLReader.h>
#include <io/IXMLWriter.h>
#include <game/IGameManager.h>
#include <res/IResourceManager.h>
#include <scr/IScriptManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

struct SCGPUCache
{
	void clear()
	{
		m_GPUPrograms.clear();
	 	for (u32 k = 0; k < E_RENDER_PATH_COUNT; k++)
		for (u32 i = 0; i < E_VERTEX_TYPE_COUNT; i++)
			for (u32 j = 0; j <= PRL_MAX_SHADER_LIGHTS; j++)
				m_GPUProgramsHash[k][i][j].clear();
		m_GPUProgramsHashByContent.clear();
		m_GPUProgramsHashByFileName.clear();
		m_GPUProgramsHashFileNames.clear();
	}

	core::array      <IGPUProgram*>   m_GPUPrograms;

	core::hash_table <u64, IUnknown*>
		m_GPUProgramsHash[E_RENDER_PATH_COUNT][E_VERTEX_TYPE_COUNT][PRL_MAX_SHADER_LIGHTS + 1];

	core::hash_table <core::stringc, IUnknown*>
		m_GPUProgramsHashByContent, m_GPUProgramsHashByFileName;

	core::hash_table <IGPUProgram*, core::stringc> m_GPUProgramsHashFileNames;
};

static SCGPUCache _Cache;

//---------------------------------------------------------------------------

void CNullDriver::clearGPUProgramHash()
{
	_bindGPUProgram(NULL);
	_Cache.clear();

	LOGGER.logInfo("Cleared GPU program hash.");
}

//---------------------------------------------------------------------------

#define MY_VERTEX_SHADER_EXT	"vsh"
#define MY_PIXEL_SHADER_EXT		"psh"
#define MY_GPU_PROGRAM_EXT		"gpu"

void CNullDriver::loadGPUProgramsFromDir(const c8 *dir, const c8 *tag, bool reload_if_exists)
{
	if (!m_UseShaders)
		return;

	LOGGER.logInfo("Loading GPU programs from '%s' . . .", dir);

	io::IFileSystem &fs = FILE_SYSTEM;

	// store initial work dir
	core::stringc old_work_dir = fs.getWorkingDirectory();

	fs.changeWorkingDirectoryTo(dir);

	const io::IFileList* flist = fs.getFileList();

	LOGGER.increaseFormatLevel();

	u32 prg_cnt = 0, prg_ovr_cnt = 0;

	for (s32 f=0; f<flist->getFileCount(); f++)
	{
		if (flist->isDirectory(f))
			continue;
		core::stringc fname = flist->getFileName(f);
		core::stringc fext = flist->getFileExt(f);
		fext.make_lower();
		if (strstr(fext.c_str(), MY_GPU_PROGRAM_EXT))
			prg_ovr_cnt++;
	}

	s32 errors = 0;
	u32 shad_cnt = 0;
	core::stringc msg;

	f32 perc = 0.f, perc_old = 0.f;

	for (s32 f=0; f<flist->getFileCount(); f++)
	{
		if (flist->isDirectory(f))
			continue;

		core::stringc fname = flist->getFullFileName(f);
		core::stringc fext = flist->getFileExt(f);
		fext.make_lower();

		if (strstr(fext.c_str(), MY_GPU_PROGRAM_EXT))
		{
			if (perc == 0.f)
			{
				msg.sprintf("GPU programs (%d) - 0%%", prg_ovr_cnt);
				GAME_MANAGER.showMessageBox(
					true, "Please wait, while loading", msg.c_str());
			}

			perc = (prg_ovr_cnt > 0) ? ((prg_cnt + 1.f) / (1.f * prg_ovr_cnt)) : 0.f; 

			IGPUProgram *gpu_prog = _getGPUProgramFromFile(
				fname.c_str(), tag,
				reload_if_exists, false, false);
			if (gpu_prog)
			{
				prg_cnt++;
				if (!gpu_prog->isOK())
					errors++;
			}

			if (perc >= perc_old + 0.1f)
			{
				msg.sprintf("GPU programs (%d) - %d%%",
					prg_ovr_cnt, s32(100.f * perc + 0.5f));
				GAME_MANAGER.showMessageBox(
					true, "Please wait, while loading", msg.c_str());
				perc_old = perc;
			}
		}
	}
	GAME_MANAGER.showMessageBox(false);

	if (errors==0)
		LOGGER.logInfo("%d GPU programs loaded", prg_cnt);
	else
		LOGGER.logInfo("%d GPU programs loaded, with %d errors",
			prg_cnt, errors);

	LOGGER.decreaseFormatLevel();

	fs.changeWorkingDirectoryTo(old_work_dir.c_str());
}

//---------------------------------------------------------------------------

const core::stringc& CNullDriver::_getGPUProgramName(
	vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt)
{
	static core::stringc name;
	if (pass.isLighting())
		name.sprintf("ShaderGen-%s-%016I64X-%dl%s",
			vid::getVertexTypeName(vertex_type), pass.getHashGPU(), lightcnt,
			pass.getFlag(EMF_FOG_ENABLE) ? "-f" : "");
	else
		name.sprintf("ShaderGen-%s-%016I64X%s",
			vid::getVertexTypeName(vertex_type), pass.getHashGPU(),
			pass.getFlag(EMF_FOG_ENABLE) ? "-f" : "");
	return name;
}

//---------------------------------------------------------------------------

const core::stringc& CNullDriver::_getShaderName(
	vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt,
	const c8 *tag)
{
	static core::stringc name;
	name.sprintf("%s-%s-%s-%s", _getGPUProgramName(vertex_type, pass, lightcnt).c_str(),
		getDriverTypeName(getDriverType()),
		getRenderPathShortName(m_RenderPath),
		tag);
	return name;
}

//---------------------------------------------------------------------------

void _writeShader(io::IXMLWriter *xml_file,
	const core::array <SGPUProgramShaderInfo> &shaders)
{
	static core::stringw uniforms_strw, attributes_strw, driver_strw, rpath_strw;
	for (u32 i = 0; i < shaders.size(); i++)
	{
		if (shaders[i].Driver == vid::EDT_NULL)
			continue;
		driver_strw = getDriverTypeName(shaders[i].Driver);
		rpath_strw = getRenderPathName(shaders[i].RenderPath);
		uniforms_strw.sprintf("%d", shaders[i].Uniforms);
		attributes_strw.sprintf("%d", shaders[i].Attributes);
		core::stringc vertex_fname = core::extractFileName(shaders[i].VertexFileName);
		core::stringc pixel_fname = core::extractFileName(shaders[i].PixelFileName);
		xml_file->writeElement(L"Shader", true,
			L"driver", driver_strw.c_str(),
			L"rend_path", rpath_strw.c_str(),
			L"tag", core::stringw(shaders[i].Tag.c_str()).c_str(),
			L"uniforms", uniforms_strw.c_str(),
			L"attributes", attributes_strw.c_str(),
			L"vertex_ver", core::stringw(VertexShaderVersionName[shaders[i].VertexVer]).c_str(),
			L"vertex_fname", core::stringw(vertex_fname.c_str()).c_str(),
			L"pixel_ver", core::stringw(PixelShaderVersionName[shaders[i].PixelVer]).c_str(),
			L"pixel_fname", core::stringw(pixel_fname.c_str()).c_str());
		xml_file->writeLineBreak();
	}
}

//---------------------------------------------------------------------------

bool _writeGPUProgramFile(const c8 *file_name,
	vid::E_VERTEX_TYPE vtype, u64 hash, u32 lightcnt,
	const core::array <SGPUProgramShaderInfo> &other_shaders,
	const core::array <SGPUProgramShaderInfo> &tagged_shaders)
{
	io::IXMLWriter *xml_file = FILE_SYSTEM.createXMLWriter(file_name);
	if (xml_file)
	{
		xml_file->writeXMLHeader();
		xml_file->writeElement(L"GPUProgram", false,
			L"vertex_type", core::stringw().sprintf(L"%d", vtype).c_str(),
			L"hash", core::stringw().sprintf(L"%016I64X", hash).c_str(),
			L"lights", core::stringw().sprintf(L"%d", lightcnt).c_str());
		xml_file->writeLineBreak();
		_writeShader(xml_file, other_shaders);
		_writeShader(xml_file, tagged_shaders);
		xml_file->writeClosingTag(L"GPUProgram");
		xml_file->writeLineBreak();
		xml_file->drop();
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

bool _loadGPUProgramFile(const c8 *file_name, const c8 *tag,
	core::array <SGPUProgramShaderInfo> &other_shaders,
	core::array <SGPUProgramShaderInfo> &tagged_shaders,
	vid::E_VERTEX_TYPE *vertex_type, u64 *hash, u32 *lights,
	bool additional_logging)
{
	io::IXMLReader *xml_file = FILE_SYSTEM.createXMLReader(file_name);
	if (!xml_file)
	{
		if (additional_logging)
			LOGGER.logErr("Can't open %s", file_name);
		return false;
	}

	bool ok = true;

	if (vertex_type)
		*vertex_type = (vid::E_VERTEX_TYPE)-1;
	if (hash)
		*hash = (u64)-1;
	if (lights)
		*lights = (u32)-1;

	while (ok && xml_file->read())
	{
		if (xml_file->getNodeType() != io::EXN_ELEMENT)
			continue;

		core::stringw xml_node_name = xml_file->getName();

		if (core::stringw("GPUProgram") == xml_node_name)
		{
			if (vertex_type)
				*vertex_type = (vid::E_VERTEX_TYPE)xml_file->getAttributeValueAsInt(
					L"vertex_type", (vid::E_VERTEX_TYPE)-1);
			if (hash)
				*hash = xml_file->getAttributeValueAsHexUInt64(L"hash", (u64)-1);
			if (lights)
				*lights = (u32)xml_file->getAttributeValueAsInt(L"lights", (u64)-1);
		}
		else
		if (core::stringw("Shader") == xml_node_name)
		{
			core::stringc tmp;
			bool tagged = false;

			SGPUProgramShaderInfo e;
			e.Driver = (E_DRIVER_TYPE)xml_file->getAttributeValueAsIndexInArray
				(L"driver",
				vid::DriverTypeName, vid::E_DRIVER_TYPE_COUNT,
				vid::EDT_NULL);
			e.RenderPath = (E_RENDER_PATH)xml_file->getAttributeValueAsIndexInArray
				(L"rend_path",
				vid::RenderPathName, vid::E_RENDER_PATH_COUNT,
				vid::ERP_FORWARD_RENDERING);

			e.Tag = xml_file->getAttributeValue(L"tag");
			if (!tag || e.Tag == tag)
				tagged = true;

			e.Uniforms = (E_UNIFORM_FLAG)
				xml_file->getAttributeValueAsInt(L"uniforms", (s32)vid::EUF_NONE);
			if (e.Uniforms == (u32)vid::EUF_NONE)
				e.Uniforms = (E_UNIFORM_FLAG)xml_file->getAttributeValueAsFlagBits(
					L"uniform_names",
					vid::UniformTypeReadableNames, vid::UniformTypeBits, vid::E_UNIFORM_TYPE_COUNT,
					(u32)vid::EUF_NONE);
			e.Attributes = (E_UNIFORM_FLAG)
				xml_file->getAttributeValueAsInt(L"attributes", 0);
			if (e.Attributes == 0)
				e.Attributes = (E_UNIFORM_FLAG)xml_file->getAttributeValueAsFlagBits(
					L"attribute_names",
					vid::AttribTypeReadableNames, vid::AttribTypeBits, vid::E_ATTRIB_TYPE_COUNT,
					0);

			e.VertexVer = (E_VERTEX_SHADER_VERSION)xml_file->getAttributeValueAsIndexInArray(
				L"vertex_ver",
				vid::VertexShaderVersionName, vid::E_VERTEX_SHADER_VERSION_COUNT,
				(u32)0);
			tmp = xml_file->getAttributeValue(L"vertex_fname");
			e.VertexFileName.sprintf("%s%s", xml_file->getFilePath(), tmp.c_str());

			e.PixelVer = (E_PIXEL_SHADER_VERSION)xml_file->getAttributeValueAsIndexInArray(
				L"pixel_ver",
				vid::PixelShaderVersionName, vid::E_PIXEL_SHADER_VERSION_COUNT,
				(u32)0);
			tmp = xml_file->getAttributeValue(L"pixel_fname");
			e.PixelFileName.sprintf("%s%s", xml_file->getFilePath(), tmp.c_str());

			core::array <SGPUProgramShaderInfo> *shadersp = &other_shaders;
			if (tagged)
				shadersp = &tagged_shaders;
			shadersp->push_back(e);
		}
		else
		{
			LOGGER.logErr(L"Unknown tag %s", xml_node_name.c_str());
			ok = false;
		}
	}
	xml_file->drop();
	
	return ok;
}

//---------------------------------------------------------------------------

bool CNullDriver::compileGPUSources(u32 uniforms, u32 attributes, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
	bool res = true;
	CNullGPUProgram *gpu_prog = _createGPUProgram(uniforms, attributes, lights_count,
		vertex_shader_ver, vertex_shader,
		pixel_shader_ver, pixel_shader);
	if (!gpu_prog->isOK())
	{
		LOGGER.logErr("While compiling GPU program");
		res = false;
	}
	gpu_prog->drop();
	return res;
}

//---------------------------------------------------------------------------

static SGPUProgramInfo _ProgramInfo;

const SGPUProgramInfo* loadGPUProgramInfo(const c8 *file_name, const c8 *tag)
{
	if (!FILE_SYSTEM.existFile(file_name))
		return NULL;
	_ProgramInfo.Hash        = 0;
	_ProgramInfo.LightsCount = 0;
	_ProgramInfo.VertexType  = (vid::E_VERTEX_TYPE)0;
	_ProgramInfo.OtherShaders.set_used(0);
	_ProgramInfo.TaggedShaders.set_used(0);

	_loadGPUProgramFile(file_name, tag, _ProgramInfo.OtherShaders, _ProgramInfo.TaggedShaders,
		&_ProgramInfo.VertexType, &_ProgramInfo.Hash, &_ProgramInfo.LightsCount,
		false);

	_ProgramInfo.TaggedShadersCount = _ProgramInfo.TaggedShaders.size();
	_ProgramInfo.OtherShadersCount  = _ProgramInfo.OtherShaders.size();
	return &_ProgramInfo;
}

const SGPUProgramInfo* initIdentityGPUProgramInfo(
	vid::E_VERTEX_TYPE vertex_type, u64 hash, u32 lights_count)
{
	_ProgramInfo.VertexType  = vertex_type;
	_ProgramInfo.Hash        = hash;
	_ProgramInfo.LightsCount = lights_count;
	_ProgramInfo.OtherShaders.set_used(0);
	_ProgramInfo.TaggedShaders.set_used(0);

	_ProgramInfo.TaggedShadersCount = _ProgramInfo.TaggedShaders.size();
	_ProgramInfo.OtherShadersCount  = _ProgramInfo.OtherShaders.size();
	return &_ProgramInfo;
}

const SGPUProgramInfo* initIdentityGPUProgramInfo(
	u32 lights_count)
{
	return initIdentityGPUProgramInfo((vid::E_VERTEX_TYPE)0, 0, lights_count);
}

bool setGPUProgramInfoLightsCount(const SGPUProgramInfo *prog_info,
	u32 lights_count)
{
	if (prog_info != &_ProgramInfo)
		return false;
	_ProgramInfo.LightsCount = lights_count;
	return true;
}

bool appendGPUProgramInfo(const SGPUProgramInfo *prog_info,
	u32                     uniforms,
	u32                     attributes,
    E_DRIVER_TYPE           driver,
	E_RENDER_PATH           rend_path,
	const c8                *tag,
	E_VERTEX_SHADER_VERSION vertex_ver,
	const c8                *vertex_file_name,
	E_PIXEL_SHADER_VERSION  pixel_ver,
	const c8                *pixel_file_name)
{
	if (prog_info != &_ProgramInfo)
		return false;
	SGPUProgramShaderInfo shader_info;
	shader_info.Uniforms       = uniforms;
	shader_info.Attributes     = attributes;
	shader_info.Driver         = driver;
	shader_info.RenderPath     = rend_path;
	shader_info.Tag            = tag;
	shader_info.VertexVer      = vertex_ver;
	shader_info.VertexFileName = vertex_file_name;
	shader_info.PixelVer      = pixel_ver;
	shader_info.PixelFileName = pixel_file_name;

	for (u32 i = 0; i < _ProgramInfo.TaggedShaders.size(); i++)
	{
		if (_ProgramInfo.TaggedShaders[i].Tag == tag
				&& _ProgramInfo.TaggedShaders[i].Driver == driver
				&& _ProgramInfo.TaggedShaders[i].RenderPath == rend_path)
		{
			_ProgramInfo.TaggedShaders[i] = shader_info;
			return true;
		}
	}
	_ProgramInfo.OtherShaders.push_back(shader_info);
	_ProgramInfo.TaggedShadersCount = _ProgramInfo.TaggedShaders.size();
	_ProgramInfo.OtherShadersCount  = _ProgramInfo.OtherShaders.size();
	return true;
}

bool writeGPUProgramInfo(const c8 *file_name, const SGPUProgramInfo *prog_info)
{
	if (prog_info != &_ProgramInfo)
		return false;
	return _writeGPUProgramFile(file_name,
		_ProgramInfo.VertexType, _ProgramInfo.Hash, _ProgramInfo.LightsCount,
		_ProgramInfo.OtherShaders, _ProgramInfo.TaggedShaders);
}

//---------------------------------------------------------------------------

struct SGPUProgram : public IUnknown
{
	SGPUProgram(class CNullGPUProgram *program, const char *name = NULL)
		: Program(program), Name(name) {}
	virtual ~SGPUProgram()
	{ Program->drop(); }
	CNullGPUProgram *Program;
	core::stringc Name;
};

//---------------------------------------------------------------------------

const c8* CNullDriver::findGPUProgramFileName(vid::IGPUProgram *gpu_prog)
{
	static core::stringc filename;
	filename = core::extractFileName(findGPUProgramFullFileName(gpu_prog));
	if (filename.size() == 0)
		filename = NONAME_FILE_NAME;
    return filename.c_str();
}

//---------------------------------------------------------------------------

const c8* CNullDriver::findGPUProgramFullFileName(vid::IGPUProgram *gpu_prog)
{
    static core::stringc filename;
	filename = _Cache.m_GPUProgramsHashFileNames.get_value(gpu_prog);
	//filename.make_lower();
	if (filename.size() == 0)
		filename = NONAME_FILE_NAME;
    return filename.c_str();
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::addGPUProgram(
	vid::E_VERTEX_TYPE vertex_type, const vid::SRenderPass &pass,
	u32 uniforms, u32 attributes, u32 lightcnt,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
	const c8 *tag)
{
	SGPUProgram *program = NULL;
	do
	{
		if (!m_UseShaders)
			break;
		
		if (!pass.isLighting())
			lightcnt = 0;
		else
			lightcnt = lightcnt <= PRL_MAX_SHADER_LIGHTS ?
				lightcnt : PRL_MAX_SHADER_LIGHTS;

		u64 hash = pass.getHashGPU();
		program = (SGPUProgram*)_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].get_value(hash);
		if (program)
		{
			LOGGER.logErr("Possibble hash conflict, same GPU program already exists "
				"(vertex_type = %s, hash = 0x%016I64X, lights = %d):",
				getVertexTypeName(vertex_type), pass.getHashGPU(), lightcnt);
			LOGGER.logErr("Vertex Shader:\n%s", vertex_shader);
			LOGGER.logErr("Pixel Shader:\n%s", pixel_shader);
			LOGGER.logErr("Skip GPU program creation");
			break;
		}

		static core::stringc keyname;
		{
			static core::stringc psh, vsh;
			u32 hash_vsh = core::hash_string(vertex_shader, 32, 0xffffffff);
			u32 hash_psh = core::hash_string(pixel_shader, 32, 0xffffffff);

			vsh.sprintf("%X.%s", hash_vsh, MY_VERTEX_SHADER_EXT);
			psh.sprintf("%X.%s", hash_psh, MY_PIXEL_SHADER_EXT);
			keyname.sprintf("%s_%s", vsh.c_str(), psh.c_str());
		}
		program = (SGPUProgram*)_Cache.m_GPUProgramsHashByContent.get_value(keyname);
		if (program)
		{
			_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].set_value(hash, program);
			break;
		}

		static core::stringc vsh, psh, gpu_file_name;
		const char *gpu_file_name_c = NULL;

		const core::stringc &progname = _getGPUProgramName(vertex_type, pass, lightcnt);

		if (m_CacheShaders)
		{
			const c8* tmp_dir =
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEMP_DATA);

			const core::stringc &shadername = _getShaderName(vertex_type, pass, lightcnt, tag);

			vsh.sprintf("%s%s." MY_VERTEX_SHADER_EXT, tmp_dir, shadername.c_str());
			psh.sprintf("%s%s." MY_PIXEL_SHADER_EXT, tmp_dir, shadername.c_str());
			gpu_file_name.sprintf("%s%s." MY_GPU_PROGRAM_EXT, tmp_dir, progname.c_str());
			gpu_file_name_c = gpu_file_name.c_str();

			const SGPUProgramInfo *prog_info = loadGPUProgramInfo(
				gpu_file_name.c_str(), tag);
			if (!prog_info)
				prog_info = initIdentityGPUProgramInfo(vertex_type, hash, lightcnt);

			io::IWriteFile *vsh_file =
				FILE_SYSTEM.createAndWriteFile(vsh.c_str());
			if (vsh_file)
			{
				vsh_file->write(vertex_shader, core::strlen(vertex_shader)+1);
				vsh_file->drop();
			}
			io::IWriteFile *psh_file =
				FILE_SYSTEM.createAndWriteFile(psh.c_str());
			if (psh_file)
			{
				psh_file->write(pixel_shader, core::strlen(pixel_shader)+1);
				psh_file->drop();
			}
			
			SGPUProgramShaderInfo e;
			e.Driver	     = getDriverType();
			e.RenderPath     = m_RenderPath;
			e.Uniforms	     = uniforms;
			e.Attributes     = attributes;
			e.VertexVer      = vertex_shader_ver;
			e.VertexFileName = vsh;
			e.PixelVer       = pixel_shader_ver;
			e.PixelFileName  = psh;
			e.Tag            = tag;

			bool has_entry_already = false;
			for (u32 i = 0; i < prog_info->TaggedShadersCount; i++)
			{
				if (e.Driver == prog_info->TaggedShaders[i].Driver
						&& e.RenderPath == prog_info->TaggedShaders[i].RenderPath)
				{
					has_entry_already = true;
					break;
				}
			}
			if (!has_entry_already)
			{
				if (appendGPUProgramInfo(prog_info,
						e.Uniforms, e.Attributes, e.Driver, e.RenderPath, e.Tag.c_str(),
						e.VertexVer, e.VertexFileName.c_str(),
						e.PixelVer, e.PixelFileName.c_str()))
					writeGPUProgramInfo(gpu_file_name.c_str(), prog_info);
			}
		}
		CNullGPUProgram* gpu_prog = _createGPUProgram(uniforms, attributes, lightcnt,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
		if (!gpu_prog->isOK())
		{
			LOGGER.logErr("While creating GPU program: %s",
				progname.c_str());
		}
		program = new SGPUProgram(gpu_prog, gpu_file_name_c);
		_Cache.m_GPUPrograms.push_back(program->Program);
		_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].set_value(hash, program);
		_Cache.m_GPUProgramsHashByContent.set_value(keyname, program);
		program->drop();
	}
	while (0);
	return program ? program->Program : NULL;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::addGPUProgram(
	u32 uniforms, u32 attributes, u32 lightcnt,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
	const c8 *tag)
{
	(void)tag; // XXX ignore for now

	SGPUProgram *program = NULL;
	do
	{
		if (!m_UseShaders)
			break;

		u32 hash_vsh = core::hash_string(vertex_shader, 32, 0xffffffff);
		u32 hash_psh = core::hash_string(pixel_shader, 32, 0xffffffff);

		static core::stringc keyname, psh, vsh;

		vsh.sprintf("%X.%s", hash_vsh, MY_VERTEX_SHADER_EXT);
		psh.sprintf("%X.%s", hash_psh, MY_PIXEL_SHADER_EXT);
		keyname.sprintf("%s_%s", vsh.c_str(), psh.c_str());

		program = (SGPUProgram*)_Cache.m_GPUProgramsHashByContent.get_value(keyname);
		if (program)
			break;

		CNullGPUProgram *gpu_prog = _createGPUProgram(uniforms, attributes, lightcnt,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
		if (!gpu_prog->isOK())
			LOGGER.logErr("While creating GPU program");

		program = new SGPUProgram(gpu_prog);
		_Cache.m_GPUPrograms.push_back(program->Program);
		_Cache.m_GPUProgramsHashByContent.set_value(keyname, program);
		program->drop();
	}
	while (0);
	return program ? program->Program : NULL;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::getGPUProgram(
	vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt)
{
	if (!m_UseShaders)
		return NULL;
	IGPUProgram *gpu_prog = pass.getGPUProgram();
	if (gpu_prog)
		return gpu_prog;

	if (m_GenShaderMaxLights == -1)
	{
		if (!SCRIPT_MANAGER.runScriptCallback(
				scr::ESCT_GET_GEN_GPU_PROGRAM_MAX_LIGHTS, &m_GenShaderMaxLights))
			m_GenShaderMaxLights = 0;
		LOGGER.logInfo("Max lights count of GPU program generator is %d",
			m_GenShaderMaxLights);
		if (m_GenShaderMaxLights== 0)
			LOGGER.logInfo("GPU program generator is disabled");
	}
	if (m_GenShaderMaxLights == 0)
		return NULL;

	if (!pass.isLighting())
		lightcnt = 0;
	else
		lightcnt = (s32)lightcnt <= m_GenShaderMaxLights ?
			lightcnt : m_GenShaderMaxLights;

	u64 hash = pass.getHashGPU();
	SGPUProgram *program =
		(SGPUProgram*)_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].get_value(hash);
	gpu_prog = program ? program->Program : NULL;
	if (!gpu_prog)
	{			
		bool res = false;
		if (SCRIPT_MANAGER.runScriptCallback(scr::ESCT_GEN_GPU_PROGRAM, &res,
				vertex_type, (void*)&pass, lightcnt))
		{
			program = (SGPUProgram*)_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].get_value(hash);
			gpu_prog = program ? program->Program : NULL;
			if (!gpu_prog)
				LOGGER.logErr("Can't add GPU program "
					"(vertex_type = %s, hash = 0x%016I64X, lights = %d)",
					getVertexTypeName(vertex_type), pass.getHashGPU(), lightcnt);
		}
	}
	return gpu_prog;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::getGPUProgram(
	vid::E_VERTEX_TYPE vertex_type, u64 hash, u32 lightcnt)
{
	if (!m_UseShaders)
		return NULL;
	lightcnt = lightcnt <= PRL_MAX_SHADER_LIGHTS ?
		lightcnt : PRL_MAX_SHADER_LIGHTS;
	SGPUProgram *program =
		(SGPUProgram*)_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lightcnt].get_value(hash);
	IGPUProgram *gpu_prog = program ? program->Program : NULL;
	return gpu_prog;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::getGPUProgram(
	const c8 *file_name, const c8 *tag, bool reload_if_exists)
{
	IGPUProgram *gpu_prog = _getGPUProgramFromFile(
		file_name, tag, reload_if_exists, true, true);
	if (!gpu_prog)
	{
		LOGGER.logErr("Loading GPU program from %s",
			file_name);
	}
	return gpu_prog;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::_getGPUProgramFromFile(
	const c8 *file_name, const c8 *tag,
	bool reload_if_exists, bool additional_logging, bool return_any_if_no_tagged)
{
	SGPUProgram *program = NULL;
	do
	{
		program = (SGPUProgram*)_Cache.m_GPUProgramsHashByFileName.get_value(file_name);
		if (program && !reload_if_exists)
			break;

		core::array <SGPUProgramShaderInfo> other_shaders, tagged_shaders;
		vid::E_VERTEX_TYPE vertex_type = (vid::E_VERTEX_TYPE)-1;
		u64 hash = (u64)-1;
		u32 lights = (u32)-1;

		bool justAllocated = false;

		if (!_loadGPUProgramFile(file_name, tag,
				other_shaders, tagged_shaders,
				&vertex_type, &hash, &lights,
				additional_logging))
		{
			LOGGER.logErr("Parsing GPU program file %s", file_name);
			break;
		}
		
		core::array <SGPUProgramShaderInfo> *shadersp = &tagged_shaders;
		if (!tagged_shaders.size() && return_any_if_no_tagged)
			shadersp = &other_shaders;
		
		bool ok = false;
		static core::stringc vsh_fname, psh_fname;
		E_VERTEX_SHADER_VERSION vsh_ver;
		E_PIXEL_SHADER_VERSION psh_ver;
		u32 uniforms = EUF_NONE;
		u32 attributes = 0;
		for (u32 i = 0; i < shadersp->size(); i++)
		{
			if ((*shadersp)[i].Driver == getDriverType()
					&& (*shadersp)[i].RenderPath == m_RenderPath)
			{
				vsh_ver	  = (*shadersp)[i].VertexVer;
				psh_ver	  = (*shadersp)[i].PixelVer;
				vsh_fname = (*shadersp)[i].VertexFileName.c_str();
				psh_fname = (*shadersp)[i].PixelFileName.c_str();
				uniforms  = (*shadersp)[i].Uniforms;
				attributes= (*shadersp)[i].Attributes;
				ok = true;
				break;
			}
		}
		if (!ok)
		{
			if (additional_logging)
				LOGGER.logErr("Can't find appropriate shader for %s in file %s",
					vid::DriverTypeName[getDriverType()], file_name);
			break;
		}

		const c8 *vsh_file_name = vsh_fname.c_str();
		const c8 *psh_file_name = psh_fname.c_str();

		core::array<c8> vertex_shader, pixel_shader;
		io::IReadFile *vsh_file =
			FILE_SYSTEM.createAndOpenFile(vsh_file_name);
		if (vsh_file)
		{
			u32 fsz = vsh_file->getSize();
			vertex_shader.set_used(fsz + 1);
			vsh_file->read(&vertex_shader[0], fsz);
			vsh_file->drop();
			vertex_shader[fsz] = 0;
		}
		io::IReadFile *psh_file =
			FILE_SYSTEM.createAndOpenFile(psh_file_name);
		if (psh_file)
		{
			u32 fsz = psh_file->getSize();
			pixel_shader.set_used(fsz + 1);
			psh_file->read(&pixel_shader[0], fsz);
			psh_file->drop();
			pixel_shader[fsz] = 0;
		}
		if (!vertex_shader.size() || !pixel_shader.size())
		{
			if (!vertex_shader.size())
				LOGGER.logErr("Can't load vertex shader file %s",
					vsh_file_name);
			if (!pixel_shader.size())
				LOGGER.logErr("Can't load pixel shader file %s",
					psh_file_name);
			LOGGER.logErr("Loading GPU program from %s",
				file_name);
			break;
		}

		if (program)
			program->Program->recreate(
				uniforms, attributes, lights != -1 ? lights : 0,
				vsh_ver, &vertex_shader[0],
				psh_ver, &pixel_shader[0]);
		else
		{
			do
			{
				if (vertex_type != (vid::E_VERTEX_TYPE)-1 &&
						hash != (u64)-1 &&
						//hash != (u64)0 &&
						lights != (u32)-1)
				{
					program = (SGPUProgram*)_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lights].get_value(hash);
					if (program && !reload_if_exists)
						break;
				}
				
				if (program)
					program->Program->recreate(
						uniforms, attributes, lights != -1 ? lights : 0,
						vsh_ver, &vertex_shader[0],
						psh_ver, &pixel_shader[0]);
				else
				{
					CNullGPUProgram *gpu_prog = _createGPUProgram(
						uniforms, attributes, lights != -1 ? lights : 0,
						vsh_ver, &vertex_shader[0],
						psh_ver, &pixel_shader[0]);
					program = new SGPUProgram(gpu_prog, file_name);
					_Cache.m_GPUPrograms.push_back(program->Program);
					justAllocated = true;

					if (vertex_type != (vid::E_VERTEX_TYPE)-1 &&
							hash != (u64)-1 &&
							//hash != (u64)0 &&
							lights != (u32)-1)
						_Cache.m_GPUProgramsHash[m_RenderPath][vertex_type][lights].set_value(hash, program);
				}
			}
			while (0);
		}
		if (!program || !program->Program->isOK())
			LOGGER.logErr("While creating GPU program: %s",
				file_name);
		if (program)
		{
			if (m_CacheShaders)
				program->Name = file_name;
			_Cache.m_GPUProgramsHashByFileName.set_value(file_name, program);
			_Cache.m_GPUProgramsHashFileNames.set_value(program->Program, file_name);
			if (justAllocated)
				program->drop();
		}
	}
	while(0);
	return program ? program->Program : NULL;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
