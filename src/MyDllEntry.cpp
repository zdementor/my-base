//|-------------------------------------------------------------------------
//| File:        MyDllEntry.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifdef __MY_BUILD_MYCEGUI_LIB__
#   include "inc/my_cegui.h"
#endif

#include <my_inc.h>
#include <ISingleton.h>
#include <io/ILogger.h>
#include <io/IFileSystem.h>
#include <mm/ISoundDriver.h>
#include <scr/IScriptManager.h>
#include <dyn/IDynamicManager.h>
#include <scn/ISceneManager.h>
#include <dev/IDevice.h>
#include <os/ITimer.h>
#include <os/ITimer.h>
#include <io/IInputDispatcher.h>
#include <img/IImageLibrary.h>
#include <game/IGameManager.h>
#include <game/IGameAIModule.h>
#include <game/IGameEventsDispatcher.h>
#include <game/IGameTasksManager.h>
#include <scn/IPathFinder.h>
#include <res/IResourceManager.h>
#include <dev/IPluginManager.h>
#include <scn/IMaterialsManager.h>

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

class CSingletonsTableInitializator
{
public:
    CSingletonsTableInitializator()
    {
		core::initSingletonsTable(
            &ISingleton <dev::IDevice>				::ms_Singleton,
            &ISingleton <dev::IPluginManager>		::ms_Singleton,
			&ISingleton <dev::IProfiler>			::ms_Singleton,
            &ISingleton <io::ILogger>				::ms_Singleton,
            &ISingleton <io::IFileSystem>			::ms_Singleton,
            &ISingleton <io::IInputDispatcher>		::ms_Singleton,
			&ISingleton <io::ICursorControl>		::ms_Singleton,
            &ISingleton <os::ITimerMain>			::ms_Singleton,
            &ISingleton <os::ITimerSecond>			::ms_Singleton,
            &ISingleton <os::IMemStatus>			::ms_Singleton,
            &ISingleton <mm::ISoundDriver>			::ms_Singleton,
			&ISingleton <scr::ICoreScriptManager>	::ms_Singleton,
            &ISingleton <scr::IScriptManager>		::ms_Singleton,			
            &ISingleton <dyn::IDynamicManager>		::ms_Singleton,
            &ISingleton <scn::ISceneManager>		::ms_Singleton,
            &ISingleton <scn::IPathFinderManager>	::ms_Singleton,
            &ISingleton <scn::ILightGridManager>	::ms_Singleton,
			&ISingleton <scn::IMaterialsManager>	::ms_Singleton,
			&ISingleton <scn::ICullingSystem>		::ms_Singleton,
            &ISingleton <img::IImageLibrary>		::ms_Singleton,
            &ISingleton <vid::IVideoDriver>			::ms_Singleton,
            &ISingleton <game::IGameManager>		::ms_Singleton,
			&ISingleton <game::IGameAIModule>		::ms_Singleton,
			&ISingleton <game::IGameEventsDispatcher>::ms_Singleton,
			&ISingleton <game::IGameTasksManager>	::ms_Singleton,
			&ISingleton <res::IResourceManager>		::ms_Singleton);
    }
}
SingletonsTableInitializator;

//----------------------------------------------------------------------------
} // end namespace my
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

// Dll Entry Point
BOOL APIENTRY DllMain( 
    HANDLE hModule, DWORD  ulReasonForCall, LPVOID lpReserved) 
{
    switch( ulReasonForCall )
    {
    case DLL_PROCESS_ATTACH:
        // Allocate memory here, if you need.
        break;
    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:
        // Not used.
        break;
   case DLL_PROCESS_DETACH:
        // Clean-up allocated memory here.
        break;
    }   
    return TRUE;
}

//----------------------------------------------------------------------------