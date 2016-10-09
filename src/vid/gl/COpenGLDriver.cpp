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
#include "COpenGLRenderTarget.h"

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
HWnd(0), HDc(0), m_RenderContext(0),
m_OpenGLHardwareOcclusionQuery(0)
{
#if MY_DEBUG_MODE 
	IVideoDriver::setClassName("COpenGLDriver::IVideoDriver");
#endif
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

	m_BackColorFormat = (img::E_COLOR_FORMAT)-1;

    for (int i = 0; i < 3; ++i)
    {
        if (i == 1)
        {
            LOGGER.logWarn("Can not create a GL device with stencil buffer, disabling stencil shadows");
            m_StencilBuffer = false;
            pfd.cStencilBits = 0;
        }
        else
        if (i == 2)
        {
            LOGGER.logErr("Can not create a GL device context");
            return false;
        }

        // get hdc
        if (!(HDc=GetDC(HWnd)))
        {
            LOGGER.logErr("Can not create a GL device context");
            continue;
        }        

        // choose pixelformat
        if (!(PixelFormat = ChoosePixelFormat(HDc, &pfd)))
        {
            LOGGER.logErr("Can not find a suitable pixel format");
            continue;
        }

		// describe pixelformat

		PIXELFORMATDESCRIPTOR dpfd;
		if (!DescribePixelFormat(HDc, PixelFormat, sizeof(dpfd), &dpfd))
		{
            LOGGER.logErr("Can not describe pixel format");
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
						(tmppfd.cColorBits >= 16) &&
						tmppfd.cDepthBits >= 16 &&
						tmppfd.iPixelType == PFD_TYPE_RGBA)
				{
					PixelFormat = j;
					dpfd = tmppfd;
					break;
				}
			}
			if (!(dpfd.dwFlags & PFD_GENERIC_FORMAT))
			{
				LOGGER.logErr("Can not find GDI generic pixel format");
			}
		}

        // set pixel format
        if (!SetPixelFormat(HDc, PixelFormat, &dpfd))
        {
            LOGGER.logErr("Can not set the pixel format");
            continue;
        }

        // create rendering context
        if (!(m_RenderContext = wglCreateContext(HDc)))
        {
            LOGGER.logErr("Can not create a GL rendering context");
            continue;
        }

        if (!setRenderContextCurrent())
        {
            LOGGER.logErr("Can not activate GL context");
            continue;
        }
		else
		{
			LOGGER.logInfo("OGL context activated");            
		}

		if (dpfd.cColorBits == 15 && dpfd.cAlphaBits == 1)
			m_BackColorFormat = img::ECF_A1R5G5B5;
		else if (dpfd.cColorBits == 16 && dpfd.cAlphaBits == 0)
			m_BackColorFormat = img::ECF_R5G6B5;
		else if (dpfd.cColorBits >= 32)
			m_BackColorFormat = img::ECF_A8R8G8B8;
		else if (dpfd.cColorBits >= 24)
			m_BackColorFormat = img::ECF_R8G8B8;

		break;
    }

	if ((u32)m_BackColorFormat >= img::E_COLOR_FORMAT_COUNT)
	{
		LOGGER.logErr("Could not detect BackBuffer color format!");
		return false;
	}

    // print renderer information
    core::stringc glRend = glGetString(GL_RENDERER);
    core::stringc glVend = glGetString(GL_VENDOR);
	core::stringc glVer = glGetString(GL_VERSION);
	core::stringc glExt = glGetString(GL_EXTENSIONS);

	LOGGER.logInfo("Video adapter model: %s %s", glRend.c_str(), glVend.c_str());
	LOGGER.logInfo("OGL driver version: %s", glVer.c_str());
	LOGGER.logInfo("Supported OpenGL extentions:\n  %s", glExt.c_str());

	m_StencilBuffer &= m_Shadows;

	// load extensions

	glewInit();

	bool vboSupport = false;

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
#elif  __MY_BUILD_GL_VER__ == MY_DRIVER_TYPE_OPENGL21
    if (!GLEW_VERSION_2_1 || !GLEW_VERSION_2_0
			|| !GLEW_VERSION_1_5 || !GLEW_VERSION_1_4 || !GLEW_VERSION_1_3
			|| !GLEW_VERSION_1_2 || !GLEW_VERSION_1_1)
	{
        LOGGER.logErr("OpenGL driver version is not 2.1 or better.");
		return false;
	}
	if (!GLEW_ARB_imaging)
	{
        LOGGER.logErr("OpenGL driver is not supported GL_ARB_imaging extension.");
		return false;
	}
	if (!GLEW_EXT_texture_compression_s3tc)
	{
		LOGGER.logWarn("OpenGL driver is not supported EXT_texture_compression_s3tc extension,"
			"be aware from using DXT textures.");
	}
	vboSupport = true;
