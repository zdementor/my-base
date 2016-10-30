//|-------------------------------------------------------------------------
//| File:        IGPUProgram.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGPUProgramHPP
#define IGPUProgramHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

struct SGPUProgramShaderInfo
{
	u32                     Uniforms;
	u32                     Attributes;
    E_DRIVER_TYPE           Driver;
	core::stringc           Tag;
	E_VERTEX_SHADER_VERSION VertexVer;
	core::stringc           VertexFileName;
	E_PIXEL_SHADER_VERSION  PixelVer;
	core::stringc           PixelFileName;

	const c8* getTag() const
	{ return Tag.c_str(); }
	const c8* getVertexFileName() const
	{ return VertexFileName.c_str(); }
	const c8* getPixelFileName() const
	{ return PixelFileName.c_str(); }
};

//---------------------------------------------------------------------------

struct SGPUProgramInfo
{
	vid::E_VERTEX_TYPE                  VertexType;
	u32                                 LightsCount;
	u64                                 Hash;
	core::array <SGPUProgramShaderInfo> TaggedShaders;
	u32                                 TaggedShadersCount;
	core::array <SGPUProgramShaderInfo> OtherShaders;
	u32                                 OtherShadersCount;

	const SGPUProgramShaderInfo *getTaggedShaderInfo(u32 i) const
	{ return &TaggedShaders[i]; }
	const SGPUProgramShaderInfo *getOtherShaderInfo(u32 i) const
	{ return &OtherShaders[i]; }
};

//---------------------------------------------------------------------------

//! Interface for working with Direct3D/OpenGL GPU programm
class __MY_VID_LIB_API__ IGPUProgram : public IUnknown
{
public:

	//! Sets a constant for the program
	virtual bool setUniformfv(E_UNIFORM_TYPE uniform,
		const f32* floats, u32 size_bytes) = 0;
	virtual bool setUniformiv(E_UNIFORM_TYPE uniform,
		const s32* integers, u32 size_bytes) = 0;
	virtual bool setUniform1f(E_UNIFORM_TYPE uniform, f32 float_val) = 0;
	virtual bool setUniform1i(E_UNIFORM_TYPE uniform, s32 int_val) = 0;

	//! Return Uniform constans mask
	virtual u32 getUniformMask() = 0;

	//! Return true id GPU program sucessfully loaded, compiled and linked
	virtual bool isOK() = 0;

	virtual void clear() = 0;

	virtual bool recreate(u32 uniforms, u32 attributes, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader) = 0;

	virtual u32 getLightsCount() = 0;
};

//---------------------------------------------------------------------------

__MY_VID_LIB_API__ const SGPUProgramInfo* loadGPUProgramInfo(
	const c8 *file_name, const c8 *tag);

__MY_VID_LIB_API__ const SGPUProgramInfo* initIdentityGPUProgramInfo(
	vid::E_VERTEX_TYPE vertex_type, u64 hash, u32 lights_count);

__MY_VID_LIB_API__ const SGPUProgramInfo* initIdentityGPUProgramInfo(
	u32 lights_count);

__MY_VID_LIB_API__ bool setGPUProgramInfoLightsCount(const SGPUProgramInfo *prog_info,
	u32 lights_count);

__MY_VID_LIB_API__ bool appendGPUProgramInfo(const SGPUProgramInfo *prog_info,
	u32                     uniforms,
	u32                     attributes,
    E_DRIVER_TYPE           driver,
	const c8                *tag,
	E_VERTEX_SHADER_VERSION vertex_ver,
	const c8                *vertex_file_name,
	E_PIXEL_SHADER_VERSION  pixel_ver,
	const c8                *pixel_file_name);

__MY_VID_LIB_API__ bool writeGPUProgramInfo(
	const c8 *file_name, const SGPUProgramInfo *prog_info);

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

