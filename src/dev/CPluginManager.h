//|-------------------------------------------------------------------------
//| File:        CPluginManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CPluginManagerHPP
#define CPluginManagerHPP
//----------------------------------------------------------------------------

#include <dev/IPluginManager.h>
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

class CPluginManager : public IPluginManager
{
public:

    //! constructor
    CPluginManager();

    //! destructor
    virtual ~CPluginManager();

	//! registering external plugin, return true on success
	virtual bool registerPlugin(
		c8* plugin_name, 
		void(*plugin_start)(), void(*plugin_shut)()
		);

	//! registering external plugin, return true on success
	virtual bool registerPlugin(
		c8* plugin_name, c8* module_name
		);

	//! unregistering external plugin, return true on success
	virtual void unregisterPlugin(c8* plugin_name);

	//! run external plugin, return true on success
	virtual bool startPlugin(c8* plugin_name);

	//! shut external plugin, return true on success
	virtual bool shutPlugin(c8* plugin_name);

	//! shut all external plugins
	virtual void shutAllPlugins();

private:

	struct SPluginEntry
	{
		SPluginEntry() : LibraryModule(NULL), PluginName(""), PluginStart(NULL), PluginShut(NULL) {}
		
		SPluginEntry(
			c8* plugin_name, void(*plugin_start)(), void(*plugin_shut)()
			) : 
		LibraryModule(NULL), PluginName(plugin_name), 
		PluginStart(plugin_start),  PluginShut(plugin_shut) {}

		SPluginEntry(
			os::ILibraryModule *lib, c8* plugin_name
			) : 
		LibraryModule(lib), PluginName(plugin_name), PluginStart(NULL),  PluginShut(NULL) {}

		os::ILibraryModule *LibraryModule;
		core::stringc PluginName;
		void (*PluginStart)();
		void (*PluginShut)();
	};

	core::array<SPluginEntry> Plugins;
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif

