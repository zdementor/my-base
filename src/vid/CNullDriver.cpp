//|-------------------------------------------------------------------------
//| File:        CNullDriver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CNullDriver.h"
#include "CTextureAnim.h"
#include "CSoftwareTexture.h"
#include "CFont.h"
#include "CNullGPUProgram.h"

#ifndef __MY_BUILD_VID_NULL_LIB__
#include "CFontTTF.h"
#endif

#include <io/ILogger.h>
#include <os/ITimer.h>
#include <dev/IDevice.h>
#include <os/IEvent.h>
#include <os/IThread.h>
#include <dev/IProfiler.h>
#include <scn/ISceneManager.h>
#include <vid/IGPUProgram.h>
#include "SRenderPool.h"
#include "CNullRenderBuffer.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

#include "EmbeddedTextures.h"

const s32 CNullDriver::fog_blur_num = 30;
const s32 CNullDriver::fog_blend_times = fog_blur_num/2;
const f32 CNullDriver::fog_blur[fog_blur_num][2] = 
{
    {0.0125f, 0.0000f},
    {0.0122f, 0.0026f},
    {0.0114f, 0.0051f},
    {0.0101f, 0.0073f},  
    {0.0084f, 0.0093f},
    {0.0063f, 0.0108f},
    {0.0039f, 0.0119f},
    {0.0013f, 0.0124f},
    {-0.0013f, 0.0124f},
    {-0.0039f, 0.0119f},
    {-0.0062f, 0.0108f},
    {-0.0084f, 0.0093f},
    {-0.0101f, 0.0073f},
    {-0.0114f, 0.0051f},
    {-0.0122f, 0.0026f},
    {-0.0125f, 0.0000f},
    {-0.0122f, -0.0026f},
    {-0.0114f, -0.0051f},
    {-0.0101f, -0.0073f},
    {-0.0084f, -0.0093f},
    {-0.0063f, -0.0108f},
    {-0.0039f, -0.0119f},
    {-0.0013f, -0.0124f},
    {0.0013f, -0.0124f},
    {0.0039f, -0.0119f},
    {0.0062f, -0.0108f},
    {0.0084f, -0.0093f},
    {0.0101f, -0.0073f},
    {0.0114f, -0.0051f},
    {0.0122f, -0.0026f}
};

//---------------------------------------------------------------------------

CNullDriver::CNullDriver(const core::dimension2d<s32>& screenSize) 
	: m_RenderPath((E_RENDER_PATH)-1), m_ScreenSize(screenSize), 
	ViewPort(0,0,0,0), m_TrianglesDrawn(0), m_DIPsDrawn(0), TextureCreationFlags(0),  
	StencilPreInitialized(true), AverageFPS(0), BackColor(255,0,0,0),
	m_LastEnabledLightsCount(-1), Transformation3DChanged(true),
	m_MaxTextureUnits(0), m_MaxLights(0), m_MaxTextureSize(1024, 1024), m_MaxAnisotropyLevel(0),
	m_NormalMapCreationAmplitude(1.0f), 
	m_PolygonFillMode(EPFM_SOLID), m_TextureFilter(ETF_NONE),
	m_Rendering(false),
	m_LightSphereTexture(NULL), m_LightGradientTexture(NULL),
	m_LightSphereWhiteTexture(NULL), m_LightGradientWhiteTexture(NULL),
	m_EnvTexture(NULL), m_WhiteTexture(NULL), m_DefaultTexture(NULL),
	m_MinimalMeshBufferSizeForVBORendering(0), m_ResetRenderStates(true), m_RenderPoolsCacheIndex(0),
	m_StencilEnabled(false), m_ScissorEnabled(false), m_ShadowColor(100,0,0,0),
	m_Fullscreen(false), m_Antialiasing(false), m_VerticalSync(false), m_Shadows(false),
	m_StencilBuffer(false), m_TwoSidedStencil(false), m_TexturesNonPowerOfTwo(false),
	m_MaxDrawBuffers(1), m_DepthStencilTexturesSupport(false), m_RenderTargetSupport(false),
	m_ColorBits(0), m_AlphaBits(0), m_DepthBits(0), m_StencilBits(0),
	m_VertexShaderVersion(0), m_PixelShaderVersion(0),
	m_CurrentVertexType((E_VERTEX_TYPE)-1), 
	m_DriverType(EDT_NULL), m_Profiler(PROFILER),
	m_DirtyTexUnit(-1), m_CurrentGPUProgram(NULL),
	m_ColorMask(ECM_RED | ECM_GREEN | ECM_BLUE | ECM_ALPHA),
	m_CurrentRenderPassType(ERP_3D_SOLID_PASS), m_GenShaderMaxLights(-1),
	m_BackColorFormat((img::E_COLOR_FORMAT)-1),
	m_CurrentRenderTarget(NULL),
	m_YInverted(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CNullDriver");
#endif

	m_Profiler.grab();
	IMAGE_LIBRARY.grab();
	FILE_SYSTEM.grab();

	setRenderPath(ERP_FORWARD_RENDERING);

    setTextureCreationFlag(ETCF_AUTOGEN_MIP_MAPS, true);

    ViewPort = core::rect<s32>(core::position2d<s32>(0,0), screenSize);

	// some needed stuff

	GlobalAmbientColor = img::SColorf(img::SColor(255, 128, 128, 128));

	GlobalLight.Type = vid::ELT_DIRECTIONAL;
	GlobalLight.AmbientColor.set (0.4f, 0.4f, 0.4f);
	GlobalLight.DiffuseColor.set (0.6f, 0.6f, 0.6f);
    GlobalLight.SpecularColor.set(0.0f, 0.0f, 0.0f);
	GlobalLight.Position.set     (-1.000000, 1000.000000, -1.000000); 
    GlobalLight.CastShadows = false;
	GlobalLight.Identificator = &GlobalLight;
	GlobalLight.Enabled = true;
	GlobalLight.Radius = 10000;

	for (s32 t=0; t<MY_MATERIAL_MAX_LAYERS; t++)
		CurrentTexture[t] = NULL;

	m_ProfileRender		= m_Profiler.allocProfileId("Rendering");
	m_ProfileSortDIPs	= m_Profiler.allocProfileId("Sort DIPs");
	m_ProfileSwapBuffers= m_Profiler.allocProfileId("Swap buffers");
	for (u32 i = 0; i < E_RENDER_PASS_COUNT; i++)
		m_ProfileIds[i] = m_Profiler.allocProfileId(RenderPassName[i]);
	for (u32 i = 0; i < E_RENDER_MODE_COUNT; i++)
		m_ProfileLightingIds[i] = m_Profiler.allocProfileId(RenderModeReadableName[i]);

	core::stringc name;
	for (u32 i = 0; i < E_UNIFORM_TYPE_COUNT; i++)
	{
		name = getUniformReadableName((E_UNIFORM_TYPE)i);
		m_UniformTypes.set_value(name, (E_UNIFORM_TYPE)i);
		name += "[0]";
		m_UniformTypes.set_value(name, (E_UNIFORM_TYPE)i);
	}

	m_Fullscreen		= DEVICE.getDeviceFlagValue(dev::EDCF_FULLSCREEN);
	m_Antialiasing		= DEVICE.getDeviceFlagValue(dev::EDCF_ANTIALIASING);
	m_VerticalSync		= DEVICE.getDeviceFlagValue(dev::EDCF_VERTICAL_SYNC);
	m_Shadows			= DEVICE.getDeviceFlagValue(dev::EDCF_SHADOWS);
	m_UseShaders		= DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS);
	m_LightingMediumQuality	= DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_MEDIUM_QUALITY);
	m_LightingHighQuality	= DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_HIGH_QUALITY);
	m_ShadersHighQuality	= DEVICE.getDeviceFlagValue(dev::EDCF_SHADERS_HIGH_QUALITY);
	m_CacheShaders			= DEVICE.getDeviceFlagValue(dev::EDCF_CACHE_SHADERS);
	m_UseFFP				= DEVICE.getDeviceFlagValue(dev::EDCF_USE_FFP);

	setTextureCreationFlag(ETCF_CREATE_POWER_OF_TWO, true); // by default
	setTextureCreationFlag(ETCF_CREATE_DIVISIBLE_BY_FOUR, true);
}

//---------------------------------------------------------------------------

CNullDriver::~CNullDriver()
{
	if (getDriverType() == EDT_NULL)
	{
		free();
		LOGGER.log("NULL graphic deactivated");
	}
}

//---------------------------------------------------------------------------

void CNullDriver::free()
{
	setRenderTarget(NULL);

	core::list<CNullRenderTarget *>::iterator it = m_RTs.begin();
	for (; it != m_RTs.end(); ++it)
	{
		CNullRenderTarget *rt = *it;
		if (rt->isLocked())
			rt->unlockObject();
		if (rt->getReferenceCounter() != 1)
			LOGGER.logErr("Can not remove Render Target. Possible leaked object!");
		rt->drop();
	}

	// dropping used singletons
	m_Profiler.drop();
    IMAGE_LIBRARY.drop();
	FILE_SYSTEM.drop();

	for (s32 t=0; t<MY_MATERIAL_MAX_LAYERS; t++)
		SAFE_DROP(CurrentTexture[t]);

	// delete all fonts
    for ( u32 i = 0; i < m_Fonts.size(); ++i )
        SAFE_DROP( m_Fonts[i].Font );
	m_Fonts.clear();

	for ( u32 p = 0; p < E_RENDER_PASS_COUNT; p++ )
	{
		m_RenderPoolsCache[0][p].clear_ptr();
		m_RenderPoolsCache[1][p].clear_ptr();
	}

	// prepare for clearing video cache

	vid::releaseTextures();
	vid::releaseGPUPrograms();

	clearGPUProgramHash();

	// clear Textures

	u32 tcnt = m_Textures.size();
	s32 nErasedTextures = 0;
	for (u32 i = 0; i < tcnt; i++)
	{
		if (m_Textures[i].Surface->isLocked())
			m_Textures[i].Surface->unlockObject();
		nErasedTextures++;
		if (m_Textures[i].Surface->getReferenceCounter() != 1)
		{
			LOGGER.logWarn("Detected referenced texture %p '%s', it may be leaked!!!",
				m_Textures[i].Surface, m_Textures[i].Filename.c_str());
			m_Textures[i].Surface->clear();
		}
		m_Textures[i].Surface->drop();
	}
	m_Textures.clear();   

	if (nErasedTextures > 0)
		LOGGER.logInfo("Video cache (%d textures) has been cleared.",
			nErasedTextures);
}

//---------------------------------------------------------------------------

bool CNullDriver::_initDriver(SExposedVideoData &out_video_data)
{
	setTextureCreationFlag(ETCF_CREATE_POWER_OF_TWO,
		queryFeature(EVDF_NON_POWER_OF_TWO_TEXTURES) == false);
	setTextureFilter(m_TextureFilter);
	setColorMask(m_ColorMask);
	setFog(m_Fog);
	setGlobalAmbientColor(getGlobalAmbientColor());
	setViewPort(0, 0, m_ScreenSize.Width, m_ScreenSize.Height);

	if (m_UseShaders && !queryFeature(EVDF_SHADER_LANGUAGE))
	{
		m_UseShaders = false;
		LOGGER.logWarn(
			"Your hardware not support GLSL/HLSL shaders, using FFP insted");
	}
	m_ShadersHighQuality = m_UseShaders && (
		m_LightingMediumQuality || m_LightingHighQuality) &&
		m_ShadersHighQuality;
	m_CacheShaders = m_UseShaders && m_CacheShaders;
	m_UseFFP = !m_UseShaders || m_UseFFP;

	LOGGER.logInfo("Video driver features:");
	LOGGER.logInfo(" Back Color Format : %s",
		img::getColorFormatName(getBackColorFormat()));
	LOGGER.logInfo(" Multitexturing    : %s",
		queryFeature(EVDF_MULITEXTURE) ? "OK" : "None");
	LOGGER.logInfo(" Anisotropic filt. : %s (%d level)",
		queryFeature(EVDF_ANISOTROPIC_FILTER) ?
			"OK" : "None", m_MaxAnisotropyLevel);
	if (getDriverFamily() != vid::EDF_NULL)
	{
		LOGGER.logInfo(" %s              : %s",
			getDriverFamily() == EDF_OPENGL ? "GLSL" : "HLSL",
			queryFeature(EVDF_SHADER_LANGUAGE) ? "OK" : "None");
		if (queryFeature(EVDF_SHADER_LANGUAGE))
		{
			LOGGER.logInfo("  Vertex shader - v.%d.%d",
				0xff & m_VertexShaderVersion >> 8,
				0xff & m_VertexShaderVersion);
			LOGGER.logInfo("  Pixel shader  - v.%d.%d",
				0xff & m_PixelShaderVersion >> 8,
				0xff & m_PixelShaderVersion);
		}
	}
	LOGGER.logInfo(" Two side stencil  : %s",
		m_TwoSidedStencil ? "OK" : "None");
	LOGGER.logInfo(" Occlusion Query   : %s",
		queryFeature(EVDF_OCCLUSION_QUERY) ? "OK" : "None");
	LOGGER.logInfo(" Render Target     : %s",
		queryFeature(EVDF_RENDER_TO_TARGET) ? "OK" : "None");
	LOGGER.logInfo(" Compressed tex.   : %s",
		queryFeature(EVDF_COMPRESSED_TEXTURES) ? "OK" : "None");
	LOGGER.logInfo(" Depth Stencil tex.: %s",
		queryFeature(EVDF_DEPTH_STENCIL_TEXTURES) ? "OK" : "None");
	LOGGER.logInfo(" Non pwr. of 2 tex.: %s",
		queryFeature(EVDF_NON_POWER_OF_TWO_TEXTURES) ? "OK" : "None");
	LOGGER.logInfo(" MRT               : %s (%d)",
		queryFeature(EVDF_MULTIPLE_RENDER_TARGETS) ? "OK" : "None",
		m_MaxDrawBuffers);

    LOGGER.logInfo("Video driver parameters: ");
    LOGGER.logInfo(" color   : %d bit", m_ColorBits);
    LOGGER.logInfo(" alpha   : %d bit", m_AlphaBits);
    LOGGER.logInfo(" depth(z): %d bit", m_DepthBits);
    LOGGER.logInfo(" stencil : %d bit", m_StencilBits);
    LOGGER.logInfo(" max lights    : %d", m_MaxLights);
    LOGGER.logInfo(" max tex. units: %d", m_MaxTextureUnits);
    LOGGER.logInfo(" max tex. res. : %d x %d",
		m_MaxTextureSize.Width, m_MaxTextureSize.Height);	

	_createEmbeddedTextures();

    return true;
}

//---------------------------------------------------------------------------

void CNullDriver::setRenderPath(E_RENDER_PATH renderPath)
{
	m_RenderPath = renderPath;
}

//---------------------------------------------------------------------------

bool CNullDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
	switch (feature)
    {
	case EVDF_MULITEXTURE:
		return getMaximalTextureUnitsAmount()>1;
    case EVDF_ANISOTROPIC_FILTER:
        return m_MaxAnisotropyLevel>0;
    case EVDF_STENCIL_BUFFER:
        return m_StencilBuffer;
    case EVDF_RENDER_TO_TARGET:
		return m_RenderTargetSupport;
	case EVDF_DEPTH_STENCIL_TEXTURES:
		return m_DepthStencilTexturesSupport;
	case EVDF_NON_POWER_OF_TWO_TEXTURES:
		return m_TexturesNonPowerOfTwo;
	case EVDF_MULTIPLE_RENDER_TARGETS:
		return getMaximalColorAttachmentsAmount() > 1;
    }

	return false;
}

//---------------------------------------------------------------------------

void CNullDriver::_createEmbeddedTextures()
{
	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			light_sphere_data, LIGHT_SPHERE_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_LightSphereTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_GRADIENT);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			light_gradient_data, LIGHT_GRADIENT_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_LightGradientTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_SPHERE_WHITE);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			light_sphere_white_data, LIGHT_SPHERE_WHITE_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_LightSphereWhiteTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_LIGHT_WHITE_GRADIENT);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			light_gradient_white_data, LIGHT_GRADIENT_WHITE_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_LightGradientWhiteTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_WHITE_WITH_BORDER);

		img::IImage *img = IMAGE_LIBRARY.createEmptyImage(
			core::dimension2di(64,64), img::ECF_A8R8G8B8);
		img->fill(img::SColor(255,255,255,255));
		img->drawBorder(0,0,63,63, img::SColor(0,0,0,0));
		vid::ITexture* tex = m_WhiteTexture =
			VIDEO_DRIVER.addTexture(tname , img);
		tex->lockObject();
		SAFE_DROP(img);		
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_ENVIRONMENT_MAP);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			envmap_data, ENVMAP_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_EnvTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}

	{
		const c8 *tname = getDefaultTextureReadableName(vid::EDT_DEFAULT_TEXTURE);

		io::IReadFile* file = FILE_SYSTEM.createMemoryReadFile(
			default_texture_data, DEFAULT_TEXTURE_DATA_LENGTH, tname, false);
		img::IImage *img = IMAGE_LIBRARY.getImage(file);		
		if (img)
		{
			img->convertTo(img::ECF_A8R8G8B8);
			vid::ITexture* tex = m_DefaultTexture =
				VIDEO_DRIVER.addTexture(tname, img);
			tex->lockObject();
			IMAGE_LIBRARY.removeImage(img);
		}
		file->drop();
	}
}

//---------------------------------------------------------------------------

void CNullDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	Transformation3DChanged = true;

	Matrices[state] = mat;

	m_ModelViewMatrix = Matrices[ETS_VIEW] * Matrices[ETS_MODEL];

	m_HWProjMatrix = Matrices[ETS_PROJ];
	if (m_YInverted)
	{
		f32 *p = (f32 *)m_HWProjMatrix.pointer();
		// Verticaly invert our picture, if needed.
		p[5]*= -1.0f;
	}
	m_HWModelViewProjMatrix = m_HWProjMatrix * m_ModelViewMatrix;

	m_ModelViewInvMatrix = m_ModelViewMatrix;
	m_ModelViewInvMatrix.makeInversed();
	m_ModelInvMatrix = Matrices[ETS_MODEL];
	m_ModelInvMatrix.makeInversed();
	m_ViewInvMatrix = Matrices[ETS_VIEW];
	m_ViewInvMatrix.makeInversed();
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::getTexture(const c8* filename)
{
	if (!filename || core::stringc(filename) == NONAME_FILE_NAME)
		return NULL;

    ITexture* texture = findTexture(filename);

    if (texture)
        return texture;

    io::IReadFile* file = 
		FILE_SYSTEM.createAndOpenFile(filename);

    if (file)
    {
		core::stringc texname = filename;

        texture = _loadTextureFromFile(file, texname);

        if (texture)
        {
            _addTexture(texture, texname.c_str());
            texture->drop(); // drop it becaus we created it, one grab to much
        }

		file->drop();
    }
    else
	{
        LOGGER.log("Could not open file of texture", filename, io::ELL_ERROR);
	}

    if (!texture)
	{
        LOGGER.log("Could not load texture", filename, io::ELL_ERROR);
	}
	
    return texture;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::getTexture(io::IReadFile* file)
{
    ITexture* texture = 0;

    if (file)
    {
        texture = findTexture(file->getFileName());

        if (texture)
            return texture;

		core::stringc texname = file->getFileName();

        texture = _loadTextureFromFile(file, texname);

        if (texture)
        {
            _addTexture(texture, texname.c_str());
            texture->drop(); // drop it because we created it, one grab to much
        }
		else
			LOGGER.log("Could not load texture", file->getFileName(), io::ELL_ERROR);
    }
	else
	{
		LOGGER.log("Could not load texture because file is null", io::ELL_ERROR);
	}

    return texture;
}

//---------------------------------------------------------------------------

vid::ITexture* CNullDriver::findTexture(const c8 *name)
{
	vid::ITexture *tex = 0;

	SSurface s;
	s.Filename = name ? name : "";
	s.Filename.make_lower();

	s32 index = m_Textures.binary_search(s);
	if (index != -1)
		tex = m_Textures[index].Surface;

    return tex;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::addTexture(const c8 *name, img::IImage *image)
{
	if (!name || !image)
		return 0;
	ITexture* t = _createDeviceDependentTexture(image);
	_addTexture(t, image->getOverridedFileName()?image->getOverridedFileName():name);
	t->drop();
	return t;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::addTexture(const c8 *name,
	const core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	if (!name)
		return 0;

	img::IImage* image = IMAGE_LIBRARY.createEmptyImage(size, format);

	ITexture* t = _createDeviceDependentTexture(image);
	image->drop();
	_addTexture(t, name);

	if (t)
		t->drop();

	return t;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::addRenderTargetTexture(const c8 *name,
	const core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	c8 rttname[255];

	ITexture *t = createRenderTargetTexture(size, format);
	if (!name)
	{
		sprintf(rttname, "RenderTargetTexture.%dx%d.%s.%p",
			size.Width, size.Height, img::getColorFormatName(format), t);
		name = rttname;
	}
	_addTexture(t, name);
	if (t)
		t->drop();

	return t;
}

//---------------------------------------------------------------------------

IRenderTarget* CNullDriver::addRenderTarget()
{
	CNullRenderTarget *rt =(CNullRenderTarget *)createRenderTarget();
	if (rt && !rt->isOK())
	{
		rt->drop();
		rt = NULL;
	}
	if (rt)
		_addRenderTarget(rt);
	return rt;
}

//---------------------------------------------------------------------------

IRenderTarget* CNullDriver::addRenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
{
	CNullRenderTarget *rt =
		(CNullRenderTarget *)createRenderTarget(size, colorFormat, depthFormat);
	if (rt && !rt->isOK())
	{
		rt->drop();
		rt = NULL;
	}
	if (rt)
		_addRenderTarget(rt);
	return rt;
}

//---------------------------------------------------------------------------

IRenderTarget* CNullDriver::addRenderTarget(
	ITexture *colorTexture, ITexture *depthTexture)
{
	CNullRenderTarget *rt =
		(CNullRenderTarget *)createRenderTarget(colorTexture, depthTexture);
	if (rt && !rt->isOK())
	{
		rt->drop();
		rt = NULL;
	}
	if (rt)
		_addRenderTarget(rt);
	return rt;
}

//---------------------------------------------------------------------------

ITextureAnim* CNullDriver::addTextureAnim( 
	core::array<ITexture*> &frames, SAnimatedTextureParams &params)
{
	// loading textures from xml file

	ITextureAnim* anitex = new CTextureAnim();

	for (u32 t=0; t<frames.size(); t++)
	{
		if (frames[t])
			anitex->addFrame(frames[t]);
	}

	anitex->setParameters(params);

	c8 texfname[255];
	sprintf(texfname, "anitexture%p", anitex);

	_addTexture(anitex, texfname);

	anitex->drop();

	return anitex;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::createTexture(img::IImage* image)
{
     if (!image)
        return 0;
    return _createDeviceDependentTexture(image);
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::createTexture(core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	ITexture *texture = _createDeviceDependentTexture(size, format);
	return texture;
}

//---------------------------------------------------------------------------

bool CNullDriver::removeTexture(ITexture* texture)
{
	if (texture->isLocked())
		return false;

	for (u32 i=0; i < m_Textures.size(); ++i)
	{
        if (texture && m_Textures[i].Surface == texture)
        {
			if (texture->getReferenceCounter()==1)
			{
				SAFE_DROP(texture);
				LOGGER.logInfo("Texture '%s' removed.",
					m_Textures[i].Filename.c_str());
				m_Textures.erase(i);
				return true;
			}
			LOGGER.logWarn("Can't remove '%s' referenced texture.",
				m_Textures[i].Filename.c_str());
			return false;
        }
	}
	LOGGER.logWarn("Can't remove texture.");

	return false;
}

//---------------------------------------------------------------------------

vid::ITexture* CNullDriver::_loadTextureFromFile(io::IReadFile* file, core::stringc& desired_texname)
{
    ITexture* texture = 0;

	if (file && strstr(file->getFileName(), ".atex") == 0)
	{
	    img::IImage* image = IMAGE_LIBRARY.getImage(file);
   
	    if (image)
		{
			// create texture from surface

	        texture = _createDeviceDependentTexture(image);	
			
			if (image->getOverridedFileName())
			{
				desired_texname = image->getOverridedFileName();	
			}
			else
			{
				desired_texname = file->getFileName();	
			}
		}		
	}
	else if (file)
	{
		// loading textures from xml file

		ITextureAnim* anitex = new CTextureAnim();	
		
		if (!anitex->loadFromFile(file))
		{
			anitex->drop();
		}
		else
		{
			texture = anitex;
		}

		desired_texname = file->getFileName();	
	}

    return texture;
}

//---------------------------------------------------------------------------

void CNullDriver::_addTexture(vid::ITexture* texture, const c8* name)
{
    if (!texture)
		return;

    SSurface s;
	s.Filename = name ? name : "";
    s.Filename.make_lower();
    s.Surface = texture;
    texture->grab();

    m_Textures.push_back(s);
}

//---------------------------------------------------------------------------

const c8* CNullDriver::findTextureName(vid::ITexture *texture)
{
    static core::stringc filename;
	filename = NONAME_FILE_NAME;
	for (u32 i=0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].Surface == texture)
		{
			filename = m_Textures[i].Filename.c_str();
			break;
		}
	}
	filename.make_lower();
	if (filename.size() == 0)
		filename = NONAME_FILE_NAME;
    return filename.c_str();
}

//---------------------------------------------------------------------------

bool CNullDriver::setTextureName(vid::ITexture *texture, const c8 *name)
{
	if (findTexture(name))
		return false;

	for (u32 i=0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].Surface == texture)
		{
			m_Textures[i].Filename = name;
			m_Textures.set_sorted(false);
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::_createDeviceDependentTexture(img::IImage* surface)
{
    return new CSoftwareTexture(surface);
}

//---------------------------------------------------------------------------

vid::ITexture* CNullDriver::_createDeviceDependentTexture(
	core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	 return new CSoftwareTexture(size, format);
}

//---------------------------------------------------------------------------

bool CNullDriver::_addRenderTarget(CNullRenderTarget *rt)
{
	if (!rt || rt->m_RTEntry != 0)
	{
		LOGGER.logErr("Can not add Render Target to the resources pool!");
		return false;
	}
	m_RTs.push_back(rt);
	rt->m_RTEntry = m_RTs.get_last();
	return true;
}

//---------------------------------------------------------------------------

bool CNullDriver::removeRenderTarget(IRenderTarget *_rt)
{
	vid::CNullRenderTarget *rt = (vid::CNullRenderTarget *)_rt;
	if (rt->isLocked())
		return false;

	bool ret = false;

	if (rt->m_RTEntry != 0)
	{
		m_RTs.erase(rt->m_RTEntry);
		rt->m_RTEntry = 0;
		ret = rt->drop();
	}

	if (!ret)
		LOGGER.logWarn("Can't remove render target.");

	return ret;
}

//---------------------------------------------------------------------------

bool CNullDriver::setRenderTarget(IRenderTarget *rt)
{
	bool ret = true;

	if (rt == m_CurrentRenderTarget)
		return ret;

	IRenderTarget *prevRT = m_CurrentRenderTarget;
	if (prevRT)
		prevRT->unbind();

	m_CurrentRenderTarget = rt;

	if (rt)
	{
		rt->grab();
		ret = rt->bind();
	}

	if (ret)
	{
		if (prevRT)
			prevRT->drop();
	}
	else
	{
		m_CurrentRenderTarget = prevRT;
		if (prevRT)
			prevRT->bind();
		if (rt)
			rt->drop();
	}

	if (!m_CurrentRenderTarget)
	{
		const core::dimension2di &scrSize = getScreenSize();
		setViewPort(0, 0, scrSize.Width, scrSize.Height);
	}

	return ret;
}

//---------------------------------------------------------------------------

IRenderTarget* CNullDriver::getRenderTarget()
{
	return m_CurrentRenderTarget;
}

//---------------------------------------------------------------------------

void CNullDriver::setViewPort(const core::rect<s32>& area)
{
}

//---------------------------------------------------------------------------

void CNullDriver::setViewPort(s32 left, s32 top, s32 right, s32 bottom)
{
	setViewPort(core::recti(left, top, right, bottom));
}

//---------------------------------------------------------------------------

const core::rect<s32>& CNullDriver::getViewPort() const
{
    return ViewPort;
}

//---------------------------------------------------------------------------

//! returns screen size
const core::dimension2d<s32>& CNullDriver::getScreenSize() const
{
    return m_ScreenSize;
}

//---------------------------------------------------------------------------

s32 CNullDriver::getCurrentFPS()
{ return FPSCounter.getEventPerSecond(); }

//---------------------------------------------------------------------------

s32 CNullDriver::getAverageFPS()
{ return AverageFPS; }

//---------------------------------------------------------------------------

u32 CNullDriver::getRenderedTrianglesCount ( E_RENDER_PASS pass )
{ return m_RenderedTrianglesCount[pass]; }

//---------------------------------------------------------------------------

u32 CNullDriver::getRenderedTrianglesCount ()
{ return m_TrianglesDrawn; }

//---------------------------------------------------------------------------

u32 CNullDriver::getRenderedDIPsCount ( E_RENDER_PASS pass )
{ return m_RenderedDIPsCount[pass]; }

//---------------------------------------------------------------------------

u32 CNullDriver::getRenderedDIPsCount ()
{ return m_DIPsDrawn; }

//---------------------------------------------------------------------------

void CNullDriver::setGlobalAmbientColor(const img::SColorf& color)
{
	GlobalAmbientColor = color;
}

//---------------------------------------------------------------------------

//! gets the dynamic ambient light color.
const img::SColorf& CNullDriver::getGlobalAmbientColor() const
{
	return GlobalAmbientColor;
}

//---------------------------------------------------------------------------

//! Sets the global light params
void CNullDriver::setGlobalLightParams(const SLight& light)
{	
	GlobalLight.Type			= ELT_DIRECTIONAL;
	GlobalLight.AmbientColor	= light.AmbientColor;
	GlobalLight.DiffuseColor	= light.DiffuseColor;
    GlobalLight.SpecularColor	= light.SpecularColor;
	GlobalLight.Position		= light.Position; 
    GlobalLight.CastShadows		= light.CastShadows;
	GlobalLight.Enabled			= light.Enabled;
}

//---------------------------------------------------------------------------
		
//! gets the global light params
const SLight& CNullDriver::getGlobalLightParams()
{
	return GlobalLight;
}

//---------------------------------------------------------------------------

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
s32 CNullDriver::getMaximalPrimitiveCount()
{
    return 65535;
}

//---------------------------------------------------------------------------

//! checks triangle count and print warning if wrong
bool CNullDriver::checkPrimitiveCount(s32 vtxCount)
{
    s32 m = getMaximalPrimitiveCount();
    if (vtxCount > m)
    {
        char tmp[1024];
        sprintf(tmp,"Could not draw triangles, too many vertices(%d), maxium is %d.", vtxCount, m);
        LOGGER.log(tmp, io::ELL_ERROR);
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------

void CNullDriver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	if (enabled && (
			flag == ETCF_CREATE_COLOR_KEY || 
			flag == ETCF_CREATE_NORMAL_MAP))
	{
		// disable other formats
		setTextureCreationFlag(ETCF_CREATE_COLOR_KEY, false);
		setTextureCreationFlag(ETCF_CREATE_NORMAL_MAP, false);
	}

    // set flag
    TextureCreationFlags = 
		(TextureCreationFlags & (~flag)) | ((((u32)!enabled)-1) & flag);
}

//---------------------------------------------------------------------------

//! Returns if a texture creation flag is enabled or disabled.
bool CNullDriver::getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag)
{
    return (TextureCreationFlags & flag)!=0;
}

//---------------------------------------------------------------------------

void CNullDriver::setFog(const SFog &fog)
{
    m_Fog = fog;

	setBackgroundColor(fog.Color);
}

//---------------------------------------------------------------------------

const SFog& CNullDriver::getFog()
{
    return m_Fog;
}

//---------------------------------------------------------------------------

//! setting ground fog color
void CNullDriver::setGroundFogColor(img::SColor gfogcol)
{
	GroundFogColor = gfogcol;
}

//---------------------------------------------------------------------------

//! returning ground fog color
img::SColor CNullDriver::getGroundFogColor()
{
	return GroundFogColor;
}

//---------------------------------------------------------------------------

void CNullDriver::OnResize(const core::dimension2di &size)
{
    m_ScreenSize = size;
}

//---------------------------------------------------------------------------

ITexture* CNullDriver::makeScreenShotTexture()
{
	bool ret = false;
	c8 name[256];
	sprintf(name, "ScreenShotTexture.%s.%dx%d",
		img::getColorFormatName(img::ECF_A8R8G8B8),
		m_ScreenSize.Width, m_ScreenSize.Height);

	ITexture *texture = findTexture(name);
	if (!texture)
		texture = addTexture(name, m_ScreenSize, img::ECF_A8R8G8B8);

	img::IImage *image = texture ? texture->lock() : NULL;
	if (image
			&& image->getDimension() == m_ScreenSize
			&& image->getColorFormat() == img::ECF_A8R8G8B8)
	{
		IRenderTarget *rt = getRenderTarget();
		setRenderTarget(NULL);

		ret = _makeScreenShot(image);
		texture->unlock();

		setRenderTarget(rt);
	}
	if (!ret)
		LOGGER.logWarn("Screenshot created with errors!");

    return texture;
}

//---------------------------------------------------------------------------

img::IImage* CNullDriver::makeScreenShotImage()
{
	c8 name[256];
	sprintf(name, "ScreenShotImage.%s.%dx%d",
		img::getColorFormatName(img::ECF_A8R8G8B8),
		m_ScreenSize.Width, m_ScreenSize.Height);

	img::IImage *image = IMAGE_LIBRARY.findImage(name);
	if (!image)
		image = IMAGE_LIBRARY.addImage(name, m_ScreenSize, img::ECF_A8R8G8B8);

	IRenderTarget *rt = getRenderTarget();
	setRenderTarget(NULL);

	if (!image 
			|| image->getDimension() != m_ScreenSize
			|| image->getColorFormat() != img::ECF_A8R8G8B8
			|| !_makeScreenShot(image))
		LOGGER.logWarn("Screenshot created with errors!");

	setRenderTarget(rt);

    return image;
}

//---------------------------------------------------------------------------

void CNullDriver::maxIndexWarning(u8 idxSize)
{	
	char tmp[1024];

    sprintf(tmp,"Warning, you use too many vertexes for an indexsize of (%d) bits.", idxSize);

    LOGGER.log(tmp, io::ELL_WARNING);

    return;
}

//---------------------------------------------------------------------------

void CNullDriver::setBackgroundColor(const img::SColor &color)
{
	BackColor = color;
	m_Fog.Color = color;
}

//---------------------------------------------------------------------------

const img::SColor& CNullDriver::getBackgroundColor()
{
	return BackColor;
}

//---------------------------------------------------------------------------

//! sets texture filtering mode
void CNullDriver::setTextureFilter(E_TEXTURE_FILTER textureFilter)
{
	if (textureFilter == ETF_ANISOTROPIC && !queryFeature(EVDF_ANISOTROPIC_FILTER))
		textureFilter = ETF_TRILINEAR;
	if (textureFilter == ETF_TRILINEAR && !queryFeature(EVDF_TRILINEAR_FILTER))
		textureFilter = ETF_BILINEAR;
	if (textureFilter == ETF_BILINEAR && !queryFeature(EVDF_BILINEAR_FILTER))
		textureFilter = ETF_NONE;

	m_TextureFilter = textureFilter;
}

//---------------------------------------------------------------------------

//! get texture filtering mode
E_TEXTURE_FILTER CNullDriver::getTextureFilter()
{
	return m_TextureFilter;
}

//---------------------------------------------------------------------------

//! clear unused VRAM
void CNullDriver::clearUnusedVideoCache()
{
	// delete unused textures

	s32 nErasedTextures = 0, nInititialTextures = m_Textures.size();

	// first dropping animated textures

	for (u32 i=0; i<m_Textures.size();)
	{
		if (m_Textures[i].Surface->getType() != ETT_ANIMATED ||
			m_Textures[i].Surface->getReferenceCounter() != 1 ||
			m_Textures[i].Surface->isLocked())
		{
			i++;
			continue;
		}
		nErasedTextures++;
		SAFE_DROP(m_Textures[i].Surface);
		m_Textures.erase(i);
	}

	// after that simple

	for (u32 i=0; i<m_Textures.size();)
	{
		if (m_Textures[i].Surface->getType() != ETT_COLOR_MAP ||
			m_Textures[i].Surface->getReferenceCounter() != 1 ||
			m_Textures[i].Surface->isLocked())
		{
			i++;
			continue;
		}
		nErasedTextures++;
		SAFE_DROP(m_Textures[i].Surface);
		m_Textures.erase(i);
	}

	if ( nErasedTextures>0 )
		LOGGER.logInfo("Unused video cache (%d textures)(from %d at all) has been cleared.", 
			nErasedTextures, nInititialTextures);
}

//----------------------------------------------------------------------------

IVideoModeList* CNullDriver::getVideoModeList()
{
#ifdef WIN32
    if (!VideoModeList.getVideoModeCount())
    {
        // enumerate video modes.
        DWORD i=0;
        DEVMODE mode;
        while (EnumDisplaySettings(NULL, i, &mode))
        {
            VideoModeList.addMode(core::dimension2di(mode.dmPelsWidth, mode.dmPelsHeight),
                mode.dmBitsPerPel);
            ++i;
        }
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode))
            VideoModeList.setDesktop(mode.dmBitsPerPel,
				core::dimension2di(mode.dmPelsWidth, mode.dmPelsHeight));
    }
#elif defined LINUX
	// linux implementation
#else
	// other implementation
#endif

    return &VideoModeList;
}

//---------------------------------------------------------------------------

void CNullDriver::_setTexture(s32 stage, ITexture* texture)
{
	if (texture && stage > m_DirtyTexUnit)
		m_DirtyTexUnit = stage;
}

//---------------------------------------------------------------------------

ITexture * CNullDriver::_getCurrentTexture(s32 stage)
{
	static u32 max_tex_units = getMaximalTextureUnitsAmount();

	if (stage < 0 || stage > (s32)max_tex_units)
		return NULL;

	return CurrentTexture[stage];
}

//---------------------------------------------------------------------------

const core::position2di& CNullDriver::getScreenCoordinatesFrom3DPosition(
	const core::vector3df &pos3d) const
{
	core::position2df pos2df(getScreenRelativeCoordinatesFrom3DPosition(pos3d));
	static core::position2di pos2di;
	
	pos2di.set(
		(s32)(pos2df.X * (f32)m_ScreenSize.Width + 0.5f),
		(s32)(pos2df.Y * (f32)m_ScreenSize.Height + 0.5f));

	// This coordinates looks like:
	// (0,0) o---------------------|
	//       |                     | 
	//       |                     |
	//       |                     |
	//       |          o          |
	//       |  (Width/2,Height/2) |
	//       |                     |
	//       |                     |
	//       |---------------------o (Width,Height)
    return pos2di;
}

//---------------------------------------------------------------------------

const core::position2df& CNullDriver::getScreenRelativeCoordinatesFrom3DPosition(
	const core::vector3df &pos3d) const
{
	static core::position2df pos2d;
	pos2d = getScreenProjectionCoordinatesFrom3DPosition(pos3d);
    pos2d.X = 0.5f * pos2d.X + 0.5f;
    pos2d.Y = - 0.5f * pos2d.Y + 0.5f;

	// This coordinates looks like:
	// (0,0) o----------o----------o (1,0)
	//       |          |          | 
	//       |          |          |
	//       |          |(0.5,0.5) |
	//       o----------o----------o
	//       |          |          |
	//       |          |          |
	//       |          |          |
	// (0,1) o----------o----------o (1,1)
    return pos2d;
}

//---------------------------------------------------------------------------

const core::position2df& CNullDriver::getScreenProjectionCoordinatesFrom3DPosition(
	const core::vector3df &pos3d) const
{
	static core::position2df pos2d(-1,-1);

    f32 transformedPos[4];

    core::matrix4 trans = Matrices[ETS_PROJ];
    trans *= Matrices[ETS_VIEW];

    transformedPos[0] = pos3d.X;
    transformedPos[1] = pos3d.Y;
	transformedPos[2] = pos3d.Z;
    transformedPos[3] = 1.0f;

    trans.multiplyWith1x4Matrix(transformedPos);

    if (transformedPos[2] <= 0.0)
		 transformedPos[3] = 0.0f;

    f32 zDiv = transformedPos[3] == 0.0f ? 1.0f :
        (1.0f / transformedPos[3]);

    pos2d.X = transformedPos[0] * zDiv;
    pos2d.Y = transformedPos[1] * zDiv;

	// This coordinates looks like:
	// (-1,1) o----------o----------o (1,1)
	//        |          |          |
	//        |          |          |
	//        |          |(0,0)     |
	//        o----------o----------o
	//        |          |          |
	//        |          |          |
	//        |          |          |
	// (-1,1) o----------o----------o (1,-1)
    return pos2d;
}

//---------------------------------------------------------------------------

const core::vector3df& CNullDriver::get3DPositionFromScreenCoordinates(
	const core::position2d<s32> &scr_pos) const
{
	static core::vector3df pos3d(0,0,0);
	LOGGER.logWarn(
		"This type of video driver didn't supports reading 3d data from frame buffer.");
	return pos3d;
}

//---------------------------------------------------------------------------

//! return hardware occlusion object
IHardwareOcclusionQuery& CNullDriver::getHardwareOcclusionQuery()
{
	return m_HardwareOcclusionQueryStub;
}

//---------------------------------------------------------------------------

u32 CNullDriver::getMinimalMeshBufferSizeForVBORendering()
{
	return m_MinimalMeshBufferSizeForVBORendering;
}

//---------------------------------------------------------------------------

void CNullDriver::setNormalMapCreationAmplitude(f32 amplitude)
{
	m_NormalMapCreationAmplitude = amplitude;
}

//---------------------------------------------------------------------------

f32 CNullDriver::getNormalMapCreationAmplitude()
{
	return m_NormalMapCreationAmplitude;
}

//---------------------------------------------------------------------------

//! Setting polygon fill mode
void CNullDriver::setPolygonFillMode(E_POLYGON_FILL_MODE mode)
{
	m_PolygonFillMode = mode;
}

//---------------------------------------------------------------------------

//! Getting polygon fill mode
E_POLYGON_FILL_MODE CNullDriver::getPolygonFillMode()
{
	return m_PolygonFillMode;
}

//---------------------------------------------------------------------------

//! Reseting all texture units
void CNullDriver::resetTextureUnits()
{
	turnOffTextureUnits();
}

//---------------------------------------------------------------------------

//! Reseting all texture units
void CNullDriver::turnOffTextureUnits()
{
	static u32 max_units_cnt_1 = getMaximalTextureUnitsAmount() - 1;
	for (s32 unit = max_units_cnt_1; unit >= 0; unit--)
		_setTexture(unit, NULL);
}

//----------------------------------------------------------------------------

//! returns the font
IFont* CNullDriver::getFont ( const c8* filename )
{
#ifndef __MY_BUILD_VID_NULL_LIB__
    // search existing font
    SFont f;
    if (!filename)
        filename = "";

    f.Filename = filename;
    f.Filename.make_lower();

    s32 index = m_Fonts.binary_search(f);
    if (index != -1)
        return m_Fonts[index].Font;

    // not existing yet. try to load font.
    IFont *ifont;
    if (strstr(filename, ".ttf")!=0)
    // ttf-font
    {
		LOGGER.logInfo("Creating TTF-font from %s",filename);
		CFontTTF * ttfFont = new CFontTTF();
        if ( !ttfFont->load(filename) )
        {   
			ttfFont->drop();
			LOGGER.logErr("loading font, using default");
            return m_Fonts[0].Font;
        }
        ifont = ttfFont;
    }
	else
    // graphic font
    {
        LOGGER.logInfo("Creating graphic-font from %s",filename);
        CFont * font = new CFont ();
        if (!font->load(filename))
        {   
			font->drop();
			LOGGER.logErr("loading font, using default");
            return m_Fonts[0].Font;
        }
        ifont = font;
    }
	LOGGER.logInfo("Font created successfully.");

    // add to fonts.
    f.Font = ifont;
    m_Fonts.push_back(f);

    return ifont;
#endif
	return NULL;
}

//----------------------------------------------------------------------------

IFont * CNullDriver::getBuiltInFont ()
{
    if (m_Fonts.empty())
        return 0;
    return m_Fonts[0].Font;
}

//---------------------------------------------------------------------------

void CNullDriver::registerPoolForRendering(
	E_RENDER_PASS pass,	const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	const core::array <const SMaterial*> &materials,
	const core::array <IRenderBuffer*> &render_buffers, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	m_RenderPools[pass].push_back(
		SRenderPool(pass, z, 0, transf, light0, enabled_lights,
			materials, render_buffers, mode, owner));
}

//---------------------------------------------------------------------------

void CNullDriver::registerPoolForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	s32 enabled_light_idx,
	const core::array <const SMaterial*> &materials,
	const core::array <IRenderBuffer*> &render_buffers, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	m_RenderPools[pass].push_back(
		SRenderPool(pass, z, 0, transf, enabled_light_idx,
			materials, render_buffers, mode, owner));
}

//---------------------------------------------------------------------------

void CNullDriver::registerPoolForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	const SMaterial &material,
	IRenderBuffer &render_buffer, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	m_RenderPools[pass].push_back(
		SRenderPool(pass, z, 0, transf, light0, enabled_lights,
			material, render_buffer, mode, owner));
}

//---------------------------------------------------------------------------

void CNullDriver::registerPoolForRendering(
	E_RENDER_PASS pass, const core::matrix4 & transf,
	s32 enabled_light_idx,
	const SMaterial & material,
	IRenderBuffer & render_buffer, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	m_RenderPools[pass].push_back(
		SRenderPool(pass, z, 0, transf, enabled_light_idx,
			material, render_buffer, mode, owner));
}

//---------------------------------------------------------------------------

void CNullDriver::registerGeometryForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	const SMaterial &material,
	void * vertices, u32 vert_count, E_VERTEX_TYPE vtype,
	void * indices, u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	_registerGeometryForRendering(
		pass, transf, light0, enabled_lights, -1,
		material, vertices, vert_count, vtype,
		indices, ind_count, itype, dpt, z, mode, owner);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGeometryForRendering(
	E_RENDER_PASS pass, const core::matrix4 & transf,
	s32 enabled_light_idx,
	const SMaterial & material,
	void * vertices, u32 vert_count, E_VERTEX_TYPE vtype,
	void * indices, u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 z,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	_registerGeometryForRendering(
		pass, transf, NULL, NULL, enabled_light_idx,
		material, vertices, vert_count, vtype,
		indices, ind_count, itype, dpt, z, mode, owner);
}

//---------------------------------------------------------------------------

void CNullDriver::_registerGeometryForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	s32 enabled_light_idx,
	const SMaterial &material,
	void *vertices, u32 vert_count, E_VERTEX_TYPE vtype,
	void *indices, u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 z, E_RENDER_MODE mode,
	const scn::ISceneNode *owner)
{
	static SRenderPool::SRenderPoolExFinder find_what;

	s32 idx = -1;

	find_what.set(z, &transf, light0, enabled_lights, enabled_light_idx,
		&material, vtype, itype, dpt, vid::ERBT_DYNAMIC, mode, owner);

	SRenderPool *rpool = NULL;

	core::hash_array <SRenderPool*> &rpools =
		m_RenderPoolsCache[m_RenderPoolsCacheIndex][pass];
	
	u64 hash = material.getHash();

	idx = rpools.find_ptr(hash, find_what);

	if (idx == -1)
	{
		// search in cache (with less strict filter)

		u32 cache_index = (m_RenderPoolsCacheIndex + 1) % 2;

		core::hash_array < SRenderPool* > & rpools_cache =
			m_RenderPoolsCache[cache_index][pass];

		find_what.Z					= -1.0f;
		find_what.Light0			= NULL;
		find_what.EnabledLights		= NULL;
		find_what.EnabledLightIdx	= -1;

		idx = rpools_cache.find_ptr(hash, find_what);
		if (idx == -1)
		{
			// create new entry
			rpool = new SRenderPool(pass, z, rpools.size(), transf,
				light0, enabled_lights, enabled_light_idx,
				material, vtype, itype, dpt, mode, owner);
			idx = rpools.push_back(hash, rpool);
		}
		else
		{
			// reuse render pool in new scene rendering
			rpool = rpools_cache(hash, idx);

			rpool->Z				= z;
			rpool->Id				= rpools.size();
			rpool->Transform		= transf;
			rpool->Light0			= light0;
			rpool->EnabledLights	= enabled_lights;
			rpool->EnabledLightIdx	= enabled_light_idx;
			rpool->Owner			= owner;

			rpools_cache.erase(hash, idx);
			idx = rpools.push_back(hash, rpool);
		}
	}
	else
		rpool = rpools(hash, idx);

	rpool->addGeometry(
		vertices, vert_count, vtype, indices, ind_count, itype);
}

