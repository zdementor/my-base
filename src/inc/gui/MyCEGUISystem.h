//|-------------------------------------------------------------------------
//| File:        MyCEGUISystem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef MyCEGUISystemHPP
#define MyCEGUISystemHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <CEGUI.h>

#if defined( __WIN32__ ) || defined( _WIN32 ) || defined (WIN32)
#   ifdef MYRENDERER_EXPORTS
#       define MYENGINE_GUIRENDERER_API __declspec(dllexport)
#   else
#       define MYENGINE_GUIRENDERER_API __declspec(dllimport)
#   endif
#else
#   define MYENGINE_GUIRENDERER_API
#endif

using namespace my;

//------------------------------------------------------------------------------------
namespace MyCEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

//! create my CEGUI system components (renderer, scripting module)
MYENGINE_GUIRENDERER_API bool create(const c8* cegui_ini_file_name, void* lua_scripter_ptr);

//! forward event to CEGUI system 
MYENGINE_GUIRENDERER_API bool onEvent(const io::SEvent& event);

//! register for future rendering
MYENGINE_GUIRENDERER_API void registerForRendering();

//! destroy my CEGUI system components
MYENGINE_GUIRENDERER_API void destroy();

//! sets widget text using utf8 encoding
MYENGINE_GUIRENDERER_API void setWindowTextUtf8(CEGUI::Window *win, const c8* utf8_text);
MYENGINE_GUIRENDERER_API void setListboxItemTextUtf8(CEGUI::ListboxItem *item, const c8* utf8_text);

//! return MyEngine texture from CEGUI texture
MYENGINE_GUIRENDERER_API vid::ITexture* getTexture(CEGUI::Texture &ceguitex);

//! set CEGUI texture from MyEngine texture
MYENGINE_GUIRENDERER_API void setTexture(CEGUI::Texture &ceguitex, vid::ITexture *mytex);

//---------------------------------------------------------------------------- 
} // end namespace MyCEGUI
//---------------------------------------------------------------------------- 

#endif //#ifndef MyCEGUISystemHPP
