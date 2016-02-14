//|-------------------------------------------------------------------------
//| File:        CoreScene.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CoreSceneHPP
#define CoreSceneHPP

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------
	
// Create shadow volume scene node
__MY_CORE_LIB_API__ scn::IShadow *createShadowForSceneNode(
	scn::ISceneNode *node, scn::E_SHADOW_TYPE type);

//! Destroy shadow volume scene node
__MY_CORE_LIB_API__ bool destroyShadowForSceneNode(scn::ISceneNode *node);

// Create caption scene node
__MY_CORE_LIB_API__ scn::ITextSceneNode *addTextSceneNode(
	scn::ISceneNode *node, s32 id, 
	vid::IFont *font, const c8 *text, img::SColor color); 

//! getting scene node params
__MY_CORE_LIB_API__ scn::SSceneNodeCommonParams getSceneNodeParams(scn::ISceneNode *node);

//! setting scene node all params
__MY_CORE_LIB_API__ void applySceneNodeParams(scn::ISceneNode *node, scn::SSceneNodeCommonParams &params);

//! setting scene node params only
__MY_CORE_LIB_API__ void applySceneNodeParams(scn::ISceneNode *node, scn::SSceneNodeParameters &params);

//! setting scene node params transformations
__MY_CORE_LIB_API__ void applySceneNodeParams(scn::ISceneNode *node, scn::SSceneNodeTransformation &params);

//! setting scene node params material
__MY_CORE_LIB_API__ void applySceneNodeParams(scn::ISceneNode *node, scn::SSceneNodeMaterial &material);

//! calculating adjacency data for animated mesh
__MY_CORE_LIB_API__ bool calculateAdjacency(scn::IAnimatedMesh *amesh);

//!
__MY_CORE_LIB_API__ scn::ISceneNode* getSceneNodeFromId(s32 parent_id);		

//!
__MY_CORE_LIB_API__ scn::ISceneNode* getRootSceneNode();

//! called each time when ISceneNode creating
__MY_CORE_LIB_API__ void onCreateSceneNode(scn::ISceneNode* snode);

//! called each time when ISceneNode deleting
__MY_CORE_LIB_API__ void onDeleteSceneNode(scn::ISceneNode* snode);

//! called each time when ISceneNode's method setOccluder run
__MY_CORE_LIB_API__ void onSetOccluder(scn::ISceneNode* snode, bool occluder);

//! called each time when IGameNode starting new animation
__MY_CORE_LIB_API__ void onAnimationBegin(scn::ISceneNode *snode, u32 animation_idx);

//! called each time when IGameNode ending new animation
__MY_CORE_LIB_API__ void onAnimationEnd(scn::ISceneNode *snode, u32 animation_idx);

//! attaching camera to this scene node
__MY_CORE_LIB_API__ void attachCamera(ISceneNode* scene_node, ICameraSceneNode* camera);

//! detaching camera from this scene node
__MY_CORE_LIB_API__ void detachCamera(ISceneNode* scene_node);

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CoreSceneHPP