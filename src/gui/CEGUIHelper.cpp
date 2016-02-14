//|-------------------------------------------------------------------------
//| File:        CEGUIHelper.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>
#include <my_cegui.h>

#include <CEGUI.h>

//----------------------------------------------------------------------------
namespace my {
namespace gui {
//----------------------------------------------------------------------------

MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const core::stringc &strc)
{
	CEGUI::String cegui_str;

	s32 len= strc.size();

	for (s32 i=0; i<len; i++)
	{
		cegui_str += (CEGUI::utf32)strc[i];
	}

	return cegui_str;
}

//----------------------------------------------------------------------------

MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const core::stringw &strw)
{
	CEGUI::String cegui_str;

	s32 len= strw.size();

	for (s32 i=0; i<len; i++)
	{
		cegui_str += (CEGUI::utf32)strw[i];
	}

	return cegui_str;
}

//----------------------------------------------------------------------------

MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const c8 *strc)
{
	CEGUI::String cegui_str;
	s32 len = core::strlen(strc);
	for (s32 i=0; i<len; i++)
		cegui_str += (CEGUI::utf32)strc[i];
	return cegui_str;
}

//----------------------------------------------------------------------------

MYENGINE_GUIRENDERER_API CEGUI::String toCEGUIString(const wchar_t *strw)
{
	CEGUI::String cegui_str;
	s32 len = core::strlen(strw);
	for (s32 i=0; i<len; i++)
		cegui_str += (CEGUI::utf32)strw[i];
	return cegui_str;	
}

//----------------------------------------------------------------------------
} // end namespace gui
} // end namespace my
//----------------------------------------------------------------------------