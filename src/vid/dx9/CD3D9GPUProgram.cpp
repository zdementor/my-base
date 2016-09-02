//|-------------------------------------------------------------------------
//| File:        CD3D9GPUProgram.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CD3D9GPUProgram.h"

#if (D3DX_SDK_VERSION < 24)
#ifdef _MSC_VER
// to make sure that we dont using DirectX from other SDK version
#pragma comment(linker, "/NODEFAULTLIB:d3dx8.lib")
#pragma comment(lib, "d3dx9.lib")    
#endif
#endif

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

CD3D9GPUProgram::CD3D9GPUProgram(u32 uniforms, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
	: m_VertexShader(NULL), m_PixelShader(NULL)
{
#if MY_DEBUG_MODE 
	setClassName("CD3D9GPUProgram");
#endif

	m_Device = ((CD3D9Driver*)m_Driver)->_getDirect3DDevice();

	for (u32 i = 0; i < E_HLSL_SHADER_TYPE_COUNT; i++)
		m_ConstantsTable[i] = NULL;

	do
	{
		if (!VIDEO_DRIVER.queryFeature(vid::EVDF_SHADER_LANGUAGE))
		{
			LOGGER.logErr("Your video hardware not support HLSL feature");
			break;
		}
		if (!recreate(uniforms, lights_count,
				vertex_shader_ver, vertex_shader,
				pixel_shader_ver, pixel_shader))
			break;
	}
	while (0);
}

//---------------------------------------------------------------------------

CD3D9GPUProgram::~CD3D9GPUProgram()
{
	_destroyProgram();
}

//---------------------------------------------------------------------------

void CD3D9GPUProgram::_destroyProgram()
{
	for (u32 i = 0; i < E_HLSL_SHADER_TYPE_COUNT; i++)
	{
		if (!m_ConstantsTable[i])
			continue;
		m_ConstantsTable[i]->Release();
		m_ConstantsTable[i] = NULL;
	}
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = NULL;
	}
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = NULL;
	}
	CNullGPUProgram::_destroyProgram();
}

//---------------------------------------------------------------------------

