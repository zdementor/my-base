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

	void _setupPointers(bool *enabledAttribs);

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
				glGenBuffers(1, &m_Buffer);
				bind();
				glBufferData(GL_ARRAY_BUFFER, size*sizeof(T), NULL,
					m_Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
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
			glDeleteBuffers(1, &m_Buffer); 
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (m_Dynamic)
			return COpenGLVertexArray<T>::lock(mode);
		if (!IVideoBuffer::lock(mode))
			return NULL;
		bind();
		m_LockedData = glMapBuffer(GL_ARRAY_BUFFER, convertToOGLLockMode(mode));
		if (!m_LockedData)
			unlock();
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (m_Dynamic)
		{
			if (!COpenGLVertexArray<T>::unlock())
				return false;
			bind();
			glBufferData(GL_ARRAY_BUFFER, m_Size * sizeof(T), m_Array,
				m_Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
			unbind();
			m_LockedData = NULL;
			return true;
		}

		if (!IVideoBuffer::unlock())
			return false;
		glUnmapBuffer(GL_ARRAY_BUFFER);
		unbind();
		m_LockedData = NULL;
		return true;
	}

	virtual bool bind()
	{
		if (!IVideoBuffer::bind())
			return false;
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		return true;
	}

	virtual bool unbind()
	{
		if (!IVideoBuffer::unbind())
			return false;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return true;
	}

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
				glGenBuffers(1, &m_Buffer);
				bind();
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(T), NULL,
					m_Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
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
			glDeleteBuffers(1, &m_Buffer); 
	}

	virtual void* lock(E_RENDER_BUFFER_LOCK_MODE mode)
	{
		if (m_Dynamic)
			return COpenGLIndexArray<T>::lock(mode);

		if (!IVideoBuffer::lock(mode))
			return NULL;
		bind();
		m_LockedData = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, convertToOGLLockMode(mode));
		if (!m_LockedData)
			unlock();
		return m_LockedData;
	}

	virtual bool unlock()
	{
		if (m_Dynamic)
		{
			if (!COpenGLIndexArray<T>::unlock())
				return false;
			bind();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size * sizeof(T), m_Array,
				m_Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
			unbind();
			m_LockedData = NULL;
			return true;
		}
		
		if (!IVideoBuffer::unlock())
			return false;
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		unbind();
		m_LockedData = NULL;
		return true;
	}

	virtual bool isOK()
	{ return m_Buffer != 0; }

	virtual bool bind()
	{
		if (!IVideoBuffer::bind())
			return false;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
		return true;
	}

	virtual bool unbind()
	{
		if (!IVideoBuffer::unbind())
			return false;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return true;
	}

private:

	GLuint m_Buffer; // Index Buffer Pointer
	bool m_Dynamic;
};

//----------------------------------------------------------------------------

class COpenGLRenderBuffer : public CNullRenderBuffer
{
public:

	COpenGLRenderBuffer(E_DRAW_PRIMITIVE_TYPE dpt)
		: CNullRenderBuffer(dpt)
	{
	}
	virtual ~COpenGLRenderBuffer()
	{
	}

	static bool ms_EnabledAttribs[MY_VERTEX_MAX_ATTRIBS];

protected:

};

//----------------------------------------------------------------------------

template <class TVert, class TInd >
class COpenGLRenderArray : public COpenGLRenderBuffer
{
public:

	COpenGLRenderArray(
		COpenGLDriver *driver,
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : COpenGLRenderBuffer(dpt),
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
		) : COpenGLRenderBuffer(dpt),
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

	virtual bool bind()
	{
		u32 isize = m_IndexArray.getSize();

		m_VertexArray._setupPointers(ms_EnabledAttribs);
		
		if (isize)
			m_IndexArray.lock(ERBLM_READ_ONLY);

		return true;
	}

	virtual void render()
	{
		if (m_IndexArray.isLocked())
		{
			void *indices = m_IndexArray.getLockedData();
			if (indices)
			{
				u32 isize = m_IndexArray.getSize();
				glDrawElements(
					m_GLDrawPrimitiveType, isize, m_GLIndicesType, indices);
			}
		}
		else
		{
			u32 vsize = m_VertexArray.getSize();
			glDrawArrays(
				m_GLDrawPrimitiveType, 0, vsize);
		}
	}

