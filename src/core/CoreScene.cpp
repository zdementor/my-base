//|-------------------------------------------------------------------------
//| File:        CoreScene.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

#include <scn/IShadow.h>
#include <scn/ITextSceneNode.h>
#include <scn/ICameraSceneNode.h>

#include "CoreSingletonsTable.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

scn::IShadow *createShadowForSceneNode(
	scn::ISceneNode *node, scn::E_SHADOW_TYPE type)
{
	if (!node)
		return NULL;
	scn::IShadow *shadow = core::g_SingletonsTable.Ptrs.SceneManager->
		createShadow( node,  type );	
	return shadow;
}

//----------------------------------------------------------------------------

bool destroyShadowForSceneNode(scn::ISceneNode *node)
{
	if (!node)
		return false;
	scn::IShadow *shadow = node->getShadow();
	if (!shadow) return false;
	shadow->drop();
	return true;
}

//----------------------------------------------------------------------------

scn::ITextSceneNode* addTextSceneNode( 	
	scn::ISceneNode *node, s32 id,
	vid::IFont *font, const c8* text, img::SColor color)
{
	if (!node)
		return NULL;
	scn::ITextSceneNode *caption = 
		core::g_SingletonsTable.Ptrs.SceneManager->
		addTextSceneNode(font, text, color, node, id);
	return caption;
}

//---------------------------------------------------------------------------

scn::SSceneNodeCommonParams getSceneNodeParams(scn::ISceneNode *node)
{
	scn::SSceneNodeCommonParams node_params;
	if (!node) 
		return node_params;
	game::E_GAME_NODE_TYPE gntype = game::EGNT_UNKNOWN; 
	game::IGameNode * gnode = node->getGameNode();
	if (gnode)
		gntype = gnode->getGameNodeType();
	core::stringc texture_filename0 = NONAME_FILE_NAME,
		texture_filename1 = NONAME_FILE_NAME;
	vid::SMaterial material;

	// reading nodes params	
	
	// Transformations
	node_params.Transformation.Position = node->getPosition(); 
	node_params.Transformation.Rotation = node->getRotation();    
	node_params.Transformation.Scale    = node->getScale();

	// Material
	if (gntype != game::EGNT_LIGHT) 
	{
		if (node->getMaterialsCount()>0)	 
			material = node->getMaterial(0);
	}
	else
	{
		vid::SLight lightData =
			((scn::ILightSceneNode*)node)->getLightParameters();
		material.getPass(0).setAmbientColor ( lightData.AmbientColor.toSColor());
		material.getPass(0).setDiffuseColor ( lightData.DiffuseColor.toSColor());
		material.getPass(0).setSpecularColor( lightData.SpecularColor.toSColor());
	}

	node_params.Material.Material = material;

	// Other Params
	s32 id = node->getID();
	s32 parent_id = -1;
	scn::ISceneNode *parent = node->getParent();
	if (parent)	
		parent_id = parent->getID();
	node_params.Parameters.Name		= node->getName();
	node_params.Parameters.ID		= id;
	node_params.Parameters.ParentID	= parent_id;
	node_params.Parameters.Type		= node->getSceneNodeType();

	return node_params;
}

//---------------------------------------------------------------------------

void applySceneNodeParams(
	scn::ISceneNode *node, scn::SSceneNodeCommonParams &params)
{
	if (!node)
		return;
	applySceneNodeParams(node, params.Parameters);
	applySceneNodeParams(node, params.Material);
	applySceneNodeParams(node, params.Transformation);
}

//----------------------------------------------------------------------------

void applySceneNodeParams(
	scn::ISceneNode *node, scn::SSceneNodeParameters &parameters)
{
	if (!node)
		return;
	scn::E_SCENE_NODE_TYPE ntype = node->getSceneNodeType(); 
	// setting node name
	node->setName(parameters.Name.c_str());
	// setting unique id
	if (!node->setID(parameters.ID))
	{
		const c8 *name = node->getName();
		core::g_SingletonsTable.Ptrs.Logger->logWarn(
			"Can't set ID %d to SceneNode %s, it's not unique", 
			parameters.ID, name); 
	}
	// setting parent
	scn::ISceneNode* parent = 
		core::g_SingletonsTable.Ptrs.SceneManager->getSceneNodeFromId(parameters.ParentID);
	if (parent)
		node->setParent(parent);
	scn::ICameraSceneNode *camera = node->getCamera();
	if (camera)
	{
		camera->updateAbsoluteTransformation();
		camera->animate();
	}
}

//----------------------------------------------------------------------------

void applySceneNodeParams(scn::ISceneNode *node,
	scn::SSceneNodeTransformation &transformations)
{
	if (!node)
		return;
	// applying transformations
	node->setPosition(transformations.Position);
	node->setRotation(transformations.Rotation); 
	node->setScale(transformations.Scale); 
	node->updateAbsoluteTransformation();
}

//----------------------------------------------------------------------------

void applySceneNodeParams(scn::ISceneNode *node, scn::SSceneNodeMaterial &material)
{
	if (!node)
		return;

	scn::E_SCENE_NODE_TYPE ntype = node->getSceneNodeType(); 

	// applying material
	if (ntype != scn::ESNT_LIGHT_SCENE_NODE) 
	{
		node->setMaterial(material.Material);	
	}  	
}

//----------------------------------------------------------------------------

bool calculateAdjacency(scn::IAnimatedMesh *amesh)
{
	if (!amesh)
		return false;
	return core::g_SingletonsTable.Ptrs.SceneManager->calculateAdjacency(amesh);
}

//----------------------------------------------------------------------------

scn::ISceneNode* getSceneNodeFromId(s32 parent_id)
{
	return core::g_SingletonsTable.Ptrs.SceneManager->getSceneNodeFromId(parent_id);
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ scn::ISceneNode* getRootSceneNode()
{
	return core::g_SingletonsTable.Ptrs.SceneManager->getRootSceneNode();
}

//----------------------------------------------------------------------------

void onCreateSceneNode(scn::ISceneNode* snode)
{
	core::g_SingletonsTable.Ptrs.SceneManager->onCreateSceneNode(snode);
}

//----------------------------------------------------------------------------

void onDeleteSceneNode(scn::ISceneNode* snode)
{
	core::g_SingletonsTable.Ptrs.SceneManager->onDeleteSceneNode(snode);
}

//----------------------------------------------------------------------------

void onSetOccluder(scn::ISceneNode* snode, bool occluder)
{
	core::g_SingletonsTable.Ptrs.SceneManager->onSetOccluder(snode, occluder); 
}

//----------------------------------------------------------------------------

void onAnimationBegin(scn::ISceneNode *snode, u32 animation_idx)
{
	core::g_SingletonsTable.Ptrs.SceneManager->onAnimationBegin(snode, animation_idx);	
}

//----------------------------------------------------------------------------

void onAnimationEnd(scn::ISceneNode *snode, u32 animation_idx)
{
	core::g_SingletonsTable.Ptrs.SceneManager->onAnimationEnd(snode, animation_idx);	
}

//----------------------------------------------------------------------------

void attachCamera(ISceneNode* scene_node, ICameraSceneNode* camera)
{
	core::g_SingletonsTable.Ptrs.SceneManager->attachCamera(scene_node, camera);	
}

//----------------------------------------------------------------------------

void detachCamera(ISceneNode* scene_node)
{
	core::g_SingletonsTable.Ptrs.SceneManager->detachCamera(scene_node);	
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------



