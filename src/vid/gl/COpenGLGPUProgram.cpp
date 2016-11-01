//|-------------------------------------------------------------------------
//| File:        COpenGLGPUProgram.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLGPUProgram.h"

#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

COpenGLGPUProgram::COpenGLGPUProgram(u32 uniforms, u32 attributes, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
#if MY_DEBUG_MODE 
	setClassName("COpenGLGPUProgram");
#endif

	m_Program.u = m_VertexShader.u = m_PixelShader.u = 0;

	do
	{
		if (!VIDEO_DRIVER.queryFeature(vid::EVDF_SHADER_LANGUAGE))
		{
			LOGGER.logErr("Your video hardware not support GLSL feature");
			break;
		}
		if (!recreate(uniforms, attributes, lights_count,
				vertex_shader_ver, vertex_shader,
				pixel_shader_ver, pixel_shader))
			break;
	}
	while (0);
}

//---------------------------------------------------------------------------

COpenGLGPUProgram::~COpenGLGPUProgram()
{
	_destroyProgram();
}

//---------------------------------------------------------------------------

void COpenGLGPUProgram::_destroyProgram()
{
	if (m_VertexShader.u)
	{
		glDeleteShader(m_VertexShader.u);
		m_VertexShader.u = 0;
	}
	if (m_PixelShader.u)
	{
		glDeleteShader(m_PixelShader.u);
		m_PixelShader.u = 0;
	}
	if (m_Program.u)
	{
		glDeleteProgram(m_Program.u);
		m_Program.u = 0;
	}
	CNullGPUProgram::_destroyProgram();
}

//---------------------------------------------------------------------------

