//|-------------------------------------------------------------------------
//| File:        CoreDynamic.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

#include "CoreSingletonsTable.h"

//----------------------------------------------------------------------------
namespace my {
namespace dyn {
//----------------------------------------------------------------------------

//! Create Dynamic object 
__MY_CORE_LIB_API__ dyn::IDynamicObject* createDynamicObjectForSceneNode(
	scn::ISceneNode *node, dyn::SDynamicObjectParams &ode_obj_params)
{
	if (!node)
		return NULL;
	return core::g_SingletonsTable.Ptrs.DynamicManager->
		addObjectForSceneNode(node, ode_obj_params);
}

//----------------------------------------------------------------------------

//! Destroy Dynamic object 
__MY_CORE_LIB_API__ bool destroyDynamicObjectForSceneNode(scn::ISceneNode *node)
{
	if (!node)
		return false;
	return core::g_SingletonsTable.Ptrs.DynamicManager->
		removeObjectForSceneNode(node);
}

//---------------------------------------------------------------------------

//! moving dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectPosition(
    dyn::IDynamicObject *dynobj, const core::vector3df &pos, bool controlled_by_ai)
{
	if (!dynobj)
		return;  
	core::g_SingletonsTable.Ptrs.DynamicManager->
		setDynamicObjectPosition(dynobj, pos, controlled_by_ai);
}

//---------------------------------------------------------------------------     

//! rotating dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectRotation(
    dyn::IDynamicObject *dynobj, const core::vector3df &rot, bool controlled_by_ai)
{   
	if (!dynobj)
		return;
	core::g_SingletonsTable.Ptrs.DynamicManager->
		setDynamicObjectRotation(dynobj, rot, controlled_by_ai);
}

//---------------------------------------------------------------------------     

//! scaling dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectScale(
	dyn::IDynamicObject *dynobj, const core::vector3df &scale)
{
	if (!dynobj)
		return;
	core::g_SingletonsTable.Ptrs.DynamicManager->
		setDynamicObjectScale(dynobj, scale);
}

//---------------------------------------------------------------------------

//! returns dynamic oject axis aligned bounding box
__MY_CORE_LIB_API__ core::aabbox3d<f32> getDynamicObjectAABBox(
	dyn::IDynamicObject *dynobj)
{
	core::aabbox3d<f32> aa_box;
	if (!dynobj)
		return aa_box;
	aa_box = core::g_SingletonsTable.Ptrs.DynamicManager->
		getDynamicObjectAABBox(dynobj);
    return aa_box;
}

//----------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//----------------------------------------------------------------------------



