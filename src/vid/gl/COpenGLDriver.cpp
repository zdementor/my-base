//|-------------------------------------------------------------------------
//| File:        COpenGLDriver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLDriver.h" 

#include "COpenGLTexture.h"
#include "COpenGLGPUProgram.h"
#include "COpenGLRenderBuffer.h"
#include "COpenGLHardwareOcclusionQuery.h"
#include "COpenGLTexture.h"
#include "COpenGLRenderTargetTexture.h"

#include <scn/ISceneManager.h>
#include <io/ILogger.h>
#include <dev/IDevice.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

const f32 inv_color = 1.0f / 255.0f;

#if MY_PLATFORM == MY_PLATFORM_WIN32    

//---------------------------------------------------------------------------

//! Win32 constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize) 
	: CNullDriver(screenSize),
HWnd(0), HDc(0), m_RenderContext(0), m_ResourceContext(0),
screenshot(0), screenshot_counter(0), StencilFogTexture(0),
RenderTargetTexture(0), m_OpenGLHardwareOcclusionQuery(0)
{
#if MY_DEBUG_MODE 
	IVideoDriver::setClassName("COpenGLDriver::IVideoDriver");
	IRunnable::setClassName("COpenGLDriver::IRunnable");
#endif

	memset(m_EnabledTextureClientState, 0, sizeof(m_EnabledTextureClientState));

	m_DriverType = (E_DRIVER_TYPE)__MY_BUILD_GL_VER__;
}

//---------------------------------------------------------------------------

bool COpenGLDriver::_initDriver(SExposedVideoData &out_video_data)
{
	bool swrast = DEVICE.getDeviceFlagValue(dev::EDCF_SOFTWARE_RASTERIZER);

    static  PIXELFORMATDESCRIPTOR pfd =
    {   
		sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
        1,                          // Version Number
        PFD_DRAW_TO_WINDOW |        // Format Must Support Window
        PFD_SUPPORT_OPENGL |        // Format Must Support OpenGL
		(swrast ? PFD_GENERIC_FORMAT : 0) |
        PFD_DOUBLEBUFFER,           // Must Support Double Buffering
        PFD_TYPE_RGBA,              // Request An RGBA Format
        24,                         // Select Our Color Depth
        0, 0, 0, 0, 0, 0,           // Color Bits Ignored
        8,                          // alpha bits in each RGBA color buffer
        0,                          // Shift Bit Ignored
        0,                          // No Accumulation Buffer
        0, 0, 0, 0,                 // Accumulation Bits Ignored
        24,                         // Depth-buffers bits
        8,                          // stencil buffer bits
        0,                          // No Auxiliary Buffer
        PFD_MAIN_PLANE,             // Main Drawing Layer
        0,                          // Reserved
        0, 0, 0                     // Layer Masks Ignored
    };

	if (out_video_data.FullScreen)
		DEVICE.switchMonitorToFullScreen();

	HWnd = reinterpret_cast<HWND>(out_video_data.Win32.HWnd);

    GLuint PixelFormat;

    for (int i = 0; i < 3; ++i)
    {
        if (i == 1)
        {
            LOGGER.logWarn(" Cannot create a GL device with stencil buffer, disabling stencil shadows");
            m_StencilBuffer = false;
            pfd.cStencilBits = 0;
        }
        else
        if (i == 2)
        {
            LOGGER.logErr(" Cannot create a GL device context");
            return false;
        }

        // get hdc
        if (!(HDc=GetDC(HWnd)))
        {
            LOGGER.logErr(" Cannot create a GL device context");
            continue;
        }        

        // choose pixelformat
        if (!(PixelFormat = ChoosePixelFormat(HDc, &pfd)))
        {
            LOGGER.logErr(" Cannot find a suitable pixel format");
            continue;
        }

		// describe pixelformat

		PIXELFORMATDESCRIPTOR dpfd;
		if (!DescribePixelFormat(HDc, PixelFormat, sizeof(dpfd), &dpfd))
		{
            LOGGER.logErr(" Cannot describe pixel format");
            continue;
        }

		if (swrast && !(dpfd.dwFlags & PFD_GENERIC_FORMAT))		
		{
			s32 nfmt = DescribePixelFormat(HDc, 0, 0, NULL);
			for (s32 j = 1; j <= nfmt; j++)
			{
				PIXELFORMATDESCRIPTOR tmppfd;
				if (DescribePixelFormat(HDc, j, sizeof(tmppfd), &tmppfd) &&
						(tmppfd.dwFlags & PFD_GENERIC_FORMAT) &&
						(tmppfd.dwFlags & PFD_DOUBLEBUFFER) &&
						tmppfd.cColorBits >= 16 &&
						tmppfd.cDepthBits >= 16 &&
						tmppfd.iPixelType == PFD_TYPE_RGBA
						)
				{
					PixelFormat = j;
					dpfd = tmppfd;
					break;
				}
			}
			if (!(dpfd.dwFlags & PFD_GENERIC_FORMAT))
			{
				LOGGER.logErr(" Cannot find GDI generic pixel format");
			}
		}

        // set pixel format
        if (!SetPixelFormat(HDc, PixelFormat, &dpfd))
        {
            LOGGER.logErr(" Cannot set the pixel format");
            continue;
        }

        // create rendering context
        if (!(m_RenderContext = wglCreateContext(HDc)))
        {
            LOGGER.logErr(" Cannot create a GL rendering context");
            continue;
        }

		// create resources context
        if (!(m_ResourceContext = wglCreateContext(HDc)))
        {
            LOGGER.logErr(" Cannot create a GL shared rendering context");
            continue;
        }

		if (!wglShareLists(m_RenderContext, m_ResourceContext))
		{
			LOGGER.logErr(" Cannot share GL contexts");
            continue;
		}

        if (!setRenderContextCurrent() || !setResourceContextCurrent())
        {
            LOGGER.logErr(" Cannot activate GL context");
            continue;
        }
		else
		{
			LOGGER.logInfo(" OGL context activated");            
		}

        break;
    }

    // print renderer information
    core::stringc glRend = glGetString(GL_RENDERER);
    core::stringc glVend = glGetString(GL_VENDOR);
	core::stringc glVer = glGetString(GL_VERSION);
	core::stringc glExt = glGetString(GL_EXTENSIONS);

	LOGGER.logInfo(" Video adapter model: %s %s", glRend.c_str(), glVend.c_str());
	LOGGER.logInfo(" OGL driver version: %s", glVer.c_str());
	LOGGER.logInfo(" Supported OpenGL extentions:\n  %s", glExt.c_str());

	m_StencilBuffer &= m_Shadows;

	// load extensions

	glewInit();

#if __MY_BUILD_GL_VER__ == MY_DRIVER_TYPE_OPENGL11
    if (!GLEW_VERSION_1_1)
	{
        LOGGER.logErr(" OpenGL driver version is not 1.1 or better.");
		return false;
	}
	if (!GLEW_EXT_bgra)
	{
        LOGGER.logWarn(" OpenGL driver is not supported GL_EXT_bgra extension.");
	}
#elif  __MY_BUILD_GL_VER__ == MY_DRIVER_TYPE_OPENGL12

    if (!GLEW_VERSION_1_1 || !GLEW_VERSION_1_2)
	{
        LOGGER.logErr(" OpenGL driver version is not 1.2 or better.");
		return false;
	}
	if (!GLEW_ARB_multitexture)
	{
        LOGGER.logErr(" OpenGL driver is not supported GL_ARB_multitexture extension.");
		return false;
	}
	if (!GLEW_ARB_texture_border_clamp)
	{
        LOGGER.logErr(" OpenGL driver is not supported GL_ARB_texture_border_clamp extension.");
		return false;
	}
	if (!GLEW_ARB_texture_env_combine)
	{
        LOGGER.logErr(" OpenGL driver is not supported GL_ARB_texture_env_combine extension.");
		return false;
	}
	if (!GLEW_ARB_texture_mirrored_repeat)
	{
        LOGGER.logErr(" OpenGL driver is not supported GL_ARB_texture_mirrored_repeat extension.");
		return false;
	}
#elif  __MY_BUILD_GL_VER__ == MY_DRIVER_TYPE_OPENGL21
    if (!GLEW_VERSION_1_4 || !GLEW_VERSION_1_3 || !GLEW_VERSION_1_2 || !GLEW_VERSION_1_2)
	{
        LOGGER.logErr(" OpenGL driver version is not 1.4 or better.");
		return false;
	}
	if (!GLEW_ARB_imaging)
	{
        LOGGER.logErr(" OpenGL driver is not supported GL_ARB_imaging extension.");
		return false;
	}
	if (!GLEW_EXT_texture_compression_s3tc)
	{
		LOGGER.logWarn(" OpenGL driver is not supported EXT_texture_compression_s3tc extension,"
			"be aware from using DXT textures.");
	}
#endif

	bool vboSupport = false;
#if defined GL_ARB_vertex_buffer_object
	vboSupport = !!GLEW_ARB_vertex_buffer_object;
#endif

	GLint temp;
#ifdef GL_VERSION_1_2
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &temp);
	m_MaxTextureUnits = temp;
#else
	m_MaxTextureUnits = 1;
#endif	

    if (m_MaxTextureUnits < 2)
		LOGGER.logWarn(" OpenGL device only has one texture unit. Disabling multitexturing.");

#ifdef GL_EXT_texture_filter_anisotropic
	if (GLEW_EXT_texture_filter_anisotropic)
	{
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &temp);
		MaxAnisotropyLevel = temp;
	}
	else
#endif
		MaxAnisotropyLevel = 0;

#ifdef GL_EXT_stencil_two_side
	if (GLEW_EXT_stencil_two_side) // for nVidia
		m_TwoSidedStencil = true;
	else
#endif
#ifdef GL_ATI_separate_stencil
	if (GLEW_ATI_separate_stencil) // for ATI
		m_TwoSidedStencil = true;
	else
#endif
		m_TwoSidedStencil = false;

	LOGGER.logInfo(" OpenGL driver features:");
	LOGGER.logInfo("  Multitexturing   : %s",
		queryFeature(EVDF_MULITEXTURE) ? "OK" : "None");
	LOGGER.logInfo("  Vertex buffer    : %s", vboSupport ?
		"OK" : "None (using vertex arrays instead)");
	LOGGER.logInfo("  Anisotropic filt.: %s (%d level)",
		queryFeature(EVDF_ANISOTROPIC_FILTER) ?
			"OK" : "None", MaxAnisotropyLevel);
	LOGGER.logInfo("  Swap control     : %s",
		WGLEW_EXT_swap_control ? "OK" : "None"); 
	LOGGER.logInfo("  GLSL             : %s",
		queryFeature(EVDF_SHADER_LANGUAGE) ? "OK" : "None");
#ifdef GL_ARB_shading_language_100
	if (!!GLEW_ARB_shading_language_100)
		LOGGER.logInfo("   Vertex/Pixel shader v.%s",
			glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));
#endif
	LOGGER.logInfo("  Two side stencil : %s",
		m_TwoSidedStencil ? "OK" : "None");
	LOGGER.logInfo("  Occlusion Query  : %s",
		queryFeature(EVDF_OCCLUSION_QUERY) ? "OK" : "None");
	LOGGER.logInfo("  Render Target    : %s",
		queryFeature(EVDF_RENDER_TO_TARGET) ? "OK" : "None");
	LOGGER.logInfo("  Compressed tex.  : %s",
		queryFeature(EVDF_COMPRESSED_TEXTURES) ? "OK" : "None");

	// OpenGL driver constants

	glGetIntegerv(GL_MAX_LIGHTS, &temp); 
	CHECK_MIN_RANGE(temp, 8);
	m_MaxLights = temp;	

	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &temp );
	m_MaxTextureSize = core::dimension2di(temp, temp);

	s32 max_tex_units = m_MaxTextureUnits;
	CHECK_MAX_RANGE(m_MaxTextureUnits, MY_MATERIAL_MAX_LAYERS);

	for (u32 i = 0; i < 2; i++)
	{
		if (i == 0)
			setRenderContextCurrent();
		else
			setResourceContextCurrent();

		// Set The Current Viewport
		glViewport(0, 0, m_ScreenSize.Width, m_ScreenSize.Height); 

		// setup ogl initial render states    
		glShadeModel(GL_SMOOTH);
	    
		setGlobalAmbientColor(getGlobalAmbientColor());

		glClearDepth(1.0f);
		glClearStencil(MY_STENCIL_ZERO_VALUE);    

		glFrontFace( GL_CCW );		

		// set fog mode
		setFog(Fog);

		glDisable(GL_COLOR_MATERIAL);
		
		// for specular highlights
#ifdef GL_VERSION_1_2
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR); 
#endif
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	}
	
    // set exposed data
    out_video_data.Win32.OGL.HDc  = reinterpret_cast<void*>(HDc);
    out_video_data.Win32.OGL.HRc  = reinterpret_cast<void*>(m_RenderContext);	
	out_video_data.DriverType = EDT_OPENGL21;
  
    // Create Our Empty Fog Textures
    StencilFogTexture = createEmptyOGLTexture(
        getStencilFogTextureSize(), getStencilFogTextureSize());  
 
    // log Video driver parameters
    DescribePixelFormat(HDc, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (pfd.cStencilBits == 0)
	{
		LOGGER.logWarn(" Device does not support stencilbuffer, disabling stencil buffer.");
		m_StencilBuffer = false;
	}
	else 
		m_StencilBuffer = true;

    LOGGER.logInfo(" Video driver parameters: ");
    LOGGER.logInfo("  color   : %d bit", pfd.cColorBits);
    LOGGER.logInfo("  alpha   : %d bit", pfd.cAlphaBits);
    LOGGER.logInfo("  depth(z): %d bit", pfd.cDepthBits);
    LOGGER.logInfo("  stencil : %d bit", pfd.cStencilBits);
    LOGGER.logInfo("  max lights    : %d", m_MaxLights);
    LOGGER.logInfo("  max tex. units: %d", max_tex_units);
    LOGGER.logInfo("  max tex. res. : %d x %d", m_MaxTextureSize.Width, m_MaxTextureSize.Height);	

	// vertical syncronization
	if (WGLEW_EXT_swap_control && wglSwapIntervalEXT(m_VerticalSync?1:0)!=0)
		LOGGER.logInfo(" Vertical Sync %s", m_VerticalSync ? "enabled" : "disabled");
	else
		LOGGER.logInfo(" Can't on/off Vertical Sync ");

	// setting texture filter
	setTextureFilter(m_TextureFilter);

	CNullDriver::setColorMask(m_ColorMask);

    return CNullDriver::_initDriver(out_video_data);
}

//---------------------------------------------------------------------------

//! win32 destructor
COpenGLDriver::~COpenGLDriver()
{
	free();
	SAFE_DROP(m_OpenGLHardwareOcclusionQuery);
    glDeleteTextures(1,&StencilFogTexture);

	setNullContextCurrent();

    if (m_RenderContext)
    {
        if (!wglDeleteContext(m_RenderContext))
            LOGGER.logWarn("Release of rendering context failed.");
        m_RenderContext = 0;
    }
	if (m_ResourceContext)
	{
		if (!wglDeleteContext(m_ResourceContext))
            LOGGER.logWarn("Release of shared rendering context failed.");
        m_ResourceContext = 0;
	}
    if (HDc)
	{
        ReleaseDC(HWnd, HDc);
		HDc = 0;
	}	
	LOGGER.logInfo("OpenGL graphic deactivated");
}

