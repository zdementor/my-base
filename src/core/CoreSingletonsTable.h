//|-------------------------------------------------------------------------
//| File:        CoreSingletonsTable.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ProxySingletonsTableHPP
#define ProxySingletonsTableHPP
//----------------------------------------------------------------------------

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
#include <scn/IAnimatedMesh.h>
#include <img/SColor.h>
#include <scn/IPathFinderManager.h>
#include <res/IResourceManager.h>
#include <dev/IPluginManager.h>
#include <game/IGameNode.h>
#include <scn/ILightGridManager.h>
#include <game/IGameNodeAI.h>
#include <game/IGameUnitsFormation.h>
#include <scn/ILightSceneNode.h>
#include <scn/ISceneNode.h>
#include <vid/IVideoDriver.h>
#include <scn/ICullingSystem.h>

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! global table of the singleton objects pointers
extern SSingletonsTable g_SingletonsTable;;

//----------------------------------------------------------------------------
} // end namespace my
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef ProxySingletonsTableHPP