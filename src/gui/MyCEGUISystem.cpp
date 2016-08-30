//|-------------------------------------------------------------------------
//| File:        MyCEGUISystem.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <gui/MyCEGUISystem.h>

#include "MyCEGUIRenderer.h"
#include "MyCEGUITexture.h"
#include "MyEngineEventPusher.h"
#include "iniFile/iniFile.h"

#include <vid/IVideoDriver.h>

#include <dev/IProfiler.h>

#include "MyCEGUILogger.h"

//---------------------------------------------------------------------------

#define MY_CEGUI_PATH_BASE               "DefaultMediaRoot"
#define MY_CEGUI_PATH_FONTS              "DefaultFonts"
#define MY_CEGUI_PATH_IMAGESETS          "DefaultImagesets"
#define MY_CEGUI_PATH_LAYOUTS            "DefaultLayouts" 
#define MY_CEGUI_PATH_LOOKNFEELS         "DefaultLookNFeels"
#define MY_CEGUI_PATH_SCHEMES            "DefaultSchemes" 

#define MY_CEGUI_PATH_COMMON_BASE        "CommonMediaRoot"
#define MY_CEGUI_PATH_COMMON_FONTS       "CommonFonts"
#define MY_CEGUI_PATH_COMMON_IMAGESETS   "CommonImagesets"
#define MY_CEGUI_PATH_COMMON_LAYOUTS     "CommonLayouts" 
#define MY_CEGUI_PATH_COMMON_LOOKNFEELS  "CommonLookNFeels"
#define MY_CEGUI_PATH_COMMON_SCHEMES     "CommonSchemes" 

#define MY_CEGUI_INI_SECTION "SETTINGS"

//------------------------------------------------------------------------------------
namespace MyCEGUI { 
//------------------------------------------------------------------------------------

using namespace CEGUI;

MyCEGUIRenderer		*myrenderer = NULL;
MyCEGUILogger		*mylogger = NULL;
EventPusher			*myeventpusher = NULL;
bool				OwnCEGUI = false;
my::dev::IProfiler	*Profiler = NULL;
my::u32				ProfileId;

//------------------------------------------------------------------------------------

bool create(const c8* cegui_ini_file_name, void* lua_scripter_ptr)
{
	if (!CEGUI::System::getSingletonPtr() && !myrenderer)
	{
		LOGGER.logInfo("Initialize CEGUI from file '%s'...", cegui_ini_file_name);

		LOGGER.increaseFormatLevel();

		// paths to the CEGUI files 
		core::stringc m_pathBase     = "../media/gui/";
		core::stringc m_pathFonts     = "fonts/";
		core::stringc m_pathImagesets = "imagesets/";
		core::stringc m_pathLayouts   = "layouts/";
		core::stringc m_pathLookNFeels= "looknfeel/";
		core::stringc m_pathSchemes   = "schemes/";
		core::stringc m_dirFonts     = "fonts/";
		core::stringc m_dirImagesets = "imagesets/";
		core::stringc m_dirLayouts   = "layouts/";
		core::stringc m_dirLookNFeels= "looknfeel/";
		core::stringc m_dirSchemes   = "schemes/";

		core::stringc m_pathCommonBase     = "../common.media/gui/";
		core::stringc m_pathCommonFonts     = "fonts/";
		core::stringc m_pathCommonImagesets = "imagesets/";
		core::stringc m_pathCommonLayouts   = "layouts/";
		core::stringc m_pathCommonLookNFeels= "looknfeel/";
		core::stringc m_pathCommonSchemes   = "schemes/";
		core::stringc m_dirCommonFonts     = "fonts/";
		core::stringc m_dirCommonImagesets = "imagesets/";
		core::stringc m_dirCommonLayouts   = "layouts/";
		core::stringc m_dirCommonLookNFeels= "looknfeel/";
		core::stringc m_dirCommonSchemes   = "schemes/";

		core::stringc _logFileName = "CEGUI.log";

		// The handy ini file helper class.
		CIniFile  m_iniFile;

		// Check if the INI file has proper settings
		m_iniFile.Path(std::string(cegui_ini_file_name));
		if (!m_iniFile.ReadFile())
		{   
			// Warn user and set defaults to use
			LOGGER.logWarn("The file '%s' could not be found; default settings will apply now.",
				cegui_ini_file_name);
		}
		else
		{
			std::string  value;

			// Load path information here 

			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_BASE);
			if (value != "")
			{
				m_pathBase = value.c_str();
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_FONTS);
			if (value != "")
			{
				m_dirFonts = value.c_str();
				m_pathFonts.sprintf("%s%s", m_pathBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_IMAGESETS);
			if (value != "")
			{
				m_dirImagesets = value.c_str();
				m_pathImagesets.sprintf("%s%s", m_pathBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_LAYOUTS);
			if (value != "")
			{
				m_dirLayouts = value.c_str();
				m_pathLayouts.sprintf("%s%s", m_pathBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_LOOKNFEELS);
			if (value != "")
			{
				m_dirLookNFeels = value.c_str();
				m_pathLookNFeels.sprintf("%s%s", m_pathBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_SCHEMES);
			if (value != "")
			{
				m_dirSchemes = value.c_str();
				m_pathSchemes.sprintf("%s%s", m_pathBase.c_str(), value.c_str());
			}
			// Commmon
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_BASE);
			if (value != "")
			{
				m_pathCommonBase = value.c_str();
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_FONTS);
			if (value != "")
			{
				m_dirCommonFonts = value.c_str();
				m_pathCommonFonts.sprintf("%s%s", m_pathCommonBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_IMAGESETS);
			if (value != "")
			{
				m_dirCommonImagesets = value.c_str();
				m_pathCommonImagesets.sprintf("%s%s", m_pathCommonBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_LAYOUTS);
			if (value != "")
			{
				m_dirCommonLayouts = value.c_str();
				m_pathCommonLayouts.sprintf("%s%s", m_pathCommonBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_LOOKNFEELS);
			if (value != "")
			{
				m_dirCommonLookNFeels = value.c_str();
				m_pathCommonLookNFeels.sprintf("%s%s", m_pathCommonBase.c_str(), value.c_str());
			}
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_SCHEMES);
			if (value != "")
			{
				m_dirCommonSchemes = value.c_str();
				m_pathCommonSchemes.sprintf("%s%s", m_pathCommonBase.c_str(), value.c_str());
			} 
			value = m_iniFile.GetValue(MY_CEGUI_INI_SECTION, "LogFileName");
			if (value != "")
			{
				_logFileName = value.c_str();
				_logFileName.sprintf("%s", value.c_str());
			}
		}

		_logFileName = core::stringc().sprintf("%s%s.%s",
			core::extractFileName(_logFileName, true),
#if MY_DEBUG_MODE
			"_d",
#else
			"",
#endif
			core::extractFileExt(_logFileName)
			);

		mylogger = new CEGUI::MyCEGUILogger();
		mylogger->setLogFilename(_logFileName.c_str());

		// create a renderer which uses the MyEngine filesystem and renderer
		// to load and render CEGUI system
		myrenderer = new CEGUI::MyCEGUIRenderer;

		//  create the CEGUI System
		try
		{
			CEGUI::System::create(*myrenderer);
		}
		catch (Exception &e)
		{
			LOGGER.logErr("%s:%d %s",
				e.getFileName().c_str(), e.getLine(), e.getMessage().c_str());
			destroy();

			LOGGER.decreaseFormatLevel();

			return false;
		}

		myrenderer->initializeLuaScripter(lua_scripter_ptr);       
          
		// getting resource provider pointer

		CEGUI::DefaultResourceProvider* rp = 
			static_cast<CEGUI::DefaultResourceProvider*>(
				CEGUI::System::getSingleton().getResourceProvider());

		// setup CEGUI data directories

		rp->setDefaultResourceGroup("");

		CEGUI::Font::setDefaultResourceGroup(MY_CEGUI_PATH_FONTS);
		CEGUI::Imageset::setDefaultResourceGroup(MY_CEGUI_PATH_IMAGESETS);
		CEGUI::WindowManager::setDefaultResourceGroup(MY_CEGUI_PATH_LAYOUTS);
		CEGUI::WidgetLookManager::setDefaultResourceGroup(MY_CEGUI_PATH_LOOKNFEELS);
		CEGUI::Scheme::setDefaultResourceGroup(MY_CEGUI_PATH_SCHEMES);

		// Calling these methods means that you can load resources without specifying the full path to them. CEGUI
		// will look for the resources in the given paths.   	

		rp->setResourceGroupDirectory(MY_CEGUI_PATH_FONTS,		(const c8*)m_pathFonts.c_str()     );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_IMAGESETS,	(const c8*)m_pathImagesets.c_str() );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_LAYOUTS,	(const c8*)m_pathLayouts.c_str()   );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_LOOKNFEELS,	(const c8*)m_pathLookNFeels.c_str());
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_SCHEMES,	(const c8*)m_pathSchemes.c_str()   );

		rp->setResourceGroupDirectory(MY_CEGUI_PATH_COMMON_FONTS,		(const c8*)m_pathCommonFonts.c_str()     );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_COMMON_IMAGESETS,	(const c8*)m_pathCommonImagesets.c_str() );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_COMMON_LAYOUTS,		(const c8*)m_pathCommonLayouts.c_str()   );
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_COMMON_LOOKNFEELS,	(const c8*)m_pathCommonLookNFeels.c_str());
		rp->setResourceGroupDirectory(MY_CEGUI_PATH_COMMON_SCHEMES,		(const c8*)m_pathCommonSchemes.c_str()   );

		// Save all current settings.
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_BASE,       (const char*)m_pathBase.c_str()     );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_FONTS,      (const char*)m_dirFonts.c_str()     );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_IMAGESETS,  (const char*)m_dirImagesets.c_str() );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_LAYOUTS,    (const char*)m_dirLayouts.c_str()   );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_LOOKNFEELS, (const char*)m_dirLookNFeels.c_str());
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_SCHEMES,    (const char*)m_dirSchemes.c_str()   );

		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_BASE,       (const char*)m_pathCommonBase.c_str()     );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_FONTS,      (const char*)m_dirCommonFonts.c_str()     );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_IMAGESETS,  (const char*)m_dirCommonImagesets.c_str() );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_LAYOUTS,    (const char*)m_dirCommonLayouts.c_str()   );
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_LOOKNFEELS, (const char*)m_dirCommonLookNFeels.c_str());
		m_iniFile.SetValue(MY_CEGUI_INI_SECTION, MY_CEGUI_PATH_COMMON_SCHEMES,    (const char*)m_dirCommonSchemes.c_str()   );
			
		// Save into the same path as the Load
		m_iniFile.WriteFile();

		LOGGER.logInfo("CEGUI initialised OK.");

		LOGGER.decreaseFormatLevel();

		OwnCEGUI = true;
	}
	else
	{
		OwnCEGUI = false;
	}

	if (!myeventpusher)
	{
		myeventpusher = new EventPusher;
	}

	if (!Profiler)
	{
		Profiler = PROFILER_PTR;
		Profiler->grab();

		ProfileId = Profiler->allocProfileId("Register CEGUI");
	}

	return true;
}

