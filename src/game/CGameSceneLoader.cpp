//|-------------------------------------------------------------------------
//| File:        CGameSceneLoader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameSceneLoader.h"

#include <scn/ISceneManager.h>
#include <dyn/IDynamicManager.h>
#include <io/IFileSystem.h>
#include <vid/IVideoDriver.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <game/IGameManager.h>
#include <img/IImageLibrary.h>
#include <res/IResourceManager.h>
#include <scn/IMesh.h>
#include <game/SGameSceneProps.h>
#include <game/IGameNodeActivatable.h>
#include <game/IGameNodeButton.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! constructor
CGameSceneLoader::CGameSceneLoader() 
{
#if MY_DEBUG_MODE  
	setClassName("CGameSceneLoader");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameSceneLoader::~CGameSceneLoader()
{
}

//----------------------------------------------------------------------------

//! loads game node common parameters from the opened xml-file
SGameNodeCommonParams CGameSceneLoader::loadGameNodeCommonParams(
	io::IXMLReader* xml, core::stringw node_name
	)
{
	SGameNodeCommonParams node_params;

	core::stringw xml_node_name;	

	int xml_elem_cnt =0;

	bool SceneTagFinded = false;

	while(xml && xml_elem_cnt<3 && xml->read())
    // парсим xml - файл
    {
		switch(xml->getNodeType())
        {
        case io::EXN_ELEMENT:
            {
				xml_elem_cnt++;
				xml_node_name = xml->getName();

				if (core::stringw("Scene") == xml_node_name)
                {
					SceneTagFinded = true;					
				}
				else
				if (SceneTagFinded && core::stringw("Transformation") == xml_node_name)
                {				
					node_params.Scene.Transformation = 
						SCENE_MANAGER.loadSceneNodeTransformation(xml);
                }
				else 
				if (SceneTagFinded && core::stringw("Parameters") == xml_node_name)
                {
					node_params.Scene.Parameters = 
						SCENE_MANAGER.loadSceneNodeParameters(xml);
				}				
				else
				{
					xml_elem_cnt=3;
					break;
				}
            }  
            break;
        }
    }

	return node_params;
}

//----------------------------------------------------------------------------

struct SGameNodeLoadData
{
	core::stringc				xml_filename;
	core::stringc				embedded_xml_file;
	game::E_GAME_NODE_TYPE		gntype;
	game::SGameNodeCommonParams	node_params;
};

//! loading scene from xml-file
bool CGameSceneLoader::loadGameScene(core::stringc xmlFileName)
{
	s32 start = TIMER.getSystemTime();

	core::stringc Msg;

	io::IXMLReader* xml =
		FILE_SYSTEM.createXMLReader(xmlFileName.c_str());

    if (!xml) 
    {   
        Msg.sprintf("Can't open xml-file %s", xmlFileName.c_str());
        LOGGER.log(Msg.c_str(), io::ELL_WARNING);
        return false;
    }

	scn::ISceneManager & smgr = SCENE_MANAGER;
	game::IGameManager & gmgr = GAME_MANAGER;

	gmgr.showMessageBox(true, "Please wait", "Loading . . .");

	core::stringc fullFileName = xmlFileName;
    core::stringc basePath = core::extractFilePath(fullFileName);

	io::E_LOG_LEVEL log_level = LOGGER.getLogLevel();

	LOGGER.setLogLevel(io::ELL_INFORMATION);
	LOGGER.log("");
	LOGGER.log("-----------------------------------------------------------------------");
    Msg.sprintf("Loading Scene from xml-file %s . . .", xmlFileName.c_str());
    LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);
	LOGGER.log("-----------------------------------------------------------------------");
	LOGGER.log("");
	LOGGER.setLogLevel(log_level);

	core::stringw xml_node_name;

	bool ScenePropsTagFounded = false;
	bool DistanceFogTagFounded = false;
	bool GroundFogTagFounded = false;
	bool SoundTagFounded = false;
	bool GlobalLightTagFounded = false;
	bool GameNodesCountTagFounded = false;	
	core::stringw GameScriptFileName=NONAME_FILE_NAME;

	vid::SFog fog_load;
	img::SColor ground_fog_color_load;
	img::SColorf global_ambient_color_load =
		img::SColorf(0.25f, 0.25f, 0.25f, 1.0f);
	scn::E_SCENE_RENDERING_MODE SceneRenderingMode = scn::ESRM_INDOOR;
	core::stringc SceneRenderingModeString;
	vid::SLight global_light_load;
	img::SColor shadow_color_load(90, 0, 0, 0);

	f32 lod_distances[scn::E_LOD_LEVEL_COUNT] =
		{ 1000.0f, 2000.0f, 3000.0f, 1000000.0f};

	s32 game_nodes_count_loaded[E_GAME_NODE_TYPE_COUNT];
	s32 game_nodes_count_waiting[E_GAME_NODE_TYPE_COUNT];
	for (u32 g = 0; g < E_GAME_NODE_TYPE_COUNT; g++)
		game_nodes_count_loaded[g] = game_nodes_count_waiting[g] = 0;

	core::array < SGameNodeLoadData > game_nodes_load_data[E_GAME_NODE_TYPE_COUNT];
	core::array < SParentConnectionEntry > ParentConnections;
	core::array < core::stringc > SoundTracksFileNames;

    while(xml && xml->read())
    {
		switch(xml->getNodeType())
        {
        case io::EXN_ELEMENT:
            {
				xml_node_name = xml->getName();				

				if (core::stringw("GameScript") == xml_node_name)
				{
					GameScriptFileName = xml->getAttributeValue(L"filename");
				}
				else if (core::stringw("GameSound") == xml_node_name)
				{
					SoundTagFounded = true;
				}
				else if (
					core::stringw("SoundTrack") == xml_node_name && SoundTagFounded)
				{
					core::stringc fname = xml->getAttributeValue(L"filename");

					if (fname.size()>0)
						SoundTracksFileNames.push_back(fname);				
				}
				else if (
					core::stringw("SceneProperties") == xml_node_name)
				{
					ScenePropsTagFounded = true;

					DistanceFogTagFounded = GroundFogTagFounded = 
					GlobalLightTagFounded = GameNodesCountTagFounded = false;
				}
				else if (
					core::stringw("DistanceFog") == xml_node_name &&
					ScenePropsTagFounded)
				{
					DistanceFogTagFounded = true;
					GroundFogTagFounded   = false;
					GlobalLightTagFounded    = false;
					GameNodesCountTagFounded = false;
				}
				else if (
					core::stringw("GroundFog") == xml_node_name &&
					ScenePropsTagFounded)
				{
					DistanceFogTagFounded = false;
					GroundFogTagFounded   = true;
					GlobalLightTagFounded    = false;
					GameNodesCountTagFounded = false;
				}
				else if (core::stringw("GlobalLight") == xml_node_name &&
					ScenePropsTagFounded)
				{
					DistanceFogTagFounded = false;
					GroundFogTagFounded   = false;
					GlobalLightTagFounded    = true;
					GameNodesCountTagFounded = false;

					global_light_load.CastShadows =
						xml->getAttributeValueAsInt(L"cast_shadows")==0 ? false : true;
				}
				else if (
					core::stringw("GameNodesCount") == xml_node_name &&
					ScenePropsTagFounded)
				{
					DistanceFogTagFounded = false;
					GroundFogTagFounded   = false;
					GlobalLightTagFounded    = false;
					GameNodesCountTagFounded = true;					
				}
				else if (
					core::stringw("Color") == xml_node_name &&
					ScenePropsTagFounded && DistanceFogTagFounded 
					)
				{
					fog_load.Color = img::SColor (
						xml->getAttributeValueAsInt(L"alpha"),
						xml->getAttributeValueAsInt(L"red"  ),
						xml->getAttributeValueAsInt(L"green"),
						xml->getAttributeValueAsInt(L"blue" ));					
				}
				else if (
					core::stringw("Color") == xml_node_name &&
					ScenePropsTagFounded && GroundFogTagFounded
					)
				{
					ground_fog_color_load = img::SColor (
						xml->getAttributeValueAsInt(L"alpha"),
						xml->getAttributeValueAsInt(L"red"  ),
						xml->getAttributeValueAsInt(L"green"),
						xml->getAttributeValueAsInt(L"blue" ));
				}
				else if (
					core::stringw("Properties") == xml_node_name &&
					ScenePropsTagFounded && DistanceFogTagFounded)
				{
					core::stringc fog_type = xml->getAttributeValue(L"type");

					for (s32 ft=0; ft<vid::E_FOG_TYPE_COUNT; ft++)
					{
						if (fog_type == vid::FogTypeName[ft])
						{
							fog_load.Type = (vid::E_FOG_TYPE)ft;
							break;
						}
					}	

					fog_load.Start   = xml->getAttributeValueAsFloat(L"start");
					fog_load.End     = xml->getAttributeValueAsFloat(L"end");
					fog_load.Density = xml->getAttributeValueAsFloat(L"density");
				}				
				else if (
					ScenePropsTagFounded &&
					core::stringw("RenderingMode") == xml_node_name)
				{
					SceneRenderingModeString = xml->getAttributeValue(L"value");

					for (s32 srm=0; srm<scn::E_SCENE_RENDERING_MODE_COUNT; srm++)
					{
						if (SceneRenderingModeString == scn::SceneRenderingModeString[srm])
						{
							SceneRenderingMode = (scn::E_SCENE_RENDERING_MODE)srm;
							break;
						}
					}					
				}
				else if (
					ScenePropsTagFounded &&
					core::stringw("LODDistances") == xml_node_name)
				{
					lod_distances[scn::ELL_LOD_NEAREST]  = xml->getAttributeValueAsFloat(L"nearest");
					lod_distances[scn::ELL_LOD_NEAR]     = xml->getAttributeValueAsFloat(L"near");
					lod_distances[scn::ELL_LOD_FAR]      = xml->getAttributeValueAsFloat(L"far");
				}
				else if (
					ScenePropsTagFounded &&
					core::stringw("GlobalAmbientColor") == xml_node_name)
				{
					global_ambient_color_load = img::SColorf(						
						f32(xml->getAttributeValueAsInt(L"red"  )) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"green")) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"blue" )) / 255.0f,		
						f32(xml->getAttributeValueAsInt(L"alpha")) / 255.0f);
				}
				else if (
					ScenePropsTagFounded &&
					core::stringw("ShadowColor") == xml_node_name)
				{
					shadow_color_load = img::SColor(	
						xml->getAttributeValueAsInt(L"alpha"),
						xml->getAttributeValueAsInt(L"red"  ),
						xml->getAttributeValueAsInt(L"green"),
						xml->getAttributeValueAsInt(L"blue" ));
				}				
				else if (
					core::stringw("Direction") == xml_node_name &&
					ScenePropsTagFounded && GlobalLightTagFounded)
				{
					global_light_load.Position = core::vector3df(						
						-f32(xml->getAttributeValueAsFloat(L"x", 0.f)),
						-f32(xml->getAttributeValueAsFloat(L"y", 0.f)),
						-f32(xml->getAttributeValueAsFloat(L"z", 0.f)));
					if (global_light_load.Position != core::vector3df(0,0,0))
						global_light_load.Enabled = true;
				}
				else if (
					core::stringw("Ambient") == xml_node_name &&
					ScenePropsTagFounded && GlobalLightTagFounded 
					)
				{
					global_light_load.AmbientColor = img::SColorf(						
						f32(xml->getAttributeValueAsInt(L"red"  )) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"green")) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"blue" )) / 255.0f,		
						f32(xml->getAttributeValueAsInt(L"alpha")) / 255.0f);
				}
				else if (
					core::stringw("Diffuse") == xml_node_name &&
					ScenePropsTagFounded && GlobalLightTagFounded)
				{
					global_light_load.DiffuseColor = img::SColorf(						
						f32(xml->getAttributeValueAsInt(L"red"  )) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"green")) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"blue" )) / 255.0f,		
						f32(xml->getAttributeValueAsInt(L"alpha")) / 255.0f);
				}
				else if (
					core::stringw("Specular") == xml_node_name &&
					ScenePropsTagFounded && GlobalLightTagFounded)
				{
					global_light_load.SpecularColor = img::SColorf(						
						f32(xml->getAttributeValueAsInt(L"red"  )) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"green")) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"blue" )) / 255.0f,		
						f32(xml->getAttributeValueAsInt(L"alpha")) / 255.0f);								
				}
				else if (
					core::stringw("Specular") == xml_node_name &&
					ScenePropsTagFounded && GlobalLightTagFounded)
				{
					global_light_load.SpecularColor = img::SColorf(						
						f32(xml->getAttributeValueAsInt(L"red"  )) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"green")) / 255.0f,
						f32(xml->getAttributeValueAsInt(L"blue" )) / 255.0f,		
						f32(xml->getAttributeValueAsInt(L"alpha")) / 255.0f);								
				}
				else if (					
					ScenePropsTagFounded && GameNodesCountTagFounded)
				{
					for (s32 gnt=0; gnt<game::E_GAME_NODE_TYPE_COUNT; ++gnt)
						if (xml_node_name == core::stringw(game::GameNodeTypeStr[gnt]))
						{
							game_nodes_count_waiting[gnt] = xml->getAttributeValueAsInt(L"count");
							break;														
						}
				}
	
				if (core::stringw("GameNode") == xml_node_name	)
				{
					// loading game node data

					core::stringc gntype_str = xml->getAttributeValue(L"game_node_type");
					E_GAME_NODE_TYPE gntype = game::EGNT_UNKNOWN;

					for (s32 gnt=0; gnt < game::E_GAME_NODE_TYPE_COUNT; ++gnt)
						if (gntype_str == game::GameNodeTypeStr[gnt])
						{
							gntype = (game::E_GAME_NODE_TYPE)gnt;
							break;
						}

					game_nodes_load_data[gntype].push_back(SGameNodeLoadData());

					SGameNodeLoadData &gnode_load_data =
						game_nodes_load_data[gntype][game_nodes_load_data[gntype].size()-1];

					gnode_load_data.gntype = game::EGNT_UNKNOWN;

					gnode_load_data.xml_filename = xml->getAttributeValue(L"filename");

					gnode_load_data.node_params = loadGameNodeCommonParams(xml, xml_node_name);	
					gnode_load_data.node_params.Game.Type = gntype;
					gnode_load_data.node_params.Dynamic.ControledByPlayer = false;

					if (gnode_load_data.xml_filename == core::stringc(NONAME_FILE_NAME).c_str())
					{
						s32 start_xml_pos=-1, end_xml_pos=-1;
						while(xml->read())
						{
							if (xml->getNodeType()==io::EXN_ELEMENT)
							{
								xml_node_name = xml->getName();				
								if (core::stringw("EmbeddedXMLFileBegin") == xml_node_name)
									start_xml_pos = xml->getPos();
								else 
								if (core::stringw("EmbeddedXMLFileEnd") == xml_node_name)
									break;
							}
							end_xml_pos   = xml->getPos();
						}

						gnode_load_data.embedded_xml_file =
							xml->getData(start_xml_pos, end_xml_pos).c_str();
					}
				}
			}
			break;
		}
	}

	if (xml) xml->drop(); 	

	ScenePropsTagFounded = DistanceFogTagFounded = GroundFogTagFounded = 
		SoundTagFounded = GlobalLightTagFounded = GameNodesCountTagFounded = false;

	// setting scene properties
	{
		SGameSceneProps scene_props;

		// Scene
		for (s32 ld=0; ld<scn::E_LOD_LEVEL_COUNT; ld++)
			scene_props.Scene.LODDistances[ld] = lod_distances[ld];

		// Video
		scene_props.Video.DistanceFog		= fog_load;
		scene_props.Video.GlobalAmbientColor= global_ambient_color_load;
		scene_props.Video.GlobalLight		= global_light_load;
		scene_props.Video.GroundFogColor	= ground_fog_color_load;
		scene_props.Video.ShadowColor = shadow_color_load;

		gmgr.setGameSceneProps(scene_props);

		smgr.setSceneRenderingMode(SceneRenderingMode);		
	}

	E_GAME_NODE_TYPE gnode_load_order[] =
	{
		EGNT_LIGHT,
		EGNT_MAIN_PLAYER,
		EGNT_PERSON,
		EGNT_DECORATION,
		EGNT_LEVEL_MAP,
		EGNT_TERRAIN,
		EGNT_SKY_DOME,
		EGNT_TRIGGER,
		EGNT_ITEM
	};

	u32 gntcnt = sizeof(gnode_load_order) / sizeof(*gnode_load_order);
	for (u32 gnt = 0; gnt < gntcnt; gnt++)
	{
		E_GAME_NODE_TYPE gntype = gnode_load_order[gnt];
		u32 gncnt = game_nodes_load_data[gntype].size();
		for (u32 gn = 0; gn < gncnt; gn++)
		{
			SGameNodeLoadData & gnode_load_data = game_nodes_load_data[gntype][gn];
			SGameNodeCommonParams & node_params = gnode_load_data.node_params;
			core::stringc & filename = gnode_load_data.xml_filename;

			// loading game node

			LOGGER.setIdentityFormatLevel();	

			IGameNode *gnode = NULL;

			core::stringc fname_only =
				core::extractFileName(gnode_load_data.xml_filename);
			
			Msg.sprintf("%s (%d / %d)\n%s", 
				GameNodeTypeShortStr[gntype],
				game_nodes_count_loaded[gntype]+1,
				game_nodes_count_waiting[gntype],
				fname_only.c_str());

			gmgr.showMessageBox(true,
				"Please wait, while loading", Msg.c_str());

			if (gnode_load_data.embedded_xml_file.size() == 0)
			{
				if (gntype == EGNT_MAIN_PLAYER)
				{
					node_params.Dynamic.ControledByPlayer = true;
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_PERSON)
				{		
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_DECORATION)
				{
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}	
				else if (gntype == EGNT_LEVEL_MAP)
				{
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_TERRAIN)
				{
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_SKY_DOME)  
				{
					node_params.Scene.Parameters.Type = scn::ESNT_SKY_BOX_SCENE_NODE;
					node_params.Dynamic.Type = dyn::EDOT_NONE;						
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_LIGHT)
				{
					node_params.Scene.Parameters.Type = scn::ESNT_LIGHT_SCENE_NODE;
					node_params.Dynamic.Type = dyn::EDOT_NONE;						
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_TRIGGER)
				{		
					node_params.Scene.Parameters.Type = scn::ESNT_TEST_SCENE_NODE;
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else if (gntype == EGNT_ITEM)
				{		
					gnode = gmgr.loadGameNode(node_params, filename.c_str());
				}
				else
				{
					Msg.sprintf( "Can't load Game Node '%s'", GameNodeTypeStr[gntype]);
					LOGGER.log(Msg.c_str(), io::ELL_WARNING);
				}
			}
			else
			{
				core::stringc & str = gnode_load_data.embedded_xml_file;
				
				c8 *arr = new c8[str.size()+1];
				memcpy(arr, str.c_str(), str.size()*sizeof(c8));
				arr[str.size()]=0;

				io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
					(void*)arr, str.size()*sizeof(c8), NONAME_FILE_NAME, false);
				gnode = gmgr.loadGameNode(node_params, file);

				SAFE_DROP(file);
				SAFE_DELETE_ARRAY(arr);                 
			}
			
			if (gnode && gnode->getSceneNode())
			{
				ParentConnections.push_back(SParentConnectionEntry(
					gnode->getSceneNode(), node_params.Scene.Parameters.ParentID));
			}

			game_nodes_count_loaded[gntype] += 1;
		}
	}

	DYNAMIC_MANAGER.updateGeometryFromDynamic();

	LOGGER.setIdentityFormatLevel();

	// setting game script name

	IGameManager::getSingleton().setGameScriptFileName(
		core::stringc(GameScriptFileName.c_str()));

	// adding sounds

	core::stringc sound_dir = 
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_TRACKS );

	LOGGER.logInfo("");
	LOGGER.logInfo("Loading soundtracks:");

	LOGGER.increaseFormatLevel();

	for (u32 s=0; s<SoundTracksFileNames.size(); s++)
	{
		Msg.sprintf("Soundtrack (%d / %d)\n%s",
			s+1, SoundTracksFileNames.size(),
			SoundTracksFileNames[s].c_str());
		gmgr.showMessageBox(true, "Please wait, while loading",
			Msg.c_str());

		core::stringc full_filename = sound_dir.c_str();
		full_filename.append(SoundTracksFileNames[s].c_str());

		gmgr.addSoundTrack(full_filename);			
	}

	LOGGER.decreaseFormatLevel();

	// setting parents to scene nodes

	for (u32 i=0; i<ParentConnections.size(); i++)
	{
		scn::ISceneNode *parent = smgr.getSceneNodeFromId(ParentConnections[i].parent_id);
		scn::ISceneNode *child  = ParentConnections[i].child;
		if (parent && child)
			child->setParent(parent);
	}

	// clearing images cache
	IMAGE_LIBRARY.clearUnusedImageCache();
	VIDEO_DRIVER.clearUnusedVideoCache();

	const SGameSceneProps &scene_prop = gmgr.getGameSceneProps();

	LOGGER.logInfo("");
	LOGGER.logInfo("Scene properties:");
	LOGGER.logInfo("Rendering Mode: %s", scn::SceneRenderingModeString[SceneRenderingMode]);
	LOGGER.logInfo("LOD Distances: ");
	LOGGER.logInfo("Nearest=%.4f, Near=%.4f, Far=%.4f, Farthest=%.4f",
		scene_prop.Scene.LODDistances[scn::ELL_LOD_NEAREST],
		scene_prop.Scene.LODDistances[scn::ELL_LOD_NEAR],
		scene_prop.Scene.LODDistances[scn::ELL_LOD_FAR],
		scene_prop.Scene.LODDistances[scn::ELL_LOD_FARTHEST]);

	LOGGER.logInfo("Global Ambient Color: RGBA { %d, %d, %d, %d }, RGBA float { %.2f, %.2f, %.2f, %.2f }",
		s32(scene_prop.Video.GlobalAmbientColor.r*255),
		s32(scene_prop.Video.GlobalAmbientColor.g*255),
		s32(scene_prop.Video.GlobalAmbientColor.b*255),
		s32(scene_prop.Video.GlobalAmbientColor.a*255),
		scene_prop.Video.GlobalAmbientColor.r,
		scene_prop.Video.GlobalAmbientColor.g,
		scene_prop.Video.GlobalAmbientColor.b,
		scene_prop.Video.GlobalAmbientColor.a);

	LOGGER.logInfo("Global Light params:");
	LOGGER.logInfo(" Cast Shadows: %d", scene_prop.Video.GlobalLight.CastShadows ? 1 : 0);
	LOGGER.logInfo(" Direction: X=%f, Y=%f, Z=%f",
		scene_prop.Video.GlobalLight.Position.X, scene_prop.Video.GlobalLight.Position.Y, scene_prop.Video.GlobalLight.Position.Z);
	LOGGER.logInfo(" Diffuse Color: RGBA { %d, %d, %d, %d }, RGBA float { %.2f, %.2f, %.2f, %.2f }",
		s32(scene_prop.Video.GlobalLight.DiffuseColor.r*255),
		s32(scene_prop.Video.GlobalLight.DiffuseColor.g*255),
		s32(scene_prop.Video.GlobalLight.DiffuseColor.b*255),
		s32(scene_prop.Video.GlobalLight.DiffuseColor.a*255),
		scene_prop.Video.GlobalLight.DiffuseColor.r,
		scene_prop.Video.GlobalLight.DiffuseColor.g,
		scene_prop.Video.GlobalLight.DiffuseColor.b,
		scene_prop.Video.GlobalLight.DiffuseColor.a);
	LOGGER.logInfo(" Ambient Color: RGBA { %d, %d, %d, %d }, RGBA float { %.2f, %.2f, %.2f, %.2f }",
		s32(scene_prop.Video.GlobalLight.AmbientColor.r*255),
		s32(scene_prop.Video.GlobalLight.AmbientColor.g*255),
		s32(scene_prop.Video.GlobalLight.AmbientColor.b*255),
		s32(scene_prop.Video.GlobalLight.AmbientColor.a*255),
		scene_prop.Video.GlobalLight.AmbientColor.r,
		scene_prop.Video.GlobalLight.AmbientColor.g,
		scene_prop.Video.GlobalLight.AmbientColor.b,
		scene_prop.Video.GlobalLight.AmbientColor.a);
	LOGGER.logInfo(" Specular Color: RGBA { %d, %d, %d, %d }, RGBA float { %.2f, %.2f, %.2f, %.2f }",
		s32(scene_prop.Video.GlobalLight.SpecularColor.r*255),
		s32(scene_prop.Video.GlobalLight.SpecularColor.g*255),
		s32(scene_prop.Video.GlobalLight.SpecularColor.b*255),
		s32(scene_prop.Video.GlobalLight.SpecularColor.a*255),
		scene_prop.Video.GlobalLight.SpecularColor.r,
		scene_prop.Video.GlobalLight.SpecularColor.g,
		scene_prop.Video.GlobalLight.SpecularColor.b,
		scene_prop.Video.GlobalLight.SpecularColor.a);

	LOGGER.logInfo("Shadows Color: RGBA (%d, %d, %d, %d)",
		scene_prop.Video.ShadowColor.getRed(), scene_prop.Video.ShadowColor.getGreen(), 
		scene_prop.Video.ShadowColor.getBlue(), scene_prop.Video.ShadowColor.getAlpha());

	LOGGER.logInfo("Distance Fog: "
		"Type=%s, Color RGBA (%d, %d, %d, %d), Start=%f, End=%f, Density=%f",
		vid::FogTypeName[scene_prop.Video.DistanceFog.Type],
		scene_prop.Video.DistanceFog.Color.getRed(),
		scene_prop.Video.DistanceFog.Color.getGreen(),
		scene_prop.Video.DistanceFog.Color.getBlue(),
		scene_prop.Video.DistanceFog.Color.getAlpha(), 
		scene_prop.Video.DistanceFog.Start, scene_prop.Video.DistanceFog.End, scene_prop.Video.DistanceFog.Density);
	LOGGER.logInfo("Ground Fog: Color RGBA (%d, %d, %d, %d)",
		scene_prop.Video.GroundFogColor.getRed(),scene_prop.Video. GroundFogColor.getGreen(),
		scene_prop.Video.GroundFogColor.getBlue(), scene_prop.Video.GroundFogColor.getAlpha());

	// scene loaded

	s32 end = TIMER.getSystemTime();

	LOGGER.setLogLevel(io::ELL_INFORMATION);
	LOGGER.logInfo("");
	LOGGER.logInfo("-----------------------------------------------------------------------");
    LOGGER.logInfo("Scene successfully loaded from %s at %.2f sec",
		xmlFileName.c_str(), (f32)(end-start)/1000.0f);
	LOGGER.logInfo("-----------------------------------------------------------------------");
	LOGGER.logInfo("");
	LOGGER.setLogLevel(log_level);

	gmgr.showMessageBox(false);

	return true;
}
//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------