// -----------------------------------------------------------------------

#endif // #ifdef WIN32

//---------------------------------------------------------------------------

// added by zola
void COpenGLDriver::makeScreenShot(ITexture* texture)
{
    if (texture==NULL) return;

    core::dimension2d<s32> TextureSize=texture->getSize();

    if (TextureSize.Width<m_ScreenSize.Width ||
		TextureSize.Height<m_ScreenSize.Height ||
		texture->getColorFormat()!=img::ECF_A8R8G8B8) return;

    if(screenshot==NULL)
        screenshot=new u32[m_ScreenSize.Width*(m_ScreenSize.Height+1)];

    glReadBuffer(GL_BACK);
    glReadPixels(0,0,
		m_ScreenSize.Width,m_ScreenSize.Height, GL_BGRA, GL_UNSIGNED_BYTE, screenshot);

    // screen shot is upside down because glReadPixels 0,0 denotes the lower-left corner ot the image!
    // the following code make the procedure slow!!

    u32* texture_buffer = (u32*)texture->lock();
    u32  texture_row    = texture->getPitch()/sizeof(u32);

    u32* screen_buffer  = screenshot+(m_ScreenSize.Height)*m_ScreenSize.Width;
    u32  screen_row     = m_ScreenSize.Width;
    u32  screen_pitch   = sizeof(u32)*m_ScreenSize.Width; 

    for(int i=0; i<m_ScreenSize.Height; i++)
    {
        memcpy(texture_buffer,screen_buffer,screen_pitch);
        texture_buffer  += texture_row;
        screen_buffer   -= screen_row;
    }

    texture->unlock();
}

//---------------------------------------------------------------------------

ITexture* COpenGLDriver::makeScreenShot()
{
    c8 name[64];
    screenshot_counter++;
    sprintf(name,"screen_%d",screenshot_counter);
    ITexture* texture=addTexture(m_ScreenSize,name);
    makeScreenShot(texture);
    return texture;
}

//----------------------------------------------------------------------------

void COpenGLDriver::_renderStencilVolume(IRenderBuffer * rbuf, bool zfail)
{
	if (!m_StencilBuffer)  
        return;

	CNullDriver::_renderStencilVolume(rbuf, zfail);

	_setVertexType(rbuf->getVertices()->getType());
	_setRenderStates();

	glPushAttrib(GL_STENCIL_BUFFER_BIT);

	if (zfail)
	// ZFail method
	{
		if (m_TwoSidedStencil)
		// With 2-sided stencil, we can avoid rendering twice:
		{
			glDisable(GL_CULL_FACE);

			// setup two sided stencil
#ifdef GL_EXT_stencil_two_side
			if (GLEW_EXT_stencil_two_side) // for nVidia
			{
				glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
				glActiveStencilFaceEXT(GL_BACK);
				glStencilOp(GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
				glActiveStencilFaceEXT(GL_FRONT);
				glStencilOp(GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);
			}
#endif
#ifdef GL_ATI_separate_stencil
			if (GLEW_ATI_separate_stencil) // for ATI
			{
				glStencilOpSeparateATI(GL_BACK,  GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP); 
				glStencilOpSeparateATI(GL_FRONT, GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP); 			
				glStencilFuncSeparateATI(GL_ALWAYS, GL_ALWAYS, 1, 0xFFFFFFFFL);
			}
#endif
			((CNullRenderBuffer*)rbuf)->draw();
		}
		else
		{
			glEnable(GL_CULL_FACE);

			// First Pass:
			// If ztest fail increment stencil buffer value
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_INCR,	// depth test fail
				GL_KEEP);	// depth test pass
			// Draw back-side of shadow volume in stencil only
			glFrontFace( GL_CW );
			((CNullRenderBuffer*)rbuf)->draw();

			// Second Pass:
			// If ztest fail decrement stencil buffer value
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_DECR,	// depth test fail
				GL_KEEP);	// depth test pass
			/// Draw front-side of shadow volume in stencil only
			glFrontFace( GL_CCW );
			((CNullRenderBuffer*)rbuf)->draw();
		}
	}
	else
	// ZPass method
	{
		if (m_TwoSidedStencil)
		// With 2-sided stencil, we can avoid rendering twice:
		{
			glDisable(GL_CULL_FACE);

			// setup on two sided stencil
#ifdef GL_EXT_stencil_two_side
			if (GLEW_EXT_stencil_two_side) // for nVidia
			{
				glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
				glActiveStencilFaceEXT(GL_FRONT);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP_EXT);
				glActiveStencilFaceEXT(GL_BACK);
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT);
			}
#endif
#ifdef GL_ATI_separate_stencil
			if (GLEW_ATI_separate_stencil) // for ATI
			{
				glStencilOpSeparateATI(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP_EXT); 
				glStencilOpSeparateATI(GL_BACK,  GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT); 
				glStencilFuncSeparateATI(GL_ALWAYS, GL_ALWAYS, 1, 0xFFFFFFFFL);
			}
#endif
			((CNullRenderBuffer*)rbuf)->draw();
		}
		else
		{
			glEnable(GL_CULL_FACE);

			// First Pass:
			// If ztest passes increment stencil buffer value		
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_KEEP,	// depth test fail
				GL_INCR);	// depth test pass
			// Draw front-side of shadow volume in stencil only
			glFrontFace( GL_CCW );
			((CNullRenderBuffer*)rbuf)->draw();

			// Second Pass:
			// Decrement stencil buffer value
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_KEEP,	// depth test fail
				GL_DECR);	// depth test pass
			// Draw back-side of shadow volume in stencil only
			glFrontFace(GL_CW);
			((CNullRenderBuffer*)rbuf)->draw();
		}
	}

	// restore initial states
	glPopAttrib();
	if (m_CurrentRenderPass.getFlag(EMF_BACK_FACE_CULLING))
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW))
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW );
}

//---------------------------------------------------------------------------

void COpenGLDriver::applyDynamicLights()
{
	CNullDriver::applyDynamicLights();

	if (!m_UseFFP)
		return;

	static u32 maxidx = getDynamicLightsMaximalAmount();
	u32 idx = 0;
	u32 enabledcnt = m_EnabledLights.size();
	for (idx = 0; idx < enabledcnt && idx < maxidx; idx++)
	{
		s32 lidx = GL_LIGHT0 + idx;
		const SLight &light = m_Lights[m_EnabledLights[idx]];
		
		static GLfloat data[4];

		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;

		if (light.Type == ELT_DIRECTIONAL)
			data[3] = 0.0f;
		else
			data[3] = 1.0f;

		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);   

		img::SColorf diffuse  = light.DiffuseColor;
		img::SColorf specular = light.SpecularColor;
		img::SColorf ambient  = light.AmbientColor;

		f32 radius = light.Radius;	

		// set diffuse color
		data[0] = diffuse.r;
		data[1] = diffuse.g;
		data[2] = diffuse.b;
		data[3] = diffuse.a;
		glLightfv(lidx, GL_DIFFUSE, data);

		// set specular color
		data[0] = specular.r;
		data[1] = specular.g;
		data[2] = specular.b;
		data[3] = specular.a;
		glLightfv(lidx, GL_SPECULAR, data);

		// set ambient color
		data[0] = ambient.r;
		data[1] = ambient.g;
		data[2] = ambient.b;
		data[3] = ambient.a;
		glLightfv(lidx, GL_AMBIENT, data);

		// 1.0f / (constant + linar * d + quadratic*(d*d);

		// set attenuation
		glLightf(lidx, GL_CONSTANT_ATTENUATION,  0.0f);
		glLightf(lidx, GL_LINEAR_ATTENUATION,    0.0f);
		glLightf(lidx, GL_QUADRATIC_ATTENUATION, 10.0f/(radius*radius));

		glEnable(lidx);
	}
	if (idx != m_LastEnabledLightsCount)
	{
		for (; idx < enabledcnt && idx < maxidx; idx++)
			glDisable(GL_LIGHT0 + idx);
		m_LastEnabledLightsCount = idx - 1;
	}
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_bindGPUProgram(IGPUProgram* gpu_prog)
{
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	if (m_UseShaders)
	{
		union UHandle
		{
			GLhandleARB u;
			void		*v;
		}
		handle;
		handle.v = gpu_prog ? ((COpenGLGPUProgram*)gpu_prog)->_getProgramHandle() : 0;
		glUseProgramObjectARB(handle.u);
	}
#endif
	return CNullDriver::_bindGPUProgram(gpu_prog);
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_beginRendering()
{
	if (!StencilPreInitialized && m_StencilBuffer)
	{
		// to pre-initialize stencil
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_STENCIL_TEST);
		StencilPreInitialized = true;
	}  
	setPolygonFillMode(m_PolygonFillMode);

	return true;
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_endRendering()
{
	glFlush();
	return true;
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_swapBuffers()
{
	bool res = true;
	
#if MY_PLATFORM == MY_PLATFORM_WIN32
	res = SwapBuffers(HDc) != 0;
#endif
	return res;
}

//---------------------------------------------------------------------------

//! queries the features of the driver, returns true if feature is available
bool COpenGLDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
    switch (feature)
    {
	case EVDF_MULITEXTURE:
		return getMaximalTextureUnitsAmount()>1;
	case EVDF_BILINEAR_FILTER:
        return true;
    case EVDF_TRILINEAR_FILTER:
        return true;
    case EVDF_ANISOTROPIC_FILTER:
        return MaxAnisotropyLevel > 0;
    case EVDF_RENDER_TO_TARGET:
        return true;
    case EVDF_MIP_MAP:
        return true;
    case EVDF_STENCIL_BUFFER:
        return m_StencilBuffer;
	case EVDF_SHADER_LANGUAGE:
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
		return GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader &&
			GLEW_ARB_shader_objects && GLEW_ARB_shading_language_100;
#else
		return false;
#endif
	case EVDF_OCCLUSION_QUERY:
#if defined(GL_ARB_occlusion_query)
		return !!GLEW_ARB_occlusion_query;
#else
		return false;
#endif
	case EVDF_COMPRESSED_TEXTURES:
#if defined(GL_EXT_texture_compression_s3tc)
		return GLEW_EXT_texture_compression_s3tc != NULL;
#else
		return false;
#endif
    };

    return false;
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setTexture(s32 stage, ITexture* texture)
{
	static u32 max_tex_units = getMaximalTextureUnitsAmount();
    if (stage < 0 || stage > (s32)max_tex_units)
		return;

	CNullDriver::_setTexture(stage, texture);

#ifdef GL_VERSION_1_2
	glActiveTexture(GL_TEXTURE0 + stage );
#endif

	if (CurrentTexture[stage] == texture && 
		!(texture && texture->getType() == vid::ETT_ANIMATED))
        return;  

	union {
		GLuint	u;
		void*	v;
	} texId;

	texId.v = texture ? texture->getHardwareTexture() : NULL;

	SAFE_DROP(CurrentTexture[stage]);

    CurrentTexture[stage] = texture;  

    if (texture == NULL)
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
    else
    {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId.u);       
		texture->grab();
	}

	if (!m_ResetRenderStates && m_CurrentRenderPass.Layers[stage].getTexture() != texture)
		m_ResetRenderStates = true;
}

//---------------------------------------------------------------------------

ITexture* COpenGLDriver::_createDeviceDependentTexture(img::IImage* surface)
{
    return new COpenGLTexture(surface, TextureCreationFlags);
}

//---------------------------------------------------------------------------

vid::ITexture* COpenGLDriver::_createDeviceDependentTexture(core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	 return new COpenGLTexture(size, format, TextureCreationFlags);
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setTextureCoords(const void* tc, const void* tc2,
	const void* tangents, const void* binormals,
	s32 tc_stride)
{
	const SRenderPass &m = getRenderPass();
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	s32 active_state = 0;

	bool unitsUsage[MY_MATERIAL_MAX_LAYERS] = {false, false, false, false};

	for (u32 u = 0; u < max_units_cnt; u++)
	{
		if (m.Layers[u].getTexture())
		{
			s32 tchnl = m.Layers[u].getTexCoordChannel();
			if (tchnl < MY_MATERIAL_MAX_LAYERS)
				unitsUsage[tchnl] = true;
		}
	}

	for (s32 u = (max_units_cnt - 1); u >= 0; u--)
	{
		const void *tex_coord_ptr = 
			(u == 1) ? tc2 : (u == 0 ? tc : NULL);
		u32 tc_components = 2;
		if (u == 3 && binormals)
		{
			tex_coord_ptr = binormals;
			tc_components = 3;
		}
		else if (u == 2 && tangents)
		{
			tex_coord_ptr = tangents;
			tc_components = 3;
		}
		else if (!unitsUsage[u])
			tex_coord_ptr = NULL;
		if (tex_coord_ptr)
		{
#ifdef GL_VERSION_1_2
			glClientActiveTexture(GL_TEXTURE0 + u);
#endif
			active_state = u;
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(tc_components, GL_FLOAT, tc_stride, tex_coord_ptr);
			m_EnabledTextureClientState[u] = true;
		}
	}
	if (active_state != 0)
	{
#ifdef GL_VERSION_1_2
		glClientActiveTexture(GL_TEXTURE0);
#endif
	}
}

//---------------------------------------------------------------------------

void COpenGLDriver::_unsetTextureCoords()
{
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	s32 active_state = 0;

	for (s32 u = (max_units_cnt - 1); u >= 0; u--)
	{
		if (m_EnabledTextureClientState[u])
		{
#ifdef GL_VERSION_1_2
			glClientActiveTexture(GL_TEXTURE0 + u);
#endif
			active_state = u;
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			m_EnabledTextureClientState[u] = false;
		}
	}
	if (active_state != 0)
	{
#ifdef GL_VERSION_1_2
		glClientActiveTexture(GL_TEXTURE0);
#endif
	}
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setBasicRenderStates()
{  
	u16 flags = m_CurrentRenderPass.getFlags();

    if (m_ResetRenderStates || m_LastRenderPass.getFlags() != flags)
    {
		// zwrite
		if (m_CurrentRenderPass.getFlag(EMF_ZWRITE_ENABLE))
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);

		// back face culling
		if (m_CurrentRenderPass.getFlag(EMF_BACK_FACE_CULLING))
		{
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
		}
		else
			glDisable(GL_CULL_FACE);

		// face culling order
		if (m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW))
			glFrontFace(GL_CW);
		else
			glFrontFace(GL_CCW );

		// blending
		if (m_CurrentRenderPass.getFlag(EMF_BLENDING))
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		// shademode
        if  (m_CurrentRenderPass.getFlag(EMF_GOURAUD_SHADING))
            glShadeModel(GL_SMOOTH);
        else
            glShadeModel(GL_FLAT);
	}

	// blending functions

	E_SRC_BLEND_FUNCTION srcBlend = m_CurrentRenderPass.getSrcBlendFunc();
	E_DST_BLEND_FUNCTION dstBlend = m_CurrentRenderPass.getDstBlendFunc();

	if (m_ResetRenderStates ||
		srcBlend != m_LastRenderPass.getSrcBlendFunc() ||
		dstBlend != m_LastRenderPass.getDstBlendFunc())
	{	
		glBlendFunc(
			convertToOGLSrcBlendFunction(srcBlend),
			convertToOGLDstBlendFunction(dstBlend));
	}

	E_BLEND_EQUATION blendEq = m_CurrentRenderPass.getBlendEquation();

	if (m_ResetRenderStates ||
		blendEq != m_LastRenderPass.getBlendEquation())
	{	
#ifdef GL_VERSION_1_4
		glBlendEquation(
			convertToOGLBlendEquation(blendEq));
#endif
	}

	// Textures filtering

	bool mipsf = !m_CurrentRenderPass.getFlag(EMF_MIP_MAP_OFF);

	for (s32 l = m_DirtyTexUnit; l >= 0; --l)
	{
		const STextureLayer &layer = m_CurrentRenderPass.Layers[l];
		ITexture * tex = layer.getTexture();
		if (!tex)
			continue;
#ifdef GL_VERSION_1_2
		glActiveTexture(GL_TEXTURE0 + l);
#endif
		bool mips = tex->hasMipMaps() && mipsf;

		E_TEXTURE_FILTER tf = ETF_NONE;
		if (layer.isFilterEnabled())
		{
			tf = m_TextureFilter;
			if (tf!=ETF_NONE && m_CurrentRenderPass.Layers[l].getType() == ETLT_LIGHT_MAP)
				tf = ETF_BILINEAR;
		}

		if (tf == ETF_ANISOTROPIC)
		// Anisotropic filter
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				mips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
#ifdef GL_EXT_texture_filter_anisotropic
			if (GLEW_EXT_texture_filter_anisotropic)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
					core::math::Min(8, MaxAnisotropyLevel));
#endif
		}
		else 
		if (tf == ETF_TRILINEAR)
		// Trilinear filter
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				mips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
		}
		else 
		if (tf == ETF_BILINEAR)
		// Bilinear filter
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				mips ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
		}
		else
		// No filter
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_NEAREST);
		}
	}

	// alpha test

	E_ALPHA_TEST atest = m_CurrentRenderPass.getAlphaTest ();

	if (m_ResetRenderStates || m_LastRenderPass.getAlphaTest () != atest)
	{
		if (atest == vid::EAT_ALWAYS)
			glDisable(GL_ALPHA_TEST); 	
		else
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(
				convertToOGLCompareFunction(AlphaTestSetups[atest].func),
				AlphaTestSetups[atest].ref);
		}
	}

	// depth buffer compare function

	E_COMPARE_TEST ztest = m_CurrentRenderPass.getDepthTest();

	if (m_ResetRenderStates || m_LastRenderPass.getDepthTest() != ztest)
	{
		if (ztest == ECT_ALWAYS)
			glDisable(GL_DEPTH_TEST);
		else
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(convertToOGLCompareFunction(ztest));
		}
	}
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setFFPRenderStates()
{
	const SRenderPass &m		= getRenderPass();
	const SRenderPass &lastm	= getLastRenderPass();
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	bool reset = m_ResetRenderStates;

	for (u32 u = 0; u < max_units_cnt; u++)
	{
		if (!reset && lastm.Layers[u] == m.Layers[u])
			continue;

		vid::ITexture *texture = m.Layers[u].getType() != vid::ETLT_NORMAL_MAP ?
			m.Layers[u].getTexture() : NULL;

		_setTexture(u, texture);

		if (texture)
		{
			// setting texture matrix
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf(m.Layers[u].getTexMatrixCurrent().toOGLTextureMatrix());

			// setting texture addressing mode

			GLint addr_mode = GL_REPEAT;
#ifdef GL_VERSION_1_2
			switch ( m.Layers[u].getTexCoordAddr() )			
			{		
			case vid::ETCAM_MIRROR:
				addr_mode = GL_MIRRORED_REPEAT;
				break;				
			case vid::ETCAM_CLAMP:
				addr_mode = GL_CLAMP_TO_EDGE;
				break;
			case vid::ETCAM_BORDER:
				addr_mode = GL_CLAMP_TO_BORDER;
				break;
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, addr_mode);
#endif
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, addr_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, addr_mode);			

			// setting texture mapping settings

			switch ( m.Layers[u].getTexCoordGen() )
			{
			case vid::ETCGT_ENVIRONMENT_MAPPING:
				{
					// turn on environment mapping mode
					glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
					glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

					glEnable( GL_TEXTURE_GEN_S );
					glEnable( GL_TEXTURE_GEN_T );
					glDisable( GL_TEXTURE_GEN_R );
					glDisable( GL_TEXTURE_GEN_Q );
				}
				break;
			case vid::ETCGT_PROJECTED_MAPPING:
				{
					static GLfloat eyePlaneS[] = {1.0, 0.0, 0.0, 0.0};
					static GLfloat eyePlaneT[] = {0.0, 1.0, 0.0, 0.0};
					static GLfloat eyePlaneR[] = {0.0, 0.0, 1.0, 0.0};
					static GLfloat eyePlaneQ[] = {0.0, 0.0, 0.0, 1.0};

					glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
					glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
					glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
					glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
					
					glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
					glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
					glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
					glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);

					glEnable(GL_TEXTURE_GEN_S);
					glEnable(GL_TEXTURE_GEN_T);
					glEnable(GL_TEXTURE_GEN_R);
					glEnable(GL_TEXTURE_GEN_Q);
				}
				break;
			default:
				{
					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);  
					glDisable(GL_TEXTURE_GEN_R);  
					glDisable(GL_TEXTURE_GEN_Q);
				}
			}
			
			// setting layers blending states
#ifdef GL_VERSION_1_2
			switch ( m.Layers[u].getTexRGBScale() )
			{
			case 3:
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
				break;
			case 2:
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
				break;
			default:
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);	
			}
#endif
			if (u==0)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
#ifdef GL_VERSION_1_2
			else 
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,		GL_MODULATE); 

				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,  GL_PREVIOUS); 
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR); 
				
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,  GL_TEXTURE); 					
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			}
#endif
		}
	}

	bool lighting = m_CurrentRenderPass.isLighting();
	bool lighting_last = m_LastRenderPass.isLighting();

	if (m_ResetRenderStates ||
		m_CurrentRenderPass.getAmbientColor() != m_LastRenderPass.getAmbientColor() ||
		m_CurrentRenderPass.getDiffuseColorCurrent() != m_LastRenderPass.getDiffuseColorCurrent() ||
		m_CurrentRenderPass.getEmissiveColorCurrent() != m_LastRenderPass.getEmissiveColorCurrent() ||
		m_CurrentRenderPass.getSpecularColor() != m_LastRenderPass.getSpecularColor() ||
		m_CurrentRenderPass.getShininess() != m_LastRenderPass.getShininess() ||
		(lighting && lighting != lighting_last))
    {
		GLfloat color[4];
		color[0] = m_CurrentRenderPass.getAmbientColor().getRed()	* inv_color;
		color[1] = m_CurrentRenderPass.getAmbientColor().getGreen()	* inv_color;
		color[2] = m_CurrentRenderPass.getAmbientColor().getBlue()	* inv_color;
		color[3] = m_CurrentRenderPass.getAmbientColor().getAlpha()	* inv_color;
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);

		color[0] = m_CurrentRenderPass.getDiffuseColorCurrent().getRed()	* inv_color;
		color[1] = m_CurrentRenderPass.getDiffuseColorCurrent().getGreen()	* inv_color;
		color[2] = m_CurrentRenderPass.getDiffuseColorCurrent().getBlue()	* inv_color;
		color[3] = m_CurrentRenderPass.getDiffuseColorCurrent().getAlpha()	* inv_color;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

		color[0] = m_CurrentRenderPass.getEmissiveColorCurrent().getRed()	* inv_color;
		color[1] = m_CurrentRenderPass.getEmissiveColorCurrent().getGreen()	* inv_color;
		color[2] = m_CurrentRenderPass.getEmissiveColorCurrent().getBlue()	* inv_color;
		color[3] = m_CurrentRenderPass.getEmissiveColorCurrent().getAlpha()	* inv_color;
		glMaterialfv(GL_FRONT, GL_EMISSION, color);

		color[0] = m_CurrentRenderPass.getSpecularColor().getRed()	* inv_color;
		color[1] = m_CurrentRenderPass.getSpecularColor().getGreen()* inv_color;
		color[2] = m_CurrentRenderPass.getSpecularColor().getBlue()	* inv_color;
		color[3] = m_CurrentRenderPass.getSpecularColor().getAlpha()* inv_color;
		glMaterialfv(GL_FRONT, GL_SPECULAR, color);

		glMaterialf(GL_FRONT, GL_SHININESS, m_CurrentRenderPass.getShininess());
    }

	if (!(VertexComponents[m_CurrentVertexType]&vid::EVC_COL))
	{
		if (m_CurrentRenderPass.getColorGen() != vid::ECGT_VERTEX ||
			m_CurrentRenderPass.getAlphaGen() != vid::EAGT_VERTEX)
		{
			glColor4ub(
				m_CurrentRenderPass.getEmissiveColorCurrent().getRed(),
				m_CurrentRenderPass.getEmissiveColorCurrent().getGreen(),
				m_CurrentRenderPass.getEmissiveColorCurrent().getBlue(),
				m_CurrentRenderPass.getDiffuseColorCurrent().getAlpha());
		}
		else
			glColor4ub(255, 255, 255, 255);
	}

	// lighting
	if (m_ResetRenderStates || lighting != lighting_last)
	{
		if (lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	// fog
	bool fog = m_CurrentRenderPass.getFlag(EMF_FOG_ENABLE);
	bool fog_last = m_LastRenderPass.getFlag(EMF_FOG_ENABLE);
	if (m_ResetRenderStates || fog != fog_last)
	{
		if (fog)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}

	_setBasicRenderStates();
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setShaderRenderStates()
{
	const SRenderPass & m		= getRenderPass();
	const SRenderPass & lastm	= getLastRenderPass();
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	bool reset = m_ResetRenderStates;

	for (u32 u = 0; u < max_units_cnt; u++)
	{
		if (!reset && lastm.Layers[u] == m.Layers[u])
			continue;
		vid::ITexture *texture = m.Layers[u].getTexture();
		_setTexture(u, texture);
		if (texture)
		{
			// setting texture addressing mode

			GLint addr_mode = GL_REPEAT;
#ifdef GL_VERSION_1_2
			switch ( m.Layers[u].getTexCoordAddr() )			
			{		
			case vid::ETCAM_MIRROR:
				addr_mode = GL_MIRRORED_REPEAT;
				break;				
			case vid::ETCAM_CLAMP:
				addr_mode = GL_CLAMP_TO_EDGE;
				break;
			case vid::ETCAM_BORDER:
				addr_mode = GL_CLAMP_TO_BORDER;
				break;
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, addr_mode);
#endif
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, addr_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, addr_mode);
		}
	}
	_setBasicRenderStates();
}

//---------------------------------------------------------------------------

void COpenGLDriver::deleteAllDynamicLights()
{
    for (s32 i=0; i<m_LastEnabledLightsCount; ++i)
	{
		s32 lidx = GL_LIGHT0 + i;    

        glDisable(lidx);

		static GLfloat data[4] = {0,0,0,0};

		glLightfv(lidx, GL_SPECULAR, data);
	}

    CNullDriver::deleteAllDynamicLights();
}

//---------------------------------------------------------------------------

//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void COpenGLDriver::setGlobalAmbientColor(const img::SColorf& color)
{
	CNullDriver::setGlobalAmbientColor(color);

    GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}

//---------------------------------------------------------------------------

// this code was sent in by Oliver Klems, thank you! (I modified the glVieport
// method just a bit.
void COpenGLDriver::setViewPort(const core::rect<s32>& area)
{
    core::rect<s32> vp = area;
    core::rect<s32> rendert(0,0, m_ScreenSize.Width, m_ScreenSize.Height);
    vp.clipAgainst(rendert);


    if (vp.getHeight()>0 && vp.getWidth()>0)
	{
        glViewport(
			vp.UpperLeftCorner.X, vp.UpperLeftCorner.Y,
            vp.getWidth(), vp.getHeight()
			);
	}

    ViewPort = vp;
}

//--------------------------------------------------------------------------

void COpenGLDriver::_setVertexType(E_VERTEX_TYPE newType)
{
	CNullDriver::_setVertexType(newType);
}

//---------------------------------------------------------------------------

//! sets transformation
void COpenGLDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	CNullDriver::setTransform(state, mat);

	if (!m_UseFFP)
		return;

    switch(state)
    {
    case ETS_VIEW:
	case ETS_MODEL:
        // OpenGL only has a modelview matrix, view and world is not existent. 
		// so lets fake these two.
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(
			core::matrix4(Matrices[ETS_VIEW] * Matrices[ETS_MODEL]).pointer());
        break;
    case ETS_PROJ:
        {
            GLfloat *glmat = (GLfloat*)mat.pointer();

            // flip z to compensate OpenGL right-hand coordinate system
            glmat[12]*= -1.0f;

            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(glmat);
        }
        break;
    }	
}

