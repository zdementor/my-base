//|-------------------------------------------------------------------------
//| File:        CNullGPUProgram.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CNullGPUProgram.h"
#include "CNullDriver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

CNullGPUProgram::CNullGPUProgram()
	: m_Driver((CNullDriver*)VIDEO_DRIVER_PTR),
	m_UniformMask(EUF_NONE), m_AttribsMask(0), m_OK(false), m_LightsCount(0),
	m_Fog(EFT_LINEAR, 0, 0.f, 0.f, 0.f)
{}

//---------------------------------------------------------------------------

bool CNullGPUProgram::recreate(u32 uniforms, u32 attributes, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
	bool res = true;
	IGPUProgram *gpu_prog = ((CNullDriver*)VIDEO_DRIVER_PTR)->_getGPUProgramBound();
	if (this != gpu_prog)
		((CNullDriver*)VIDEO_DRIVER_PTR)->_bindGPUProgram(this);
	if (uniforms & EUF_TEXTURE0)
		res = setUniform1i(EUT_TEXTURE0, 0) && res;
	if (uniforms & EUF_TEXTURE1)
		res = setUniform1i(EUT_TEXTURE1, 1) && res;
	if (uniforms & EUF_TEXTURE2)
		res = setUniform1i(EUT_TEXTURE2, 2) && res;
	if (uniforms & EUF_TEXTURE3)
		res = setUniform1i(EUT_TEXTURE3, 3) && res;
	if (this != gpu_prog)
		((CNullDriver*)VIDEO_DRIVER_PTR)->_bindGPUProgram(gpu_prog);
	if (!lights_count)
		uniforms = uniforms & ~(EUF_LIGHTING);
	else
	if (!(uniforms & EUF_LIGHTING))
		lights_count = 0;
	CHECK_RANGE(lights_count, 0, PRL_MAX_SHADER_LIGHTS);
	m_UniformMask = uniforms;
	m_AttribsMask = attributes;
	m_LightsCount = lights_count;
	m_Fog = SFog(EFT_LINEAR, 0, 0.f, 0.f, 0.f);
	m_OK = res;
	return m_OK;
}

//---------------------------------------------------------------------------

void CNullGPUProgram::_destroyProgram()
{
	m_UniformMask = EUF_NONE;
	m_OK = false;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