bool COpenGLGPUProgram::recreate(u32 uniforms, u32 attributes, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
	u32 errors = 0;

	_destroyProgram();

	m_Program.u = glCreateProgram();

	// compiling shaders
	m_VertexShader.u = _createShader(
		GL_VERTEX_SHADER, vertex_shader, &errors);
	m_PixelShader.u = _createShader(
		GL_FRAGMENT_SHADER, pixel_shader, &errors);
	if (errors > 0)
	{
		_destroyProgram();
		return false;
	}

	// linking shaders into program
	glAttachShader(m_Program.u, m_VertexShader.u);
	glAttachShader(m_Program.u, m_PixelShader.u);

	for (u32 i = 0; i < E_ATTRIB_TYPE_COUNT; i++)
	{
		if (attributes & vid::AttribTypeBits[i])
			glBindAttribLocation(m_Program.u,
				getOGLAttribLocation((E_ATTRIB_TYPE)i),
				getAttribReadableName((E_ATTRIB_TYPE)i));
	}

	glLinkProgram(m_Program.u);

	GLint status = 0;
	glGetProgramiv(m_Program.u, GL_LINK_STATUS, &status);
	if (!status)
	{
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;
		glGetProgramiv(m_Program.u, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar *infoLog = new GLchar[maxLength];
		glGetProgramInfoLog(m_Program.u, maxLength, &length, infoLog);
		LOGGER.logErr("GLSL program linking errors:\n%s",
			(c8*)infoLog);
		delete [] infoLog;
		errors = errors + 1;
	}
	if (errors > 0)
	{
		_destroyProgram();
		return false;
	}

	// get uniforms information

	GLint num = 0, maxlen = 0;;
	glGetProgramiv(m_Program.u, GL_ACTIVE_UNIFORMS, &num);
	glGetProgramiv(m_Program.u, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

	// seems that some implementations use an extra null terminator
	++maxlen;
	core::array<c8> buf(maxlen);
	buf.set_used(maxlen);
	for (s32 i = 0; i < num && maxlen > 3; ++i)
	{
		GLint size;
		GLenum type;
		memset(buf.pointer(), 0, maxlen);
		glGetActiveUniform(
			m_Program.u, i, maxlen, 0, &size, &type, (GLchar*)buf.pointer());
		if (buf[0] == 'g' && buf[1] == 'l' && buf[2] == '_')
		{
			// skip OpenGL build-in uniforms
			continue;
		}
		GLint loc = glGetUniformLocation(m_Program.u, buf.pointer());
		if (size > 1)
		{
			s32 j = core::strlen(buf.pointer());
			while (j)
			{
				if (buf[j] == '[')
				{
					buf[j] = 0;
					break;
				}
				j--;
			}
		}
		E_UNIFORM_TYPE unitype = VIDEO_DRIVER.getUniformType(buf.pointer());
		if (unitype != EUT_NONE)
		{
			u32 size_bytes = 4;
			switch (type)
			{
			case GL_FLOAT:
				break;
			case GL_FLOAT_VEC2:
				size_bytes *= 2;
				break;
			case GL_FLOAT_VEC3:
				size_bytes *= 3;
				break;
			case GL_FLOAT_VEC4:
				size_bytes *= 4;
				break;
			case GL_FLOAT_MAT2:
				size_bytes *= 4;
				break;
			case GL_FLOAT_MAT3:
				size_bytes *= 9;
				break;
			case GL_FLOAT_MAT4:
				size_bytes *= 16;
				break;
			case GL_INT:
			case GL_SAMPLER_1D:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_1D_SHADOW:
			case GL_SAMPLER_2D_SHADOW:
				break;
			case GL_INT_VEC2:
				size_bytes *= 2;
				break;
			case GL_INT_VEC3:
				size_bytes *= 3;
				break;
			case GL_INT_VEC4:
				size_bytes *= 4;
				break;
			default:
				LOGGER.logErr("Unknown OpenGL uniform type %X",
					type);
				errors++;
				break;
			}

			if (unitype == vid::EUT_LIGHTING)
			{
				if (size != lights_count)
				{
					LOGGER.logWarn("Incorrect lights count in GPU program (%d actual, %d expected)",
						size, lights_count);
					lights_count = size;
				}
			}
			m_Uniforms[unitype].Location = loc;
			m_Uniforms[unitype].Type = type;
			m_Uniforms[unitype].Size = size;
			m_Uniforms[unitype].Bytes = size * size_bytes;
		}
		else
		{
			LOGGER.logWarn("Unknown uniform '%s'", buf.pointer());
			errors++;
		}
	}

	// validating uniforms

	for (u32 i = 0; i < vid::E_UNIFORM_TYPE_COUNT; i++)
	{
		if (uniforms & vid::UniformTypeBits[i])
		{
			s32 location = m_Uniforms[i].Location;
			if (location == -1)
			{
				LOGGER.logErr("Can't validate uniform '%s'",
					getUniformName((E_UNIFORM_TYPE)i));
			}
		}
	}

	if (!errors)
		return CNullGPUProgram::recreate(uniforms, attributes, lights_count,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
	return false;
}

//---------------------------------------------------------------------------

GLuint COpenGLGPUProgram::_createShader(
	GLenum shaderType, const c8* shader, u32 *errors)
{
	GLuint shaderHandle = glCreateShader(shaderType);

	glShaderSource(shaderHandle, 1, &shader, NULL);
	glCompileShader(shaderHandle);

	GLint status = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar *infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
		LOGGER.logErr("GLSL %s Shader compilation errors:\n%s",
			(shaderType == GL_VERTEX_SHADER) ? "Vertex" : "Pixel",
			(c8*)infoLog);
		delete [] infoLog;
		*errors = *errors + 1;
	}
	return shaderHandle;
}

//---------------------------------------------------------------------------

bool COpenGLGPUProgram::setUniformfv(
	E_UNIFORM_TYPE uniform, const f32* floats, u32 size_bytes)
{
	GLint loc = m_Uniforms[uniform].Location;
	if (loc == -1)
	{
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
		return false;
	}

	GLenum type = m_Uniforms[uniform].Type;
	s32 count = m_Uniforms[uniform].Size;
	u32 actual_size = m_Uniforms[uniform].Bytes;

	if (size_bytes != actual_size)
	{
		LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
			getUniformName(uniform), actual_size, size_bytes);
		return false;
	}

	switch (type)
	{
	case GL_FLOAT:
		glUniform1fv(loc, count, floats);
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(loc, count, floats);
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(loc, count, floats);
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(loc, count, floats);
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(loc, count, false, floats);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(loc, count, false, floats);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(loc, count, false, floats);
		break;
	default:
		LOGGER.logErr("Unknown OpenGL uniform float type %X",
			type);
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

bool COpenGLGPUProgram::setUniformiv(
	E_UNIFORM_TYPE uniform, const s32* integers, u32 size_bytes)
{
	GLint loc = m_Uniforms[uniform].Location;
	if (loc == -1)
	{
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
		return false;
	}

	GLenum type = m_Uniforms[uniform].Type;
	s32 count = m_Uniforms[uniform].Size;
	u32 actual_size = m_Uniforms[uniform].Bytes;

	if (size_bytes != actual_size)
	{
		LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
			getUniformName(uniform), actual_size, size_bytes);
		return false;
	}

	switch (type)
	{
	case GL_INT:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
		glUniform1iv(loc, count, integers);
		break;
	case GL_INT_VEC2:
		glUniform2iv(loc, count, integers);
		break;
	case GL_INT_VEC3:
		glUniform3iv(loc, count, integers);
		break;
	case GL_INT_VEC4:
		glUniform4iv(loc, count, integers);
		break;
	default:
		LOGGER.logErr("Unknown OpenGL uniform integer type %X",
			type);
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

bool COpenGLGPUProgram::setUniform1f(
	E_UNIFORM_TYPE uniform, f32 float_val)
{
	GLint loc = m_Uniforms[uniform].Location;
	if (loc == -1)
	{
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
		return false;
	}
	GLenum type = m_Uniforms[uniform].Type;
	s32 count = m_Uniforms[uniform].Size;
	if (count != 1)
	{
		LOGGER.logErr("Wrong OpenGL uniform float size %d (expected 1)",
			count);
		return false;
	}

	switch (type)
	{
	case GL_FLOAT:
		glUniform1f(loc, float_val);
		break;
	default:
		LOGGER.logErr("Unknown OpenGL uniform float type %X",
			type);
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

bool COpenGLGPUProgram::setUniform1i(
	E_UNIFORM_TYPE uniform, s32 int_val)
{
	GLint loc = m_Uniforms[uniform].Location;
	if (loc == -1)
	{
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
		return false;
	}
	GLenum type = m_Uniforms[uniform].Type;
	s32 count = m_Uniforms[uniform].Size;
	if (count != 1)
	{
		LOGGER.logErr("Wrong OpenGL uniform integer size %d (expected 1)",
			count);
		return false;
	}

	switch (type)
	{
	case GL_INT:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
		glUniform1i(loc, int_val);
		break;
	default:
		LOGGER.logErr("Unknown OpenGL uniform integer type %X",
			type);
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21