#endif

	GLint temp;
#ifdef GL_VERSION_1_2
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &temp);
	m_MaxTextureUnits = temp;
#else
	m_MaxTextureUnits = 1;
#endif	
	CHECK_MAX_RANGE(m_MaxTextureUnits, MY_MATERIAL_MAX_LAYERS);

    if (m_MaxTextureUnits < 2)
		LOGGER.logWarn(
			"OpenGL device only has one texture unit. Disabling multitexturing.");

#ifdef GL_EXT_texture_filter_anisotropic
	if (GLEW_EXT_texture_filter_anisotropic)
	{
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &temp);
		m_MaxAnisotropyLevel = temp;
	}
	else
#endif
		m_MaxAnisotropyLevel = 0;

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

#ifdef GL_ARB_vertex_array_object
	if (GLEW_ARB_vertex_array_object)
		COpenGLVertexArrayObject::ms_VAOSupport = true;
	else
#endif
		COpenGLVertexArrayObject::ms_VAOSupport = false;

#if defined(GL_ARB_texture_non_power_of_two)
	m_TexturesNonPowerOfTwo = !!GLEW_ARB_texture_non_power_of_two;
#else
	m_TexturesNonPowerOfTwo = false;
#endif

#if  __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&m_MaxDrawBuffers);
	CHECK_RANGE(m_MaxDrawBuffers, 1, MY_MAX_COLOR_ATTACHMENTS);
#else
	m_MaxDrawBuffers = 1;
#endif

	m_RenderTargetSupport = m_DepthStencilTexturesSupport =
#if defined(GL_ARB_framebuffer_object)
		!!GLEW_ARB_framebuffer_object;
#else
        false;
#endif

#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
	u32 shVerMaj = 0, shVerMin = 0;
	const GLubyte *shVer = glGetString(GL_SHADING_LANGUAGE_VERSION);
	sscanf((const c8 *)shVer, "%d.%d", &shVerMaj, &shVerMin);
	CHECK_RANGE(shVerMaj, 0, 255);
	CHECK_RANGE(shVerMin, 0, 255);

	m_PixelShaderVersion = m_VertexShaderVersion =
		(0xff00 & shVerMaj << 8) | (0xff & shVerMin);
#endif

	if (!vboSupport)
		LOGGER.logWarn(
			"Vertex Buffer Objects not supported (using vertex arrays instead).");
	if (!COpenGLVertexArrayObject::ms_VAOSupport)
		LOGGER.logWarn("Vertex Array Objects not supported.");

	// OpenGL driver constants

	glGetIntegerv(GL_MAX_LIGHTS, &temp); 
	CHECK_MIN_RANGE(temp, 8);
	m_MaxLights = temp;	

	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &temp );
	m_MaxTextureSize = core::dimension2di(temp, temp);

	// setup ogl initial render states    
	glShadeModel(GL_SMOOTH);
    
	glClearDepth(1.0f);
	glClearStencil(MY_STENCIL_ZERO_VALUE);    

	glFrontFace( GL_CCW );		

	glDisable(GL_COLOR_MATERIAL);
	
	// for specular highlights
