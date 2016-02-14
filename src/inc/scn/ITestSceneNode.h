//|-------------------------------------------------------------------------
//| File:        ITestSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITestSceneNodeHPP
#define ITestSceneNodeHPP
//-------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class __MY_SCN_LIB_API__ ITestSceneNode : public ISceneNode
{
public:

	// constructor
	ITestSceneNode(ISceneNode* parent, s32 id)
		: ISceneNode(ESNT_TEST_SCENE_NODE, parent, id) {}

	virtual E_GEOM_PRIMITIVE_TYPE getPrimitiveType() = 0;

	virtual void setPrimitiveType(E_GEOM_PRIMITIVE_TYPE type) = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

