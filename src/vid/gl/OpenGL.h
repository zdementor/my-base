//|-------------------------------------------------------------------------
//| File:        COpenGL.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLHPP
#define COpenGLHPP
//---------------------------------------------------------------------------

#include "../CNullDriver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

#if defined __MY_BUILD_VID_GL11_LIB__
#	define __MY_BUILD_GL_VER__ MY_DRIVER_TYPE_OPENGL11
#elif defined __MY_BUILD_VID_GL21_LIB__
#	define __MY_BUILD_GL_VER__ MY_DRIVER_TYPE_OPENGL21
#elif defined __MY_BUILD_VID_GL32_LIB__
#	define __MY_BUILD_GL_VER__ MY_DRIVER_TYPE_OPENGL32
#else
#	error "Building unknown OpenGL driver version"
#endif

#if defined __MY_BUILD_VID_MESA_LIB__
#	define GLAPI extern
#	define GLAPIENTRY __stdcall
#	define GLEWAPI extern
#endif

#include <GL/glew.h>
#include <GL/wglew.h>

#define MY_VERTEX_MAX_ATTRIBS (MY_MATERIAL_MAX_LAYERS+3)

//---------------------------------------------------------------------------

#if MY_PLATFORM == MY_PLATFORM_WIN32
typedef HWND  MyWindow;
typedef HGLRC MyGLHwContext;
#endif

//---------------------------------------------------------------------------

struct MyGLContext
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
	HDC                     hdc;
#endif
	MyGLHwContext           context;
	my::img::E_COLOR_FORMAT colorFormat;
	my::u8                  colorBits;
	my::u8                  alphaBits;
	my::u8                  depthBits;
	my::u8                  stencilBits;
};

//---------------------------------------------------------------------------

struct MyGLWindow
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
	HDC           hdc;
#endif
	MyWindow      window;
	MyGLContext   *context;
};

//---------------------------------------------------------------------------

MyGLWindow* createGLWindow(MyWindow window);
bool destroyGLWindow(MyGLWindow *win);

MyGLContext* createGLContext(MyGLWindow *win, bool swrast,
	SExposedVideoData *out_video_data = NULL);
bool destroyGLContext(MyGLContext *ctx);

bool setGLContext(MyGLWindow *win, MyGLContext *ctx);

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
