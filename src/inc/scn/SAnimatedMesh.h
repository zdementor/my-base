//|-------------------------------------------------------------------------
//| File:        SAnimatedMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SAnimatedMeshHPP
#define SAnimatedMeshHPP
//---------------------------------------------------------------------------

#include <scn/IAnimatedMesh.h>
#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn{
//---------------------------------------------------------------------------

//! Simple implementation of the IAnimatedMesh interface.
struct __MY_SCN_LIB_API__ SAnimatedMesh : public IAnimatedMesh
{
    //! constructor
	SAnimatedMesh() : m_OveralFramesCount(0), m_CurrentFrame(0)
    {
	#if MY_DEBUG_MODE  
		IUnknown::setClassName("SAnimatedMesh");
	#endif

        Type = EMT_UNKNOWN;
    }

    //! destructor
    virtual ~SAnimatedMesh()
    {
        // drop meshes
        for (u32 i=0; i<Meshes.size(); ++i)
            Meshes[i]->drop();
    };

	//! Returns the IMesh interface for a frame.
	virtual IMesh* getMesh() 
    { return getMesh(m_CurrentFrame); }

    //! Returns the IMesh interface for a frame.
	virtual IMesh* getMesh(f32 frame) 
    {
		if ((u32)frame < (u32)Meshes.size())
			return Meshes[(u32)frame];
		return NULL;
    }

	//! adds a Mesh
    void addMesh(IMesh* mesh)
    {
        if (mesh)
        {
            mesh->grab();
            Meshes.push_back(mesh);

			m_OveralFramesCount = Meshes.size();
        }
    }
     
    //! Returns an axis aligned bounding box of the mesh.
    //! \return A bounding box of this mesh is returned.
    virtual const core::aabbox3d<f32>& getBoundingBox() const
    { return Box; }

	//! calc bounding box
    virtual void recalculateBoundingBox()
    {
        Box.reset(0,0,0);

        if (Meshes.empty())
            return;

        Box = Meshes[0]->getBoundingBox();

        for (u32 i=1; i<Meshes.size(); ++i)
            Box.addInternalBox(Meshes[i]->getBoundingBox());                
    }

    //! Returns the type of the animated mesh.
    virtual E_MESH_TYPE getAnimatedMeshType() const
	{ return Type; }

	//! Gets the frame count of the animated mesh.
    virtual u32 getOveralFramesCount() 
	{ return m_OveralFramesCount; }

	//! rescale normals
	virtual void rescaleAnimatedMeshNormals(const core::vector3df &newScale) 
	{
		for (u32 i=0; i<Meshes.size(); i++)
			Meshes[i]->rescaleMeshNormals(newScale);
	}

	virtual bool calculateTangents()
	{
		for (u32 i = 0; i < Meshes.size(); i++)
			Meshes[i]->calculateTangents();
		return true;
	}

    core::aabbox3df Box;
    core::array <IMesh*> Meshes;
    E_MESH_TYPE Type;

	u32 m_OveralFramesCount, m_CurrentFrame;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

