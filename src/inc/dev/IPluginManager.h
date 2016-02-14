//|-------------------------------------------------------------------------
//| File:        IPluginManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IPluginManagerHPP
#define IPluginManagerHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

class __MY_DEV_LIB_API__ IPluginManager :
	public IUnknown, public ISingleton<IPluginManager>
{
public:

	//! registering external plugin, return true on success
	//! \param plugin_name: string name of the plugin
	//! \param plugin_start: plugin run function pointer
	//! \param plugin_shut:: plugin destroy function pointer
	virtual bool registerPlugin(
		c8* plugin_name, 
		void(*plugin_start)(), void(*plugin_shut)()
		) = 0;

	//! registering external plugin, return true on success
	//! \param plugin_name: string name of the plugin
	//! \param module_name: file name of the plugin (*.dll)
	virtual bool registerPlugin(
		c8* plugin_name, c8* module_name
		) = 0;

	//! unregistering external plugin, return true on success
	//! \param plugin_name: string name of the plugin, we want to unregister
	virtual void unregisterPlugin(c8* plugin_name) = 0;

	//! run external plugin, return true on success
	virtual bool startPlugin(c8* plugin_name) = 0;

	//! shut external plugin, return true on success
	virtual bool shutPlugin(c8* plugin_name) = 0;

	//! shut all external plugins
	virtual void shutAllPlugins() = 0;

    //! Return singleton object
    static IPluginManager& getSingleton();

    //! Return pointer to singleton object
    static IPluginManager* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_DEV_LIB_API__ IPluginManager* createPluginManager();

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif

