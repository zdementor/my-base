//|-------------------------------------------------------------------------
//| File:        CCoreScriptManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CCoreScriptManager.h"

#include <scr/IScriptManager.h>
#include <io/ILogger.h>
#include <dev/IDevice.h>

#ifndef TOLUA_STATIC
#	define TOLUA_STATIC
#endif
#include <tolua++.h>
#include <laddons.h>

#include <exception>

#include "MyLuaBind.h"

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

CCoreScriptManager::CCoreScriptManager()
	: m_MaxKBytesGC(16384)
{
#if MY_DEBUG_MODE  
	setClassName("CCoreScriptManager");  
#endif
	if (!LOGGER_PTR)
		io::createLogger(NULL);
	else
		LOGGER.grab();

	m_FullGameScriptsDir = "./";

	// starting lua		
	m_LuaState = lua_open();

	// init all standard libraries
#if LUA_VERSION_NUM < 501
	luaopen_base(m_LuaState);
	luaopen_io(m_LuaState);
	luaopen_string(m_LuaState);
	luaopen_table(m_LuaState);
	luaopen_math(m_LuaState);
#else
	luaL_openlibs(m_LuaState);
#endif
	luaopen_bit(m_LuaState);
	luainit_os_addon(m_LuaState);

	_initScript();

	setScriptMaxMemoryKBytes(m_MaxKBytesGC);

	LOGGER.logInfo("Core scripter (%s) created", LUA_VERSION);
}

//----------------------------------------------------------------------------

CCoreScriptManager::~CCoreScriptManager()
{
	// shutdoun lua
	if (m_LuaState)
		lua_close(m_LuaState);

	LOGGER.logInfo("Core scripter destroyed");

	LOGGER.drop();
}

//----------------------------------------------------------------------------

bool CCoreScriptManager::runScript(const c8* filename)
{
	if (!m_LuaState)
		return false;

	int status;
	core::stringc fullfilename = m_FullGameScriptsDir;
	fullfilename.append(filename);
	
	try
	{
		//Load and compile our lua file
		status = luaL_loadfile(m_LuaState, fullfilename.c_str());
		if(status)
		{
			LOGGER.logErr("Failed to compile script %s:\n%s",
				fullfilename.c_str(), lua_tostring(m_LuaState,-1));
			return false;
		}
	}
	catch (std::exception &e)
	{
		LOGGER.logErr("Failed to compile script %s:\n%s",
			fullfilename.c_str(), e.what());
		return false;
	}

	try
	{
		//Run the 'main' function
		status = lua_pcall(m_LuaState, 0, 0, 0);
		if(status)
		{
			LOGGER.logErr("Failed to run script %s:\n%s",
				fullfilename.c_str(), lua_tostring(m_LuaState,-1));
			return false;
		}
	}
	catch (std::exception &e)
	{
		LOGGER.logErr("Failed to run script %s:\n%s",
			fullfilename.c_str(), e.what());
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------

void CCoreScriptManager::clearScriptMemory()
{
	int oldMemoryUse = lua_getgccount(m_LuaState);

#if LUA_VERSION_NUM < 501
	lua_setgcthreshold(m_LuaState,0); // force the garbage collection
#else
	lua_gc(m_LuaState, LUA_GCCOLLECT, 0);
#endif
	
	int newMemoryUse = lua_getgccount(m_LuaState);

	LOGGER.logInfo("Script memory (old mem %d, new mem %d) has been cleared.", 
		oldMemoryUse, newMemoryUse);

#if LUA_VERSION_NUM < 501
	lua_setgcthreshold(m_LuaState, m_MaxKBytesGC); // restore defaults
#else
	lua_gc(m_LuaState, LUA_GCCOLLECT, m_MaxKBytesGC);
#endif
}

//----------------------------------------------------------------------------

u32 CCoreScriptManager::getScriptMemoryKBytes()
{
	return lua_getgccount(m_LuaState);
}

//----------------------------------------------------------------------------

void CCoreScriptManager::setScriptMaxMemoryKBytes(u32 max_kbytes)
{
#if LUA_VERSION_NUM < 501
	lua_setgcthreshold(m_LuaState, max_kbytes);
#endif
}

//----------------------------------------------------------------------------

void CCoreScriptManager::_initScript()
{
	// bind My Engine API
	tolua_MyLuaBind_open(m_LuaState);
}

//----------------------------------------------------------------------------

__MY_EXPORT__ ICoreScriptManager* createCoreScriptManager()
{
	return new CCoreScriptManager();
}

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------
