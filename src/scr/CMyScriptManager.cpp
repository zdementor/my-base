//|-------------------------------------------------------------------------
//| File:        CMyScriptManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMyScriptManager.h"

#include "dev/IDevice.h"
#include "io/ILogger.h"

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

CMyScriptManager::CMyScriptManager()
{
#if MY_DEBUG_MODE  
	setClassName("CMyScriptManager");   	
#endif

	LOGGER.log("Lua script activating. . . ");

	m_CoreScripter = ICoreScriptManager::getSingletonPtr();

	m_LuaState = (lua_State*)getCoreScripterPtr();

	core::stringc scr_dir = DEVICE.getStartupDir();
	scr_dir.append("/");
	
	if (m_CoreScripter)
		m_CoreScripter->setScriptsFullDir(scr_dir.c_str());

	memset(&m_ScriptCallbacks, 0, sizeof(m_ScriptCallbacks));

	LOGGER.log(" Lua activated.");
}

//----------------------------------------------------------------------------

CMyScriptManager::~CMyScriptManager()
{
	clearScriptMemory();

	for (u32 i = 0; i < ESCT_SCRIPT_CALLBACK_TYPE_COUNT; i++)
		SAFE_DELETE(m_ScriptCallbacks[i]);

	LOGGER.log("Lua script deactivated.");
} 

//----------------------------------------------------------------------------

inline bool CMyScriptManager::_luaLookup(SScriptFunctor* functor)
{
	if (!functor || functor->FunctionName.size()==0)
		return false;

	if (functor->NeedLookup)
	{
		// retrieve function by name
		lua_getglobal(m_LuaState, functor->FunctionName.c_str());

		// reference function
		functor->FunctionIndex = luaL_ref(m_LuaState, LUA_REGISTRYINDEX);   

		functor->NeedLookup = false;
	}

	// retrieve function by its index
	lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, functor->FunctionIndex);

	return true;
}

//----------------------------------------------------------------------------

inline bool CMyScriptManager::_luaCall(SScriptFunctor *functor, s32 nargs)
{
	s32 nresults = 1;

	// call it
    int error = lua_pcall(m_LuaState, nargs, nresults, 0);
    if (error)
    {
		LOGGER.logErr("Unable to call Lua function '%s': %s", 
			functor->FunctionName.c_str(), lua_tostring(m_LuaState, -1));
        lua_pop(m_LuaState, nresults);
		return false;
    }

	// retrieve result
	if (lua_isboolean(m_LuaState, -1))
	{
		functor->ResultBool = lua_toboolean(m_LuaState, -1) ? 1 : 0;
		functor->ResultInt = functor->ResultBool ? 1 : 0;
		functor->ResultFloat = functor->ResultBool ? 1.f : 0.f;
		functor->ResultString = "";
		
	}
	else if (lua_isnumber(m_LuaState, -1))
	{
		functor->ResultFloat = lua_tonumber(m_LuaState, -1);
		functor->ResultInt = (s32)functor->ResultFloat;
		functor->ResultBool = functor->ResultInt != 0;
		functor->ResultString = "";
	}
	else if (lua_isstring(m_LuaState, -1))
	{
		functor->ResultBool = false;
		functor->ResultInt = 0;
		functor->ResultFloat = 0.f;
		functor->ResultString = lua_tostring(m_LuaState, -1);
	}
	else
	{
		functor->ResultBool = false;
		functor->ResultInt = 0;
		functor->ResultFloat = 0.f;
		functor->ResultString = "";
	}
	lua_pop(m_LuaState, nresults);

	return true;
}

//----------------------------------------------------------------------------

bool CMyScriptManager::runScriptText(const c8* script_text)
{
	if (!m_LuaState)
		return false;

	static c8 ch[255];

	// execute lua script

    int status;

	//Load and compile our lua text
	status = luaL_loadbuffer(m_LuaState, script_text, strlen(script_text), "Lua Script text");

	if(status)
    {
        sprintf(ch, "failed to compile script text %s", script_text);
		LOGGER.log(ch, io::ELL_ERROR);

		sprintf(ch, "%s", lua_tostring(m_LuaState,-1));
		LOGGER.log(ch, io::ELL_ERROR);

        return false;
    }

	//Run the 'main' function
    status = lua_pcall(m_LuaState, 0, 0, 0);
    if(status)
    {		
		sprintf(ch, "failed to run script text %s", script_text);
		LOGGER.log(ch, io::ELL_ERROR);

        sprintf(ch, "%s", lua_tostring(m_LuaState, -1));
		LOGGER.log(ch, io::ELL_ERROR);

        return false;
    }

	return true;
}

//----------------------------------------------------------------------------

void CMyScriptManager::setScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 *func)
{
	if (m_ScriptCallbacks[type] &&
			m_ScriptCallbacks[type]->FunctionName == func)
		return;
	SAFE_DELETE(m_ScriptCallbacks[type]);
	if (func)
		m_ScriptCallbacks[type] = new scr::SScriptFunctor(func);
}

//----------------------------------------------------------------------------

__MY_EXPORT__ IScriptManager* createMyScriptManager()
{
	return new CMyScriptManager();
}

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------