//|-------------------------------------------------------------------------
//| File:        CoreDynamic.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CoreDynamicHPP
#define CoreDynamicHPP

//----------------------------------------------------------------------------
namespace my {
namespace dyn {
//----------------------------------------------------------------------------

//! Create Dynamic object 
__MY_CORE_LIB_API__ my::dyn::IDynamicObject* createDynamicObjectForSceneNode(
	scn::ISceneNode *node, dyn::SDynamicObjectParams &ode_obj_params
	);
//! Destroy Dynamic object 
__MY_CORE_LIB_API__ bool destroyDynamicObjectForSceneNode(scn::ISceneNode *node);

//! moving dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectPosition(
	dyn::IDynamicObject* dynobj, const core::vector3df &pos, bool controlled_by_ai = false
	);
//! rotating dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectRotation(
	dyn::IDynamicObject* dynobj, const core::vector3df &rot, bool controlled_by_ai = false
	);
//! scaling dynamic object
__MY_CORE_LIB_API__ void setDynamicObjectScale(
	dyn::IDynamicObject* dynobj, const core::vector3df &scale
	);
//! returns dynamic oject axis aligned bounding box
__MY_CORE_LIB_API__ core::aabbox3d<f32> getDynamicObjectAABBox (dyn::IDynamicObject* dynobj);

//----------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CoreDynamicHPP