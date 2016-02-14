//|-------------------------------------------------------------------------
//| File:        IDummyTransformationSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IDummyTransformationSceneNodeHPP
#define IDummyTransformationSceneNodeHPP
//----------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Dummy scene node for adding additional transformations to the scene graph.
//! This scene node does not render itself, and does not respond to set/getPosition,
//! set/getRotation and set/getScale. Its just a simple scene node that takes a 
//! matrix as relative transformation, making it possible to insert any transformation
//! anywhere into the scene graph.
//! This scene node is for example used by the IAnimatedSceneNode for emulating
//! joint scene nodes when playing skeletal animations.
class __MY_SCN_LIB_API__ IDummyTransformationSceneNode : public ISceneNode
{
public:

    //! constructor
    IDummyTransformationSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id)
        : ISceneNode(type, parent, id) {}

    //! setting the current relative transformation matrix.
    //! This is the matrix, this scene node uses instead of scale, translation
    //! and rotation.
    virtual void setRelativeTransformation(const core::matrix4& m) = 0;

    //! Returns a reference to the current bounding box of this scene node
    virtual core::aabbox3df& getBoundingBox() = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

