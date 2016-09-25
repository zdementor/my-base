//|-------------------------------------------------------------------------
//| File:        CLensFlareSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CLensFlareSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <game/IGameManager.h>
#include <scn/ICameraSceneNode.h>
#include <dyn/IDynamicManager.h>
#include <dev/IDevice.h>
#include <io/IInputDispatcher.h>
#include <scn/ICullingSystem.h>
#include <res/IResourceManager.h>

//--------------------------------------------------------------------------------
namespace my{
namespace scn{
//--------------------------------------------------------------------------------

core::queue<CLensFlareSceneNode*> CLensFlareSceneNode::m_LensCullQueue;
bool CLensFlareSceneNode::m_VisibilityCalc = false;

//--------------------------------------------------------------------------------

CLensFlareSceneNode::CLensFlareSceneNode(ISceneNode* parent, s32 id) 
	: ILensFlareSceneNode(ESNT_LENS_FLARE_SCENE_NODE, parent, id), 
	m_CurrentCullStatus(false), 
	m_VisibilityStatus(ELVS_INVISIBLE), m_Visibility(0.0f), m_PrevTimeMs(0),
	m_Material(getMaterial(m_Materials.push_back(vid::SMaterial())))
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CLensFlareSceneNode");
#endif

	const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE_WHITE);

	m_Corona = VIDEO_DRIVER.getTexture(tname);

	m_ScreenHeight = (f32) VIDEO_DRIVER.getScreenSize().Height;
	m_ScreenWidth  = (f32) VIDEO_DRIVER.getScreenSize().Width; 

	m_StartPos.set(m_ScreenWidth / 2.0f, m_ScreenHeight / 2.0f); 

	setEtalonScreenSize(core::dimension2d<s32>(1024,768));

	m_Material.getPass(0).setDepthTest(vid::ECT_ALWAYS);
	m_Material.getPass(0).setAlphaTest(vid::EAT_ALWAYS);
    m_Material.getPass(0).setLightingMode(vid::ELM_NONE);
	m_Material.getPass(0).setFlag(vid::EMF_BLENDING, true);
	m_Material.getPass(0).setBlendFuncs(
		vid::ESBF_ONE, vid::EDBF_ONE_MINUS_SRC_COLOR);
	m_Material.getPass(0).Layers[0].setTexture(m_Corona);
}

//--------------------------------------------------------------------------------

CLensFlareSceneNode::~CLensFlareSceneNode()
{
	m_Lens.clear();

	m_LensCullQueue.erase(this);

	SAFE_DROP(m_Corona);
}

//---------------------------------------------------------------------------

void CLensFlareSceneNode::OnBeforeCulling()
{
	m_VisibilityCalc=false;

	ISceneNode::OnBeforeCulling();
}

//---------------------------------------------------------------------------

void CLensFlareSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		if (IsVisible )
		{
			m_PosNode = getAbsolutePosition(); 	

			if ( CULLING_SYSTEM.isCull (
					m_PosNode, E_CULLING_TESTS(
						ECT_SCREEN_BOUNDS_CULL |
						//ECT_VIEW_FRUSTRUM_CULL |
						ECT_OCCLUDERS_BOTTOM_VOLUME_CULL |
						ECT_OCCLUDERS_FRUSTRUM_CULL
						) )	)
			// basic culling tests
			{
				m_LensCullQueue.erase(this);

				m_CurrentCullStatus = true;
				m_Visibility=0.0f;			
			}
			else
			// other cull checks (more heavily)
			{
				m_LensCullQueue.push_back(this);

				if (!m_VisibilityCalc)
				{
					if (m_LensCullQueue.is_ready(this))
					{
						m_LensCullQueue.switch_next();

						if (CULLING_SYSTEM.isCull(
								m_PosNode, E_CULLING_TESTS(ECT_DYNAMIC_MANAGER_CULL)))
						{
							m_CurrentCullStatus = true;
							m_Visibility=0.0f;
						}
						else
						{
							m_CurrentCullStatus = false; 							
						}

						m_CurrentCullStatus = false;
					}				
				}
			}
		}   
		else
		{
			m_LensCullQueue.erase(this);

			m_CurrentCullStatus = true;
			m_Visibility=0.0f;
		}

		m_Culled = m_CurrentCullStatus;

		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CLensFlareSceneNode::OnPreRender(u32 timeMs)
{
	ICameraSceneNode  *camera = SCENE_MANAGER.getActiveCamera();    

	if (!camera) return;  

	m_CurrentLensColor = m_Material.getPass(0).getDiffuseColor();

	if (m_Visibility < 1.0f)
	{
		s32 delta_ms = m_PrevTimeMs?(timeMs-m_PrevTimeMs):0;

		m_Visibility += 0.0005f * delta_ms;
		CHECK_MAX_RANGE(m_Visibility,1.0f);

		m_CurrentLensColor.setRed  (m_Visibility*m_CurrentLensColor.getRed  ()); 
		m_CurrentLensColor.setBlue (m_Visibility*m_CurrentLensColor.getBlue ()); 
		m_CurrentLensColor.setGreen(m_Visibility*m_CurrentLensColor.getGreen()); 
	}

	m_Material.getPass(0).setLightingMode(vid::ELM_NONE);
	m_Material.getPass(0).setFlag(vid::EMF_BLENDING, true);
	m_Material.getPass(0).setBlendFuncs(
		vid::ESBF_ONE, vid::EDBF_ONE_MINUS_SRC_COLOR);

	m_PrevTimeMs = timeMs;

	vid::IVideoDriver &driver = VIDEO_DRIVER;	

	m_ScreenPosition =
		driver.getScreenCoordinatesFrom3DPosition(m_PosNode);

	// draw corona
    _registerLensForRendering (	m_Corona,
		core::position2df ( m_ScreenPosition.X, m_ScreenPosition.Y ),
		m_Scale2D );

	// camera position
	core::vector3df cam_tgt = camera->getTarget();
	core::vector3df cam_pos = camera->getPosition();    

	// check the angle cut off  

	if ( ( (m_PosNode-cam_pos).normalize() ).getAngleDeg( (cam_tgt-cam_pos).normalize() ) > 40 )
        return;
   
    f32 dX = (f32)(m_ScreenPosition.X-m_StartPos.X);
    f32 dY = (f32)(m_ScreenPosition.Y-m_StartPos.Y);    
    
    // draw lens flares
    core::position2d<f32> drawPos;
    for (u32 i=0; i<m_Lens.size(); i++)
    {
		drawPos = core::position2d<f32>(
			(f32)m_StartPos.X + (f32)dX * m_Lens[i].spaceConst, 
            (f32)m_StartPos.Y + (f32)dY * m_Lens[i].spaceConst);
		_registerLensForRendering ( m_Lens[i].getTexture(), drawPos, m_Scale2D);
    }

	if (isDebugDataVisible())
	{
		driver.register3DBoxForRendering(
			vid::ERP_3D_SOLID_PASS, getAbsoluteTransformation(),
			getBoundingBox(), 0xffffffff,
			true);
	}

	ISceneNode::OnPreRender(timeMs);
}

//--------------------------------------------------------------------------------

void CLensFlareSceneNode::_registerLensForRendering(vid::ITexture *tex,
	const core::position2df &drawPos, const core::dimension2df &scale)
{  
	core::dimension2df drawSize(32.0f, 32.0f);
    if (tex)
	{
		const core::dimension2di & sz = tex->getOriginalSize();
		drawSize.set(sz.Width, sz.Height);
	}
	drawSize.Width *= 0.5f * scale.Width;
	drawSize.Height *= 0.5f * scale.Height;

	m_Material.getPass(0).Layers[0].setTexture(tex);

	core::rectf drawRect((drawPos.X - drawSize.Width), (drawPos.Y - drawSize.Width),
			(s32)(drawPos.X + drawSize.Width), (s32)(drawPos.Y + drawSize.Height));
	core::rectf texRect(0.0f, 0.0f, 1.0f, 1.0f);

	VIDEO_DRIVER.register2DImageForRendering(tex,
		drawRect, texRect, m_CurrentLensColor);
}

//--------------------------------------------------------------------------------