//---------------------------------------------------------------------------

//! Sets the fog mode.
void COpenGLDriver::setFog(const SFog &fog)
{
    CNullDriver::setFog(fog);

	// setting fog color
    img::SColorf color(fog.Color);
    GLfloat data[4] = {color.r, color.g, color.b, color.a};
    glFogfv(GL_FOG_COLOR, data); 

	// setting fog type
    glFogi(GL_FOG_MODE, convertToOGLFogMode(fog.Type)); 

	// setting fog quality
	glHint(GL_FOG_HINT, GL_NICEST/*GL_FASTEST*/);

	// setting fog parameters
	glFogf(GL_FOG_START, fog.Start); 
	glFogf(GL_FOG_END, fog.End); 
	glFogf(GL_FOG_DENSITY, fog.Density); 
}

//---------------------------------------------------------------------------

//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COpenGLDriver::OnResize(const core::dimension2d<s32>& size)
{
	if (m_ScreenSize == size)
		return;
    CNullDriver::OnResize(size);
    glViewport(0, 0, size.Width, size.Height); 
}

//---------------------------------------------------------------------------

// Create An Empty Texture
GLuint COpenGLDriver::createEmptyOGLTexture(s32 w, s32 h)                                           
{
    GLuint txtnumber;    // Texture ID
    unsigned int* data;  // Stored Data

    // Create Storage Space For Texture Data (w x h x 4)
    data = (unsigned int*)new GLuint[((w * h)* 4 * sizeof(unsigned int))];
    ZeroMemory(data,((w * h)* 4 * sizeof(unsigned int)));   // Clear Storage Memory

    glGenTextures(1, &txtnumber);                               // Create 1 Texture
    // Build Texture Using Information In data
    glBindTexture(GL_TEXTURE_2D, txtnumber);                    // Bind The Texture
    glTexImage2D(
		GL_TEXTURE_2D, 0, 4, w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
		);                       
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);         

    delete [] data;                                             // Release data

    return txtnumber;                                           // Return The Texture ID
}

//---------------------------------------------------------------------------

