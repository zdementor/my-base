//|-------------------------------------------------------------------------
//| File:        CNullGPUProgram.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullGPUProgramHPP
#define CNullGPUProgramHPP
//---------------------------------------------------------------------------

#include <vid/IGPUProgram.h>
#include <vid/SFog.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

struct SUniformLight
{
	SUniformLight()
		: DiffuseColor(0x00000000),
		AmbientColor(0x00000000),
		SpecularColor(0x00000000),
		Position(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	img::SColorf	DiffuseColor;
	img::SColorf	AmbientColor;
	img::SColorf	SpecularColor;
	core::vector3df	Position;
	f32				Radius;
};

class CNullGPUProgram : public IGPUProgram
{
public:

	CNullGPUProgram();

	virtual bool setUniformfv(E_UNIFORM_TYPE uniform,
		const f32* floats, u32 size_bytes) { return true; }
	virtual bool setUniformiv(E_UNIFORM_TYPE uniform,
		const s32* integer, u32 size_bytess) { return true; }
	virtual bool setUniform1f(E_UNIFORM_TYPE uniform, f32 float_val) { return true; }
	virtual bool setUniform1i(E_UNIFORM_TYPE uniform, s32 int_val) { return true; }

	virtual u32 getUniformMask() { return m_UniformMask; }

	virtual bool isOK() { return m_OK; }

	virtual void clear() { _destroyProgram(); }

	virtual bool recreate(u32 uniforms, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

	virtual u32 getLightsCount() { return m_LightsCount; }

	const SFog& getFog() { return m_Fog; }
	void setFog(const SFog &fog) { m_Fog = fog; }

protected:

	virtual void _destroyProgram();

	class CNullDriver *m_Driver;

private:

	u32		m_UniformMask;
	bool	m_OK;
	u32		m_LightsCount;

	SFog m_Fog;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

