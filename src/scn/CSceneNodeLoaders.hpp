//|-------------------------------------------------------------------------
//| File:        CSceneNodeLoaders.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <scn/ISceneNodeLoader.h>
#include <scn/SAnimatedMesh.h>
#include <scn/SMesh.h>
#include <img/IImageLibrary.h>
#include <dev/IDevice.h>
#include <vid/IFont.h>

#include "CAnimatorsLoader.hpp"

#include "nodes/CParticleAffectorFadeOut.h"
#include "nodes/CParticleAffectorGravity.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

#define PRE_LOAD_SCENE_NODE_MACRO(scene_node_type)                            \
LOGGER.logInfo("Loading %s Scene Node from xml-file %s . . .",                \
	SceneNodeTypeShortStr[scene_node_type], xml_fname.c_str());               \
LOGGER.increaseFormatLevel();

//----------------------------------------------------------------------------

#define POST_LOAD_SCENE_NODE_MACRO(scene_node_type)                           \
LOGGER.decreaseFormatLevel(); 	                                              \
if (!node) {                                                                  \
	LOGGER.logInfo("Error loading %s Scene Node from %s",                     \
		SceneNodeTypeShortStr[scene_node_type], xml_fname.c_str());           \
} else {                                                                      \
	LOGGER.logInfo("%s Scene Node successfully loaded from %s",               \
		SceneNodeTypeShortStr[scene_node_type], xml_fname.c_str());           \
}

//----------------------------------------------------------------------------

struct SChildEntry
{
	core::stringc FileName;
	core::vector3df Position, Rotation, Scale;

	SChildEntry()
		: FileName(""), Position(0,0,0), Rotation(0,0,0), Scale(1,1,10) {}
	SChildEntry(
		core::stringc filename, 
		core::vector3df pos, core::vector3df rot, core::vector3df scl
		) : FileName(filename), Position(pos), Rotation(rot), Scale(scl) {}
};

//----------------------------------------------------------------------------

struct SCaptionEntry
{
	core::stringc FontFileName;
	core::stringc Text;
	s32 FontSize;
	img::SColor Color;
	core::vector3df Position;
	bool Visible;

	SCaptionEntry() : FontFileName(""),
		Position(0,0,0), Text("Caption"), FontSize(16), 
		Color(100,255,255,255), Visible(false) {}
	SCaptionEntry(
		const c8* font_filename,
		const wchar_t* text,
		s32 font_size,
		const img::SColor& color,
		const core::vector3df& pos,
		bool visible)
		: FontFileName(font_filename),
	Position(pos), Text(text), FontSize(font_size), 
	Color(color), Visible(visible) {}
};

//----------------------------------------------------------------------------

class CSceneNodeLoader : public ISceneNodeLoader
{
public:

	void bindChilds(ISceneNode* node, core::array<SChildEntry> &childs)
	{
		LOGGER.increaseFormatLevel();

		for (u32 ch=0; ch<childs.size(); ch++)
		{
			scn::ISceneNode* child_node = 
				SCENE_MANAGER.loadSceneNode(childs[ch].FileName.c_str());
					
			if (child_node) 
			{
				child_node->setParent(node,true);
				child_node->setPosition(childs[ch].Position);
				child_node->setRotation(childs[ch].Rotation);
				child_node->setScale   (childs[ch].Scale   );
				child_node->updateAbsoluteTransformation();
			}
		}

		LOGGER.decreaseFormatLevel();		
	}

	void bindCaption(ISceneNode* node, SCaptionEntry &caption)
	{
		LOGGER.increaseFormatLevel();

		if (caption.FontFileName!="")
		{
			core::stringc 
				font_full_fname = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_FONTS );
			font_full_fname.append(caption.FontFileName);

			vid::IFont* font = 
				VIDEO_DRIVER.getFont(font_full_fname.c_str());   
			if (font)
				font->setSize(caption.FontSize);

			ITextSceneNode* caption_node = node->attachCaption(
				font, caption.Text.c_str(), caption.Color);
					
			if (caption_node) 
			{
				caption_node->setParent(node);
				caption_node->setPosition(caption.Position);
				caption_node->updateAbsoluteTransformation();
				caption_node->setVisible(caption.Visible);
			}
		}

		LOGGER.decreaseFormatLevel();		
	}
};

//----------------------------------------------------------------------------

bool parseAddAnimatorTag    (io::IXMLReader* xml, core::stringc        &AnimatorFileName );
bool parseBindChildTag      (io::IXMLReader* xml, SChildEntry          &child            );
bool parseBindCaptionTag    (io::IXMLReader* xml, SCaptionEntry        &caption          );
bool parseMaterialTag       (io::IXMLReader* xml, SSceneNodeMaterial   &material         );
bool parseParametersTag     (io::IXMLReader* xml, SSceneNodeParameters &parameters       );
bool parseTransformationTag (io::IXMLReader* xml, SSceneNodeTransformation &transf       );

//----------------------------------------------------------------------------

class CAnimatedMeshSceneNodeLoader : public CSceneNodeLoader
{
	struct SAnimEntry
	{
		SAnimEntry() : filename(""), start(-1), end(-1), speed(150.0f), looped(true) {}

		core::stringc filename;
		s32 start, end;
		f32 speed;
		bool looped;
	};

	struct SMeshEntry
	{
		core::stringc FileName;
		s32 AnimationFPS;
		E_LOD_LEVEL LodLevel;

		SMeshEntry() : LodLevel(ELL_LOD_NEAREST), FileName(""),  AnimationFPS(0) {}

			SMeshEntry(core::stringc filename, s32 ani_fps, E_LOD_LEVEL lod)
			: LodLevel(lod), FileName(filename), AnimationFPS(ani_fps) {}
	};

public:

	CAnimatedMeshSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CAnimatedMeshSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::IAnimatedMeshSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_ANIMATED_MESH_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::stringc basePath = core::extractFilePath(xml_fname);
    
		core::array<SMeshEntry> meshes;

		f32 anim_speed=1;
		bool anim_smooth = false;

		core::stringc fNameAnimCurr="";
		core::array<core::stringc> fNameAnim;
		core::stringc tmpStr;

		core::array<SAnimEntry> SpecialAnimation;

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

		core::vector3df imposter_shift(0,0,0);
		f32 imposter_view_offset = 0;
		bool imposter_centered = false;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		core::rectf cursor_lock_rect;

		SImposterParameters imposter_params;
		SImposterCreationParameters imp_creation_params;

		PRE_LOAD_SCENE_NODE_MACRO(sntype);		

		scn::ISceneManager &smgr   = SCENE_MANAGER;
		vid::IVideoDriver  &driver = VIDEO_DRIVER;

		const c8 *meshes_path = RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES);
		const c8 *xml_path = RESOURCE_MANAGER.getMediaDirFull(res::EMT_XML_SCRIPTS);

		bool with_imposter = false;

		bool scene_tag_finished = false;

		bool shadow_on = false;
		E_SHADOW_TYPE shadow_type = ESHT_SHADOW_FAKE;

		while(xml->read() && !scene_tag_finished)
		{   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {   
				if (core::stringw("Mesh") == xml->getName())
		        {
					SMeshEntry mesh_entry;

					mesh_entry.FileName = meshes_path;
				    mesh_entry.FileName.append(xml->getAttributeValue(L"filename"));

					mesh_entry.AnimationFPS    = xml->getAttributeValueAsInt  (L"animation_fps"   );

					mesh_entry.LodLevel = (E_LOD_LEVEL)xml->getAttributeValueAsInt  (L"lod" );	
					
					meshes.push_back(mesh_entry);
				}
				else 
				if (core::stringw("Animation") == xml->getName())
	            {
			        anim_speed  = xml->getAttributeValueAsFloat(L"speed" );	
					anim_smooth = xml->getAttributeValueAsBool(L"smooth" );	
				}
				else
				if (core::stringw("Shadow") == xml->getName())
				{
					shadow_on = xml->getAttributeValueAsBool(L"on");
					shadow_on &= DEVICE.getDeviceFlagValue(dev::EDCF_SHADOWS);

					core::stringc shadow_type_str = xml->getAttributeValue(L"type");
					
					for (s32 sht=0; sht<E_SHADOW_TYPE_COUNT; sht++)
					{
						if (shadow_type_str == ShadowTypeStr[sht])
						{
							shadow_type = (E_SHADOW_TYPE)sht;
							break;
						}
					}				
				}	
				else
				if (core::stringw("BindImposter") == xml->getName())
				{
					imposter_params.ShiftCustom.X = xml->getAttributeValueAsFloat(L"shift_x");
					imposter_params.ShiftCustom.Y = xml->getAttributeValueAsFloat(L"shift_y");
					imposter_params.ShiftCustom.Z = xml->getAttributeValueAsFloat(L"shift_z");

					imposter_params.ViewOffset = xml->getAttributeValueAsFloat(L"view_offset");

					imposter_params.Centered = xml->getAttributeValueAsInt(L"centered")==0?false:true;

					with_imposter = true;

					s32 xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType() == io::EXN_ELEMENT)
						{
							if (core::stringw("CreationParams") == xml->getName())
							{	
								imp_creation_params.ViewsCount = xml->getAttributeValueAsInt(L"view_count", 8);				
								
								imp_creation_params.OptimalViewAngle  = xml->getAttributeValueAsFloat(L"view_angle", 25.0f);

								imp_creation_params.CameraViewScale.set(
									xml->getAttributeValueAsFloat(L"view_scale_width", 1.0f),
									xml->getAttributeValueAsFloat(L"view_scale_height", 1.0f)
									);
								imp_creation_params.SizeScale.set(
									xml->getAttributeValueAsFloat(L"size_scale_width", 1.0f),
									xml->getAttributeValueAsFloat(L"size_scale_height", 1.0f)
									);

								imp_creation_params.TextureResolution = xml->getAttributeValueAsFloat(L"tex_res", 64);
								imp_creation_params.AnimationFrameStep = xml->getAttributeValueAsFloat(L"frame_step", 2.0);
								imp_creation_params.Compress = xml->getAttributeValueAsBool(L"compress", true);

								core::vector3df amb_col  = xml->getAttributeValueAsVector(L"ambient_color", core::vector3df(150,150,150));
								core::vector3df diff_col = xml->getAttributeValueAsVector(L"diffuse_color", core::vector3df(255,255,255));
								core::vector3df spec_col = xml->getAttributeValueAsVector(L"specular_color", core::vector3df(255,255,255));

								imp_creation_params.AmbientColor.set(255, amb_col.X, amb_col.Y, amb_col.Z);
								imp_creation_params.DiffuseColor.set(255, diff_col.X, diff_col.Y, diff_col.Z);
								imp_creation_params.SpecularColor.set(255, spec_col.X, spec_col.Y, spec_col.Z);
							}
							else
							if (core::stringw("FileName") == xml->getName())
							{
								core::stringc full_file_name = xml_path;
								full_file_name.append(xml->getAttributeValue(L"value"));
								if (FILE_SYSTEM.existFile(full_file_name.c_str()))
								{
									imposter_params.setFileName(full_file_name.c_str());
								}
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
				else if (parseAddAnimatorTag(xml, fNameAnimCurr)) 
				{
					fNameAnim.push_back(fNameAnimCurr);	
				}
				else if (core::stringw("Register") == xml->getName()) 
				{
					SAnimEntry anim;

					anim.filename = xml->getAttributeValue(L"filename");
					anim.start    = xml->getAttributeValueAsInt(L"start_frame");
					anim.end      = xml->getAttributeValueAsInt(L"end_frame");
					anim.speed    = xml->getAttributeValueAsFloat(L"speed");
					anim.looped   = xml->getAttributeValueAsInt(L"loop")==0 ? false : true;

					SpecialAnimation.push_back(anim);	
				} 				
				else if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseTransformationTag(xml, node_transformation)) 
				{
					finded_transformation = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName()) {}
				else
				if (core::stringw("Game") == xml->getName() ||
						core::stringw("Sound") == xml->getName() ||
						core::stringw("Dynamic") == xml->getName())  
				{					
					scene_tag_finished = true;
				}
			}
        }
  
	    if (xml) xml->drop(); 

		for (u32 me=0; me<meshes.size()&&me<E_LOD_LEVEL_COUNT; me++)
		{
			scn::IAnimatedMesh *mesh = smgr.findMesh(meshes[me].FileName.c_str());

			if (!mesh)
			{
				mesh = smgr.getMesh( meshes[me].FileName.c_str(), scn::ELL_LOD_NEAREST );

				if (mesh)
				{
					mesh->setAnimationFPS(meshes[me].AnimationFPS);

					// registering animations

					mesh->startRegisteringAnimations();				

					for (u32 i=0; i<SpecialAnimation.size(); i++)
					{
						if (SpecialAnimation[i].filename.size()>0)
						{						
							core::stringc filename = meshes_path;
							filename.append(SpecialAnimation[i].filename.c_str());

							mesh->registerAnimation(filename.c_str(),
								SpecialAnimation[i].speed, SpecialAnimation[i].looped);
						}
						else
						{
							mesh->registerAnimation(
								SpecialAnimation[i].start, SpecialAnimation[i].end, 
								SpecialAnimation[i].speed, SpecialAnimation[i].looped);
						}
					}

					mesh->setAnimationOveralSmooth(anim_smooth);

					mesh->endRegisteringAnimations();					
				}
			}

			if (mesh)
			{
				if (!node)
					node = smgr.addAnimatedMeshSceneNode(); 
				node->setAnimatedMesh(mesh, (E_LOD_LEVEL)me); 				
			}
			else
			{
				LOGGER.logErr("Couldn't load mesh from %s", meshes[me].FileName.c_str());
			}
	    }

		if (node)  
		{
			node->setFileName(xml_fname.c_str());

			// animation params

			node->setAnimationOveralSpeed (anim_speed);
			node->setAnimationOveralSmooth(anim_smooth);							

			// set animation
			node->setCurrentAnimation(0);	
			
			IAnimatedMesh *amesh = NULL;

			E_LOD_LEVEL lowest_lod = (E_LOD_LEVEL)(E_LOD_LEVEL_COUNT-1);

			for (s32 am=(E_LOD_LEVEL_COUNT-1); am>=0; am--)
			{
				lowest_lod = (E_LOD_LEVEL)am;
				amesh = node->getAnimatedMesh(lowest_lod);
				if (amesh)
					break;
			}

			with_imposter = with_imposter && amesh;
	
			// adding shadow
			if (shadow_on)
				node->attachShadow(shadow_type);
        
			// adding animators
			for (int a=0; a<(f32)fNameAnim.size(); a++)
			{
				core::stringc anim_full_fname = basePath;
				anim_full_fname.append(fNameAnim[a]);

				scn::ISceneNodeAnimator* anim = CAnimatorsLoader::load(anim_full_fname);
						
				if (anim) 
				{
					node->addAnimator(anim);    
					anim->drop(); 
				}
			}		

			if (found_material)
				node->setParameters(node_material);
			if (finded_transformation)
				node->setParameters(node_transformation);				
			if (finded_parameters)
				node->setParameters(node_parameters);

			// attaching imposter
			if (with_imposter)
				node->attachImposter(imp_creation_params, imposter_params);

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

		POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CBillboardSceneNodeLoader : public CSceneNodeLoader
{
public:

	CBillboardSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CBillboardSceneNodeLoader");
	#endif
	}

	//! helper function
	void loadTextures(scn::IBillboardSceneNode * node,
		s32 anim_type,
		core::array < SAnimationFrame > & frames,
		s32 time_per_frame,	bool loop)
	{
		SAnimationFrameParameters params;
		params.TimePerFrame = time_per_frame; 
		params.Loop = loop;	
		
		u32 dir_idx = node->getAnimationDirectionSetsCount(anim_type);

		node->setAnimationDirectionSetFrames(anim_type, dir_idx, frames, params);

		frames.clear();
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::IBillboardSceneNode* node = 0;	

		E_SCENE_NODE_TYPE sntype = ESNT_BILLBOARD_SCENE_NODE;
		
		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		core::stringc basePath = core::extractFilePath(xml_fname);

		core::stringc fNameAnimCurr="";
		core::array < core::stringc > fNameAnim;
		core::stringc texFname="";	
		bool VerticalAligned = false;

		vid::IVideoDriver & driver = VIDEO_DRIVER;

		core::array < SChildEntry > childs;
		SChildEntry child;

		SCaptionEntry caption;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		scn::SBillboardParams billparams;
		
		PRE_LOAD_SCENE_NODE_MACRO(sntype);

		bool color_key_old =
			driver.getTextureCreationFlag(ETCF_CREATE_COLOR_KEY);
		
		driver.setTextureCreationFlag(ETCF_CREATE_COLOR_KEY, true);
		
		s32 xml_pos = xml->getPos();

	    while (xml->read() && !scene_tag_finished)
	    {
			if ( xml->getNodeType() == io::EXN_ELEMENT)
			{
				if (parseAddAnimatorTag(xml, fNameAnimCurr)) 
				{
					fNameAnim.push_back(fNameAnimCurr);	
				}
				else 
				if (core::stringw("UpAxis") == xml->getName())
				{
					billparams.UpAxis=xml->getAttributeValueAsVector(L"value");
				}
				else 
				if (core::stringw("Size") == xml->getName())
				{
					billparams.Size.Width = xml->getAttributeValueAsInt(L"width");
					billparams.Size.Height = xml->getAttributeValueAsInt(L"height");
				}
				else
				if (core::stringw("FrameResolution") == xml->getName())
				{
					billparams.FrameResolution.set(
						xml->getAttributeValueAsInt(L"width", 0),
						xml->getAttributeValueAsInt(L"height", 0));
				}
				else 
				if (core::stringw("Flip") == xml->getName())
				{
					billparams.FlipWidth = xml->getAttributeValueAsInt(L"width")==0?false:true;
					billparams.FlipHeight = xml->getAttributeValueAsInt(L"height")==0?false:true;
				}
				else 
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}	
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}			
		}

		xml->setPos(xml_pos);				
    
		node = SCENE_MANAGER.addBillboardSceneNode();

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			node->setSize(billparams.Size);

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			core::stringc tex_path = SCENE_MANAGER.getTexturesPath();

			s32 AnimationSetTagCount = 0, DirectionSetTagCount = 0;

			bool loop = true;
			s32 anim_type = 0;
			s32 time_per_frame = 25;
			core::array < SAnimationFrame > Frames;
			core::array < vid::ITexture * > Textures;

			bool need_to_add_texture_set = false;

			io::E_LOG_LEVEL loglevel = LOGGER.getLogLevel();

			bool dir_order_ccw=true;

			while(node && xml && xml->read())
			{
				if ( xml->getNodeType() == io::EXN_ELEMENT)
				{
					if (core::stringw("SpriteAnimations") == xml->getName())
					{						
						dir_order_ccw = xml->getAttributeValueAsInt(L"dir_order_ccw")==0 ? false : true;;
					}
					else if (core::stringw("AnimationSet") == xml->getName())
					{
						if (AnimationSetTagCount != 0 && DirectionSetTagCount != 0)
						{
							loadTextures(node, anim_type, Frames, time_per_frame, loop);						
							DirectionSetTagCount = 0;
						}

						AnimationSetTagCount++;

						anim_type = xml->getAttributeValueAsInt(L"index");
						loop = xml->getAttributeValueAsInt(L"loop")==0 ? false : true;
						time_per_frame = xml->getAttributeValueAsInt(L"time_per_frame");

					}
					else if (core::stringw("DirectionSet") == xml->getName())
					{
						if (AnimationSetTagCount != 0 && DirectionSetTagCount != 0)
							loadTextures(node, anim_type, Frames, time_per_frame, loop);	

						DirectionSetTagCount++;
					}
					else if (core::stringw("TextureFileName") == xml->getName())
					{						
						core::stringc fname = xml->getAttributeValue(L"value");
						core::stringc full_fname = tex_path;
						full_fname.append(fname);

						Textures.push_back(driver.getTexture(full_fname.c_str()));
					}
					else if (core::stringw("Frame") == xml->getName())
					{
						vid::ITexture * tex = 0;
						s32 tex_index = xml->getAttributeValueAsInt(L"tex_index");
						CHECK_RANGE(tex_index, 0, s32(Textures.size() - 1));

						if (Textures.size())
							tex = Textures[tex_index];

						core::recti tci(
							xml->getAttributeValueAsInt(L"tc_left", -1),
							xml->getAttributeValueAsInt(L"tc_top", -1),
							xml->getAttributeValueAsInt(L"tc_right", -1),
							xml->getAttributeValueAsInt(L"tc_bottom", -1));
						core::rectf tcf(0.0f, 0.0f, 1.0f, 1.0f);
						if (tex && tci.UpperLeftCorner.X >=0 && tci.UpperLeftCorner.Y >=0 &&
							tci.LowerRightCorner.X >=0 && tci.LowerRightCorner.Y >=0)
						{
							tcf.UpperLeftCorner.set(
								(f32)tci.UpperLeftCorner.X / (tex->getSize().Width),
								(f32)tci.UpperLeftCorner.Y / (tex->getSize().Height));
							tcf.LowerRightCorner.set(
								(f32)tci.LowerRightCorner.X / (tex->getSize().Width),
								(f32)tci.LowerRightCorner.Y / (tex->getSize().Height));
						}

						core::vector2di toffsi(
							xml->getAttributeValueAsInt(L"toffs_x", 0),
							xml->getAttributeValueAsInt(L"toffs_y", 0));
						core::vector2df toffsf(0.0f, 0.0f);
						if (tex && (toffsi.X != 0 || toffsi.Y != 0))
						{
							toffsf.set(
								(f32)toffsi.X / (tex->getSize().Width),
								(f32)toffsi.Y / (tex->getSize().Height));
						}

						Frames.push_back(SAnimationFrame(tex, tcf, toffsf));
					}
					else if (core::stringw("AxisAligment") == xml->getName())
					{
						core::stringc axis_align_type = xml->getAttributeValue(L"type");

						for (s32 i=0; i<E_AXIS_ALIGMENT_TYPE_COUNT; i++)
						{
							if (axis_align_type==scn::AxisAligmentTypeStr[i])
							{
								billparams.AxisAligmentType = (E_AXIS_ALIGMENT_TYPE)i;
								break;
							}
						}

						billparams.AlignAxis.set(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z"));
					}
				}			
			}

			node->setBillboardParameters(billparams);

			if (AnimationSetTagCount != 0 && DirectionSetTagCount != 0)
				loadTextures(node, anim_type, Frames, time_per_frame, loop);					

			LOGGER.setLogLevel(loglevel);

			for (s32 a=0; a<(s32)fNameAnim.size(); a++)
			{
				core::stringc anim_full_fname = basePath;
				anim_full_fname.append(fNameAnim[a]);

				scn::ISceneNodeAnimator* anim = CAnimatorsLoader::load(anim_full_fname);
					
				if (anim) 
				{
					node->addAnimator(anim);    
					anim->drop(); 
				}
			}

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

		driver.setTextureCreationFlag(ETCF_CREATE_COLOR_KEY, color_key_old);

		POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CLensFlareSceneNodeLoader : public CSceneNodeLoader
{
public:

	CLensFlareSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CLensFlareSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ILensFlareSceneNode* node = 0;	
		
		E_SCENE_NODE_TYPE sntype = ESNT_LENS_FLARE_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::stringc basePath =
			core::extractFilePath(xml_fname);

		vid::ITexture* Corona;
		core::array<scn::SLens> Lens;

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;
    
		core::dimension2d<s32> etalonScreen;    
    
		s32 parent_id =-1;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;
    
		PRE_LOAD_SCENE_NODE_MACRO(sntype);

		while(xml->read()&&!scene_tag_finished)
	    {
			if ( xml->getNodeType() == io::EXN_ELEMENT)
			{
				if (core::stringw("Corona") == xml->getName())
				{
					core::stringc fNameCorona = SCENE_MANAGER.getTexturesPath();
					fNameCorona.append(xml->getAttributeValue(L"filename")); 

					Corona = VIDEO_DRIVER.getTexture(fNameCorona.c_str());
			    }
				else if (core::stringw("Lens") == xml->getName())
				{
					core::stringc fName = SCENE_MANAGER.getTexturesPath();
	                fName.append(xml->getAttributeValue(L"filename"));                    
		            
					f32 spaceConst = xml->getAttributeValueAsFloat(L"space");

					Lens.push_back(scn::SLens(VIDEO_DRIVER.getTexture(fName.c_str()), spaceConst));
			    }
				else if (core::stringw("EtalonScreen") == xml->getName())
				{
					etalonScreen = 
						core::dimension2d<s32>(
							xml->getAttributeValueAsInt(L"width"),
							xml->getAttributeValueAsInt(L"height")
							);
			    }
				else if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}  
		}

		if (xml) xml->drop(); 		

		node = SCENE_MANAGER.addLensFlareSceneNode(); 

		if (node) 
		{		
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);				
			}

			node->setCorona(Corona);

			for  (u32 l=0; l<Lens.size(); l++)
				node->addLens(Lens[l]);	
			
			node->setEtalonScreenSize(etalonScreen);	

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}
    
	    POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CLightSceneNodeLoader : public CSceneNodeLoader
{
public:

	CLightSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CLightSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ILightSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_LIGHT_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::stringc basePath = core::extractFilePath(xml_fname);

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

	    vid::SLight lightData;
		core::stringc fNameAnimCurr="";
		core::array<core::stringc> fNameAnim;
		core::vector3df lvol_center_offset(0,0,0), lvol_size_change(0,0,0);

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		PRE_LOAD_SCENE_NODE_MACRO(sntype);

	    while(xml->read()&&!scene_tag_finished)
	    {
			if (xml->getNodeType()==io::EXN_ELEMENT)
	        {    
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName())
		        {
					s32 xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType() == io::EXN_ELEMENT)
						{							
							if (core::stringw("LightParameters") == xml->getName())
							{
								lightData.Enabled     = xml->getAttributeValueAsBool(L"enabled");								
								lightData.CastShadows = xml->getAttributeValueAsBool(L"cast_shadows");
								lightData.Radius      = xml->getAttributeValueAsFloat(L"radius");
								core::stringc ltype_str = xml->getAttributeValue(L"type");
								lightData.Dynamic		= xml->getAttributeValueAsBool(L"dynamic", false);

								for (s32 i=0; i<vid::E_LIGHT_TYPE_COUNT; i++)
								{
									if (ltype_str==vid::LightTypeName[i])
									{
										lightData.Type = (vid::E_LIGHT_TYPE)i;
										break;
									}
								}

								xml_pos = xml->getPos();

								while(xml->read())
								{
									if (xml->getNodeType() == io::EXN_ELEMENT)
									{
										if (core::stringw("DiffuseColorF") == xml->getName())
										{		
											lightData.DiffuseColor.r = xml->getAttributeValueAsFloat(L"red");
											lightData.DiffuseColor.g = xml->getAttributeValueAsFloat(L"green");
											lightData.DiffuseColor.b = xml->getAttributeValueAsFloat(L"blue");
											lightData.DiffuseColor.a = xml->getAttributeValueAsFloat(L"alpha");

										}
										else
										if (core::stringw("AmbientColorF") == xml->getName())
										{		
											lightData.AmbientColor.r = xml->getAttributeValueAsFloat(L"red");
											lightData.AmbientColor.g = xml->getAttributeValueAsFloat(L"green");
											lightData.AmbientColor.b = xml->getAttributeValueAsFloat(L"blue");
											lightData.AmbientColor.a = xml->getAttributeValueAsFloat(L"alpha");
										}
										else
										if (core::stringw("SpecularColorF") == xml->getName())
										{		
											lightData.SpecularColor.r = xml->getAttributeValueAsFloat(L"red");
											lightData.SpecularColor.g = xml->getAttributeValueAsFloat(L"green");
											lightData.SpecularColor.b = xml->getAttributeValueAsFloat(L"blue");
											lightData.SpecularColor.a = xml->getAttributeValueAsFloat(L"alpha");
										}
										else
										if (core::stringw("IntensityWave") == xml->getName())
										{		
											lightData.IntensityWaveEnabled = xml->getAttributeValueAsBool(L"enabled");

											core::stringc wtype_str = xml->getAttributeValue(L"type");

											for (s32 i=0; i<core::E_FUNC_TYPE_COUNT; i++)
											{
												if (wtype_str==core::getFuncTypeName((core::E_FUNC_TYPE)i))
												{
													lightData.IntensityWave.func = (core::E_FUNC_TYPE)i;
													break;
												}
											}

											lightData.IntensityWave.base      = xml->getAttributeValueAsFloat(L"base");
											lightData.IntensityWave.amplitude = xml->getAttributeValueAsFloat(L"ampl");
											lightData.IntensityWave.phase     = xml->getAttributeValueAsFloat(L"phase");
											lightData.IntensityWave.frequency = xml->getAttributeValueAsFloat(L"freq");
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
							if (core::stringw("LightedVolumeCenterOffset") == xml->getName())
							{
								lvol_center_offset = xml->getAttributeValueAsVector(L"value", core::vector3df(0,0,0));	
							}
							else
							if (core::stringw("LightedVolumeSizeChange") == xml->getName())
							{
								lvol_size_change = xml->getAttributeValueAsVector(L"value", core::vector3df(0,0,0));	
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
				else if (parseAddAnimatorTag(xml, fNameAnimCurr)) 
					fNameAnim.push_back(fNameAnimCurr);					
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop();     

		node = SCENE_MANAGER.addLightSceneNode();

		if (node) 		
	    {
			node->setFileName(xml_fname.c_str());

		    // add animators
			core::stringc fName;
			for (u32 a=0; a<(s32)fNameAnim.size(); a++)
			{
				core::stringc anim_full_fname = basePath;
				anim_full_fname.append(fNameAnim[a]);

				scn::ISceneNodeAnimator* anim = CAnimatorsLoader::load(anim_full_fname);
					
				if (anim) 
				{
					node->addAnimator(anim);    
					anim->drop(); 
				}
			}	

	        // set light data
			node->setLightParameters(lightData);
			node->setLightedVolumeCenterOffset(lvol_center_offset);
			node->setLightedVolumeSizeChange(lvol_size_change);

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
	    }

	    POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CKTreeSceneNodeLoader : public CSceneNodeLoader
{
public:

	CKTreeSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CKTreeSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::IKTreeSceneNode* node = 0;	
		
		E_SCENE_NODE_TYPE sntype = ESNT_KTREE_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;
    
		core::stringc fName, phys_fname;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		s32 MaxTrianglesPerNode = 2048;

	    PRE_LOAD_SCENE_NODE_MACRO(sntype);
		
	    while(xml->read()&&!scene_tag_finished)
	    {   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {					
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName())
				{
					s32 xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType() == io::EXN_ELEMENT)
						{
							if (core::stringw("MeshFileName") == xml->getName())
							{
								fName = RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES);
								fName.append(xml->getAttributeValue(L"value"));			
							}
							else
							if (core::stringw("PhysicalMeshFileName") == xml->getName())
							{
								phys_fname = RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES);
								phys_fname.append(xml->getAttributeValue(L"value"));
							}
							else
							if (core::stringw("MaxTrianglesPerNode") == xml->getName())
							{
								MaxTrianglesPerNode = xml->getAttributeValueAsInt(L"value");
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
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}	
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop(); 	

		scn::IAnimatedMesh* mesh = SCENE_MANAGER.getMesh(
			fName.c_str(), scn::ELL_LOD_NEAREST);

		if (!mesh)
		{
			LOGGER.logErr("Couldn't load mesh from %s", fName.c_str());
		}  
	
		if (mesh)
		{ 
			node = SCENE_MANAGER.addKTreeSceneNode(); 		

			if (node)
			{
				node->setFileName(xml_fname.c_str());

				((IKTreeSceneNode*)node)->setMaxTrianglesPerNode(MaxTrianglesPerNode);
				node->setAnimatedMesh(mesh);

				if (found_material)
				{
					node->setMaterial(node_material.Material);	
				}

				if (phys_fname.size()>0)
				{
					scn::IAnimatedMesh *amesh = 
						SCENE_MANAGER.getMesh(phys_fname.c_str());

					if (amesh)
					{
						node->setPhysicalMesh(amesh);
					}
				}

				if (finded_parameters)
				{
					node->setParameters(node_parameters);
				}

				CSceneNodeLoader::bindChilds(node, childs);
				CSceneNodeLoader::bindCaption(node, caption);
			}
		}

		POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CParticleSystemSceneNodeLoader : public CSceneNodeLoader
{
public:

	CParticleSystemSceneNodeLoader()
	{
	#if MY_DEBUG_MODE 
		setClassName("CParticleSystemSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::IParticleSystemSceneNode* node = 0;	
		
		E_SCENE_NODE_TYPE sntype = ESNT_PARTICLE_SYSTEM_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();	

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;
   
		PRE_LOAD_SCENE_NODE_MACRO(sntype);

		node = SCENE_MANAGER.addParticleSystemSceneNode();    

		s32 xml_pos = xml->getPos();

		while(xml->read()&&!scene_tag_finished)
	    {
			if (xml->getNodeType()==io::EXN_ELEMENT)
			{
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName())
		        {
					xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType()==io::EXN_ELEMENT)
						{
							if (core::stringw("Emitter")==xml->getName())
							{
								core::stringc emitter_type_str = xml->getAttributeValue(L"type");				        
								bool enabled = xml->getAttributeValueAsBool(L"enabled");
								bool global_transform = xml->getAttributeValueAsBool(L"global_transform",true);
								E_PARTICLE_EMITTER_TYPE emitter_type = EPET_BOX;							

								for (s32 i=0; i<E_PARTICLE_EMITTER_TYPE_COUNT; i++)
								{
									if (emitter_type_str==ParticleEmitterTypeStr[i])
									{
										emitter_type = (E_PARTICLE_EMITTER_TYPE)i;
										break;
									}
								}

								IParticleEmitter* emitter = node->addEmitter(emitter_type);
								emitter->setEnabled(enabled);
								emitter->setGlobalParticlesTransformation(global_transform);

								xml_pos = xml->getPos();

								while(xml->read())
								{
									if (xml->getNodeType()==io::EXN_ELEMENT)
									{
										if (core::stringw("Texture") == xml->getName())
										{                            
											core::stringc fname = xml->getAttributeValue(L"filename");
											vid::ITexture *texture = VIDEO_DRIVER.findTexture(fname.c_str());
											if (!texture)
											{
												core::stringc full_fname =
													RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES);
												full_fname.append(fname);
												texture = VIDEO_DRIVER.getTexture(full_fname.c_str());
											}
											emitter->setTexture(texture);	
										}
										else
										if (core::stringw("ParticleSize") == xml->getName())
										{
											core::dimension2d<f32> particle_size(
												xml->getAttributeValueAsFloat(L"width"),				        
												xml->getAttributeValueAsFloat(L"height")
												);
											emitter->setParticleSize(particle_size);				
										}
										else if (core::stringw("AppearVolume") == xml->getName())
										{
											core::aabbox3d<f32> appear_volume(
												xml->getAttributeValueAsFloat(L"xmin"), 
												xml->getAttributeValueAsFloat(L"ymin"), 
												xml->getAttributeValueAsFloat(L"zmin"), 
												xml->getAttributeValueAsFloat(L"xmax"),
												xml->getAttributeValueAsFloat(L"ymax"), 
												xml->getAttributeValueAsFloat(L"zmax")
												);
											emitter->setAppearVolume(appear_volume);
										}
										else if (core::stringw("ParticleDirection") == xml->getName())
										{
											core::vector3df particle_direction(
												xml->getAttributeValueAsFloat(L"x"), 
												xml->getAttributeValueAsFloat(L"y"), 
												xml->getAttributeValueAsFloat(L"z")
												);		
											emitter->setDirection(particle_direction); 		
										}
										else if (core::stringw("ParticlesPerSecond") == xml->getName())
										{
											u32 min_pps = xml->getAttributeValueAsInt(L"min");
											u32 max_pps = xml->getAttributeValueAsInt(L"max");													
											emitter->setMinParticlePerSecond(min_pps);
											emitter->setMaxParticlePerSecond(max_pps);
										}
										else if (core::stringw("LifeTime") == xml->getName())
										{
											u32 life_time_min = xml->getAttributeValueAsInt(L"min");
											u32 life_time_max = xml->getAttributeValueAsInt(L"max");		
											emitter->setMinLifeTime(life_time_min);
											emitter->setMaxLifeTime(life_time_max);
										}
										else if (core::stringw("MinStartColor") == xml->getName())
										{
											img::SColor min_start_color(
												xml->getAttributeValueAsInt(L"alpha"), 
												xml->getAttributeValueAsInt(L"red"), 
												xml->getAttributeValueAsInt(L"green"), 
												xml->getAttributeValueAsInt(L"blue") 
												);										
											emitter->setMinStartColor(min_start_color);		
										}
										else if (core::stringw("MaxStartColor") == xml->getName())
										{
											img::SColor max_start_color(
												xml->getAttributeValueAsInt(L"alpha"), 
												xml->getAttributeValueAsInt(L"red"), 
												xml->getAttributeValueAsInt(L"green"), 
												xml->getAttributeValueAsInt(L"blue") 
												);			
											emitter->setMaxStartColor(max_start_color);
										}							
										else if (core::stringw("AngleDegrees") == xml->getName())
										{
											s32 angle_degrees = xml->getAttributeValueAsInt(L"value");
											emitter->setMaxAngleDegrees(angle_degrees);		
										}
										else if (core::stringw("Affector") == xml->getName())
										{										
											core::stringc affector_type_str = xml->getAttributeValue(L"type");				        
											bool enabled = xml->getAttributeValueAsBool(L"enabled");
											E_PARTICLE_AFFECTOR_TYPE affector_type = EPAT_FADEOUT;	
											u32 affect_time_ms = xml->getAttributeValueAsInt(L"affect_time_ms");

											for (s32 i=0; i<E_PARTICLE_AFFECTOR_TYPE_COUNT; i++)
											{
												if (affector_type_str==ParticleAffectorTypeStr[i])
												{
													affector_type = (E_PARTICLE_AFFECTOR_TYPE)i;
													break;
												}
											}

											IParticleAffector* affector = emitter->addAffector(affector_type);
											affector->setEnabled(enabled);
											affector->setAffectTimeMs(affect_time_ms);

											xml_pos = xml->getPos();

											while(xml->read())
											{
												if (xml->getNodeType()==io::EXN_ELEMENT)
												{
													if (core::stringw("TargetColor") == xml->getName() &&
														affector_type == EPAT_FADEOUT															
														)
													{
														img::SColor target_color = img::SColor(
															xml->getAttributeValueAsInt(L"alpha"), 
															xml->getAttributeValueAsInt(L"red"), 
															xml->getAttributeValueAsInt(L"green"), 
															xml->getAttributeValueAsInt(L"blue") 
															);
														((CParticleFadeOutAffector*)affector)->setTargetColor(target_color);
													}							
													else 
													if (core::stringw("GravityValue") == xml->getName() &&
														affector_type == EPAT_GRAVITY
														)
													{	
														core::vector3df gravity = core::vector3df(
															xml->getAttributeValueAsFloat(L"x"), 
															xml->getAttributeValueAsFloat(L"y"), 
															xml->getAttributeValueAsFloat(L"z")
															);
														((CParticleGravityAffector*)affector)->setGravity(gravity);
													}
													else
													{
														break;
													}

													xml_pos = xml->getPos();
												}	
											}

											xml->setPos(xml_pos);
										}
										else
										{
											break;
										}

										xml_pos = xml->getPos();
									}								
								}
								
								xml->setPos(xml_pos);
							}
							else if (parseMaterialTag(xml, node_material)) 
							{
								found_material = true;
							}
							else
							{
								break;
							}

							xml_pos = xml->getPos();
						}
					}

					xml->setPos(xml_pos);
				}
				else
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{	
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		xml->setPos(xml_pos);

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}	

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
	    }

		if (xml) xml->drop();

		POST_LOAD_SCENE_NODE_MACRO(sntype);
   
		return node;
	}
};

//----------------------------------------------------------------------------

class CSkyBoxSceneNodeLoader : public CSceneNodeLoader
{
public:

	CSkyBoxSceneNodeLoader()
	{
	#if MY_DEBUG_MODE 
		setClassName("CSkyBoxSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ISkyBoxSceneNode* node = 0;

		E_SCENE_NODE_TYPE sntype = ESNT_SKY_BOX_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::stringc basePath = core::extractFilePath(xml_fname);

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;
		
		// textures filenames
		core::stringc UpImageFileName ="";
	    core::stringc DownImageFileName ="";
		core::stringc LeftImageFileName ="";    
		core::stringc RightImageFileName ="";
	    core::stringc FrontImageFileName ="";
	    core::stringc BackImageFileName ="";

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;
		
		PRE_LOAD_SCENE_NODE_MACRO(sntype);

	    while(xml->read()&&!scene_tag_finished)
		{   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {
				if (core::stringw("TopSideTexture") == xml->getName())
				{
					UpImageFileName = SCENE_MANAGER.getTexturesPath();
					UpImageFileName.append(xml->getAttributeValue(L"filename"));
	            }
				else if (core::stringw("BottomSideTexture") == xml->getName())
			    {   
					DownImageFileName = SCENE_MANAGER.getTexturesPath();
					DownImageFileName.append(xml->getAttributeValue(L"filename"));
	            }
				else if (core::stringw("RightSideTexture") == xml->getName())
			    {   
					RightImageFileName = SCENE_MANAGER.getTexturesPath();
					RightImageFileName.append(xml->getAttributeValue(L"filename"));
	            }
				else if (core::stringw("LeftSideTexture") == xml->getName())
			    {   
					LeftImageFileName = SCENE_MANAGER.getTexturesPath();
					LeftImageFileName.append(xml->getAttributeValue(L"filename"));
	            }					
				else if (core::stringw("FrontSideTexture") == xml->getName())
			    {   
					FrontImageFileName = SCENE_MANAGER.getTexturesPath();
					FrontImageFileName.append(xml->getAttributeValue(L"filename"));
	            }
				else if (core::stringw("BackSideTexture") == xml->getName())
			    {   
					BackImageFileName = SCENE_MANAGER.getTexturesPath();
					BackImageFileName.append(xml->getAttributeValue(L"filename"));
	            }
				else if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
	        }
		}

		if (xml) xml->drop(); 

		bool flgmip= VIDEO_DRIVER.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

		VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, false);

		node = SCENE_MANAGER.addSkyBoxSceneNode(
			VIDEO_DRIVER.getTexture(UpImageFileName.c_str()),
			VIDEO_DRIVER.getTexture(DownImageFileName.c_str()),
			VIDEO_DRIVER.getTexture(RightImageFileName.c_str()),
			VIDEO_DRIVER.getTexture(LeftImageFileName.c_str()),			
			VIDEO_DRIVER.getTexture(FrontImageFileName.c_str()),
			VIDEO_DRIVER.getTexture(BackImageFileName.c_str())
			);

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

		VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS,flgmip);
		
	    POST_LOAD_SCENE_NODE_MACRO(sntype);

		return node;
	}    
};

//----------------------------------------------------------------------------

class CTestSceneNodeLoader : public CSceneNodeLoader
{
public:

	CTestSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTestSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ITestSceneNode* node = 0;	
		
		E_SCENE_NODE_TYPE sntype = ESNT_TEST_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		E_GEOM_PRIMITIVE_TYPE geom_type = EGPT_BOX;

		PRE_LOAD_SCENE_NODE_MACRO(sntype);
		
	    while(xml->read()&&!scene_tag_finished)
	    {   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {					
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName())
				{
					s32 xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType() == io::EXN_ELEMENT)
						{
							if (core::stringw("GeomPrimitive") == xml->getName())
							{
								core::stringc geom_type_str = xml->getAttributeValue(L"value");	

								for (s32 i=0; i<E_GEOM_PRIMITIVE_TYPE_COUNT; i++)
								{
									if (geom_type_str==GeomPrimitiveTypeStr[i])
									{
										geom_type = (E_GEOM_PRIMITIVE_TYPE)i;
										break;
									}
								}
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
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}	
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop(); 	

		node = SCENE_MANAGER.addTestSceneNode(); 		

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

	    POST_LOAD_SCENE_NODE_MACRO(sntype);		

		return node;
	}
};

//----------------------------------------------------------------------------

class CTextSceneNodeLoader : public CSceneNodeLoader
{
public:

	CTextSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTextSceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ITextSceneNode* node = 0;	
		
		E_SCENE_NODE_TYPE sntype = ESNT_TEXT_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;    

		core::stringc font_full_fname;
		s32 font_size;
		core::stringc text;
		img::SColor text_color;
		
		PRE_LOAD_SCENE_NODE_MACRO(sntype);
		
	    while(xml->read()&&!scene_tag_finished)
	    {   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {					
				if (core::stringw(SceneNodeCorePropertiesTagValue) == xml->getName())
				{
					s32 xml_pos = xml->getPos();

					while(xml->read())
					{
						if (xml->getNodeType() == io::EXN_ELEMENT)
						{
							if (core::stringw("Font") == xml->getName())
							{
								core::stringc font_fname = xml->getAttributeValue(L"filename");
								font_full_fname =
									RESOURCE_MANAGER.getMediaDirFull ( res::EMT_FONTS );
								font_full_fname.append(font_fname);

								font_size  = xml->getAttributeValueAsInt(L"size");
							}								
							else
							if (core::stringw("Text") == xml->getName())
							{
								text = xml->getAttributeValue(L"value");	
							}								
							else
							if (core::stringw("TextColor") == xml->getName())
							{
								text_color = xml->getAttributeValueAsColor(L"value");	
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
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}	
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop(); 	

		node = SCENE_MANAGER.addTextSceneNode(NULL, "Text"); 		

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			vid::IFont* font = 
				VIDEO_DRIVER.getFont(font_full_fname.c_str());
			if (font)
			{
				font->setSize(font_size);
				node->setFont(font);
			}
			node->setText     (text.c_str());
			node->setTextColor(text_color);  

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

	    POST_LOAD_SCENE_NODE_MACRO(sntype);		

		return node;
	}
};

//----------------------------------------------------------------------------

class CDummySceneNodeLoader : public CSceneNodeLoader
{
public:

	CDummySceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CDummySceneNodeLoader");
	#endif
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::IDummyTransformationSceneNode* node = 0;

		E_SCENE_NODE_TYPE sntype = ESNT_DUMMY_TANSFORMATION_SCENE_NODE;

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		PRE_LOAD_SCENE_NODE_MACRO(sntype);	

		while(xml->read()&&!scene_tag_finished)
	    {   
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {					
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else
				if (parseBindCaptionTag(xml, caption)) {}
				else
				if (parseBindChildTag(xml, child)) 
				{					
					childs.push_back(child);
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop(); 	

		node = SCENE_MANAGER.addDummyTransformationSceneNode(); 	

		if (node)
		{
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

	    POST_LOAD_SCENE_NODE_MACRO(sntype);		

		return node;
	}
};

//----------------------------------------------------------------------------

class CTerrainSceneNodeLoader : public CSceneNodeLoader
{
protected:

	E_SCENE_NODE_TYPE m_SceneNodeType;

public:

	CTerrainSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTerrainSceneNodeLoader");
	#endif

		m_SceneNodeType = ESNT_TERRAIN_SCENE_NODE;
	}

	virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml)
	{
		scn::ITerrainSceneNode* node = 0;		

		if (!xml) return NULL;

		core::stringc xml_fname = xml->getFileName();

		xml->grab();	

		core::array<SChildEntry> childs;
		SChildEntry child;

		SCaptionEntry caption;
   
	    core::stringc fName;
		core::stringc HeightMapFileName(NONAME_FILE_NAME);
	    core::stringc FogMapFileName(NONAME_FILE_NAME);
		core::array<STileSetParams> TileSetParams;
		s32 TileRepeatNumber =10;
		f32 GridPointSpacing =16;
		f32 HeightScale = 1;
		scn::E_TERRAIN_COLOR_SCHEMA ColorScheme = scn::ETCS_WATER_GREENLANDS;

		SSceneNodeMaterial node_material;
		SSceneNodeParameters node_parameters;
		SSceneNodeTransformation node_transformation;
		bool found_material = false, finded_parameters = false, finded_transformation = false;

		bool scene_tag_finished = false;

		PRE_LOAD_SCENE_NODE_MACRO(m_SceneNodeType);

		while(xml->read()&&!scene_tag_finished)
	    {
			if (xml->getNodeType() == io::EXN_ELEMENT)
	        {   
				if (core::stringw("TileSet") == xml->getName())
			    {
					TileSetParams.push_back(STileSetParams());
				}
				else
				if (core::stringw("Tile0TextureFileName") == xml->getName())
			    {
					core::stringc fname = xml->getAttributeValue(L"value");
					if (fname.size()>0 && fname != NONAME_FILE_NAME)
					{
						core::stringc tex_full_fname = SCENE_MANAGER.getTexturesPath();
						tex_full_fname.append(fname);

						if (TileSetParams.size() > 0)
							TileSetParams[TileSetParams.size() - 1].setTile0FileName(tex_full_fname.c_str());
					}
				}
				else
				if (core::stringw("Tile1TextureFileName") == xml->getName())
			    {
					core::stringc fname = xml->getAttributeValue(L"value");
					if (fname.size()>0 && fname != NONAME_FILE_NAME)
					{
						core::stringc tex_full_fname = SCENE_MANAGER.getTexturesPath();
						tex_full_fname.append(fname);

						if (TileSetParams.size() > 0)
							TileSetParams[TileSetParams.size() - 1].setTile1FileName(tex_full_fname.c_str());
					}
				}
				else
				if (core::stringw("Tile2TextureFileName") == xml->getName())
			    {
					core::stringc fname = xml->getAttributeValue(L"value");
					if (fname.size()>0 && fname != NONAME_FILE_NAME)
					{
						core::stringc tex_full_fname = SCENE_MANAGER.getTexturesPath();
						tex_full_fname.append(fname);

						if (TileSetParams.size() > 0)
							TileSetParams[TileSetParams.size() - 1].setTile2FileName(tex_full_fname.c_str());
					}
				}				 
				else
				if (core::stringw("HeightMapFileName") == xml->getName())
				{
					HeightMapFileName = xml->getAttributeValue(L"value");
	            }
				else 
				if (core::stringw("SplattingMapFileName") == xml->getName())
			    {
					core::stringc fname = xml->getAttributeValue(L"value");
					if (fname.size()>0 && fname != NONAME_FILE_NAME)
					{
						core::stringc tex_full_fname = SCENE_MANAGER.getTexturesPath();
						tex_full_fname.append(fname);

						if (TileSetParams.size() > 0)
							TileSetParams[TileSetParams.size() - 1].setSplatMapFileName(tex_full_fname.c_str());
					}
				}
	            else 
				if (core::stringw("TileRepeatNumber") == xml->getName())
		        {
				    TileRepeatNumber  = xml->getAttributeValueAsInt(L"value", 1);
				}
				else 
				if (core::stringw("GridPointSpacing") == xml->getName())
		        {
				    GridPointSpacing  = xml->getAttributeValueAsFloat(L"value", 32);
				}
				else 
				if (core::stringw("HeightScale") == xml->getName())
		        {
				    HeightScale  = xml->getAttributeValueAsFloat(L"value", 0.1f);
				}						
	            else 					
				if (parseMaterialTag(xml, node_material)) 
				{
					found_material = true;
				}
				else
				if (parseParametersTag(xml, node_parameters)) 
				{
					finded_parameters = true;
				}
				else if (
					core::stringw("Game") == xml->getName() ||
					core::stringw("Sound") == xml->getName() ||
					core::stringw("Dynamic") == xml->getName()
					)  
				{					
					scene_tag_finished = true;
				}
			}
		}

		if (xml) xml->drop(); 

		core::stringc hmap_full_fname = NONAME_FILE_NAME;
		core::stringc fog_full_fname  = NONAME_FILE_NAME;

		if (HeightMapFileName.size()>0 && HeightMapFileName != NONAME_FILE_NAME)
		{
			hmap_full_fname = SCENE_MANAGER.getTexturesPath();
			hmap_full_fname.append(HeightMapFileName);
		}

		if (FogMapFileName.size() > 0 && FogMapFileName != NONAME_FILE_NAME)
		{
			fog_full_fname = SCENE_MANAGER.getTexturesPath();
			fog_full_fname.append(FogMapFileName);
		}  
	
		if (hmap_full_fname  != NONAME_FILE_NAME)
		{
			node = (scn::ITerrainSceneNode*)SCENE_MANAGER.
				addLibMiniTerrainSceneNode(0, -1, 
					hmap_full_fname.c_str(), fog_full_fname.c_str(),
					GridPointSpacing, HeightScale);
			if (node)
			{
				node->setTileSets(TileSetParams);
			}
		}
	
		if (node)
		{
			node->setFileName(xml_fname.c_str());

			if (found_material)
			{
				node->setMaterial(node_material.Material);	
			}

			node->setTileRepeatNumber(TileRepeatNumber);

			if (finded_parameters)
			{
				node->setParameters(node_parameters);
			}

			CSceneNodeLoader::bindChilds(node, childs);
			CSceneNodeLoader::bindCaption(node, caption);
		}

		POST_LOAD_SCENE_NODE_MACRO(m_SceneNodeType);

		return node;
	}
};

//----------------------------------------------------------------------------

class CTerrainLibMiniSceneNodeLoader : public CTerrainSceneNodeLoader
{
public:

	CTerrainLibMiniSceneNodeLoader() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTerrainLibMiniSceneNodeLoader");
	#endif

		m_SceneNodeType = ESNT_TERRAIN_SCENE_NODE;
	}	
};

//----------------------------------------------------------------------------

bool parseAddAnimatorTag(io::IXMLReader* xml, core::stringc &AnimatorFileName)
{
	if (xml)
    {
		if (core::stringw("BindAnimator") == xml->getName())
        {
			AnimatorFileName = xml->getAttributeValue(L"filename");        
            return true;
        }       
    }
    return false;
}

//----------------------------------------------------------------------------

bool parseBindChildTag(io::IXMLReader* xml, SChildEntry &child)
{
	if (xml)
    {
		if (core::stringw("BindChild") == xml->getName())
        {			
			core::stringc fname = xml->getAttributeValue(L"filename");

			if (fname.size()>0)
			{
				core::stringc xml_fname = xml->getFileName();
				core::stringc base_path = core::extractFilePath(xml_fname);

				child.FileName = base_path;
				child.FileName.append(fname);
				child.Position = xml->getAttributeValueAsVector(L"pos");  
				child.Rotation = xml->getAttributeValueAsVector(L"rot");  
				child.Scale    = xml->getAttributeValueAsVector(L"scl");  

				return true;
			}			            
        }       
    }
    return false;
}

//----------------------------------------------------------------------------

bool parseBindCaptionTag(io::IXMLReader* xml, SCaptionEntry& caption )
{
	if (xml)
    {
		if (core::stringw("BindCaption") == xml->getName())
        {			
			caption.FontFileName = xml->getAttributeValue(L"font");

			if (caption.FontFileName.size()>0)
			{
				caption.Color    = xml->getAttributeValueAsColor(L"color");     
				caption.Position = xml->getAttributeValueAsVector(L"pos");
				f32 height       = xml->getAttributeValueAsFloat(L"height");  
				caption.FontSize = height*(f32)VIDEO_DRIVER.getScreenSize().Height; 
				caption.Text     = xml->getAttributeValue(L"text");  
				caption.Visible  = xml->getAttributeValueAsBool(L"visible");
				caption.FontSize = xml->getAttributeValueAsInt(L"font_size");

				return true;
			}			            
        }       
    }
    return false;
}

//----------------------------------------------------------------------------

bool parseMaterialTag(io::IXMLReader* xml, SSceneNodeMaterial &material)
{
	if (xml)
    {
		if (core::stringw("Material") == xml->getName())
        {
			material = SCENE_MANAGER.loadSceneNodeMaterial(xml);
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------

bool parseParametersTag(io::IXMLReader* xml, SSceneNodeParameters &parameters)
{
	if (xml)
    {
		if (core::stringw("Parameters") == xml->getName())
        {
			parameters = SCENE_MANAGER.loadSceneNodeParameters(xml);

            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------

bool parseTransformationTag (io::IXMLReader* xml, SSceneNodeTransformation &transf )
{
	if (xml)
    {
		if (core::stringw("Transformation") == xml->getName())
        {
			transf = SCENE_MANAGER.loadSceneNodeTransformation(xml);

            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
