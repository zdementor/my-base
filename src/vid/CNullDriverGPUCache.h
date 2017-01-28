//|-------------------------------------------------------------------------
//| File:        CNullDriverGPUCache.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullDriverGPUCache
#define CNullDriverGPUCache
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

#define PRL_MAX_SHADER_LIGHTS	32
#define PRL_MAX_FFP_LIGHTS		8

//---------------------------------------------------------------------------

class CGPUCache
{
	friend class CNullDriver;
private:
	void clear()
	{
		m_GPUPrograms.clear();
	 	for (u32 k = 0; k < E_RENDER_PATH_COUNT; k++)
		for (u32 i = 0; i < E_VERTEX_TYPE_COUNT; i++)
			for (u32 j = 0; j <= PRL_MAX_SHADER_LIGHTS; j++)
				m_GPUProgramsHash[k][i][j].clear();
		m_GPUProgramsHashByContent.clear();
		m_GPUProgramsHashByFileName.clear();
		m_GPUProgramsHashFileNames.clear();
	}

	core::array      <IGPUProgram*>   m_GPUPrograms;

	core::hash_table <u64, IUnknown*>
		m_GPUProgramsHash[E_RENDER_PATH_COUNT][E_VERTEX_TYPE_COUNT][PRL_MAX_SHADER_LIGHTS + 1];

	core::hash_table <core::stringc, IUnknown*>
		m_GPUProgramsHashByContent, m_GPUProgramsHashByFileName;

	core::hash_table <IGPUProgram*, core::stringc> m_GPUProgramsHashFileNames;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif