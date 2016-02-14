//|-------------------------------------------------------------------------
//| File:        CGameSceneSaver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CGameSceneSaver.h"

#include <scn/ISceneManager.h>
#include <dyn/IDynamicManager.h>
#include <io/IFileSystem.h>
#include <vid/IVideoDriver.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <game/IGameManager.h>
#include <mm/ISoundDriver.h>
#include <res/IResourceManager.h>
#include <game/SGameSceneProps.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

const s32 MAX_STRLEN=255;

//----------------------------------------------------------------------------

void printXMLSlpitter(io::IXMLWriter* xml)
{
    if (!xml) return;

    xml->writeComment(L"******************************************************************"); 
    xml->writeLineBreak();
}

//----------------------------------------------------------------------------

void printXMLHeader(io::IXMLWriter* xml)
{
    if (!xml) return;

    xml->writeXMLHeader();
    xml->writeLineBreak();

    printXMLSlpitter(xml);

    xml->writeComment(L"This file created by My Scene Editor"); xml->writeLineBreak();
    xml->writeComment(L"                                    "); xml->writeLineBreak();
    
    printXMLSlpitter(xml);

    xml->writeLineBreak();
}

//----------------------------------------------------------------------------

CGameSceneSaver::CGameSceneSaver()
{
#if MY_DEBUG_MODE  
	setClassName("CGameSceneSaver");
#endif
}

//----------------------------------------------------------------------------

CGameSceneSaver::~CGameSceneSaver()
{
}

//----------------------------------------------------------------------------

