//|-------------------------------------------------------------------------
//| File:        my_cegui.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_ceguiHPP
#define my_ceguiHPP
//----------------------------------------------------------------------------

#include <gui/MyCEGUISystem.h>

//----------------------------------------------------------------------------
namespace my {
namespace gui {
//----------------------------------------------------------------------------

MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const core::stringc &strc);
MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const core::stringw &strw);
MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const c8      *strc);
MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const wchar_t *strw);

#define CEGUI_URECT(x1,y1,x2,y2) CEGUI::URect(cegui_reldim(x1), cegui_reldim(y1), cegui_reldim(x2), cegui_reldim(y2))
#define CEGUI_UVECTOR2(x,y)      CEGUI::UVector2(cegui_reldim(x), cegui_reldim(y))

//----------------------------------------------------------------------------
} // end namespace gui
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_ceguiHPP