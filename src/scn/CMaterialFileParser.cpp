//|-------------------------------------------------------------------------
//| File:        CMaterialFileParser.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CMaterialFileParser.h"

#include "CMaterialsManager.h"

#include <io/IFileSystem.h>
#include <io/IReadFile.h>
#include <io/ILogger.h>
#include <io/IXMLReader.h>
#include <io/IXMLWriter.h>
#include <scn/ISceneManager.h>
#include <vid/ITexture.h>
#include <res/IResourceManager.h>
#include <scn/IMaterialsManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

void CMaterialFileParser::loadMaterials(
	const c8 *fname, core::array<SMeshBufferMaterial> &materials,
	bool load_textures, bool reload_if_exists,
	u32 &loaded, u32 &reloaded, u32 &errors)
{
	LOGGER.logInfo("Loading materials from %s . . .", fname);

	io::IXMLReader *xml = FILE_SYSTEM.createXMLReader(fname);
	if (!xml)
	{
		LOGGER.logErr(" Can't open material file");
		errors++;
		return;
	}	
		
	// parsing materials

	u32 m_cnt = 0;

	LOGGER.increaseFormatLevel();

	core::stringc old_path = ISceneManager::getSingleton().getTexturesPath();

	while (xml->read())
	{
		SMeshBufferMaterial material;

		if (xml->getNodeType()==io::EXN_ELEMENT &&
				core::stringw("Material") == xml->getName())
		{
			core::stringc name = xml->getAttributeValue(L"name");

			LOGGER.logInfo("Parsing material '%s' . . .", name.c_str());

			s32 mat_idx = !reload_if_exists ? -1 :
				MATERIALS_MANAGER.findMaterialByName(name.c_str());

			LOGGER.increaseFormatLevel();

			if (CMaterialFileParser::loadMaterialTo(xml, material, load_textures)) 
			{
				if (mat_idx != -1)
				{
					MATERIALS_MANAGER.setMaterial(mat_idx, material.Material);
					reloaded++;
				}
				else
				{
					materials.push_back(SMeshBufferMaterial());
					materials[materials.size()-1] = material;
					loaded++;
				}

				u64 hash = material.Material.getHash();
				for (u32 i = 0; i < material.Material.getPassesCount(); i++)
				{
					const vid::SRenderPass &pass = material.Material.getPass(i);
					LOGGER.logInfo(" Pass %d: %s, gpu hash 0x%016I64X, ffp hash 0x%016I64X)",
						i, pass.isTransparent() ? "transp" : "solid",
						pass.getHashGPU(), pass.getHashFFP());
				}				
				LOGGER.logInfo(
					"Material '%s' parsed (%d passes, %s, hash 0x%016I64X)", 
					material.Name.c_str(), material.Material.getPassesCount(),
					material.Material.isTransparent() ? "transp" : "solid", hash);

				m_cnt++;
			}
			else
			{
				LOGGER.logErr("Material '%s' parsed with errors", name.c_str() );
				errors++;
			}
			LOGGER.decreaseFormatLevel();
		}
	}

	ISceneManager::getSingleton().setTexturesPath(old_path);	
	
	LOGGER.decreaseFormatLevel();

	if (m_cnt)
		LOGGER.logInfo(" %d materials loaded", m_cnt);
	else
	{
		LOGGER.logWarn(" There is no any material loaded");
		errors++;
	}

	SAFE_DROP(xml);	
}

//---------------------------------------------------------------------------

