//|-------------------------------------------------------------------------
//| File:        CD3D9GPUProgram.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9GPUProgramHPP
#define CD3D9GPUProgramHPP
//---------------------------------------------------------------------------

#include "../CNullGPUProgram.h"
#include "CD3D9Driver.h"

#include <d3d/d3dx9.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

enum E_HLSL_SHADER_TYPE
{
	EHST_VERTEX = 0,
	EHST_PIXEL,

	E_HLSL_SHADER_TYPE_COUNT
};

class CD3D9GPUProgram : public CNullGPUProgram
{
public:

	CD3D9GPUProgram(u32 uniforms, u32 attributes, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);
	virtual ~CD3D9GPUProgram();

	virtual void* getHardwareHandle() const { return (void*)0; }
	virtual bool setUniformfv(E_UNIFORM_TYPE uniform,
		const f32* floats, u32 size_bytes);
	virtual bool setUniformiv(E_UNIFORM_TYPE uniform,
		const s32* integers, u32 size_bytes);
	virtual bool setUniform1f(E_UNIFORM_TYPE uniform,	f32 float_val);
	virtual bool setUniform1i(E_UNIFORM_TYPE uniform, s32 int_val);

	virtual bool recreate(u32 uniforms, u32 attributes, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

	IDirect3DVertexShader9* _getVertexShaderHandle() const { return m_VertexShader; }
	IDirect3DPixelShader9* _getPixelShaderHandle() const { return m_PixelShader; }

private:

	virtual void _destroyProgram();

	IDirect3DVertexShader9* _createVertexShader(
		E_VERTEX_SHADER_VERSION shader_ver, const c8 *shader, u32 *errors);
	IDirect3DPixelShader9* _createPixelShader(
		E_PIXEL_SHADER_VERSION shader_ver, const c8 *shader, u32 *errors);

	IDirect3DVertexShader9 *m_VertexShader;
	IDirect3DPixelShader9 *m_PixelShader;

	LPD3DXCONSTANTTABLE m_ConstantsTable[E_HLSL_SHADER_TYPE_COUNT];

	IDirect3DDevice9	*m_Device;

	struct SUniformHLSL
	{
		SUniformHLSL()
		{
			for (u32 i = 0; i < E_HLSL_SHADER_TYPE_COUNT; i++)
			{
				Table[i] = NULL;
				Index[i] = -1;
				Type[i]  = (D3DXPARAMETER_TYPE)-1;
				Bytes[i] = 0;
			}
		}
		LPD3DXCONSTANTTABLE Table[E_HLSL_SHADER_TYPE_COUNT];
		s32					Index[E_HLSL_SHADER_TYPE_COUNT];		
		D3DXPARAMETER_TYPE	Type[E_HLSL_SHADER_TYPE_COUNT];
		u32					Bytes[E_HLSL_SHADER_TYPE_COUNT];
	}
	m_Uniforms[vid::E_UNIFORM_TYPE_COUNT];
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

