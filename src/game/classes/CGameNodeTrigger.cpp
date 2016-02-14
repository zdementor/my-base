//|-------------------------------------------------------------------------
//| File:        CGameNodeTrigger.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeTrigger.h"

#include <io/ILogger.h>
#include <game/GameTasks.h>
#include <game/IGameTasksManager.h>
#include <io/IFileSystem.h>
#include <io/IXMLWriter.h>
#include <io/IWriteFile.h>
#include <scr/IScriptManager.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

CGameNodeTrigger::CGameNodeTrigger(
	scn::ISceneNode* n, SGameNodeParams &params
	) : IGameNodeTrigger(n, SGameNodeParams(params, EGNT_TRIGGER)),
m_ActivateScriptFileName(NONAME_FILE_NAME), m_ActivateScriptText(""),
m_DeactivateScriptFileName(NONAME_FILE_NAME), m_DeactivateScriptText(""),
m_CurrActivated(false)
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeTrigger");	
#endif
}

//----------------------------------------------------------------------------

CGameNodeTrigger::~CGameNodeTrigger()
{
}

//----------------------------------------------------------------------------

bool CGameNodeTrigger::activate(IGameNode* activator)
{
	game::IGameTasksManager &gtmgr = GAME_TASK_MANAGER;

	for (u32 i=0; i<m_TargetNodes.size(); i++)
	{
		// registering task (node activation)
		gtmgr.registerGameTask( new game::SGameTaskActivate(this, m_TargetNodes[i]) ); 				
	}

	if (m_ActivateScriptText.size()>0)		
		SCRIPT_MANAGER.runScriptText(m_ActivateScriptText.c_str());		

	if (m_ActivateScriptFileName.size() && m_ActivateScriptFileName != NONAME_FILE_NAME)
		SCRIPT_MANAGER.runScript(m_ActivateScriptFileName.c_str());	

	m_Activated = true;	

	return true;
}

//----------------------------------------------------------------------------

bool CGameNodeTrigger::deactivate()
{
	if (m_ActivateScriptText.size()>0)		
		SCRIPT_MANAGER.runScriptText(m_DeactivateScriptText.c_str());		

	if (m_DeactivateScriptFileName.size() && m_DeactivateScriptFileName != NONAME_FILE_NAME)		
		SCRIPT_MANAGER.runScript(m_DeactivateScriptFileName.c_str());		

	m_Activated = false;	

	return true;
}

//----------------------------------------------------------------------------

void CGameNodeTrigger::setActivateScriptFileName(const c8* filename)
{
	m_ActivateScriptFileName=filename;
}

//----------------------------------------------------------------------------

const c8* CGameNodeTrigger::getActivateScriptFileName()
{
	return m_ActivateScriptFileName.c_str();
}

//----------------------------------------------------------------------------

void CGameNodeTrigger::setActivateScriptText(const c8* script_text)
{
	m_ActivateScriptText=script_text;
}

//----------------------------------------------------------------------------

const c8* CGameNodeTrigger::getActivateScriptText()
{
	return m_ActivateScriptText.c_str();
}

//----------------------------------------------------------------------------

void CGameNodeTrigger::setDeactivateScriptFileName(const c8* filename)
{
	m_DeactivateScriptFileName=filename;
}

//----------------------------------------------------------------------------

const c8* CGameNodeTrigger::getDeactivateScriptFileName()
{
	return m_DeactivateScriptFileName.c_str();
}

//----------------------------------------------------------------------------

void CGameNodeTrigger::setDeactivateScriptText(const c8* script_text)
{
	m_DeactivateScriptText=script_text;
}

//----------------------------------------------------------------------------

const c8* CGameNodeTrigger::getDeactivateScriptText()
{
	return m_DeactivateScriptText.c_str();
}

//---------------------------------------------------------------------------

//!
const c8* CGameNodeTrigger::getGameCorePropertiesXMLString()
{
	core::stringw xmlbuf;

	io::IWriteFile* memw = FILE_SYSTEM.createMemoryWriteFile("", xmlbuf);
	io::IXMLWriter* xmlw = FILE_SYSTEM.createXMLWriter(memw);

	xmlw->writeLineBreak();
	xmlw->writeElement(L"TriggerParams", false);

	xmlw->writeLineBreak(); 
	xmlw->writeElement(
		L"ActivateScriptFileName", false, 
		L"value", core::stringw(m_ActivateScriptFileName.c_str()).c_str()
		);
	xmlw->writeLineBreak(); 
	xmlw->writeElement(
		L"DeactivateScriptFileName", false, 
		L"value", core::stringw(m_DeactivateScriptFileName.c_str()).c_str()
		);
	xmlw->writeLineBreak(); 
	xmlw->writeElement(
		L"ActivateScriptText", false, 
		L"value", core::stringw(m_ActivateScriptText.c_str()).c_str()
		);
	xmlw->writeLineBreak(); 
	xmlw->writeElement(
		L"DeactivateScriptText", false, 
		L"value", core::stringw(m_DeactivateScriptText.c_str()).c_str()
		);
	xmlw->writeLineBreak(); 
	xmlw->writeClosingTag(L"TriggerParams");
	xmlw->writeLineBreak();  

	xmlw->drop();
	memw->drop();

	static core::stringc xmlstr;
	core::stringc xmlbufc = xmlbuf.c_str();
	xmlstr.append(xmlbufc);

	return xmlstr.c_str();
} 	

//----------------------------------------------------------------------------

//! Trigger registry funcs

//----------------------------------------------------------------------------

IGameNode* Alloc__StandardTrigger(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeTrigger(n, params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardTriggerCreator;

IGameNode* Create_StandardTrigger(SGameNodeCommonParams &node_params, bool log_info)
{
	// override default params
	node_params.Dynamic.Type = dyn::EDOT_SENSELESS;
	node_params.Dynamic.GeomForm = dyn::EDOGF_BOX;							
	node_params.Dynamic.DynamicConstraints = dyn::EDTC_NO_TRANSF;

	return StandardTriggerCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardTriggerCloner;

IGameNode* Clone__StandardTrigger(IGameNode *game_node, bool log_info)
{
	return StandardTriggerCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

class CStandardTriggerLoader : public CGameNodeBaseLoader
{
public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);
		_loadGameNode(node_params);

		game::IGameNodeTrigger* trigger = SAFE_CAST_TO_GAME_NODE_TRIGGER(GameNode);

		io::IXMLReader* xml = XMLFile;

		if (xml && GameNode && trigger)
		{
			core::stringc activate_script_file_name   = NONAME_FILE_NAME;
			core::stringc deactivate_script_file_name = NONAME_FILE_NAME;
			core::stringc activate_script_text   = "";
			core::stringc deactivate_script_text = "";

			xml->setPos(GameDataXMLPos);

			while(xml && xml->read())
			{   
				if (xml->getNodeType() == io::EXN_ELEMENT)
				{					
					if (core::stringw("Game") == xml->getName())
					{
						s32 xml_pos = xml->getPos();

						while(xml->read())
						{
							if (xml->getNodeType() == io::EXN_ELEMENT)
							{
								if (core::stringw(GameNodeCorePropertiesTagValue) == xml->getName())
								{
									xml_pos = xml->getPos();

									while(xml->read())
									{
										if (core::stringw("TriggerParams") == xml->getName())
										{
											xml_pos = xml->getPos();

											while(xml->read())
											{
												if (xml->getNodeType() == io::EXN_ELEMENT)
												{
													if (core::stringw("ActivateScriptFileName") == xml->getName())
													{
														activate_script_file_name = XMLFile->getAttributeValue(L"value");
													}
													else
													if (core::stringw("DeactivateScriptFileName") == xml->getName())
													{
														deactivate_script_file_name = XMLFile->getAttributeValue(L"value");
													}
													else
													if (core::stringw("ActivateScriptText") == xml->getName())
													{
														activate_script_text = XMLFile->getAttributeValue(L"value");
													}
													else
													if (core::stringw("DeactivateScriptText") == xml->getName())
													{
														deactivate_script_text = XMLFile->getAttributeValue(L"value");
													}
													else
													{
														break;
													}

												}
												xml_pos = xml->getPos();
											}
											xml_pos = xml->getPos();
										}								
										else
										{
											break;
										}
									}
									xml_pos = xml->getPos();
								}								
								else
								{
									break;
								}
							}
							xml_pos = xml->getPos();
						}
						xml->setPos(xml_pos);
					}					
					else
					{
						break;
					}					
				}
			}	

			trigger->setActivateScriptFileName  (activate_script_file_name.c_str());
			trigger->setDeactivateScriptFileName(deactivate_script_file_name.c_str());
			trigger->setActivateScriptText  (activate_script_text.c_str());
			trigger->setDeactivateScriptText(deactivate_script_text.c_str());
		}

		_postLoadGameNode(node_params);	

		return GameNode;
	}
}
StandardTriggerLoader;

IGameNode* Load___StandardTrigger(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardTriggerLoader.loadGameNode(node_params, file);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
