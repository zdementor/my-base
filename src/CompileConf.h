//|-------------------------------------------------------------------------
//| File:        CompileConf.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CompileConfHPP
#define CompileConfHPP
//----------------------------------------------------------------------------

//! The defines for different operating system are:
//! WIN32 for Windows
//! OS2 for OS/2
//! MACOS for MacOs
//! LINUX for linux and unix (it is defined here if no other os is defined)
#if !defined(WIN32) && !defined(OS2) && !defined(MACOS)
    #define LINUX
#endif

//-----------------------------------------------------------------------------

//! Define _IRR_COMPILE_WITH_DIRECTX_9_compile 
//! the my engine with DirectX9.
//! If you only want to use the software device or opengl this can be useful.
//! This switch is mostly disabled because people do not get the g++ compiler compile
//!  directX header files, and directX is only available on windows platforms.
#if defined(WIN32) && !defined(__GNUC__)
    #define _IRR_COMPILE_WITH_DIRECTX_9_
#endif

//-----------------------------------------------------------------------------

//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the irrlicht engine with OpenGL.
//! If you do not wish to be the engine to compiled with OpengGL, comment this
//! define. 
#define _IRR_COMPILE_WITH_OPENGL_

//-----------------------------------------------------------------------------

//! Define _IRR_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
//! This enables the engine to read from compressed .zip archives. If you
//! disable this feature, the engine can still read archives, but only uncompressed
//! ones. 
#define _IRR_COMPILE_WITH_ZLIB_

//-----------------------------------------------------------------------------

//! Define _IRR_COMPILE_WITH_JPEGLIB_ to enable compiling the engine using libjpeg.
//! This enables the engine to read jpeg images. If you comment this out,
//! the engine will no longer read .jpeg images.

#define _IRR_COMPILE_WITH_LIBJPEG_

//-----------------------------------------------------------------------------

//! Define _IRR_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libpng.
//! This enables the engine to read png images. If you comment this out,
//! the engine will no longer read .png images. 

#define _IRR_COMPILE_WITH_LIBPNG_

//-----------------------------------------------------------------------------

//! Define _MY_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libmini.
//! This enables the engine to generate mimmap landscaping with LibLini Library

#define _MY_COMPILE_WITH_LIBPNG_

//-----------------------------------------------------------------------------

//! Define _MY_USE_ODE_DYNAMIC_ to enable compiling the engine using ode.
//! This enables the ode physics in MyEngine

#define _MY_USE_ODE_DYNAMIC_

//-----------------------------------------------------------------------------

//! Define _MY_COMPILE_WITH_CAL3D_ to enable compiling the engine with cal3d lib.
//! This enables the cal3d - character animation library in MyEngine

#define _MY_COMPILE_WITH_CAL3D_

//-----------------------------------------------------------------------------

//! Define _MY_COMPILE_WITH_LUA_ to enable compiling the engine with lua lib.
//! This enables the lua - script library in MyEngine

#define _MY_COMPILE_WITH_LUA_

//-----------------------------------------------------------------------------

//! Define _MY_COMPILE_WITH_CALUA_ to enable compiling the engine with calua lib.
//! This enables the calua - lua script ehancement library

#define _MY_COMPILE_WITH_CALUA_

//-----------------------------------------------------------------------------
#endif
