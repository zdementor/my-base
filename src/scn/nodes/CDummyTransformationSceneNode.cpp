//|-------------------------------------------------------------------------
//| File:        CDummyTransformationSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CDummyTransformationSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! constructor
CDummyTransformationSceneNode::CDummyTransformationSceneNode(
    ISceneNode* parent, s32 id
	) : 
IDummyTransformationSceneNode(ESNT_DUMMY_TANSFORMATION_SCENE_NODE, parent, id)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CDummyTransformationSceneNode");
#endif

	m_Material.getPass(0).setDiffuseColor ( img::SColor(100, 100,100,0) );
	m_Material.getPass(0).setDepthTest ( vid::ECT_ALWAYS );
	m_Material.getPass(0).setBlendFuncs(
		vid::ESBF_ONE, vid::EDBF_ONE_MINUS_SRC_COLOR);
}

//----------------------------------------------------------------------------

//! destructor
CDummyTransformationSceneNode::~CDummyTransformationSceneNode()
{
}

//----------------------------------------------------------------------------

//! setting the current relative transformation matrix.
//! This is the matrix, this scene node uses instead of scale, translation
//! and rotation.
void CDummyTransformationSceneNode::setRelativeTransformation(const core::matrix4& m)
{
    RelativeTransformation = m;

	m_TransformationChanged = true;
}

//----------------------------------------------------------------------------

//! Sets the scale of the node.
void CDummyTransformationSceneNode::setScale(const core::vector3df& scale) {}

//----------------------------------------------------------------------------
    
//! Sets the rotation of the node.
void CDummyTransformationSceneNode::setRotation(
	const core::vector3df& rotation, bool controlled_by_ai ) {}

//----------------------------------------------------------------------------
    
//! Sets the position of the node.
void CDummyTransformationSceneNode::setPosition(const core::vector3df& newpos, bool controlled_by_ai)
{
	RelativeTransformation.setTranslation(newpos);

	m_TransformationChanged = true;
}

//------------------------------------------------------------------------

//! check visibility of this node
void CDummyTransformationSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		m_Culled = !IsVisible;

		ISceneNode::OnCheckCulling();
	}
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
