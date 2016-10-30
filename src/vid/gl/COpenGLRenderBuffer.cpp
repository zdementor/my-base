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

bool COpenGLRenderBuffer::ms_EnabledAttribs[MY_VERTEX_MAX_ATTRIBS] = { 0, 0, 0, 0, 0, 0, 0 };

bool COpenGLVertexArrayObject::ms_VAOSupport = false;

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimple >::_setupOffsets() {}

//----------------------------------------------------------------------------

template <>
void COpenGLVertexArray < S3DVertexSimple > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_VertexOffset),
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertexSimpleColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_VertexOffset),
		GL_NONE,          0, 0,            NULL,
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertex1TCoords > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_NormalOffset),
		GL_NONE,  0, 0,            NULL,
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertex1TCoordsColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_NormalOffset),
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertex2TCoords > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_NormalOffset),
		GL_NONE,  0, 0,            NULL,
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoord2Offset),
		GL_NONE,  0, 0,            NULL,
		GL_NONE,  0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertex2TCoordsColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_NormalOffset),
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoord2Offset),
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL
		);
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
void COpenGLVertexArray < S3DVertex1TCoordsTBN > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_NormalOffset),
		GL_NONE,  0, 0,            NULL,
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_NONE,  0, 0,            NULL,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_TangentOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_BinormalOffset)
		);
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
void COpenGLVertexArray < S3DVertex1TCoordsTBNColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_NormalOffset),
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_NONE,          0, 0,            NULL,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_TangentOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_BinormalOffset)
		);
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
void COpenGLVertexArray < S3DVertex2TCoordsTBN > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT, 3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_NormalOffset),
		GL_NONE,  0, 0,            NULL,
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_FLOAT, 2, m_DataStride, _getPointer(m_TCoord2Offset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_TangentOffset),
		GL_FLOAT, 3, m_DataStride, _getPointer(m_BinormalOffset)
		);
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
void COpenGLVertexArray < S3DVertex2TCoordsTBNColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         3, m_DataStride, _getPointer(m_VertexOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_NormalOffset),
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoord2Offset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_TangentOffset),
		GL_FLOAT,         3, m_DataStride, _getPointer(m_BinormalOffset)
		);
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
void COpenGLVertexArray < S3DVertex1TCoordsRHWColoured > ::_setupPointers(bool *enabledAttribs)
{
	m_Driver->_setupGLAttributes(
		enabledAttribs,
		GL_FLOAT,         4, m_DataStride, _getPointer(m_VertexOffset),
		GL_NONE,          0, 0,            NULL,
		GL_UNSIGNED_BYTE, 4, m_DataStride, _getPointer(m_ColorOffset),
		GL_FLOAT,         2, m_DataStride, _getPointer(m_TCoordOffset),
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL,
		GL_NONE,          0, 0,            NULL
		);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------