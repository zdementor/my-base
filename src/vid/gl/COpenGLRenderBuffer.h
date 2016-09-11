//|-------------------------------------------------------------------------
//| File:        COpenGLRenderBuffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLRenderBufferHPP
#define COpenGLRenderBufferHPP
//----------------------------------------------------------------------------

#include "../CNullRenderBuffer.h"
#include "COpenGLDriver.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

template < class T >
class COpenGLVertexArray : public IVertexBuffer
{
public:

	COpenGLVertexArray( COpenGLDriver * driver, s32 size, bool stub = false )
		: IVertexBuffer(size), m_Driver(driver), m_Array(0),
		m_VertexOffset(0), m_NormalOffset(0), m_TCoordOffset(0),
		m_TCoord2Offset(0), m_ColorOffset(0), m_TangentOffset(0),
		m_BinormalOffset(0), m_DataStride(sizeof(T)), m_OwnData(true)
	{
		if (size && !stub)
			m_Array = new T[size];
		_setupOffsets();
	}

	COpenGLVertexArray( COpenGLDriver * driver, const T * array, s32 size, bool own_data = false )
		: IVertexBuffer(size), m_Driver(driver), m_Array(0),
		m_VertexOffset(0), m_NormalOffset(0), m_TCoordOffset(0),
		m_TCoord2Offset(0), m_ColorOffset(0), m_TangentOffset(0),
		m_BinormalOffset(0), m_DataStride(sizeof(T)), m_OwnData(own_data)
	{
		if (size && array)
			m_ConstArray = array;
		_setupOffsets();
	}

	virtual ~COpenGLVertexArray()
	{
		if (m_OwnData)
			SAFE_DELETE_ARRAY(m_Array);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		return m_Array;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		return true;
	}

	virtual E_VERTEX_TYPE getType()
	{ return T::Type; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Array != 0; }

	void setPointers();
	void unsetPointers();

protected:

	void _setupOffsets();

	virtual const void* _getPointer(const void* offset)
	{ return (void*)((size_t)m_Array + (size_t)offset); }

	COpenGLDriver* m_Driver;
	union
	{
		T * m_Array; // Vertex Array Pointer
		const T * m_ConstArray;
	};

private:

	bool m_OwnData;

	void *m_VertexOffset; 
	void *m_NormalOffset; 
	void *m_TCoordOffset; 
	void *m_TCoord2Offset; 
	void *m_ColorOffset; 
	void *m_TangentOffset;
	void *m_BinormalOffset;

	int m_DataStride;
};

//----------------------------------------------------------------------------

template < class T >
class COpenGLIndexArray : public IIndexBuffer
{
public:

	COpenGLIndexArray( COpenGLDriver * driver, s32 size, bool stub = false )
		: IIndexBuffer(size), m_Driver(driver), m_Array(NULL), m_OwnData(true)
	{
		if (size && !stub)
			m_Array = new T[size];
	}

	COpenGLIndexArray( COpenGLDriver * driver, const T * array, s32 size, bool own_data = false )
		: IIndexBuffer(size), m_Driver(driver), m_Array(NULL), m_OwnData(own_data)
	{
		if (size && array)
			m_ConstArray = array;
	}

	virtual ~COpenGLIndexArray()
	{
		if (m_OwnData)
			SAFE_DELETE_ARRAY(m_Array);
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (!IVideoBuffer::lock(mode))
			return NULL;
		return m_Array;
	}

	virtual bool unlock()
	{
		if (!IVideoBuffer::unlock())
			return false;
		return true;
	}

	virtual E_INDEX_TYPE getType()
	{ return (sizeof(T)==2) ? EIT_16_BIT : EIT_32_BIT; }

	virtual u32 getTypeSize()
	{ return sizeof(T); }

	virtual bool isOK()
	{ return m_Array != 0; }

protected:

    COpenGLDriver* m_Driver;
	union
	{
		T * m_Array; // Index Array Pointer
		const T * m_ConstArray;
	};

private:

	bool m_OwnData;
};

//----------------------------------------------------------------------------

template < class T >
class COpenGLVertexBuffer : public COpenGLVertexArray < T >
{
public:

	COpenGLVertexBuffer(bool dynamic, COpenGLDriver *driver, s32 size)
		: COpenGLVertexArray <T> (driver, size, !dynamic),
		m_Dynamic(dynamic), m_Buffer(0)
	{
		if (size)
		{
			try
			{
				glGenBuffersARB(1, &m_Buffer);
				bind();
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, size*sizeof(T), NULL,
					m_Dynamic ? GL_STREAM_DRAW_ARB : GL_STATIC_DRAW_ARB);
				unbind();
			}			
			catch(...)
			{
				m_MaxSize = 0;
			}
		}
	}

	virtual ~COpenGLVertexBuffer()
	{
		if (m_Buffer)
			glDeleteBuffersARB(1, &m_Buffer); 
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (m_Dynamic)
			return COpenGLVertexArray<T>::lock(mode);
		if (!IVideoBuffer::lock(mode))
			return NULL;
		bind();
		return glMapBufferARB(GL_ARRAY_BUFFER_ARB, convertToOGLLockMode(mode));
	}

	virtual bool unlock()
	{
		if (m_Dynamic)
		{
			if (!COpenGLVertexArray<T>::unlock())
				return false;
			bind();
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_Size * sizeof(T), m_Array,
				m_Dynamic ? GL_STREAM_DRAW_ARB : GL_STATIC_DRAW_ARB);
			unbind();
			return true;
		}

		if (!IVideoBuffer::unlock())
			return false;
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		unbind();
		return true;
	}

	void bind()
	{ glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_Buffer); }

	void unbind()
	{ glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); }

	virtual bool isOK()
	{ return m_Buffer != 0; }

protected:

	virtual const void* _getPointer(const void *offset)
	{ return offset; }

private:

	GLuint m_Buffer; // Vertex Buffer Pointer
	bool m_Dynamic;
};

//----------------------------------------------------------------------------

template < class T >
class COpenGLIndexBuffer : public COpenGLIndexArray < T >
{
public:

	COpenGLIndexBuffer(bool dynamic, COpenGLDriver *driver, s32 size )
		: COpenGLIndexArray <T> (driver, size, !dynamic),
		m_Dynamic(dynamic), m_Buffer(0)
	{
		if (size)
		{
			try
			{
				glGenBuffersARB(1, &m_Buffer);
				bind();
				glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size*sizeof(T), NULL,
					m_Dynamic ? GL_STREAM_DRAW_ARB : GL_STATIC_DRAW_ARB);
				unbind();
			}
			catch(...)
			{
				m_MaxSize = 0;
			}
		}
	}

	virtual ~COpenGLIndexBuffer()
	{
		if (m_Buffer)
			glDeleteBuffersARB(1, &m_Buffer); 
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (m_Dynamic)
			return COpenGLIndexArray<T>::lock(mode);

		if (!IVideoBuffer::lock(mode))
			return NULL;
		bind();
		return glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, convertToOGLLockMode(mode));
	}

	virtual bool unlock()
	{
		if (m_Dynamic)
		{
			if (!COpenGLIndexArray<T>::unlock())
				return false;
			bind();
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_Size * sizeof(T), m_Array,
				m_Dynamic ? GL_STREAM_DRAW_ARB : GL_STATIC_DRAW_ARB);
			unbind();
			return true;
		}
		
		if (!IVideoBuffer::unlock())
			return false;
		glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
		unbind();
		return true;
	}

	virtual bool isOK()
	{ return m_Buffer != 0; }

	void bind()
	{ glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_Buffer); }

	void unbind()
	{ glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0); }

private:

	GLuint m_Buffer; // Index Buffer Pointer
	bool m_Dynamic;
};

//----------------------------------------------------------------------------

template <class TVert, class TInd >
class COpenGLRenderArray : public CNullRenderBuffer
{
public:

	COpenGLRenderArray(
		COpenGLDriver *driver,
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : CNullRenderBuffer(dpt),
		m_GLDrawPrimitiveType(convertToOGLPrimitiveType(dpt)),
		m_GLIndicesType((sizeof(TInd)==2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
		m_VertexArray(driver, vert_size),
		m_IndexArray(driver, ind_size)
	{
#if MY_DEBUG_MODE 
		setClassName("COpenGLRenderArray");
#endif
	}

	COpenGLRenderArray(
		COpenGLDriver * driver,
		const TVert * vertices, s32 vert_size,
		const TInd * indices, s32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt,
		bool own_data = false
		) : CNullRenderBuffer(dpt),
		m_GLDrawPrimitiveType(convertToOGLPrimitiveType(dpt)),
		m_GLIndicesType((sizeof(TInd)==2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
		m_VertexArray(driver, vertices, vert_size, own_data),
		m_IndexArray(driver, indices, ind_size, own_data)
	{
#if MY_DEBUG_MODE 
		setClassName("COpenGLRenderArray");
#endif
	}

	virtual ~COpenGLRenderArray()
	{
	}

	virtual E_RENDER_BUFFER_TYPE getType()
	{ return ERBT_DYNAMIC; }

	virtual IVertexBuffer* getVertices()
	{ return &m_VertexArray; }

	virtual IIndexBuffer* getIndices()
	{ return &m_IndexArray; }

	virtual void draw()
	{
		u32 vsize = m_VertexArray.getSize();
		u32 isize = m_IndexArray.getSize();

		m_VertexArray.setPointers();

		if (isize)
		{
			void *indices = m_IndexArray.lock(ERBLM_READ_ONLY);
			if (indices)
				glDrawElements(
					m_GLDrawPrimitiveType, isize, m_GLIndicesType, indices);
			m_IndexArray.unlock();
		}
		else
			glDrawArrays(
				m_GLDrawPrimitiveType, 0, vsize);

		m_VertexArray.unsetPointers();
	}

private:

	COpenGLVertexArray < TVert > m_VertexArray;
	COpenGLIndexArray < TInd > m_IndexArray;

	GLuint m_GLDrawPrimitiveType;
	GLuint m_GLIndicesType;
};

//----------------------------------------------------------------------------

template < class TVert, class TInd >
class COpenGLVertexBufferObject : public CNullRenderBuffer
{
public:

	COpenGLVertexBufferObject(
		bool dynamic,
		vid::COpenGLDriver *driver, 
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : CNullRenderBuffer(dpt),
		m_GLDrawPrimitiveType(convertToOGLPrimitiveType(dpt)),
		m_GLIndicesType((sizeof(TInd)==2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
		m_Dynamic(dynamic),
		m_VertexBuffer(dynamic, driver, vert_size),
		m_IndexBuffer(dynamic, driver, ind_size)
	{
#if MY_DEBUG_MODE 
		setClassName("COpenGLVertexBufferObject");
#endif
		POST_CREATE_VBO("OGL vertex buffer object");
	}

	virtual ~COpenGLVertexBufferObject()
	{
	}

	virtual E_RENDER_BUFFER_TYPE getType()
	{ return m_Dynamic ? ERBT_DYNAMIC : ERBT_STATIC; }

	virtual IVertexBuffer* getVertices()
	{ return &m_VertexBuffer; }

	virtual IIndexBuffer* getIndices()
	{ return &m_IndexBuffer; }

	virtual void draw()
	{
		u32 vsize = m_VertexBuffer.getSize();
		u32 isize = m_IndexBuffer.getSize();

		m_VertexBuffer.bind();
		m_VertexBuffer.setPointers();
		
		if (isize)
		{
			m_IndexBuffer.bind();
			glDrawElements(
				m_GLDrawPrimitiveType, isize, m_GLIndicesType, 0);
			m_IndexBuffer.unbind();
		}
		else
			glDrawArrays(
				m_GLDrawPrimitiveType, 0, vsize);

		m_VertexBuffer.unsetPointers();
		m_VertexBuffer.unbind();
	}

private:

	COpenGLVertexBuffer < TVert > m_VertexBuffer;
	COpenGLIndexBuffer < TInd > m_IndexBuffer;

	GLuint m_GLDrawPrimitiveType;
	GLuint m_GLIndicesType;

	bool m_Dynamic;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef COpenGLRenderBufferHPP