	virtual bool unbind()
	{
		if (m_IndexArray.isLocked())
			m_IndexArray.unlock();

		return true;
	}

private:

	COpenGLVertexArray < TVert > m_VertexArray;
	COpenGLIndexArray < TInd > m_IndexArray;

	GLuint m_GLDrawPrimitiveType;
	GLuint m_GLIndicesType;
};

//----------------------------------------------------------------------------

class COpenGLVertexArrayObject : public COpenGLRenderBuffer
{
public:

	COpenGLVertexArrayObject(E_DRAW_PRIMITIVE_TYPE dpt)
		: COpenGLRenderBuffer(dpt), m_VAO(0), m_VAOCompleted(false)
	{
		if (ms_VAOSupport)
		{
#if GL_ARB_vertex_array_object
			glGenVertexArrays(1, &m_VAO);
#endif
		}
	}
	virtual ~COpenGLVertexArrayObject()
	{
		if (m_VAO)
		{
#if GL_ARB_vertex_array_object
			glDeleteVertexArrays(1, &m_VAO);
#endif
			m_VAO = 0;
		}
	}

	static bool ms_VAOSupport;

protected:

	GLuint m_VAO;
	bool m_VAOCompleted;
};

//----------------------------------------------------------------------------

template < class TVert, class TInd >
class COpenGLVertexBufferObject : public COpenGLVertexArrayObject
{
public:

	COpenGLVertexBufferObject(
		bool dynamic,
		vid::COpenGLDriver *driver, 
		s32 vert_size, s32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt
		) : COpenGLVertexArrayObject(dpt),
		m_GLDrawPrimitiveType(convertToOGLPrimitiveType(dpt)),
		m_GLIndicesType((sizeof(TInd)==2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
		m_Dynamic(dynamic),
		m_VertexBuffer(dynamic, driver, vert_size),
		m_IndexBuffer(dynamic, driver, ind_size)
	{
#if MY_DEBUG_MODE 
		setClassName("COpenGLVertexBufferObject");
#endif
		memset(m_EnabledAttribs, 0, sizeof(m_EnabledAttribs));

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

	virtual bool bind()
	{
		u32 vsize = m_VertexBuffer.getSize();
		u32 isize = m_IndexBuffer.getSize();

		if (m_VAO)
		{
#if GL_ARB_vertex_array_object
			glBindVertexArray(m_VAO);
			if (!m_VAOCompleted)
			{
				m_VAOCompleted = true;
				if (vsize)
					m_VertexBuffer.bind();
				m_VertexBuffer._setupPointers(m_EnabledAttribs);
				if (isize)
					m_IndexBuffer.bind();
			}
#endif
		}
		else
		{
			if (vsize)
				m_VertexBuffer.bind();
			m_VertexBuffer._setupPointers(ms_EnabledAttribs);
			if (isize)
				m_IndexBuffer.bind();
		}

		return true;
	}
	
	virtual void render()
	{
		u32 vsize = m_VertexBuffer.getSize();
		u32 isize = m_IndexBuffer.getSize();

		if (vsize)
		{
			if (isize)
			{
				glDrawElements(
					m_GLDrawPrimitiveType, isize, m_GLIndicesType, 0);
			}
			else
			{
				glDrawArrays(
					m_GLDrawPrimitiveType, 0, vsize);
			}
		}
	}

	virtual bool unbind()
	{
		if (m_VAO)
		{
#if GL_ARB_vertex_array_object
			glBindVertexArray(0);
#endif
		}
		if (m_IndexBuffer.isBound())
			m_IndexBuffer.unbind();
		if (m_VertexBuffer.isBound())
			m_VertexBuffer.unbind();

		return true;
	}

private:

	COpenGLVertexBuffer < TVert > m_VertexBuffer;
	COpenGLIndexBuffer < TInd > m_IndexBuffer;

	GLuint m_GLDrawPrimitiveType;
	GLuint m_GLIndicesType;

	bool m_Dynamic;

	bool m_EnabledAttribs[MY_VERTEX_MAX_ATTRIBS];
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef COpenGLRenderBufferHPP
