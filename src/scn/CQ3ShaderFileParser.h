//|-------------------------------------------------------------------------
//| File:        CQ3ShaderFileParser.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CQ3ShaderFileParserHPP
#define CQ3ShaderFileParserHPP
//---------------------------------------------------------------------------

#include <my_inc.h>

#include "Q3Helper.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SMeshBufferMaterial;

//---------------------------------------------------------------------------

//! Meshloader capable of loading 3ds meshes.
class CQ3ShaderFileParser
{
public:

	static void loadShaders(
		const c8 *sh_fname, core::array<SMeshBufferMaterial> &shader_materials,
		bool load_textures, bool reload_if_exists,
		u32 &loaded, u32 &reloaded, u32 &errors);
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

