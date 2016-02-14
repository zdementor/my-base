//|-------------------------------------------------------------------------
//| File:        CGameNodeLevelMap.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CGameNodeLevelMap.h"

#include <game/IGameNodeDoor.h>
#include <game/IGameNodePlatform.h>
#include <game/IGameNodeTrigger.h>
#include <game/IGameNodeButton.h>
#include <scn/ILightSceneNode.h>
#include <img/IImageLibrary.h>
#include <dyn/IDynamicManager.h>
#include <scn/IPathFinderManager.h>
#include <scn/IBSPLevelMesh.h>
#include <scn/IDummyTransformationSceneNode.h>
#include <scn/ITestSceneNode.h>
#include <scn/IPathFinder.h>
#include <scn/IKTreeSceneNode.h>
#include <io/ICursorControl.h>

#include <game/GameClassesRegistry.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

const f32 MIN_TRANSP_DIST = 0.001f;

//----------------------------------------------------------------------------

//! constructor
CGameNodeLevelMap::CGameNodeLevelMap(
	scn::ISceneNode* n, SGameNodeParams &params
	) : 
IGameNodeLevelMap(n, SGameNodeParams(params, EGNT_LEVEL_MAP))
{
#if MY_DEBUG_MODE  
	setClassName("CGameNodeLevelMap");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CGameNodeLevelMap::~CGameNodeLevelMap()
{
	clearLevelParts();
}

//----------------------------------------------------------------------------

//!
void CGameNodeLevelMap::clearLevelParts()
{
	m_SceneNode->removeAll();
	for (u32 i=0; i<m_LevelParts.size(); i++)
		SAFE_DROP(m_LevelParts[i].SceneNode);		
	m_LevelParts.clear();
}

//----------------------------------------------------------------------------

//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeLevelMap::OnPreDynamic(f64 delta_time_sec)
{
	IGameNode::OnPreDynamic(delta_time_sec);	

	bool editor = SCENE_MANAGER.isSceneEditorStarted();

	core::vector3df player_pos(0,0,0), cam_pos(0,0,0), cam_view_vec(0,0,0);

	IGameNode* main_player = 
		GAME_MANAGER.getMainPlayerGameNode();
	if (main_player)
		player_pos = main_player->getDislocation();

	scn::ISceneManager		&smgr		= SCENE_MANAGER;
	io::ICursorControl		&cursor		= CURSOR_CONTROL;
	scn::IPathFinderManager	&pathfinder	= PATH_FINDER_MANAGER;

	bool orthogonal = false;
	scn::ICameraSceneNode* cam = smgr.getActiveCamera();
	if (cam)
	{
		cam_pos = cam->getAbsolutePosition();	

		cam_view_vec = cam->getTarget()-cam_pos;
		cam_view_vec.normalize();
		orthogonal = cam->getProjectionType()==scn::ECPT_ORTHOGONAL;
	}

	core::vector3df control_pos;
	if (editor)
		control_pos=cam_pos;
	else
		control_pos=player_pos;

	for (u32 lp=0; lp<m_LevelParts.size(); lp++)
	{
		m_LevelParts[lp].OpasDetected = false;
		m_LevelParts[lp].WasTransparent = m_LevelParts[lp].OpasWanted < 1.0f;

		if (!m_LevelParts[lp].SceneNode || (
				m_LevelParts[lp].SceneNode->isVisible() &&
				m_LevelParts[lp].SceneNode->isCulled()))
		{
			m_LevelParts[lp].OpasWanted = 1.0f;
			m_LevelParts[lp].OpasDetected = true;
			continue;
		}

		core::aabbox3df bb = 
			m_LevelParts[lp].SceneNode->getTransformedBoundingBox();
		bb.MinEdge.Y = player_pos.Y;

		f32 new_opas = m_LevelParts[lp].TranspDist > MIN_TRANSP_DIST ?
			-1.0f : m_LevelParts[lp].OpasWanted;

		static core::plane3df bb_sides[4];
		bb.getSidePlanes(bb_sides);

		if (orthogonal)
		{
			if (m_LevelParts[lp].TranspDist==MIN_TRANSP_DIST)
				cam_pos = player_pos;
			else
				cam_pos = cam_view_vec;
		}

		if (control_pos.X >= bb.MinEdge.X && control_pos.X <= bb.MaxEdge.X &&
			control_pos.Z >= bb.MinEdge.Z && control_pos.Z <= bb.MaxEdge.Z)
		{
			new_opas = 0.0f;
			m_LevelParts[lp].OpasDetected = true;
		}

		if (!editor)
		{
			if (new_opas != 0.0f && m_LevelParts[lp].TranspDist > MIN_TRANSP_DIST)		
			{
				f32 opas = 0.0f;
				static core::array <core::plane3df> occl_planes;
				occl_planes.set_used(0);

				bb.getOcclusionPlanes(cam_pos, occl_planes, orthogonal);

				s32 occl_size = occl_planes.size();
				f32 d1 = FLT_MIN;
				for (s32 p=0; p<occl_size; p++)
				{
					f32 d = occl_planes[p].getDistanceTo(player_pos);
					if (d > d1)
						d1 = d;
				}
				opas = d1 / m_LevelParts[lp].TranspDist;
				new_opas = opas;
				m_LevelParts[lp].OpasDetected = true;
			}
		}

		if (new_opas != 0.0f && m_LevelParts[lp].TranspDist > MIN_TRANSP_DIST)
		{
			f32 opas = 0.0f;
			f32 d1 = 0.0f, d2 = 0.0f; 
			for (s32 s=0; s<4; s++)
			{
				f32 d = bb_sides[s].getDistanceTo(control_pos);
				if (d>0)
				// front
				{
					if (d1==0.0f)	{ d1 = d; }
					else			{ d2 = d; break; }
				}
			}
			opas = core::math::Sqrt(d1*d1 + d2*d2) / m_LevelParts[lp].TranspDist;
			if (new_opas==-1.0f || opas<new_opas)
				new_opas = opas;
			m_LevelParts[lp].OpasDetected = true;
		}

		if (new_opas>1.0f)
			new_opas=1.0f;
		else
		if (new_opas<0.001f)
			new_opas=0.0f;

		m_LevelParts[lp].OpasWanted = new_opas;
	}

	for (u32 lp=0; lp<m_LevelParts.size(); lp++)
	{
		s32 targeted_by_idx = m_LevelParts[lp].TargetedByIdx;
		if (targeted_by_idx != -1)
		{
			if (m_LevelParts[targeted_by_idx].WasTransparent)
			{
				s32 team_size = m_LevelParts[lp].TeamIdxs.size();
				for (s32 i=0; i<team_size; i++)
				{
					s32 team_idx = m_LevelParts[lp].TeamIdxs[i];
					if (!m_LevelParts[team_idx].OpasDetected)
					{
						m_LevelParts[team_idx].OpasWanted = m_LevelParts[targeted_by_idx].OpasWanted;
						m_LevelParts[team_idx].OpasDetected = true;
					}
				}
			}
			m_LevelParts[lp].OpasWanted = m_LevelParts[targeted_by_idx].OpasWanted;
		}
	}
}

//----------------------------------------------------------------------------

//! calling every time after doing all game stuff (dynamic, scriptin, etc.)
void CGameNodeLevelMap::OnPostDynamic(f64 delta_time_sec)
{
	s32 cnt=0;

	for (u32 lp=0; lp<m_LevelParts.size(); lp++)
	{
		if (!m_LevelParts[lp].SceneNode || (
				m_LevelParts[lp].SceneNode->isVisible() &&
				m_LevelParts[lp].SceneNode->isCulled())) 
			continue;

		cnt++;

		f32 &opasity = m_LevelParts[lp].OpasCurrent;
		f32 &tgt_opasity = m_LevelParts[lp].OpasWanted;
		s32 team_size = m_LevelParts[lp].TeamIdxs.size();
		for (s32 i=0; i<team_size; i++)
		{
			s32 team_idx = m_LevelParts[lp].TeamIdxs[i];
			f32 opas = m_LevelParts[team_idx].OpasWanted;
			if (opas < tgt_opasity)
				tgt_opasity = opas;
		}
		CHECK_RANGE(tgt_opasity, 0.0f, 1.0f);

		opasity = tgt_opasity;

		if (opasity == 0)
		{
			m_LevelParts[lp].SceneNode->setVisible(false);
		}
		else
		{
			m_LevelParts[lp].SceneNode->setVisible(true);
			f32 alpha = opasity * 255.0f;
			u32 mcnt = m_LevelParts[lp].SceneNode->getMaterialsCount();
			for (u32 m = 0; m < mcnt; m++)
			{
				SLevelPart &lpart = m_LevelParts[lp];
				vid::SMaterial &mat = lpart.SceneNode->getMaterial(m);
				u32 pcnt = mat.getPassesCount();
				for (u32 p=0; p < pcnt; p++)
				{	
					vid::SRenderPass &pass = mat.getPass(p);
					if (!lpart.BlendingAlwaysRequired)
					{
						if (alpha >= 255)
							pass.setFlag(vid::EMF_BLENDING, false);
						else
							pass.setFlag(vid::EMF_BLENDING,	true);
					}
					img::SColor c = pass.getDiffuseColor();
					c.setAlpha(alpha);
					pass.setDiffuseColor(c);
				}
			}
		}

		if (m_LevelParts[lp].Occluder && opasity == 1.0f)
			m_LevelParts[lp].SceneNode->setOccluder(true);
		else
			m_LevelParts[lp].SceneNode->setOccluder(false);
	}
}

//----------------------------------------------------------------------------

//! 
void CGameNodeLevelMap::addLevelPart(const SLevelPart& level_part)
{	
	m_LevelParts.push_back(level_part);	

	SLevelPart& new_lpart = m_LevelParts[m_LevelParts.size()-1];

	CHECK_MIN_RANGE(new_lpart.TranspDist, MIN_TRANSP_DIST);

	scn::ISceneNode* snode = level_part.SceneNode; 

	SAFE_GRAB(snode);
}

//----------------------------------------------------------------------------

void fillWalkabilityMap(
	img::IImage *walk_map_out, img::IImage *height_map, s32 xorig, s32 yorig, core::aabbox3df &bbox
	);
const f32 WALK_DELTA_H_LIMIT = 32.001f;

img::SColor empty_color (255,0,0,255);
img::SColor unwalk_color(255,255,0,0);
img::SColor walk_color  (255,0,0,0);

//----------------------------------------------------------------------------

//!
void CGameNodeLevelMap::initializeLevelParts()
{
	SGameNodeCommonParams node_params = getGameCommonParams();

	scn::ISceneNode* scene_node = m_SceneNode;
	scn::IKTreeSceneNode* mesh_snode = SAFE_CAST_TO_KTREE_SCENE_NODE(scene_node);

	scn::ISceneManager& smgr = SCENE_MANAGER;
	game::IGameManager& gmgr = GAME_MANAGER;

	clearLevelParts();

	if (mesh_snode)
	{
		scn::IAnimatedMesh* msh = mesh_snode->getAnimatedMesh();

		if (msh && msh->getAnimatedMeshType() == scn::EMT_BSP)
		{
			core::array<SLevelMapToLoadEntry> decorations_to_load;

			core::array<scn::ISceneNode*> child_scene_nodes;

			core::vector3df level_pos = mesh_snode->getPosition();
			core::vector3df level_rot = mesh_snode->getRotation();

			mesh_snode->setPosition(core::vector3df(0,0,0));
			mesh_snode->setRotation(core::vector3df(0,0,0));
			mesh_snode->updateAbsoluteTransformation();

			core::array<IGameNodeActivatable*> act_nodes;	

			s32 doorcnt = ((scn::IBSPLevelMesh*)msh)->getEntitiesCount(scn::EBSPET_FUNC_DOOR);

			LOGGER.logInfo("Loading doors (%d to go) . . .", doorcnt);

			LOGGER.increaseFormatLevel();

			s32 dcnt = 0, pcnt = 0, d = 0;

			core::stringc ch;

			for (d=0; d<doorcnt; d++)
			{
				scn::IBSPEntity &q3ent = 
					((scn::IBSPLevelMesh*)msh)->getEntity(scn::EBSPET_FUNC_DOOR, d);

				scn::IAnimatedMesh *doormesh = q3ent.getModelMesh();

				if (!doormesh)
					continue;

				// angle : determines the opening direction of door (up = -1, down = -2).
				s32 angle      = q3ent.getKeyValueAsFloat("angle");						

				// door sounds file names
				core::stringc door_start_snd = q3ent.getKeyValue("sound_start");
				core::stringc door_end_snd   = q3ent.getKeyValue("sound_end");

				core::stringc target     = q3ent.getKeyValue("target");
				core::stringc targetname = q3ent.getKeyValue("targetname");
				s32           spawnflags = q3ent.getKeyValueAsInt("spawnflags");
				f32           lip        = q3ent.getKeyValueAsInt("lip");
				core::stringc teamname   = "";//q3ent.getKeyValue("team");

				scn::ISceneNode *door = smgr.addAnimatedMeshSceneNode();

				SGameNodeCommonParams door_params = node_params;

				core::stringc name = "";
				if (angle==-1 || angle==-2)
				{
					door_params.Game.Type = EGNT_PLATFORM;
					name.sprintf("platform.bsp.%p", door);								
				}
				else
				{
					door_params.Game.Type = EGNT_DOOR;
					name.sprintf("door.bsp.%p", door);								
				}

				LOGGER.logInfo("Loading %d door '%s':", d, name);

				LOGGER.increaseFormatLevel();

				door_params.Game.Life = 0.0f;

				door_params.Scene.Parameters.ID = -1;
				door_params.Scene.Parameters.Name = name.c_str();
				door_params.Scene.Transformation.Position.set(0,0,0);
				door_params.Scene.Transformation.Rotation.set(0,0,0);

				door_params.Dynamic.Type = dyn::EDOT_PSEUDO_DYNAMIC;
				door_params.Dynamic.GeomForm = dyn::EDOGF_BOX;	
				door_params.Dynamic.DynamicConstraints = dyn::EDTC_NO_TRANSF;							
				
				scn::IAnimatedMeshSceneNode *animated_mesh_scene_node =
					SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(door);
				if (animated_mesh_scene_node)
					animated_mesh_scene_node->setAnimatedMesh(doormesh);
				doormesh->clearGeometry();

				door->lockMaterials();
				door->setParent(mesh_snode);

				child_scene_nodes.push_back(door);

				IGameNode* game_node = gmgr.addGameNode(door_params, door); 
				
				if (game_node)
				{
					game_node->setAutoCreatedFlag(true);

					// adding sound effects

					core::stringc sound_dir = 
						RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_EFFECTS );

					if (door_start_snd.size()>0)
					{
						core::stringc full_filename = sound_dir.c_str();
						full_filename.append(door_start_snd.c_str());
						game_node->addSoundEffect(
							SOUND_DRIVER.getSound(full_filename.c_str(), false));
					}
					if (door_end_snd.size()>0)
					{
						core::stringc full_filename = sound_dir.c_str();
						full_filename.append(door_end_snd.c_str());
						game_node->addSoundEffect(
							SOUND_DRIVER.getSound(full_filename.c_str(), false));
					}
				}
				
				IGameNodeDoor* gnode_door = 
					SAFE_CAST_TO_GAME_NODE_DOOR(game_node);

				IGameNodePlatform* gnode_platform = 
					SAFE_CAST_TO_GAME_NODE_PLATFORM(game_node);

				if (gnode_door)
				{
					act_nodes.push_back(gnode_door);

					gnode_door->setLip(lip);
					gnode_door->setTargetNameKey(targetname.c_str());
					gnode_door->setTeamNameKey(teamname.c_str());
					gnode_door->setTargetKey(target.c_str());

					gnode_door->setOpenAngle(angle);

					// attaching trigger to the door

					core::stringc tr_ch;
					tr_ch.sprintf("trigger.%s", name.c_str());

					core::aabbox3df bbox = door->getBoundingBox();

					SGameNodeCommonParams trigger_params = node_params;

					trigger_params.Game.Type = EGNT_TRIGGER;
					trigger_params.Game.Life = 0.0f;
					trigger_params.Scene.Parameters.ID = -1;
					trigger_params.Scene.Parameters.Name = tr_ch.c_str();
					trigger_params.Dynamic.Type = dyn::EDOT_SENSELESS;
					trigger_params.Dynamic.GeomForm = dyn::EDOGF_BOX;
					trigger_params.Scene.Parameters.Type = scn::ESNT_TEST_SCENE_NODE;
					trigger_params.Dynamic.DynamicConstraints = dyn::EDTC_NO_TRANSF;

					game_node = gmgr.createGameNode(trigger_params, true);
					
					IGameNodeTrigger* gnode_trigger = SAFE_CAST_TO_GAME_NODE_TRIGGER(game_node);

					if (gnode_trigger)
					{
						gnode_trigger->setAutoCreatedFlag(true);
						gnode_trigger->addTargetNode(gnode_door);									

						scn::ISceneNode *trigger = gnode_trigger->getSceneNode();

						if(trigger)
						{
							trigger->setParent(door);
							trigger->setPosition(bbox.getCenter());

							scn::ITestSceneNode *test_snode = 
								static_cast<scn::ITestSceneNode*>(trigger);

							if (test_snode)
							{
								test_snode->setScale(
									bbox.getExtend() + core::vector3df(200.0f, 200.0f, 200.0f));

								vid::SRenderPass &pass = test_snode->getMaterial(0).getPass(0);
								pass.setAlphaGen ( vid::EAGT_CUSTOM );

								img::SColor ac = pass.getAmbientColor ();
								img::SColor dc = pass.getDiffuseColor ();

								ac.setAlpha(50);
								dc.setAlpha(50);

								pass.setAmbientColor ( ac );
								pass.setDiffuseColor ( dc );

								pass.setFlag(vid::EMF_BLENDING, true);
								pass.setBlendFuncs(
									vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
								pass.setLightingMode(vid::ELM_8_LIGHTS);
							}									

							trigger->setVisible(false);
						}									
					}

					dcnt++;
				}
				else if (gnode_platform)
				{
					act_nodes.push_back(gnode_platform);

					gnode_platform->setLip(lip);
					gnode_platform->setTargetNameKey(targetname.c_str());
					gnode_platform->setTeamNameKey(teamname.c_str());
					gnode_platform->setTargetKey(target.c_str());

					pcnt++;
				}
				LOGGER.decreaseFormatLevel();
			}
			
			LOGGER.decreaseFormatLevel();
			
			LOGGER.logInfo(" %d/%d doors/platforms loaded", dcnt, pcnt);
			
			s32 trcnt = ((scn::IBSPLevelMesh*)msh)->getEntitiesCount(
				scn::EBSPET_FUNC_TRANSPARENT);

			LOGGER.logInfo("Loading level parts (%d to go) . . .", trcnt);

			LOGGER.increaseFormatLevel();
			
			s32 tcnt=0, tskipped=0;

			for (s32 tr=0; tr<trcnt; tr++)
			{
				scn::IBSPEntity &q3ent = ((scn::IBSPLevelMesh*)msh)->getEntity(
					scn::EBSPET_FUNC_TRANSPARENT, tr);

				scn::IAnimatedMesh *part_mesh = q3ent.getModelMesh();

				scn::IAnimatedMeshSceneNode *part_node = NULL;
				if (part_mesh)
				{
					part_node = smgr.addAnimatedMeshSceneNode();
					part_node->setAnimatedMesh(part_mesh);
					part_mesh->clearGeometry();
				}							     										
				
				core::stringc target     = q3ent.getKeyValue("target");
				core::stringc targetname = q3ent.getKeyValue("targetname");
				core::stringc teamname   = q3ent.getKeyValue("team");
				s32 dist                 = q3ent.getKeyValueAsInt("transp_dist");
				bool nooccluder          = (bool)BIT_VALUE(q3ent.getKeyValueAsInt("spawnflags"),1);

				if (!part_mesh && teamname.size()==0)
				{
					tskipped++;
					continue;
				}

				SLevelPart level_part;
				level_part.SceneNode     = part_node;
				level_part.TargetKey     = target;
				level_part.TargetNameKey = targetname;
				level_part.TeamKey       = teamname;
				level_part.TranspDist    = dist;
				level_part.Occluder      = !nooccluder;
				
				addLevelPart(level_part);							

				LOGGER.logInfo(" %d level part, team='%s', occluder=%d",
					tcnt, teamname.c_str(), !nooccluder);
					
				tcnt++;
			}
			
			LOGGER.decreaseFormatLevel();
			LOGGER.logInfo(" %d level parts loaded, %d skipped", tcnt, tskipped);

			connectLevelParts();
	
			// loading buttons

			s32 buttoncnt = ((scn::IBSPLevelMesh*)msh)->getEntitiesCount(
				scn::EBSPET_FUNC_BUTTON);

			LOGGER.logInfo("Loading buttons (%d to go) . . .",
				buttoncnt);

			LOGGER.increaseFormatLevel();

			s32 bcnt = 0, bskipped=0;

			for (s32 b=0; b<buttoncnt; b++)
			{
				scn::IBSPEntity &q3ent = ((scn::IBSPLevelMesh*)msh)->getEntity(
					scn::EBSPET_FUNC_BUTTON, b);

				scn::IAnimatedMesh *buttonmesh = q3ent.getModelMesh();

				if (!buttonmesh)
				{
					bskipped++;
					continue;
				}
			
				scn::IAnimatedMeshSceneNode *button = smgr.addAnimatedMeshSceneNode();
				button->setAnimatedMesh(buttonmesh);
				button->lockMaterials();
				button->setParent(mesh_snode);

				child_scene_nodes.push_back(button);

				c8 ch[255];
				sprintf(ch, "button.bsp.%p", button);	

				SGameNodeCommonParams button_params = node_params;

				button_params.Game.Type = EGNT_BUTTON;
				button_params.Game.Life = 1.0f;

				button_params.Scene.Parameters.ID = -1;
				button_params.Scene.Parameters.Name = ch;
				button_params.Scene.Transformation.Position.set(0,0,0);
				button_params.Scene.Transformation.Rotation.set(0,0,0);

				button_params.Dynamic.Type = dyn::EDOT_PSEUDO_DYNAMIC;
				button_params.Dynamic.GeomForm = dyn::EDOGF_BOX;
				button_params.Dynamic.DynamicConstraints = dyn::EDTC_NO_TRANSF;

				IGameNode* game_node = gmgr.addGameNode(button_params, button);  
				
				IGameNodeButton* gnode_button = 
					SAFE_CAST_TO_GAME_NODE_BUTTON(game_node);

				if (gnode_button)
				{
					act_nodes.push_back(gnode_button);

					gnode_button->setAutoCreatedFlag(true);								

					// button sounds file names
					core::stringc button_start_snd = q3ent.getKeyValue("sound_start");
					core::stringc button_end_snd   = q3ent.getKeyValue("sound_end");

					// angle : determines the direction in which the button will move
					// (up = -1, down = -2)
					gnode_button->setOpenAngle( q3ent.getKeyValueAsFloat("angle") );
						
					gnode_button->setWaitTimeSec( q3ent.getKeyValueAsInt("wait") );
					gnode_button->setMoveSpeed( q3ent.getKeyValueAsInt("speed") );
					gnode_button->setTargetKey( q3ent.getKeyValue("target") );								

					// adding button sound effects

					core::stringc sound_dir = 
						RESOURCE_MANAGER.getMediaDirFull ( res::EMT_SOUND_EFFECTS );

					if (button_start_snd.size()>0)
					{
						core::stringc full_filename = sound_dir.c_str();
						full_filename.append(button_start_snd.c_str());
						gnode_button->addSoundEffect(
							SOUND_DRIVER.getSound(full_filename.c_str(), false));
					}
					if (button_end_snd.size()>0)
					{
						core::stringc full_filename = sound_dir.c_str();
						full_filename.append(button_end_snd.c_str());
						gnode_button->addSoundEffect(
							SOUND_DRIVER.getSound(full_filename.c_str(), false));
					}								
				}
				else
				{
					button->remove();
					button = 0;
				}

				if (button)
					bcnt++;
			}	
			
			LOGGER.decreaseFormatLevel();
			LOGGER.logInfo(" %d buttons loaded, %d skipped", bcnt,bskipped);					

			s32 lightcnt = ((scn::IBSPLevelMesh*)msh)->getEntitiesCount(scn::EBSPET_LIGHT);
			LOGGER.logInfo("Loading dynamic lights (%d to go) . . .", lightcnt);
			lightcnt=0;

			LOGGER.increaseFormatLevel();

			s32 lcnt = 0;
			for (s32 l = 0; l < lightcnt; l++)
			{
				scn::IBSPEntity &q3ent = 
					((scn::IBSPLevelMesh*)msh)->getEntity(scn::EBSPET_LIGHT, l);  

				core::vector3df pos = q3ent.getKeyValueAsVector("origin");
				f32 radius          = q3ent.getKeyValueAsFloat("radius");

				scn::ILightSceneNode *light =
					SCENE_MANAGER.addLightSceneNode();

				if (light)
				{
					c8 ch[255];
					sprintf(ch, "light.bsp.%p", light);	

					SGameNodeCommonParams light_params;

					light_params.Game.Type = EGNT_LIGHT;
					light_params.Scene.Parameters.ID = -1;
					light_params.Scene.Parameters.Name = ch;
					light_params.Scene.Transformation.Position.set(pos.X, pos.Z, pos.Y);
					light_params.Scene.Material.Material.getPass(0).setDiffuseColor(
						img::SColor (255,255,255,255) );
					light_params.Scene.Material.Material.getPass(0).setAmbientColor(
						img::SColor (255,155,155,155) );

					IGameNode* gnode_light = 
						GAME_MANAGER.addGameNode(light_params, light);  

					light->setParent(scene_node);

					child_scene_nodes.push_back(light);					
					
					vid::SLight ligparam = light->getLightParameters();
					ligparam.Radius = radius;
					light->setLightParameters(ligparam);

					if (gnode_light)
					{
						gnode_light->setAutoCreatedFlag(true);
					}
					else
					{
						light->remove();
						light = 0;
					}
				}

				if (light)
					lcnt++;
			}					

			LOGGER.logInfo("%d lights loaded", lcnt);

			LOGGER.decreaseFormatLevel();						

			s32 infnullcnt = ((scn::IBSPLevelMesh*)msh)->getEntitiesCount(scn::EBSPET_INFO_NULL);

			s32 cnt_to_load = 0;

			for (s32 n=0; n<infnullcnt; n++)
			{
				scn::IBSPEntity &q3ent = 
					((scn::IBSPLevelMesh*)msh)->getEntity(scn::EBSPET_INFO_NULL, n);  

				core::vector3df origin   = q3ent.getKeyValueAsVector("origin");
				core::stringc node_fname = q3ent.getKeyValue("snode");

				if (node_fname.size()>0)
				{
					decorations_to_load.push_back(
						SLevelMapToLoadEntry(
							core::vector3df(origin.X, origin.Z, origin.Y), 
						node_fname.c_str()));
					cnt_to_load++;
				}
			}	

			LOGGER.logInfo("Finded %d nodes to load later", cnt_to_load);

			// initializing team/target keys	

			LOGGER.logInfo("Initializing team/target keys for activatable nodes (%d to go) . . .",
				act_nodes.size());

			LOGGER.increaseFormatLevel();

			for (u32 an=0; an<act_nodes.size(); an++)
			{
				IGameNodeActivatable* gnode_act = act_nodes[an];
								
				core::stringc team_key = gnode_act->getTeamNameKey();
				core::stringc target_key = gnode_act->getTargetKey();

				if (team_key.size()>0 || target_key.size()>0)
				{
					for (u32 lpa=0; lpa<act_nodes.size(); lpa++)
					{
						if (an==lpa)
							continue;

						IGameNodeActivatable* gnode_act_another = act_nodes[lpa]; 

						core::stringc team_key_another = gnode_act_another->getTeamNameKey();

						if ( team_key.size()>0 && team_key!=NONAME && team_key_another==team_key)
						{
							gnode_act_another->addTeamNode(gnode_act);									
							LOGGER.logInfo(
								"Node %d '%s' connected to node %d '%s' via '%s' team key", 
								an,
								GameNodeTypeStr[gnode_act->getGameNodeType()],
								lpa,
								GameNodeTypeStr[gnode_act_another->getGameNodeType()],
								team_key.c_str());
						}

						core::stringc targetname_key_another = gnode_act_another->getTargetNameKey();

						if (target_key.size() > 0 &&
							target_key != NONAME &&
							targetname_key_another == target_key)
						{				
							gnode_act->addTargetNode(gnode_act_another);

							IGameNodeButton *gn_act_btn =
								SAFE_CAST_TO_GAME_NODE_BUTTON(gnode_act);
											
							if (gn_act_btn)
							{
								gnode_act_another->setActivateMode(EOAM_BY_BUTTON);

								// same as button wait time
								gnode_act_another->setWaitTimeSec(gnode_act->getWaitTimeSec());
							}
							LOGGER.logInfo(
								"Node %d '%s' connected to node %d '%s' via '%s' target key", 
								an,
								GameNodeTypeStr[gnode_act->getGameNodeType()],
								lpa,
								GameNodeTypeStr[gnode_act_another->getGameNodeType()],
								target_key.c_str());
						}
					}
				}
			}
			
			LOGGER.decreaseFormatLevel();

			core::stringc mesh_full_fname =	mesh_snode->getFileName();

			core::matrix4 abs     = mesh_snode->getAbsoluteTransformation();
			core::matrix4 invabs  = abs;
			invabs.makeInversed();

			core::vector3df main_player_start(0,0,0);

			if (((scn::IBSPLevelMesh*)msh)->getEntitiesCount(scn::EBSPET_INFO_PLAYER_START)>0)
			{
				scn::IBSPEntity &q3ent = 
					((scn::IBSPLevelMesh*)msh)->getEntity(scn::EBSPET_INFO_PLAYER_START, 0);
				main_player_start = q3ent.getKeyValueAsVector("origin");
			}

			core::aabbox3df bbox = mesh_snode->getBoundingBox();
			core::aabbox3df tbbox = mesh_snode->getTransformedBoundingBox();

			bbox.MinEdge.Y -= 5;
			bbox.MaxEdge.Y += 5;

			tbbox.MinEdge.Y -= 5;
			tbbox.MaxEdge.Y += 5;

			if (mesh_full_fname.size()>0)
			{
				LOGGER.log("Initializing pathfinder . . .");

				LOGGER.increaseFormatLevel();

				core::stringc mesh_fname = core::extractFileName(mesh_full_fname);  
				core::stringc mesh_fpath = core::extractFilePath(mesh_full_fname);

				mesh_fname.append("_walkmap.bmp");

				core::stringc walkmap_full_fname = mesh_fpath;
				walkmap_full_fname.append(mesh_fname);	

				core::stringc walkmap_full_fname_temp = 
					RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEMP_DATA );							
				walkmap_full_fname_temp.append(mesh_fname);	

				img::IImage *walk_map = 0;

				if (FILE_SYSTEM.existFile(walkmap_full_fname.c_str()))
				{
					walk_map = IMAGE_LIBRARY.getImage(
						walkmap_full_fname.c_str());
				}
				
				if (!walk_map)
				{
					// try to find in temp directory											

					if (FILE_SYSTEM.existFile(walkmap_full_fname_temp.c_str()))
					{
						walk_map = IMAGE_LIBRARY.getImage(
							walkmap_full_fname_temp.c_str());
					}
				}
				
				bool res = false;

				bool creating_map = false;

				if (!walk_map)
				{
					creating_map = true;
					
					dyn::IDynamicObject  *dobj = mesh_snode->getDynamicObject();
					dyn::IDynamicManager &dmgr = DYNAMIC_MANAGER;

					if (dobj)
					{
						f32 xdiap = bbox.getExtend().X;
						f32 ydiap = bbox.getExtend().Y;
						f32 zdiap = bbox.getExtend().Z;

						s32 map_size_x = xdiap/32.0f;
						s32 map_size_y = zdiap/32.0f;

						LOGGER.logInfo(
							"Generating walkability map (%d x %d, from %.2f,%.2f) . . .", 
							map_size_x, map_size_y,
							main_player_start.X, main_player_start.Y); 

						walk_map = IMAGE_LIBRARY.createEmptyImage(
							core::dimension2di(map_size_x, map_size_y),
							img::ECF_R8G8B8);

						img::IImage *height_map = IMAGE_LIBRARY.createEmptyImage(
							core::dimension2di(map_size_x, map_size_y),
							img::ECF_R8G8B8);

						// creating map here

						if (walk_map && height_map)
						{				
							walk_map->fill(empty_color);	
							height_map->fill(empty_color);

							f32 xstep = (f32)xdiap/(f32)map_size_x;
							f32 zstep = (f32)zdiap/(f32)map_size_y;

							s32 xorig = core::math::Abs(main_player_start.X-bbox.MinEdge.X) / xstep;
							s32 yorig = map_size_y -
								core::math::Abs(main_player_start.Y-bbox.MinEdge.Z) / zstep;

							if (xorig<0 || xorig>=map_size_x || yorig<0 || yorig>=map_size_y)
							{
								xorig = yorig = -1;
							}									
								
							LOGGER.logInfo(" Calculation started from %d,%d", xorig, yorig); 

							f32 c = 255;
							f32 pos_y = tbbox.MaxEdge.Y;

							for (s32 x=0; x<map_size_x; x++)
							{
								for (s32 y=0; y<map_size_y; y++)
								{
									core::vector3df pos(
										bbox.MinEdge.X + x*xstep + 0.05f,
										0,
										bbox.MinEdge.Z + y*zstep + 0.05f);
									invabs.transformVect(pos);											

									core::line3df ray;
									ray.start = core::vector3df(pos.X, tbbox.MaxEdge.Y+10, pos.Z+zstep);
									ray.end   = core::vector3df(pos.X, tbbox.MinEdge.Y-10, pos.Z+zstep);

									s32 map_x = x;
									s32 map_y = map_size_y-y-1;

									dyn::SCollisionPoints collisions;

									if (dmgr.findCollisionsWithRay(dobj, ray, collisions))
									{
										s32 idx = collisions.getNearestTo(ray.start);
										const dyn::SCollisionPoint &collision = collisions.getPoints()[idx];
									
										pos_y = collision.Pos.Y;

										f32 dy = pos_y-tbbox.MinEdge.Y;
										if (dy >= 0 && dy <= ydiap)
											c = (dy/(ydiap-1.0f)) * 255.0f;
									}

									height_map->setPixel( map_x, map_y, img::SColor(0, 0, c+0.5f, 0) );
								}								
							}									

							// creating walkability map	

							fillWalkabilityMap( walk_map, height_map, xorig, yorig, bbox );		

							// saving map 

							core::stringc msg = " Saving walkability map to ";
							msg.append(walkmap_full_fname_temp);

							LOGGER.log(msg.c_str());

							IMAGE_LIBRARY.saveImageToFile(
								walk_map, walkmap_full_fname_temp.c_str());	
						}

						SAFE_DROP(height_map);
					}
					else
					{
						LOGGER.logErr("There is no dynamic object, attached to this node!");
					}
				}

				if (walk_map)
				{
					scn::IPathFinder *pfnd = PATH_FINDER_MANAGER.addAStarPathFinder(
						mesh_snode, walk_map);                          
					res = pfnd ? true : false;

					if (creating_map)
					{							
						walk_map->drop();
					}
					else
					{
						IMAGE_LIBRARY.removeImage(walk_map);
					}
				}

				LOGGER.decreaseFormatLevel();

				if (res)
				{
					LOGGER.logInfo("Pathfinder initialized successfully");
				}
				else
				{
					LOGGER.logErr("Can't initialize pathfinder!");
				}				
			}	

			((scn::IBSPLevelMesh*)msh)->freeUnusedData();			

			if (decorations_to_load.size()>0)
			{
				LOGGER.log("Loading nodes . . .");

				LOGGER.increaseFormatLevel();

				s32 ncnt = 0;

				for (u32 n=0; n<decorations_to_load.size(); n++)
				{
					if (decorations_to_load[n].XMLFileName.size()>0)
					{
						core::stringc full_name =
							RESOURCE_MANAGER.getMediaDirFull( res::EMT_XML_SCRIPTS );

						full_name.append(decorations_to_load[n].XMLFileName.c_str());

						scn::ISceneNode *snode = smgr.loadSceneNode(full_name.c_str());

						if (snode)
						{						
							snode->setParent(mesh_snode);	   					
							snode->setPosition(decorations_to_load[n].Origin);
							child_scene_nodes.push_back(snode);
							ncnt++;
						}
					}
				}	

				mesh_snode->setPosition(level_pos);
				mesh_snode->setRotation(level_rot);
				mesh_snode->updateAbsoluteTransformation();	  				

				LOGGER.decreaseFormatLevel();						

				LOGGER.logInfo(" %d nodes loaded", ncnt);
			}

			for (u32 i=0; i<child_scene_nodes.size(); i++)
			{
				child_scene_nodes[i]->setPosition(child_scene_nodes[i]->getPosition());
			}
		}
	}
}

//----------------------------------------------------------------------------

//!
void CGameNodeLevelMap::connectLevelParts()
{
	core::stringc msg;

	LOGGER.log("Connecting level parts . . .");	

	u32 lp=0;

	for (lp=0; lp<m_LevelParts.size(); lp++)
	{					
		core::stringc &team_key   = m_LevelParts[lp].TeamKey;
		core::stringc &target_key = m_LevelParts[lp].TargetKey;

		if (team_key.size()>0 || target_key.size()>0)
		{
			for (u32 lpa=0; lpa<m_LevelParts.size(); lpa++)
			{
				if (lp==lpa)
					continue;

				core::stringc team_key_another = m_LevelParts[lpa].TeamKey;

				if ( team_key.size()>0 && team_key!=NONAME && team_key_another==team_key)
				{
					m_LevelParts[lpa].TeamIdxs.push_back(lp);
					m_LevelParts[lpa].TranspDist=MIN_TRANSP_DIST;

					msg.sprintf(
						" Node %d connected to node %d via '%s' team key", 
						lp,	lpa, team_key.c_str()
						);
					//LOGGER.log(msg);
				}

				core::stringc targetname_key_another = m_LevelParts[lpa].TargetNameKey;

				if (target_key.size()>0 && target_key!=NONAME && targetname_key_another==target_key)
				{					
					m_LevelParts[lpa].TargetedByIdx=lp;
					m_LevelParts[lpa].TranspDist=MIN_TRANSP_DIST;
				
					msg.sprintf(
						" Node %d connected to node %d  via '%s' target key", 
						lp,	lpa, target_key.c_str()
						);
					//LOGGER.log(msg);
				}
			}
		}
	}

	for (lp=0; lp<m_LevelParts.size(); lp++)
	{
		SLevelPart &lpart = m_LevelParts[lp];
		scn::ISceneNode* snode = lpart.SceneNode;

		if (!snode)
			continue;

		snode->setParent(m_SceneNode);

		s32 mcnt = snode->getMaterialsCount();
		for (s32 m = 0; m < mcnt; m++)
		{
			vid::SMaterial &mat = snode->getMaterial(m);

			s32 pcnt = mat.getPassesCount();
			for (s32 p=0; p < pcnt; p++)
			{
				vid::SRenderPass &pass = mat.getPass(p);
				if (pass.getAlphaTest() != vid::EAT_ALWAYS)
				{
					lpart.BlendingAlwaysRequired = true;
					m = mcnt;
					break;
				}
			}
		}
		for (s32 m = 0; m < mcnt; m++)
		{
			vid::SMaterial &mat = snode->getMaterial(m);

			s32 pcnt = mat.getPassesCount();
			for (s32 p=0; p<pcnt; p++)
			{
				vid::SRenderPass &pass = mat.getPass(p);
				pass.setBlendFuncs(
					vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);					
				pass.setLightingMode(vid::ELM_NONE);
				pass.setFlag(vid::EMF_ZWRITE_ENABLE, true);
				pass.setFlag(vid::EMF_BLENDING, lpart.BlendingAlwaysRequired);
				pass.setAlphaTest(vid::EAT_ALWAYS);
				pass.setAlphaGen(vid::EAGT_CUSTOM);
				pass.setSpecularColor(0x00000000);
				pass.setShininess(0.0f);
			}
		}

		if (m_LevelParts[lp].TeamIdxs.size() == 0)
			m_LevelParts[lp].Occluder = false;
	}

	msg.sprintf(" %d level parts connected.", m_LevelParts.size());
	LOGGER.log(msg);
}

