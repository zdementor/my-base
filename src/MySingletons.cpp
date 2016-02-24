//|-------------------------------------------------------------------------
//| File:        MySingletons.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <ISingleton.h>
#include <io/ILogger.h>
#include <io/IFileSystem.h>
#include <mm/ISoundDriver.h>
#include <scr/IScriptManager.h>
#include <dyn/IDynamicManager.h>
#include <scn/ISceneManager.h>
#include <dev/IDevice.h>
#include <os/ITimer.h>
#include <io/IInputDispatcher.h>
#include <img/IImageLibrary.h>
#include <game/IGameManager.h>
#include <game/IGameAIModule.h>
#include <game/IGameEventsDispatcher.h>
#include <game/IGameTasksManager.h>
#include <scn/IPathFinderManager.h>
#include <res/IResourceManager.h>
#include <dev/IPluginManager.h>
#include <scn/ILightGridManager.h>
#include <scn/IMaterialsManager.h>
#include <scn/ICullingSystem.h>
#include <io/ICursorControl.h>
#include <dev/IProfiler.h>
#include <os/IMemStat.h>

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

#define MY_DEFINE_SINGLETON(n, t, tret) \
n::t** ISingleton <n::t>::ms_Singleton = NULL; \
namespace n { \
tret& t::getSingleton() \
{ return ISingleton <t>::getSingleton(); } \
tret* t::getSingletonPtr() \
{ return ISingleton <t>::getSingletonPtr(); } \
}

//----------------------------------------------------------------------------

// DEV - Singletons
MY_DEFINE_SINGLETON(dev, IDevice,			IDevice);
MY_DEFINE_SINGLETON(dev, IProfiler,			IProfiler);
MY_DEFINE_SINGLETON(dev, IPluginManager,	IPluginManager);

// IO - Singletons
MY_DEFINE_SINGLETON(io, ILogger,			ILogger);
MY_DEFINE_SINGLETON(io, IFileSystem,		IFileSystem);
MY_DEFINE_SINGLETON(io, IInputDispatcher,	IInputDispatcher);
MY_DEFINE_SINGLETON(io, ICursorControl,		ICursorControl);

// OS - Singletons
MY_DEFINE_SINGLETON(os, ITimerMain,		ITimer);
MY_DEFINE_SINGLETON(os, ITimerSecond,	ITimer);
MY_DEFINE_SINGLETON(os, IMemStatus,		IMemStatus);

// SND - Singletons
MY_DEFINE_SINGLETON(mm, ISoundDriver, ISoundDriver);

// SCR - Singletons
MY_DEFINE_SINGLETON(scr, IScriptManager,		IScriptManager);
MY_DEFINE_SINGLETON(scr, ICoreScriptManager,	ICoreScriptManager);

// DYN - Singletons
MY_DEFINE_SINGLETON(dyn, IDynamicManager, IDynamicManager);

// SCN - Singletons
MY_DEFINE_SINGLETON(scn, ISceneManager,			ISceneManager);
MY_DEFINE_SINGLETON(scn, IPathFinderManager,	IPathFinderManager);
MY_DEFINE_SINGLETON(scn, ILightGridManager,		ILightGridManager);
MY_DEFINE_SINGLETON(scn, IMaterialsManager,		IMaterialsManager);
MY_DEFINE_SINGLETON(scn, ICullingSystem,		ICullingSystem);

// IMG - Singletons
MY_DEFINE_SINGLETON(img, IImageLibrary, IImageLibrary);

// VID - Singletons
MY_DEFINE_SINGLETON(vid, IVideoDriver, IVideoDriver);

namespace vid {

E_VERTEX_TYPE S3DVertexSimple::Type				= EVT_SIMPLE;
E_VERTEX_TYPE S3DVertexSimpleColoured::Type		= EVT_SIMPLE_COLOURED;
E_VERTEX_TYPE S3DVertex1TCoords::Type			= EVT_1TCOORDS;
E_VERTEX_TYPE S3DVertex1TCoordsColoured::Type	= EVT_1TCOORDS_COLOURED;
E_VERTEX_TYPE S3DVertex2TCoords::Type			= EVT_2TCOORDS;
E_VERTEX_TYPE S3DVertex2TCoordsColoured::Type	= EVT_2TCOORDS_COLOURED;
E_VERTEX_TYPE S3DVertex1TCoordsTBN::Type		= EVT_1TCOORDS_TBN;
E_VERTEX_TYPE S3DVertex1TCoordsTBNColoured::Type= EVT_1TCOORDS_TBN_COLOURED;
E_VERTEX_TYPE S3DVertex2TCoordsTBN::Type		= EVT_2TCOORDS_TBN;
E_VERTEX_TYPE S3DVertex2TCoordsTBNColoured::Type= EVT_2TCOORDS_TBN_COLOURED;
E_VERTEX_TYPE S3DVertex1TCoordsRHWColoured::Type= EVT_1TCOORDS_RHW_COLOURED;

} // end namespace vid

// GAME - Singletons
MY_DEFINE_SINGLETON(game, IGameManager,				IGameManager);
MY_DEFINE_SINGLETON(game, IGameAIModule,			IGameAIModule);
MY_DEFINE_SINGLETON(game, IGameEventsDispatcher,	IGameEventsDispatcher);
MY_DEFINE_SINGLETON(game, IGameTasksManager,		IGameTasksManager);

// RES - Singletons
MY_DEFINE_SINGLETON(res, IResourceManager, IResourceManager);

//----------------------------------------------------------------------------
} // end namespace my
//----------------------------------------------------------------------------