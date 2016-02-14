//|-------------------------------------------------------------------------
//| File:        IKTreeSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IKTreeSceneNodeHPP
#define IKTreeSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/IAnimatedMeshSceneNode.h>
#include "ILightGrid.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Interface for a K-Tree Scene Node
class __MY_SCN_LIB_API__ IKTreeSceneNode :
	public IAnimatedMeshSceneNode, public ILightGrid
{
public:	

	//! constructor
	IKTreeSceneNode(ISceneNode* parent, s32 id)
		: IAnimatedMeshSceneNode(ESNT_KTREE_SCENE_NODE, parent, id) {}

	//!
	virtual void setMaxTrianglesPerNode(s32 max_triangles_per_node) = 0;

	//!
	virtual s32 getMaxTrianglesPerNode() = 0;

	//! creates the tree
    virtual bool recreateTree() = 0;

	//! make dirt spot
	virtual void makeDirtSpot(
		const core::vector3df &abs_pos, const core::vector3df &size,
		const img::SColor &color, bool hipriority=false) = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