bool CMaterialFileParser::loadMaterialTo(
	io::IXMLReader *xml, SMeshBufferMaterial &material, bool load_textures)
{
	bool ok = true;

	if (xml &&
		xml->getNodeType()==io::EXN_ELEMENT &&
		core::stringw("Material") == xml->getName()		
		)
    {
		core::stringc tmpStr;
		core::stringc basePath = ISceneManager::getSingleton().getTexturesPath();

		core::stringc name = xml->getAttributeValue(L"name");

		if (name.size()>0)
		{
			material.Name = name;
		}

		int xml_elem_cnt =0;

		s32 xml_pos = xml->getPos();

		s32 pass_index = 0;
		bool first_pass = true;
		bool finished = false;

		while(ok && !finished && xml && xml->read())
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					xml_elem_cnt++;
					core::stringw xml_node_name = xml->getName();

					if (core::stringw("Pass") == xml_node_name)
					{
						if (!first_pass)
							pass_index++;
						else
							first_pass = false;
						vid::SMaterial &m = material.Material;
						if (pass_index >= (s32)material.Material.getPassesCount())
							m.addPass(vid::SRenderPass());
					}
					else
					if (core::stringw("GPUProgram") == xml_node_name)
					{
						core::stringc rel_fname = xml->getAttributeValue(L"filename");;
						core::stringc full_file_name;
						full_file_name.sprintf("%s%s",
							RESOURCE_MANAGER.getMediaDirFull(res::EMT_MATERIALS), rel_fname.c_str());
						vid::IGPUProgram *gpu_prog = VIDEO_DRIVER.getGPUProgram(full_file_name.c_str());
						material.Material.getPass(pass_index).setGPUProgram(gpu_prog);
					}
					else
					if (core::stringw("Layer") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"index");
						core::stringc texture_filename = xml->getAttributeValue(L"texture");	
						core::stringc coords_gen_str = xml->getAttributeValue(L"tc_gen");
						s32 rgb_scale = xml->getAttributeValueAsInt(L"rgb_scale");
						core::stringc tex_layer_type_str = xml->getAttributeValue(L"type");
						core::stringc tex_coord_addr_str = xml->getAttributeValue(L"tc_addr");
						s32 tc_channel = xml->getAttributeValueAsInt(L"tc_channel", 0);
						bool filter = xml->getAttributeValueAsBool(L"filter", true);

						CHECK_RANGE(rgb_scale,  1, 3);
						CHECK_RANGE(tc_channel, 0, 1);

						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer layer;
							int i=0;
							bool coords_gen_ok = false;
							for (i=0; i<vid::E_TEXCOORD_GEN_TYPE_COUNT; i++)
							{
								if (coords_gen_str == vid::TexCoordGenTypeName[i]) 
								{
									layer.setTexCoordGen ( (vid::E_TEXCOORD_GEN_TYPE)i );
									coords_gen_ok = true;
									break;
								}
							}	
							if (coords_gen_str.size()!=0 && !coords_gen_ok)
							{
								LOGGER.logErr("Unknown tex coord gen type %s",
									coords_gen_str.c_str());
								ok = false;	
							}

							bool coords_addr_ok = false;
							for (i=0; i<vid::E_TEXCOORD_ADDRESS_MODE_COUNT; i++)
							{
								if (tex_coord_addr_str == vid::TexCoordAddrModeName[i]) 
								{
									layer.setTexCoordAddr((vid::E_TEXCOORD_ADDRESS_MODE)i);
									coords_addr_ok = true;
									break;
								}
							}	
							if (tex_coord_addr_str.size()!=0 && !coords_addr_ok)
							{
								LOGGER.logErr("Unknown tex coord addr mode %s",
									tex_coord_addr_str.c_str());
								ok = false;	
							}

							bool tex_layer_type_ok = false;
							for (i = 0; i < vid::E_TEXTURE_LAYER_TYPE_COUNT; i++)
							{
								if (tex_layer_type_str == vid::TextureLayerTypeName[i]) 
								{
									layer.setType((vid::E_TEXTURE_LAYER_TYPE)i);
									tex_layer_type_ok = true;
									break;
								}
							}	
							if (tex_layer_type_str.size() != 0 && !tex_layer_type_ok)
							{
								LOGGER.logErr("Unknown texture layer type %s",
									tex_layer_type_str.c_str());
								ok = false;	
							}

							layer.setTexRGBScale(rgb_scale );
							layer.setTexCoordChannel(tc_channel);

							if (load_textures && texture_filename != NONAME_FILE_NAME)
							{
								vid::ITexture *tex = VIDEO_DRIVER.findTexture(texture_filename.c_str());
								if (!tex)
								{
									tmpStr = basePath;
									tmpStr.append(texture_filename.c_str());
									tex = VIDEO_DRIVER.getTexture(tmpStr.c_str());
								}
								if (!tex)
									ok = false;
								layer.setTexture(tex, layer.getType(), tc_channel);								
							}
							layer.enableFilter(filter);
							material.Material.getPass(pass_index).Layers[layer_index] = layer;
						}
					}
					else
					if (core::stringw("TCGenProjection") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];
							layer.setProjectionParams(
								xml->getAttributeValueAsFloat(L"width", 0.f),
								xml->getAttributeValueAsFloat(L"height", 0.f),
								xml->getAttributeValueAsVector(L"pos", core::vector3df(0.f, 0.f, 0.f)),
								xml->getAttributeValueAsVector(L"dir", core::vector3df(0.f, 1.f, 0.f)),
								xml->getAttributeValueAsVector(L"up", core::vector3df(1.f, 0.f, 0.f)));
						}
					}
					else
					if (core::stringw("TCModTranform") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];
							vid::STexCoordModifier tcmod(vid::ETCMT_TRANSFORM);
							tcmod.setMatrix(
								xml->getAttributeValueAsFloat(L"m00", 0),
								xml->getAttributeValueAsFloat(L"m01", 0),
								xml->getAttributeValueAsFloat(L"m10", 0),
								xml->getAttributeValueAsFloat(L"m11", 0));
							tcmod.setTranslate(
								xml->getAttributeValueAsFloat(L"t0", 0),
								xml->getAttributeValueAsFloat(L"t1", 0));
							layer.addTexCoordModifier(tcmod);
						}
					}
					else
					if (core::stringw("TCModTurbulent") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];
							vid::STexCoordModifier tcmod(vid::ETCMT_TURBULENT);

							core::stringc func_str = xml->getAttributeValue(L"type");
							core::E_FUNC_TYPE func = core::EFT_SIN;
							for (u32 i = 0; i < core::E_FUNC_TYPE_COUNT; i++)
							{
								if (func_str == core::FuncTypeName[i])
								{
									func = (core::E_FUNC_TYPE)i;
									break;
								}
							}
							tcmod.setTurbulent(core::wave(func,
								xml->getAttributeValueAsFloat(L"base", 0.f),
								xml->getAttributeValueAsFloat(L"ampl", 0.f),
								xml->getAttributeValueAsFloat(L"phase", 0.f),
								xml->getAttributeValueAsFloat(L"freq", 0.f)));

							layer.addTexCoordModifier(tcmod);
						}
					}
					else
					if (core::stringw("TCModScroll") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];

							vid::STexCoordModifier tcmod(vid::ETCMT_SCROLL);
							tcmod.setScroll(
								xml->getAttributeValueAsFloat(L"s0", 0),
								xml->getAttributeValueAsFloat(L"s1", 0));

							layer.addTexCoordModifier(tcmod);
						}
					}
					else
					if (core::stringw("TCModScale") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];
							vid::STexCoordModifier tcmod(vid::ETCMT_SCALE);
							tcmod.setScale(
								xml->getAttributeValueAsFloat(L"s0", 0),
								xml->getAttributeValueAsFloat(L"s1", 0));

							layer.addTexCoordModifier(tcmod);
						}
					}
					else
					if (core::stringw("TCModRotate") == xml_node_name)
					{
						s32 layer_index = xml->getAttributeValueAsInt(L"layer", 0);
						if (layer_index >= 0 && layer_index< vid::MY_MATERIAL_MAX_LAYERS)
						{
							vid::STextureLayer &layer =
								material.Material.getPass(pass_index).Layers[layer_index];
							vid::STexCoordModifier tcmod(vid::ETCMT_ROTATE);

							tcmod.setRotateSpeed(xml->getAttributeValueAsFloat(L"rot", 0));

							layer.addTexCoordModifier(tcmod);
						}
					}
					else
					if (core::stringw("Flags") == xml_node_name)
					{
						material.Material.getPass(pass_index).setFlag(vid::EMF_GOURAUD_SHADING,
							xml->getAttributeValueAsInt(L"qouraud_shading") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_ZWRITE_ENABLE,
							xml->getAttributeValueAsInt(L"zwrite_enable") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_BACK_FACE_CULLING,
							xml->getAttributeValueAsInt(L"back_face_culling") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_FRONT_FACE_CCW,
							xml->getAttributeValueAsInt(L"front_face_ccw") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_FOG_ENABLE,
							xml->getAttributeValueAsInt(L"fog_enable") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_MIP_MAP_OFF,
							xml->getAttributeValueAsInt(L"mip_map_off") != 0);
						material.Material.getPass(pass_index).setFlag(vid::EMF_BLENDING,
							xml->getAttributeValueAsInt(L"blending") != 0);
					}
					else
					if (core::stringw("LightingMode") == xml_node_name)
					{
						vid::E_LIGHTING_MODE mode =
							(vid::E_LIGHTING_MODE)xml->getAttributeValueAsInt(L"value", 0);
						if (mode >= vid::E_LIGHTING_MODE_COUNT)
							mode = vid::ELM_NONE;
						material.Material.getPass(pass_index).setLightingMode(mode);
					}
					else
					if (core::stringw("AlphaTest") == xml_node_name)
					{
						core::stringc atest_str = xml->getAttributeValue(L"value");
						bool atest_ok = false;
						for (int i=0; i<vid::E_ALPHA_TEST_COUNT; i++)
						{
							if (atest_str == vid::AlphaTestName[i]) 
							{
								material.Material.getPass(pass_index).setAlphaTest (
									(vid::E_ALPHA_TEST)i );
								atest_ok = true;
								break;
							}							
						}

						if (!atest_ok)
						{
							LOGGER.log("Unknown alpha test func", atest_str.c_str(), io::ELL_ERROR);
							ok = false;						
						}
					}
					else
					if (core::stringw("DepthTest") == xml_node_name)
					{
						core::stringc dtest_str = xml->getAttributeValue(L"value");
						bool dtest_ok = false;
						for (int i=0; i<vid::E_COMPARE_TEST_COUNT; i++)
						{
							if (dtest_str == vid::CompareTestName[i]) 
							{
								material.Material.getPass(pass_index).setDepthTest (
									(vid::E_COMPARE_TEST)i );
								dtest_ok = true;
								break;
							}							
						}

						if (!dtest_ok)
						{
							LOGGER.log("Unknown depth test func", dtest_str.c_str(), io::ELL_ERROR);
							ok = false;						
						}
					}
					else 
					if (core::stringw("Ambient") == xml_node_name)
					{
						img::SColor c = xml->getAttributeValueAsColor(L"rgba", 0x00000000);
						if (c.color == 0x00000000)
							c = img::SColor(
								xml->getAttributeValueAsInt(L"alpha"),
								xml->getAttributeValueAsInt(L"red"  ),
								xml->getAttributeValueAsInt(L"green"),
								xml->getAttributeValueAsInt(L"blue" ));
						material.Material.getPass(pass_index).setAmbientColor(c);
					}
					else 
					if (core::stringw("Diffuse") == xml_node_name)
					{
						img::SColor c = xml->getAttributeValueAsColor(L"rgba", 0x00000000);
						if (c.color == 0x00000000)
							c = img::SColor(
								xml->getAttributeValueAsInt(L"alpha"),
								xml->getAttributeValueAsInt(L"red"  ),
								xml->getAttributeValueAsInt(L"green"),
								xml->getAttributeValueAsInt(L"blue" ));
						material.Material.getPass(pass_index).setDiffuseColor(c);
					}
					else 
					if (core::stringw("Emissive") == xml_node_name)
					{
						img::SColor c = xml->getAttributeValueAsColor(L"rgba", 0x00000000);
						if (c.color == 0x00000000)
							c = img::SColor(
								xml->getAttributeValueAsInt(L"alpha"),
								xml->getAttributeValueAsInt(L"red"  ),
								xml->getAttributeValueAsInt(L"green"),
								xml->getAttributeValueAsInt(L"blue" ));
						material.Material.getPass(pass_index).setEmissiveColor(c);
					}
					else
					if (core::stringw("Specular") == xml_node_name)
					{
						img::SColor c = xml->getAttributeValueAsColor(L"rgba", 0x00000000);
						if (c.color == 0x00000000)
						{
							c = img::SColor(
								xml->getAttributeValueAsInt(L"alpha"),
								xml->getAttributeValueAsInt(L"red"  ),
								xml->getAttributeValueAsInt(L"green"),
								xml->getAttributeValueAsInt(L"blue" ));
						}
						material.Material.getPass(pass_index).setSpecularColor(c);
					}
					else 
					if (core::stringw("Shininess") == xml_node_name)
					{
						material.Material.getPass(pass_index).
							setShininess ( xml->getAttributeValueAsFloat(L"value") );
					}
					else 
					if (core::stringw("BlendFunc") == xml_node_name)
					{
						bool eq_blend_ok = true, src_blend_ok = false, dst_blend_ok = false;

						core::stringc eq_blend_str = xml->getAttributeValue(L"eq");
						core::stringc src_blend_func_str = xml->getAttributeValue(L"src");
						core::stringc dst_blend_func_str = xml->getAttributeValue(L"dst");

						vid::E_BLEND_EQUATION eq_blend = vid::EBE_ADD;
						vid::E_SRC_BLEND_FUNCTION src_blend_func;
						vid::E_DST_BLEND_FUNCTION dst_blend_func;

						for (s32 e = 0; e < vid::E_BLEND_EQUATION_COUNT; e++)
						{
							if (eq_blend_str == vid::SrcBlendFunctionName[e]) 
							{
								eq_blend = (vid::E_BLEND_EQUATION)e;
								break;
							}
						}
						for (s32 s=0; s<vid::E_SRC_BLEND_FUNCTION_COUNT; s++)
						{
							if (src_blend_func_str == vid::SrcBlendFunctionName[s]) 
							{
								src_blend_func = (vid::E_SRC_BLEND_FUNCTION)s;
								src_blend_ok = true;
								break;
							}
						}
						for (s32 s=0; s<vid::E_SRC_BLEND_FUNCTION_COUNT; s++)
						{
							if (src_blend_func_str == vid::SrcBlendFunctionName[s]) 
							{
								src_blend_func = (vid::E_SRC_BLEND_FUNCTION)s;
								src_blend_ok = true;
								break;
							}
						}
						for (s32 d=0; d<vid::E_DST_BLEND_FUNCTION_COUNT; d++)
						{
							if (dst_blend_func_str == vid::DstBlendFunctionName[d]) 
							{
								dst_blend_func = (vid::E_DST_BLEND_FUNCTION)d;
								dst_blend_ok = true;
								break;
							}
						}

						if (!src_blend_ok)
						{
							LOGGER.logWarn("Unknown src blend '%s'", src_blend_func_str.c_str());
							src_blend_func = vid::ESBF_ONE;
							ok = false;
						}
						if (!dst_blend_ok)
						{
							LOGGER.logWarn("Unknown dest blend '%s'", dst_blend_func_str.c_str());
							dst_blend_func = vid::EDBF_ONE;
							ok = false;
						}
						material.Material.getPass(pass_index).setBlendFuncs(
							src_blend_func, dst_blend_func);
					}
					else
					if (core::stringw("ColorGen") == xml_node_name)
					{
						core::stringc cgen_str = xml->getAttributeValue(L"type");
						vid::E_COLOR_GEN_TYPE cgen = vid::ECGT_VERTEX;
						for (u32 i = 0; i < vid::E_COLOR_GEN_TYPE_COUNT; i++)
						{
							if (cgen_str == vid::ColorGenTypeName[i])
							{
								cgen = (vid::E_COLOR_GEN_TYPE)i;
								break;
							}
						}
						material.Material.getPass(pass_index).setColorGen(cgen);
					}
					else
					if (core::stringw("AlphaGen") == xml_node_name)
					{
						core::stringc agen_str = xml->getAttributeValue(L"type");
						vid::E_ALPHA_GEN_TYPE agen = vid::EAGT_VERTEX;
						for (u32 i = 0; i < vid::E_ALPHA_GEN_TYPE_COUNT; i++)
						{
							if (agen_str == vid::AlphaGenTypeName[i])
							{
								agen = (vid::E_ALPHA_GEN_TYPE)i;
								break;
							}
						}
						material.Material.getPass(pass_index).setAlphaGen(agen);
					}
					else
					if (core::stringw("ColorFunc") == xml_node_name)
					{
						core::stringc func_str = xml->getAttributeValue(L"type");
						core::E_FUNC_TYPE func = core::EFT_SIN;
						for (u32 i = 0; i < core::E_FUNC_TYPE_COUNT; i++)
						{
							if (func_str == core::FuncTypeName[i])
							{
								func = (core::E_FUNC_TYPE)i;
								break;
							}
						}
						material.Material.getPass(pass_index).setColorFunc(
							core::wave(func,
								xml->getAttributeValueAsFloat(L"base", 0.f),
								xml->getAttributeValueAsFloat(L"ampl", 0.f),
								xml->getAttributeValueAsFloat(L"phase", 0.f),
								xml->getAttributeValueAsFloat(L"freq", 0.f)
								));
					}
					else
					if (core::stringw("AlphaFunc") == xml_node_name)
					{
						core::stringc func_str = xml->getAttributeValue(L"type");
						core::E_FUNC_TYPE func = core::EFT_SIN;
						for (u32 i = 0; i < core::E_FUNC_TYPE_COUNT; i++)
						{
							if (func_str == core::FuncTypeName[i])
							{
								func = (core::E_FUNC_TYPE)i;
								break;
							}
						}
						material.Material.getPass(pass_index).setAlphaFunc(
							core::wave(func,
								xml->getAttributeValueAsFloat(L"base", 0.f),
								xml->getAttributeValueAsFloat(L"ampl", 0.f),
								xml->getAttributeValueAsFloat(L"phase", 0.f),
								xml->getAttributeValueAsFloat(L"freq", 0.f)
								));
					}
					else
					if (core::stringw("Material") == xml_node_name)
					{
						finished = true;
						xml->setPos(xml_pos);
					}
					else
					{
						finished = true;
						xml->setPos(xml_pos);
					}
					
					xml_pos = xml->getPos();
                }			
            }  
        }
    }

	return ok;
}