//----------------------------------------------------------------------------

//! LevelMap registry funcs

//----------------------------------------------------------------------------

//! LevelMap registry funcs
IGameNode* Alloc__StandardLevelMap(scn::ISceneNode* n, SGameNodeParams &params)
{
	return new CGameNodeLevelMap(n,params);
}

//----------------------------------------------------------------------------

CGameNodeBaseCreator StandardLevelMapCreator;

IGameNode* Create_StandardLevelMap(SGameNodeCommonParams &node_params, bool log_info)
{
	return StandardLevelMapCreator.createGameNode(node_params, log_info);
}

//----------------------------------------------------------------------------

CGameNodeBaseCloner StandardLevelMapCloner;

IGameNode* Clone__StandardLevelMap(IGameNode *game_node, bool log_info)
{
	return StandardLevelMapCloner.cloneGameNode(game_node, log_info);
}

//----------------------------------------------------------------------------

struct SCalcEntry
{
	s32 x, y, last_x, last_y;

	SCalcEntry()
		: x(-1), y(-1), last_x(-1), last_y(-1) {}

	SCalcEntry(s32 _x, s32 _y, s32 _last_x, s32 _last_y)
		: x(_x), y(_y), last_x(_last_x), last_y(_last_y) {}
};

//----------------------------------------------------------------------------

