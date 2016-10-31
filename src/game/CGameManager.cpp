//|-------------------------------------------------------------------------
//| File:        CGameManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameManager.h"
#include "CGameAIModule.h"
#include "CGameEventsDispatcher.h"
#include "CGameTasksManager.h"

#include "CGameUnitsFormation.h"
#include "CGameInventory.h"

#include "CGameSceneSaver.h"
#include "CGameSceneLoader.h"

#include "CPathFindTask.h"

#include <img/IImageLibrary.h>
#include <io/IFileSystem.h>
#include <scn/ISceneManager.h>
#include <dyn/IDynamicManager.h>
#include <mm/ISoundDriver.h>
#include <scr/IScriptManager.h>
#include <mm/ISound.h>
#include <os/ITimer.h>
#include <io/ILogger.h>
#include <io/ICursorControl.h>
#include <res/IResourceManager.h>
#include <scn/IPathFinderManager.h>
#include <io/IInputDispatcher.h>
#include <dev/IDevice.h>
#include <dev/IProfiler.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {	
//----------------------------------------------------------------------------
	
core::list<IGameNode*>   CGameManager::GameNodesByType[E_GAME_NODE_TYPE_COUNT];
core::list<IGameNode*>   CGameManager::GameNodes;
core::list<IGameNodeAI*> CGameManager::GameNodesAI;

//----------------------------------------------------------------------------

CGameManager::CGameManager () 
	: CurrentMainPlayer(NULL), m_GameStopped(true), GameScriptFileName(NONAME_FILE_NAME), 
RemainDeltaSeconds(0.0f), GameSceneFileName(NONAME_GAMESCENE_FILE_NAME),
m_CallbackOnShowMessage(NULL), 
m_CallbackBeforeLoadScene(NULL), 
m_CallbackAfterLoadScene(NULL),
m_CallbackBeforeSaveScene(NULL), 
m_CallbackAfterSaveScene(NULL), 
m_CallbackOnCreateNode(NULL), 
m_CallbackOnDeleteNode(NULL), 
m_CallbackOnDieNode(NULL), 
m_CallbackOnCollectItem(NULL),
m_CallbackOnThrowItem(NULL),
m_CallbackOnUseItem(NULL),
m_NewGameSceneLoaded(false),
m_ActiveGameNodesCount(0),
m_DoGameCount(0),
m_Profiler(PROFILER),
m_ShiftTimeSec(0.f), m_StopTimeSec(0.f)
{
#if MY_DEBUG_MODE  
	IGameManager::setClassName("CGameManager");
#endif

	// grabbing all needed singletons

	m_Profiler.grab();
	DYNAMIC_MANAGER.grab();
	SOUND_DRIVER.grab();
	SCENE_MANAGER.grab();	
	VIDEO_DRIVER.grab();
	FILE_SYSTEM.grab();	
	INPUT_DISPATCHER.grab();

	// creating all game singletons

	new CGameAIModule();
	new CGameEventsDispatcher();
	new CGameTasksManager();

	// some intialization

	thisFrameTime = previousFrameTime = 0.0f;
	thisDeltaTime = previousDeltaTime = 0.0f;	

	// registering game nodes Standard classes

	registerGameNodeClass( "Standard", EGNT_MAIN_PLAYER,   Alloc__StandardMainPlayer,   Create_StandardMainPlayer,   Clone__StandardMainPlayer,   Load___StandardMainPlayer   );
	registerGameNodeClass( "Standard", EGNT_PERSON,        Alloc__StandardPerson,       Create_StandardPerson,       Clone__StandardPerson,       Load___StandardPerson       );
	registerGameNodeClass( "Standard", EGNT_DECORATION,    Alloc__StandardDecoration,   Create_StandardDecoration,   Clone__StandardDecoration,   Load___StandardDecoration   );
	registerGameNodeClass( "Standard", EGNT_LEVEL_MAP,     Alloc__StandardLevelMap,     Create_StandardLevelMap,     Clone__StandardLevelMap,     Load___StandardLevelMap     );
	registerGameNodeClass( "Standard", EGNT_TERRAIN,       Alloc__StandardTerrain,      Create_StandardTerrain,      Clone__StandardTerrain,      Load___StandardTerrain      );
	registerGameNodeClass( "Standard", EGNT_BUTTON,        Alloc__StandardButton,       Create_StandardButton,       Clone__StandardButton,       Load___StandardButton       );
	registerGameNodeClass( "Standard", EGNT_WEAPON,        Alloc__StandardWeapon,       Create_StandardWeapon,       Clone__StandardWeapon,       Load___StandardWeapon       );
	registerGameNodeClass( "Standard", EGNT_DOOR,          Alloc__StandardDoor,         Create_StandardDoor,         Clone__StandardDoor,         Load___StandardDoor         );
	registerGameNodeClass( "Standard", EGNT_PLATFORM,      Alloc__StandardPlatform,     Create_StandardPlatform,     Clone__StandardPlatform,     Load___StandardPlatform     );	
	registerGameNodeClass( "Standard", EGNT_TRIGGER,       Alloc__StandardTrigger,      Create_StandardTrigger,      Clone__StandardTrigger,      Load___StandardTrigger      );
	registerGameNodeClass( "Standard", EGNT_LIGHT,         Alloc__StandardLight,        Create_StandardLight,        Clone__StandardLight,        Load___StandardLight        );
	registerGameNodeClass( "Standard", EGNT_ITEM,          Alloc__StandardItem,         Create_StandardItem,         Clone__StandardItem,         Load___StandardItem         );	
	registerGameNodeClass( "Standard", EGNT_SKY_DOME,      Alloc__StandardSkyDome,      Create_StandardSkyDome,      Clone__StandardSkyDome,      Load___StandardSkyDome      );	

	setSoundTracksVolume(0.5f);
	setSoundEffectsVolume(0.5f);

	for (u32 i = 0; i < (u32)E_GAME_PROFILE_TYPES_COUNT; i++)
		m_ProfileIds[i]	= m_Profiler.allocProfileId(GameProfileTypeNames[i]);

	setGameSceneProps(getGameSceneProps());
}

//----------------------------------------------------------------------------

CGameManager::~CGameManager()
{
	stopGame();

	clearGameScene();

	m_Profiler.drop();
	DYNAMIC_MANAGER.drop();
	SOUND_DRIVER.drop();
	SCENE_MANAGER.drop();	
	INPUT_DISPATCHER.drop();
	VIDEO_DRIVER.drop();
	FILE_SYSTEM.drop();	
		
	GAME_AI_MODULE.drop();	
	GAME_EVENTS_DISPATCHER.drop();	
	GAME_TASK_MANAGER.drop();	
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::getGameNode(u32 idx)
{
	if (idx>=0 && idx<GameNodes.size())
	{
		core::list<IGameNode*>::iterator iter = 
			GameNodes.begin() + idx;
		IGameNode* game_node = (*iter);
		return game_node;
	}
	return NULL;	
}

//----------------------------------------------------------------------------

u32 CGameManager::getGameNodesCount()
{
	return GameNodes.size();
}

//----------------------------------------------------------------------------

u32 CGameManager::getActiveGameNodesCount()
{
	return m_ActiveGameNodesCount;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::getGameNode( E_GAME_NODE_TYPE gntype, u32 idx )
{
	if (idx >= 0 && idx<GameNodesByType[gntype].size())
	{
		core::list<IGameNode*>::iterator iter = 
			GameNodesByType[gntype].begin() + idx;
		IGameNode* game_node = (*iter);
		return game_node;
	}
	return NULL;	
}

//----------------------------------------------------------------------------

u32 CGameManager::getGameNodesCount(E_GAME_NODE_TYPE gntype)
{
	return GameNodesByType[gntype].size();
}

//----------------------------------------------------------------------------

E_GAME_NODE_TYPE CGameManager::getGameNodeType(IGameNode* game_node)
{
	E_GAME_NODE_TYPE gntype;	
	getGameNodeTypeAndIndex(game_node, &gntype);
	return gntype;
}

//----------------------------------------------------------------------------

u32 CGameManager::getGameNodeIndex(IGameNode* game_node)
{
	return getGameNodeTypeAndIndex(game_node);
}

//----------------------------------------------------------------------------

u32 CGameManager::getGameNodeTypeAndIndex(
	IGameNode* game_node, E_GAME_NODE_TYPE *gntype, u32 *idx)
{
	E_GAME_NODE_TYPE _gntype = EGNT_UNKNOWN; 
	u32 _idx =-1;
	IGameNode* gnode = NULL;		

	for (u32 gnt=0; game_node!=NULL && gnt<E_GAME_NODE_TYPE_COUNT; gnt++)
	{
		core::list<IGameNode*>::iterator iter = GameNodesByType[gnt].begin();

		u32 gn =0;

		for(; iter!= GameNodesByType[gnt].end(); ++iter, gn++)
		{
			gnode = *iter;		
			if (gnode && gnode == game_node)
			{
				_gntype = (E_GAME_NODE_TYPE)gnt;
				_idx = gn;
				break;
			}
		}
	}

	if (gntype) (*gntype)=_gntype;
	if (idx) (*idx)=_idx;	

	return _idx;
}

//----------------------------------------------------------------------------

void CGameManager::applyGameNodeCommonParams(
	scn::ISceneNode *node, SGameNodeCommonParams &node_params)
{
	if (!node)
		return;

	scn::E_SCENE_NODE_TYPE ntype = node->getSceneNodeType();

	// scene parameters
	node->setParameters(node_params.Scene);

	// dynamic parameters
	node->detachDynamicObject();
    if (ntype!=scn::ESNT_LIGHT_SCENE_NODE &&
			node_params.Dynamic.Type != dyn::EDOT_NONE)
		node->attachDynamicObject(node_params.Dynamic);	

	// game node parameters
	node->detachGameNode();	
	node->attachGameNode(node_params.Game);	

	// game node parameters
	IGameNode *gn = node->getGameNode();
	if (gn)
		gn->updateGameCommonParams();
}

//----------------------------------------------------------------------------

const core::list<IGameNode*>& CGameManager::getGameNodesByType(
	E_GAME_NODE_TYPE gntype)
{
	return GameNodesByType[gntype];
}

//----------------------------------------------------------------------------

const core::list<IGameNode*>& CGameManager::getGameNodes()
{
	return GameNodes;
}

//----------------------------------------------------------------------------

const core::list<IGameNodeAI*>& CGameManager::getGameNodesAI()
{
	return GameNodesAI;
}

//----------------------------------------------------------------------------

bool CGameManager::registerGameNodeClass(c8 *class_name, E_GAME_NODE_TYPE game_node_type,
	IGameNode* (*alloc_func )(scn::ISceneNode* n, SGameNodeParams &params),
	IGameNode* (*create_func)(SGameNodeCommonParams &node_params, bool log_info),
	IGameNode* (*clone_func )(IGameNode *game_node, bool log_info),
	IGameNode* (*load_func  )(SGameNodeCommonParams &node_params, io::IReadFile* file))
{
	core::stringc game_class_name(class_name);

	if (game_class_name.c_str()<=0 || 
		!alloc_func|| !create_func || !clone_func || !load_func)
		return false;

	LOGGER.logInfo("Registering %s game class '%s' . . .",
		GameNodeTypeStr[game_node_type], class_name);

	for (u32 i=0; i<GameClassesData[game_node_type].size(); i++)
	{
		if (GameClassesData[game_node_type][i].ClassName == game_class_name)
		{
			LOGGER.logErr(" Unable to register game class! Class already have been registered.");
			return false;
		}		
	}

	SGameClassData new_game_class_data;

	new_game_class_data.ClassName= class_name;
	new_game_class_data.Alloc = alloc_func;
	new_game_class_data.Create= create_func;
	new_game_class_data.Clone = clone_func;
	new_game_class_data.Load  = load_func;

	GameClassesData[game_node_type].push_back(new_game_class_data);

	LOGGER.logInfo(" Class successfully registered.");

	return false;
}

//----------------------------------------------------------------------------

IPathFindTask* CGameManager::createPathFindTask()
{
	return new game::CPathFindTask();
}

//----------------------------------------------------------------------------

const SGameSceneProps& CGameManager::getGameSceneProps()
{
	return GameSceneProps;
}

//----------------------------------------------------------------------------

void CGameManager::setGameSceneProps(const SGameSceneProps& props)
{
	GameSceneProps = props;

	// Scene
	for (s32 ld=0; ld<scn::E_LOD_LEVEL_COUNT; ld++)
		SCENE_MANAGER.setLODDistance(
			(scn::E_LOD_LEVEL)ld,  props.Scene.LODDistances[ld]);

	// Video
	VIDEO_DRIVER.setShadowColor( props.Video.ShadowColor );
	VIDEO_DRIVER.setGroundFogColor(props.Video.GroundFogColor);
	VIDEO_DRIVER.setFog(props.Video.DistanceFog);
	VIDEO_DRIVER.setGlobalAmbientColor(props.Video.GlobalAmbientColor);						
	VIDEO_DRIVER.setGlobalLightParams(props.Video.GlobalLight);	
}

//----------------------------------------------------------------------------

core::array<SItemDropTableLine>& CGameManager::getItemDropTable(E_GAME_NODE_TYPE type)
{
	if (type>=E_GAME_NODE_TYPE_COUNT)
		return *((core::array<SItemDropTableLine>*)NULL);  
	return m_ItemDropTables[type];
}

//----------------------------------------------------------------------------

IGameUnitsFormation* CGameManager::addGameUnitsFormation(
	core::dimension2di dim, s32 formation_id)
{
	IGameUnitsFormation* formation = new CGameUnitsFormation(dim, formation_id);	

	return formation;
}

//----------------------------------------------------------------------------

IGameUnitsFormation* CGameManager::getGameUnitsFormationFromID(s32 formation_id)
{
	core::list<IGameNode*>::iterator iter =
		GameNodesByType[EGNT_UNITS_FORMATION].begin();
    for(; iter!=GameNodesByType[EGNT_UNITS_FORMATION].end(); ++iter)
    {
        IGameNode* o=(*iter);
		if (o && o->getGameNodeType() == EGNT_UNITS_FORMATION) 
		{
			IGameUnitsFormation* f = static_cast<IGameUnitsFormation*>(o);
			if ( f && f->getFormationID() == formation_id )
				return f;
		}
    }
	return NULL;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::addGameNode(
	SGameNodeCommonParams &node_params, scn::ISceneNode* node)
{
	if (node) 
	{		
		applyGameNodeCommonParams(node, node_params);		
		return node->getGameNode();
	}
	return NULL;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::addGameNodeForSceneNode(scn::ISceneNode* n, SGameNodeParams &params)
{
	for (u32 i=0; i<GameClassesData[params.Type].size(); i++)
		if (params.ClassName == GameClassesData[params.Type][i].ClassName)
			return GameClassesData[params.Type][i].Alloc(n, params);
	LOGGER.logErr("Can't allocate %s game node, unknown class '%s' !",
		GameNodeTypeStr[params.Type], params.ClassName.c_str());
	return NULL;
}

//----------------------------------------------------------------------------

bool CGameManager::removeGameNodeForSceneNode(scn::ISceneNode* n)
{
	bool res = false;
	if (!n)
		return res;	
	IGameNode *game_node = n->getGameNode();
	if (game_node)
		res = game_node->drop();
	return res;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::createGameNode(
	E_GAME_NODE_TYPE game_node_type, scn::E_SCENE_NODE_TYPE scene_node_type, bool log_info)
{
	SGameNodeCommonParams node_params;
	node_params.Game.Type = game_node_type;
	node_params.Scene.Parameters.Type = scene_node_type;
	return createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::createGameNode(SGameNodeCommonParams &node_params, bool log_info)
{
	IGameNode* gn = 0;
	E_GAME_NODE_TYPE gntype = node_params.Game.Type;
	for (u32 i=0; i<GameClassesData[gntype].size(); i++)
	{
		if (node_params.Game.ClassName == GameClassesData[gntype][i].ClassName)
		{
			gn = GameClassesData[gntype][i].Create(node_params, log_info);
			break;
		}
	}
	if (!gn)
	{
		LOGGER.logErr("Can't create %s game node, unknown class '%s' !",
			GameNodeTypeStr[gntype], node_params.Game.ClassName.c_str());
	}
	return gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::cloneGameNode(IGameNode* game_node, bool log_info)
{
	if (!game_node)
		return 0;

	IGameNode* gn = 0;	

	E_GAME_NODE_TYPE gntype = game_node->getGameNodeType();

	for (u32 i=0; i<GameClassesData[gntype].size(); i++)
	{
		if (GameClassesData[gntype][i].ClassName == game_node->getGameClassName() )
		{
			gn = GameClassesData[gntype][i].Clone(game_node, log_info);
			break;
		}
	}

	if (!gn)
	{
		LOGGER.logErr("Can't clone %s game node, unknown class '%s' !",
			GameNodeTypeStr[gntype], game_node->getGameClassName());
	}
	else
		_createDropItemsForGameNode(gn);

	return gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::reloadGameNode(IGameNode* gn)
{
	if (!gn)
		return NULL;

	scn::ISceneNode *sn = gn->getSceneNode();
	if (!sn)
		return NULL;

	bool dbg_vis = sn->isDebugDataVisible();

	scn::ISceneNode* parent = sn->getParent();
	core::stringc file_name = sn->getFileName();
	core::stringc node_name = sn->getName();	

	gn->updateGameCommonParams();

	SGameNodeCommonParams node_params = gn->getGameCommonParams();

	// removing scene node with its game node

	if (sn->remove())	LOGGER.logInfo("Node '%s' was dropped", node_name.c_str());	
	else                LOGGER.logInfo("Can't drop '%s' Node",  node_name.c_str());	

	// removing scene node etalon, if it exists

	scn::ISceneNode *etalon_snode = 
		SCENE_MANAGER.findSceneNodeEtalon(file_name.c_str());
	if (etalon_snode)
		SCENE_MANAGER.removeSceneNodeEtalon(file_name.c_str());

	// loading scene node again

	io::IFileSystem       &fsys   = FILE_SYSTEM;
	res::IResourceManager &resmgr = RESOURCE_MANAGER;

	io::IReadFile *file = fsys.createAndOpenFile(file_name.c_str());

	IGameNode* reloaded_gn = _forcedLoadGameNode(node_params, file);

	SAFE_DROP(file);

	if (reloaded_gn)
	{
		scn::ISceneNode *reloaded_sn = reloaded_gn->getSceneNode();

		if (reloaded_sn)
		{	
			reloaded_sn->setDebugDataVisible(dbg_vis);
			reloaded_sn->setParent(parent);
			reloaded_sn->setParameters(node_params.Scene.Transformation);
			reloaded_sn->setParameters(node_params.Scene.Parameters);			
		}
	}

	return reloaded_gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::loadGameNode(
	SGameNodeCommonParams &node_params, const c8* fname)
{     
	IGameNode* gn = 0;

	core::stringc xml_file_name =
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_XML_SCRIPTS);
    xml_file_name.append(fname); 

	io::IReadFile *file =
		FILE_SYSTEM.createAndOpenFile(xml_file_name.c_str());
	if (file)
		gn = loadGameNode(node_params, file);
	else
		LOGGER.logErr("Can't load %s game node from file %s",
			GameNodeTypeStr[node_params.Game.Type], xml_file_name.c_str() );
	SAFE_DROP(file);

	return gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::loadGameNode(const c8* fname)
{
	IGameNode* gn = 0;

	io::IReadFile *file = FILE_SYSTEM.createAndOpenFile(fname);
	if (file)
	{
		SGameNodeCommonParams node_params;
		node_params.Scene.Parameters.Type = scn::ESNT_UNKNOWN_SCENE_NODE;
		node_params.Game.Type = EGNT_UNKNOWN;	
		node_params.Scene.Parameters.Name = core::extractFileName(fname);

		gn = loadGameNode(node_params, file);
	}
	else
		LOGGER.logErr("Can't load game node from file %s", fname);
	SAFE_DROP(file);

	return gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::loadGameNode(game::E_GAME_NODE_TYPE gntype, const c8* fname)
{
	IGameNode *gnode = 0;

	SGameNodeCommonParams node_params;
	node_params.Scene.Parameters.Type = scn::ESNT_UNKNOWN_SCENE_NODE;
	node_params.Game.Type = gntype;	
	node_params.Scene.Parameters.Name = fname;

	gnode = loadGameNode(node_params, fname);

	return gnode;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	IGameNode* gn = 0;

	if (!file)
		return NULL;

	static core::stringc fname = "";
	fname = file->getFileName();

	bool cloned = false;

	if (fname != NONAME_FILE_NAME)
	{
		core::list<IGameNode*>::iterator iter =
			GameNodesByType[node_params.Game.Type].begin();
		for(; iter!= GameNodesByType[node_params.Game.Type].end(); ++iter)
		{
			IGameNode *gnode = *iter;	
			scn::ISceneNode *scene_node = 0;
			if (gnode && (scene_node = gnode->getSceneNode()) &&
					fname == scene_node->getFileName())
			{	
				gn = cloneGameNode(gnode, true);
				scn::SSceneNodeCommonParams scene_params = scene_node->getParameters();	
				scn::SSceneNodeParameters empty_sn_params;
				scn::SSceneNodeTransformation empty_sn_transf;
				if (node_params.Scene.Parameters == empty_sn_params)				
					node_params.Scene.Parameters = scene_params.Parameters;   
				if (node_params.Scene.Transformation == empty_sn_transf)				
					node_params.Scene.Transformation = scene_params.Transformation;	  
				cloned = true;
				break;
			}
		}
	}	

	if (!gn)
		gn = _forcedLoadGameNode(node_params, file);

	if (gn)
	{
		scn::ISceneNode *sn = gn->getSceneNode();
		if (sn)
		{	
			sn->setParameters(node_params.Scene.Transformation);
			sn->setParameters(node_params.Scene.Parameters);
			dyn::IDynamicObject* dobj = sn->getDynamicObject(); 
			if (cloned && dobj)
				dobj->updateGeomParams();   				
		}
	}

	return gn;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::_forcedLoadGameNode(
	SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	IGameNode *gn = 0;

	io::IXMLReader *xml = FILE_SYSTEM.createXMLReader(file);  		

	if (!xml)                                                                                     
	{                                                                                             
		LOGGER.logWarn("Can't open xml-file %s", file->getFileName());
	}
	else 
	{
		bool game_data_finded = false;

		E_GAME_NODE_TYPE game_node_type = EGNT_UNKNOWN;
		core::stringc class_name_str = "Standard";
		core::stringc game_node_type_str ="EGNT_UNKNOWN";		

		while (!game_data_finded && xml->read())
		{
			if (xml->getNodeType()==io::EXN_ELEMENT)
			{   					
				if (core::stringw("Game") == xml->getName())
				{
					game_data_finded = true;

					game_node_type_str = xml->getAttributeValue(L"game_node_type");
					class_name_str     = xml->getAttributeValue(L"class_name");	
					
					for (s32 i=0; i<E_GAME_NODE_TYPE_COUNT; i++)
						if (game_node_type_str==GameNodeTypeStr[i])
							game_node_type = (E_GAME_NODE_TYPE)i;
				}					
			}
		}

		xml->drop();

		if (game_node_type != EGNT_UNKNOWN && (
				node_params.Game.Type == EGNT_UNKNOWN ||
				node_params.Game.Type == game_node_type))
		{
			node_params.Game.Type		= game_node_type;
			node_params.Game.ClassName	= class_name_str;

			bool class_finded = false;

			for (u32 i=0; i < GameClassesData[game_node_type].size(); i++)
			{
				if (node_params.Game.ClassName == GameClassesData[game_node_type][i].ClassName)
				{
					gn = GameClassesData[game_node_type][i].Load(node_params, file);
					class_finded = true;
					break;
				}
			}

			if (!class_finded)
			{
				LOGGER.logErr("Unknown game node class '%s - %s' !", 
					game_node_type_str.c_str(), class_name_str);
			}
		}
		else
		{
			LOGGER.logErr("Waiting '%s' game node type, got '%s' !", 
				GameNodeTypeStr[node_params.Game.Type], game_node_type_str.c_str());
		}
	}

	if (!gn)
	{
		LOGGER.logErr("Can't load game node, from %s",
			file->getFileName());
	}
	else
	{
		_createDropItemsForGameNode(gn);
	}

	return gn;
}

//----------------------------------------------------------------------------

void CGameManager::clearGameScene(bool clear_resourses)
{
	IGameTasksManager::getSingleton().clearGameTasks();
	IGameEventsDispatcher::getSingleton().clearGameEvents();

	SCENE_MANAGER.clearScene(0xFFFFFFFF, clear_resourses);	

	core::list<IGameNode*>::iterator iter = GameNodes.begin();
	if (iter != GameNodes.end())
		LOGGER.logErr("Remain undeleted game nodes, after clearGameScene!");

	removeAllSoundTracks();

	SOUND_DRIVER.clearUnusedSoundCache();

	setGameSceneProps(SGameSceneProps());

	GameSceneFileName = NONAME_GAMESCENE_FILE_NAME;
}

//----------------------------------------------------------------------------

void CGameManager::loadGameSceneLater(const c8* filename)
{
	SGameTaskLoadScene *gtask = new SGameTaskLoadScene();
	gtask->FileName = filename;

	GAME_TASK_MANAGER.registerGameTask(gtask);
}

//----------------------------------------------------------------------------

bool CGameManager::loadGameScene(const c8* filename)
{
	bool res = false;

	core::stringc fullfilename = 
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_XML_SCRIPTS);
	fullfilename.append(filename);

	bool game_was_started = isGameStarted();	

	stopGame();
	stopAllSoundTracks();

	vid::E_RENDER_PATH rend_path = VIDEO_DRIVER.getRenderPath();
	VIDEO_DRIVER.setRenderPath(vid::ERP_FORWARD_RENDERING);

	if (m_CallbackBeforeLoadScene)
		m_CallbackBeforeLoadScene();
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_BEFORE_LOAD_SCENE, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_BEFORE_LOAD_SCENE, (bool*)NULL);
		
	clearGameScene(false);

	res = GameSceneLoader.loadGameScene(fullfilename);

	setCurrentMainPlayer(0);

	setSoundEffectsVolume(getSoundEffectsVolume());
	setSoundTracksVolume(getSoundTracksVolume());

	if (game_was_started) 
		startGame();

	GameSceneFileName = fullfilename;

	m_NewGameSceneLoaded = true;

	if (m_CallbackAfterLoadScene)
		m_CallbackAfterLoadScene();
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_AFTER_LOAD_SCENE, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_AFTER_LOAD_SCENE, (bool*)NULL);

	VIDEO_DRIVER.setRenderPath(rend_path);

	return res;
}

//----------------------------------------------------------------------------

bool CGameManager::saveGameScene(const c8 *filename)
{
	bool res = false;

	if (m_CallbackBeforeSaveScene)
		m_CallbackBeforeSaveScene();
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_BEFORE_SAVE_SCENE, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_BEFORE_SAVE_SCENE, (bool*)NULL);

	do
	{
		core::stringc fullfilename;

		if (!filename && GameSceneFileName.size() > 0
				&& GameSceneFileName != NONAME_GAMESCENE_FILE_NAME)
			fullfilename = GameSceneFileName.c_str();
		else
		if (!filename)
			break;
		else
			fullfilename = filename;

		const c8 *xml_scripts_dir = RESOURCE_MANAGER.getMediaDirFull(
			res::EMT_XML_SCRIPTS);

		if (!FILE_SYSTEM.existFile(fullfilename.c_str()))
		// try relative path
			fullfilename.sprintf("%s%s", xml_scripts_dir, filename);

		GameSceneSaver.saveGameScene(fullfilename);

		if (!FILE_SYSTEM.existFile(fullfilename.c_str()))
		{
			GameSceneFileName = NONAME_GAMESCENE_FILE_NAME;
			break;
		}

		const c8 *file_path = core::extractFilePath(fullfilename.c_str());
		const c8 *file_name = core::extractFileName(fullfilename.c_str());
		const c8 *rel_file_path = core::getRelativePathBetweenDirectories(
			xml_scripts_dir, file_path);

		GameSceneFileName.sprintf("%s%s", rel_file_path, file_name);

		res = true;
	}
	while (0);

	if (m_CallbackAfterSaveScene)
		m_CallbackAfterSaveScene();
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_AFTER_SAVE_SCENE, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_AFTER_SAVE_SCENE, (bool*)NULL);

	return res;
}

//----------------------------------------------------------------------------

const c8* CGameManager::getGameSceneFileName()
{
	return GameSceneFileName.c_str();
}

//----------------------------------------------------------------------------

IGameNodeMainPlayer* CGameManager::setCurrentMainPlayer(s32 idx)
{
	CurrentMainPlayer = 
		static_cast<IGameNodeMainPlayer*>(getGameNode(EGNT_MAIN_PLAYER, idx));	

	return CurrentMainPlayer;
}

//----------------------------------------------------------------------------

s32 CGameManager::getMainPlayersCount()
{
	return getGameNodesCount(EGNT_MAIN_PLAYER);	
}

//----------------------------------------------------------------------------

IGameNodeMainPlayer* CGameManager::getMainPlayerGameNode()
{
	return CurrentMainPlayer;
}

//----------------------------------------------------------------------------

scn::ISceneNode* CGameManager::getMainPlayerSceneNode()
{
	return CurrentMainPlayer ? CurrentMainPlayer->getSceneNode() : NULL;
}

//----------------------------------------------------------------------------

dyn::IDynamicObject* CGameManager::getMainPlayerDynamicObject()
{
	return CurrentMainPlayer ? CurrentMainPlayer->getDynamicObject() : NULL;
}

//----------------------------------------------------------------------------

scn::ICameraSceneNode* CGameManager::getMainPlayerCamera()
{
	if (CurrentMainPlayer)
	{
		scn::ISceneNode* scene_node = CurrentMainPlayer->getSceneNode();
		return scene_node ? scene_node->getCamera() : NULL;
	}
	return NULL;
}

//----------------------------------------------------------------------------

IGameNode* CGameManager::getGameNodeFromScreenCoordinates(
	const core::position2di &scr_pos)
{
	dyn::IDynamicManager	&dynmgr	= DYNAMIC_MANAGER;
	dev::IDevice			&device	= DEVICE;	
	scn::ISceneManager		&smgr	= SCENE_MANAGER;

	core::line3df ray = smgr.getRayFromScreenCoordinates(scr_pos);

    scn::ISceneNode *node =0;

	core::vector3df dir = ray.getVector();
	dir.normalize();
	dir*=1000000;

	ray.end = ray.start + dir;

	const core::list<scn::ISceneNode*> &vis_list = smgr.getVisibleSceneNodesList();

	scn::ISceneNode* best = 0;
    f32 bestdistance = 9999999999.0f;
    core::vector3df linemiddle = ray.getMiddle();
    core::vector3df linevect   = ray.getVector().normalize();
	core::vector3df pos = ray.start; 		
	f32 halflength = (f32)(ray.getLength() * 0.5);

    core::list<scn::ISceneNode*>::iterator it = vis_list.begin();

    for (; it != vis_list.end(); ++it)
    {
        scn::ISceneNode *current = *it;

		if (current->getGameNode())
		{
			core::vector3df abs_pos = current->getAbsolutePosition();
			core::aabbox3df tbb = current->getTransformedBoundingBox();

			if (current->getSceneNodeType() == scn::ESNT_LIGHT_SCENE_NODE)
			{
				tbb.reset(current->getAbsolutePosition());
				tbb.MinEdge-=core::vector3df(10,10,10);
				tbb.MaxEdge+=core::vector3df(10,10,10);
			}

            if (tbb.intersectsWithLine(linemiddle, linevect, halflength))
            {
                f32 dist =  (f32)abs_pos.getDistanceFrom(pos);
                if (dist < bestdistance)
                {
                    best = current;
                    bestdistance = dist;
                }
            }
        }      
    }

	if (!best)
	{
		dyn::SCollisionPoints collisions;

		if (dynmgr.findCollisionsWithRay(ray,
				dyn::EDCF_MONOLITH_COLLID | dyn::EDCF_DYNAMIC_COLLID,
				collisions))
		// Dynamic Engine collision stuff
		{		 		
			const core::array<dyn::SCollisionPoint> &points = collisions.getPoints();
			// find nearest dynamic object
			s32 idx = collisions.getNearestTo(ray.start);
			while (idx >= 0 && idx < (s32)points.size())
			{
				dyn::IDynamicObject *dynobj = points[idx].DynObj;
				if (dynobj && dynobj->getSceneNode() &&
						dynobj->getSceneNode()->getGameNode())
				{
					best = dynobj->getSceneNode();
					break;
				}
				idx++;
			}
		}
	}

	return best ? best->getGameNode() : NULL;
}

//----------------------------------------------------------------------------

void CGameManager::stopGame()
{
	if (m_GameStopped)
		return;

	m_GameStopped = true;

	// zero counters
	DynamicPSCounter.registerEvent(0xffffffff);
	GamePSCounter.registerEvent(0xffffffff);
	ScriptPSCounter.registerEvent(0xffffffff);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_GAME_STOPPED, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_GAME_STOPPED, (bool*)NULL);

	LOGGER.logInfo("Game stopped");
}

//----------------------------------------------------------------------------

void CGameManager::startGame()
{
	if (!m_GameStopped)
		return;

	m_GameStopped = false;

	playSoundTrack(0);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_GAME_STARTED, (bool*)NULL);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_GAME_STARTED, (bool*)NULL);

	LOGGER.logInfo("Game started");
}

//----------------------------------------------------------------------------

void CGameManager::_doGame ()
{
	f64 start_game_time_sec = TIMER.getSystemTimeSec();

    if (m_GameStopped)
    {
        previousFrameTime = 0;
        thisDeltaTime = 0;

        return;
    }

	m_Profiler.startProfiling(m_ProfileIds[EGPT_PRE_GAME]);

    // timer operations

    f64 CurrentTimeSec = TIMER.getTimeSec();

    u32 CurrentTimeMs = CurrentTimeSec*1000.0;  

    if (previousFrameTime == 0)
    {
        previousFrameTime = thisFrameTime = CurrentTimeSec;     
    }
    else
    {
        previousFrameTime = thisFrameTime;

        thisFrameTime = CurrentTimeSec;

        thisDeltaTime = thisFrameTime - previousFrameTime;      

        previousDeltaTime = thisDeltaTime * 0.05 + previousDeltaTime * 0.95;
    } 

    scn::ISceneManager          &smgr     = SCENE_MANAGER;
    scn::IPathFinderManager     &pathmgr  = PATH_FINDER_MANAGER;
    game::IGameTasksManager     &gtmgr    = GAME_TASK_MANAGER;
    game::IGameEventsDispatcher &gedisp   = GAME_EVENTS_DISPATCHER;
    game::IGameAIModule         &aimod    = GAME_AI_MODULE;
    dyn::IDynamicManager        &dynmgr   = DYNAMIC_MANAGER;   
    scr::IScriptManager         &scrmgr   = SCRIPT_MANAGER;
    mm::ISoundDriver            &snddrv   = SOUND_DRIVER;

    f32 DeltaTimeSec = thisDeltaTime + RemainDeltaSeconds;  

    bool interpolate = true;

	dynmgr.restoreSecondObjectsTransformationsForInterpolation(); 

    core::list<IGameNode*>::iterator iter;

    //! a list of all active gamenodes contained in the game
    core::list<IGameNode*> active_game_nodes;

	static f32 ElapsedTimeSec = 0;
	const  f32 DYNAMIC_STEP_SEC = 0.025f;
	static f32 OveralDynamicStep = 0.001f;

	//-----------------------
	// Pre Game Stuff  
	//-----------------------

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_PRE_GAME]);

do_game:

	m_Profiler.startProfiling(m_ProfileIds[EGPT_PRE_GAME]);

	if (m_DoGameCount++ > 3)
	{       
		gedisp.dispatchGameEvents();
		aimod.doAI();   
		gtmgr.validateGameTasks();  
	}
	
	active_game_nodes.clear();
	m_ActiveGameNodesCount = 0;

	static const E_GAME_NODE_TYPE gntypearr[] = 
	{ 
		EGNT_MAIN_PLAYER,
		EGNT_PERSON,
		EGNT_DECORATION,
		EGNT_LEVEL_MAP,
		EGNT_SKY_DOME,
		EGNT_LIGHT, 
		EGNT_TRIGGER,
		EGNT_DOOR,
		EGNT_PLATFORM,
		EGNT_BUTTON,
		EGNT_ITEM,
		EGNT_WEAPON,
		EGNT_UNITS_FORMATION,
		EGNT_UNKNOWN
	};
	static const s32 gntypecnt = sizeof(gntypearr)/sizeof(E_GAME_NODE_TYPE);

	for (u32 i = 0; i < gntypecnt; i++)
	{
		E_GAME_NODE_TYPE gntype = gntypearr[i];

		iter = GameNodesByType[gntype].begin();
	    
		if (gntype==EGNT_PERSON)
		{
			for(; iter!=GameNodesByType[gntype].end(); ++iter) 
			{
				IGameNode* gn = (*iter);

				if (gn->isGaming())
				{
					active_game_nodes.push_back(gn);
					pathmgr.mapUnitPosition(gn->getDislocation());                                                                              
					gn->setEnabled(true);
				}
				else
				{
					gn->setEnabled(false);
				}
			}
		}
		else
		{
			for(; iter!=GameNodesByType[gntype].end(); ++iter) 
			{
				IGameNode* gn = (*iter);

				if (gn->isGaming())
				{
					active_game_nodes.push_back(gn);                        
					gn->setEnabled(true);
				}
				else
				{  					
					gn->setEnabled(false);
				}
			}
		}   
	}

	m_ActiveGameNodesCount = active_game_nodes.size(); 

	s32 now_time_ms = TIMER.getTime();  
	
	for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
	{
		(*iter)->OnPreGame(now_time_ms);  
	}

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_PRE_GAME]);

	m_Profiler.startProfiling(m_ProfileIds[EGPT_DO_GAME]);
	m_Profiler.stopProfiling(m_ProfileIds[EGPT_DO_GAME]);

	s32 cnt=0;

	while (DeltaTimeSec >0)
    {
        DeltaTimeSec -= DYNAMIC_STEP_SEC;

		m_Profiler.startProfiling(m_ProfileIds[EGPT_DYNAMIC]);

		//-----------------------
		// Pre Dynamic Stuff  
		//-----------------------	

		for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
		{
			(*iter)->OnPreDynamic(DYNAMIC_STEP_SEC);           
		}

		//-----------------------
		// do game dynamic 
		//-----------------------

		if (!cnt)
		{
			dynmgr.storeFirstObjectsTransformationsForInterpolation();
		}
       
        DynamicPSCounter.registerEvent(CurrentTimeMs);  

        dynmgr.preDynamic();
        dynmgr.calcDynamic(DYNAMIC_STEP_SEC);  
        dynmgr.postDynamic();   
		dynmgr.updateGeometryFromDynamic();  
		dynmgr.storeSecondObjectsTransformationsForInterpolation();

		m_Profiler.stopProfiling(m_ProfileIds[EGPT_DYNAMIC]);

		//-----------------------
		// do game content  
		//-----------------------

		m_Profiler.startProfiling(m_ProfileIds[EGPT_DO_GAME]);

		// game content  counter
		GamePSCounter.registerEvent(CurrentTimeMs);     

		gtmgr.performGameTasks(false);   

		m_Profiler.stopProfiling(m_ProfileIds[EGPT_DO_GAME]);

		//-----------------------
		// Post Dynamic Stuff  
		//-----------------------

		m_Profiler.startProfiling(m_ProfileIds[EGPT_DYNAMIC]);

		if (m_NewGameSceneLoaded)
		{
			active_game_nodes.clear();
		}

		for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
		{
			(*iter)->OnPostDynamic(DYNAMIC_STEP_SEC);  
		}
	    
		gtmgr.removeCompletedTasks();

		ElapsedTimeSec = RemainDeltaSeconds;

		if (!cnt)
		{
			OveralDynamicStep	= 0;
		}

		OveralDynamicStep += DYNAMIC_STEP_SEC;

		m_DoGameCompleted = true;

		cnt++;

		m_Profiler.stopProfiling(m_ProfileIds[EGPT_DYNAMIC]);
    }

	m_Profiler.startProfiling(m_ProfileIds[EGPT_DYNAMIC]);

	RemainDeltaSeconds = DeltaTimeSec;

	// dynamic interpolation

	ElapsedTimeSec += thisDeltaTime;

	f32 interpolation_factor = ElapsedTimeSec/OveralDynamicStep;   
	dynmgr.interpolateObjectsTransformations(interpolation_factor);

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_DYNAMIC]);

	//-----------------------
	// Post Game Stuff  
	//-----------------------

	m_Profiler.startProfiling(m_ProfileIds[EGPT_POST_GAME]);
	
	now_time_ms = TIMER.getTime();

	for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
	{
		(*iter)->OnPostGame(now_time_ms);  
	}

	pathmgr.clearUnitMappings();  

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_POST_GAME]);

    //-----------------------
    // scripting
	//-----------------------

	m_Profiler.startProfiling(m_ProfileIds[EGPT_SCRIPT]);

    static s32 counter = 0; 
    
    if (counter == 10) counter =0;
    
    if (counter++ == 0 && GameScriptFileName != NONAME_FILE_NAME)
    {
        // scripting counter
        ScriptPSCounter.registerEvent(CurrentTimeMs);

        // Main Game Script
        scrmgr.runScript(GameScriptFileName.c_str());
    }   

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_SCRIPT]);

	//-----------------------
    // updating sound
	//-----------------------

	m_Profiler.startProfiling(m_ProfileIds[EGPT_SOUND]);

    static f64 last_sound_update = 0.0;
	f64 now = TIMER.getTimeSec();

    if (last_sound_update == 0.0 || core::math::Abs(now-last_sound_update)>0.25)
    {
        for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
        {
            (*iter)->OnPreSound();
        }

		// positioning sound listener pos
		scn::ICameraSceneNode* cam = smgr.getActiveCamera();
		if (cam)
		{
			snddrv.moveTo(cam->getPosition());
			snddrv.setOrientation(cam->getTarget(), cam->getUpVector());
		}

        snddrv.updateSounds();

        for(iter=active_game_nodes.begin(); iter!=active_game_nodes.end(); ++iter) 
        {
            (*iter)->OnPostSound(); 
        }

        last_sound_update = now;
    }   

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_SOUND]);

    if (m_NewGameSceneLoaded)
    {
        DeltaTimeSec = DYNAMIC_STEP_SEC;   
        
        m_NewGameSceneLoaded = false;   

        goto do_game;
    } 
}