#ifdef GL_VERSION_1_2
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR); 
#endif
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	// set exposed data
    out_video_data.Win32.OGL.HDc  = reinterpret_cast<void*>(HDc);
    out_video_data.Win32.OGL.HRc  = reinterpret_cast<void*>(m_RenderContext);	
	out_video_data.DriverType = EDT_OPENGL21;
  
    DescribePixelFormat(HDc, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.cStencilBits == 0)
	{
		LOGGER.logWarn("Device does not support Stencil Buffer, disabling it.");
		m_StencilBuffer = false;
	}
	else 
		m_StencilBuffer = true;

	m_ColorBits = pfd.cColorBits;
    m_AlphaBits = pfd.cAlphaBits;
    m_DepthBits = pfd.cDepthBits;
    m_StencilBits = pfd.cStencilBits;

	if (WGLEW_EXT_swap_control && wglSwapIntervalEXT(m_VerticalSync?1:0)!=0)
		LOGGER.logInfo("Vertical Sync %s", m_VerticalSync ? "enabled" : "disabled");
	else
		LOGGER.logInfo("Can't on/off Vertical Sync ");

    return CNullDriver::_initDriver(out_video_data);
}

//---------------------------------------------------------------------------

COpenGLDriver::~COpenGLDriver()
{
	free();

	SAFE_DROP(m_OpenGLHardwareOcclusionQuery);

	setNullContextCurrent();

    if (m_RenderContext)
    {
        if (!wglDeleteContext(m_RenderContext))
            LOGGER.logWarn("Release of rendering context failed.");
        m_RenderContext = 0;
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

void COpenGLDriver::renderPass(E_RENDER_PASS pass)
{
	CNullDriver::renderPass(pass);

	_setupAttributes(COpenGLRenderBuffer::ms_EnabledAttribs);

#ifdef GL_VERSION_1_2
	glClientActiveTexture(GL_TEXTURE0);
#endif
}

//---------------------------------------------------------------------------

#define MY_ENABLE_CLIENT_STATE(index, state, preFunc) \
	if (!enabledAttribs[index]) \
	{ \
		preFunc; \
		glEnableClientState(state); \
		enabledAttribs[index] = true; \
	}

#define MY_DISABLE_CLIENT_STATE(index, state, preFunc) \
	if (enabledAttribs[index]) \
	{ \
		preFunc; \
		glDisableClientState(state); \
		enabledAttribs[index] = false; \
	}

//---------------------------------------------------------------------------

void COpenGLDriver::_setupAttributes(
	bool *enabledAttribs,
	GLenum type0, s32 size0, s32 stride0, const void *ptr0,
	GLenum type1, s32 size1, s32 stride1, const void *ptr1,
	GLenum type2, s32 size2, s32 stride2, const void *ptr2,
	GLenum type3, s32 size3, s32 stride3, const void *ptr3,
	GLenum type4, s32 size4, s32 stride4, const void *ptr4,
	GLenum type5, s32 size5, s32 stride5, const void *ptr5,
	GLenum type6, s32 size6, s32 stride6, const void *ptr6
	)
{
	(void)size1; // normal

	if (type0 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(0, GL_VERTEX_ARRAY, (void)0)
		glVertexPointer(size0, type0, stride0, ptr0);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(0, GL_VERTEX_ARRAY, (void)0)
	}

	if (type1 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(1, GL_NORMAL_ARRAY, (void)0)
		glNormalPointer(type1, stride1, ptr1);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(1, GL_NORMAL_ARRAY, (void)0)
	}

	if (type2 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(2, GL_COLOR_ARRAY, (void)0)
		glColorPointer(size2, type2, stride2, ptr2);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(2, GL_COLOR_ARRAY, (void)0)
	}

	if (type3 != GL_NONE)
	{
#ifdef GL_VERSION_1_2
		glClientActiveTexture(GL_TEXTURE0);
#endif
		MY_ENABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size3, type3, stride3, ptr3);
	}
	else
	{
#ifdef GL_VERSION_1_2
		MY_DISABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE0))
#else
		MY_DISABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, (void)0)
#endif
	}

#ifdef GL_VERSION_1_2
	if (type4 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE1);
		MY_ENABLE_CLIENT_STATE(4, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size4, type4, stride4, ptr4);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(4, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE1))
	}

	if (type5 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE2);
		MY_ENABLE_CLIENT_STATE(5, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size5, type5, stride5, ptr5);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(5, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE2))
	}

	if (type6 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE3);
		MY_ENABLE_CLIENT_STATE(6, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size6, type6, stride6, ptr6);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(6, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE3))
	}
