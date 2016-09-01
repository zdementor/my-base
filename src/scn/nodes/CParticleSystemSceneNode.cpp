//|-------------------------------------------------------------------------
//| File:        CParticleSystemSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CParticleSystemSceneNode.h"

#include <scn/ISceneManager.h>
#include <scn/ICameraSceneNode.h>
#include <vid/IVideoDriver.h>
#include <os/ITimer.h>
#include <scn/ICullingSystem.h>
#include <res/IResourceManager.h>
#include <scn/IParticleAffector.h>

#include "CParticleEmitterBox.h"
#include "CParticleEmitterPoint.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CParticleSystemSceneNode::CParticleSystemSceneNode(ISceneNode* parent, s32 id)
	: IParticleSystemSceneNode(ESNT_PARTICLE_SYSTEM_SCENE_NODE, parent, id),
	m_LastVisibility(true), m_TimeShift(0), m_JustShowedUp(false),
	m_ParticlesCount(0), m_Enabled(true), m_LastEmitTime(0),
	m_Material(getMaterial(m_Materials.push_back(vid::SMaterial())))
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CParticleSystemSceneNode");
#endif
	m_Material.getPass(0).setDepthTest(vid::ECT_ALWAYS);
	m_Material.getPass(0).setAlphaTest(vid::EAT_ALWAYS);
    m_Material.getPass(0).setLightingMode(vid::ELM_NONE);
	m_Material.getPass(0).setFlag(vid::EMF_BLENDING, true);
	m_Material.getPass(0).setBlendFuncs(
		vid::ESBF_ONE, vid::EDBF_ONE_MINUS_SRC_COLOR);

	const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE_WHITE);

	m_Material.getPass(0).Layers[0].setTexture(
		VIDEO_DRIVER.getTexture(tname));
}

//---------------------------------------------------------------------------

CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
    _removeAllEmitters();  
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::OnCheckCulling()
{     
	if (!m_CullingChecked)
	{
		m_Culled = (IsVisible || m_ParticlesCount>0) ?
			CULLING_SYSTEM.isCull(this) : true;

		ISceneNode::OnCheckCulling();

		if ( !m_LastVisibility && !m_Culled )
			m_JustShowedUp = true; 			

		m_LastVisibility = !m_Culled; 
	} 
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::OnPreRender(u32 timeMs)
{
	scn::ISceneManager &smgr   = SCENE_MANAGER;
	vid::IVideoDriver  &driver = VIDEO_DRIVER;    

	ICameraSceneNode* camera = smgr.getActiveCamera();

	if (!camera)
		return;

	bool dbg_vis = isDebugDataVisible();

	// calculate vectors for letting particles look to camera
	core::vector3df pos = getAbsolutePosition();

	core::vector3df campos	= camera->getAbsolutePosition();
	core::vector3df target	= camera->getTarget();
	core::vector3df up		= camera->getUpVector();

	core::vector3df view = target-campos;
	view.normalize();
	core::vector3df horizontal = up.getCrossProduct(view);
	horizontal.normalize();
	core::vector3df vertical = horizontal.getCrossProduct(view);
	vertical.normalize();

	if (m_JustShowedUp) 
	{
		m_TimeShift = timeMs - m_LastEmitTime;	
		m_JustShowedUp = false;
		m_LastEmitTime=0;
	}

	// animate particle syatem
	_burn(timeMs-m_TimeShift, horizontal, vertical);

	core::list<IParticleEmitter*>::iterator it = m_EmitterList.begin();
	for ( ; it != m_EmitterList.end(); ++it )
	{
		IParticleEmitter* emitter = (*it);	

		vid::ITexture *tex = emitter->getTexture();

		m_Material.getPass(0).Layers[0].setTexture(tex);

		driver.registerGeometryForRendering(vid::ERP_3D_TRANSP_2ND_PASS,
			core::matrix4(), NULL, NULL, m_Material,
			emitter->getVertices(), emitter->getParticlesCount()*4, vid::EVT_1TCOORDS_COLOURED,
			emitter->getIndices(), emitter->getParticlesCount()*2 *3, EIT_16_BIT,
			vid::EDPT_TRIANGLE_LIST);

		if (dbg_vis && emitter->isEnabled())
		{
			core::vector3df dir_0 = emitter->getAppearVolume().getCenter();
			core::vector3df dir_1;
			getAbsoluteTransformation().transformVect(dir_0);
			dir_1 = dir_0 + emitter->getDirection() * 300.0f;
			driver.register3DBoxForRendering(getAbsoluteTransformation(),
				emitter->getAppearVolume(), 0xffffff00);
			driver.register3DLineForRendering(core::matrix4(),
				dir_0, dir_1, 0xff00ff00);
		}
	}

	if (dbg_vis)
	{
		driver.register3DBoxForRendering(getAbsoluteTransformation(),
			getBoundingBox(), 0xffffffff);
	}

	ISceneNode::OnPreRender(timeMs);        
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::OnPostRender(u32 timeMs)
{
	m_TransformedBoundingBox = getBoundingBox();
	getAbsoluteTransformation().transformBox(m_TransformedBoundingBox);

	ISceneNode::OnPostRender(timeMs);
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::_burn(u32 timeMs,
	const core::vector3df& horiz_normalized, 
	const core::vector3df& vert_normalized)
{
	u32 now = timeMs;

	s32 timediff = (m_Enabled&&IsVisible&&m_LastEmitTime)?(now - m_LastEmitTime):0;
    m_LastEmitTime = now;	

    // run emitters

	while (timediff >= 0)
	{
		const static s32 time_min_step = 30;
		u32 time_step = timediff>time_min_step?time_min_step:timediff;

		m_ParticlesCount = 0;

		m_Box.reset(getAbsolutePosition());
		
		core::list<IParticleEmitter*>::iterator it = m_EmitterList.begin();
		for (; it!=m_EmitterList.end(); ++it)
		{
			IParticleEmitter* emitter = (*it);	

			core::matrix4 transformation;
			if (emitter->isGlobalParticlesTransformation())
				transformation = AbsoluteTransformation;

			emitter->emitt(now, time_step, transformation, horiz_normalized, vert_normalized);
			m_Box.addInternalBox( emitter->getBoundingBox() );

			m_ParticlesCount += emitter->getParticlesCount();
		}

		now += time_step;
		timediff-=time_min_step;
	}

	// transform bbox from global coord to local

	core::matrix4 invabs = AbsoluteTransformation;
	invabs.makeInversed();

	invabs.transformBox(m_Box);
}

//---------------------------------------------------------------------------

bool CParticleSystemSceneNode::set(ISceneNode *other)
{
	IParticleSystemSceneNode* ps =
		SAFE_CAST_TO_PARTICLE_SYSTEM_SCENE_NODE(other);
	if (!ps || this == other)
		return false;
	_removeAllEmitters();
	for (s32 i=0; i<ps->getEmittersCount(); i++)
	{
		IParticleEmitter* emitter = ps->getEmitter(i);
		addEmitter(emitter->getType())->set(emitter);
	}
	return true;
}

//---------------------------------------------------------------------------

IParticleEmitter* CParticleSystemSceneNode::addEmitter(E_PARTICLE_EMITTER_TYPE etype)
{
	IParticleEmitter *emitter = 0;
	
	core::aabbox3df box(-7.f, 0.f, -7.f, 7.f, 1.f, 7.f);
	core::vector3df direction(0.00f,0.03f,0.00f);
	u32 minParticlesPerSecond = 100;
	u32 maxParticlePerSecond = 200;
	img::SColor minStartColor(255,127,0,0);
	img::SColor maxStartColor(255,0,127,0);
	u32 lifeTimeMin = 4200, lifeTimeMax = 5800;
	s32 maxAngleDegrees = 180;

	if (etype == EPET_POINT)
	{
		emitter = new CParticlePointEmitter(
			direction, minParticlesPerSecond,
			maxParticlePerSecond, minStartColor, maxStartColor,
			lifeTimeMin, lifeTimeMax, maxAngleDegrees);
	}
	else 
	if (etype == EPET_BOX)
	{
		emitter = new CParticleBoxEmitter(
			box, direction, minParticlesPerSecond,
			maxParticlePerSecond, minStartColor, maxStartColor, 
			lifeTimeMin, lifeTimeMax, maxAngleDegrees);
	}

	if (emitter)
	{
		emitter->setParticleSize(core::dimension2df(10.f, 10.f));
		m_EmitterList.push_back(emitter);
	}
	
	m_LastEmitTime = TIMER.getTime();

	return emitter;
}

//---------------------------------------------------------------------------

bool CParticleSystemSceneNode::removeEmitter(s32 idx)
{
	if (idx < 0 || idx>=(s32)m_EmitterList.size())
		return false;

	core::list<IParticleEmitter*>::iterator it = m_EmitterList.begin() + idx;

	(*it)->drop();
	m_EmitterList.erase(it);

	return true;
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::_removeAllEmitters()
{
	core::list<IParticleEmitter*>::iterator it = m_EmitterList.begin();
	while (it != m_EmitterList.end())
	{
		(*it)->drop();
		it = m_EmitterList.erase(it);
	}
}

//---------------------------------------------------------------------------

IParticleEmitter* CParticleSystemSceneNode::getEmitter(s32 idx)
{
	return *(m_EmitterList.begin() + idx);  
}

//---------------------------------------------------------------------------

s32 CParticleSystemSceneNode::getEmittersCount()
{
	return m_EmitterList.size();
}

//---------------------------------------------------------------------------

void CParticleSystemSceneNode::setEnabled(bool enabled)
{
	m_Enabled = enabled;
}

//---------------------------------------------------------------------------

s32 CParticleSystemSceneNode::getParticlesCount()
{
	return m_ParticlesCount;
}

//---------------------------------------------------------------------------

const c8* CParticleSystemSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0), false);
		xmlstr.append(mat_xml);
	}

	core::list<IParticleEmitter*>::iterator eit = m_EmitterList.begin();
	for (; eit!=m_EmitterList.end(); ++eit)
	{
		IParticleEmitter* emitter = *eit;

		str.sprintf(
			"<Emitter type=\"%s\" enabled=\"%d\" global_transform=\"%d\" >\n", 
			ParticleEmitterTypeStr[emitter->getType()], emitter->isEnabled(), emitter->isGlobalParticlesTransformation()
			);
		xmlstr.append(str);

		vid::ITexture* texture = emitter->getTexture();		
		core::stringc fullfname = VIDEO_DRIVER.findTextureName(texture);
		core::stringc fname =  core::extractFileName(fullfname);
		core::stringc fpath =  core::extractFilePath(fullfname);
		core::stringc texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
		core::stringc relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
		core::stringc relfullfname;
		relfullfname.append(relpath.c_str());
		relfullfname.append(fname.c_str());
	
		str.sprintf(
			"<Texture filename=\"%s\" />\n", 
			relfullfname.c_str()
			);
		xmlstr.append(str);

		str.sprintf(
			"<ParticleSize width=\"%.f\" height=\"%.f\" />\n", 
			emitter->getParticleSize().Width, emitter->getParticleSize().Height
			);
		xmlstr.append(str);

		core::aabbox3df volume = emitter->getAppearVolume();
		str.sprintf(
			"<AppearVolume xmin=\"%.f\" ymin=\"%.f\" zmin=\"%.f\" xmax=\"%.f\" ymax=\"%.f\" zmax=\"%.f\" />\n", 
			volume.MinEdge.X, volume.MinEdge.Y, volume.MinEdge.Z,
			volume.MaxEdge.X, volume.MaxEdge.Y, volume.MaxEdge.Z
			);
		xmlstr.append(str);

		core::vector3df dir = emitter->getDirection();
		str.sprintf(
			"<ParticleDirection x=\"%.4f\" y=\"%.4f\" z=\"%.4f\" />\n", 
			dir.X, dir.Y, dir.Z
			);
		xmlstr.append(str);

		str.sprintf(
			"<ParticlesPerSecond min=\"%d\" max=\"%d\" />\n", 
			emitter->getMinParticlePerSecond(), emitter->getMaxParticlePerSecond()
			);
		xmlstr.append(str);

		str.sprintf(
			"<LifeTime min=\"%d\" max=\"%d\" />\n", 
			emitter->getMinLifeTime(), emitter->getMaxLifeTime()
			);
		xmlstr.append(str);

		img::SColor c = emitter->getMinStartColor();
		str.sprintf(
			"<MinStartColor red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" />\n", 
			c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()						
			);
		xmlstr.append(str);

		c = emitter->getMaxStartColor();
		str.sprintf(
			"<MaxStartColor red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" />\n", 
			c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()					
			);
		xmlstr.append(str);

		str.sprintf(
			"<AngleDegrees value=\"%d\" />\n", 
			emitter->getMaxAngleDegrees()				
			);
		xmlstr.append(str);

		core::list<IParticleAffector*>& aff_list = emitter->getAffectorsList();
		core::list<IParticleAffector*>::iterator ait = aff_list.begin();
		for (; ait!=aff_list.end(); ++ait)
		{
			IParticleAffector* affector = *ait;

			str.sprintf(
				"<Affector type=\"%s\" enabled=\"%d\" affect_time_ms=\"%d\" >\n", 
				ParticleAffectorTypeStr[affector->getType()], 
				affector->isEnabled(),
				affector->getAffectTimeMs()
				);
			xmlstr.append(str);

			if (affector->getType()==EPAT_FADEOUT)
			{
				img::SColor tgtc = ((IParticleFadeOutAffector*)affector)->getTargetColor();

				str.sprintf(
					"<TargetColor red=\"%d\" green=\"%d\" blue=\"%d\" alpha=\"%d\" />\n", 
					tgtc.getRed(), tgtc.getGreen(), tgtc.getBlue(), tgtc.getAlpha()					
					);
				xmlstr.append(str);
			}
			else
			if (affector->getType()==EPAT_GRAVITY)
			{
				core::vector3df gravity = ((IParticleGravityAffector*)affector)->getGravity();

				str.sprintf(
					"<GravityValue x=\"%.4f\" y=\"%.4f\" x=\"%.4f\" />\n", 
					gravity.X, gravity.Y, gravity.Z				
					);
				xmlstr.append(str);
			}

			xmlstr.append("</Affector>\n");
		}
		
		xmlstr.append("</Emitter>\n");
	}

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------