//----------------------------------------------------------------------------

bool onEvent(const io::SEvent& event)
{
	return myeventpusher->OnEvent(event);
}

//----------------------------------------------------------------------------

void registerForRendering()
{
	static u32 cur_sys_time_ms = 0;
	static u32 cur_sys_time_sec = 0;

	Profiler->startProfiling(ProfileId);

	cur_sys_time_ms = myrenderer->getSystemTime();
    cur_sys_time_sec = cur_sys_time_ms / 1000;

	static u32 last_sys_time_sec = cur_sys_time_sec;

    u32 time_elapsed_sec = cur_sys_time_sec - last_sys_time_sec;

	CEGUI::System::getSingleton().injectTimePulse(time_elapsed_sec);
	CEGUI::System::getSingleton().renderGUI();

	last_sys_time_sec = cur_sys_time_sec;

	Profiler->stopProfiling(ProfileId);
}

//----------------------------------------------------------------------------

void destroy()
{
	if (OwnCEGUI && CEGUI::System::getSingletonPtr())
	{
		CEGUI::System::destroy();
	}

	if (myeventpusher)
	{
		delete myeventpusher;
		myeventpusher = NULL;  
	}

	if (myrenderer)
	{
		delete myrenderer;   
		myrenderer = NULL;  
	}

	if (mylogger)
	{
		delete mylogger;
		mylogger= NULL;
	}

	SAFE_DROP(Profiler);
}

//----------------------------------------------------------------------------

void setWindowTextUtf8(CEGUI::Window *win, const c8* utf8_text)
{
	if (!win || !utf8_text)
		return;
	win->setText((CEGUI::utf8*)utf8_text);
}

//----------------------------------------------------------------------------

void setListboxItemTextUtf8(CEGUI::ListboxItem *item, const c8* utf8_text)
{
	if (!item || !utf8_text)
		return;
	item->setText((CEGUI::utf8*)utf8_text);
}

//----------------------------------------------------------------------------

vid::ITexture* getTexture(CEGUI::Texture &ceguitex)
{
	if (!(&ceguitex))
		return NULL;
	return ((MyCEGUITexture*)&ceguitex)->getTexture();
}

//----------------------------------------------------------------------------

void setTexture(CEGUI::Texture &ceguitex, vid::ITexture *mytex)
{
	if (!(&ceguitex))
		return;
	(*(MyCEGUITexture*)&ceguitex).setTexture(mytex);
}

//----------------------------------------------------------------------------
} // end namespace MyCEGUI
//---------------------------------------------------------------------------- 