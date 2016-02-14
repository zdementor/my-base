//|-------------------------------------------------------------------------
//| File:        Core.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! global table of the singleton objects pointers
SSingletonsTable g_SingletonsTable;

static stringc _PrintableString;

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ const SSingletonsTable& getSingletonsTable()
{ return g_SingletonsTable; }

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ const c8* getSingletonsTableAsPrintableString()
{
	_PrintableString.sprintf(
		"&Device                = %p, Device                = %p\n"
		"&PluginManager         = %p, PluginManager         = %p\n"
		"&Profiler              = %p, Profiler              = %p\n"
		"&Logger                = %p, Logger                = %p\n"
		"&FileSystem            = %p, FileSystem            = %p\n"
		"&InputDispatcher       = %p, InputDispatcher       = %p\n"
		"&CursorControl         = %p, CursorControl         = %p\n"
		"&TimerMain             = %p, TimerMain             = %p\n"
		"&TimerSecond           = %p, TimerSecond           = %p\n"
		"&SoundDriver           = %p, SoundDriver           = %p\n"
		"&CoreScriptManager     = %p, CoreScriptManager     = %p\n"
		"&ScriptManager         = %p, ScriptManager         = %p\n"
		"&DynamicManager        = %p, DynamicManager        = %p\n"
		"&SceneManager          = %p, SceneManager          = %p\n"
		"&PathFinderManager     = %p, PathFinderManager     = %p\n"
		"&LightGridManager      = %p, LightGridManager      = %p\n"
		"&MaterialsManager      = %p, MaterialsManager      = %p\n"
		"&CullingSystem         = %p, CullingSystem         = %p\n"
		"&ImageLibrary          = %p, ImageLibrary          = %p\n"
		"&VideoDriver           = %p, VideoDriver           = %p\n"
		"&GameManager           = %p, GameManager           = %p\n"
		"&GameAIModule          = %p, GameAIModule          = %p\n"
		"&GameEventsDispatcher  = %p, GameEventsDispatcher  = %p\n"
		"&GameTasksManager      = %p, GameTasksManager      = %p\n"
		"&ResourceManager       = %p, ResourceManager       = %p",
		&g_SingletonsTable.Ptrs.Device,					g_SingletonsTable.Ptrs.Device,
		&g_SingletonsTable.Ptrs.PluginManager,			g_SingletonsTable.Ptrs.PluginManager,
		&g_SingletonsTable.Ptrs.Profiler,				g_SingletonsTable.Ptrs.Profiler,
		&g_SingletonsTable.Ptrs.Logger,					g_SingletonsTable.Ptrs.Logger,
		&g_SingletonsTable.Ptrs.FileSystem,				g_SingletonsTable.Ptrs.FileSystem,
		&g_SingletonsTable.Ptrs.InputDispatcher,		g_SingletonsTable.Ptrs.InputDispatcher,
		&g_SingletonsTable.Ptrs.CursorControl,			g_SingletonsTable.Ptrs.CursorControl,
		&g_SingletonsTable.Ptrs.TimerMain,				g_SingletonsTable.Ptrs.TimerMain,
		&g_SingletonsTable.Ptrs.TimerSecond,			g_SingletonsTable.Ptrs.TimerSecond,
		&g_SingletonsTable.Ptrs.SoundDriver,			g_SingletonsTable.Ptrs.SoundDriver,
		&g_SingletonsTable.Ptrs.CoreScriptManager,		g_SingletonsTable.Ptrs.CoreScriptManager,
		&g_SingletonsTable.Ptrs.ScriptManager,			g_SingletonsTable.Ptrs.ScriptManager,
		&g_SingletonsTable.Ptrs.DynamicManager,			g_SingletonsTable.Ptrs.DynamicManager,
		&g_SingletonsTable.Ptrs.SceneManager,			g_SingletonsTable.Ptrs.SceneManager,
		&g_SingletonsTable.Ptrs.PathFinderManager,		g_SingletonsTable.Ptrs.PathFinderManager,
		&g_SingletonsTable.Ptrs.LightGridManager,		g_SingletonsTable.Ptrs.LightGridManager,
		&g_SingletonsTable.Ptrs.MaterialsManager,		g_SingletonsTable.Ptrs.MaterialsManager,
		&g_SingletonsTable.Ptrs.CullingSystem,			g_SingletonsTable.Ptrs.CullingSystem,
		&g_SingletonsTable.Ptrs.ImageLibrary,			g_SingletonsTable.Ptrs.ImageLibrary,
		&g_SingletonsTable.Ptrs.VideoDriver,			g_SingletonsTable.Ptrs.VideoDriver,
		&g_SingletonsTable.Ptrs.GameManager,			g_SingletonsTable.Ptrs.GameManager,
		&g_SingletonsTable.Ptrs.GameAIModule,			g_SingletonsTable.Ptrs.GameAIModule,
		&g_SingletonsTable.Ptrs.GameEventsDispatcher,	g_SingletonsTable.Ptrs.GameEventsDispatcher,
		&g_SingletonsTable.Ptrs.GameTasksManager,		g_SingletonsTable.Ptrs.GameTasksManager,
		&g_SingletonsTable.Ptrs.ResourceManager,		g_SingletonsTable.Ptrs.ResourceManager);
	return _PrintableString.c_str();
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void initSingletonsTable(
	dev::IDevice				***Device_ms_Singleton,
	dev::IPluginManager			***PluginManager_ms_Singleton,
	dev::IProfiler				***Profiler_ms_Singleton,
	io::ILogger					***Logger_ms_Singleton,
	io::IFileSystem				***FileSystem_ms_Singleton,
	io::IInputDispatcher		***InputDispatcher_ms_Singleton,
	io::ICursorControl			***CursorControl_ms_Singleton,
	os::ITimerMain				***TimerMain_ms_Singleton,
	os::ITimerSecond			***TimerSecond_ms_Singleton,
	mm::ISoundDriver			***SoundDriver_ms_Singleton,
	scr::ICoreScriptManager		***CoreScriptManager_ms_Singleton,
	scr::IScriptManager			***ScriptManager_ms_Singleton,
	dyn::IDynamicManager		***DynamicManager_ms_Singleton,
	scn::ISceneManager			***SceneManager_ms_Singleton,
	scn::IPathFinderManager		***PathFinderManager_ms_Singleton,
	scn::ILightGridManager		***LightGridManager_ms_Singleton,
	scn::IMaterialsManager		***MaterialsManager_ms_Singleton,
	scn::ICullingSystem			***CullingSystem_ms_Singleton,
	img::IImageLibrary			***ImageLibrary_ms_Singleton,
	vid::IVideoDriver			***VideoDriver_ms_Singleton,
	game::IGameManager			***GameManager_ms_Singleton,
	game::IGameAIModule			***GameAIModule_ms_Singleton,
	game::IGameEventsDispatcher	***GameEventsDispatcher_ms_Singleton,
	game::IGameTasksManager		***GameTasksManager_ms_Singleton,
	res::IResourceManager		***ResourceManager_ms_Singleton)
{
	*Device_ms_Singleton				= &g_SingletonsTable.Ptrs.Device;	
	*PluginManager_ms_Singleton			= &g_SingletonsTable.Ptrs.PluginManager;
	*Profiler_ms_Singleton				= &g_SingletonsTable.Ptrs.Profiler;
	*Logger_ms_Singleton				= &g_SingletonsTable.Ptrs.Logger;
	*FileSystem_ms_Singleton			= &g_SingletonsTable.Ptrs.FileSystem;
	*InputDispatcher_ms_Singleton		= &g_SingletonsTable.Ptrs.InputDispatcher;
	*CursorControl_ms_Singleton			= &g_SingletonsTable.Ptrs.CursorControl;
	*TimerMain_ms_Singleton				= &g_SingletonsTable.Ptrs.TimerMain;
	*TimerSecond_ms_Singleton			= &g_SingletonsTable.Ptrs.TimerSecond;
	*SoundDriver_ms_Singleton			= &g_SingletonsTable.Ptrs.SoundDriver;
	*CoreScriptManager_ms_Singleton		= &g_SingletonsTable.Ptrs.CoreScriptManager;
	*ScriptManager_ms_Singleton			= &g_SingletonsTable.Ptrs.ScriptManager;
	*DynamicManager_ms_Singleton		= &g_SingletonsTable.Ptrs.DynamicManager;
	*SceneManager_ms_Singleton			= &g_SingletonsTable.Ptrs.SceneManager;
	*PathFinderManager_ms_Singleton		= &g_SingletonsTable.Ptrs.PathFinderManager;	
	*LightGridManager_ms_Singleton		= &g_SingletonsTable.Ptrs.LightGridManager;	
	*MaterialsManager_ms_Singleton		= &g_SingletonsTable.Ptrs.MaterialsManager;	
	*CullingSystem_ms_Singleton			= &g_SingletonsTable.Ptrs.CullingSystem;	
	*ImageLibrary_ms_Singleton			= &g_SingletonsTable.Ptrs.ImageLibrary;
	*VideoDriver_ms_Singleton			= &g_SingletonsTable.Ptrs.VideoDriver;
	*GameManager_ms_Singleton			= &g_SingletonsTable.Ptrs.GameManager;
	*GameAIModule_ms_Singleton			= &g_SingletonsTable.Ptrs.GameAIModule;
	*GameEventsDispatcher_ms_Singleton	= &g_SingletonsTable.Ptrs.GameEventsDispatcher;
	*GameTasksManager_ms_Singleton		= &g_SingletonsTable.Ptrs.GameTasksManager;
	*ResourceManager_ms_Singleton		= &g_SingletonsTable.Ptrs.ResourceManager;
}

//-----------------------------------------------------------------------------

//! used nodes identificators in the ID-Factory
static core::array<s32> UsedNodesIDs[EUID_UNIQUE_ID_TYPE_COUNT];  

//-----------------------------------------------------------------------------

//! making sure that new Unique ID is unique in the ID-Factory
bool testUniqueID(E_UNIQUE_ID_TYPE uid_type, s32 id)
{
	u32 i;

	// making sure that new id is unique
	for (i=0; i<UsedNodesIDs[uid_type].size(); i++)
	{
		if (UsedNodesIDs[uid_type][i] == id)
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

//! add new Unique ID to the ID-Factory
__MY_CORE_LIB_API__ bool addUniqueID(E_UNIQUE_ID_TYPE uid_type, s32 id)
{		
	if (id == -1)
		return true;

	if (testUniqueID( uid_type, id ))
	{
		// storing new identificator
		UsedNodesIDs[uid_type].push_back(id);

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

//! remove Unique ID from the ID-Factory
__MY_CORE_LIB_API__ bool removeUniqueID(E_UNIQUE_ID_TYPE uid_type, s32 id)
{
	if (id== -1)
		return false;

	for (u32 i=0; i<UsedNodesIDs[uid_type].size(); i++)
	{
		if (UsedNodesIDs[uid_type][i] == id)
		{
			UsedNodesIDs[uid_type].erase(i);

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

