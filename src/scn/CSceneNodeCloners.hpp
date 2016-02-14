//|-------------------------------------------------------------------------
//| File:        CSceneNodeCloners.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <scn/ISceneNodeCloner.h>
#include <scn/SAnimatedMesh.h>
#include <scn/SMesh.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CSceneNodeCloner : public ISceneNodeCloner
{
private:

	bool preCloneSceneNode(ISceneNode *node_to_clone, bool log_info)
	{
		if (!node_to_clone)
			return false;

		E_SCENE_NODE_TYPE sntype = m_SceneNodeType;

		if (log_info) 
			LOGGER.logInfo("Cloning %s Scene Node . . .", SceneNodeTypeShortStr[sntype]);

		if (node_to_clone->getSceneNodeType() != sntype)
		{
			LOGGER.logWarn("Wrong Scene Node Type (wanted '%s', got '%s') !",
				SceneNodeTypeShortStr[sntype],
				SceneNodeTypeShortStr[node_to_clone->getSceneNodeType()]);
			return false;
		}
		return true;
	}

	void postCloneSceneNode(ISceneNode *node, ISceneNode *node_to_clone, bool log_info)
	{
		E_SCENE_NODE_TYPE sntype = m_SceneNodeType;

		if (node&&node_to_clone)
		{
			LOGGER.increaseFormatLevel();

			const core::list<ISceneNode*>& childs = node_to_clone->getChildren();
			core::list<ISceneNode*>::iterator it = childs.begin();
			for (; it!=childs.end(); ++it)
			{
				ISceneNode* child_to_clone = *it;

				if (!child_to_clone->isClonableChild())
					continue;
				
				ISceneNode* child_cloned = 
					SCENE_MANAGER.cloneSceneNode(child_to_clone, true, log_info);
				if (child_cloned)
				{
					child_cloned->setParent  (node,true);
					child_cloned->setPosition(child_to_clone->getPosition());
					child_cloned->setRotation(child_to_clone->getRotation());
					child_cloned->setScale   (child_to_clone->getScale()   );
					child_cloned->updateAbsoluteTransformation();
					child_cloned->setVisible (child_to_clone->isVisible());
				}
			}

			ITextSceneNode* caption_to_clone = node_to_clone->getCaption();
			if (caption_to_clone)
			{
				ITextSceneNode* caption_cloned = node->attachCaption(
					caption_to_clone->getFont(),
					caption_to_clone->getText(),
					caption_to_clone->getTextColor() 					
					);
				if (caption_cloned)
				{
					caption_cloned->setParent  (node);
					caption_cloned->setPosition(caption_to_clone->getPosition()); 					
					caption_cloned->updateAbsoluteTransformation();
					caption_cloned->setVisible(caption_to_clone->isVisible());
				}
			}

			node->setPosition(node_to_clone->getPosition());
			node->setRotation(node_to_clone->getRotation());
			node->setScale   (node_to_clone->getScale()   );
			node->updateAbsoluteTransformation();

			LOGGER.decreaseFormatLevel();
		}

		if (!node)
			LOGGER.logErr("Couldn't clone %s Scene Node node!",
				SceneNodeTypeShortStr[sntype]);
		else if (log_info)   
			LOGGER.logInfo("%s Scene Node successfully cloned",
				SceneNodeTypeShortStr[sntype]);
	}

	ISceneNode* createSceneNode(bool log_info)
	{
		SSceneNodeCommonParams snode_params;
		snode_params.Parameters.Type = m_SceneNodeType;

		return SCENE_MANAGER.createSceneNode(snode_params, log_info);
	}

	E_SCENE_NODE_TYPE m_SceneNodeType;

public:

	CSceneNodeCloner(E_SCENE_NODE_TYPE sntype)
		: m_SceneNodeType(sntype) {}

	ISceneNode * cloneSceneNode(ISceneNode *node_to_clone, bool log_info)
	{
		scn::ISceneNode* node = 0;		

		if (!node_to_clone ||
			node_to_clone->getSceneNodeType() != m_SceneNodeType)
			return node;

		if (preCloneSceneNode(node_to_clone, log_info))
		{
			node = createSceneNode(log_info);
			if (node)
				node->set(node_to_clone);	
		}

		postCloneSceneNode(node, node_to_clone, log_info);

		return node;
	}
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