bool registerToCalc(
	img::IImage *map, s32 x, s32 y, s32 last_x, s32 last_y, s32 map_size_x, s32 map_size_y,
	core::array<SCalcEntry> &reg_arr
	)
{
	bool res = false;

	if (
		!map || 
		x<0 || x>=map_size_x || y<0 || y>=map_size_y ||
		last_x<0 || last_x>=map_size_x || last_y<0 || last_y>=map_size_y
		)
		return res;

	img::SColor xy_col = map->getPixel(x, y);
	img::SColor last_xy_col = map->getPixel(last_x, last_y);

	if (xy_col == walk_color || last_xy_col == unwalk_color)
		return res;

	res = true;

	reg_arr.push_back(SCalcEntry(x, y, last_x, last_y));

	return res;
}

//----------------------------------------------------------------------------

bool fillWalkabilityMap(
	img::IImage *map, core::vector3df** positions, 
	s32 x, s32 y, s32 last_x, s32 last_y, s32 map_size_x, s32 map_size_y,
	core::array<SCalcEntry> &reg_arr
	)
{
	bool res = false;

	if (
		!map ||!positions || 
		x<0 || x>=map_size_x || y<0 || y>=map_size_y ||
		last_x<0 || last_x>=map_size_x || last_y<0 || last_y>=map_size_y
		)
		return res;

	img::SColor xy_col = map->getPixel(x, y);
	img::SColor last_xy_col = map->getPixel(last_x, last_y);

	if (xy_col == walk_color || last_xy_col == unwalk_color)
		return res;

	res = true;

	core::vector3df dh = positions[x][y] - positions[last_x][last_y];

	if (core::math::Abs(dh.Y)<WALK_DELTA_H_LIMIT)
	{
		map->setPixel(x, y, walk_color);
	}
	else
	{
		map->setPixel(x, y, unwalk_color);
	}

	s32 x1 = x-1;
	s32 y1 = y;		
	registerToCalc(	map, x1, y1, x, y, map_size_x, map_size_y, reg_arr );

	s32 x2 = x;
	s32 y2 = y-1;
	registerToCalc(	map, x2, y2, x, y, map_size_x, map_size_y, reg_arr );
		
	s32 x3 = x+1;
	s32 y3 = y;
	registerToCalc(	map, x3, y3, x, y, map_size_x, map_size_y, reg_arr );		

	s32 x4 = x;
	s32 y4 = y+1;
	registerToCalc( map, x4, y4, x, y, map_size_x, map_size_y, reg_arr );

	return res;
}