//---------------------------------------------------------------------------

SGeometryBufferEntry CNullDriver::registerGeometryBufferForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	const SMaterial &material,
	u32 vert_count, E_VERTEX_TYPE vtype,
	u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder,
	E_RENDER_MODE mode,
	const scn::ISceneNode *owner)
{
	return _registerGeometryBufferForRendering(
		pass, transf, light0, enabled_lights, -1,
		material, vert_count, vtype, ind_count, itype,
		dpt, zorder, mode, owner);
}

//---------------------------------------------------------------------------

SGeometryBufferEntry CNullDriver::registerGeometryBufferForRendering(
	E_RENDER_PASS pass, const core::matrix4 & transf,
	s32 enabled_light_idx,
	const SMaterial & material,
	u32 vert_count, E_VERTEX_TYPE vtype,
	u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder,
	E_RENDER_MODE mode,
	const scn::ISceneNode *owner)
{
	return _registerGeometryBufferForRendering(
		pass, transf, NULL, NULL, enabled_light_idx,
		material, vert_count, vtype, ind_count, itype,
		dpt, zorder, mode, owner);
}

//---------------------------------------------------------------------------

SGeometryBufferEntry CNullDriver::_registerGeometryBufferForRendering(
	E_RENDER_PASS pass, const core::matrix4 &transf,
	const SLight *light0, u64 enabled_lights,
	s32 enabled_light_idx,
	const SMaterial & material,
	u32 vert_count, E_VERTEX_TYPE vtype,
	u32 ind_count, E_INDEX_TYPE itype,
	E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder,
	E_RENDER_MODE mode, const scn::ISceneNode *owner)
{
	SGeometryBufferEntry entry(NULL, NULL, 0);

	static SRenderPool::SRenderPoolExFinder find_what;

	s32 idx = -1;

	find_what.set(zorder, &transf, light0, enabled_lights, enabled_light_idx,
		&material, vtype, itype, dpt, vid::ERBT_DYNAMIC, mode, owner);

	SRenderPool *rpool = NULL;

	core::hash_array <SRenderPool*> &rpools =
		m_RenderPoolsCache[m_RenderPoolsCacheIndex][pass];
	
	u64 hash = material.getHash();

	idx = rpools.find_ptr(hash, find_what);

	if (idx == -1)
	{
		// search in cache (with less strict filter)

		u32 cache_index = (m_RenderPoolsCacheIndex + 1) % 2;

		core::hash_array < SRenderPool* > & rpools_cache =
			m_RenderPoolsCache[cache_index][pass];

		find_what.Z					= -1.0f;
		find_what.Light0			= NULL;
		find_what.EnabledLights		= NULL;
		find_what.EnabledLightIdx	= -1;

		idx = rpools_cache.find_ptr(hash, find_what);
		if (idx == -1)
		{
			// create new entry
			rpool = new SRenderPool(pass, zorder, rpools.size(), transf,
				light0, enabled_lights, enabled_light_idx,
				material, vtype, itype, dpt, mode, owner);
			idx = rpools.push_back(hash, rpool);
		}
		else
		{
			// reuse render pool in new scene rendering
			rpool = rpools_cache(hash, idx);

			rpool->Z				= zorder;
			rpool->Id				= rpools.size();
			rpool->Transform		= transf;
			rpool->Light0			= light0;
			rpool->EnabledLights	= enabled_lights;
			rpool->EnabledLightIdx	= enabled_light_idx;
			rpool->Owner			= owner;

			rpools_cache.erase(hash, idx);
			idx = rpools.push_back(hash, rpool);
		}
	}
	else
		rpool = rpools(hash, idx);

	entry = rpool->reserveGeometry(vert_count, ind_count);

	return entry;
}

//---------------------------------------------------------------------------