//----------------------------------------------------------------------------

void CGameManager::doGame()
{	
	_doGame();
}

//---------------------------------------------------------------------------

void CGameManager::preRenderFrame()
{
	m_Profiler.startProfiling(m_ProfileIds[EGPT_WHOLE_GAME]);

	DYNAMIC_MANAGER.preRenderFrame();

	m_DoGameCompleted = false;
}

//---------------------------------------------------------------------------

void CGameManager::postRenderFrame()
{
	f64 now = TIMER.getSystemTimeSec();
    f64 prev_now = now;  

	if (m_DoGameCompleted)
	{
		game::IGameTasksManager &gtmgr = GAME_TASK_MANAGER;
		gtmgr.performGameTasks(true);  
		gtmgr.removeCompletedTasks();
	}

	DYNAMIC_MANAGER.postRenderFrame();

	m_Profiler.stopProfiling(m_ProfileIds[EGPT_WHOLE_GAME]);
}

//---------------------------------------------------------------------------

void CGameManager::showMessageBox(
	bool show, const c8 *caption, const c8 *message)
{
	if (m_CallbackOnShowMessage)
		m_CallbackOnShowMessage(show, caption, message);

	static core::stringc capt_msg;
	capt_msg = "";
	capt_msg.append(caption);
	capt_msg.append(";");
	capt_msg.append(message);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SHOW_MESSAGE, (bool*)NULL,
		show, capt_msg.c_str());
}