#endif
}

//---------------------------------------------------------------------------

bool COpenGLDriver::_makeScreenShot(img::IImage *image)
{
	c8 *p = (c8 *)image->getData();
	u32 pitch = image->getBytesPerPixel() * image->getDimension().Width;

	glReadBuffer(GL_BACK);

	// reading in verticaly reversed order
	for (s32 i=0; i < m_ScreenSize.Height; i++)
	{
		glReadPixels(0, m_ScreenSize.Height - 1 - i,
			m_ScreenSize.Width, 1, GL_BGRA, GL_UNSIGNED_BYTE, p);
		p += pitch;
	}

	return true;
}

//----------------------------------------------------------------------------

void COpenGLDriver::_renderStencilVolume(IRenderBuffer * rbuf, const SRenderPass &pass, bool zfail)
{
	if (!m_StencilBuffer)  
        return;

	if (!((CNullRenderBuffer*)rbuf)->bind())
	{
		LOGGER.logErr(__FUNCTION__ ": Can not bind buffer to render!");
		return;
	}

	CNullDriver::_renderStencilVolume(rbuf, pass, zfail);

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
			((CNullRenderBuffer*)rbuf)->render();
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
			((CNullRenderBuffer*)rbuf)->render();

			// Second Pass:
			// If ztest fail decrement stencil buffer value
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_DECR,	// depth test fail
				GL_KEEP);	// depth test pass
			// Draw front-side of shadow volume in stencil only
			glFrontFace( GL_CCW );
			((CNullRenderBuffer*)rbuf)->render();
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
			((CNullRenderBuffer*)rbuf)->render();
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
			((CNullRenderBuffer*)rbuf)->render();

			// Second Pass:
			// Decrement stencil buffer value
			glStencilOp( 
				GL_KEEP,	// stencil test fail
				GL_KEEP,	// depth test fail
				GL_DECR);	// depth test pass
			// Draw back-side of shadow volume in stencil only
			glFrontFace(GL_CW);
			((CNullRenderBuffer*)rbuf)->render();
		}
	}

	((CNullRenderBuffer*)rbuf)->unbind();

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
		GLuint prog = gpu_prog ? ((COpenGLGPUProgram*)gpu_prog)->_getProgramHandle() : 0;
		glUseProgram(prog);
	}
#endif
	return CNullDriver::_bindGPUProgram(gpu_prog);
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_beginRendering()
{
	CNullDriver::_beginRendering();

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
	CNullDriver::_endRendering();

	glFlush();

	return true;
}

//----------------------------------------------------------------------------

bool COpenGLDriver::_swapBuffers()
{
	CNullDriver::_swapBuffers();

	bool res = true;
#if MY_PLATFORM == MY_PLATFORM_WIN32
	res = SwapBuffers(HDc) != 0;
#endif
	return res;
}

//---------------------------------------------------------------------------

bool COpenGLDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
    switch (feature)
    {
	case EVDF_BILINEAR_FILTER:
    case EVDF_TRILINEAR_FILTER:
    case EVDF_MIP_MAP:
        return true;
	case EVDF_SHADER_LANGUAGE:
	case EVDF_OCCLUSION_QUERY:
#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
		return true;
#else
		return false;
#endif
	case EVDF_COMPRESSED_TEXTURES:
#if defined(GL_EXT_texture_compression_s3tc)
		return GLEW_EXT_texture_compression_s3tc != NULL;
#else
		return false;
#endif
	default:
		break;
    };

	return CNullDriver::queryFeature(feature);
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

