//|-------------------------------------------------------------------------
//| File:        IMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMeshHPP
#define IMeshHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <scn/IMeshBuffer.h>
#include <vid/IVideoDriver.h>
#include <scn/IAnimatedMeshSceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------	

//! Class for accessing a mesh with multiple mesh buffers.
//! A IMesh is nothing more than a collection of some mesh buffers (IMeshBuffer).
//! SMesh is a simple implementation of an IMesh.
class __MY_SCN_LIB_API__ IMesh : public IUnknown
{
public:

    //! destructor
    virtual ~IMesh(){};

    //! Returns the amount of mesh buffers.
    //! \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh.
    virtual s32 getMeshBuffersCount() = 0;

    //! Returns pointer to a mesh buffer.
    //! \param Zero based index of the mesh buffer. The maximum value is
    //! getMeshBuffersCount() - 1;
    //! \return Returns the pointer to the mesh buffer or 
    //! NULL if there is no such mesh buffer.
    virtual IMeshBuffer* getMeshBuffer(s32 nr) = 0;

    //! Returns an axis aligned bounding box of the mesh.
    //! \return A bounding box of this mesh is returned.
    virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

    //! Returns an axis aligned bounding box of the mesh.
    //! \return A bounding box of this mesh is returned.
    virtual core::aabbox3d<f32>& getBoundingBox() = 0;

	//! adds a Mesh Buffer
    virtual bool addMeshBuffer(IMeshBuffer* buf) = 0;

    //! remove all Mesh Buffers
    virtual void removeMeshBuffers() = 0;

    //! Sets a flag of all contained materials to a new value.
    //! \param flag: Flag to set in all materials.
    //! \param newvalue: New value to set in all materials.
    virtual void setMaterialFlag(vid::E_MATERIAL_FLAG flag, bool newvalue) = 0;

	//! rescale normals
	virtual void rescaleMeshNormals(const core::vector3df &newScale) = 0;

	//! Returns the type of the mesh.
    //! \returns Type of the mesh.
    virtual E_MESH_TYPE getMeshType() const
    {
        return EMT_UNKNOWN;
    }

	//!
	virtual void clearMeshGeometry()
	{
		u32 mbc = getMeshBuffersCount();

		for (u32 i=0; i<mbc; ++i)
			getMeshBuffer(i)->clearMeshBufferGeometry();
	}

	virtual void uploadToRenderBuffers(IAnimatedMeshSceneNode *node, bool create_indices = true)
	{
		u32 mbcnt = (u32)getMeshBuffersCount();
		bool need_recreate = false;
		if (mbcnt != node->getRenderBuffersCount())
			need_recreate = true;
		else
		{
			for (u32 i = 0; i < mbcnt; ++i)
			{
				vid::IRenderBuffer *rb = node->getRenderBuffer(i);
				IMeshBuffer *mb = getMeshBuffer(i);
				vid::E_DRAW_PRIMITIVE_TYPE dpt = mb->getDrawPrimitiveType();
				bool need_indices = (dpt != vid::EDPT_TRIANGLE_LIST || create_indices);
				u32 vcnt = need_indices ? mb->getVertexCount() : mb->getIndexCount();
				u32 icnt = need_indices ? mb->getIndexCount() : 0;
				vid::E_VERTEX_TYPE vtype = mb->getVertexType();
				E_INDEX_TYPE itype = mb->getIndexType();

				if (dpt != rb->getDrawPrimitiveType()
						|| vcnt > rb->getVertices()->getMaxSize()
						|| icnt > rb->getIndices()->getMaxSize()
						|| (vcnt && vtype != rb->getVertices()->getType())
						|| (icnt && itype != rb->getIndices()->getType()))
				{
					need_recreate = true;
					break;
				}
			}
		}
		if (need_recreate)
		{
			node->clearRenderBuffers();
			node->clearMaterials();

			for (u32 i = 0; i < mbcnt; ++i)
			{
				IMeshBuffer *mb = getMeshBuffer(i);
				vid::E_DRAW_PRIMITIVE_TYPE dpt = mb->getDrawPrimitiveType();
				bool need_indices = (dpt != vid::EDPT_TRIANGLE_LIST || create_indices);
				u32 vcnt = need_indices ? mb->getVertexCount() : mb->getIndexCount();
				u32 icnt = need_indices ? mb->getIndexCount() : 0;
				vid::E_VERTEX_TYPE vtype = mb->getVertexType();
				E_INDEX_TYPE itype = mb->getIndexType();
			
				node->addMaterial(mb->getMaterial());
				node->addRenderBuffer(
					VIDEO_DRIVER.createRenderBuffer(
						node->getAnimationOveralFramesCount() == 1 ?
							vid::ERBT_STATIC : vid::ERBT_DYNAMIC,
						vtype, vcnt, itype,	icnt, dpt));
			}
		}
		for (u32 i = 0; i < mbcnt; ++i)
		{
			IMeshBuffer *mb = getMeshBuffer(i);
			vid::E_DRAW_PRIMITIVE_TYPE dpt = mb->getDrawPrimitiveType();
			bool need_indices = (dpt != vid::EDPT_TRIANGLE_LIST || create_indices);
			mb->uploadToRenderBuffer(node->getRenderBuffer(i), need_indices);
		}
	}

	virtual bool calculateTangents() { return false; }

private:

};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef IMeshHPP