//----------------------------------------------------------------------------

IGameInventory* CGameManager::createGameInventory(game::IGameNode *owner)
{
	return new CGameInventory(owner);
}

//----------------------------------------------------------------------------

SGameNodeParams CGameManager::getGameNodeParams(IGameNode *gnode)
{
	SGameNodeParams params;

	return params;	
}

//----------------------------------------------------------------------------

void CGameManager::applyGameNodeParams(IGameNode *gnode, SGameNodeParams &params)
{
}

//----------------------------------------------------------------------------

s32 CGameManager::getDynamicPS()
{
	return  DynamicPSCounter.getEventPerSecond();
}

//----------------------------------------------------------------------------

s32 CGameManager::getGamePS()
{
	return  GamePSCounter.getEventPerSecond();
}

//---------------------------------------------------------------------------

s32 CGameManager::getScriptPS()
{
	return  ScriptPSCounter.getEventPerSecond();
}

//---------------------------------------------------------------------------

bool CGameManager::isGameStarted()
{
	return !m_GameStopped;
}

//---------------------------------------------------------------------------

void CGameManager::setGameScriptFileName(core::stringc &scr_fname)
{
	GameScriptFileName = scr_fname;
}

//---------------------------------------------------------------------------

core::stringc CGameManager::getGameScriptFileName()
{
	return GameScriptFileName;
}

