//|-------------------------------------------------------------------------
//| File:        CPluginManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CPluginManager.h"
#include <io/ILogger.h>
#include <os/ILibraryModule.h>

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

namespace os {

//! creating library module object
__MY_EXPORT__ os::ILibraryModule* createLibraryModule(c8 *module_name);

} // end namespace os

//----------------------------------------------------------------------------
namespace dev {
//----------------------------------------------------------------------------

//! constructor
CPluginManager::CPluginManager()  
{
#if MY_DEBUG_MODE 
    setClassName("CPluginManager");
#endif 
}

//----------------------------------------------------------------------------

CPluginManager::~CPluginManager()
{
	shutAllPlugins();

	for (u32 i=0; i<Plugins.size(); i++)
	{
		SAFE_DROP(Plugins[i].LibraryModule);
	}
	Plugins.clear();
}

//----------------------------------------------------------------------------

//! registering external plugin, return true on success
bool CPluginManager::registerPlugin(		
	c8* plugin_name, void(*plugin_start)(), void(*plugin_shut)())
{
	c8 msg[255];

	for (u32 i=0; i<Plugins.size(); i++)
	{
		if (Plugins[i].PluginName == core::stringc(plugin_name))
		{			
			sprintf(msg, "Plugin with such name '%s' is already existed", plugin_name);
			LOGGER.log(msg, io::ELL_WARNING);

			return false;
		}
	}

	Plugins.push_back(SPluginEntry(plugin_name, plugin_start, plugin_shut));

	sprintf(msg, "Plugin '%s' was successfully registered", plugin_name);
	LOGGER.log(msg);

	return true;
}

//----------------------------------------------------------------------------

//! registering external plugin, return true on success
bool CPluginManager::registerPlugin(
	c8* plugin_name, c8* module_name
	)
{
	core::stringc module_name_str = module_name;

	if (module_name_str.size() > 0)
	{
		c8 msg[255];

		for (u32 i=0; i<Plugins.size(); i++)
		{
			if (Plugins[i].PluginName == core::stringc(plugin_name))
			{			
				sprintf(msg, "Plugin with such name '%s' is already existed", plugin_name);
				LOGGER.log(msg, io::ELL_WARNING);

				return false;
			}
		}

		Plugins.push_back( SPluginEntry(os::createLibraryModule(module_name), plugin_name) );

		sprintf(msg, "Plugin '%s' was successfully registered", plugin_name);
		LOGGER.log(msg);

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

//! unregistering external plugin, return true on success
void CPluginManager::unregisterPlugin(c8* plugin_name)
{
	for (u32 i=0; i<Plugins.size();)
	{
		if (Plugins[i].PluginName == core::stringc(plugin_name))
		{	
			shutPlugin(plugin_name);
			SAFE_DROP(Plugins[i].LibraryModule);
			Plugins.erase(i);
		}
		else
		{
			i++;
		}
	}
}

//----------------------------------------------------------------------------

bool CPluginManager::startPlugin(c8* plugin_name)
{
	for (u32 i=0; i<Plugins.size(); i++)
	{
		if (Plugins[i].PluginName == core::stringc(plugin_name))
		{	
			if (!Plugins[i].PluginStart && Plugins[i].LibraryModule)
				Plugins[i].PluginStart = 
					(void (*)())(Plugins[i].LibraryModule->getProcAddress("startPlugin"));
			if (!Plugins[i].PluginShut && Plugins[i].LibraryModule)
				Plugins[i].PluginShut = 
					(void (*)())(Plugins[i].LibraryModule->getProcAddress("shutPlugin"));
			if (Plugins[i].PluginStart)
			// run plugin
			{
				Plugins[i].PluginStart();
				return true;
			}
		}
	}
	LOGGER.logErr("Can't start plugin '%s'", plugin_name);
	return false;
}

//----------------------------------------------------------------------------

bool CPluginManager::shutPlugin(c8* plugin_name)
{
	for (u32 i = 0; i < Plugins.size(); i++)
	{
		if (Plugins[i].PluginName == core::stringc(plugin_name) &&
				Plugins[i].PluginShut)
		{	
			// run plugin
			Plugins[i].PluginShut();
			return true;
		}
	}
	LOGGER.logWarn("Can't shut plugin '%s'", plugin_name);
	return false;
}

//----------------------------------------------------------------------------

//! shut all external plugins
void CPluginManager::shutAllPlugins()
{
	for (u32 i=0; i<Plugins.size(); i++)
	{
		if (Plugins[i].PluginShut)
		{	
			// shut plugin
			Plugins[i].PluginShut();
		}
	}
}

//----------------------------------------------------------------------------

IPluginManager *createPluginManager()
{
	return new CPluginManager;
}

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace irr
//----------------------------------------------------------------------------