void CGameSceneSaver::saveGameNodeCommonParams(
	io::IXMLWriter* xml,IGameNode* game_node)
{
	if (!xml || !game_node || game_node->isAutoCreated()) 
		return;

	scn::ISceneNode *node = game_node->getSceneNode();

	if (!node) return;

	game_node->updateGameCommonParams();

	core::stringw str_w1, str_w2, str_w3, str_w4, str_w5;
	core::stringw str_w6, str_w7, str_w8, str_w9, str_w10;

	SGameNodeCommonParams node_params = game_node->getGameCommonParams();
	
	core::stringc fname = node->getFileName();
	core::stringc name  = node->getName();

	bool embedded_xml_text = false;	

	if (fname.size() == 0 ||
			fname[0] == '#' ||
			name == NONAME_SCENENODE_NAME ||
			fname == NONAME_SCENENODE_NAME ||
			name == NONAME_FILE_NAME ||
			fname == NONAME_FILE_NAME)
	{
		if (node_params.Game.Type == EGNT_LIGHT ||
				node_params.Game.Type == EGNT_TRIGGER)
			embedded_xml_text = true;
		else
		{
			LOGGER.logWarn("%s (%s) Scene Node not saved!", 
				node->getName(), game::GameNodeTypeStr[node_params.Game.Type]);
			SceneSaveWarnings++;
			return;
		}
	}

	str_w1 = game::GameNodeTypeStr[node_params.Game.Type];
	str_w2 = node_params.Game.ClassName.c_str();

	{
		const c8 *xml_scripts_dir = RESOURCE_MANAGER.getMediaDirFull(res::EMT_XML_SCRIPTS);
		const c8 *full_file_name = node->getFileName();
		const c8 *file_path = core::extractFilePath(full_file_name);
		const c8 *file_name = core::extractFileName(full_file_name);
		const c8 *rel_file_path = core::getRelativePathBetweenDirectories(xml_scripts_dir, file_path);
		core::stringc fname_to_save;
		fname_to_save.sprintf("%s%s", rel_file_path, file_name);

		xml->writeElement(
			L"GameNode", false, 
			L"game_node_type", str_w1.c_str(), 
			L"filename", 
			core::stringw(fname_to_save.c_str()).c_str()
			);
		xml->writeLineBreak();
	}

	xml->writeElement(L"Scene");
	xml->writeLineBreak();	 
	SCENE_MANAGER.saveSceneNodeTransformation(xml, node_params.Scene.Transformation);
	SCENE_MANAGER.saveSceneNodeParameters(xml, node_params.Scene.Parameters);
 	xml->writeClosingTag(L"Scene");
	xml->writeLineBreak();	

	if (embedded_xml_text)
	{
		xml->writeElement(L"EmbeddedXMLFileBegin",true);
		xml->writeLineBreak();	

		game::E_GAME_NODE_TYPE game_node_type  = game_node->getGameNodeType();
		scn::E_SCENE_NODE_TYPE scene_node_type = node->getSceneNodeType();

		core::stringc core_scn_str = "";
		core_scn_str.sprintf("<%s>\n%s\n</%s>",
			scn::SceneNodeCorePropertiesTagValue,
			game_node->getSceneCorePropertiesXMLString(),
			scn::SceneNodeCorePropertiesTagValue);

		core::stringc core_game_str = "";
		core_game_str.sprintf("<%s>\n%s\n</%s>",
			game::GameNodeCorePropertiesTagValue,
			game_node->getGameCorePropertiesXMLString(),
			game::GameNodeCorePropertiesTagValue);

		core::stringc core_dyn_str = "";
		core_dyn_str.sprintf("<%s>\n%s\n</%s>",
			dyn::DynamicObjectCorePropertiesTagValue,
			game_node->getDynamicCorePropertiesXMLString(),
			dyn::DynamicObjectCorePropertiesTagValue);

		core::stringc str;
		core::stringc xmlstr; 

		xml->writeElement(
			L"Scene", false, 
			L"scene_node_type", core::stringw(scn::SceneNodeTypeStr[scene_node_type]).c_str()
			);
		xml->writeLineBreak(); 
		xmlstr = core_scn_str+"\n";
		xml->write( core::stringw(xmlstr.c_str()).c_str() );
 		xml->writeClosingTag(L"Scene");
		xml->writeLineBreak();  

		str.sprintf(
			"<Game game_node_type=\"%s\" class_name=\"%s\" >\n",
			game::GameNodeTypeStr[game_node_type],
			node_params.Game.ClassName.c_str()
			);
		xmlstr = str;
		xmlstr.append(core_game_str+"\n");
		str.sprintf("</Game>\n");
		xmlstr.append(str);

		str.sprintf("<Dynamic>\n");
		xmlstr.append(str);
		xmlstr.append(core_dyn_str+"\n");
		str.sprintf("</Dynamic>\n");
		xmlstr.append(str);

		str.sprintf("<Sound>\n");
		xmlstr.append(str);
		str.sprintf("</Sound>\n");
		xmlstr.append(str);  

		xml->write(core::stringw(xmlstr.c_str()).c_str());	

		xml->writeElement(L"EmbeddedXMLFileEnd",true);
		xml->writeLineBreak();
	}
	
	xml->writeClosingTag(L"GameNode");
	xml->writeLineBreak();
	xml->writeLineBreak();
}

//----------------------------------------------------------------------------

//! saving scene to xml-file
void CGameSceneSaver::saveGameScene(core::stringc xmlFileName)
{
	SceneSaveWarnings=0;	

    core::stringc xmlShortFileName = core::extractFileName(xmlFileName);

	wchar_t strw1[MAX_STRLEN], strw2[MAX_STRLEN], strw3[MAX_STRLEN];
	wchar_t strw4[MAX_STRLEN];	

    core::stringc Msg;
    Msg.sprintf("Saving Scene to %s . . .", xmlFileName.c_str());
    LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);

    io::IXMLWriter* xml = FILE_SYSTEM.createXMLWriter(xmlFileName.c_str());

    if (!xml) 
	{	
		Msg.sprintf("Can not create xml-file %s, save process aborted!", xmlFileName.c_str());
		LOGGER.log(Msg.c_str(), io::ELL_ERROR);
		return;
	}

	printXMLHeader(xml);

	core::stringw strw, strww;
    
    xml->writeElement(L"GameScene");    
    xml->writeLineBreak();
    xml->writeLineBreak();	

	strw = IGameManager::getSingleton().getGameScriptFileName().c_str();

	xml->writeElement(L"GameScript", true, L"filename", strw.c_str());
	xml->writeLineBreak();
	xml->writeLineBreak();		

	xml->writeElement(L"GameSound");    
	xml->writeLineBreak();

	core::stringc sound_track_dir = 
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_TRACKS );

	u32 sound_tracks_count = 
		IGameManager::getSingleton().getSoundTracksCount();

	for (u32 s=0; s<sound_tracks_count; s++)
	{
		mm::ISound* snd = 
			IGameManager::getSingleton().getSoundTrack(s);

		if (snd)
		{
			core::stringc fullname = SOUND_DRIVER.findSoundFullFileName(snd).c_str();
			core::stringc path = core::extractFilePath(fullname);
			core::stringc fname = core::extractFileName(fullname);
			core::stringc relpath = core::getRelativePathBetweenDirectories(sound_track_dir, path);
			core::stringc fname_to_save = relpath + fname;

			strw = fname_to_save.c_str();			

			xml->writeElement(L"SoundTrack", true, L"filename", strw.c_str());
			xml->writeLineBreak();				
		}
	}

	xml->writeClosingTag(L"GameSound");    
    xml->writeLineBreak();
	xml->writeLineBreak();	

	xml->writeElement(L"SceneProperties");
	xml->writeLineBreak();	

	SGameSceneProps scene_props = IGameManager::getSingleton().getGameSceneProps();	
	
	scn::E_SCENE_RENDERING_MODE SceneRenderingMode = 
		SCENE_MANAGER.getSceneRenderingMode();

	strw = scn::SceneRenderingModeString[SceneRenderingMode];
	xml->writeElement(L"RenderingMode", true, L"value", strw.c_str());
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%f", scene_props.Scene.LODDistances[scn::ELL_LOD_NEAREST] );
	swprintf(strw2, MAX_STRLEN, L"%f", scene_props.Scene.LODDistances[scn::ELL_LOD_NEAR   ] );
	swprintf(strw3, MAX_STRLEN, L"%f", scene_props.Scene.LODDistances[scn::ELL_LOD_FAR    ] );
	xml->writeElement(L"LODDistances", true, L"nearest", strw1, L"near", strw2, L"far", strw3);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalAmbientColor.r*255) );
	swprintf(strw2, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalAmbientColor.g*255) );
	swprintf(strw3, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalAmbientColor.b*255) );
	swprintf(strw4, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalAmbientColor.a*255) );
	xml->writeElement(L"GlobalAmbientColor", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	xml->writeElement(L"GlobalLight", false, L"cast_shadows", scene_props.Video.GlobalLight.CastShadows ? L"1" : L"0");
	xml->writeLineBreak();
	
	swprintf(strw1, MAX_STRLEN, L"%f", -scene_props.Video.GlobalLight.Position.X);
	swprintf(strw2, MAX_STRLEN, L"%f", -scene_props.Video.GlobalLight.Position.Y);
	swprintf(strw3, MAX_STRLEN, L"%f", -scene_props.Video.GlobalLight.Position.Z);
	xml->writeElement(L"Direction", true, L"x", strw1, L"y", strw2, L"z", strw3);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.AmbientColor.r*255) );
	swprintf(strw2, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.AmbientColor.g*255) );
	swprintf(strw3, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.AmbientColor.b*255) );
	swprintf(strw4, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.AmbientColor.a*255) );
	xml->writeElement(L"Ambient", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.DiffuseColor.r*255) );
	swprintf(strw2, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.DiffuseColor.g*255) );
	swprintf(strw3, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.DiffuseColor.b*255) );
	swprintf(strw4, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.DiffuseColor.a*255) );
	xml->writeElement(L"Diffuse", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.SpecularColor.r*255) );
	swprintf(strw2, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.SpecularColor.g*255) );
	swprintf(strw3, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.SpecularColor.b*255) );
	swprintf(strw4, MAX_STRLEN, L"%d", s32(scene_props.Video.GlobalLight.SpecularColor.a*255) );
	xml->writeElement(L"Specular", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	xml->writeClosingTag(L"GlobalLight");
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%d", scene_props.Video.ShadowColor.getRed() );
	swprintf(strw2, MAX_STRLEN, L"%d", scene_props.Video.ShadowColor.getGreen() );
	swprintf(strw3, MAX_STRLEN, L"%d", scene_props.Video.ShadowColor.getBlue() );
	swprintf(strw4, MAX_STRLEN, L"%d", scene_props.Video.ShadowColor.getAlpha() );
	xml->writeElement(L"ShadowColor", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	xml->writeElement(L"DistanceFog");
	xml->writeLineBreak();	

	swprintf(strw1, MAX_STRLEN, L"%d", scene_props.Video.DistanceFog.Color.getRed());
	swprintf(strw2, MAX_STRLEN, L"%d", scene_props.Video.DistanceFog.Color.getGreen());
	swprintf(strw3, MAX_STRLEN, L"%d", scene_props.Video.DistanceFog.Color.getBlue());
	swprintf(strw4, MAX_STRLEN, L"%d", scene_props.Video.DistanceFog.Color.getAlpha());

	xml->writeElement(
		L"Color", true, 		
		L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4
		);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%.4f", scene_props.Video.DistanceFog.Start);
	swprintf(strw2, MAX_STRLEN, L"%.4f", scene_props.Video.DistanceFog.End);
	swprintf(strw3, MAX_STRLEN, L"%.4f", scene_props.Video.DistanceFog.Density);

	xml->writeElement(
		L"Properties", true, 
		L"type", core::stringw(vid::FogTypeName[scene_props.Video.DistanceFog.Type]).c_str(),
		L"start", strw1, L"end", strw2, L"density", strw3
		);
	xml->writeLineBreak();

	xml->writeClosingTag(L"DistanceFog");
	xml->writeLineBreak();

	xml->writeElement(L"GroundFog");
	xml->writeLineBreak();	

	swprintf(strw1, MAX_STRLEN, L"%d", scene_props.Video.GroundFogColor.getRed());
	swprintf(strw2, MAX_STRLEN, L"%d", scene_props.Video.GroundFogColor.getGreen());
	swprintf(strw3, MAX_STRLEN, L"%d", scene_props.Video.GroundFogColor.getBlue());
	swprintf(strw4, MAX_STRLEN, L"%d", scene_props.Video.GroundFogColor.getAlpha());
	xml->writeElement(L"Color", true, L"red", strw1, L"green", strw2, L"blue", strw3, L"alpha", strw4);
	xml->writeLineBreak();

	xml->writeClosingTag(L"GroundFog");
	xml->writeLineBreak();

	u32 i=0, g;

	xml->writeElement(L"GameNodesCount");
	xml->writeLineBreak();		

	for (g=0; g<EGNT_UNITS_FORMATION; g++)
	{		
		E_GAME_NODE_TYPE gntype = (E_GAME_NODE_TYPE)g;

		core::stringw gn_cnt_strw;
		gn_cnt_strw.sprintf("%d", GAME_MANAGER.getGameNodesCount(gntype));

		xml->writeElement(
			core::stringw(GameNodeTypeStr[gntype]).c_str(), 
			true, L"count", gn_cnt_strw.c_str()
			);
		xml->writeLineBreak();
	}

	xml->writeClosingTag(L"GameNodesCount");
	xml->writeLineBreak();

	xml->writeClosingTag(L"SceneProperties");
	xml->writeLineBreak();
	xml->writeLineBreak();
	
	for (g=0; g<EGNT_UNITS_FORMATION; g++)
	{
		for (i=0; i<GAME_MANAGER.getGameNodesCount((E_GAME_NODE_TYPE)g); i++)
		{
			saveGameNodeCommonParams(
				xml, GAME_MANAGER.getGameNode((E_GAME_NODE_TYPE)g, i)
				);	
		}
	}

    xml->writeClosingTag(L"GameScene");    
    xml->writeLineBreak();

	xml->drop();

	if (SceneSaveWarnings!=0)
		Msg.sprintf("Scene saved to %s, but with %d warnings!", xmlFileName.c_str(), SceneSaveWarnings);
	else
		Msg.sprintf("Scene succesfully saved to %s", xmlFileName.c_str());

    LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
