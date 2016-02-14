//|-------------------------------------------------------------------------
//| File:        CLightSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CLightSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <scn/ICullingSystem.h>

#include "../CGeometryCreator.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

IMesh* CLightSceneNode::LightSphereMesh = NULL;

//---------------------------------------------------------------------------

CLightSceneNode::CLightSceneNode( ISceneNode* parent, s32 id, vid::SLight lightData )
	: ILightSceneNode(ESNT_LIGHT_SCENE_NODE, parent, id),
	m_LightedVolumeCenterOffset(core::vector3df(0,0,0)), 
	m_LightedVolumeSizeChange(core::vector3df(0,0,0))
{
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CLightSceneNode");
#endif
    
    setLightParameters(lightData);

	m_RenderPass.setLightingMode(vid::ELM_NONE);
    m_RenderPass.setFlag ( vid::EMF_FOG_ENABLE,			false	);
    m_RenderPass.setFlag ( vid::EMF_ZWRITE_ENABLE,		true	);
	m_RenderPass.setFlag ( vid::EMF_BACK_FACE_CULLING,	false	);
	m_RenderPass.setDiffuseColor ( img::SColor(100, 50, 50, 50) );

    if (LightSphereMesh==NULL)
    {
		IAnimatedMesh *amesh = SCENE_MANAGER.getMesh("#sphere_mesh");
		if (amesh)
			LightSphereMesh = amesh->getMesh(); 
    }
}

//---------------------------------------------------------------------------

CLightSceneNode::~CLightSceneNode()
{
}

//---------------------------------------------------------------------------

void CLightSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		m_Culled = !IsVisible || CULLING_SYSTEM.isCull(this);
		
		ISceneNode::OnCheckCulling();
	}  	
}

//---------------------------------------------------------------------------

