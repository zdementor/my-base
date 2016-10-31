//|-------------------------------------------------------------------------
//| File:        IRenderBuffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IVertexBufferHPP
#define IVertexBufferHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ IVideoBuffer
{
public:

	IVideoBuffer(u32 max_size)
		: m_Bound(false), m_Locked(false),
	m_Size(max_size), m_MaxSize(max_size),
	m_LockedData(NULL)
	{}

	virtual bool bind()
	{
		if (m_Bound || !isOK())
			return false;
		m_Bound = true;
		return true;
	}
	virtual bool unbind()
	{
		if (!m_Bound || !isOK())
			return false;
		m_Bound = false;
		return true;
	}
	bool isBound() 
	{ return m_Bound; }

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode = ERBLM_WRITE_ONLY)
	{
		if (m_Locked || !isOK())
			return NULL;
		m_Locked = true;
		return (void*)~0;
	}
	virtual bool unlock()
	{
		if (!m_Locked || !isOK())
			return false;
		m_Locked = false;
		return true;
	}
	bool isLocked() 
	{ return m_Locked; }

	void setSize(u32 size)
	{
		if (size <= m_MaxSize)
			m_Size = size;
	}

	virtual u32 getTypeSize() = 0;

	virtual u32 getSize ()
	{ return m_Size; }

	virtual u32 getMaxSize ()
	{ return m_MaxSize; }

	virtual u32 getDataSize()
	{ return m_Size * getTypeSize(); }

	virtual u32 getDataMaxSize()
	{ return m_MaxSize * getTypeSize(); }

	virtual void* getLockedData()
	{ return m_LockedData; }

	virtual bool isOK() = 0;

protected:
	bool	m_Bound;
	bool	m_Locked;
	u32		m_Size;
	u32		m_MaxSize;
	void	*m_LockedData;
};

//---------------------------------------------------------------------------

//! Class for holding a hardware vertex buffer
class __MY_VID_LIB_API__ IVertexBuffer : public IVideoBuffer
{
public:
	IVertexBuffer(u32 max_size) : IVideoBuffer(max_size) {}
    virtual E_VERTEX_TYPE getType() = 0;
};

//---------------------------------------------------------------------------

//! Class for holding a hardware index buffer
class __MY_VID_LIB_API__ IIndexBuffer : public IVideoBuffer
{
public:
	IIndexBuffer(u32 max_size) : IVideoBuffer(max_size) {}
	virtual E_INDEX_TYPE getType() = 0;
};

//---------------------------------------------------------------------------

//! Class for holding a hardware render buffer,
//! which consists from vertex and index buffer
class __MY_VID_LIB_API__ IRenderBuffer : public IUnknown
{
public:
	IRenderBuffer(E_DRAW_PRIMITIVE_TYPE dpt)
		: m_DrawPrimitiveType(dpt) {}
	virtual ~IRenderBuffer(){}

	virtual E_RENDER_BUFFER_TYPE getType() = 0;

	virtual IIndexBuffer* getIndices() = 0;
	virtual IVertexBuffer* getVertices() = 0;

	void setDrawPrimitiveType(E_DRAW_PRIMITIVE_TYPE dpt)
	{ m_DrawPrimitiveType = dpt; }

	virtual E_DRAW_PRIMITIVE_TYPE getDrawPrimitiveType()
	{ return m_DrawPrimitiveType; }

	virtual u32 getPrimitiveCount()
	{
		s32 cnt = (s32)getIndices()->getSize();
		if (!cnt)
			cnt = (s32)getVertices()->getSize();
		if (m_DrawPrimitiveType == EDPT_TRIANGLE_LIST)
			cnt /= 3;
		else if (m_DrawPrimitiveType == EDPT_TRIANGLE_FAN ||
			m_DrawPrimitiveType == EDPT_TRIANGLE_STRIP)
		{
			cnt -= 2;
			CHECK_MIN_RANGE(cnt, 0);
		}
		else if (m_DrawPrimitiveType == EDPT_LINE_LIST)
			cnt /= 2;
		else if (m_DrawPrimitiveType == EDPT_LINE_STRIP)
		{
			cnt -= 1;
			CHECK_MIN_RANGE(cnt, 0);
		}
		else if (m_DrawPrimitiveType == EDPT_POINT_LIST)
			;
		else
			cnt = 0;
		return cnt;
	}

protected:
	E_DRAW_PRIMITIVE_TYPE m_DrawPrimitiveType;
};

//---------------------------------------------------------------------------

MY_FORCEINLINE u32 getPrimitiveCountForPrimitiveType(E_DRAW_PRIMITIVE_TYPE pt, u32 vertCount)
{
	switch (pt)
	{
	case EDPT_TRIANGLE_LIST:
		return vertCount / 3;
	case EDPT_TRIANGLE_STRIP:
	case EDPT_TRIANGLE_FAN:
		return (vertCount > 2) ? (vertCount - 2) : 0;
	case EDPT_POINT_LIST:
		return vertCount;
    case EDPT_LINE_LIST:
		return vertCount / 2;
    case EDPT_LINE_STRIP:
		return (vertCount > 1) ? (vertCount - 1) : 0;
	default:
		break;
	}
	return 0;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif