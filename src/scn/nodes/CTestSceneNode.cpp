//|-------------------------------------------------------------------------
//| File:        CTestSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CTestSceneNode.h"
#include "../CGeometryCreator.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <scn/IMeshBuffer.h>
#include <scn/ICullingSystem.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CTestSceneNode::CTestSceneNode(
	core::vector3df size, E_GEOM_PRIMITIVE_TYPE geom_type, 
	ISceneNode* parent, s32 id
	) : 
ITestSceneNode(parent, id), 
m_PrimitiveMesh(0), m_GeomPrimitiveType(E_GEOM_PRIMITIVE_TYPE_COUNT)
{
#if MY_DEBUG_MODE 
	setClassName("CTestSceneNode");
#endif

	setPrimitiveType(geom_type);
	setScale(size);
}

//---------------------------------------------------------------------------

//! destructor
CTestSceneNode::~CTestSceneNode()
{
	SAFE_DROP(m_PrimitiveMesh);
}

//---------------------------------------------------------------------------

E_GEOM_PRIMITIVE_TYPE CTestSceneNode::getPrimitiveType()
{
	return m_GeomPrimitiveType;
}

//---------------------------------------------------------------------------

void CTestSceneNode::setPrimitiveType(E_GEOM_PRIMITIVE_TYPE type)
{
	if (m_GeomPrimitiveType==type)
		return;

	m_GeomPrimitiveType = type;

	SAFE_DROP(m_PrimitiveMesh);

	if (m_GeomPrimitiveType==EGPT_BOX)
	{
		m_PrimitiveMesh = SCENE_MANAGER.getMesh("#box_mesh");
	}
	else
	if (m_GeomPrimitiveType==EGPT_SPHERE)
	{
		m_PrimitiveMesh = SCENE_MANAGER.getMesh("#sphere_mesh");
	}
	else
	if (m_GeomPrimitiveType==EGPT_CYLINDER)
	{
		m_PrimitiveMesh = SCENE_MANAGER.getMesh("#cylinder_mesh");
	}
	else
	{
		m_PrimitiveMesh = SCENE_MANAGER.getMesh("#caped_cylinder_mesh");
	}

	SAFE_GRAB(m_PrimitiveMesh);

	m_BoundingBox = m_PrimitiveMesh->getBoundingBox();	
}

//---------------------------------------------------------------------------

//! check visibility of this node
void CTestSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		if (IsVisible)
		{
			m_Culled = CULLING_SYSTEM.isCull(this);
		}
		else
		{
			m_Culled = true;		
		}

		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CTestSceneNode::OnPreRender(u32 timeMs)
{
    scn::ISceneManager &smgr = SCENE_MANAGER;		

    ISceneNode::OnPreRender(timeMs);
}


//---------------------------------------------------------------------------

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTestSceneNode::getBoundingBox() const
{
    return m_BoundingBox;
} 

//---------------------------------------------------------------------------

//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialsCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
vid::SMaterial& CTestSceneNode::getMaterial(s32 i)
{
    return Material;
}

//---------------------------------------------------------------------------

//! returns amount of materials used by this scene node.
s32 CTestSceneNode::getMaterialsCount()
{
    return 1;
}

//---------------------------------------------------------------------------

//!
bool CTestSceneNode::set(ISceneNode* other)
{
	if (!other || this == other
			|| other->getSceneNodeType()!=ESNT_TEST_SCENE_NODE)
	{
		return false;
	}

	setPrimitiveType( ((ITestSceneNode*)other)->getPrimitiveType() );
	setMaterial(other->getMaterial(0));

	return true;
}

//---------------------------------------------------------------------------

//!
const c8* CTestSceneNode::getSceneCorePropertiesXMLString()
{
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0));
		xmlstr.append(mat_xml);
	}

	xmlstr.sprintf("<GeomPrimitive type=\"%s\" />\n",
		GeomPrimitiveTypeStr[m_GeomPrimitiveType]);

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------


