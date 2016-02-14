//|-------------------------------------------------------------------------
//| File:        IMeshBuffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMeshBufferHPP
#define IMeshBufferHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <vid/SMaterial.h>
#include <vid/S3DVertex.h>
#include <vid/IRenderBuffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Struct for holding a mesh with a single material
//! SMeshBuffer is a simple implementation of a MeshBuffer.
class __MY_SCN_LIB_API__ IMeshBuffer : public IUnknown
{
public:

	//! constructor 1
	IMeshBuffer() : m_DrawPrimitiveType(vid::EDPT_TRIANGLE_LIST) {}

	//! constructor 2
	IMeshBuffer(vid::E_DRAW_PRIMITIVE_TYPE dpt) : m_DrawPrimitiveType(dpt) {}

    //! destructor
    virtual ~IMeshBuffer() {}

    //! returns the material of this meshbuffer
    virtual vid::SMaterial& getMaterial() = 0;

    //! returns the material of this meshbuffer
    virtual const vid::SMaterial& getMaterial() const = 0;

    //! returns which type of vertex data is stored.
    virtual vid::E_VERTEX_TYPE getVertexType() const = 0;	

	//! reserving memory for vertices
	virtual bool setVerticesUsed(u32 used) { return false; }

	//! adding new vertex to the mesh buffer, return true on success
	virtual bool addVertex(const vid::S3DBaseVertex* vertex, vid::E_VERTEX_TYPE type) { return false; }

	//! adding new vertex to the mesh buffer, return true on success
	virtual bool addVertex(const vid::S3DBaseVertex& vertex, vid::E_VERTEX_TYPE type) { return false; }

	//! returns which type of vertex data is stored.
	virtual const vid::S3DBaseVertex& getVertex(u32 idx) const = 0;
	virtual vid::S3DBaseVertex& getVertex(u32 idx) = 0;
	
    //! returns pointer to vertex data. The data is a array of vertices. Which vertex
    //! type is used can be determinated with getVertexType().
    virtual const void* getVertices() const = 0; 

    //! returns pointer to vertex data. The data is a array of vertices. Which vertex
    //! type is used can be determinated with getVertexType().
    virtual void* getVertices() = 0; 

    //! returns amount of vertices
    virtual s32 getVertexCount() const = 0;

	//! return vertex normal
	virtual const core::vector3df& getVertexNormal(u32 idx) const  = 0;
	virtual core::vector3df& getVertexNormal(u32 idx) = 0;

	//! returns index value
	virtual u32 getIndex(u32 idx) const = 0;

    //! returns pointer to Indices
    virtual const void* getIndices() const = 0;

    //! returns pointer to Indices
    virtual void* getIndices() = 0;

    //! returns amount of indices
    virtual s32 getIndexCount() const = 0;

    //! returns an axis aligned bounding box
    virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

    //! returns an axis aligned bounding box
    virtual core::aabbox3d<f32>& getBoundingBox() = 0;

	//! recalculates the bounding box. should be called if the mesh changed.
    virtual void recalculateBoundingBox() = 0;

	//! rescale normals
	virtual void rescaleMeshBufferNormals(const core::vector3df &newScale)=0;

	//! reserving memory for indices
	virtual bool setIndicesUsed(u32 used) { return false; }

	//! adding index to mesh buffer
	virtual void addIndex(u32 index_value) {}

	virtual vid::E_DRAW_PRIMITIVE_TYPE getDrawPrimitiveType()
	{ return m_DrawPrimitiveType; }

	virtual void clearMeshBufferGeometry() {}

	virtual bool calculateTangents() { return false; }

	virtual bool splitByPlane(
		core::plane3df div_plane, IMeshBuffer *front_mb, IMeshBuffer *back_mb
		) {	return false; }

	//! merge two besh buffer into one
	virtual bool mergeWith(IMeshBuffer *other) { return false; }

	//! returns index size 
	virtual E_INDEX_TYPE getIndexType() = 0;

	virtual void uploadToRenderBuffer(
		vid::IRenderBuffer *rb, bool create_indicies = true)
	{
		// updating render buffer from mesh buffer geometry
		u8 *pVertData = (u8*)rb->getVertices()->lock();
		u8 *pIndData = (u8*)rb->getIndices()->lock();
		u32 vcnt = getVertexCount();
		u32 icnt = getIndexCount();

		if (!create_indicies && icnt &&
				m_DrawPrimitiveType == vid::EDPT_TRIANGLE_LIST)
		{
			u8 *vertices = (u8*)getVertices();
			u32 vsize = vid::VertexSize[getVertexType()];
			rb->getVertices()->setSize(icnt);
			if (getIndexType() == EIT_16_BIT)
			{
				u16 *indices = (u16*)getIndices();
				for (u32 i = 0; i < icnt; i++)
					memcpy(pVertData + i * vsize, vertices + indices[i] * vsize, vsize);
			}
			else
			{
				u32 *indices = (u32*)getIndices();
				for (u32 i = 0; i < icnt; i++)
					memcpy(pVertData + i * vsize, vertices + indices[i] * vsize, vsize);
			}
			rb->getIndices()->setSize(0);
		}
		else
		{
			rb->getVertices()->setSize(vcnt);
			memcpy(pVertData, getVertices(), rb->getVertices()->getDataSize());
			rb->getIndices()->setSize(icnt);
			memcpy(pIndData, getIndices(), rb->getIndices()->getDataSize());
		}
		rb->setDrawPrimitiveType(m_DrawPrimitiveType);
		rb->getVertices()->unlock();
		rb->getIndices()->unlock();
	}

private:

	vid::E_DRAW_PRIMITIVE_TYPE m_DrawPrimitiveType;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

