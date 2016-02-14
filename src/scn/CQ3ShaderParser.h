//|-------------------------------------------------------------------------
//| File:        CQ3ShaderParser.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CQ3ShaderParserHPP
#define CQ3ShaderParserHPP
//---------------------------------------------------------------------------

#include <scn/IMaterialsManager.h>

#include "Q3Helper.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SMeshBufferMaterial;

//---------------------------------------------------------------------------

//! Meshloader capable of loading 3ds meshes.
class CQ3ShaderParser
{
public:	

	static bool parseShaderTextToMaterial(
		const c8* shader_name, c8* shader_string, SMeshBufferMaterial &material, 
		bool load_textures
		);
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