//----------------------------------------------------------------------------

//! create walk bounds along all walls
void postProcessWalkabilityMap(
	img::IImage *walk_map_in, img::IImage *walk_map_out, core::vector3df** positions,
	s32 map_size_x, s32 map_size_y
	)
{
	if (!walk_map_in || !walk_map_out || !positions || 
		walk_map_in->getDimension()!=walk_map_out->getDimension()
		)
	{
		LOGGER.log("Can't post process walkability map!", io::ELL_ERROR);
		return;
	}

	s32 map_size_x_1 = map_size_x-1;
	s32 map_size_y_1 = map_size_y-1;

	for (s32 x=1; x<map_size_x_1; x++)
	{
		for (s32 y=1; y<map_size_y_1; y++)
		{	
			img::SColor c  = walk_map_in->getPixel(x, y);
			img::SColor c1 = walk_map_in->getPixel(x-1, y);
			img::SColor c2 = walk_map_in->getPixel(x, y-1);
			img::SColor c3 = walk_map_in->getPixel(x+1, y);
			img::SColor c4 = walk_map_in->getPixel(x, y+1);

			f32 h  = positions[x][y].Y;
			f32 h1 = positions[x-1][y].Y;
			f32 h2 = positions[x][y-1].Y;
			f32 h3 = positions[x+1][y].Y;
			f32 h4 = positions[x][y+1].Y;

			f32 dh1 = h-h1;
			f32 dh2 = h-h2;
			f32 dh3 = h-h3;
			f32 dh4 = h-h4;

			if (core::math::Abs(dh1)>WALK_DELTA_H_LIMIT || core::math::Abs(dh2)>WALK_DELTA_H_LIMIT || 
				core::math::Abs(dh3)>WALK_DELTA_H_LIMIT || core::math::Abs(dh4)>WALK_DELTA_H_LIMIT ||
				c1==unwalk_color || c2==unwalk_color || 
				c3==unwalk_color || c4==unwalk_color
				)
			{
				walk_map_out->setPixel(x,y,unwalk_color);
			}
		}
	}
}

