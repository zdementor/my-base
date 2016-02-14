//|-------------------------------------------------------------------------
//| File:        CGameNodeItem.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeItem.h"

#include <dev/IDevice.h>
#include <game/IGameManager.h>
#include <io/ILogger.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

enum  
{
	ITEM_SOUND_COLLECT = 0,
	ITEM_SOUND_THROW,
	ITEM_SOUND_USE,	

	ITEM_SOUNDS_COUNT
};

//----------------------------------------------------------------------------

//! constructor
CGameNodeItem::CGameNodeItem(
	scn::ISceneNode* n, SGameNodeParams &params
	) :
IGameNodeItem(n, SGameNodeParams(params, EGNT_ITEM))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeItem");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeItem::~CGameNodeItem()
{
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeItem::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::_updateParams(delta_time_sec);
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeItem::OnPostDynamic(f64 delta_time_sec)
{
	core::list< core::list< SGameTask* >::iterator >::iterator it = 
		m_InputGameTasksIterators.begin();

	for (; it!=m_InputGameTasksIterators.end(); ++it)
	{
		SGameTask* gtask = **it;

		if (!gtask->Completed)
		{
			if (gtask->getType() == EGTT_ACTIVATE)
			{
				if (gtask->Sender->getGameNodeType()==game::EGNT_MAIN_PLAYER)
					collectToInventory(gtask->Sender->getInventory());					

				gtask->Completed = true;
			}
			else
			{
				gtask->Completed = true;
			}
		}
	}
}

//----------------------------------------------------------------------------

//! take this item to inventory
void CGameNodeItem::collectToInventory(IGameInventory& inventory)
{
	inventory.addItem(this);

	m_SceneNode->setParent(inventory.getOwner()->getSceneNode());
	m_SceneNode->setVisible(false);		

	IGameManager::getSingleton().onCollectItem(inventory.getOwner(), this);

	playSoundEffect(ITEM_SOUND_COLLECT);			

	use();
}

//----------------------------------------------------------------------------

//! throw this item from inventory
void CGameNodeItem::throwFromInventory()
{
	if (m_ParentInventory)
	{
		m_ParentInventory->removeObject(this);

		m_SceneNode->setParent(m_ParentInventory->getOwner()->getSceneNode()->getParent());
		m_SceneNode->setVisible(true);			

		IGameManager::getSingleton().onThrowItem(m_ParentInventory->getOwner(), this);

		playSoundEffect(ITEM_SOUND_THROW);
	}
}

//----------------------------------------------------------------------------

//! use this item
bool CGameNodeItem::use()
{
	if (m_ParentInventory)
	{
		IGameManager::getSingleton().onUseItem(m_ParentInventory->getOwner(), this);

		playSoundEffect(ITEM_SOUND_USE);

		return true;
	}

	return false;	
}

//----------------------------------------------------------------------------

//! Item registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardItem(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeItem(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardItemCreator;

IGameNode* Create_StandardItem(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardItemCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

class CStandardItemCloner : public CGameNodeBaseCloner
{
public:

	virtual IGameNode* cloneGameNode(IGameNode* game_node_to_clone, bool log_info)
	{
		game::IGameNode* game_node = CGameNodeBaseCloner::cloneGameNode(game_node_to_clone, log_info);
		if (game_node)
		{
			game::IGameNodeItem* item = SAFE_CAST_TO_GAME_NODE_ITEM(game_node);
			game::IGameNodeItem* item_to_clone = SAFE_CAST_TO_GAME_NODE_ITEM(game_node_to_clone);
			if (item&&item_to_clone)
			{
				item->setItemName      (item_to_clone->getItemName()      );
				item->setItemKeyName   (item_to_clone->getItemKeyName()   );
				item->setItemShortDescr(item_to_clone->getItemShortDescr());
				item->setItemDescr     (item_to_clone->getItemDescr()     );
				item->setItemImageName (item_to_clone->getItemImageName() );
			}
		}
		return game_node;
	}
}
StandardItemCloner;

IGameNode* Clone__StandardItem(IGameNode *game_node, bool log_info)
{
	return StandardItemCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

class CStandardItemLoader : public CGameNodeBaseLoader
{
public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);
		_loadGameNode(node_params);

		game::IGameNodeItem* item = SAFE_CAST_TO_GAME_NODE_ITEM(GameNode);

		if (XMLFile && GameNode && item)
		{
			core::stringc item_name, item_key_name, item_descr, item_short_descr, item_image_name; 

			// loading game data

			XMLFile->setPos(GameDataXMLPos);

			bool game_data_finded = false;	
			
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Game") == XMLFile->getName())	
					{
						game_data_finded = true;
					}					
					else
					if (game_data_finded)
					{
						if (core::stringw("ItemName") == XMLFile->getName())
						{
							item_name = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("ItemKeyName") == XMLFile->getName())
						{
							item_key_name = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("ItemShortDescription") == XMLFile->getName())
						{
							item_short_descr = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("ItemDescr") == XMLFile->getName())
						{
							item_descr = XMLFile->getAttributeValue(L"value");							
						}
						else
						if (core::stringw("ItemImageName") == XMLFile->getName())
						{
							item_image_name = XMLFile->getAttributeValue(L"value");							
						}						
						else
						if (notGameSector(XMLFile))	
						{
							break;  
						}
					} 					
					else
					{
						break;  
					}
				}
			}

			item->setItemName      (item_name.c_str()       );
			item->setItemKeyName   (item_key_name.c_str()   );
			item->setItemShortDescr(item_short_descr.c_str());
			item->setItemDescr     (item_descr.c_str()      );
			item->setItemImageName (item_image_name.c_str() );
		}

		_postLoadGameNode(node_params);	

		return GameNode;
	}
}
StandardItemLoader;

IGameNode* Load___StandardItem(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardItemLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
