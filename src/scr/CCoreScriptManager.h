//|-------------------------------------------------------------------------
//| File:        CCoreScriptManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCoreScriptManagerHPP
#define CCoreScriptManagerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <scr/ICoreScriptManager.h>

extern "C" {
	#include <lauxlib.h>
	#include <lualib.h>
	#include <lua.h>
}

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

//! Script Manager. Used as wrapper for script engine
class CCoreScriptManager : public ICoreScriptManager
{
public:

	CCoreScriptManager();
	virtual ~CCoreScriptManager();

	virtual bool runScript(const c8* filename);
	virtual void clearScriptMemory();
	virtual u32 getScriptMemoryKBytes();
	virtual void setScriptMaxMemoryKBytes(u32 max_kbytes);

	virtual void* getCoreScripterPtr()
	{ return (void*)m_LuaState; }

	virtual void setScriptsFullDir(const c8* dir)
	{ m_FullGameScriptsDir = dir; }

	virtual const c8* getScriptsFullDir()
	{ return m_FullGameScriptsDir.c_str(); }

protected:

	void _initScript();

	core::stringc m_FullGameScriptsDir;
	lua_State *m_LuaState; // The lua_State that this script module uses.

	u32 m_MaxKBytesGC;
};

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CCoreScriptManagerHPP 