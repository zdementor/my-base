//|-------------------------------------------------------------------------
//| File:        CD3D9RenderBuffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9RenderBufferHPP
#define CD3D9RenderBufferHPP
//----------------------------------------------------------------------------

#include "../CNullRenderBuffer.h"
#include <d3d/d3d9.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

template <class T>
class CD3D9VertexArray : public IVertexBuffer
{
public:

	CD3D9VertexArray(IDirect3DDevice9 * device, s32 size)
		: IVertexBuffer(size), m_Device(device), m_Array(0), m_OwnData(true)
	{
		if (size)
			m_Array = new T[size];
	}

	CD3D9VertexArray(IDirect3DDevice9 * device, const T * array, s32 size, bool own_data)
		: IVertexBuffer(size), m_Device(device), m_Array(0), m_OwnData(own_data)
	{
		if (size && array)
			m_ConstArray = array;
	}

	virtual ~CD3D9VertexArray ()
	{
		if (m_OwnData)
			SAFE_DELETE_ARRAY(m_Array);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		m_LockedData = m_Array;
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		m_LockedData = NULL;
		return true;
	}

	virtual E_VERTEX_TYPE getType()
	{ return T::Type; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Array != 0; }

protected:

	IDirect3DDevice9 * m_Device;

private:

	bool m_OwnData;
	union
	{
		T * m_Array; // Vertex Array Pointer
		const T * m_ConstArray;
	};
};

//----------------------------------------------------------------------------

template < class T >
class CD3D9IndexArray : public IIndexBuffer
{
public:

	CD3D9IndexArray(IDirect3DDevice9 * device, s32 size)
		: IIndexBuffer(size), m_Device(device), m_Array(NULL), m_OwnData(true)
	{
		if (size)
			m_Array = new T[size];
	}

	CD3D9IndexArray(IDirect3DDevice9 * device, const T * array, s32 size, bool own_data = false)
		: IIndexBuffer(size), m_Device(device), m_Array(NULL), m_OwnData(own_data)
	{
		if (size && array)
			m_ConstArray = array;
	}

	virtual ~CD3D9IndexArray()
	{
		if (m_OwnData)
			SAFE_DELETE_ARRAY(m_Array);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		m_LockedData = m_Array;
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		m_LockedData = NULL;
		return true;
	}

	virtual E_INDEX_TYPE getType()
	{ return (sizeof(T)==2) ? EIT_16_BIT : EIT_32_BIT; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Array != 0; }

protected:

    IDirect3DDevice9* m_Device;

private:

	bool m_OwnData;
	union
	{
		T *  m_Array; // Index Array Pointer
		const T * m_ConstArray;
	};
};

//----------------------------------------------------------------------------

template < class T >
class CD3D9VertexBuffer : public IVertexBuffer
{
public:

	CD3D9VertexBuffer( IDirect3DDevice9* device, s32 size )
		: IVertexBuffer(size), m_Device(device), m_Buffer(NULL)
	{
		if (size)
		{
			try
			{
				m_Device->CreateVertexBuffer(
					size * sizeof(T), D3DUSAGE_WRITEONLY,	
					convertToD3D9VertexType(T::Type),
					D3DPOOL_DEFAULT, &m_Buffer, NULL);
			}
			catch(...)
			{
				m_MaxSize = 0;
			}
		}
	}

	virtual ~CD3D9VertexBuffer()
	{
		SAFE_RELEASE(m_Buffer);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		HRESULT hr = m_Buffer->Lock(0, sizeof(m_LockedData), (void**)&m_LockedData, 0);
		if (FAILED(hr) || !m_LockedData)
			unlock();
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		m_Buffer->Unlock();
		m_LockedData = NULL;
		return true;
	}

	virtual E_VERTEX_TYPE getType()
	{ return T::Type; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Buffer != NULL; }

	virtual bool bind()
	{
		if (!IVideoBuffer::bind())
			return false;
		m_Device->SetStreamSource(0, m_Buffer, 0, sizeof(T));
		return true;
	}

	virtual bool unbind()
	{
		if (!IVideoBuffer::unbind())
			return false;
		m_Device->SetStreamSource(0, 0, 0, 0);
		return true;
	}

private:

	IDirect3DVertexBuffer9* m_Buffer; // Vertex Buffer Pointer
	IDirect3DDevice9* m_Device;
};

//----------------------------------------------------------------------------

template < class T >
class CD3D9IndexBuffer : public IIndexBuffer
{
public:

	CD3D9IndexBuffer( IDirect3DDevice9* device, s32 size )
		: IIndexBuffer(size), m_Device(device), m_Buffer(NULL)
	{
		if (size)
		{
			try
			{
				m_Device->CreateIndexBuffer(
					size * sizeof(T), D3DUSAGE_WRITEONLY, 
					(sizeof(T)==2) ?  D3DFMT_INDEX16 : D3DFMT_INDEX32,
					D3DPOOL_DEFAULT, &m_Buffer, NULL);
			}
			catch(...)
			{
				m_MaxSize = 0;
			}
		}
	}

	virtual ~CD3D9IndexBuffer()
	{
		SAFE_RELEASE(m_Buffer);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		HRESULT hr = m_Buffer->Lock(0, sizeof(m_LockedData), (void**)&m_LockedData, 0);
		if (FAILED(hr) || !m_LockedData)
			unlock();
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		m_Buffer->Unlock();
		m_LockedData = NULL;
		return true;
	}

	virtual E_INDEX_TYPE getType()
	{ return (sizeof(T)==2) ? EIT_16_BIT : EIT_32_BIT; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Buffer != NULL; }

	virtual bool bind()
	{
		if (!IVideoBuffer::bind())
			return false;
		m_Device->SetIndices(m_Buffer);
		return true;
	}

	virtual bool unbind()
	{
		if (!IVideoBuffer::unbind())
			return false;
		m_Device->SetIndices(0);
		return true;
	}

private:

	IDirect3DIndexBuffer9* m_Buffer; // Index Buffer Pointer
	IDirect3DDevice9* m_Device;
};

//----------------------------------------------------------------------------

template <class TVert, class TInd >
class CD3D9RenderArray : public CNullRenderBuffer
{
public:

	CD3D9RenderArray(
		IDirect3DDevice9 * device,
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : CNullRenderBuffer(dpt), m_Device(device),
		m_D3DDrawPrimitiveType(convertToD3D9PrimitiveType(dpt)),
		m_D3DIndicesType((sizeof(TInd)==2) ?  D3DFMT_INDEX16 : D3DFMT_INDEX32),
		m_VertexArray(device, vert_size),
		m_IndexArray(device, ind_size)
	{
#if MY_DEBUG_MODE 
		setClassName("CD3D9RenderArray");
#endif
	}

	CD3D9RenderArray(
		IDirect3DDevice9 * device,
		const TVert * vertices, s32 vert_size,
		const TInd * indices, s32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt,
		bool own_data = false
		) : CNullRenderBuffer(dpt), m_Device(device),
		m_D3DDrawPrimitiveType(convertToD3D9PrimitiveType(dpt)),
		m_D3DIndicesType((sizeof(TInd)==2) ?  D3DFMT_INDEX16 : D3DFMT_INDEX32),
		m_VertexArray(device, vertices, vert_size, own_data),
		m_IndexArray(device, indices, ind_size, own_data)
	{
#if MY_DEBUG_MODE 
		setClassName("CD3D9RenderArray");
#endif
	}

	virtual ~CD3D9RenderArray()
	{
	}

	virtual E_RENDER_BUFFER_TYPE getType()
	{ return ERBT_DYNAMIC; }

	virtual IVertexBuffer * getVertices()
	{ return &m_VertexArray; }

	virtual IIndexBuffer * getIndices()
	{ return &m_IndexArray; }

	virtual bool bind()
	{
		u32 vsize = m_VertexArray.getSize();
		u32 isize = m_IndexArray.getSize();

		if (vsize)
			m_VertexArray.lock(ERBLM_READ_ONLY);
		if (isize)
			m_IndexArray.lock(ERBLM_READ_ONLY);

		return true;
	}

	virtual void render()
	{
		void *vertices = NULL;
		if (m_VertexArray.isLocked())
			vertices = m_VertexArray.getLockedData();

		void *indices = NULL;
		if (m_IndexArray.isLocked())
			indices = m_IndexArray.getLockedData();

		if (vertices)
		{
			u32 vsize = m_VertexArray.getSize();
			if (indices)
			{
				u32 isize = m_IndexArray.getSize();
				u32 primCount = getPrimitiveCountForPrimitiveType(m_DrawPrimitiveType, isize);
				m_Device->DrawIndexedPrimitiveUP(
					m_D3DDrawPrimitiveType, 0, vsize, primCount,
					indices, m_D3DIndicesType, vertices, sizeof(TVert));
			}
			else
			{
				u32 primCount = getPrimitiveCountForPrimitiveType(m_DrawPrimitiveType, vsize);
				m_Device->DrawPrimitiveUP(
					m_D3DDrawPrimitiveType, primCount, vertices, sizeof(TVert));
			}
		}
	}

	virtual bool unbind()
	{
		if (m_IndexArray.isLocked())
			m_IndexArray.unlock();
		if (m_VertexArray.isLocked())
			m_VertexArray.unlock();

		return true;
	}

private:

	IDirect3DDevice9 *m_Device;

	CD3D9VertexArray < TVert > m_VertexArray;
	CD3D9IndexArray < TInd > m_IndexArray;

	D3DPRIMITIVETYPE	m_D3DDrawPrimitiveType;
	D3DFORMAT			m_D3DIndicesType;
};

//----------------------------------------------------------------------------

template <class TVert, class TInd>
class CD3D9VertexBufferObject : public CNullRenderBuffer
{
public:

	CD3D9VertexBufferObject(
		IDirect3DDevice9 * device,
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : CNullRenderBuffer(dpt), m_Device(device),
		m_D3DDrawPrimitiveType(convertToD3D9PrimitiveType(dpt)),
		m_D3DIndicesType((sizeof(TInd)==2) ?  D3DFMT_INDEX16 : D3DFMT_INDEX32),
		m_VertexBuffer(device, vert_size),
		m_IndexBuffer(device, ind_size)
	{
	#if MY_DEBUG_MODE 
		setClassName("CD3D9VertexBufferObject");
	#endif
		POST_CREATE_VBO("D3D9 vertex buffer object");
	}

	virtual ~CD3D9VertexBufferObject()
	{
	}

	virtual E_RENDER_BUFFER_TYPE getType()
	{ return ERBT_STATIC; }

	virtual IVertexBuffer *getVertices()
	{ return &m_VertexBuffer; }

	virtual IIndexBuffer *getIndices()
	{ return &m_IndexBuffer; }

	virtual bool bind()
	{
		u32 vsize = m_VertexBuffer.getSize();
		u32 isize = m_IndexBuffer.getSize();

		if (vsize)
			m_VertexBuffer.bind();
		if (isize)
			m_IndexBuffer.bind();

		return true;
	}

	virtual void render()
	{
		if (m_VertexBuffer.isBound())
		{
			u32 vsize = m_VertexBuffer.getSize();

			if (m_IndexBuffer.isBound())
			{
				u32 isize = m_IndexBuffer.getSize();
				u32 primCount = getPrimitiveCountForPrimitiveType(m_DrawPrimitiveType, isize);
				m_Device->DrawIndexedPrimitive(
					m_D3DDrawPrimitiveType, 0, 0, vsize, 0, primCount);
			}
			else
			{
				u32 primCount = getPrimitiveCountForPrimitiveType(m_DrawPrimitiveType, vsize);
				m_Device->DrawPrimitive(
					m_D3DDrawPrimitiveType, 0, primCount);
			}
		}
	}

	virtual bool unbind()
	{
		if (m_VertexBuffer.isBound())
			m_VertexBuffer.unbind();
		if (m_IndexBuffer.isBound())
			m_IndexBuffer.unbind();

		return true;
	}

private:

	CD3D9VertexBuffer < TVert > m_VertexBuffer;
	CD3D9IndexBuffer < TInd > m_IndexBuffer;

	IDirect3DDevice9 * m_Device;

	D3DPRIMITIVETYPE	m_D3DDrawPrimitiveType;
	D3DFORMAT			m_D3DIndicesType;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CD3D9RenderBufferHPP
