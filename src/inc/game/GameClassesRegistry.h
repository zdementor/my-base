//|-------------------------------------------------------------------------
//| File:        GameClassesRegistry.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef GameClassesRegistryHPP
#define GameClassesRegistryHPP
//----------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <game/IGameNode.h>
#include <io/IXMLReader.h>
#include <io/ILogger.h>
#include <res/IResourceManager.h>
#include <io/IFileSystem.h>
#include <scn/ISceneManager.h>
#include <mm/ISoundDriver.h>
#include <game/IGameManager.h>
#include <scn/ILightGridManager.h>
#include <game/IGameNodeAI.h>
#include <game/IGameNodeMainPlayer.h>
#include <game/IGameNodeWeapon.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Main Player registry funcs
IGameNode* Alloc__StandardMainPlayer(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardMainPlayer(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardMainPlayer(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardMainPlayer(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Decoration registry funcs
IGameNode* Alloc__StandardDecoration(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardDecoration(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardDecoration(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardDecoration(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Level Map registry funcs
IGameNode* Alloc__StandardLevelMap(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardLevelMap(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardLevelMap(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardLevelMap(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Terrain registry funcs
IGameNode* Alloc__StandardTerrain(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardTerrain(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardTerrain(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardTerrain(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Button registry funcs
IGameNode* Alloc__StandardButton(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardButton(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardButton(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardButton(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Weapon registry funcs
IGameNode* Alloc__StandardWeapon(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardWeapon(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardWeapon(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardWeapon(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Door registry funcs
IGameNode* Alloc__StandardDoor(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardDoor(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardDoor(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardDoor(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Platform registry funcs
IGameNode* Alloc__StandardPlatform(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardPlatform(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardPlatform(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardPlatform(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Person registry funcs
IGameNode* Alloc__StandardPerson(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardPerson(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardPerson(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardPerson(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Trigger registry funcs
IGameNode* Alloc__StandardTrigger(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardTrigger(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardTrigger(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardTrigger(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Light registry funcs
IGameNode* Alloc__StandardLight(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardLight(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardLight(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardLight(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Item registry funcs
IGameNode* Alloc__StandardItem(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardItem(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardItem(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardItem(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! Vehicle registry funcs
IGameNode* Alloc__StandardVehicle(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardVehicle(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardVehicle(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardVehicle(SGameNodeCommonParams &node_params, io::IReadFile* file);

//! SkyDome registry funcs
IGameNode* Alloc__StandardSkyDome(scn::ISceneNode* n, SGameNodeParams &params);
IGameNode* Create_StandardSkyDome(SGameNodeCommonParams &node_params, bool log_info);
IGameNode* Clone__StandardSkyDome(IGameNode *game_node, bool log_info);
IGameNode* Load___StandardSkyDome(SGameNodeCommonParams &node_params, io::IReadFile* file);

//----------------------------------------------------------------------------

class CGameNodeBaseCreator
{
public:

	static IGameNode* createGameNode(SGameNodeCommonParams node_params, bool log_info)
	{
		E_GAME_NODE_TYPE gntype = node_params.Game.Type;

		if (log_info)
			LOGGER.logInfo( "Creating %s Node . . .", GameNodeTypeShortStr[gntype]);

		LOGGER.increaseFormatLevel();

		scn::ISceneNode *scene_node = 
			SCENE_MANAGER.createSceneNode(node_params.Scene, log_info);	
		
		IGameNode *game_node = NULL;

		if (scene_node)                                                                        
			game_node = GAME_MANAGER.addGameNode(node_params, scene_node);       

		LOGGER.decreaseFormatLevel();

		if (!game_node)                                                                                    
			LOGGER.logErr("Can't create %s Node !", GameNodeTypeShortStr[gntype]);
		else if (log_info)                                                                                         
			LOGGER.logInfo("%s Node successfully created", GameNodeTypeShortStr[gntype]);

		return game_node;
	}
};

//----------------------------------------------------------------------------

class CGameNodeBaseCloner
{
public:

	virtual IGameNode* cloneGameNode(IGameNode* game_node_to_clone, bool log_info)
    {
        if (!game_node_to_clone) 
			return NULL;

        scn::ISceneManager& smgr = SCENE_MANAGER;

        scn::ISceneNode* scene_node_to_clone = game_node_to_clone->getSceneNode();

        game_node_to_clone->updateGameCommonParams();   

        SGameNodeCommonParams node_params = game_node_to_clone->getGameCommonParams();

        E_GAME_NODE_TYPE gntype = node_params.Game.Type;

        static core::stringc Msg;

        if (log_info)
        {
            Msg.sprintf( "Cloning %s Node . . .", GameNodeTypeShortStr[gntype] );         
            LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);    
        }

        LOGGER.increaseFormatLevel();

        scn::ISceneNode* scene_node_cloned =
            SCENE_MANAGER.cloneSceneNode( scene_node_to_clone, true, log_info );   
        
        if (scene_node_cloned)
        {
            scn::SSceneNodeCommonParams scene_params = scene_node_cloned->getParameters(); 
            scn::SSceneNodeParameters empty_sn_params;
            scn::SSceneNodeTransformation empty_sn_transf;
            
            if (node_params.Scene.Parameters == empty_sn_params)                
                node_params.Scene.Parameters = scene_params.Parameters;

            if (node_params.Scene.Transformation == empty_sn_transf)                
                node_params.Scene.Transformation = scene_params.Transformation;
            
            node_params.Scene.Material = scene_params.Material;
        }
		else
			return NULL;

        IGameNode* game_node_cloned = 
            GAME_MANAGER.addGameNode(node_params, scene_node_cloned);        
        
        IGameNodeAI* gnai_cloned   = SAFE_CAST_TO_GAME_NODE_AI(game_node_cloned);
        IGameNodeAI* gnai_to_clone = SAFE_CAST_TO_GAME_NODE_AI(game_node_to_clone);

        if (gnai_cloned && gnai_to_clone)
        {
            SGameNodeAIParams ai_pars = gnai_to_clone->getParametersAI();

            gnai_cloned->setParametersAI(ai_pars);

            // binding hurt effects

            const core::array<SHurtEffectParameters>& hurt_effects = 
                gnai_to_clone->getHurtEffects();

            for (u32 i=0; i<hurt_effects.size(); i++)
            {
                const SHurtEffectParameters& hurt_to_clone = hurt_effects[i];
                scn::ISceneNode* sn_to_clone = ((SHurtEffectParameters&)hurt_to_clone).getSceneNode();
                if (sn_to_clone)
                {
                    scn::ISceneNode* sn_cloned = smgr.cloneSceneNode(sn_to_clone, true, log_info);
                    if (sn_cloned)
                    {
                        SHurtEffectParameters new_effect(sn_cloned);
                        new_effect.set(hurt_to_clone);

                        gnai_cloned->bindHurtEffect(new_effect);
                    }
                }
            }

            // binding weapons

            s32 w_cnt = gnai_to_clone->getInventory().getWeaponsCount();

            for (s32 w=0; w<w_cnt; w++)
            {
                s32 w_subcnt =  gnai_to_clone->getInventory().getWeaponsSubCount(w);

                for (s32 ws=0; ws<w_subcnt; ws++)
                {
                    IGameNodeWeapon* weapon_to_clone = gnai_to_clone->getInventory().getWeapon(w, ws);

                    IGameNode* gn_cloned = 
                        IGameManager::getSingleton().cloneGameNode(weapon_to_clone, log_info);

                    IGameNodeWeapon* weapon_cloned = 
                        SAFE_CAST_TO_GAME_NODE_WEAPON(gn_cloned);

                    if (weapon_cloned)
                    {
                        gnai_cloned->bindWeapon(w, weapon_cloned, weapon_to_clone->getBindedBoneName());

                        weapon_cloned->setAutoCreatedFlag(true);

                        scn::ISceneNode* sn_cloned   = weapon_cloned->getSceneNode();
                        scn::ISceneNode* sn_to_clone = weapon_to_clone->getSceneNode();

                        sn_cloned->setPosition(sn_to_clone->getPosition());
                        sn_cloned->setRotation(sn_to_clone->getRotation());
                    }
                }
            }

            gnai_cloned->selectWeapon(0);
        }

        if (game_node_cloned)
        {
            s32 snd_cnt = game_node_to_clone->getSoundEffectsCount();
            for (s32 s=0; s<snd_cnt; s++)
            {
                mm::ISound* snd_to_clone = game_node_to_clone->getSoundEffect(s);

                mm::ISound* snd_cloned = SOUND_DRIVER.cloneSound(snd_to_clone);
                if (snd_cloned)
                {
                    snd_cloned->setVolume(GAME_MANAGER.getSoundEffectsVolume());
                    game_node_cloned->addSoundEffect(snd_cloned);   
                }
            }
        }
             
        LOGGER.decreaseFormatLevel();

        if (!game_node_cloned)                                                                                    
        {            
            core::stringc fname = 
                game_node_to_clone->getSceneNode() ?
                game_node_to_clone->getSceneNode()->getFileName() : "";

            Msg.sprintf(
                "Error cloning %s Node %s",
                GameNodeTypeShortStr[gntype], 
                fname.c_str()
                );               
            LOGGER.log(Msg.c_str(), io::ELL_ERROR);                                                   
        }                                                                                             
        else 
        if (log_info)                                                                                         
        {       
            core::stringc fname = 
                game_node_to_clone->getSceneNode() ?
                game_node_to_clone->getSceneNode()->getFileName() : "";

            core::vector3df pos  = scene_node_cloned->getAbsolutePosition();
            core::vector3df npos = scene_node_to_clone->getAbsolutePosition();

            Msg.sprintf(
                "%s Node %s successfully cloned.", 
                GameNodeTypeShortStr[gntype], 
                fname.c_str()
                );         
            LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);                                             
        } 

        return game_node_cloned;
    }	
};

//----------------------------------------------------------------------------

class CGameNodeBaseLoader
{
protected:

	core::stringc XMLFileName;
	core::stringc Msg;
	IGameNode* GameNode;
	io::IXMLReader* XMLFile;
	s32 SceneDataXMLPos, GameDataXMLPos, SoundDataXMLPos, DynamicDataXMLPos;

	struct SSoundEffectEntry
	{
		SSoundEffectEntry(): FileName(""), Looping(false) {}

		SSoundEffectEntry(core::stringc _fname, bool _looping)
			: FileName(_fname), Looping(_looping) {}

		core::stringc FileName;
		bool Looping;
	};

	bool _parseItemDropTable(SItemDropTableLine& drop_table_line, io::IXMLReader* XMLFile)
	{              
		if (core::stringw("ItemDropTable") == XMLFile->getName())	
		{
			s32 xml_pos = XMLFile->getPos();

			drop_table_line.DropProb = XMLFile->getAttributeValueAsFloat(L"drop_prob"); 

			while(XMLFile->read())
			{	
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{  
					if (core::stringw("ItemDrop") == XMLFile->getName())
					{	
						core::stringc item_file_name = XMLFile->getAttributeValue(L"filename");
						f32 prob = XMLFile->getAttributeValueAsFloat(L"prob",0.0f);
						core::vector3df pos  = XMLFile->getAttributeValueAsVector(L"pos");
						if (prob>0.0f&&item_file_name!="")
						{
							drop_table_line.Items.push_back(
								SItemDropTableCell(item_file_name.c_str(), prob, pos)
								);
						}
					}
					else 	
					{
						break;
					}
				}

				xml_pos = XMLFile->getPos();
			}

			XMLFile->setPos(xml_pos);

			s32 drop_cnt = drop_table_line.Items.size();

			if (drop_cnt>0)
			{
				for (s32 i=1; i<drop_cnt; i++)
				{
					drop_table_line.Items[i].Prob = 
						drop_table_line.Items[i-1].Prob + drop_table_line.Items[i].Prob;				
					CHECK_MIN_RANGE(drop_table_line.Items[i].Prob, 0.001f);
				}

				f32 nk = 1.0f / drop_table_line.Items[drop_cnt-1].Prob;  
				for (s32 i=0; i<drop_cnt; i++)
				{
					drop_table_line.Items[i].Prob *= nk;				
				}
			}

			return true;
		}

		return false;
	}

	bool _parseGameCoreParameters(SGameNodeParams &node_game_params, io::IXMLReader* XMLFile)
	{              
		if (core::stringw(game::GameNodeCorePropertiesTagValue) == XMLFile->getName())
		{ 
			s32 xml_pos = XMLFile->getPos();

			while(XMLFile->read())
			{
				if (XMLFile->getNodeType() == io::EXN_ELEMENT)
				{
					if (core::stringw("GameParameters") == XMLFile->getName())
					{    
						node_game_params.Life     = XMLFile->getAttributeValueAsFloat(L"life",     0.0f);
						node_game_params.MovSpeed = XMLFile->getAttributeValueAsFloat(L"mov_speed",0.0f);					
						node_game_params.RotSpeed = XMLFile->getAttributeValueAsFloat(L"rot_speed",0.0f);		
					}
					else
					{
						break;
					}
				}

				xml_pos = XMLFile->getPos();
			}

			XMLFile->setPos(xml_pos);

			return true;
		}	

		return false;
	}

public:

	CGameNodeBaseLoader()
		: XMLFile(0), GameNode(0) {}

	bool notGameSector( io::IXMLReader* XMLFile)
	{
		if (core::stringw("Scene") == XMLFile->getName()||
			core::stringw("Dynamic") == XMLFile->getName() ||
			core::stringw("Sound") == XMLFile->getName()
			)
		{
			return true;
		}
		return false;
	}

	IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{	
		_preLoadGameNode (node_params, file);
		_loadGameNode    (node_params);		
		_postLoadGameNode(node_params);	
		
		return GameNode;
	}

	void _preLoadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{	
		XMLFileName = "";
		Msg = "";
		GameNode =0;
		XMLFile = 0;
		DynamicDataXMLPos = SceneDataXMLPos=GameDataXMLPos=SoundDataXMLPos=0;

		E_GAME_NODE_TYPE gntype = node_params.Game.Type;

		XMLFileName = file->getFileName(); 		

		XMLFile = FILE_SYSTEM.createXMLReader(file);  
		
		Msg.sprintf( "Loading %s Node . . .", GameNodeTypeShortStr[gntype] );         
		LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);    

		LOGGER.increaseFormatLevel();

		if (!XMLFile)                                                                                     
		{                                                                                             
			Msg.sprintf("Can't open xml-file %s", XMLFileName.c_str());                               
			LOGGER.log(Msg.c_str(), io::ELL_WARNING);                    
		}
		else 
		{
			XMLFile->rewindToBegin();

			bool scene_data_finded = false, game_data_finded = false; 
			bool sound_data_finded = false, dynamic_data_finded = false;

			SceneDataXMLPos = GameDataXMLPos = SoundDataXMLPos = DynamicDataXMLPos = XMLFile->getPos();	
			
			core::stringc game_node_type = "";
			core::stringc class_name = "";
			f32 life=100, move_speed=100, rot_speed=100;

			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Scene") == XMLFile->getName())
					{
						scene_data_finded = true;
					}
					else 
					if (core::stringw("Game") == XMLFile->getName())
					{
						game_data_finded = true;

						game_node_type = XMLFile->getAttributeValue(L"game_node_type");
						class_name     = XMLFile->getAttributeValue(L"class_name");	
					}
					else 
					if (core::stringw("Sound") == XMLFile->getName())
					{
						sound_data_finded = true;
					}
					else 
					if (core::stringw("Dynamic") == XMLFile->getName())
					{
						dynamic_data_finded = true;
					}
				}
				
				if (!scene_data_finded)
				{
					SceneDataXMLPos = XMLFile->getPos();
				}

				if (!game_data_finded)
				{
					GameDataXMLPos = XMLFile->getPos();
				}

				if (!sound_data_finded)
				{
					SoundDataXMLPos = XMLFile->getPos();
				}

				if (!dynamic_data_finded)
				{
					DynamicDataXMLPos = XMLFile->getPos();
				}				
			}

			if (game_node_type!=GameNodeTypeStr[node_params.Game.Type] ||
				node_params.Game.ClassName != class_name
				)
			{
				Msg.sprintf(
					"Waiting '%s - %s' game node from xml, goted '%s - %s', loading process aborted!", 
					GameNodeTypeStr[node_params.Game.Type], 
					node_params.Game.ClassName.c_str(),
					game_node_type.c_str(), 
					class_name.c_str()
					);               
				LOGGER.log(Msg.c_str(), io::ELL_ERROR); 

				SAFE_DROP(XMLFile);
			}		
		}		
	}

	struct SWeaponEntry
	{
		SWeaponEntry() 
			: Number(-1), FileName(""), TargetBoneName(""), Pos(0,0,0), Rot(0,0,0), Enabled(false) {}

		SWeaponEntry(
			s32 number, bool enabled, core::stringc &fname, core::stringc &bone, 
			core::vector3df pos, core::vector3df rot, core::vector3df lpos
			) 
		: Number(number), FileName(fname), TargetBoneName(bone), 
		Pos(pos), Rot(rot), LightPos(lpos), Enabled(enabled) {}

		bool Enabled;
		core::stringc FileName;
		core::stringc TargetBoneName;
		s32 Number;
		core::vector3df Pos, Rot, LightPos;
	};

	void _loadGameNode(SGameNodeCommonParams &node_params, bool cache_it=true)
	{																								  
		if (XMLFile)                                                                                     
		{
			// loading dynamic data

			XMLFile->setPos(DynamicDataXMLPos);
	
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Dynamic") == XMLFile->getName())	
					{   						
						s32 xml_pos = XMLFile->getPos();

						while(XMLFile->read())
						{
							if (XMLFile->getNodeType() == io::EXN_ELEMENT)
							{
								if (core::stringw(dyn::DynamicObjectCorePropertiesTagValue) == XMLFile->getName())
								{ 
									xml_pos = XMLFile->getPos();

									while(XMLFile->read())
									{
										if (XMLFile->getNodeType() == io::EXN_ELEMENT)
										{
											if (core::stringw("DynamicParameters") == XMLFile->getName())
											{ 
												core::stringc dynType = XMLFile->getAttributeValue(L"dynamic_type");

												for (s32 i=0; i<dyn::E_DYNAMIC_OBJECT_TYPE_COUNT; i++)
												{
													if (dynType == dyn::DynamicObjectTypeStr[i])
													{
														node_params.Dynamic.Type = (dyn::E_DYNAMIC_OBJECT_TYPE)i;
														break;
													}
												}

												core::stringc geomForm = XMLFile->getAttributeValue(L"geom_form");

												for (s32 i=0; i<dyn::E_DYN_OBJ_GEOM_FORM_COUNT; i++)
												{
													if (geomForm == dyn::GeomFormStr[i]) 
													{
														node_params.Dynamic.GeomForm = (dyn::E_DYN_OBJ_GEOM_FORM)i;
														break;
													}
												}

												node_params.Dynamic.MassValue          = XMLFile->getAttributeValueAsFloat(L"mass_value");
												node_params.Dynamic.SurfMu             = XMLFile->getAttributeValueAsFloat(L"surf_mu");					
												node_params.Dynamic.SurfBounce         = XMLFile->getAttributeValueAsFloat(L"surf_bounce");
												node_params.Dynamic.SurfBounceVel      = XMLFile->getAttributeValueAsFloat(L"surf_bounce_vel");
												node_params.Dynamic.SurfSoft           = XMLFile->getAttributeValueAsFloat(L"surf_soft");
												node_params.Dynamic.DynamicConstraints = XMLFile->getAttributeValueAsInt(L"dynamic_constraints");						
											}
											else
											{
												break;
											}

											xml_pos = XMLFile->getPos();
										}
									}

									XMLFile->setPos(xml_pos);
								}
								else
								{
									break;
								}

								xml_pos = XMLFile->getPos();
							}
						}

						XMLFile->setPos(xml_pos);
					}
					else
					{
						break;
					}
				}	
			}

			// loading scene data

			XMLFile->setPos(SceneDataXMLPos);

			scn::ISceneNode* scene_node = SCENE_MANAGER.loadSceneNode(XMLFile, cache_it);			

			if (scene_node)                                                                                     
			{       
				scn::SSceneNodeCommonParams scene_params = scene_node->getParameters();	
				scn::SSceneNodeParameters empty_sn_params;
				scn::SSceneNodeTransformation empty_sn_transf;
				
				if (node_params.Scene.Parameters == empty_sn_params)				
					node_params.Scene.Parameters = scene_params.Parameters;

				if (node_params.Scene.Transformation == empty_sn_transf)				
					node_params.Scene.Transformation = scene_params.Transformation;
				
				node_params.Scene.Material = scene_params.Material;

				// adding game node

				GameNode = GAME_MANAGER.addGameNode(node_params, scene_node);       
			}

			if (!GameNode)
				return;	

			// clearing item drop table to load new for this game node

			core::array<SItemDropTableLine>& drop_table =
				IGameManager::getSingleton().getItemDropTable(GameNode->getGameNodeType());
			SItemDropTableLine e(GameNode->getSceneNode()->getFileName());
			s32 idx = drop_table.binary_search(e);
			if (idx==-1)
			{
				drop_table.push_back(e);
				idx = drop_table.size()-1;
			}   
			SItemDropTableLine& drop_table_line = drop_table[idx];
			drop_table_line.Items.set_used(0); 			

			// loading game data

			XMLFile->setPos(GameDataXMLPos);

			bool game_data_finded = false; 

			IGameNodeAI* gnai = SAFE_CAST_TO_GAME_NODE_AI(GameNode);     
			if (gnai)
			{
				IGameNodeMainPlayer* gnplayer = SAFE_CAST_TO_GAME_NODE_MAIN_PLAYER(gnai);				

				core::array<SWeaponEntry> weapons;	
				core::array<SGameCameraStyle> camera_styles;

				while(XMLFile->read())
				{
					if (XMLFile->getNodeType()==io::EXN_ELEMENT)
					{    
						if (core::stringw("Game") == XMLFile->getName())	
						{
							game_data_finded = true;
						}
						else 						
						if (game_data_finded && _parseItemDropTable(drop_table_line, XMLFile)) {}
						else 
						if (game_data_finded && _parseGameCoreParameters(node_params.Game, XMLFile)) 
						{
							GameNode->setParameters(node_params.Game);
						}
						else
						if (game_data_finded && core::stringw("AI") == XMLFile->getName())
						{	
							SGameNodeAIParams ai_par;
							ai_par.ViewDist				= XMLFile->getAttributeValueAsFloat(L"view_dist");
							ai_par.MeleAttackRange		= XMLFile->getAttributeValueAsFloat(L"mele_attack_range");
							ai_par.MeleAttackDamage		= XMLFile->getAttributeValueAsFloat(L"mele_attack_damage");
							ai_par.MeleAttackAngle		= XMLFile->getAttributeValueAsFloat(L"mele_attack_angle");
							ai_par.FireAttackHorizMaxAngle	= XMLFile->getAttributeValueAsFloat(L"fire_attack_horiz_max_angle");
							ai_par.FireAttackVertMaxAngle	= XMLFile->getAttributeValueAsFloat(L"fire_attack_vert_max_angle");
							ai_par.Armor				= XMLFile->getAttributeValueAsFloat(L"armor");
							ai_par.Power				= XMLFile->getAttributeValueAsFloat(L"power");
							ai_par.Dexterity			= XMLFile->getAttributeValueAsFloat(L"dexterity");
							ai_par.Accuracy				= XMLFile->getAttributeValueAsFloat(L"accuracy");
							ai_par.StandReadyMaxTimeMs	= XMLFile->getAttributeValueAsInt  (L"stand_ready_max_time");
							ai_par.MemoryTime			= XMLFile->getAttributeValueAsFloat(L"memory_time");
							ai_par.FriendlyDamageCoeff	= XMLFile->getAttributeValueAsFloat(L"friendly_damage_coeff");
							ai_par.BloodColor			= XMLFile->getAttributeValueAsColor(
								(L"blood_color"), img::SColor(255,255,0,0));

							IGameNodeAI* gnai = SAFE_CAST_TO_GAME_NODE_AI(GameNode);

							if (gnai)
							{
								gnai->setParametersAI(ai_par); 
							}
						}	
						else 
						if (game_data_finded && core::stringw("BindHurtEffect") == XMLFile->getName())
						{
							core::stringc fname  = XMLFile->getAttributeValue(L"filename");
							s32 delay_after_hurt = XMLFile->getAttributeValueAsInt(L"delay_after_hurt");;
							s32 life_time        = XMLFile->getAttributeValueAsInt(L"life_time");	
							bool hit   = XMLFile->getAttributeValueAsInt(L"hit")==0 ? false : true;
							bool death = XMLFile->getAttributeValueAsInt(L"death")==0 ? false : true;
							core::vector3df scale=XMLFile->getAttributeValueAsVector(L"scale", core::vector3df(1,1,1));
							core::vector3df offset=XMLFile->getAttributeValueAsVector(L"offset", core::vector3df(0,0,0));

							if (fname.size()>0)
							{			
								LOGGER.log("Binding hurt effect . . .");					

								core::stringc full_fname = XMLFile->getFilePath();
								full_fname.append(fname.c_str());					

								scn::ISceneNode *snode = 
									SCENE_MANAGER.loadSceneNode(full_fname.c_str());

								if (snode)
								{
									snode->setScale(scale);

									snode->setVisible(false);

									SHurtEffectParameters hurt(snode);
									hurt.DelayAfterHurt= delay_after_hurt;
									hurt.LifeTime      = life_time;									
									hurt.Offset = offset;
									hurt.Hit   = hit;
									hurt.Death = death;

									gnai->bindHurtEffect(hurt);
								}
							}
						}	
						else 
						if (game_data_finded && core::stringw("BindWeapon") == XMLFile->getName())
						{
							s32 number                = XMLFile->getAttributeValueAsInt(L"number"); 
							bool enabled              = XMLFile->getAttributeValueAsBool(L"enabled"); 
							core::stringc filename    = XMLFile->getAttributeValue(L"filename");
							core::stringc target_bone = XMLFile->getAttributeValue(L"target_bone");

							core::vector3df pos = XMLFile->getAttributeValueAsVector(L"pos");
							core::vector3df rot = XMLFile->getAttributeValueAsVector(L"rot");
							core::vector3df lpos = XMLFile->getAttributeValueAsVector(L"lpos");
							
							if (filename.size()>0 && target_bone.size()>0)
							{
								weapons.push_back(SWeaponEntry(
									number, enabled, filename, target_bone, pos, rot, lpos));
							}
						}
						else
						if (game_data_finded && core::stringw("Camera") == XMLFile->getName())	
						{
							s32 xml_pos = XMLFile->getPos();

							while(XMLFile->read())
							{	
								if (XMLFile->getNodeType()==io::EXN_ELEMENT)
								{  
									if (core::stringw("Style") == XMLFile->getName())
									{	
										SGameCameraStyle cam_style;

										cam_style.Name          = XMLFile->getAttributeValue(L"name");
										cam_style.RotationSpeed = XMLFile->getAttributeValueAsFloat(L"rot_speed");
										cam_style.MoveSpeed     = XMLFile->getAttributeValueAsFloat(L"move_speed");
										cam_style.ZoomSpeed     = XMLFile->getAttributeValueAsFloat(L"zoom_speed");
										cam_style.MinZoomValue	= XMLFile->getAttributeValueAsFloat(L"min_zoom");
										cam_style.MaxZoomValue	= XMLFile->getAttributeValueAsFloat(L"max_zoom");
										cam_style.MouseSenseX   = XMLFile->getAttributeValueAsFloat(L"mouse_sense_x");
										cam_style.MouseSenseY   = XMLFile->getAttributeValueAsFloat(L"mouse_sense_y");
										    
										xml_pos = XMLFile->getPos();

										while(XMLFile->read())
										{
											if (XMLFile->getNodeType()==io::EXN_ELEMENT)
											{    
												if (core::stringw("Projection") == XMLFile->getName())	
												{
													cam_style.UpVector      = XMLFile->getAttributeValueAsVector(L"up_vec");
													
													core::stringc type_str  = XMLFile->getAttributeValue(L"type");

													for (s32 i=0; i<scn::E_CAMERA_PROJECTION_TYPE_COUNT; i++)
													{
														if (type_str==scn::CameraProjectionTypeStr[i])
														{
															cam_style.ProjType = (scn::E_CAMERA_PROJECTION_TYPE)i;
														}
													}
												}
												else
												if (core::stringw("CursorLockRect") == XMLFile->getName())	
												{  
													cam_style.setCursorLockRect(
														core::rectf(
															XMLFile->getAttributeValueAsFloat(L"left"), 
															XMLFile->getAttributeValueAsFloat(L"top"), 
															XMLFile->getAttributeValueAsFloat(L"right"), 
															XMLFile->getAttributeValueAsFloat(L"bottom")
															)
														);  

													xml_pos = XMLFile->getPos();

													bool min_zoom_tag = false;
													bool max_zoom_tag = false;

													while(XMLFile->read())
													{
														if (XMLFile->getNodeType()==io::EXN_ELEMENT)
														{    
															if (core::stringw("ActiveRectRel") ==
																	XMLFile->getName())	
															{
																cam_style.setCursorActiveRectRel(core::rectf(
																	XMLFile->getAttributeValueAsFloat(L"left"),
																	XMLFile->getAttributeValueAsFloat(L"top"),
																	XMLFile->getAttributeValueAsFloat(L"right"),
																	XMLFile->getAttributeValueAsFloat(L"bottom")));
															}
															else 
															if (core::stringw("FirstZoom") == XMLFile->getName())
															{
																min_zoom_tag = true;
																max_zoom_tag = false;
																
																cam_style.FirstZoom.Value     = XMLFile->getAttributeValueAsFloat(L"value");
																cam_style.FirstZoom.UseCenter = XMLFile->getAttributeValueAsBool(L"use_center");

																cam_style.FirstZoom.Center.FOVDegrees         = XMLFile->getAttributeValueAsFloat(L"fov_deg");
																cam_style.FirstZoom.Center.NearClip          = XMLFile->getAttributeValueAsFloat(L"near_clip");
																cam_style.FirstZoom.Center.FarClip           = XMLFile->getAttributeValueAsFloat(L"far_clip"); 													
																cam_style.FirstZoom.Center.ViewVolume.Width  = XMLFile->getAttributeValueAsFloat(L"view_width");
																cam_style.FirstZoom.Center.ViewVolume.Height = XMLFile->getAttributeValueAsFloat(L"view_height");  													

																cam_style.FirstZoom.LeftBottom.FOVDegrees = cam_style.FirstZoom.LeftTop.FOVDegrees =
																	cam_style.FirstZoom.RightBottom.FOVDegrees = cam_style.FirstZoom.RightTop.FOVDegrees =
																		cam_style.FirstZoom.Center.FOVDegrees;

																cam_style.FirstZoom.LeftBottom.NearClip = cam_style.FirstZoom.LeftTop.NearClip =
																	cam_style.FirstZoom.RightBottom.NearClip = cam_style.FirstZoom.RightTop.NearClip =
																		cam_style.FirstZoom.Center.NearClip;

																cam_style.FirstZoom.LeftBottom.FarClip = cam_style.FirstZoom.LeftTop.FarClip =
																	cam_style.FirstZoom.RightBottom.FarClip = cam_style.FirstZoom.RightTop.FarClip =
																		cam_style.FirstZoom.Center.FarClip;

																cam_style.FirstZoom.LeftBottom.ViewVolume = cam_style.FirstZoom.LeftTop.ViewVolume =
																	cam_style.FirstZoom.RightBottom.ViewVolume = cam_style.FirstZoom.RightTop.ViewVolume =
																		cam_style.FirstZoom.Center.ViewVolume;
															}
															else 
															if (core::stringw("SecondZoom") == XMLFile->getName())
															{
																min_zoom_tag = false;
																max_zoom_tag = true;
																
																cam_style.SecondZoom.Value     = XMLFile->getAttributeValueAsFloat(L"value");
																cam_style.SecondZoom.UseCenter = XMLFile->getAttributeValueAsBool(L"use_center");

																cam_style.SecondZoom.Center.FOVDegrees         = XMLFile->getAttributeValueAsFloat(L"fov_deg");
																cam_style.SecondZoom.Center.NearClip          = XMLFile->getAttributeValueAsFloat(L"near_clip");
																cam_style.SecondZoom.Center.FarClip           = XMLFile->getAttributeValueAsFloat(L"far_clip"); 													
																cam_style.SecondZoom.Center.ViewVolume.Width  = XMLFile->getAttributeValueAsFloat(L"view_width");
																cam_style.SecondZoom.Center.ViewVolume.Height = XMLFile->getAttributeValueAsFloat(L"view_height");  													

																cam_style.SecondZoom.LeftBottom.FOVDegrees = cam_style.SecondZoom.LeftTop.FOVDegrees =
																	cam_style.SecondZoom.RightBottom.FOVDegrees = cam_style.SecondZoom.RightTop.FOVDegrees =
																		cam_style.SecondZoom.Center.FOVDegrees;

																cam_style.SecondZoom.LeftBottom.NearClip = cam_style.SecondZoom.LeftTop.NearClip =
																	cam_style.SecondZoom.RightBottom.NearClip = cam_style.SecondZoom.RightTop.NearClip =
																		cam_style.SecondZoom.Center.NearClip;

																cam_style.SecondZoom.LeftBottom.FarClip = cam_style.SecondZoom.LeftTop.FarClip =
																	cam_style.SecondZoom.RightBottom.FarClip = cam_style.SecondZoom.RightTop.FarClip =
																		cam_style.SecondZoom.Center.FarClip;

																cam_style.SecondZoom.LeftBottom.ViewVolume = cam_style.SecondZoom.LeftTop.ViewVolume =
																	cam_style.SecondZoom.RightBottom.ViewVolume = cam_style.SecondZoom.RightTop.ViewVolume =
																		cam_style.SecondZoom.Center.ViewVolume;
															}
															else 
															if ((min_zoom_tag || max_zoom_tag) && 
																(core::stringw("Center") == XMLFile->getName())	
															   )
															{
																SCameraZoom &cz = min_zoom_tag?cam_style.FirstZoom:cam_style.SecondZoom;
																cz.Center.LookatTranslation.set(XMLFile->getAttributeValueAsVector(L"lookat_transl"));
																cz.Center.PositionTranslation.set(XMLFile->getAttributeValueAsVector(L"pos_transl"));  			
															} 
															else 
															if ((min_zoom_tag || max_zoom_tag) && 
																(core::stringw("LeftTop") == XMLFile->getName())	
															   )
															{
																SCameraZoom &cz = min_zoom_tag?cam_style.FirstZoom:cam_style.SecondZoom;
																cz.LeftTop.LookatTranslation.set(XMLFile->getAttributeValueAsVector(L"lookat_transl"));
																cz.LeftTop.PositionTranslation.set(XMLFile->getAttributeValueAsVector(L"pos_transl"));  			
															} 
															else  
															if ((min_zoom_tag || max_zoom_tag) &&
																(core::stringw("RightTop") == XMLFile->getName())	
																)
															{
																SCameraZoom &cz = min_zoom_tag?cam_style.FirstZoom:cam_style.SecondZoom;
																cz.RightTop.LookatTranslation.set(XMLFile->getAttributeValueAsVector(L"lookat_transl"));
																cz.RightTop.PositionTranslation.set(XMLFile->getAttributeValueAsVector(L"pos_transl"));
															} 
															else

															if ((min_zoom_tag || max_zoom_tag) &&
																(core::stringw("LeftBottom") == XMLFile->getName())	
																)
															{
																SCameraZoom &cz = min_zoom_tag?cam_style.FirstZoom:cam_style.SecondZoom;
																cz.LeftBottom.LookatTranslation.set(XMLFile->getAttributeValueAsVector(L"lookat_transl"));
																cz.LeftBottom.PositionTranslation.set(XMLFile->getAttributeValueAsVector(L"pos_transl"));
															} 
															else
															if ((min_zoom_tag || max_zoom_tag) &&
																(core::stringw("RightBottom") == XMLFile->getName())	
																)
															{
																SCameraZoom &cz = min_zoom_tag?cam_style.FirstZoom:cam_style.SecondZoom;
																cz.RightBottom.LookatTranslation.set(XMLFile->getAttributeValueAsVector(L"lookat_transl"));
																cz.RightBottom.PositionTranslation.set(XMLFile->getAttributeValueAsVector(L"pos_transl"));
															} 
															else
															{
																break;
															}
														} 

														xml_pos = XMLFile->getPos();
													}

													XMLFile->setPos(xml_pos);  
												}
												else
												{
													break;
												}
											} 

											xml_pos = XMLFile->getPos();
										}

										XMLFile->setPos(xml_pos);

										camera_styles.push_back(cam_style);                            							
									}
									else 	
									{
										break;
									}
								}

								xml_pos = XMLFile->getPos();
							}

							XMLFile->setPos(xml_pos);
						}
						else 
						if (notGameSector(XMLFile))	
						{
							break;
						}
					}
				}

				for (u32 w=0; w<weapons.size(); w++)
				{
					LOGGER.log("Binding weapon . . .");	

					SGameNodeCommonParams weapon_params;

					weapon_params.Game.Type = EGNT_WEAPON;

					weapon_params.Scene.Parameters.ID = -1;
					weapon_params.Dynamic.Type = dyn::EDOT_NONE;
					weapon_params.Dynamic.GeomForm = dyn::EDOGF_BOX;
					weapon_params.Dynamic.DynamicConstraints = dyn::EDTC_NO_TRANSF;
	
					IGameNode* weapon_node = 
						GAME_MANAGER.loadGameNode(weapon_params, weapons[w].FileName.c_str());  
							
					IGameNodeWeapon* gnode_weapon = SAFE_CAST_TO_GAME_NODE_WEAPON(weapon_node);

					if (gnode_weapon)
					{
						gnode_weapon->setAutoCreatedFlag(true);

						gnode_weapon->setFlashLightPosition(weapons[w].LightPos);

						gnai->bindWeapon(
							weapons[w].Number, gnode_weapon, weapons[w].TargetBoneName.c_str()
							);

						gnode_weapon->setWeaponEnabled(weapons[w].Enabled);

						scn::ISceneNode *anode = gnode_weapon->getSceneNode();

						anode->setPosition(weapons[w].Pos);
						anode->setRotation(weapons[w].Rot);

						core::stringc name;
						name.sprintf("weapon.%p.%s", anode, weapons[w].FileName.c_str());

						anode->setName(name.c_str());
					}
				}
				
				gnai->selectWeapon(0); 

				if (gnplayer)
				{
					for (u32 c=0; c<camera_styles.size(); c++)
					{
						gnplayer->addCameraStyle(camera_styles[c]);

						core::stringc msg;
						msg.sprintf("Added '%s' camera style", camera_styles[c].Name.c_str());
						LOGGER.log(msg);	
					}

					if (camera_styles.size()>0)
					{	
						// default camera style
						gnplayer->setCameraStyle(camera_styles[0].Name.c_str());
					}
				}
			}
			else
			{
				while(XMLFile->read())
				{
					if (XMLFile->getNodeType()==io::EXN_ELEMENT)
					{    
						if (core::stringw("Game") == XMLFile->getName())	
						{
							game_data_finded = true;
						}	
						else 						
						if (game_data_finded && _parseItemDropTable(drop_table_line, XMLFile)) {}
						else 						
						if (game_data_finded && _parseGameCoreParameters(node_params.Game, XMLFile)) 
						{
							GameNode->setParameters(node_params.Game);
						}
						else 
						if (notGameSector(XMLFile))	
						{
							break;
						}					
					}
				} 
			}

			// loading sound data

			XMLFile->setPos(SoundDataXMLPos);

			bool sound_data_finded = false;		
			
			core::array<SSoundEffectEntry> SoundEffects;
			
			while(XMLFile->read())
			{
				if (XMLFile->getNodeType()==io::EXN_ELEMENT)
				{    
					if (core::stringw("Sound") == XMLFile->getName())	
					{
						sound_data_finded = true;
					}
					else 
					if (sound_data_finded && core::stringw("SoundEffect") == XMLFile->getName())
					{
						core::stringc snd_fname = XMLFile->getAttributeValue(L"filename");
						bool looping = XMLFile->getAttributeValueAsInt(L"loop") ==0 ? false : true;

						SoundEffects.push_back(SSoundEffectEntry(snd_fname, looping));
					}
					else
					{
						break;
					}
				}
			}

			// adding sound effects
			core::stringc sound_dir = 
				RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_EFFECTS );

			for (u32 s=0; s<SoundEffects.size(); s++)
			{
				core::stringc full_filename = sound_dir.c_str();
				full_filename.append(SoundEffects[s].FileName.c_str());

				mm::ISound * snd = 
					SOUND_DRIVER.getSound(full_filename.c_str(), SoundEffects[s].Looping);

				if (snd)
				{
					snd->setVolume(GAME_MANAGER.getSoundEffectsVolume());

					GameNode->addSoundEffect(snd);	
				}
			}	
		} 	
	}

	void _postLoadGameNode(SGameNodeCommonParams &node_params)
	{	
		E_GAME_NODE_TYPE gntype = node_params.Game.Type;

		LOGGER.decreaseFormatLevel();

		if (!GameNode)                                                                                    
		{                                                                                             
			Msg.sprintf(
				"Error loading %s Node from %s", 
				GameNodeTypeShortStr[gntype], XMLFileName.c_str()
				);               
			LOGGER.log(Msg.c_str(), io::ELL_ERROR);                                                   
		}                                                                                             
		else                                                                                          
		{                                                                                             
			Msg.sprintf(
				"%s Node successfully loaded from %s", 
				GameNodeTypeShortStr[gntype], XMLFileName.c_str()
				);         
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);   
		} 

		SAFE_DROP(XMLFile);
	}
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef GameClassesRegistryHPP