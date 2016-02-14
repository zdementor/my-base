//|-------------------------------------------------------------------------
//| File:        CNullRenderBuffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullRenderBufferHPP
#define CNullRenderBufferHPP
//----------------------------------------------------------------------------

#include <vid/IRenderBuffer.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

class CNullRenderBuffer : public IRenderBuffer
{
public:

	CNullRenderBuffer(E_DRAW_PRIMITIVE_TYPE dpt)
		: IRenderBuffer(dpt)
	{
		m_Count++;
	}

	virtual ~CNullRenderBuffer()
	{
		m_Count--;
	}

	virtual void draw() = 0;

	static s32 m_Count;
};

//----------------------------------------------------------------------------

#define PRE_CREATE_VBO(name)

#define POST_CREATE_VBO(name) \
	if (m_VertexBuffer->getMaxSize() != vert_size || m_IndexBuffer->getMaxSize() != ind_size ) \
		LOGGER.logErr("Can't create %s (tri=%d vert=%d type='%s' isize=%dbit)", \
			name, ind_size/3, vert_size, VertexTypeName[m_VertexBuffer->getType()], sizeof(TInd)*8)

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CNullRenderBuffer