bool CD3D9GPUProgram::recreate(u32 uniforms, u32 lights_count,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
	u32 errors = 0;

	_destroyProgram();

	// compiling shaders
	m_VertexShader = _createVertexShader(
		vertex_shader_ver, vertex_shader, &errors);
	m_PixelShader = _createPixelShader(
		pixel_shader_ver, pixel_shader, &errors);
	if (errors > 0)
	{
		_destroyProgram();
		return false;
	}

	// get uniforms information

	for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
	{
		LPD3DXCONSTANTTABLE table = m_ConstantsTable[t];
		if (!table)
			continue;

		// currently we only support top level parameters.
		// Should be enough for the beginning. (TODO)

		D3DXCONSTANTTABLE_DESC tblDesc;
		HRESULT hr = table->GetDesc(&tblDesc);
		if (!FAILED(hr))
		{
			for (int i = 0; i < (int)tblDesc.Constants; ++i)
			{
				D3DXCONSTANT_DESC d;
				UINT n = 1;
				D3DXHANDLE cHndl = table->GetConstant(NULL, i);
				if (!FAILED(table->GetConstantDesc(cHndl, &d, &n)))
				{
					E_UNIFORM_TYPE unitype = VIDEO_DRIVER.getUniformType(d.Name);
					if (unitype != EUT_NONE)
					{
						m_Uniforms[unitype].Index[t] = i;
						m_Uniforms[unitype].Table[t] = table;
						m_Uniforms[unitype].Type[t] =  d.Type;
						if (unitype == vid::EUT_LIGHTING)
						{
							u32 size = d.Bytes / sizeof(SUniformLight);
							if (lights_count != size)
							{
								LOGGER.logWarn("Incorrect lights count in GPU program (%d actual, %d expected)",
									size, lights_count);
								lights_count = size;
							}
						}
						m_Uniforms[unitype].Bytes[t] = d.Bytes;
					}
					else
					{
						LOGGER.logErr("Unknown uniform '%s'", d.Name);
						errors++;
					}
				}
				else
				{
					LOGGER.logErr("Can't get constant %d description for uniform '%s'",
						i, d.Name);
					errors++;
				}
			}
		}
		else
		{
			LOGGER.logErr("Can't get constants description");
			errors++;
		}
	}

	// validating uniforms

	for (u32 i = 0; i < vid::E_UNIFORM_TYPE_COUNT; i++)
	{
		if (uniforms & vid::UniformTypeBits[i])
		{
			u32 noIndex = 0;
			for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
			{
				s32 index = m_Uniforms[i].Index[t];
				LPD3DXCONSTANTTABLE table = m_Uniforms[i].Table[t];

				if (index == -1 || !table)
				{
					noIndex++;
					continue;
				}
			}
			if (noIndex == E_HLSL_SHADER_TYPE_COUNT)
			{
				LOGGER.logErr("Can't validate uniform '%s'",
					getUniformName((E_UNIFORM_TYPE)i));
			}
		}
	}

	if (!errors)
		return CNullGPUProgram::recreate(uniforms, lights_count,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
	return false;
}

//---------------------------------------------------------------------------

static HRESULT MyD3DXCompileShader(
	LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
	LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
	LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader,
	LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable)
{
#if ( D3DX_SDK_VERSION < 24 )
// directly link functions, old d3d sdks didn't try to load external dlls
// when linking to the d3dx9.lib

	// invoke static linked function
	return D3DXCompileShader(
		pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName,
		pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
#else
// try to load shader functions from the dll and print error if failed.

	typedef HRESULT (WINAPI *D3DXCompileShaderFunction)(
		LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
		LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
		LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader,
		LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable);

	static bool LoadFailed = false;
	static D3DXCompileShaderFunction pFn = 0;

	if (!pFn && !LoadFailed)
	{
		// try to load dll
		core::stringc strDllName = "d3dx9_";
		strDllName.sprintf("d3dx9_%d.dll", (int)D3DX_SDK_VERSION);

		HMODULE hMod = LoadLibrary(strDllName.c_str());
		if (hMod)
			pFn = (D3DXCompileShaderFunction)GetProcAddress(hMod, "D3DXCompileShader");
		if (!pFn)
		{
			LoadFailed = true;
			LOGGER.logErr("%s: Could not load shader function D3DXCompileShader from dll, shaders disabled",
				strDllName.c_str());
		}
	}

	if (pFn)
	{
		// call already loaded function
		return (*pFn)(
			pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName,
			pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
	}
	return false;
#endif // D3DX_SDK_VERSION < 24
}

//---------------------------------------------------------------------------

IDirect3DVertexShader9* CD3D9GPUProgram::_createVertexShader(
	E_VERTEX_SHADER_VERSION shader_ver, const c8* shader, u32 *errors)
{
	LPD3DXBUFFER buf = 0;
	LPD3DXBUFFER err = 0;

	DWORD flags = 0;

	// compile without debug info
	HRESULT h = MyD3DXCompileShader(
		shader, (u32)strlen(shader),
		0, // macros
		0, // no includes
		"main", VertexShaderVersionTargetName[shader_ver], flags,
		&buf, &err, &m_ConstantsTable[EHST_VERTEX]);

	if (FAILED(h))
	{
		LOGGER.logErr("HLSL vertex shader compilation failed:");
		if (err)
		{
			LOGGER.logErr((c8*)err->GetBufferPointer());
			err->Release();
			if (buf)
				buf->Release();
		}
		(*errors)++;
		return NULL;
	}

	if (err)
		err->Release();

	if (buf)
	{
		IDirect3DVertexShader9 *sh;
		if (FAILED(m_Device->CreateVertexShader(
				(DWORD*)buf->GetBufferPointer(), &sh)))
		{
			LOGGER.logErr("Could not create HLSL vertex shader.");
			buf->Release();
			(*errors)++;
			return NULL;
		}
		buf->Release();
		return sh;
	}
	(*errors)++;
	return NULL;
}

//---------------------------------------------------------------------------

IDirect3DPixelShader9* CD3D9GPUProgram::_createPixelShader(
	E_PIXEL_SHADER_VERSION shader_ver, const c8* shader, u32 *errors)
{
	LPD3DXBUFFER buf = 0;
	LPD3DXBUFFER err = 0;

	DWORD flags = 0;

	// compile without debug info
	HRESULT h = MyD3DXCompileShader(
		shader, (u32)strlen(shader),
		0, // macros
		0, // no includes
		"main", PixelShaderVersionTargetName[shader_ver], flags,
		&buf, &err, &m_ConstantsTable[EHST_PIXEL]);

	if (FAILED(h))
	{
		LOGGER.logErr("HLSL pixel shader compilation failed:");
		if (err)
		{
			LOGGER.logErr((c8*)err->GetBufferPointer());
			err->Release();
			if (buf)
				buf->Release();
		}
		(*errors)++;
		return NULL;
	}

	if (err)
		err->Release();

	if (buf)
	{
		IDirect3DPixelShader9 *sh;
		if (FAILED(m_Device->CreatePixelShader(
				(DWORD*)buf->GetBufferPointer(), &sh)))
		{
			LOGGER.logErr("Could not create HLSL pixel shader.");
			buf->Release();
			(*errors)++;
			return NULL;
		}
		buf->Release();
		return sh;
	}
	(*errors)++;
	return NULL;
}

//---------------------------------------------------------------------------

bool CD3D9GPUProgram::setUniformfv(
	E_UNIFORM_TYPE uniform, const f32* floats, u32 size_bytes)
{
	u32 noIndex = 0;
	u32 errors = 0;

	for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
	{
		s32 index = m_Uniforms[uniform].Index[t];
		LPD3DXCONSTANTTABLE table = m_Uniforms[uniform].Table[t];

		if (index == -1 || !table)
		{
			noIndex++;
			continue;
		}

		u32 actual_size = m_Uniforms[uniform].Bytes[t];

		if (size_bytes != actual_size)
		{
			errors++;
			LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
				getUniformName(uniform), actual_size, size_bytes);
			continue;
		}

		D3DXPARAMETER_TYPE type = m_Uniforms[uniform].Type[t];

		switch (type)
		{
		case D3DXPT_FLOAT:
			break;
		default:
			errors++;
			LOGGER.logErr("Unknown uniform float type %X",
				type);
			continue;
		}

		D3DXHANDLE cHndl = table->GetConstant(NULL, index);
		if (!cHndl)
		{
			LOGGER.logErr("Uniform to set not found (index=%d)",
				index);
			errors++;
			noIndex++;
			continue;
		}

		HRESULT hr = table->SetFloatArray(m_Device,
			cHndl, floats, size_bytes / sizeof(*floats));
		if (FAILED(hr))
		{
			LOGGER.logErr("Can't set float array for uniform");
			errors++;
			continue;
		}
	}

	if (noIndex == E_HLSL_SHADER_TYPE_COUNT)
	{
		errors++;
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
	}

	return !errors;
}

//---------------------------------------------------------------------------

bool CD3D9GPUProgram::setUniformiv(
	E_UNIFORM_TYPE uniform, const s32* integers, u32 size_bytes)
{
	u32 noIndex = 0;
	u32 errors = 0;

	for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
	{
		s32 index = m_Uniforms[uniform].Index[t];
		LPD3DXCONSTANTTABLE table = m_Uniforms[uniform].Table[t];

		if (index == -1 || !table)
		{
			noIndex++;
			continue;
		}

		u32 actual_size = m_Uniforms[uniform].Bytes[t];

		if (size_bytes != actual_size)
		{
			errors++;
			LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
				getUniformName(uniform), actual_size, size_bytes);
			continue;
		}

		D3DXPARAMETER_TYPE type = m_Uniforms[uniform].Type[t];

		switch (type)
		{
		case D3DXPT_INT:
			break;
		case D3DXPT_SAMPLER:
		case D3DXPT_SAMPLER1D:
		case D3DXPT_SAMPLER2D:
		case D3DXPT_SAMPLER3D:
		case D3DXPT_SAMPLERCUBE:
			continue;
		default:
			errors++;
			LOGGER.logErr("Unknown uniform integer type %X",
				type);
			continue;
		}

		D3DXHANDLE cHndl = table->GetConstant(NULL, index);
		if (!cHndl)
		{
			LOGGER.logErr("Uniform to set not found (index=%d)",
				index);
			errors++;
			noIndex++;
			continue;
		}

		HRESULT hr = table->SetIntArray(m_Device,
			cHndl, integers, size_bytes / sizeof(*integers));
		if (FAILED(hr))
		{
			LOGGER.logErr("Can't set integer array for uniform");
			errors++;
			continue;
		}
	}

	if (noIndex == E_HLSL_SHADER_TYPE_COUNT)
	{
		errors++;
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
	}

	return !errors;
}

//---------------------------------------------------------------------------

bool CD3D9GPUProgram::setUniform1f(E_UNIFORM_TYPE uniform, f32 float_val)
{
	u32 noIndex = 0;
	u32 errors = 0;

	for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
	{
		s32 index = m_Uniforms[uniform].Index[t];
		LPD3DXCONSTANTTABLE table = m_Uniforms[uniform].Table[t];

		if (index == -1 || !table)
		{
			noIndex++;
			continue;
		}

		u32 size_bytes = sizeof(float_val);
		u32 actual_size = m_Uniforms[uniform].Bytes[t];

		if (size_bytes != actual_size)
		{
			errors++;
			LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
				getUniformName(uniform), actual_size, size_bytes);
			continue;
		}

		D3DXPARAMETER_TYPE type = m_Uniforms[uniform].Type[t];

		switch (type)
		{
		case D3DXPT_FLOAT:
			break;
		default:
			errors++;
			LOGGER.logErr("Unknown uniform float type %X",
				type);
			continue;
		}

		D3DXHANDLE cHndl = table->GetConstant(NULL, index);
		if (!cHndl)
		{
			LOGGER.logErr("Uniform to set not found (index=%d)",
				index);
			errors++;
			noIndex++;
			continue;
		}

		HRESULT hr = table->SetFloat(m_Device, cHndl, float_val);
		if (FAILED(hr))
		{
			LOGGER.logErr("Can't set float for uniform");
			errors++;
			continue;
		}
	}

	if (noIndex == E_HLSL_SHADER_TYPE_COUNT)
	{
		errors++;
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
	}

	return !errors;
}

