//|-------------------------------------------------------------------------
//| File:        COpenGLRenderBuffer.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLRenderBuffer.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimple >::_setupOffsets() {}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimple > ::setPointers()
{
	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimple > ::unsetPointers()
{
	glDisableClientState(GL_VERTEX_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimpleColoured > ::_setupOffsets()
{
	static S3DVertexSimpleColoured vertex;
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimpleColoured > ::setPointers()
{
	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimpleColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoords >::_setupOffsets()
{
	static S3DVertex1TCoords vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoords > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		NULL,
		NULL,
		NULL,
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoords > ::unsetPointers()
{
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsColoured > ::_setupOffsets()
{
	static S3DVertex1TCoordsColoured vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsColoured > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		NULL,
		NULL,
		NULL,
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoords > ::_setupOffsets()
{
	static S3DVertex2TCoords vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_TCoord2Offset = (void*)((u8*)&vertex.TCoords2 - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoords > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		_getPointer(m_TCoord2Offset),
		NULL,
		NULL,
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoords > ::unsetPointers()
{
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsColoured > ::_setupOffsets()
{
	static S3DVertex2TCoordsColoured vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_TCoord2Offset = (void*)((u8*)&vertex.TCoords2 - (u8*)&vertex.Pos);
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsColoured > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		_getPointer(m_TCoord2Offset),
		NULL,
		NULL,
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBN > ::_setupOffsets()
{
	static S3DVertex1TCoordsTBN vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_BinormalOffset = (void*)((u8*)&vertex.Binormal - (u8*)&vertex.Pos);
	m_TangentOffset = (void*)((u8*)&vertex.Tangent - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBN > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		NULL,
		_getPointer(m_TangentOffset),
		_getPointer(m_BinormalOffset),
		m_DataStride);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBN > ::unsetPointers()
{
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBNColoured > ::_setupOffsets()
{
	static S3DVertex2TCoordsTBNColoured vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
	m_BinormalOffset = (void*)((u8*)&vertex.Binormal - (u8*)&vertex.Pos);
	m_TangentOffset = (void*)((u8*)&vertex.Tangent - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBNColoured > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		NULL,
		_getPointer(m_TangentOffset),
		_getPointer(m_BinormalOffset),
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsTBNColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBN > ::_setupOffsets()
{
	static S3DVertex2TCoordsTBN vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_TCoord2Offset = (void*)((u8*)&vertex.TCoords2 - (u8*)&vertex.Pos);
	m_BinormalOffset = (void*)((u8*)&vertex.Binormal - (u8*)&vertex.Pos);
	m_TangentOffset = (void*)((u8*)&vertex.Tangent - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBN > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		_getPointer(m_TCoord2Offset),
		_getPointer(m_TangentOffset),
		_getPointer(m_BinormalOffset),
		m_DataStride);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBN > ::unsetPointers()
{
	glDisableClientState(GL_NORMAL_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBNColoured > ::_setupOffsets()
{
	static S3DVertex2TCoordsTBNColoured vertex;
	m_NormalOffset = (void*)((u8*)&vertex.Normal - (u8*)&vertex.Pos);
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_TCoord2Offset = (void*)((u8*)&vertex.TCoords2 - (u8*)&vertex.Pos);
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
	m_BinormalOffset = (void*)((u8*)&vertex.Binormal - (u8*)&vertex.Pos);
	m_TangentOffset = (void*)((u8*)&vertex.Tangent - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBNColoured > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		_getPointer(m_TCoord2Offset),
		_getPointer(m_TangentOffset),
		_getPointer(m_BinormalOffset),
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, m_DataStride, _getPointer(m_NormalOffset));
	glEnableClientState(GL_NORMAL_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex2TCoordsTBNColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsRHWColoured > ::_setupOffsets()
{
	static S3DVertex1TCoordsRHWColoured vertex;
	m_TCoordOffset = (void*)((u8*)&vertex.TCoords - (u8*)&vertex.Pos);
	m_ColorOffset = (void*)((u8*)&vertex.Color - (u8*)&vertex.Pos);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsRHWColoured > ::setPointers()
{
	m_Driver->_setTextureCoords(
		_getPointer(m_TCoordOffset),
		NULL,
		NULL,
		NULL,
		m_DataStride);

	glVertexPointer(3, GL_FLOAT, m_DataStride, _getPointer(m_VertexOffset));
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, m_DataStride, _getPointer(m_ColorOffset));
	glEnableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertex1TCoordsRHWColoured > ::unsetPointers()
{
	glDisableClientState(GL_COLOR_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);

	m_Driver->_unsetTextureCoords();
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------