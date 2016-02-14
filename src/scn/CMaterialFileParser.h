//|-------------------------------------------------------------------------
//| File:        CMaterialFileParser.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMaterialFileParserHPP
#define CMaterialFileParserHPP
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
class CMaterialFileParser
{
public:

	static void loadMaterials(
		const c8 *fname, core::array<SMeshBufferMaterial>& materials,
		bool load_textures, bool reload_if_exists,
		u32 &loaded, u32 &reloaded, u32 &errors);

	static bool loadMaterialTo(
		io::IXMLReader *xml, SMeshBufferMaterial &material, bool load_textures);

	static void getMaterialXMLTextTo(
		core::stringc &out_buf, const vid::SMaterial &material,
		bool with_textures = true, const c8 *name = NULL);
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