//! Setting polygon fill mode
void COpenGLDriver::setPolygonFillMode(E_POLYGON_FILL_MODE mode)
{
	CNullDriver::setPolygonFillMode(mode);	

	if (m_PolygonFillMode==EPFM_SOLID)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	if (m_PolygonFillMode==EPFM_WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	// EPFM_POINT
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

//----------------------------------------------------------------------------

//! Reseting all texture units
void COpenGLDriver::resetTextureUnits()
{
	static u32 max_units_cnt_1 = getMaximalTextureUnitsAmount() - 1;
	for (s32 unit = max_units_cnt_1; unit >= 0; unit--)
	{
#ifdef GL_VERSION_1_2
		glActiveTexture(GL_TEXTURE0 + unit);	
#endif
		glMatrixMode(GL_TEXTURE);		
		glLoadIdentity(); 

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);  
		glDisable(GL_TEXTURE_GEN_R);  
		glDisable(GL_TEXTURE_GEN_Q);
	}
	CNullDriver::resetTextureUnits();
}

//----------------------------------------------------------------------------

IRenderBuffer * COpenGLDriver::createRenderBuffer(
	E_RENDER_BUFFER_TYPE rbt,
	E_VERTEX_TYPE vtype, u32 vert_size,
	E_INDEX_TYPE itype, u32 ind_size,
	E_DRAW_PRIMITIVE_TYPE dpt)
{
	switch (vtype)
	{
	case EVT_SIMPLE_COLOURED:
		return createRenderBufferTemplate <S3DVertexSimpleColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS:
		return createRenderBufferTemplate <S3DVertex1TCoords> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS:
		return createRenderBufferTemplate <S3DVertex2TCoords> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_COLOURED:
		return createRenderBufferTemplate <S3DVertex2TCoordsColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN:
		return createRenderBufferTemplate <S3DVertex1TCoordsTBN> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsTBNColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN:
		return createRenderBufferTemplate <S3DVertex2TCoordsTBN> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN_COLOURED:
		return createRenderBufferTemplate <S3DVertex2TCoordsTBNColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_RHW_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsRHWColoured> (
			rbt, vert_size, itype, ind_size, dpt );
	}
	// fail safe
	return createRenderBufferTemplate <S3DVertexSimple> (
		rbt, vert_size, itype, ind_size, dpt );
}

//----------------------------------------------------------------------------

IRenderBuffer * COpenGLDriver::createDynamicRenderBuffer(
	const void * varray, E_VERTEX_TYPE vtype, u32 vert_size,
	const void * iarray, E_INDEX_TYPE itype, u32 ind_size,
	E_DRAW_PRIMITIVE_TYPE dpt)
{
	switch (vtype)
	{
	case EVT_SIMPLE_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertexSimpleColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt );
	case EVT_1TCOORDS:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoords> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoords> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsTBN> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsTBNColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsTBN>  (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsTBNColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_RHW_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsRHWColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	}
	// fail safe
	return createDynamicRenderBufferTemplate <S3DVertexSimple> (
		varray, vert_size, iarray, itype, ind_size, dpt);
}

//----------------------------------------------------------------------------

template <class TVert>
inline IRenderBuffer * COpenGLDriver::createRenderBufferTemplate(
	E_RENDER_BUFFER_TYPE rbt,
	u32 vert_size, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt)
{
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	if (!GLEW_ARB_vertex_buffer_object)
#endif
	{
		if (itype == EIT_32_BIT)
			return new COpenGLRenderArray <TVert, u32> (this, vert_size, ind_size, dpt);
		return new COpenGLRenderArray <TVert, u16> (this, vert_size, ind_size, dpt);
	}
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	if (itype == EIT_32_BIT)
	 	return new COpenGLVertexBufferObject <TVert, u32> (
			rbt == ERBT_DYNAMIC, this, vert_size, ind_size, dpt);
	return new COpenGLVertexBufferObject <TVert, u16> (
		rbt == ERBT_DYNAMIC, this, vert_size, ind_size, dpt);
#endif
}

//----------------------------------------------------------------------------

template <class TVert>
inline IRenderBuffer * COpenGLDriver::createDynamicRenderBufferTemplate(
	const void * varray, u32 vert_size,
	const void * iarray, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt)
{
	if (itype == EIT_32_BIT)
		return new COpenGLRenderArray <TVert, u32> (
			this, (const TVert*)varray, vert_size, (const u32 *)iarray, ind_size, dpt );
	
	return new COpenGLRenderArray <TVert, u16> (
		this, (const TVert*)varray, vert_size, (const u16 *)iarray, ind_size, dpt);
}

//----------------------------------------------------------------------------

void COpenGLDriver::_enableStencil()
{
	if (!m_StencilBuffer || _isStencilEnabled())
		return;
	glEnable(GL_STENCIL_TEST);
	CNullDriver::_enableStencil();
}

//----------------------------------------------------------------------------

void COpenGLDriver::_clearStencil()
{
	if (!m_StencilBuffer)
		return;
	glClear(GL_STENCIL_BUFFER_BIT);
}

//----------------------------------------------------------------------------

void COpenGLDriver::_setupStencil(
	E_STENCIL_OPERATION fail,  // to do if stencil test fails
	E_STENCIL_OPERATION zfail, // to do if stencil test passes and Z test fails
	E_STENCIL_OPERATION zpass, // to do if both stencil and Z tests pass
	E_COMPARE_TEST  func,  // Stencil Test passes if ((ref & mask) func (stencil & mask)) is true
	s32 ref,        // Reference value used in stencil test
	u32 mask        // Mask value used in stencil test
	)
{
	if (!m_StencilBuffer)
		return;

	// setup stencil compare fuction, 
	// reference value, and masks.
	// Stencil test passes if ((ref & mask) func (stencil & mask)) is true.
	glStencilFunc ( convertToOGLCompareFunction ( func ), ref, mask );

	// setup stencil operations
	glStencilOp (
		convertToOGLStencilOp(fail),
		convertToOGLStencilOp(zfail),	
		convertToOGLStencilOp(zpass));
}

//----------------------------------------------------------------------------

void COpenGLDriver::_disableStencil()
{
	if (!m_StencilBuffer || !_isStencilEnabled())
		return;
	glDisable( GL_STENCIL_TEST );
	CNullDriver::_disableStencil();
}

//---------------------------------------------------------------------------

void COpenGLDriver::_enableScissor()
{
	glEnable(GL_SCISSOR_TEST);
}

//---------------------------------------------------------------------------

void COpenGLDriver::_setupScissor(s32 left, s32 top, u32 width, u32 height)
{
	s32 bottom = m_ScreenSize.Height - (top + height);
	glScissor(left, bottom, width, height);
}

//---------------------------------------------------------------------------

void COpenGLDriver::_disableScissor()
{
	glDisable(GL_SCISSOR_TEST);
}

//---------------------------------------------------------------------------

void COpenGLDriver::_renderScreenRect(const img::SColor &color)
{}

//---------------------------------------------------------------------------

//! sets texture filtering mode
void COpenGLDriver::setTextureFilter(E_TEXTURE_FILTER textureFilter)
{
	CNullDriver::setTextureFilter(textureFilter);

	if (m_TextureFilter == ETF_ANISOTROPIC && MaxAnisotropyLevel == 0 )
	{
		m_TextureFilter = ETF_BILINEAR;
		LOGGER.logWarn("OpenGL has no anisotropic filtering. Fall back to bilinear.");
	}
}

//---------------------------------------------------------------------------

bool COpenGLDriver::setColorRenderTarget(ITexture* texture,
	bool clearBackBuffer, bool clearZBuffer, img::SColor color)
{
    // check if we should set the previous RT back

    bool ret = true;

	if (RenderTargetTexture && texture != RenderTargetTexture)
    {
		// flush old render target
		_setTexture(0, RenderTargetTexture);

		// Reading GL texture data in inversed order,
		// because frame buffer have Y-inversed pixel data
		u32 hsrc = 0, hdst = CurrentRendertargetSize.Height - 1;
		for (; hsrc < (u32)CurrentRendertargetSize.Height; hsrc ++, hdst--)
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
				0, hdst, 0, hsrc, CurrentRendertargetSize.Width, 1);
    }

    if (texture == 0)
    {
		CurrentRendertargetSize = core::dimension2d<s32>(0,0);
		glViewport(0,0,m_ScreenSize.Width,m_ScreenSize.Height);		
    }
    else
    {
        // we want to set a new target. so do this.
		CurrentRendertargetSize = texture->getSize();
        glViewport(0, 0, CurrentRendertargetSize.Width, CurrentRendertargetSize.Height);
 	}

	_setTexture(0, texture);

	RenderTargetTexture = (COpenGLTexture*)texture;

    GLbitfield mask = 0;
    if (clearBackBuffer)
    {
		glClearColor(
			color.getRed() * inv_color, color.getGreen() * inv_color,
            color.getBlue() * inv_color, color.getAlpha() * inv_color
			);
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (clearZBuffer)
    {
        glDepthMask(GL_TRUE);
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    glClear(mask);

    return ret; 
}

//---------------------------------------------------------------------------

ITexture* COpenGLDriver::createRenderTargetTexture(core::dimension2d<s32> size)
{
	return new COpenGLRenderTargetTexture(size);
}

//---------------------------------------------------------------------------

void COpenGLDriver::setColorMask(bool r, bool g, bool b, bool a)
{
	CNullDriver::setColorMask(r, g, b, a);
	glColorMask(r, g, b, a);
}

//---------------------------------------------------------------------------

void COpenGLDriver::clearZBuffer()
{
	glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT); 
}

//---------------------------------------------------------------------------

void COpenGLDriver::clearColorBuffer()
{
	glClearColor(
		BackColor.getRed()   * inv_color,
        BackColor.getGreen() * inv_color,
        BackColor.getBlue()  * inv_color,
        BackColor.getAlpha() * inv_color
        );
    glClear(GL_COLOR_BUFFER_BIT);
}

//---------------------------------------------------------------------------

bool COpenGLDriver::setResourceContextCurrent()
{
	// activate resource context
	if (!wglMakeCurrent(HDc, m_ResourceContext))
	{
		LOGGER.logErr("wglMakeCurrent for render context failed");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

bool COpenGLDriver::setRenderContextCurrent()
{
	// activate rendering context
	if (!wglMakeCurrent(HDc, m_RenderContext))
	{
		LOGGER.logErr("wglMakeCurrent for render context failed");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

bool COpenGLDriver::setNullContextCurrent()
{
	if (!wglMakeCurrent(0, 0))
	{
		LOGGER.logWarn("wglMakeCurrent(0, 0) failed.");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------

// returns the current size of the screen or rendertarget
core::dimension2d<s32> COpenGLDriver::getCurrentRenderTargetSize()
{
    if ( CurrentRendertargetSize.Width == 0 )
        return m_ScreenSize;
    else
        return CurrentRendertargetSize;
}

//---------------------------------------------------------------------------

CNullGPUProgram* COpenGLDriver::_createGPUProgram(u32 uniforms, u32 lightcnt,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	if (m_UseShaders)
		return new COpenGLGPUProgram(uniforms, lightcnt,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
#endif
	CNullGPUProgram *gpu_prog = gpu_prog = new CNullGPUProgram();
	gpu_prog->recreate(0, 0, EVSV_GLSL_1_0, NULL, EPSV_GLSL_1_0, NULL);
	return gpu_prog;
}

//---------------------------------------------------------------------------

void COpenGLDriver::polygonOffsetEnable(f32 factor, f32 units)
{
	glPolygonOffset ( factor, units );
	glEnable(GL_POLYGON_OFFSET_FILL);
}

//---------------------------------------------------------------------------

void COpenGLDriver::polygonOffsetDisable()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
}

//---------------------------------------------------------------------------

const core::vector3df& COpenGLDriver::get3DPositionFromScreenCoordinates(
	const core::position2di &scr_pos) const 
{
	GLint viewport[4] = { 0, 0, m_ScreenSize.Width, m_ScreenSize.Height }; 

	const f32 *glmat = m_ModelViewMatrix.pointer();
	
	GLdouble mv_matrix[16] = 
	{
		glmat[ 0], glmat[ 1], glmat[ 2], glmat[ 3],
		glmat[ 4], glmat[ 5], glmat[ 6], glmat[ 7], 
		glmat[ 8], glmat[ 9], glmat[10], glmat[11], 
		glmat[12], glmat[13], glmat[14], glmat[15]
	};

	glmat = Matrices[ETS_PROJ].pointer();

	GLdouble proj_matrix[16] = 
	{
		glmat[ 0], glmat[ 1], glmat[ 2], glmat[ 3],
		glmat[ 4], glmat[ 5], glmat[ 6], glmat[ 7], 
		glmat[ 8], glmat[ 9], glmat[10], glmat[11], 
		glmat[12], glmat[13], glmat[14], glmat[15]
	};

	GLfloat Z; 
	GLdouble newx, newy, newz; 

	GLfloat normaly = (GLfloat)viewport[3]-(GLfloat)scr_pos.Y-1; 

	// reads a block of pixels from the frame buffer.
	// Depth values are read from the depth buffer. Each component is converted to 
	// floating point such that the minimum depth value maps to 0.0 and the maximum value
	// maps to 1.0. Each component is then multiplied by GL_DEPTH_SCALE, added to GL_DEPTH_BIAS, 
	// and finally clamped to the range [0,1]. 
	glReadPixels((GLdouble)scr_pos.X, normaly, 1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &Z); 

	// maps window coordinates to object coordinates.
	gluUnProject((GLdouble)scr_pos.X, (GLdouble)normaly, (GLdouble)Z,
		mv_matrix, proj_matrix, viewport, &newx, &newy, &newz);

	static core::vector3df pos3d(newx, newy, newz);

	return pos3d;
}

//---------------------------------------------------------------------------

//! return hardware occlusion object
IHardwareOcclusionQuery& COpenGLDriver::getHardwareOcclusionQuery()
{
	if (m_OpenGLHardwareOcclusionQuery)
	{
		return *m_OpenGLHardwareOcclusionQuery;
	}

	return m_HardwareOcclusionQueryStub;
}

//---------------------------------------------------------------------------

#if MY_PLATFORM == MY_PLATFORM_WIN32    

//---------------------------------------------------------------------------

#ifdef __MY_BUILD_VID_GL11_LIB__
__MY_EXPORT__ IVideoDriver* createOpenGL11Driver(
#endif
#ifdef __MY_BUILD_VID_GL12_LIB__
__MY_EXPORT__ IVideoDriver* createOpenGL12Driver(
#endif
#ifdef __MY_BUILD_VID_GL21_LIB__
__MY_EXPORT__ IVideoDriver* createOpenGL21Driver(
#endif
#ifdef __MY_BUILD_VID_GL32_LIB__
__MY_EXPORT__ IVideoDriver* createOpenGL32Driver(
#endif
	SExposedVideoData &out_video_data,
	E_TEXTURE_FILTER textureFilter)
{
    COpenGLDriver* ogl =  new COpenGLDriver(out_video_data.WindowSize);
	if (!ogl->_initDriver(out_video_data))
    {
        ((IVideoDriver*)ogl)->drop();
        ogl = 0;
    }
	else
		ogl->setTextureFilter(textureFilter);
    return ogl; 
}

//---------------------------------------------------------------------------

#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
