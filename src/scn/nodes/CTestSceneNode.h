//|-------------------------------------------------------------------------
//| File:        CTestSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTestSceneNodeHPP
#define CTestSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISceneManager.h>
#include <scn/ITestSceneNode.h>
#include <vid/S3DVertex.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CTestSceneNode : public ITestSceneNode
{
public:

    //! constructor
    CTestSceneNode(
		core::vector3df size, E_GEOM_PRIMITIVE_TYPE geom_type, 
		ISceneNode* parent, s32 id
		);

    //! destructor
    virtual ~CTestSceneNode();

	virtual E_GEOM_PRIMITIVE_TYPE getPrimitiveType();

	virtual void setPrimitiveType(E_GEOM_PRIMITIVE_TYPE type);

	//! check visibility of this node
	virtual void OnCheckCulling();

    virtual void OnPreRender(u32 timeMs);

    //! returns the axis aligned bounding box of this node
    virtual const core::aabbox3d<f32>& getBoundingBox() const;

    //! returns the material based on the zero based index i. To get the amount
    //! of materials used by this scene node, use getMaterialsCount().
    //! This function is needed for inserting the node into the scene hirachy on a
    //! optimal position for minimizing renderstate changes, but can also be used
    //! to directly modify the material of a scene node.
    virtual vid::SMaterial& getMaterial(s32 i);

    //! returns amount of materials used by this scene node.
    virtual s32 getMaterialsCount();

	//!
	virtual bool set(ISceneNode* other);

	//!
	virtual const c8* getSceneCorePropertiesXMLString();

private:

    core::aabbox3d<f32> m_BoundingBox;

    vid::SMaterial Material;

	IAnimatedMesh* m_PrimitiveMesh;

	E_GEOM_PRIMITIVE_TYPE m_GeomPrimitiveType;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