//----------------------------------------------------------------------------

void fillWalkabilityMap(
	img::IImage *walk_map_out, img::IImage *height_map, s32 xorig, s32 yorig, core::aabbox3df &bbox
	)
{
	if (!walk_map_out || !height_map || xorig==-1 || yorig==-1 ||
		walk_map_out->getDimension()!=height_map->getDimension()
		)
		return;

	s32 map_size_x = walk_map_out->getDimension().Width;
	s32 map_size_y = walk_map_out->getDimension().Height;

	f32 xdiap = bbox.getExtend().X;
	f32 ydiap = bbox.getExtend().Y;
	f32 zdiap = bbox.getExtend().Z;						

	f32 xstep = (f32)xdiap/(f32)map_size_x;
	f32 zstep = (f32)zdiap/(f32)map_size_y;

	core::vector3df** positions = new core::vector3df*[map_size_x];

	for (s32 pi=0; pi<map_size_x; pi++)
	{
		positions[pi] = new core::vector3df[map_size_y];
	}

	for (s32 x=0; x<map_size_x; x++)
	{
		for (s32 y=0; y<map_size_y; y++)
		{
			s32 map_x = x;
			s32 map_y = map_size_y-y-1;

			f32 g = height_map->getPixel(map_x, map_y).getGreen();							

			positions[x][map_size_y-1-y] = core::vector3df(
				bbox.MinEdge.X + x*xstep,
				ydiap*g/255.0f,
				bbox.MinEdge.Z + y*zstep
				);						
		}								
	}	

	core::array<SCalcEntry> reg_array;

	reg_array.push_back(SCalcEntry(xorig, yorig, xorig, yorig));

	while (reg_array.size()>0)
	{
		core::array<SCalcEntry> tmp_reg_array;

		for (u32 r=0; r<reg_array.size(); r++)
		{
			fillWalkabilityMap(
				walk_map_out, positions, 
				reg_array[r].x, reg_array[r].y, 
				reg_array[r].last_x, reg_array[r].last_y, 
				map_size_x, map_size_y,
				tmp_reg_array
				);	
		}								

		reg_array = tmp_reg_array;
	}

	// fill other pixels with unwalk color

	for (s32 xxx=0; xxx<map_size_x; xxx++)
	{
		for (s32 yyy=0; yyy<map_size_y; yyy++)
		{
			if (walk_map_out->getPixel(xxx, yyy)!=walk_color)
			{
				walk_map_out->setPixel(xxx, yyy, unwalk_color);
			}
		}
	}

	// post process walkability map

	img::IImage *tmp_map = IMAGE_LIBRARY.createEmptyImage(
		core::dimension2di(map_size_x, map_size_y), img::ECF_R8G8B8
		);

	walk_map_out->copyTo(
		tmp_map, core::position2d<s32>(0,0), core::rect<s32>(0,0,map_size_x-1, map_size_y-1)
		);	

	postProcessWalkabilityMap(walk_map_out, tmp_map, positions, map_size_x, map_size_y);

	tmp_map->copyTo(
		walk_map_out, core::position2d<s32>(0,0), core::rect<s32>(0,0,map_size_x-1, map_size_y-1)
		);		

	walk_map_out->drawBorder(0, 0, map_size_x-1, map_size_y-1, unwalk_color);
	//walk_map_out->floodFill(xorig, yorig, walk_color, img::SColor(255,0,255,255));

	// adding walk and height map 

	walk_map_out->blendWith(height_map, img::EIO_ADD);	

	// free allocated memory

	SAFE_DROP(tmp_map);

	for (s32 pi=0; pi<map_size_x; pi++)
	{
		SAFE_DELETE_ARRAY(positions[pi]);
	}
	SAFE_DELETE_ARRAY(positions);	
}
   
//----------------------------------------------------------------------------

class CStandardLevelMapLoader : public CGameNodeBaseLoader
{
public:

	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file)
	{
		_preLoadGameNode(node_params, file);    
		_loadGameNode(node_params, false);    
		_postLoadGameNode(node_params);	

		IGameNodeLevelMap *level_game_node = SAFE_CAST_TO_GAME_NODE_LEVEL_MAP(GameNode);
		if (level_game_node)
		{
			level_game_node->initializeLevelParts();
		}

		return GameNode;
	}
}
StandardLevelMapLoader;

IGameNode* Load___StandardLevelMap(SGameNodeCommonParams &node_params, io::IReadFile* file)
{
	return StandardLevelMapLoader.loadGameNode(node_params, file);
}
 
//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