//---------------------------------------------------------------------------

bool CD3D9GPUProgram::setUniform1i(E_UNIFORM_TYPE uniform, s32 int_val)
{
	u32 noIndex = 0;
	u32 errors = 0;

	for (u32 t = 0; t < E_HLSL_SHADER_TYPE_COUNT; t++)
	{
		s32 index = m_Uniforms[uniform].Index[t];
		LPD3DXCONSTANTTABLE table = m_Uniforms[uniform].Table[t];

		if (index == -1 || !table)
		{
			noIndex++;
			continue;
		}

		u32 size_bytes = sizeof(int_val);
		u32 actual_size = m_Uniforms[uniform].Bytes[t];

		if (size_bytes != actual_size)
		{
			errors++;
			LOGGER.logErr("Incorrect uniform '%s' size bytes (actual=%d, expected=%d)",
				getUniformName(uniform), actual_size, size_bytes);
			continue;
		}

		D3DXPARAMETER_TYPE type = m_Uniforms[uniform].Type[t];

		switch (type)
		{
		case D3DXPT_INT:
			break;
		case D3DXPT_SAMPLER:
		case D3DXPT_SAMPLER1D:
		case D3DXPT_SAMPLER2D:
		case D3DXPT_SAMPLER3D:
		case D3DXPT_SAMPLERCUBE:
			continue;
		default:
			errors++;
			LOGGER.logErr("Unknown uniform integer type %X",
				type);
			continue;
		}

		D3DXHANDLE cHndl = table->GetConstant(NULL, index);
		if (!cHndl)
		{
			LOGGER.logErr("Uniform to set not found (index=%d)",
				index);
			errors++;
			noIndex++;
			continue;
		}

		HRESULT hr = table->SetInt(m_Device, cHndl, int_val);
		if (FAILED(hr))
		{
			LOGGER.logErr("Can't set integer for uniform");
			errors++;
			continue;
		}
	}

	if (noIndex == E_HLSL_SHADER_TYPE_COUNT)
	{
		errors++;
		LOGGER.logErr("Can't set uniform '%s', due to unknown location",
			getUniformName(uniform));
	}

	return !errors;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