void CLightSceneNode::OnPreRender(u32 timeMs)
{
    scn::ISceneManager &smgr = SCENE_MANAGER;
	vid::IVideoDriver  &driver = VIDEO_DRIVER;

	vid::SLight &light = CurrentLightData;

	LightData.ScreenPosition =
		driver.getScreenCoordinatesFrom3DPosition(LightData.Position);
	LightData.ScreenRect =
		driver.getScreenRectFrom3DVolume(LightData.LightedVolume);

	light = LightData;

	if (light.IntensityWaveEnabled)
	{
		f32 intens = light.IntensityWave.get_value(timeMs);

		CHECK_RANGE(intens, 0, 1);

		light.DiffuseColor  = light.DiffuseColor * intens;
		light.AmbientColor  = light.AmbientColor * intens;
		light.SpecularColor = light.SpecularColor * intens;
	}

	if (CurrentLightData.Enabled)
		driver.addDynamicLight(CurrentLightData);

	bool drawDebug = isDebugDataVisible();
	bool drawLight = drawDebug || smgr.getSceneRenderFlag(ESRF_RENDER_LIGHTS);

	if (drawLight || drawDebug)
	{
		if (drawDebug)
		{
			driver.register3DBoxForRendering(getAbsoluteTransformation(),
				getBoundingBox(), img::SColor(0xffffffff));

			driver.register3DBoxForRendering(core::matrix4(),
				LightData.LightedVolume, img::SColor(0xffffff00));	
		}

		if (LightSphereMesh)
		{
			u32 mbcnt = LightSphereMesh->getMeshBuffersCount();
			for (u32 mbi = 0; mbi < mbcnt; mbi++)
			{
				scn::IMeshBuffer *mb = LightSphereMesh->getMeshBuffer(mbi);

				vid::E_VERTEX_TYPE vtype = mb->getVertexType();
				void *verts = mb->getVertices();

				core::matrix4 scale;
				vid::SMaterial m;

				m.getPass(0).setLightingMode(vid::ELM_NONE);

				if (drawDebug)
				{
					scale.setScale(core::vector3df(LightData.Radius, LightData.Radius, LightData.Radius) * 2.f );	

					m.getPass(0).setFlag(vid::EMF_BLENDING, true);
					m.getPass(0).setBlendFuncs(vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
					m.getPass(0).setEmissiveColor(img::SColor(0xffffffcc));
					m.getPass(0).setDiffuseColor(img::SColor(0x11000000));
					m.getPass(0).setColorGen(vid::ECGT_CUSTOM);
					m.getPass(0).setFlag(vid::EMF_FRONT_FACE_CCW, false);

					driver.registerGeometryForRendering(
						vid::ERP_3D_TRANSP_1ST_PASS,
						getAbsoluteTransformation() * scale, NULL, 0,
						m,
						mb->getVertices(), mb->getVertexCount(),  mb->getVertexType(),
						mb->getIndices(), mb->getIndexCount(),  mb->getIndexType(),
						mb->getDrawPrimitiveType());

					m.getPass(0).setFlag(vid::EMF_FRONT_FACE_CCW, true);

					driver.registerGeometryForRendering(
						vid::ERP_3D_TRANSP_1ST_PASS,
						getAbsoluteTransformation() * scale, NULL, 0,
						m,
						mb->getVertices(), mb->getVertexCount(),  mb->getVertexType(),
						mb->getIndices(), mb->getIndexCount(),  mb->getIndexType(),
						mb->getDrawPrimitiveType());
				}

				scale.setScale(core::vector3df(25.f, 25.f, 25.f));
				m.getPass(0).setColorGen(vid::ECGT_CUSTOM);
				m.getPass(0).setEmissiveColor(img::SColor(0xffffff99));
				m.getPass(0).setDiffuseColor(img::SColor(0xff000000));
				m.getPass(0).setFlag(vid::EMF_BLENDING, true);
				m.getPass(0).setBlendFuncs(vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
				m.getPass(0).setFlag(vid::EMF_FRONT_FACE_CCW, false);
				driver.registerGeometryForRendering(
					vid::ERP_3D_SOLID_PASS,
					getAbsoluteTransformation() * scale, NULL, 0,
					m,
					mb->getVertices(), mb->getVertexCount(),  mb->getVertexType(),
					mb->getIndices(), mb->getIndexCount(),  mb->getIndexType(),
					mb->getDrawPrimitiveType());

				scale.setScale(core::vector3df(20.f, 20.f, 20.f));
				m.getPass(0).setColorGen(vid::ECGT_CUSTOM);
				m.getPass(0).setEmissiveColor(LightData.DiffuseColor.toSColor());
				m.getPass(0).setDiffuseColor(img::SColor(0xff000000));
				m.getPass(0).setFlag(vid::EMF_BLENDING, true);
				m.getPass(0).setBlendFuncs(vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
				m.getPass(0).setFlag(vid::EMF_FRONT_FACE_CCW, true);
				driver.registerGeometryForRendering(
					vid::ERP_3D_SOLID_PASS,
					getAbsoluteTransformation() * scale, NULL, 0,
					m,
					mb->getVertices(), mb->getVertexCount(),  mb->getVertexType(),
					mb->getIndices(), mb->getIndexCount(),  mb->getIndexType(),
					mb->getDrawPrimitiveType());
			}
		}
	}

    ISceneNode::OnPreRender(timeMs);	
}

//------------------------------------------------------------------------  

void CLightSceneNode::updateAbsoluteTransformation()
{	
	vid::IVideoDriver  &driver = VIDEO_DRIVER;

	bool transf_changed = m_TransformationChanged || m_ParentTransformationChanged;

	ISceneNode::updateAbsoluteTransformation();

	if (transf_changed)
		_updateLightData();
}

//---------------------------------------------------------------------------

//! updating light data
void CLightSceneNode::_updateLightData()
{
	vid::IVideoDriver  &driver = VIDEO_DRIVER;

	LightData.Position = getAbsolutePosition();

	core::vector3df r(LightData.Radius,LightData.Radius,LightData.Radius);

	LightData.BoundingBox.reset(LightData.Position);
	LightData.BoundingBox.addInternalPoint(LightData.Position+r);
	LightData.BoundingBox.addInternalPoint(LightData.Position-r);

	LightData.LightedVolumeCenter = LightData.Position;
	LightData.LightedVolumeSize   = BBox.getExtend();

	if (m_LightedVolumeCenterOffset != core::vector3df(0,0,0)||
		m_LightedVolumeSizeChange != core::vector3df(0,0,0))
	{	
		LightData.LightedVolumeCenter += m_LightedVolumeCenterOffset;
		LightData.LightedVolumeSize   += m_LightedVolumeSizeChange;

		f32 sx2 = LightData.LightedVolumeSize.X/2.0f;
		f32 sy2 = LightData.LightedVolumeSize.Y/2.0f;
		f32 sz2 = LightData.LightedVolumeSize.Z/2.0f;

		LightData.LightedVolume.MinEdge.set(
			-sx2 + LightData.LightedVolumeCenter.X,
			-sy2 + LightData.LightedVolumeCenter.Y,
			-sz2 + LightData.LightedVolumeCenter.Z
			);
		LightData.LightedVolume.MaxEdge.set(
			sx2 + LightData.LightedVolumeCenter.X,
			sy2 + LightData.LightedVolumeCenter.Y,
			sz2 + LightData.LightedVolumeCenter.Z
			);
	}
	else
	{
		LightData.LightedVolume = LightData.BoundingBox;
	}
}

//---------------------------------------------------------------------------

const vid::SLight& CLightSceneNode::getLightParameters() const
{
    return LightData;
}

//---------------------------------------------------------------------------

void CLightSceneNode::setLightParameters(const vid::SLight &lparams)
{
	core::vector3df pos = LightData.Position;

    LightData = lparams;

    LightData.Identificator = this;
	LightData.Position = pos;

	f32 r = LightData.Radius;

	BBox = core::aabbox3d<f32>(	-r, -r, -r, r, r, r	);

	_updateLightData();
}

//---------------------------------------------------------------------------

const core::vector3df& CLightSceneNode::getLightedVolumeCenterOffset()
{
	return m_LightedVolumeCenterOffset;
}

//---------------------------------------------------------------------------

void CLightSceneNode::setLightedVolumeCenterOffset(const core::vector3df& offset)
{
	m_LightedVolumeCenterOffset = offset;

	_updateLightData();
}

//---------------------------------------------------------------------------

const core::vector3df& CLightSceneNode::getLightedVolumeSizeChange()
{
	return m_LightedVolumeSizeChange;
}

//---------------------------------------------------------------------------

void CLightSceneNode::setLightedVolumeSizeChange(const core::vector3df& change)
{
	m_LightedVolumeSizeChange = change;

	_updateLightData();
}

//---------------------------------------------------------------------------

const core::aabbox3d<f32>& CLightSceneNode::getBoundingBox() const
{
    return BBox;
}

//---------------------------------------------------------------------------

bool CLightSceneNode::set(ISceneNode* other)
{
	if (!other|| this == other 
			|| other->getSceneNodeType()!=ESNT_LIGHT_SCENE_NODE)
	{
		return false;
	}

	ILightSceneNode *other_light = (ILightSceneNode*)other;

	vid::SLight lparams = other_light->getLightParameters();
	core::vector3df lvol_center_offset = other_light->getLightedVolumeCenterOffset();
	core::vector3df lvol_size_change   = other_light->getLightedVolumeSizeChange();

	setLightParameters(lparams);
	setLightedVolumeCenterOffset(lvol_center_offset);
	setLightedVolumeSizeChange(lvol_size_change);

	return true;
}

//---------------------------------------------------------------------------

const c8* CLightSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0));
		xmlstr.append(mat_xml);
	}

	str.sprintf( 
		"<LightParameters enabled=\"%d\" cast_shadows=\"%d\" radius=\"%.f\" type=\"%s\" dynamic=\"%d\" >\n", 
		LightData.Enabled?1:0, LightData.CastShadows?1:0, 
		LightData.Radius, vid::LightTypeName[LightData.Type], LightData.Dynamic?1:0);
	xmlstr.append(str);

	str.sprintf( 
		"<DiffuseColorF red=\"%.2f\" green=\"%.2f\" blue=\"%.2f\" alpha=\"%.2f\" />\n", 
		LightData.DiffuseColor.r, LightData.DiffuseColor.g, LightData.DiffuseColor.b, LightData.DiffuseColor.a
		);
	xmlstr.append(str);

	str.sprintf( 
		"<AmbientColorF red=\"%.2f\" green=\"%.2f\" blue=\"%.2f\" alpha=\"%.2f\" />\n", 
		LightData.AmbientColor.r, LightData.AmbientColor.g, LightData.AmbientColor.b, LightData.AmbientColor.a
		);
	xmlstr.append(str);

	str.sprintf( 
		"<SpecularColorF red=\"%.2f\" green=\"%.2f\" blue=\"%.2f\" alpha=\"%.2f\" />\n", 
		LightData.SpecularColor.r, LightData.SpecularColor.g, LightData.SpecularColor.b, LightData.SpecularColor.a
		);
	xmlstr.append(str);

	str.sprintf( 
		"<IntensityWave enabled=\"%d\" type=\"%s\" base=\"%.4f\" ampl=\"%.4f\" phase=\"%.4f\" freq=\"%.4f\" />\n", 
		LightData.IntensityWaveEnabled?1:0, 
		core::getFuncTypeName(LightData.IntensityWave.func), 
		LightData.IntensityWave.base,
		LightData.IntensityWave.amplitude,
		LightData.IntensityWave.phase,
		LightData.IntensityWave.frequency
		);
	xmlstr.append(str);

	str.sprintf("</LightParameters>\n");
	xmlstr.append(str);

	str.sprintf( 
		"<LightedVolumeCenterOffset value=\"%.2f,%.2f,%.2f\" />\n", 
		m_LightedVolumeCenterOffset.X, m_LightedVolumeCenterOffset.Y, m_LightedVolumeCenterOffset.Z
		);
	xmlstr.append(str);

	str.sprintf( 
		"<LightedVolumeSizeChange value=\"%.2f,%.2f,%.2f\" />\n", 
		m_LightedVolumeSizeChange.X, m_LightedVolumeSizeChange.Y, m_LightedVolumeSizeChange.Z
		);
	xmlstr.append(str);

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

