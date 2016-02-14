//|-------------------------------------------------------------------------
//| File:        CQ3ShaderFileParser.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CQ3ShaderFileParser.h"
#include "CQ3ShaderParser.h"
#include "CMaterialsManager.h"

#include <io/IFileSystem.h>
#include <io/IReadFile.h>
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

static char *s_shaderText;

//---------------------------------------------------------------------------

#define MAX_SHADER_LENGTH 4096

//---------------------------------------------------------------------------

struct SShaderEntry
{
	core::stringc ShaderName;
	c8 *ShaderText;

	SShaderEntry() 
		: ShaderName(""), ShaderText(0) {}

	SShaderEntry(const c8 *shader_name, c8 *start_ptr, s32 shader_len) 
		: ShaderName(shader_name), ShaderText(0) 
	{
		if (shader_len<=0 || shader_len>MAX_SHADER_LENGTH)
		{
			return;
		}

		ShaderText = new c8[shader_len+1];

		memcpy(ShaderText, start_ptr, sizeof(c8)*shader_len);
		ShaderText[shader_len]=0;
	}	

	~SShaderEntry() 
	{
		SAFE_DELETE_ARRAY(ShaderText);
	}
};

//---------------------------------------------------------------------------

void CQ3ShaderFileParser::loadShaders(
	const c8 *sh_fname, core::array<SMeshBufferMaterial> &shader_materials, 
	bool load_textures, bool reload_if_exists,
	u32 &loaded, u32 &reloaded, u32 &errors)
{
	c8 ch[255];

	core::array<SShaderEntry*> shader_entries;

	sprintf(ch, "Loading Q3 shaders from %s . . .", sh_fname);
	LOGGER.log(ch);	

	io::IReadFile *shader_file = FILE_SYSTEM.createAndOpenFile(sh_fname);
	if (!shader_file)
	{
		sprintf(ch, "Can't open shader file");
		LOGGER.log(ch, io::ELL_WARNING);
		errors++;
		return;
	}

	s_shaderText = new c8[shader_file->getSize()+1];
	s_shaderText[shader_file->getSize()]=0;

	shader_file->read(s_shaderText, shader_file->getSize());

	// removing all formatting
	q3::COM_Compress(s_shaderText);

	char *p, *token;

	p = s_shaderText;

	// look for label
	while ( 1 ) 
	{
		// find shader name
		token = q3::COM_ParseExt( &p, true );
		if ( token[0] == 0 ) 
		{
			break;
		}	

		char strippedName[MAX_QPATH];	
		q3::COM_StripExtension( token, strippedName );

		c8 *start_sh = p;

		// skip shader body
		q3::SkipBracedSection(&p);

		c8 *end_sh = p;

		shader_entries.push_back(
			new SShaderEntry(strippedName, start_sh, s32(end_sh-start_sh))
			);
	}

	// parsing finded shaders

	LOGGER.increaseFormatLevel();

	SMeshBufferMaterial material;

	u32 sh_cnt=0;
	for (u32 sh=0; sh < shader_entries.size(); sh++)
	{
		LOGGER.logInfo("Parsing Q3 shader '%s' . . .",
			shader_entries[sh]->ShaderName.c_str());

		s32 mat_idx = !reload_if_exists ? -1 :
			MATERIALS_MANAGER.findMaterialByName(shader_entries[sh]->ShaderName.c_str());

		LOGGER.increaseFormatLevel();

		if (CQ3ShaderParser::parseShaderTextToMaterial(
				shader_entries[sh]->ShaderName.c_str(), 
				shader_entries[sh]->ShaderText, material, 
				load_textures))
		{
			if (mat_idx != -1)
			{
				MATERIALS_MANAGER.setMaterial(mat_idx, material.Material);
				reloaded++;
			}
			else
			{
				shader_materials.push_back(material);
				loaded++;
			}
				
			u64 hash = material.Material.getHash();

			for (u32 i = 0; i < material.Material.getPassesCount(); i++)
			{
				const vid::SRenderPass &pass = material.Material.getPass(i);
				LOGGER.logInfo(" Pass %d: %s, gpu hash 0x%016I64X, ffp hash 0x%016I64X)",
					i, pass.isTransparent() ? "transp" : "solid",
					pass.getHashGPU(), pass.getHashFFP());
			}

			LOGGER.logInfo("Q3 shader '%s' parsed (%d passes, %s, hash 0x%016I64X)", 
				shader_entries[sh]->ShaderName.c_str(), material.Material.getPassesCount(),
				material.Material.isTransparent() ? "transp" : "solid",
				hash);
			sh_cnt++;
		}
		else
		{
			LOGGER.logErr("Q3 shader '%s' parsed with errors", 
				shader_entries[sh]->ShaderName.c_str());
			errors++;
		}

		LOGGER.decreaseFormatLevel();

		SAFE_DELETE(shader_entries[sh]);
	}

	LOGGER.decreaseFormatLevel();

	sprintf(ch, " %d shaders loaded", sh_cnt);
	LOGGER.log(ch);	

	SAFE_DELETE(s_shaderText);
	SAFE_DROP(shader_file);	
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
