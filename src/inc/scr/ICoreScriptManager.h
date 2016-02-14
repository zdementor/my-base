//|-------------------------------------------------------------------------
//| File:        ICoreScriptManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2010 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ICoreScriptManagerHPP
#define ICoreScriptManagerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

//! Core Script Manager. Used as wrapper for script engine
class __MY_SCR_LIB_API__ ICoreScriptManagerInterface : public IUnknown
{
public:

	virtual bool runScript(const c8* filename) = 0;

	virtual void clearScriptMemory() = 0;

	virtual u32 getScriptMemoryKBytes() = 0;

	virtual void setScriptMaxMemoryKBytes(u32 max_kbytes) = 0;

	virtual void* getCoreScripterPtr() = 0;

	virtual void setScriptsFullDir(const c8* dir) = 0;
	virtual const c8* getScriptsFullDir() = 0;
};

//----------------------------------------------------------------------------

//! Core Script Manager. Used as wrapper for script engine
class __MY_SCR_LIB_API__ ICoreScriptManager :
	public ICoreScriptManagerInterface, public ISingleton <ICoreScriptManager>
{
public:

	//!	Return singleton object
	static ICoreScriptManager& getSingleton();

	//! Return pointer to singleton object
	static ICoreScriptManager* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_SCR_LIB_API__ ICoreScriptManager* createCoreScriptManager();

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IScriptManagerHPP