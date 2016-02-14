//|-------------------------------------------------------------------------
//| File:        CLibraryModule.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CLibraryModule.h"
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! constructor
CLibraryModule::CLibraryModule(c8 *module_name)
	:
m_LibraryInstance(NULL), m_LibraryName(module_name), m_Loaded(false)
{
#if MY_DEBUG_MODE  
	setClassName("CLibraryModule");
#endif
}

//---------------------------------------------------------------------------

//! destructor
CLibraryModule::~CLibraryModule()
{
	unload();
}

//---------------------------------------------------------------------------

//! load dynamic library, attached to this object
bool CLibraryModule::load()
{
	m_Loaded = true;

	if(!m_LibraryInstance)
    {
		m_LibraryInstance = LoadLibrary( m_LibraryName.c_str() );

		if (!m_LibraryInstance)
		{
			core::stringc msg;
			msg.sprintf("Could not load %s library", m_LibraryName.c_str());
			LOGGER.log(msg.c_str(), io::ELL_ERROR);

			return false;
		}

		return true;
    }

	return false;
}

//---------------------------------------------------------------------------

//! unload dynamic library, attached to this object
bool CLibraryModule::unload()
{
	if(m_LibraryInstance)
    {

    }

	m_LibraryInstance = NULL;
	m_Loaded = false;

	return false;
}

//---------------------------------------------------------------------------

//! return address from dynamic labrary module
void* CLibraryModule::getProcAddress(c8 *proc_name)
{
	if (!m_Loaded)
		load();

	void *proc_addr = NULL;

	if(m_LibraryInstance)
    {
		proc_addr = GetProcAddress(m_LibraryInstance, proc_name);

		if (!proc_addr)
		{
			core::stringc msg;
			msg.sprintf(
				"Could not load '%s' procedure from %s library", 
				proc_name, m_LibraryName.c_str()
				);
			LOGGER.log(msg.c_str(), io::ELL_ERROR);
		}
    }

	return proc_addr;
}

//---------------------------------------------------------------------------

//! creating library module object
__MY_EXPORT__ os::ILibraryModule* createLibraryModule(c8 *module_name)
{
	return new os::CLibraryModule(module_name);
}

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------