//---------------------------------------------------------------------------

void CGameManager::addSoundTrack(core::stringc &snd_fname)
{
	mm::ISound* sound = 
		SOUND_DRIVER.getSound(snd_fname.c_str(), true);

	if (sound)
	{
		sound->grab();
		sound->setVolume(SoundTracksVolume);

		GameSoundTracks.push_back(sound);
	}
}

//---------------------------------------------------------------------------

void CGameManager::removeAllSoundTracks()
{
	core::list<mm::ISound*>::iterator s = GameSoundTracks.begin();

	for (; s!=GameSoundTracks.end(); ++s)
	{
		mm::ISound *sound = *s;

		if (sound)
			sound->drop();
	}

	GameSoundTracks.clear();
}
	
//---------------------------------------------------------------------------
	
mm::ISound* CGameManager::getSoundTrack(s32 idx)
{
	if (idx>=0 && idx<(s32)GameSoundTracks.size())
	{
		return *(GameSoundTracks.begin() + idx);
	}

	return NULL;	
}

//---------------------------------------------------------------------------

s32 CGameManager::getSoundTracksCount()
{
	return GameSoundTracks.size();
}

//----------------------------------------------------------------------------

bool CGameManager::playSoundTrack(s32 idx)
{
	mm::ISound* snd = getSoundTrack(idx);
	
	if (snd)
	{
		snd->play();
		snd->moveTo(core::vector3df(0,0,0));

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool CGameManager::pauseSoundTrack(s32 idx)
{
	mm::ISound* snd = getSoundTrack(idx);
	
	if (snd)
	{
		snd->pause();

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------

void CGameManager::stopAllSoundTracks()
{
	core::list<mm::ISound*>::iterator it = GameSoundTracks.begin();

	for (; it!=GameSoundTracks.end(); ++it)
	{
		if (*it)
			(*it)->stop();
	}
}

//---------------------------------------------------------------------------

void CGameManager::setSoundTracksVolume(f32 volume)
{
	CHECK_RANGE(volume, 0.0f, 1.0f);

	SoundTracksVolume = volume;

	core::list<mm::ISound*>::iterator it = GameSoundTracks.begin();

	for (; it!=GameSoundTracks.end(); ++it)
	{
		if (*it)
			(*it)->setVolume(volume);
	}
}

//---------------------------------------------------------------------------

f32 CGameManager::getSoundTracksVolume()
{
	return SoundTracksVolume;
}

//---------------------------------------------------------------------------

void CGameManager::setSoundEffectsVolume(f32 volume)
{
	CHECK_RANGE(volume, 0.0f, 1.0f);

	SoundEffectsVolume = volume;

	core::list<IGameNode*>::iterator node_it = GameNodes.begin();

	for (; node_it!=GameNodes.end(); ++node_it)
	{
		IGameNode *gnode = *node_it;

		if (!gnode)
			continue;

		gnode->setSoundEffectsVolume(volume);
	}
}

//---------------------------------------------------------------------------

f32 CGameManager::getSoundEffectsVolume()
{
	return SoundEffectsVolume;
}

//---------------------------------------------------------------------------
//!      game events
//---------------------------------------------------------------------------

void CGameManager::onDeleteNode(IGameNode* gnode)
{
	if (!gnode)
		return;

	if (m_CallbackOnDeleteNode)
		m_CallbackOnDeleteNode(gnode);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_DELETE_GAME_NODE, (bool*)NULL,
		(void*)gnode);

	GameNodes.erase(gnode->m_GameNodesIterator);	
	GameNodesByType[gnode->getGameNodeType()].erase(gnode->m_GameNodesByTypeIterator);	

	if (gnode->m_GameNodesAIIterator!=0)
	{
		GameNodesAI.erase(gnode->m_GameNodesAIIterator);
	}

	gnode->m_GameNodesIterator=0;
	gnode->m_GameNodesByTypeIterator=0;
	gnode->m_GameNodesAIIterator=0;

	if (gnode == getMainPlayerGameNode())
		setCurrentMainPlayer(0);

	IGameEventsDispatcher::getSingleton().onDeleteNode(gnode);
	IGameTasksManager::getSingleton().onDeleteNode(gnode);
}

//----------------------------------------------------------------------------

void CGameManager::onCreateNode(IGameNode* gnode)
{
	if (!gnode)
		return;

	if (m_CallbackOnCreateNode)
		m_CallbackOnCreateNode(gnode);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_CREATE_GAME_NODE, (bool*)NULL,
		(void*)gnode);

	GameNodes.push_back(gnode);
	GameNodesByType[gnode->getGameNodeType()].push_back(gnode);

	gnode->m_GameNodesIterator       = GameNodes.get_last();
	gnode->m_GameNodesByTypeIterator = GameNodesByType[gnode->getGameNodeType()].get_last();

	IGameNodeAI *gnode_ai = SAFE_CAST_TO_GAME_NODE_AI(gnode);

	if (gnode_ai)
	{
		GameNodesAI.push_back(gnode_ai);

		gnode->m_GameNodesAIIterator = GameNodesAI.get_last();
	}

	if (gnode->getGameNodeType() == game::EGNT_MAIN_PLAYER
			&& !getMainPlayerGameNode())
		setCurrentMainPlayer(0);
}

//----------------------------------------------------------------------------

void CGameManager::onDieNode(IGameNode* gnode)
{
	if (!gnode)
		return;

	if (m_CallbackOnDieNode)
		m_CallbackOnDieNode(gnode);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_DIE_GAME_NODE, (bool*)NULL,
		(void*)gnode);

	// creating drop

	IGameInventory& inventory = gnode->getInventory();     
	IGameNodeItem* item = NULL;
	while (item = inventory.getItem(0))
	{
		inventory.removeItem(item);
		item->getSceneNode()->setParent(SCENE_MANAGER.getRootSceneNode());
		item->getSceneNode()->setVisible(true);
	}
}

//----------------------------------------------------------------------------

void CGameManager::onCollectItem(IGameNode* gnode, IGameNodeItem* item)
{
	if (m_CallbackOnCollectItem)
		m_CallbackOnCollectItem(gnode, item);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_COLLECT_GAME_ITEM, (bool*)NULL,
		(void*)gnode, (void*)item);
}

//----------------------------------------------------------------------------

void CGameManager::onThrowItem(IGameNode* gnode, IGameNodeItem* item)
{
	if (m_CallbackOnThrowItem)
		m_CallbackOnThrowItem(gnode, item);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_THROW_GAME_ITEM, (bool*)NULL,
		(void*)gnode, (void*)item);
}

//----------------------------------------------------------------------------

//! calling on use item
void CGameManager::onUseItem(IGameNode* gnode, IGameNodeItem* item)
{
	if (m_CallbackOnUseItem)
		m_CallbackOnUseItem(gnode, item);

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_USE_GAME_ITEM, (bool*)NULL,
		(void*)gnode, (void*)item);
}

//----------------------------------------------------------------------------

__MY_EXPORT__ game::IGameManager* createGameManager()
{
	return new game::CGameManager();
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