vid::ITexture* COpenGLDriver::_createDeviceDependentTexture(
	core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	 return new COpenGLTexture(size, format, TextureCreationFlags);
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
		bool frontCCW;
		if (!m_YInverted)
		{
			// Reversed culling to correctly cull scene defined in Direct3D
			// left handed coordiname system (OpenGL have right handed system).
			//  ^ Y
			//  |    ^ Z
			//  |   /
			//  |  /
			//  | /
			//  o-------> X
			//
			frontCCW = !m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW);
		}
		else
		{
			// Due to OpenGL FBO vertical inversion, must invert culling order.
			// So we gain double inversion (Direct3D inversion and OpenGL FBO inversion).
			// which results in the normal culling order.
			frontCCW = m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW);
		}
		glFrontFace(frontCCW ? GL_CCW : GL_CW);

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
					core::math::Min(8, m_MaxAnisotropyLevel));
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
#if GL_ARB_framebuffer_object
		if (tex->getColorFormat() == img::ECF_DEPTH24_STENCIL8)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		}
#endif
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

void COpenGLDriver::setGlobalAmbientColor(const img::SColorf& color)
{
	CNullDriver::setGlobalAmbientColor(color);

    GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}

//---------------------------------------------------------------------------

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
        glLoadMatrixf(m_ModelViewMatrix.pointer());
        break;
    case ETS_PROJ:
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(m_HWProjMatrix.pointer());
        break;
	default:
		break;
    }	
}

//---------------------------------------------------------------------------

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

void COpenGLDriver::OnResize(const core::dimension2d<s32>& size)
{
	if (m_ScreenSize == size)
		return;
    CNullDriver::OnResize(size);
    glViewport(0, 0, size.Width, size.Height); 
}

//---------------------------------------------------------------------------

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

IRenderBuffer* COpenGLDriver::createRenderBuffer(
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
#if __MY_BUILD_GL_VER__ < MY_DRIVER_TYPE_OPENGL21
	if (itype == EIT_32_BIT)
		return new COpenGLRenderArray <TVert, u32> (this, vert_size, ind_size, dpt);
	return new COpenGLRenderArray <TVert, u16> (this, vert_size, ind_size, dpt);
#endif
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
	s32 bottom;
	// Scissor defining in the Screen Coordinate system
	//
	//       Y ^-------o (Width, Height)
	//         |       |
	//         |       |
	//  (0, 0) o-------> X 
	//
	if (m_YInverted)
	{
		// Verticaly reversed Scissor for the Y-inversed FBO rendering
		bottom = top;
	}
	else
	{
		// Normal Scissor Coordinate system
		bottom = m_ScreenSize.Height - (top + height);
	}

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

void COpenGLDriver::setTextureFilter(E_TEXTURE_FILTER textureFilter)
{
	CNullDriver::setTextureFilter(textureFilter);

	if (m_TextureFilter == ETF_ANISOTROPIC && m_MaxAnisotropyLevel == 0 )
	{
		m_TextureFilter = ETF_BILINEAR;
		LOGGER.logWarn("OpenGL has no anisotropic filtering. Fall back to bilinear.");
	}
}

//---------------------------------------------------------------------------

ITexture* COpenGLDriver::createRenderTargetTexture(
	const core::dimension2di &size, img::E_COLOR_FORMAT format)
{
	return (queryFeature(EVDF_RENDER_TO_TARGET)) ?
		new COpenGLRenderTargetTexture(size, format, TextureCreationFlags) : NULL;
}

//---------------------------------------------------------------------------

IRenderTarget* COpenGLDriver::createRenderTarget()
{
	return (queryFeature(EVDF_RENDER_TO_TARGET)) ?
		new COpenGLRenderTarget() : NULL;
}

//---------------------------------------------------------------------------

IRenderTarget* COpenGLDriver::createRenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
{
	return (queryFeature(EVDF_RENDER_TO_TARGET)) ?
		new COpenGLRenderTarget(size, colorFormat, depthFormat) : NULL;
}

//---------------------------------------------------------------------------

IRenderTarget* COpenGLDriver::createRenderTarget(
	ITexture *colorTexture, ITexture *depthTexture)
{
	return (queryFeature(EVDF_RENDER_TO_TARGET)) ?
		new COpenGLRenderTarget(colorTexture, depthTexture) : NULL;
}

//---------------------------------------------------------------------------

bool COpenGLDriver::setRenderTarget(IRenderTarget *rt)
{
	bool ret = CNullDriver::setRenderTarget(rt);
	if (m_CurrentRenderTarget != NULL)
	{
		// Must setup Y-inversed rendering,
		// to compensate OpenGL FBO vertical picture inversion.
		m_YInverted = true;
	}
	else
	{
		m_YInverted = false;
	}
	return ret;
}

//---------------------------------------------------------------------------

void COpenGLDriver::setColorMask(bool r, bool g, bool b, bool a)
{
	CNullDriver::setColorMask(r, g, b, a);
	glColorMask(r, g, b, a);
}

//---------------------------------------------------------------------------

void COpenGLDriver::clearDepth()
{
	glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT); 
}