void CNullDriver::register2DImageForRendering(
	const SMaterial & material,
	const core::rectf & drawRect, const core::rectf & texRect,
	const img::SColor & color,
	E_RENDER_MODE mode)
{
	register2DImageForRendering(
		material, drawRect, texRect, color, color, color, color, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::register2DImageForRendering(
	const SMaterial & material,
	const core::rectf & drawRect, const core::rectf & texRect,
	const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_2D_PASS, 1.0f, material, drawRect, texRect,
		color0, color1, color2, color3, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::register2DImageForRendering(
	ITexture * texture,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	register2DImageForRendering(texture, drawRect, texRect,
		color, color, color, color, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::register2DImageForRendering(
	ITexture * texture,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_2D_PASS, 1.0f, texture, drawRect, texRect,
		color0, color1, color2, color3,	useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIImageForRendering(
	ITexture * texture, f32 z,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_GUI_2D_PASS, z, texture, drawRect, texRect,
		color, color, color, color, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIImageForRendering(
	ITexture * texture, f32 z,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_GUI_2D_PASS, z, texture, drawRect, texRect,
		color0, color1, color2, color3, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIImageForRendering(
	ITexture * texture,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color, 
    bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_GUI_2D_PASS, 1.0f, texture, drawRect, texRect,
		color, color, color, color, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIImageForRendering(
	ITexture * texture,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
    bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	_registerImageForRendering(ERP_GUI_2D_PASS, 1.0f, texture, drawRect, texRect,
		color0, color1, color2, color3, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIRectangleForRendering(
	const core::rectf & drawRect, const img::SColor & color,
	bool useAlphaBlending, bool useColorBlending, E_RENDER_MODE mode)
{
	registerGUIRectangleForRendering(
		drawRect, color, color, color, color, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::registerGUIRectangleForRendering(
	const core::rectf & drawRect,
	const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	static const core::rectf texRect(0.0f, 0.0f, 0.0f, 0.0f);
	registerGUIImageForRendering(NULL, 1.0f, drawRect, texRect,
		color0,	color1, color2, color3, useAlphaBlending, useColorBlending, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::_registerImageForRendering(
	E_RENDER_PASS pass, f32 z, ITexture * texture,
	const core::rectf & drawRect, const core::rectf & texRect, 
    const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	static SMaterial material;

	static const E_VERTEX_TYPE			vtype		= EVT_1TCOORDS_RHW_COLOURED;
	static const E_INDEX_TYPE			itype		= EIT_16_BIT;
	static const E_DRAW_PRIMITIVE_TYPE	dpt			= EDPT_TRIANGLE_LIST;

	static SRenderPass & pass0 = material.getPass(0);

	pass0.Layers[0].setTexture ( texture );

	static bool init_done = false;

	if (!init_done)
	{
		pass0.setFlag(EMF_ZWRITE_ENABLE, true);
		pass0.setLightingMode(vid::ELM_NONE);
		pass0.setFlag(EMF_GOURAUD_SHADING, false);
		pass0.setFlag(EMF_MIP_MAP_OFF, true);
		pass0.setDepthTest(ECT_LEQUAL);
		init_done = true;
	}

	if (useAlphaBlending)
	{
		pass0.setFlag( EMF_BLENDING, true );
		pass0.setBlendFuncs(ESBF_SRC_ALPHA, EDBF_ONE_MINUS_SRC_ALPHA);
	}
	else if (useColorBlending)
	{
		pass0.setFlag(EMF_BLENDING, true);
		pass0.setBlendFuncs(ESBF_ONE, EDBF_ONE_MINUS_SRC_COLOR);
	}
	else
	{
		pass0.setFlag(EMF_BLENDING, false);
	}

	static S3DVertex1TCoordsRHWColoured vertices[4] =
	{
		S3DVertex1TCoordsRHWColoured(0.0f, 0.0f , 0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsRHWColoured(0.0f, 0.0f , 0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsRHWColoured(0.0f, 0.0f , 0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsRHWColoured(0.0f, 0.0f , 0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f)
	};

	if (getDriverFamily() == vid::EDF_OPENGL)
	{
		s32 xPlus = -(m_ScreenSize.Width>>1);
		f32 xFact = 1.0f / (m_ScreenSize.Width>>1);
		s32 yPlus = m_ScreenSize.Height-(m_ScreenSize.Height>>1);
		f32 yFact = 1.0f / (m_ScreenSize.Height>>1);

		vertices[0].Pos.X = (f32)(drawRect.UpperLeftCorner.X+xPlus) * xFact;
		vertices[0].Pos.Y = (f32)(yPlus-drawRect.UpperLeftCorner.Y) * yFact;
		vertices[1].Pos.X = (f32)(drawRect.LowerRightCorner.X+xPlus) * xFact;
		vertices[1].Pos.Y = (f32)(yPlus- drawRect.UpperLeftCorner.Y) * yFact;
		vertices[2].Pos.X = (f32)(drawRect.LowerRightCorner.X+xPlus) * xFact;
		vertices[2].Pos.Y = (f32)(yPlus-drawRect.LowerRightCorner.Y) * yFact;
		vertices[3].Pos.X = (f32)(drawRect.UpperLeftCorner.X+xPlus) * xFact;
		vertices[3].Pos.Y = (f32)(yPlus-drawRect.LowerRightCorner.Y) * yFact;

		vertices[0].Color.color = color0.toOpenGLColor();
		vertices[1].Color.color = color1.toOpenGLColor();
		vertices[2].Color.color = color2.toOpenGLColor();
		vertices[3].Color.color = color3.toOpenGLColor();
	}
	else
	{
		vertices[0].Pos.X = drawRect.UpperLeftCorner.X	- 0.5f;
		vertices[0].Pos.Y = drawRect.UpperLeftCorner.Y	- 0.5f;
		vertices[1].Pos.X = drawRect.LowerRightCorner.X	- 0.5f;
		vertices[1].Pos.Y = drawRect.UpperLeftCorner.Y	- 0.5f;
		vertices[2].Pos.X = drawRect.LowerRightCorner.X	- 0.5f;
		vertices[2].Pos.Y = drawRect.LowerRightCorner.Y	- 0.5f;
		vertices[3].Pos.X = drawRect.UpperLeftCorner.X	- 0.5f;
		vertices[3].Pos.Y = drawRect.LowerRightCorner.Y	- 0.5f;

		vertices[0].Color.color = color0.color;
		vertices[1].Color.color = color1.color;
		vertices[2].Color.color = color2.color;
		vertices[3].Color.color = color3.color;
	}

	vertices[0].TCoords.set(texRect.UpperLeftCorner.X,	texRect.UpperLeftCorner.Y);
	vertices[1].TCoords.set(texRect.LowerRightCorner.X,	texRect.UpperLeftCorner.Y);
	vertices[2].TCoords.set(texRect.LowerRightCorner.X,	texRect.LowerRightCorner.Y);
	vertices[3].TCoords.set(texRect.UpperLeftCorner.X,	texRect.LowerRightCorner.Y);

	u16 indices[6];
	if (m_YInverted)
	{
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;
	}
	else
	{
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
	}

	registerGeometryForRendering(
		pass, core::matrix4(), NULL, NULL,
		material, vertices, 4, vtype, indices, 6, itype, dpt, z, mode);

	pass0.Layers[0].setTexture(0);
}

//---------------------------------------------------------------------------

void CNullDriver::_registerImageForRendering(
	E_RENDER_PASS pass, f32 z,
	const SMaterial & material,
	const core::rectf & drawRect, const core::rectf & texRect,
	const img::SColor & color0,	const img::SColor & color1,
	const img::SColor & color2,	const img::SColor & color3,
	E_RENDER_MODE mode)
{
	static E_VERTEX_TYPE			vtype	= EVT_1TCOORDS_COLOURED;
	static E_INDEX_TYPE				itype	= EIT_16_BIT;
	static E_DRAW_PRIMITIVE_TYPE	dpt		= EDPT_TRIANGLE_LIST;

	s32 xPlus = -(m_ScreenSize.Width>>1);
    f32 xFact = 1.0f / (m_ScreenSize.Width>>1);
    s32 yPlus = m_ScreenSize.Height-(m_ScreenSize.Height>>1);
    f32 yFact = 1.0f / (m_ScreenSize.Height>>1);

	static S3DVertex1TCoordsColoured vertices[4] =
	{
		S3DVertex1TCoordsColoured(0.0f, 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsColoured(0.0f, 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsColoured(0.0f, 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, 0xffffffff, 0.0f, 0.0f),
		S3DVertex1TCoordsColoured(0.0f, 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, 0xffffffff, 0.0f, 0.0f)
	};

	if (getDriverFamily() == vid::EDF_OPENGL)
	{
		vertices[0].Color.color = color0.toOpenGLColor();
		vertices[1].Color.color = color1.toOpenGLColor();
		vertices[2].Color.color = color2.toOpenGLColor();
		vertices[3].Color.color = color3.toOpenGLColor();
	}
	else
	{
		vertices[0].Color.color = color0.color;
		vertices[1].Color.color = color1.color;
		vertices[2].Color.color = color2.color;
		vertices[3].Color.color = color3.color;
	}

	vertices[0].Pos.X = (f32)(drawRect.UpperLeftCorner.X+xPlus) * xFact;
	vertices[0].Pos.Y = (f32)(yPlus-drawRect.UpperLeftCorner.Y) * yFact;
	vertices[1].Pos.X = (f32)(drawRect.LowerRightCorner.X+xPlus) * xFact;
	vertices[1].Pos.Y = (f32)(yPlus- drawRect.UpperLeftCorner.Y) * yFact;
	vertices[2].Pos.X = (f32)(drawRect.LowerRightCorner.X+xPlus) * xFact;
	vertices[2].Pos.Y = (f32)(yPlus-drawRect.LowerRightCorner.Y) * yFact;
	vertices[3].Pos.X = (f32)(drawRect.UpperLeftCorner.X+xPlus) * xFact;
	vertices[3].Pos.Y = (f32)(yPlus-drawRect.LowerRightCorner.Y) * yFact;

	vertices[0].TCoords.set(texRect.UpperLeftCorner.X,	texRect.UpperLeftCorner.Y);
	vertices[1].TCoords.set(texRect.LowerRightCorner.X,	texRect.UpperLeftCorner.Y);
	vertices[2].TCoords.set(texRect.LowerRightCorner.X,	texRect.LowerRightCorner.Y);
	vertices[3].TCoords.set(texRect.UpperLeftCorner.X,	texRect.LowerRightCorner.Y);

	u16 indices[6];
	if (m_YInverted)
	{
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;
	}
	else
	{
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
	}

	registerGeometryForRendering(
		pass, core::matrix4(), NULL, NULL,
		material, vertices, 4, vtype, indices, 6, itype, dpt, z, mode);
}

//--------------------------------------------------------------------------

void CNullDriver::register3DLineForRendering(
	E_RENDER_PASS pass,
	const core::matrix4 &transf,
	const core::vector3df &start, const core::vector3df &end,
    const img::SColor &color,
	bool depth_test,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
	static SMaterial material;

	static const E_VERTEX_TYPE			vtype	= EVT_SIMPLE_COLOURED;
	static const E_DRAW_PRIMITIVE_TYPE	dpt		= EDPT_LINE_LIST;

	static SRenderPass & pass0 = material.getPass(0);

	static bool init_done = false;
	if (!init_done)
	{
		pass0.setLightingMode(vid::ELM_NONE);
		pass0.setFlag(EMF_GOURAUD_SHADING, false);
		pass0.setFlag(EMF_MIP_MAP_OFF, true);
		init_done = true;
	}

	if (depth_test)
	{
		pass0.setFlag(EMF_ZWRITE_ENABLE, true);
		pass0.setDepthTest(ECT_LESS);
	}
	else
	{
		pass0.setFlag(EMF_ZWRITE_ENABLE, false);
		pass0.setDepthTest(ECT_ALWAYS);
	}

	if (useAlphaBlending)
	{
		pass0.setFlag( EMF_BLENDING, true );
		pass0.setBlendFuncs(ESBF_SRC_ALPHA, EDBF_ONE_MINUS_SRC_ALPHA);
		pass = ERP_3D_TRANSP_1ST_PASS;
	}
	else if (useColorBlending)
	{
		pass0.setFlag(EMF_BLENDING, true);
		pass0.setBlendFuncs(ESBF_ONE, EDBF_ONE_MINUS_SRC_COLOR);
		pass = ERP_3D_TRANSP_1ST_PASS;
	}
	else
	{
		pass0.setFlag(EMF_BLENDING, false);
	}

	static S3DVertexSimpleColoured vertices[2] =
	{
		S3DVertexSimpleColoured(0.0f, 0.0f , 0.0f, 0xffffffff),
		S3DVertexSimpleColoured(0.0f, 0.0f , 0.0f, 0xffffffff)
	};

	if (getDriverFamily() == vid::EDF_OPENGL)
	{
		vertices[0].Color.color = color.toOpenGLColor();
		vertices[1].Color.color = color.toOpenGLColor();
	}
	else
	{
		vertices[0].Color.color = color.color;
		vertices[1].Color.color = color.color;
	}

	vertices[0].Pos = start;
	vertices[1].Pos = end;

	registerGeometryForRendering(
		pass, transf, NULL, NULL, material,
		vertices, 2, vtype, NULL, 0, (E_INDEX_TYPE)0, dpt, -1.0f, mode);
}

//---------------------------------------------------------------------------

void CNullDriver::register3DBoxForRendering(
	E_RENDER_PASS pass,
	const core::matrix4 &transf,
	const core::aabbox3df &box, const img::SColor &color,
	bool depth_test,
	bool useAlphaBlending, bool useColorBlending,
	E_RENDER_MODE mode)
{
    static core::vector3df edges[8];
    box.getEdges(edges);
    
    // TODO: optimize into one big call.
    register3DLineForRendering(pass, transf, edges[5], edges[1], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[1], edges[3], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[3], edges[7], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[7], edges[5], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[0], edges[2], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[2], edges[6], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[6], edges[4], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[4], edges[0], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[1], edges[0], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[3], edges[2], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[7], edges[6], color, depth_test, useAlphaBlending, useColorBlending, mode);
    register3DLineForRendering(pass, transf, edges[5], edges[4], color, depth_test, useAlphaBlending, useColorBlending, mode);
}

#include <limits.h>

//---------------------------------------------------------------------------

const core::recti& CNullDriver::getScreenRectFrom3DVolume(
	const core::aabbox3df &box3d) const
{
	static core::recti r(INT_MAX, INT_MAX, INT_MIN, INT_MIN);

	core::vector3df edges[8];
	box3d.getEdges(edges);

	for (u32 i = 0; i < 8; i++)
	{
		core::vector2di pos = getScreenCoordinatesFrom3DPosition(edges[i]);

		if (r.UpperLeftCorner.X > pos.X)
			r.UpperLeftCorner.X = pos.X;
		if (r.UpperLeftCorner.Y > pos.Y)
			r.UpperLeftCorner.Y = pos.Y;
		if (r.LowerRightCorner.X < pos.X)
			r.LowerRightCorner.X = pos.X;
		if (r.LowerRightCorner.Y < pos.Y)
			r.LowerRightCorner.Y = pos.Y;
	}
	return r;
}

//---------------------------------------------------------------------------

void CNullDriver::_renderLightedRenderPools(
	core::array <SRenderPool*> & rpools)
{
	u32 rp_size = rpools.size();
	bool stencil = false, alpha = false;
	s32 stencil_lidx = -1, shadowground_lidx = -1;
	f32 zorder = -1;
	const scn::ISceneNode *owner = 0;

	core::position2di scissorPos(0, 0);
	core::dimension2di scissorDim(m_ScreenSize);
	core::recti scissorClip(0, 0, scissorDim.Width, scissorDim.Height);

	memset(m_RenderedLightedDIPsCount, 0, sizeof(m_RenderedLightedDIPsCount));
	memset(m_RenderedLightedTrianglesCount, 0, sizeof(m_RenderedLightedTrianglesCount));

	_enableScissor();

	_setupScissor(scissorPos.X, scissorPos.Y,
		scissorDim.Width, scissorDim.Height);

	u32 colmask = getColorMask();

	for (u32 i = 0; i < rp_size; i++)
	{
		SRenderPool &rpool = *rpools[i];

		u32 dips_count = getRenderedDIPsCount();
		u32 tris_count = getRenderedTrianglesCount();

		setTransform(ETS_MODEL, core::matrix4());

		// setup lighting
		_applyDynamicLights(rpool.Light0,  rpool.EnabledLights, rpool.EnabledLightIdx);

		// setup transformation
		setTransform(ETS_MODEL, rpool.Transform);

		s32 l = (u32)rpool.EnabledLightIdx;
		s32 dlcnt = getDynamicLightsCount();

		if (l < 0 || l >= dlcnt)
		{
			LOGGER.logErr("Incorrect Light index %d (light count %d) "
				"while rendering Lightng Pool (mode %d).", l, dlcnt, rpool.Mode);
			continue;
		}

		m_Profiler.startProfiling(m_ProfileLightingIds[rpool.Mode]);

		const vid::SLight & light = getDynamicLight(l);
		if (light.Enabled)
		{
			scissorPos.set(0, 0);
			scissorDim.set(m_ScreenSize);

			if (light.Type == ELT_POINT)
			{
				core::recti r = light.ScreenRect;
				r.clipAgainst(scissorClip);
				if (r.isValid())
				{
					scissorPos.set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y);
					scissorDim.set(r.getWidth(), r.getHeight());
				}
			}		

			switch (rpool.Mode)
			{
			case ERM_RENDER_INTO_STENCIL:
				{
					if (!stencil)
					{							
						stencil = true;
						_enableStencil();
					}
					else if (stencil_lidx != l || owner != rpool.Owner || zorder != rpool.Z)
					{
						clearStencil();
					}

					if (alpha && shadowground_lidx != l)
					{
						setColorMask(false, false, false, true);
						clearColor(BackColor);
						alpha = false;
					}

					setColorMask(false, false, false, false);

					stencil_lidx = l;
					owner = rpool.Owner;
					zorder = rpool.Z;

					_setupStencil(ESO_KEEP, ESO_KEEP, ESO_DECR, ECT_ALWAYS);

					_setupScissor(scissorPos.X, scissorPos.Y,
						scissorDim.Width, scissorDim.Height);

					u32 rb_size = rpool.RenderBuffers->size();
					for (u32 k = 0; k < rb_size; k++)
					{
						vid::IRenderBuffer *rb = (*rpool.RenderBuffers)[k];
						const vid::SMaterial *mat = (*rpool.Materials)[k];

						renderBuffer(rb, *mat);
					}
				}
				break;
			case ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZPASS:
			case ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZFAIL:
				{
					bool zfail =
						(rpool.Mode == ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZFAIL);
					if (!stencil)
					{
						stencil = true;
						_enableStencil();
					}
					else if (stencil_lidx != l || owner != rpool.Owner || zorder != rpool.Z)
					{
						clearStencil();
					}

					if (alpha && shadowground_lidx != l)
					{
						setColorMask(false, false, false, true);
						clearColor(BackColor);
						alpha = false;
					}

					setColorMask(false, false, false, false);

					stencil_lidx = l;
					owner = rpool.Owner;
					zorder = rpool.Z;

					_setupStencil(ESO_KEEP, ESO_KEEP, ESO_KEEP, ECT_ALWAYS);

					_setupScissor(scissorPos.X, scissorPos.Y,
						scissorDim.Width, scissorDim.Height);

					u32 rb_size = rpool.RenderBuffers->size();
					for (u32 k = 0; k < rb_size; k++)
					{
						vid::IRenderBuffer *rb = (*rpool.RenderBuffers)[k];
						const vid::SMaterial *mat = (*rpool.Materials)[k];

						_renderStencilVolume(rb, mat->getPass(0), zfail);
					}
					//_setupStencil(ESO_KEEP, ESO_KEEP, ESO_KEEP, ECT_ALWAYS);
					setColorMask(colmask);
				}
				break;
			case ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR:
			case ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_ALPHA:
				{
					if (stencil)
					{
						if (stencil_lidx != l || owner != rpool.Owner || zorder != rpool.Z)
						{
							clearStencil();
							_disableStencil();
							stencil = false;
						}
						else
  						{
							_setupStencil(ESO_KEEP, ESO_KEEP, ESO_KEEP,
								ECT_NOTEQUAL, MY_STENCIL_ZERO_VALUE);
						}
					}

					if (alpha && shadowground_lidx != l)
					{
						setColorMask(false, false, false, true);
						clearColor(BackColor);
						alpha = false;
					}

					if (rpool.Mode == ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_ALPHA)
					{
						setColorMask(false, false, false, true);
						alpha = true;
					}
					else
						setColorMask(true, true, true, false);

					shadowground_lidx = l;

					core::vector3df light_pos      = light.Position;
					core::vector3df lighted_center = light.LightedVolumeCenter;

					_setupScissor(scissorPos.X, scissorPos.Y,
						scissorDim.Width, scissorDim.Height);

					// render lighted geometry
					vid::SRenderPass pass;
					vid::STextureLayer layer1;

					u32 rb_size = rpool.RenderBuffers->size();
					for (u32 k = 0; k < rb_size; k++)
					{
						vid::IRenderBuffer * rb = (*rpool.RenderBuffers)[k];
						const vid::SMaterial * mat = (*rpool.Materials)[k];

						u32 passes_size = mat->getPassesCount();
						for (u32 p = 0; p < passes_size; p++)
						{
							const vid::SRenderPass &pass = mat->getPass(0);
							if (light.Type == ELT_POINT &&
								rpool.Mode == ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR)
							{
								vid::SRenderPass pass = mat->getPass(0);
								f32 lr2 = 2 * light.Radius;
								layer1.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
								layer1.setProjectionParams(lr2, lr2, 
									light_pos, core::vector3df(0,-1,0), core::vector3df(1,0,0));
								layer1.setTexCoordAddr( vid::ETCAM_CLAMP );
								layer1.setTexture(m_LightSphereWhiteTexture, vid::ETLT_ATTENUATION_MAP);
								pass.Layers[0] = layer1;
								pass.setLightingMode(vid::ELM_NONE);

								renderBuffer(rb, pass);
							}
							else
								renderBuffer(rb, pass);
						}
					}
					setColorMask(colmask);
				}
				break;
			case ERM_RENDER_WITH_STENCIL_TEST_LIGHTING:
				{
					if (stencil)
					{
						if (stencil_lidx != l || owner != rpool.Owner || zorder != rpool.Z)
						{
							clearStencil();
							_disableStencil();
							stencil = false;
						}
						else
						{
							_setupStencil(ESO_KEEP, ESO_KEEP, ESO_KEEP,
								ECT_EQUAL, MY_STENCIL_ZERO_VALUE);
						}
					}

					if (alpha && shadowground_lidx != l)
					{
						setColorMask(false, false, false, true);
						clearColor(BackColor);
						alpha = false;
						setColorMask(colmask);
					}

					core::vector3df light_pos      = light.Position;
					core::vector3df lighted_center = light.LightedVolumeCenter;

					_setupScissor(scissorPos.X, scissorPos.Y,
						scissorDim.Width, scissorDim.Height);

					// render lighted geometry
					u32 rb_size = rpool.RenderBuffers->size();
					
					for (u32 k = 0; k < rb_size; k++)
					{
						vid::IRenderBuffer * rb = (*rpool.RenderBuffers)[k];
						const vid::SMaterial * mat = (*rpool.Materials)[k];

						u32 passes_size = mat->getPassesCount();
						for (u32 p = 0; p < 1 && p < passes_size; p++)
						{					
							f32 lr2 = 2 * light.Radius;

							vid::SRenderPass pass;
							vid::STextureLayer layer1, layer2, layer3;

							// border texture 1
							layer1.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
							layer1.setProjectionParams(
								light.LightedVolumeSize.Z, light.LightedVolumeSize.X, 
								lighted_center, core::vector3df(0,-1,0), core::vector3df(1,0,0));	
							layer1.setTexCoordAddr(vid::ETCAM_CLAMP);
							layer1.setTexture(m_WhiteTexture, vid::ETLT_ATTENUATION_MAP);
							layer1.enableFilter(false);

							// border texture 2
							layer2.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
							layer2.setProjectionParams(
								lr2, light.LightedVolumeSize.Y, 
								core::vector3df(light_pos.X, lighted_center.Y, light_pos.Z), 
								core::vector3df(1,0,0), core::vector3df(0,1,0));	
							layer2.setTexCoordAddr( vid::ETCAM_CLAMP);
							layer2.setTexture(m_LightGradientTexture, vid::ETLT_ATTENUATION_MAP);

							if (m_UseShaders && m_ShadersHighQuality)
							{
								const vid::SRenderPass &origPass = mat->getPass(p);

								pass.Layers[0] = origPass.Layers[0];
								pass.Layers[1] = layer1;
								pass.Layers[2] = layer2;
								pass.Layers[3] = origPass.Layers[3]; // normal map

								pass.setAmbientColor(origPass.getAmbientColor());
								pass.setDiffuseColor(origPass.getDiffuseColor());
								pass.setEmissiveColor(0x00000000);
								pass.setSpecularColor(origPass.getSpecularColor());
								pass.setShininess(origPass.getShininess());

								pass.setLightingMode(vid::ELM_8_LIGHTS);
							}
							else
							{
								layer3.setTexCoordGen(vid::ETCGT_PROJECTED_MAPPING);
								layer3.setProjectionParams(lr2, lr2, 
									light_pos, core::vector3df(0,-1,0), core::vector3df(1,0,0));
								layer3.setTexCoordAddr(vid::ETCAM_CLAMP);
								layer3.setTexture(m_LightSphereTexture, vid::ETLT_ATTENUATION_MAP);

								pass.Layers[0] = mat->getPass(p).Layers[0];
								pass.Layers[1] = layer1;
								pass.Layers[2] = layer2;
								pass.Layers[3] = layer3;

								pass.setLightingMode(vid::ELM_NONE);

								pass.setAmbientColor(0x000000);
								pass.setDiffuseColor(0x000000);
								pass.setEmissiveColor(light.DiffuseColor.toSColor());
								pass.setSpecularColor(0x000000);

								pass.setColorGen(vid::ECGT_CUSTOM);
							}
							pass.setFlag(EMF_MIP_MAP_OFF,		false);
							pass.setFlag(EMF_GOURAUD_SHADING,	false);
							pass.setFlag(EMF_BLENDING,			true);
							pass.setFlag(EMF_FOG_ENABLE,		true);
							pass.setFlag(EMF_ZWRITE_ENABLE,		false);
							pass.setDepthTest(vid::ECT_EQUAL);
							pass.setBlendFuncs(vid::ESBF_ONE_MINUS_DST_ALPHA, vid::EDBF_ONE);

							renderBuffer(rb, pass);
						}
					}
				}
				break;
			default:
				LOGGER.logErr("Unknown render mode=%d in lighting pass",
					rpool.Mode);
				break;
			}
		}

		dips_count = getRenderedDIPsCount() - dips_count;
		tris_count = getRenderedTrianglesCount() - tris_count;

		m_RenderedLightedTrianglesCount[rpool.Mode] += tris_count;
		m_RenderedLightedDIPsCount[rpool.Mode] += dips_count;

		m_Profiler.stopProfiling(m_ProfileLightingIds[rpool.Mode]);
	}

	_disableScissor();

	if (stencil)
	{
		_disableStencil();
		clearStencil();
	}
}

//---------------------------------------------------------------------------

void CNullDriver::renderBuffer(IRenderBuffer *rbuf, const SRenderPass &pass)
{
	if (!((CNullRenderBuffer*)rbuf)->bind())
	{
		LOGGER.logErr(__FUNCTION__ ": Can not bind buffer to render!");
		return;
	}

	_renderBuffer(rbuf, pass);

	((CNullRenderBuffer*)rbuf)->unbind();
}

//---------------------------------------------------------------------------

void CNullDriver::renderBuffer(IRenderBuffer *rbuf, const SMaterial &mat)
{
	if (!((CNullRenderBuffer*)rbuf)->bind())
	{
		LOGGER.logErr(__FUNCTION__ ": Can not bind buffer to render!");
		return;
	}

	u32 pCnt = mat.getPassesCount();
	for (u32 p = 0; p < pCnt; p++)
		_renderBuffer(rbuf, mat.getPass(p));

	((CNullRenderBuffer*)rbuf)->unbind();
}

//---------------------------------------------------------------------------

void CNullDriver::_renderBuffer(IRenderBuffer *rbuf, const SRenderPass &pass)
{
	m_TrianglesDrawn += rbuf->getPrimitiveCount();
	m_DIPsDrawn++;

	setRenderPass(pass);
	_setVertexType(rbuf->getVertices()->getType());
	_setRenderStates();
	
	u32 lsize = getDynamicLightsCount();

	u32 granSize = (m_UseShaders && m_CurrentGPUProgram) ?
		m_CurrentGPUProgram->getLightsCount() : PRL_MAX_FFP_LIGHTS;
	u32 maskBits = (granSize == PRL_MAX_FFP_LIGHTS) ?
		0xff : 0xf;

	if (m_EnabledLights.size() > granSize)
	// render lighted DIP in multipass (if needed)
	{	
		u64 lBits = m_LightsBits;
		bool flagBlend = m_CurrentRenderPass.getFlag(vid::EMF_BLENDING);
		vid::E_BLEND_EQUATION blenEq = m_CurrentRenderPass.getBlendEquation();
		vid::E_SRC_BLEND_FUNCTION blendSrcFunc = m_CurrentRenderPass.getSrcBlendFunc();
		vid::E_DST_BLEND_FUNCTION blendDstFunc = m_CurrentRenderPass.getDstBlendFunc();
		vid::E_COMPARE_TEST depthTest = m_CurrentRenderPass.getDepthTest();
		img::SColor colEmiss = m_CurrentRenderPass.getEmissiveColor();
		img::SColorf globAmb = getGlobalAmbientColor();

		bool firstPass = true;
		for (u32 i = 0; i < lsize; i += granSize)
		{
			m_LightsBits = lBits & (maskBits << i);
			u32 enabled = 0;
			if (m_CurrentRenderPass.isLighting())
			{
				for (u32 j = i; j < i + granSize; j++)
				{
					if (m_LightsBits & (u64)(1 << j))
						enabled++;
				}
				for (u32 j = i + granSize; enabled < granSize && j < lsize; j++)
				{
					if (lBits & (u64)(1 << j))
					{
						m_LightsBits |= (u64)(1 << j);
						enabled++;
						i++;
					}
				}
			}
			else
				m_LightsBits = 0;

			if (!firstPass && !enabled)
				break;

			applyDynamicLights();

			if (!firstPass)
			{
				setGlobalAmbientColor(img::SColorf(0.0f, 0.0f, 0.0f, 0.0f));
				m_CurrentRenderPass.setEmissiveColor(0x00000000);
				m_CurrentRenderPass.setFlag(vid::EMF_BLENDING, true);
				m_CurrentRenderPass.setBlendEquation(vid::EBE_ADD);
				m_CurrentRenderPass.setBlendFuncs(vid::ESBF_ONE, vid::EDBF_ONE);
				m_CurrentRenderPass.setDepthTest(vid::ECT_LEQUAL);
				m_TrianglesDrawn += rbuf->getPrimitiveCount();
				m_DIPsDrawn++;
				_setRenderStates();
			}
			((CNullRenderBuffer*)rbuf)->render();

			firstPass = false;
		}
		setGlobalAmbientColor(globAmb);
		m_CurrentRenderPass.setEmissiveColor(colEmiss);
		m_CurrentRenderPass.setFlag(vid::EMF_BLENDING, flagBlend);
		m_CurrentRenderPass.setBlendEquation(blenEq);
		m_CurrentRenderPass.setBlendFuncs(blendSrcFunc, blendDstFunc);
		m_CurrentRenderPass.setDepthTest(depthTest);
		if (m_LightsBits != lBits)
		{
			m_LightsBits = lBits;
			applyDynamicLights();
		}
	}
	else
	{
		((CNullRenderBuffer*)rbuf)->render();
	}
}

//---------------------------------------------------------------------------

void CNullDriver::renderBufferWithLight(IRenderBuffer *rbuf, const SMaterial &mat, u32 enabledLight)
{
	if (!((CNullRenderBuffer*)rbuf)->bind())
	{
		LOGGER.logErr(__FUNCTION__ ": Can not bind buffer to render!");
		return;
	}

	u32 pCnt = mat.getPassesCount();
	for (u32 p = 0; p < pCnt; p++)
		_renderBuffer(rbuf, mat.getPass(p), enabledLight);

	((CNullRenderBuffer*)rbuf)->unbind();
}

//---------------------------------------------------------------------------

void CNullDriver::_renderBuffer(IRenderBuffer *rbuf, const SRenderPass &pass, u32 enabledLight)
{
	u32 lsize = getDynamicLightsCount();
	u64 lBits = m_LightsBits;

	if (enabledLight >= lsize)
		return;

	m_LightsBits  = u64(1 << enabledLight);
	applyDynamicLights();

	setRenderPass(pass);
	_setVertexType(rbuf->getVertices()->getType());
	_setRenderStates();

	((CNullRenderBuffer*)rbuf)->render();

	m_TrianglesDrawn += rbuf->getPrimitiveCount();
	m_DIPsDrawn++;

	if (m_LightsBits != lBits)
	{
		m_LightsBits = lBits;
		applyDynamicLights();
	}
}

//---------------------------------------------------------------------------

void CNullDriver::_renderStencilVolume(IRenderBuffer *rbuf, const SRenderPass &pass, bool zfail)
{
	m_TrianglesDrawn += (m_TwoSidedStencil ? 1 : 2) * rbuf->getPrimitiveCount();
	m_DIPsDrawn += (m_TwoSidedStencil ? 1 : 2) ;

	setRenderPass(pass);
	_setVertexType(rbuf->getVertices()->getType());
	_setRenderStates();
}

//---------------------------------------------------------------------------

void CNullDriver::_sort()
{
	m_Profiler.startProfiling(m_ProfileSortDIPs);
	for (u32 i = 0; i < E_RENDER_PASS_COUNT; i++)
	{
		core::array < SRenderPool* > &rpools_all = m_RenderData[i];
		core::array < SRenderPool > &rpools = m_RenderPools[i];
		const core::array <SRenderPool*> &rpools_cache =
			m_RenderPoolsCache[m_RenderPoolsCacheIndex][i].get_as_array();
		u32 s1 = rpools.size(), s2 = rpools_cache.size();
		rpools_all.set_used(s1 + s2);
		for (u32 j = 0; j < s1; j++)
			rpools_all[j] = &rpools[j];
		memcpy(rpools_all.pointer() + s1,
			rpools_cache.const_pointer(), s2 * sizeof(SRenderPool*));
		rpools_all.set_sorted(false);
		rpools_all.sort_ptr();
	}
	m_Profiler.stopProfiling(m_ProfileSortDIPs);
}

//---------------------------------------------------------------------------

void CNullDriver::renderAll()
{
	for (u32 i = 0; i < E_RENDER_PASS_COUNT; i++)
		renderPass((E_RENDER_PASS)i);
}

//---------------------------------------------------------------------------

void CNullDriver::renderPass(E_RENDER_PASS pass)
{
	(m_RenderPath == ERP_FORWARD_RENDERING) ?
		_renderForward(pass) : _renderDeferred(pass);
}

//---------------------------------------------------------------------------

void CNullDriver::_renderForward(E_RENDER_PASS pass)
{
	if (!m_Rendering)
		return;

	u32 sys_time_ms = TIMER.getSystemTime();

	core::matrix4 m_proj = Matrices[ETS_PROJ];
	core::matrix4 m_view = Matrices[ETS_VIEW];
	core::matrix4 m_model = Matrices[ETS_MODEL];

	u32 i = pass;
	{
		m_CurrentRenderPassType = (E_RENDER_PASS)i;

		m_Profiler.startProfiling(m_ProfileIds[i]);

		u32 dips_count = getRenderedDIPsCount();
		u32 tris_count = getRenderedTrianglesCount();

		core::array <SRenderPool*> &rpools_all = m_RenderData[i];

		u32 rp_size = rpools_all.size();
		if (rp_size)
		{
			switch (i)
			{
			case ERP_3D_SKY_PASS:
			case ERP_3D_SOLID_PASS:
			case ERP_3D_DIRT_PASS:
			case ERP_3D_TRANSP_1ST_PASS:
			case ERP_3D_TRANSP_2ND_PASS:
			case ERP_3D_TRANSP_3RD_PASS:
			case ERP_GUI_3D_SOLID_PASS:
			case ERP_GUI_3D_TRANSP_PASS:
				{
					// setting transformations
					core::matrix4 m_proj_shift = m_proj;
					if (i == ERP_3D_DIRT_PASS)
					{
						core::vector3df tr = m_proj_shift.getTranslation();
						tr.Z -= 0.001f; // shift matrix in z-direction, to avoid potential Z-fighting
						m_proj_shift.setTranslation(tr);
					}
					
					setTransform(ETS_PROJ, m_proj_shift);
					setTransform(ETS_VIEW, m_view);

					for (u32 j = 0; j < rp_size; j++)
					{
						SRenderPool & rpool = *rpools_all[j];
						rpool.update ( sys_time_ms );

						setTransform(ETS_MODEL, core::matrix4());

						// setup lighting
						_applyDynamicLights(
							rpool.Light0, rpool.EnabledLights, rpool.EnabledLightIdx);

						// setup transformation
						setTransform(ETS_MODEL, rpool.Transform);

						// render geometry
						u32 rb_size = rpool.RenderBuffers->size();
						for ( u32 k = 0;  k < rb_size; k++ )
						{
							vid::IRenderBuffer *rb = (*rpool.RenderBuffers)[k];
							const vid::SMaterial *mat = (*rpool.Materials)[k];

							renderBuffer(rb, *mat);
						}
					}
				}
				break;
			case ERP_3D_LIGHTING_PASS:
				{
					// setting transformations
					setTransform(ETS_PROJ, m_proj);
					setTransform(ETS_VIEW, m_view);

					// save settings
					E_TEXTURE_FILTER old_texfilter = getTextureFilter();
					img::SColorf old_global_amb_color = getGlobalAmbientColor();

					// change settings
					setGlobalAmbientColor(img::SColorf(0,0,0,0));	
					if (old_texfilter != ETF_NONE)
						setTextureFilter(ETF_BILINEAR);

					_renderLightedRenderPools(rpools_all);		

					setGlobalAmbientColor(old_global_amb_color);
					setTextureFilter(old_texfilter);
				}
				break;
			case ERP_2D_PASS:
			case ERP_GUI_2D_PASS:
				{
					// zero transformations
					setTransform(ETS_PROJ, core::matrix4());
					setTransform(ETS_VIEW, core::matrix4());
					setTransform(ETS_MODEL, core::matrix4());

					for ( u32 j = 0; j < rp_size; j++ )
					{
						SRenderPool & rpool = *rpools_all[j];
						rpool.update ( sys_time_ms );

						// render geometry
						u32 rb_size = rpool.RenderBuffers->size();
						for ( u32 k = 0;  k < rb_size; k++ )
						{
							vid::IRenderBuffer *rb = (*rpool.RenderBuffers)[k];
							const vid::SMaterial *mat = (*rpool.Materials)[k];
	
							renderBuffer(rb, *mat);
						}
					}
				}
				break;
			default:
				LOGGER.logErr("Unknown render pass %s (%d)",
					vid::RenderPassName[i], i);
				break;
			}
		}

		dips_count = getRenderedDIPsCount() - dips_count;
		tris_count = getRenderedTrianglesCount() - tris_count;

		m_RenderedDIPsCount[i]		+= dips_count;
		m_RenderedTrianglesCount[i] += tris_count;

		static core::stringc profileInfo;
		profileInfo = "";

		if (m_Profiler.isProfiling())
			profileInfo.sprintf("(%6d tris, %4d dips)", tris_count, dips_count);

		m_Profiler.stopProfiling(m_ProfileIds[i], profileInfo.c_str());
	}
}

//---------------------------------------------------------------------------

void CNullDriver::_renderDeferred(E_RENDER_PASS pass)
{
	if (!m_Rendering)
		return;

	setColorMask(true, true, true, true);

	u32 sys_time_ms = TIMER.getSystemTime();

	core::matrix4 m_proj = Matrices[ETS_PROJ];
	core::matrix4 m_view = Matrices[ETS_VIEW];
	core::matrix4 m_model = Matrices[ETS_MODEL];

	u32 i = pass;
	{
		m_CurrentRenderPassType = (E_RENDER_PASS)i;

		m_Profiler.startProfiling(m_ProfileIds[i]);

		u32 dips_count = getRenderedDIPsCount();
		u32 tris_count = getRenderedTrianglesCount();

		core::array <SRenderPool*> &rpools_all = m_RenderData[i];

		u32 rp_size = rpools_all.size();
		if (rp_size)
		{
			switch (i)
			{
			case ERP_3D_SKY_PASS:
			case ERP_3D_TRANSP_1ST_PASS:
			case ERP_3D_TRANSP_2ND_PASS:
			case ERP_3D_TRANSP_3RD_PASS:
			case ERP_GUI_3D_SOLID_PASS:
			case ERP_GUI_3D_TRANSP_PASS:
			case ERP_3D_DIRT_PASS:
				break;
			case ERP_3D_SOLID_PASS:
				{
					// setting transformations
					core::matrix4 m_proj_shift = m_proj;
					if (i == ERP_3D_DIRT_PASS)
					{
						core::vector3df tr = m_proj_shift.getTranslation();
						tr.Z -= 0.001f; // shift matrix in z-direction, to avoid potential Z-fighting
						m_proj_shift.setTranslation(tr);
					}
					
					setTransform(ETS_PROJ, m_proj_shift);
					setTransform(ETS_VIEW, m_view);

					for (u32 j = 0; j < rp_size; j++)
					{
						SRenderPool & rpool = *rpools_all[j];
						rpool.update ( sys_time_ms );

						setTransform(ETS_MODEL, core::matrix4());

						// setup lighting
						_applyDynamicLights(
							NULL, 0, -1);

						// setup transformation
						setTransform(ETS_MODEL, rpool.Transform);

						// render geometry
						u32 rb_size = rpool.RenderBuffers->size();
						for ( u32 k = 0;  k < rb_size; k++ )
						{
							vid::IRenderBuffer *rb = (*rpool.RenderBuffers)[k];
							const vid::SMaterial *mat = (*rpool.Materials)[k];

							renderBuffer(rb, mat->getPass(0));
						}
					}
				}
				break;
			case ERP_3D_LIGHTING_PASS:
				break;
			case ERP_2D_PASS:
			case ERP_GUI_2D_PASS:
				break;
			default:
				LOGGER.logErr("Unknown render pass %s (%d)",
					vid::RenderPassName[i], i);
				break;
			}
		}

		dips_count = getRenderedDIPsCount() - dips_count;
		tris_count = getRenderedTrianglesCount() - tris_count;

		m_RenderedDIPsCount[i]		+= dips_count;
		m_RenderedTrianglesCount[i] += tris_count;

		static core::stringc profileInfo;
		profileInfo = "";

		if (m_Profiler.isProfiling())
			profileInfo.sprintf("(%6d tris, %4d dips)", tris_count, dips_count);

		m_Profiler.stopProfiling(m_ProfileIds[i], profileInfo.c_str());
	}
}

//---------------------------------------------------------------------------

bool CNullDriver::_beginRendering()
{
	return true;
}

//---------------------------------------------------------------------------

bool CNullDriver::_endRendering()
{
	return true;
}

//---------------------------------------------------------------------------

bool CNullDriver::beginRendering()
{
	if (m_Rendering)
		return false;

	_sort();

	m_Profiler.startProfiling(m_ProfileRender);

	m_Rendering = true;
	m_ResetRenderStates = true;
	m_CurrentVertexType = (E_VERTEX_TYPE)-1;

	_beginRendering();

	m_TrianglesDrawn = 0;
	m_DIPsDrawn = 0;

	memset(m_RenderedDIPsCount, 0, sizeof(m_RenderedDIPsCount));
	memset(m_RenderedTrianglesCount, 0, sizeof(m_RenderedTrianglesCount));

	BackColor.setAlpha(0);

	setColorMask(true, true, true, true);	

	clearColor(BackColor);
	clearDepth();

	setColorMask(true, true, true, false);

	setGlobalAmbientColor(getGlobalAmbientColor());

	resetTextureUnits();

	return true;
}

//---------------------------------------------------------------------------

void CNullDriver::endRendering()
{
	if (!m_Rendering)
		return;

	// reset render data
	for (u32 i = 0; i < E_RENDER_PASS_COUNT; i++)
	{
		core::array <SRenderPool*> &rpools_all = m_RenderData[i];
		u32 rp_size = rpools_all.size();
		for ( u32 j = 0; j < rp_size; j++ )
		{
			SRenderPool *rpool = rpools_all[j];
			if (rpool->Type == ERPT_RENDER_CACHE)
				rpool->setIdentity();
		}
		rpools_all.set_used(0);
	}

	// clear unused cache
	u32 cache_index = ( m_RenderPoolsCacheIndex + 1 ) % 2;
	for ( u32 i = 0; i < E_RENDER_PASS_COUNT; i++ )
	{
		m_RenderPools[i].clear();
		m_RenderPoolsCache[cache_index][i].clear_ptr();
	}

	if (m_UseShaders)
		_bindGPUProgram(NULL);

	m_RenderPoolsCacheIndex = cache_index;

	_endRendering();

	m_Profiler.stopProfiling(m_ProfileRender);

	if (m_Profiler.isProfiling())
	{
		static core::stringc profileInfo;
		profileInfo = "";

		for (unsigned i = 0 ; i < E_RENDER_MODE_COUNT; i++)
		{
			profileInfo.sprintf("(%6d tris, %4d dips)",
				m_RenderedLightedTrianglesCount[i], m_RenderedLightedDIPsCount[i]);
			m_Profiler.setProfileInfo(m_ProfileLightingIds[i], profileInfo.c_str());
		}
	}

	m_Rendering = false;
}

//---------------------------------------------------------------------------

void CNullDriver::swapBuffers()
{
	m_Profiler.startProfiling(m_ProfileSwapBuffers);

	_swapBuffers();

	m_RenderCount++;

	FPSCounter.registerEvent( TIMER.getSystemTime() );

	static s32 sum=0;
    s32 fps = getCurrentFPS();
    s32 avg = 0;
    if (sum!=0 || fps>0)
    {  
        sum += fps;
        avg=sum / m_RenderCount;
    }
	AverageFPS = avg;	

	m_Profiler.stopProfiling(m_ProfileSwapBuffers);
}

//---------------------------------------------------------------------------

bool CNullDriver::isRendering()
{
	return m_Rendering;
}

//---------------------------------------------------------------------------

const u32& CNullDriver::getDynamicLightsMaximalAmount() const
{
    return m_MaxLights;
}

//---------------------------------------------------------------------------

s32 CNullDriver::getDynamicLightsCount()
{
    return m_Lights.size();
}

//---------------------------------------------------------------------------

void CNullDriver::deleteAllDynamicLights()
{
	m_LastEnabledLightsCount = -1;
	m_EnabledLights.set_used(0);
    m_Lights.set_used(0);
}

//---------------------------------------------------------------------------

void CNullDriver::addDynamicLight(const SLight& light)
{
	if (light.Type == ELT_POINT && light.Radius<=0)
		return;
	m_Lights.push_back(light);
}

//---------------------------------------------------------------------------

const SLight& CNullDriver::getDynamicLight(s32 idx)
{
    if (idx<0 || idx>=(s32)m_Lights.size())
        return *((SLight*)0);

    return m_Lights[idx];
}

//---------------------------------------------------------------------------

void CNullDriver::setDynamicLight(s32 idx, const SLight & light)
{
	if (idx < 0 || idx >= (s32)m_Lights.size())
        return;
    m_Lights[idx] = light;
}

//---------------------------------------------------------------------------

void CNullDriver::setDynamicLightEnabled(s32 idx, bool enabled)
{
	if (idx < 0 || idx >= (s32)m_Lights.size())
        return;
    m_Lights[idx].Enabled = enabled;
	if (enabled)
		m_LightsBits |= (u64)(1 << idx);
	else
		m_LightsBits &= ~(u64)(1 << idx);
}

//---------------------------------------------------------------------------

bool CNullDriver::isDynamicLightEnabled(s32 idx)
{
	if (idx < 0 || idx >= (s32)m_Lights.size())
        return false;

	return m_Lights[idx].Enabled;
}

//---------------------------------------------------------------------------

f64 CNullDriver::getFrameCurrentRenderTimeSec()
{
	return m_Profiler.getProfileParameter(m_ProfileRender)->CurrentTimeSec;
}

//---------------------------------------------------------------------------

f64 CNullDriver::getFrameFilteredRenderTimeSec()
{
	return m_Profiler.getProfileParameter(m_ProfileRender)->FilteredTimeSec;
}

//---------------------------------------------------------------------------

E_UNIFORM_TYPE CNullDriver::getUniformType(const c8 *name)
{
	E_UNIFORM_TYPE type = m_UniformTypes.get_value(core::const_stringc(name));
	return type;
}

//---------------------------------------------------------------------------

bool CNullDriver::_bindGPUProgram(IGPUProgram* gpu_prog)
{
	SAFE_DROP(m_CurrentGPUProgram);
	m_CurrentGPUProgram = gpu_prog;
	SAFE_GRAB(m_CurrentGPUProgram);

	if (!gpu_prog)
		return true;
	if (!gpu_prog->isOK())
		return false;

	bool res = true;

	const SRenderPass &pass = m_CurrentRenderPass;
	u32 mask = gpu_prog->getUniformMask();

	if (mask & EUF_MODEL_VIEW_PROJ_MATRIX)
	{
		res = gpu_prog->setUniformfv(EUT_MODEL_VIEW_PROJ_MATRIX,
			m_HWModelViewProjMatrix.pointer(), 16 * sizeof(f32)) && res;
	}
	if (mask & EUF_MODEL_VIEW_MATRIX)
	{
		res = gpu_prog->setUniformfv(EUT_MODEL_VIEW_MATRIX,
			m_ModelViewMatrix.pointer(), 16 * sizeof(f32)) && res;
	}
	if (mask & EUF_MODEL_MATRIX)
	{
		res = gpu_prog->setUniformfv(EUT_MODEL_MATRIX,
			Matrices[ETS_MODEL].pointer(), 16 * sizeof(f32)) && res;
	}
	if (mask & EUF_MODEL_VIEW_MATRIX_3X3)
	{
		f32 *mvm = m_ModelViewMatrix.pointer();
		f32 nm[9] =
		{
			mvm[0], mvm[1], mvm[2],
			mvm[4], mvm[5], mvm[6],
			mvm[8], mvm[9], mvm[10],
		};
		res = gpu_prog->setUniformfv(EUT_MODEL_VIEW_MATRIX_3X3,
			nm, sizeof(nm)) && res;
	}
	if (mask & EUF_MODEL_MATRIX_3X3)
	{
		f32 *mvm = Matrices[ETS_MODEL].pointer();
		f32 nm[9] =
		{
			mvm[0], mvm[1], mvm[2],
			mvm[4], mvm[5], mvm[6],
			mvm[8], mvm[9], mvm[10],
		};
		res = gpu_prog->setUniformfv(EUT_MODEL_MATRIX_3X3,
			nm, sizeof(nm)) && res;
	}

	if (mask & EUF_GLOBAL_AMBIENT_COLOR)
	{
		res = gpu_prog->setUniformfv(EUT_GLOBAL_AMBIENT_COLOR,
			(f32*)&GlobalAmbientColor, sizeof(GlobalAmbientColor)) && res;
	}
	if (mask & EUF_MATERIAL_COLORS)
	{
		struct SUniformMaterialColors
		{
			img::SColorf	DiffuseColor;
			img::SColorf	AmbientColor;			
			img::SColorf	SpecularColor;
			img::SColorf	EmissiveColor;
		}
		matColors;
		matColors.DiffuseColor = img::SColorf(pass.getDiffuseColorCurrent());
		matColors.AmbientColor = img::SColorf(pass.getAmbientColor());
		matColors.SpecularColor = img::SColorf(pass.getSpecularColor());
		matColors.EmissiveColor = img::SColorf(pass.getEmissiveColorCurrent());
		res = gpu_prog->setUniformfv(EUT_MATERIAL_COLORS,
			(f32*)&matColors, sizeof(matColors)) && res;
	}
	if (mask & EUF_MATERIAL_SHININESS)
	{
		f32 shininess = pass.getShininess();
		CHECK_RANGE(shininess, 0.001f, 128.0f);
		res = gpu_prog->setUniform1f(EUT_MATERIAL_SHININESS,
			shininess) && res;
	}

	if (mask & EUF_LIGHTING)
	{
		s32 lightcnt = gpu_prog->getLightsCount();
		s32 actual_lightcnt = m_EnabledLights.size();
		SUniformLight Lights[PRL_MAX_SHADER_LIGHTS];
		for (s32 i = 0; i < lightcnt; i++)
		{
			SUniformLight &l = Lights[i];

			if (i >= actual_lightcnt)
			{
				memset(&l, 0, sizeof(l));
				continue;
			}

			const SLight &light = m_Lights[m_EnabledLights[i]];

			l.DiffuseColor = light.DiffuseColor;
			l.AmbientColor = light.AmbientColor;			
			l.SpecularColor = light.SpecularColor;
			l.Position = light.Position;
			Matrices[ETS_VIEW].transformVect(l.Position);

			if (light.Type == vid::ELT_POINT)
			{
				l.Radius = light.Radius;

				// Light amplify
				f32 ampl = 2.0f;
				l.DiffuseColor.r *= ampl; 
				l.DiffuseColor.g *= ampl; 
				l.DiffuseColor.b *= ampl; 
				l.AmbientColor.r *= ampl;
				l.AmbientColor.g *= ampl;
				l.AmbientColor.b *= ampl;
			}
			else
			{
				core::vector3df orig(0.f, 0.f, 0.f);
				Matrices[ETS_VIEW].transformVect(orig);
				l.Position -= orig;

				// This is a trick, to emulate directional light via point light
				l.Radius = 10000000.0f;
				l.Position.normalize();				
				l.Position *= l.Radius / 10.f;
			}
		}
		res = gpu_prog->setUniformfv(EUT_LIGHTING,
			(f32*)Lights, lightcnt * sizeof(*Lights)) && res;
	}

	if (mask & EUF_EYE_POS)
	{
		core::vector3df eye_pos(0,0,0);
		m_ViewInvMatrix.transformVect(eye_pos);

		res = gpu_prog->setUniformfv(EUT_EYE_POS,
			(f32*)&eye_pos, sizeof(eye_pos)) && res;
	}

	if (mask & EUF_TEXTURE_MATRIX0)
		res = gpu_prog->setUniformfv(EUT_TEXTURE_MATRIX0,
			pass.Layers[0].getTexMatrixCurrent().pointer(), 16 * sizeof(f32)) && res;
	if (mask & EUF_TEXTURE_MATRIX1)
		res = gpu_prog->setUniformfv(EUT_TEXTURE_MATRIX1,
			pass.Layers[1].getTexMatrixCurrent().pointer(), 16 * sizeof(f32)) && res;
	if (mask & EUF_TEXTURE_MATRIX2)
		res = gpu_prog->setUniformfv(EUT_TEXTURE_MATRIX2,
			pass.Layers[2].getTexMatrixCurrent().pointer(), 16 * sizeof(f32)) && res;
	if (mask & EUF_TEXTURE_MATRIX3)
		res = gpu_prog->setUniformfv(EUT_TEXTURE_MATRIX3,
			pass.Layers[3].getTexMatrixCurrent().pointer(), 16 * sizeof(f32)) && res;

	if (mask & EUF_FOG_PARAMS)
	{
		if (((CNullGPUProgram*)gpu_prog)->getFog() != m_Fog
				// For D3D must reset uniforms all the times
				// TODO - reserch this issue
				|| getDriverFamily() == vid::EDF_DIRECTX
			)
		{
			f32 fog[3] = { m_Fog.Start, m_Fog.End, m_Fog.Density };
			res = gpu_prog->setUniformfv(EUT_FOG_PARAMS, fog, sizeof(fog)) && res;
			if (mask & EUF_FOG_COLOR)
			{
				img::SColorf col(m_Fog.Color);
				res = gpu_prog->setUniformfv(EUT_FOG_COLOR, (f32*)&col, 3 * sizeof(f32)) && res;
			}
		}
		((CNullGPUProgram*)gpu_prog)->setFog(m_Fog);
	}

	return res;
}

//---------------------------------------------------------------------------

IGPUProgram* CNullDriver::_getGPUProgramBound()
{
	 return m_CurrentGPUProgram;
}

//---------------------------------------------------------------------------

void CNullDriver::applyDynamicLights()
{
	u32 lsize = getDynamicLightsCount();
	u32 idx = 0, i = 0;
	m_EnabledLights.set_used(0);
	for (i = 0; i < lsize; i++)
	{
		if (!(m_LightsBits & (u64)(1 << i)))
			continue;
		m_EnabledLights.push_back(i);
	}
}

//---------------------------------------------------------------------------

void CNullDriver::_applyDynamicLights(
	const SLight *light0, u64 enabled_lights, s32 lidx)
{
	do
	{
		m_LightsBits = 0;

		u32 lsize = getDynamicLightsCount();
		if (lidx != -1)
		{
			for (u32 i = 0; i < lsize; i++)			
				m_Lights[i].Enabled = false;
			if (lidx < 0 || (u32)lidx >= lsize)
				break;
			m_Lights[lidx].Enabled = true;
			m_LightsBits |= (u64)(1 << lidx);
		}
		else
		{
			u32 esize = enabled_lights ? lsize : 0;
			for (u32 i = esize; i < lsize; i++)
				m_Lights[i].Enabled = false;
			for (u32 i = 0; i < esize && i < lsize; i++)
			{
				m_Lights[i].Enabled = (enabled_lights & (u64)(1 << i)) != 0;
				if (m_Lights[i].Enabled)
					m_LightsBits |= (u64)(1 << i);
			}
			if (light0)
				setDynamicLight(0, *light0);
			else
			{
				m_Lights[0].Enabled = false;
				m_LightsBits &= ~1;
			}
		}
	}
	while (0);

	applyDynamicLights();
}

//---------------------------------------------------------------------------

void CNullDriver::_setRenderStates()
{
	CNullGPUProgram *gpu_prog = NULL;

	if (m_UseShaders)
	{
		u32 lightcnt = m_EnabledLights.size();
		gpu_prog = (CNullGPUProgram*)getGPUProgram(
			m_CurrentVertexType, m_CurrentRenderPass, lightcnt);
	}

	if (m_ResetRenderStates || m_LastRenderPass != m_CurrentRenderPass)
	{
		if (m_UseShaders && gpu_prog)
			_setShaderRenderStates();
		else
			_setFFPRenderStates();
    }

	if (m_UseShaders)
	{
		if (!_bindGPUProgram(gpu_prog))
			LOGGER.logErr("Can't bind GPU program (uniforms=0x%08X, %d max lights) "
				"for render pass (vtype=%s, hash=0x%016I64X, %d lights)",
				gpu_prog ? gpu_prog->getUniformMask() : 0,
					gpu_prog ? gpu_prog->getLightsCount() : 0,
				vid::getVertexTypeName(m_CurrentVertexType),
				m_CurrentRenderPass.getHashGPU(), m_EnabledLights.size());
	}

	m_LastRenderPass = m_CurrentRenderPass;

	m_ResetRenderStates = false;
	m_DirtyTexUnit = -1;
}

//---------------------------------------------------------------------------

int CNullDriver::_clip2DLineRect(
	const core::rect<s32>* clip, core::position2d<s32>& p1, core::position2d<s32>& p2)
{
	enum { LEFT=1, RIGHT=2, BOTTOM=4, TOP=8};

	#define OUTCODE(x, y, rc, c) \
	c = 0; \
	if (y < rc->UpperLeftCorner.Y) c |= TOP; \
	else if (y > rc->LowerRightCorner.Y) c |= BOTTOM; \
	if (x < rc->UpperLeftCorner.X) c |= LEFT; \
	else if (x > rc->LowerRightCorner.X) c |= RIGHT;

	char outcode1=0;
	char outcode2=0;
	char outside;
	core::position2d<s32> tmp;

	// do trivial rejects and outcodes
	if (p1.Y > clip->LowerRightCorner.Y)
		outcode1 = BOTTOM;
	else if (p1.Y < clip->UpperLeftCorner.Y)
		outcode1 = TOP;
	if (p2.Y > clip->LowerRightCorner.Y)
		outcode2 = BOTTOM;
	else if (p2.Y < clip->UpperLeftCorner.Y)
		outcode2 = TOP;

	// trivially outside
	if (outcode1 & outcode2)
		return 0;

	if (p1.X < clip->UpperLeftCorner.X)
		outcode1 |= LEFT;
	else if (p1.X > clip->LowerRightCorner.X)
		outcode1 |= RIGHT;
	if (p2.X < clip->UpperLeftCorner.X)
		outcode2 |= LEFT;
	else if (p2.X > clip->LowerRightCorner.X)
		outcode2 |= RIGHT;

	// trivially outside
	if (outcode1 & outcode2)
		return 0;

	// trivially accept
	if (outcode1 == 0 && outcode2 == 0)
		return 1;

	while (outcode1 | outcode2)
	{
		// may be partially inside box, find an outside point

		if (outcode1)
			outside = outcode1;
		else
			outside = outcode2;

		// clip each side
		if (outside & TOP) // y=y0+t(y1-y0)
		{	
			tmp.Y = clip->UpperLeftCorner.Y;         
			tmp.X = p1.X + s32( f32(p2.X-p1.X) * f32(tmp.Y-p1.Y) / f32(p2.Y-p1.Y));

		} 
		else if (outside & BOTTOM)
		{
			tmp.Y = clip->LowerRightCorner.Y;
			tmp.X = p1.X + s32( f32(p2.X-p1.X) * f32(tmp.Y-p1.Y) / f32(p2.Y-p1.Y));
		} 
		else if (outside & RIGHT)
		{
			tmp.X = clip->LowerRightCorner.X;
			tmp.Y = p1.Y + s32( f32(p2.Y-p1.Y) * f32(tmp.X-p1.X) / f32(p2.X-p1.X));
		} 
		else if (outside & LEFT)
		{
			tmp.X = clip->UpperLeftCorner.X;
			tmp.Y = p1.Y + s32( f32(p2.Y-p1.Y) * f32(tmp.X-p1.X) / f32(p2.X-p1.X));
		}

		if (outside == outcode1)
		{
			p1 = tmp;
			OUTCODE(p1.X, p1.Y, clip, outcode1)
		} 
		else
		{
			p2 = tmp;
			OUTCODE(p2.X, p2.Y, clip, outcode2)
		}

		if (outcode1 & outcode2)
			return false;

	}

	return 2;
}

//---------------------------------------------------------------------------

#ifdef __MY_BUILD_VID_NULL_LIB__

__MY_IMPORT__ IVideoDriver* createDirectX9Driver(
	SExposedVideoData &out_video_data, E_TEXTURE_FILTER textureFilter);
__MY_IMPORT__ IVideoDriver* createOpenGL11Driver(
	SExposedVideoData &out_video_data, E_TEXTURE_FILTER textureFilter);
__MY_IMPORT__ IVideoDriver* createOpenGL21Driver(
	SExposedVideoData &out_video_data, E_TEXTURE_FILTER textureFilter);

//! creates a video driver
__MY_EXPORT__ IVideoDriver* createVideoDriver(
	SExposedVideoData &out_video_data, E_TEXTURE_FILTER textureFilter)
{
	switch (out_video_data.DriverType)
	{
	case EDT_OPENGL11:
		return createOpenGL11Driver(out_video_data, textureFilter);
	case EDT_OPENGL21:
		return createOpenGL21Driver(out_video_data, textureFilter);
	case EDT_DIRECTX9:
		return createDirectX9Driver(out_video_data, textureFilter);
	default:
		break;
	}
	out_video_data.DriverType = EDT_NULL;
	CNullDriver* null = new CNullDriver(out_video_data.WindowSize);
	if (!null->_initDriver(out_video_data))
	{
		((IVideoDriver*)null)->drop();
		null = NULL;
	}
	return null;
}

#endif

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