//---------------------------------------------------------------------------

void CMaterialFileParser::getMaterialXMLTextTo(
	core::stringc &out_buf, const vid::SMaterial &material,
	bool with_textures, const c8 *name)
{
	res::IResourceManager &resmgr = RESOURCE_MANAGER;
	int p, passcnt = material.getPassesCount();

	if (name)
		out_buf.sprintf("<Material name=\"%s\" >\n", name);
	else
		out_buf = "<Material>\n";

	core::stringc tmp;

	for (p = 0; p < passcnt; p++)
	{
		s32 l;
		const vid::SRenderPass &pass = material.getPass(p);

		out_buf.append("<Pass>\n");

		vid::IGPUProgram *gpu_prog = pass.getGPUProgram();
		if (gpu_prog)
		{
			core::stringc full_file_name = VIDEO_DRIVER.findGPUProgramFullFileName(gpu_prog);
			if (full_file_name.size() > 0)
			{
				const c8 *fname = core::extractFileName(full_file_name.c_str());
				const c8 *fpath = core::extractFilePath(full_file_name.c_str());
				const c8 *rel_fpath = core::getRelativePathBetweenDirectories(
					RESOURCE_MANAGER.getMediaDirFull(res::EMT_MATERIALS), fpath);
				tmp.sprintf("<GPUProgram filename=\"%s%s\" />\n", rel_fpath, fname);
				out_buf.append(tmp);
			}
			else
			{
				LOGGER.logErr("Can't detect GPU program file name!");
			}
		}

		for (l = 0; l < vid::MY_MATERIAL_MAX_LAYERS; l++)
		{
			const vid::STextureLayer &layer = pass.Layers[l];
			vid::ITexture *texture = layer.getTexture();
			if (!texture)
				continue;
			tmp.sprintf(
				"<Layer index=\"%d\" texture=\"%s\" type=\"%s\" rgb_scale=\"%d\" "
				"tc_gen=\"%s\" tc_addr=\"%s\" tc_channel=\"%d\" filter=\"%d\" />\n",
				l, with_textures ? resmgr.getTextureFileNameRel(texture) : NONAME_FILE_NAME,
				vid::TextureLayerTypeName[layer.getType()],
				layer.getTexRGBScale(),
				vid::TexCoordGenTypeName[layer.getTexCoordGen()],
				vid::TexCoordAddrModeName[layer.getTexCoordAddr()],
				layer.getTexCoordChannel(), layer.isFilterEnabled());
			out_buf.append(tmp);
		}

		for (l = 0; l < vid::MY_MATERIAL_MAX_LAYERS; l++)
		{
			const vid::STextureLayer &layer = pass.Layers[l];
			vid::ITexture *texture = layer.getTexture();
			if (!texture)
				continue;
			if (layer.getTexCoordGen() == vid::ETCGT_PROJECTED_MAPPING)
			{
				tmp.sprintf(
					"<TCGenProjection layer=\"%d\" width=\"%.4f\" height=\"%.4f\" "
					"pos=\"%.4f,%.4f,%.4f\" dir=\"%.4f,%.4f,%.4f\" up=\"%.4f,%.4f,%.4f\" />\n",
					l, layer.getProjectionWidth(), layer.getProjectionHeight(),
					layer.getProjectionPos().X, layer.getProjectionPos().Y, layer.getProjectionPos().Z,
					layer.getProjectionDir().X, layer.getProjectionDir().Y, layer.getProjectionDir().Z,
					layer.getProjectionUp().X, layer.getProjectionUp().Y, layer.getProjectionUp().Z);
				out_buf.append(tmp);
			}
			for (u32 i = 0; i < layer.getTexCoordModifiersCount(); i++)
			{
				const vid::STexCoordModifier &tcmod = layer.getTexCoordModifier(i);
				tmp = "";
				if (tcmod.getType() == vid::ETCMT_TRANSFORM)
				{
					tmp.sprintf("<TCModTranform layer=\"%d\" m00=\"%.4f\" m01=\"%.4f\" "
						"m10=\"%.4f\" m11=\"%.4f\" t0=\"%.4f\" t1=\"%.4f\" />\n",
						l, tcmod.getMatrix00(), tcmod.getMatrix01(),
						tcmod.getMatrix10(), tcmod.getMatrix11(),
						tcmod.getTranslate0(), tcmod.getTranslate1());
				}
				else
				if (tcmod.getType() == vid::ETCMT_TURBULENT)
				{
					const core::wave &func = tcmod.getTurbulent();
					tmp.sprintf("<TCModTurbulent layer=\"%d\" type=\"%s\" base=\"%.3f\" "
						"ampl=\"%.3f\" phase=\"%.3f\" freq=\"%.3f\" />\n",
						l, core::FuncTypeName[func.func], func.base,
						func.amplitude, func.phase, func.frequency);
				}
				else
				if (tcmod.getType() == vid::ETCMT_SCROLL)
				{
					tmp.sprintf("<TCModScroll layer=\"%d\" s0=\"%.4f\" s1=\"%.4f\" />\n",
						l, tcmod.getScroll0(), tcmod.getScroll1());
				}
				else
				if (tcmod.getType() == vid::ETCMT_SCALE)
				{
					tmp.sprintf("<TCModScale layer=\"%d\" s0=\"%.4f\" s1=\"%.4f\" />\n",
						l, tcmod.getScale0(), tcmod.getScale1());
				}
				else
				if (tcmod.getType() == vid::ETCMT_ROTATE)
				{
					tmp.sprintf("<TCModRotate layer=\"%d\" rot=\"%.4f\" />\n",
						l, tcmod.getRotateSpeed());
				}
				out_buf.append(tmp);
			}
		}

		tmp.sprintf(
			"<Flags qouraud_shading=\"%d\" zwrite_enable=\"%d\" back_face_culling=\"%d\" "
			"front_face_ccw=\"%d\" fog_enable=\"%d\" mip_map_off=\"%d\" blending=\"%d\" />\n",
			pass.getFlag(vid::EMF_GOURAUD_SHADING),
			pass.getFlag(vid::EMF_ZWRITE_ENABLE),
			pass.getFlag(vid::EMF_BACK_FACE_CULLING),
			pass.getFlag(vid::EMF_FRONT_FACE_CCW),
			pass.getFlag(vid::EMF_FOG_ENABLE),
			pass.getFlag(vid::EMF_MIP_MAP_OFF),
			pass.getFlag(vid::EMF_BLENDING));
		out_buf.append(tmp);

		tmp.sprintf("<LightingMode value=\"%d\" />\n", pass.getLightingMode());
		out_buf.append(tmp);

		tmp.sprintf("<Ambient rgba=\"%d,%d,%d,%d\" />\n",
			pass.getAmbientColor().getRed(),
			pass.getAmbientColor().getGreen(),
			pass.getAmbientColor().getBlue(),
			pass.getAmbientColor().getAlpha());
		out_buf.append(tmp);

		tmp.sprintf("<Diffuse rgba=\"%d,%d,%d,%d\" />\n",
			pass.getDiffuseColor().getRed(),
			pass.getDiffuseColor().getGreen(),
			pass.getDiffuseColor().getBlue(),
			pass.getDiffuseColor().getAlpha());
		out_buf.append(tmp);

		tmp.sprintf("<Emissive rgba=\"%d,%d,%d,%d\" />\n",
			pass.getEmissiveColor().getRed(),
			pass.getEmissiveColor().getGreen(),
			pass.getEmissiveColor().getBlue(),
			pass.getEmissiveColor().getAlpha());
		out_buf.append(tmp);

		tmp.sprintf("<Specular rgba=\"%d,%d,%d,%d\" />\n",
			pass.getSpecularColor().getRed(),
			pass.getSpecularColor().getGreen(),
			pass.getSpecularColor().getBlue(),
			pass.getSpecularColor().getAlpha());
		out_buf.append(tmp);

		tmp.sprintf("<Shininess value=\"%.2f\" />\n", pass.getShininess());
		out_buf.append(tmp);

		tmp.sprintf("<AlphaTest value=\"%s\" />\n", vid::AlphaTestName[pass.getAlphaTest()]);
		out_buf.append(tmp);

		tmp.sprintf("<DepthTest value=\"%s\" />\n", vid::CompareTestName[pass.getDepthTest()]);
		out_buf.append(tmp);

		tmp.sprintf("<BlendFunc eq=\"%s\" src=\"%s\" dst=\"%s\" />\n",
			vid::BlendEquationName[pass.getBlendEquation()],
			vid::SrcBlendFunctionName[pass.getSrcBlendFunc()],
			vid::DstBlendFunctionName[pass.getDstBlendFunc()]);
		out_buf.append(tmp);

		tmp.sprintf("<ColorGen type=\"%s\" />\n",
			vid::ColorGenTypeName[pass.getColorGen()]);
		out_buf.append(tmp);
		if (pass.getColorGen() == vid::ECGT_FUNCTION)
		{
			const core::wave &func = pass.getColorFunc();
			tmp.sprintf("<ColorFunc type=\"%s\" base=\"%.3f\" ampl=\"%.3f\" phase=\"%.3f\" freq=\"%.3f\" />\n",
				core::FuncTypeName[func.func], func.base, func.amplitude, func.phase, func.frequency);
			out_buf.append(tmp);
		}

		tmp.sprintf("<AlphaGen type=\"%s\" />\n",
			vid::AlphaGenTypeName[pass.getAlphaGen()]);
		out_buf.append(tmp);
		if (pass.getAlphaGen() == vid::EAGT_FUNCTION)
		{
			const core::wave &func = pass.getAlphaFunc();
			tmp.sprintf("<AlphaFunc type=\"%s\" base=\"%.3f\" ampl=\"%.3f\" phase=\"%.3f\" freq=\"%.3f\" />\n",
				core::FuncTypeName[func.func], func.base, func.amplitude, func.phase, func.frequency);
			out_buf.append(tmp);
		}

		out_buf.append("</Pass>\n");
	}

	out_buf.append("</Material>\n");
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
