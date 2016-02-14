//|-------------------------------------------------------------------------
//| File:        CDummyTransformationSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDummyTransformationSceneNodeHPP
#define CDummyTransformationSceneNodeHPP
//----------------------------------------------------------------------------

#include <scn/IDummyTransformationSceneNode.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CDummyTransformationSceneNode : public IDummyTransformationSceneNode
{
public:

    //! constructor
    CDummyTransformationSceneNode(ISceneNode* parent, s32 id);

    //! destructor
    ~CDummyTransformationSceneNode();

    //! returns the axis aligned bounding box of this node
    virtual const core::aabbox3d<f32>& getBoundingBox() const
	{ return m_Box; }

    //! returns the material based on the zero based index i
    virtual vid::SMaterial& getMaterial(s32 i)
	{ return m_Material; }
    
    //! returns amount of materials used by this scene node.
    virtual s32 getMaterialsCount()
	{ return 1; }

    //! setting the current relative transformation matrix.
    //! This is the matrix, this scene node uses instead of scale, translation
    //! and rotation.
    virtual void setRelativeTransformation(const core::matrix4& m);

	//! Returns a reference to the current bounding box of this scene node
    virtual core::aabbox3df& getBoundingBox()
	{ return m_Box; }

	//! Sets the scale of the node.
	virtual void setScale(const core::vector3df& scale);
    
    //! Sets the rotation of the node.
    virtual void setRotation(const core::vector3df& rotation, bool controlled_by_ai = false);
    
    //! Sets the position of the node.
    virtual void setPosition(const core::vector3df& newpos, bool controlled_by_ai = false);

	//! Gets the rotation of the node.
    virtual const core::vector3df& getRotation() const
	{ return m_Rot = RelativeTransformation.getRotationDegrees(); }

	//! Gets the position of the node.
	virtual const core::vector3df& getPosition() const
	{ return m_Pos = RelativeTransformation.getTranslation(); }

	//! check visibility of this node
	virtual void OnCheckCulling();

	virtual const core::matrix4& getRelativeTransformation()
	{ return RelativeTransformation; }

	virtual const c8* getSceneCorePropertiesXMLString()
	{
		LOGGER.logErr("Invalid call %s", __FUNCTION__);
		return NULL;
	}

private:

    core::aabbox3d<f32> m_Box;

	vid::SMaterial m_Material;

	mutable core::vector3df m_Pos, m_Rot;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