bool CLensFlareSceneNode::setLens(u32 idx, const SLens &lens)
{
	if (idx<0||idx>=m_Lens.size()) 
		return false;
	m_Lens[idx] = lens;
	return true;
}

//--------------------------------------------------------------------------------

bool CLensFlareSceneNode::setLens(u32 idx, vid::ITexture *tex, f32 space)
{
	if (idx < 0 || idx >= m_Lens.size()) 
		return false;
	m_Lens[idx].setTexture(tex);
	m_Lens[idx].spaceConst = space;
	return true;
}

//--------------------------------------------------------------------------------

const SLens& CLensFlareSceneNode::getLens(u32 idx) const
{
	if (idx<0||idx>=m_Lens.size()) 
		return *((SLens*)0);
	return m_Lens[idx];
}
//--------------------------------------------------------------------------------

void CLensFlareSceneNode::setCorona(vid::ITexture *t)
{
	SAFE_DROP(m_Corona);
	m_Corona = t;
	SAFE_GRAB(m_Corona);
}

//--------------------------------------------------------------------------------

void CLensFlareSceneNode::setEtalonScreenSize(const core::dimension2di &scrSize)
{
	setEtalonScreenSize(scrSize.Width, scrSize.Height);
}

//--------------------------------------------------------------------------------

void CLensFlareSceneNode::setEtalonScreenSize(u32 scrWidth, u32 scrHeight)
{
	m_EtalonScreenSize.set(scrWidth, scrHeight);

	// setting scale for lens images rendering
	// (we want they are have same looking in all screen resolutions)
	f32 scalex = (f32)m_ScreenWidth / (f32)m_EtalonScreenSize.Width;
	f32 scaley = (f32)m_ScreenHeight / (f32)m_EtalonScreenSize.Height;		
	
	m_Scale2D = core::dimension2d<f32>(scalex>0?scalex:1.0f, scaley>0?scaley:1.0f);
}

//---------------------------------------------------------------------------

bool CLensFlareSceneNode::set(ISceneNode* other)
{
	if (!other || this == other 
			|| other->getSceneNodeType()!=ESNT_LENS_FLARE_SCENE_NODE)
		return false;

	ILensFlareSceneNode *other_lens = (ILensFlareSceneNode*)other;

	setCorona(other_lens->getCorona());

	for  (u32 l=0; l<other_lens->getLensCount(); l++)
	{
		SLens single_lens = other_lens->getLens(l);
		addLens(single_lens);	
	}
	
	setEtalonScreenSize( other_lens->getEtalonScreenSize() );

	return true;
}

//---------------------------------------------------------------------------

//!
const c8* CLensFlareSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0), false);
		xmlstr.append(mat_xml);
	}

    core::stringc fullfname = 
		VIDEO_DRIVER.findTextureName(m_Corona);
    core::stringc fname =  core::extractFileName(fullfname);
    core::stringc fpath =  core::extractFilePath(fullfname);
    core::stringc texpath = 
		RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
    core::stringc relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
    core::stringc relfullfname = "";
	relfullfname.sprintf("%s%s", relpath.c_str(), fname.c_str());

	str.sprintf("<Corona filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);	

	for (u32 i=0; i<m_Lens.size(); i++)
	{
		fullfname = VIDEO_DRIVER.findTextureName(m_Lens[i].getTexture());
		fname =  core::extractFileName(fullfname);
		fpath =  core::extractFilePath(fullfname);
		texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
		relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
		relfullfname.sprintf("%s%s", relpath.c_str(), fname.c_str());

		str.sprintf("<Lens filename=\"%s\" space=\"%.3f\"/>\n",
			relfullfname.c_str(), m_Lens[i].spaceConst);
		xmlstr.append(str);
	}

	str.sprintf("<EtalonScreen width=\"%d\" height=\"%d\" />\n",
		m_EtalonScreenSize.Width, m_EtalonScreenSize.Height);
	xmlstr.append(str);	

	return xmlstr.c_str();
}

//--------------------------------------------------------------------------------
} // namespace scn
} // namespace my
//--------------------------------------------------------------------------------