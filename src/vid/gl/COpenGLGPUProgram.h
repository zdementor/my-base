//|-------------------------------------------------------------------------
//| File:        COpenGLGPUProgram.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLGPUProgramHPP
#define COpenGLGPUProgramHPP
//---------------------------------------------------------------------------

#include "../CNullGPUProgram.h"
#include "COpenGLDriver.h"

#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class COpenGLGPUProgram : public CNullGPUProgram
{
public:

	COpenGLGPUProgram(u32 uniforms, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);
	virtual ~COpenGLGPUProgram();

	virtual bool setUniformfv(E_UNIFORM_TYPE uniform,
		const f32* floats, u32 size_bytes);
	virtual bool setUniformiv(E_UNIFORM_TYPE uniform,
		const s32* integers, u32 size_bytes);
	virtual bool setUniform1f(E_UNIFORM_TYPE uniform, f32 float_val);
	virtual bool setUniform1i(E_UNIFORM_TYPE uniform, s32 int_val);

	virtual bool recreate(u32 uniforms, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

	GLuint _getProgramHandle() const { return m_Program.u; }

private:

	virtual void _destroyProgram();

	GLuint _createShader(
		GLenum shaderType, const c8 *shader, u32 *errors);

	union UHandle
	{
		GLuint	u;
	}
	m_Program, m_VertexShader, m_PixelShader;

	struct SUniform
	{
		SUniform()
			: Location(-1), Type(GL_FLOAT), Size(0), Bytes(0) {}
		s32		Location;
		GLenum	Type;
		s32		Size, Bytes;
	}
	m_Uniforms[vid::E_UNIFORM_TYPE_COUNT];
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
#endif