//----------------------------------------------------------------------------

void COpenGLDriver::clearStencil()
{
	if (!m_StencilBuffer)
		return;
	glClear(GL_STENCIL_BUFFER_BIT);
}

//---------------------------------------------------------------------------

void COpenGLDriver::clearColor(u8 r, u8 g, u8 b, u8 a)
{
	glClearColor(
		r * inv_color, g * inv_color, b * inv_color, a * inv_color);
    glClear(GL_COLOR_BUFFER_BIT);
}

//---------------------------------------------------------------------------

void COpenGLDriver::render2DRect(const SMaterial &material,
	const core::rectf &drawRect, const core::rectf &texRect)
{
	// Recalculate draw Rectangle
	// from normalized Screen space to identity Rasterizator space
	// (0, 0) 0---1 (1, 0)             (-1, 1) 0---1 ( 1, 1)
	//        | \ |           ---->            | \ |
	// (0, 1) 3---2 (1, 1)             (-1,-1) 3---2 ( 1,-1)
	f32 drawLeft   = drawRect.UpperLeftCorner.X * 2.f - 1.f;
	f32 drawTop    =(drawRect.UpperLeftCorner.Y * 2.f - 1.f) * (-1);
	f32 drawRight  = drawRect.LowerRightCorner.X * 2.f - 1.f;
	f32 drawBottom =(drawRect.LowerRightCorner.Y * 2.f - 1.f) * (-1);

	vid::S3DVertex1TCoords vertices[] =
	{
		S3DVertex1TCoords(drawLeft,  drawTop,    0, 0, 0, 0, texRect.Left,  texRect.Top),
		S3DVertex1TCoords(drawRight, drawTop,    0, 0, 0, 0, texRect.Right,	texRect.Top),
		S3DVertex1TCoords(drawRight, drawBottom, 0, 0, 0, 0, texRect.Right,	texRect.Bottom),
		S3DVertex1TCoords(drawLeft,  drawBottom, 0, 0, 0, 0, texRect.Left,  texRect.Bottom),
	};

	if (getRenderTarget())
	{
		vertices[1] = S3DVertex1TCoords(drawLeft,  drawBottom, 0, 0, 0, 0, texRect.Left,  texRect.Bottom);
		vertices[3] = S3DVertex1TCoords(drawRight, drawTop,    0, 0, 0, 0, texRect.Right,	texRect.Top);
	}

	// store initial render states
	bool stencil = _isStencilEnabled();
	core::matrix4 matModel = getTransform(ETS_MODEL);
	core::matrix4 matView = getTransform(ETS_VIEW);
	core::matrix4 matProj = getTransform(ETS_PROJ);

	// identity Rasterizator space
	core::matrix4 m;
	setTransform(ETS_MODEL, m);
	setTransform(ETS_VIEW, m);
	setTransform(ETS_PROJ, m);

	// seet render states
	_disableStencil();

	// draw
	COpenGLRenderArray<vid::S3DVertex1TCoords, u16> rbuf(
		this,
		vertices, sizeof(vertices) / sizeof(*vertices),
		NULL, 0,
		vid::EDPT_TRIANGLE_FAN,
		false);
	renderBuffer(&rbuf, material);

	// restore modified states
	if (stencil)
		_enableStencil();
	setTransform(ETS_MODEL, matModel);
	setTransform(ETS_VIEW, matView);
	setTransform(ETS_PROJ, matProj);
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
