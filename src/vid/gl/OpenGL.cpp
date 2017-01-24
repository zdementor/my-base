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

#include "OpenGL.h"
#include <dev/SExposedDeviceData.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

MyGLWindow* createGLWindow(MyWindow window)
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
	HDC hdc = GetDC(window);
	if (!hdc)
	{
		LOGGER.logErr("Can not create a GL device context");
		return NULL;
	}
#else
#	error Unimplemented!!!
#endif

	MyGLWindow *win = new MyGLWindow;
	memset(win, 0, sizeof(*win));
#if MY_PLATFORM == MY_PLATFORM_WIN32
	win->hdc = hdc;
#endif
	win->window = window;

	return win;
}

//---------------------------------------------------------------------------

bool destroyGLWindow(MyGLWindow *win)
{
	if (!win)
		return false;
#if MY_PLATFORM == MY_PLATFORM_WIN32
    if (win->hdc)
        ReleaseDC(win->window, win->hdc);
#else
#	error Unimplemented!!!
#endif
	delete win;
	return true;
}

//---------------------------------------------------------------------------

MyGLContext* createGLContext(MyGLWindow *win, bool swrast,
	SExposedVideoData *out_video_data)
{
	MyGLContext *ctx = NULL;
	if (!win || !win->hdc)
		return NULL;

#if MY_PLATFORM == MY_PLATFORM_WIN32
	static  PIXELFORMATDESCRIPTOR pfd =
    {   
		sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
        1,                          // Version Number
        PFD_DRAW_TO_WINDOW |        // Support Window
			PFD_SUPPORT_OPENGL |        // Support OpenGL
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
	HDC hdc = win->hdc;

    for (int i = 0; i < 3; ++i)
    {
		HGLRC hglrc = 0;
		int pfi = 0;

        if (i == 1)
        {
            LOGGER.logWarn(
				"Can not create a GL device with stencil buffer, disabling stencil");
            pfd.cStencilBits = 0;
        }
        else
        if (i == 2)
        {
            LOGGER.logErr("Can not create a GL device context");
            return NULL;
        }

        // choose pixelformat
        if (!(pfi = ChoosePixelFormat(hdc, &pfd)))
        {
            LOGGER.logErr("Can not find a suitable pixel format");
            continue;
        }

		// describe pixelformat

		PIXELFORMATDESCRIPTOR dpfd;
		if (!DescribePixelFormat(hdc, pfi, sizeof(dpfd), &dpfd))
		{
            LOGGER.logErr("Can not describe pixel format");
            continue;
        }

		if (swrast && !(dpfd.dwFlags & PFD_GENERIC_FORMAT))		
		{
			s32 nfmt = DescribePixelFormat(hdc, 0, 0, NULL);
			for (s32 j = 1; j <= nfmt; j++)
			{
				PIXELFORMATDESCRIPTOR tmppfd;
				if (DescribePixelFormat(hdc, j, sizeof(tmppfd), &tmppfd) &&
						(tmppfd.dwFlags & PFD_GENERIC_FORMAT) &&
						(tmppfd.dwFlags & PFD_DOUBLEBUFFER) &&
						(tmppfd.cColorBits >= 16) &&
						tmppfd.cDepthBits >= 16 &&
						tmppfd.iPixelType == PFD_TYPE_RGBA)
				{
					pfi = j;
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
        if (!SetPixelFormat(hdc, pfi, &dpfd))
        {
            LOGGER.logErr("Can not set the pixel format");
            continue;
        }

		// finally describe pixelformat

		if (!DescribePixelFormat(hdc, pfi, sizeof(dpfd), &pfd))
		{
            LOGGER.logErr("Can not describe pixel format");
            continue;
        }

        // create rendering context
        if (!(hglrc = wglCreateContext(hdc)))
        {
            LOGGER.logErr("Can not create a GL rendering context");
            continue;
        }

        if (!wglMakeCurrent(hdc, hglrc))
        {
            LOGGER.logErr("Can not activate GL context");
            continue;
        }
		else
		{
			LOGGER.logInfo("OGL context activated");            
		}

		ctx = new MyGLContext;
		memset(ctx, 0, sizeof(*ctx));
		ctx->context = hglrc;
		ctx->hdc = hdc;
		ctx->colorFormat = (img::E_COLOR_FORMAT)-1;
		if (dpfd.cColorBits == 15 && dpfd.cAlphaBits == 1)
			ctx->colorFormat = img::ECF_A1R5G5B5;
		else if (dpfd.cColorBits == 16 && dpfd.cAlphaBits == 0)
			ctx->colorFormat = img::ECF_R5G6B5;
		else if (dpfd.cColorBits >= 32)
			ctx->colorFormat = img::ECF_A8R8G8B8;
		else if (dpfd.cColorBits >= 24)
			ctx->colorFormat = img::ECF_R8G8B8;
		ctx->colorBits   = pfd.cColorBits;
		ctx->alphaBits   = pfd.cAlphaBits;
		ctx->depthBits   = pfd.cDepthBits;
		ctx->stencilBits = pfd.cStencilBits;

		if (out_video_data)
		{
			// set exposed data
			out_video_data->Win32.OGL.HDc  = reinterpret_cast<void*>(hdc);
			out_video_data->Win32.OGL.HRc  = reinterpret_cast<void*>(hglrc);	
		}
		break;
	}
#else
#	error Unimplemented!!!
#endif
	return ctx;
}

//---------------------------------------------------------------------------

bool destroyGLContext(MyGLContext *ctx)
{
	bool res = true;
	if (!ctx)
		return res;
	if (ctx->context)
    {
#if MY_PLATFORM == MY_PLATFORM_WIN32
		if (!wglDeleteContext(ctx->context))
		{
            LOGGER.logWarn("Release of rendering context failed.");
			res = false;
		}
#else
#	error Unimplemented!!!
#endif
    }
	delete ctx;
	return res;
}

//---------------------------------------------------------------------------

bool setGLContext(MyGLWindow *win, MyGLContext *ctx)
{
	if (win && win->hdc)
	{
#if MY_PLATFORM == MY_PLATFORM_WIN32
		if (ctx && ctx->hdc == win->hdc)
		{
			if (!wglMakeCurrent(win->hdc, ctx->context))
			{
				LOGGER.logErr("wglMakeCurrent failed");
				return false;
			}
		}
		else
		{
			if (!wglMakeCurrent(win->hdc, 0))
			{
				LOGGER.logErr("wglMakeCurrent failed");
				return false;
			}
		}
#else
#	error Unimplemented!!!
#endif
		win->context = ctx;
	}
	return true;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
