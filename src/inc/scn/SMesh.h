//|-------------------------------------------------------------------------
//| File:        SMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SMeshHPP
#define SMeshHPP
//---------------------------------------------------------------------------

#include <scn/IMeshBuffer.h>
#include <scn/IMesh.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Simple implementation of the IMesh interface.
struct __MY_SCN_LIB_API__ SMesh : public IMesh
{
    //! constructor
	SMesh() : TangentsCalulated(false)
    {
	#if MY_DEBUG_MODE  
		IUnknown::setClassName("SMesh");
	#endif

		Type = EMT_UNKNOWN;
    }

    //! destructor
    ~SMesh()
    {
        // drop buffers
        removeMeshBuffers();
    };

    //! returns amount of mesh buffers.
    virtual s32 getMeshBuffersCount()
    {
        return MeshBuffers.size();
    }

    //! returns pointer to a mesh buffer
    virtual IMeshBuffer* getMeshBuffer(s32 nr)
    {
        return MeshBuffers[nr];
    }

    //! returns an axis aligned bounding box
    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return BoundingBox;
    }  

    //! Returns an axis aligned bounding box of the mesh.
    //! \return A bounding box of this mesh is returned.
    virtual core::aabbox3d<f32>& getBoundingBox()
    {
        return BoundingBox;
    }

    //! recalculates the bounding box
    void recalculateBoundingBox()
    {
        if (MeshBuffers.size())
        {
            BoundingBox = MeshBuffers[0]->getBoundingBox();
            for (u32 i=1; i<MeshBuffers.size(); ++i)
                BoundingBox.addInternalBox(MeshBuffers[i]->getBoundingBox());
        }
        else
            BoundingBox.reset(0.0f, 0.0f, 0.0f);
    }

    //! adds a Mesh Buffer
    virtual bool addMeshBuffer(IMeshBuffer* buf)
    {
        if (buf)
        {
            buf->grab();
            MeshBuffers.push_back(buf);

			return true;
        }

		return false;
    }

    //! remove all Mesh Buffers
    virtual void removeMeshBuffers()
    {
		// drop buffers
        for (u32 i=0; i<MeshBuffers.size(); ++i)
		{
            SAFE_DROP(MeshBuffers[i]);
		}
		MeshBuffers.clear();
    }

    //! sets a flag of all contained materials to a new value
    virtual void setMaterialFlag(vid::E_MATERIAL_FLAG flag, bool newvalue)
    {
        for (u32 i=0; i<MeshBuffers.size(); ++i)
		{
            MeshBuffers[i]->getMaterial().setFlag(flag, newvalue);
		}
    }

	//! rescale normals
	virtual void rescaleMeshNormals(const core::vector3df &newScale) 
	{
		for (u32 i=0; i<MeshBuffers.size(); ++i)
		{
			MeshBuffers[i]->rescaleMeshBufferNormals(newScale);
		}
	}

	//! Returns the type of the animated mesh.
    virtual E_MESH_TYPE getMeshType() const
	{
	    return Type;
    }

	virtual bool calculateTangents()
	{
		if (!TangentsCalulated)
			for (u32 i=0; i<MeshBuffers.size(); ++i)
				MeshBuffers[i]->calculateTangents();
		return true;
	}

    core::array<IMeshBuffer*> MeshBuffers;
    core::aabbox3d<f32> BoundingBox;

	E_MESH_TYPE Type;

	bool TangentsCalulated;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

