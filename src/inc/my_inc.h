//|-------------------------------------------------------------------------
//| File:        my_inc.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_incHPP
#define my_incHPP
//----------------------------------------------------------------------------

// main header

// forward declarations of commonly used classes
#include "ForwardDecl.h"

//----------------------------------------------------------------------------

// compile config
#include "../CompileConf.h" 

//-----------------------------------------------------------------------------

// Miscellaneous helper macro

#define DEVICE						my::dev::IDevice::getSingleton()
#define PLUGIN_MANAGER				my::dev::IPluginManager::getSingleton()
#define PROFILER					my::dev::IProfiler::getSingleton()
#define LOGGER						my::io::ILogger::getSingleton()
#define FILE_SYSTEM					my::io::IFileSystem::getSingleton()
#define INPUT_DISPATCHER			my::io::IInputDispatcher::getSingleton()
#define CURSOR_CONTROL				my::io::ICursorControl::getSingleton()
#define TIMER						my::os::ITimerMain::getSingleton()
#define TIMER_SECOND				my::os::ITimerSecond::getSingleton()
#define SOUND_DRIVER				my::mm::ISoundDriver::getSingleton()
#define CORE_SCRIPT_MANAGER			my::scr::ICoreScriptManager::getSingleton()
#define SCRIPT_MANAGER				my::scr::IScriptManager::getSingleton()
#define DYNAMIC_MANAGER				my::dyn::IDynamicManager::getSingleton()
#define SCENE_MANAGER				my::scn::ISceneManager::getSingleton()
#define PATH_FINDER_MANAGER			my::scn::IPathFinderManager::getSingleton()
#define LIGHT_GRID_MANAGER			my::scn::ILightGridManager::getSingleton()
#define MATERIALS_MANAGER			my::scn::IMaterialsManager::getSingleton()
#define CULLING_SYSTEM				my::scn::ICullingSystem::getSingleton()
#define IMAGE_LIBRARY				my::img::IImageLibrary::getSingleton()
#define VIDEO_DRIVER				my::vid::IVideoDriver::getSingleton()
#define GAME_MANAGER				my::game::IGameManager::getSingleton()
#define GAME_AI_MODULE				my::game::IGameAIModule::getSingleton()
#define GAME_EVENTS_DISPATCHER		my::game::IGameEventsDispatcher::getSingleton()
#define GAME_TASK_MANAGER			my::game::IGameTasksManager::getSingleton()
#define RESOURCE_MANAGER			my::res::IResourceManager::getSingleton()

#define DEVICE_PTR					my::dev::IDevice::getSingletonPtr()
#define PLUGIN_MANAGER_PTR			my::dev::IPluginManager::getSingletonPtr()
#define PROFILER_PTR				my::dev::IProfiler::getSingletonPtr()
#define LOGGER_PTR					my::io::ILogger::getSingletonPtr()
#define FILE_SYSTEM_PTR				my::io::IFileSystem::getSingletonPtr()
#define INPUT_DISPATCHER_PTR		my::io::IInputDispatcher::getSingletonPtr()
#define CURSOR_CONTROL_PTR			my::io::ICursorControl::getSingletonPtr()
#define TIMER_PTR					my::os::ITimerMain::getSingletonPtr()
#define TIMER_SECOND_PTR			my::os::ITimerSecond::getSingletonPtr()
#define SOUND_DRIVER_PTR			my::mm::ISoundDriver::getSingletonPtr()
#define CORE_SCRIPT_MANAGER_PTR		my::scr::ICoreScriptManager::getSingletonPtr()
#define SCRIPT_MANAGER_PTR			my::scr::IScriptManager::getSingletonPtr()
#define DYNAMIC_MANAGER_PTR			my::dyn::IDynamicManager::getSingletonPtr()
#define SCENE_MANAGER_PTR			my::scn::ISceneManager::getSingletonPtr()
#define PATH_FINDER_MANAGER_PTR		my::scn::IPathFinderManager::getSingletonPtr()
#define LIGHT_GRID_MANAGER_PTR		my::scn::ILightGridManager::getSingletonPtr()
#define MATERIALS_MANAGER_PTR		my::scn::IMaterialsManager::getSingletonPtr()
#define CULLING_SYSTEM_PTR			my::scn::ICullingSystem::getSingletonPtr()
#define IMAGE_LIBRARY_PTR			my::img::IImageLibrary::getSingletonPtr()
#define VIDEO_DRIVER_PTR			my::vid::IVideoDriver::getSingletonPtr()
#define GAME_MANAGER_PTR			my::game::IGameManager::getSingletonPtr()
#define GAME_AI_MODULE_PTR			my::game::IGameAIModule::getSingletonPtr()
#define GAME_EVENTS_DISPATCHER_PTR	my::game::IGameEventsDispatcher::getSingletonPtr()
#define GAME_TASK_MANAGER_PTR		my::game::IGameTasksManager::getSingletonPtr()
#define RESOURCE_MANAGER_PTR		my::res::IResourceManager::getSingletonPtr()

#define SAFE_DELETE(p)             do { if (p) { delete (p);     (p)=NULL; } } while (0)
#define SAFE_DELETE_ARRAY(p)       do { if (p) { delete[] (p);   (p)=NULL; } } while (0)
#define SAFE_RELEASE(p)            do { if (p) { (p)->Release(); (p)=NULL; } } while (0)
#define SAFE_DROP(p)               do { if (p) { (p)->drop();    (p)=NULL; } } while (0)
#define SAFE_GRAB(p)               do { if (p) { (p)->grab(); } } while (0)
#define SAFE_CALL(p,f)             do { if (p) { (p)->f; } } while (0)

#define SAFE_RETURN_CONST_PTR(str_init, maxlen)	\
	core::stringc str = str_init;				\
	static c8 buffer[maxlen];					\
	s32 size = str.size()+1;					\
	strncpy(buffer, maxlen, str.c_str());		\
    return buffer; 

#define CHECK_RANGE(v, rmin, rmax) do { if (v<rmin) v=rmin; else if (v>rmax) v=rmax; } while (0)
#define CHECK_MIN_RANGE(v, rmin)   do { if (v<rmin) v=rmin; } while (0)
#define CHECK_MAX_RANGE(v, rmax)   do { if (v>rmax) v=rmax; } while (0)
#define CHECK_NO_ZERO(v)           do { if (v==0) v=0.0001; } while (0)

#define BIT_VALUE(v, bit)          ((v&bit)?1:0)

//----------------------------------------------------------------------------

// Some constants

#define AVG_TIME_VALUES_COUNT 25

//----------------------------------------------------------------------------
// Initial platform/compiler-related stuff to set.

#define MY_PLATFORM_WIN32 1
#define MY_PLATFORM_LINUX 2
#define MY_PLATFORM_APPLE 3

#define MY_COMPILER_MSVC 1
#define MY_COMPILER_GNUC 2
#define MY_COMPILER_BORL 3

//----------------------------------------------------------------------------
// Finds the compiler type and version.

#if defined( _MSC_VER )
#   define MY_COMPILER MY_COMPILER_MSVC
#   define MY_COMP_VER _MSC_VER

#elif defined( __GNUC__ )
#   define MY_COMPILER MY_COMPILER_GNUC
#   define MY_COMP_VER (((__GNUC__)*100)+__GNUC_MINOR__)

#elif defined( __BORLANDC__ )
#   define MY_COMPILER MY_COMPILER_BORL
#   define MY_COMP_VER __BCPLUSPLUS__

#else
#   error "No known compiler. Abort! Abort!"

#endif

//----------------------------------------------------------------------------
// Finds the current CPU architecture

#define MY_X32 1
#define MY_X64 2

#define __i386__

#if defined __i386__
#	define MY_CPU_ARCH MY_X32
#elif defined __amd64__
#	define MY_CPU_ARCH MY_X64
#else
#	error "Unsuppurted target CPU. Abort! Abort!"
#endif

//----------------------------------------------------------------------------
// Finds the current platform 

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define MY_PLATFORM MY_PLATFORM_WIN32

#elif defined( __APPLE_CC__)
#   define MY_PLATFORM MY_PLATFORM_APPLE

#else
#   define MY_PLATFORM MY_PLATFORM_LINUX
#endif

//----------------------------------------------------------------------------
// include needed libraries

#if MY_PLATFORM == MY_PLATFORM_WIN32    
#	include <wchar.h>
#	include <windows.h>
#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32  

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <typeinfo.h>
#include <io.h>
#include <memory.h>

//----------------------------------------------------------------------------
// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define MY_QUOTE_INPLACE(x) # x
#define MY_QUOTE(x) MY_QUOTE_INPLACE(x)
#define MY_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

//----------------------------------------------------------------------------
// Windows Settings
#if MY_PLATFORM == MY_PLATFORM_WIN32

//----------------------------------------------------------------------------

// Export/Import API

#	define __MY_IMPORT__ __declspec(dllimport)
#	define __MY_EXPORT__ __declspec(dllexport)

// for my dev module
#   ifdef __MY_USE_DEV_LIB__           // use library from client
#       define __MY_DEV_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_DEV_LIB__  // building library
#       define __MY_DEV_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_DEV_LIB_API__ 
#   endif

// for my base module
#   ifdef __MY_USE_SCN_LIB__           // use library from client
#       define __MY_SCN_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_SCN_LIB__  // building library
#       define __MY_SCN_LIB_API__   __MY_EXPORT__
#   else                                // some other case (buid static library for example)
#       define __MY_SCN_LIB_API__ 
#   endif

// for my core module
#   ifdef __MY_USE_CORE_LIB__           // use library from client
#       define __MY_CORE_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_CORE_LIB__  // building library
#       define __MY_CORE_LIB_API__   __MY_EXPORT__
#   else                                // some other case (buid static library for example)
#       define __MY_CORE_LIB_API__ 
#   endif

// for my os module
#   ifdef __MY_USE_OS_LIB__           // use library from client
#       define __MY_OS_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_OS_LIB__  // building library
#       define __MY_OS_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_OS_LIB_API__ 
#   endif

// for my io module
#   ifdef __MY_USE_IO_LIB__           // use library from client
#       define __MY_IO_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_IO_LIB__  // building library
#       define __MY_IO_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_IO_LIB_API__ 
#   endif

// for my img module
#   ifdef __MY_USE_IMG_LIB__           // use library from client
#       define __MY_IMG_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_IMG_LIB__ // building library
#       define __MY_IMG_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_IMG_LIB_API__ 
#   endif

// for my vid modules (ogl, d3d8, d3d9)
#   ifdef __MY_USE_VID_LIB__           // use library from client
#       define __MY_VID_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_VID_LIB__  // building library
#       define __MY_VID_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_VID_LIB_API__ 
#   endif

// for my scr module
#   ifdef __MY_USE_SCR_LIB__           // use library from client
#       define __MY_SCR_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_SCR_LIB__  // building library
#       define __MY_SCR_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_SCR_LIB_API__ 
#   endif

// for my mm module
#   ifdef __MY_USE_MM_LIB__           // use library from client
#       define __MY_MM_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_MM_LIB__  // building library
#       define __MY_MM_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_MM_LIB_API__ 
#   endif

// for my dyn module
#   ifdef __MY_USE_DYN_LIB__           // use library from client
#       define __MY_DYN_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_DYN_LIB__  // building library
#       define __MY_DYN_LIB_API__   __MY_EXPORT__
#   else                              // some other case (buid static library for example)
#       define __MY_DYN_LIB_API__ 
#   endif

// for my game module
#   ifdef __MY_USE_GAME_LIB__           // use library from client
#       define __MY_GAME_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_GAME_LIB__  // building library
#       define __MY_GAME_LIB_API__   __MY_EXPORT__
#   else                               // some other case (buid static library for example)
#       define __MY_GAME_LIB_API__ 
#   endif

// for my scn module
#   ifdef __MY_USE_SCN_LIB__           // use library from client
#       define __MY_SCN_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_SCN_LIB__  // building library
#       define __MY_SCN_LIB_API__   __MY_EXPORT__
#   else                               // some other case (buid static library for example)
#       define __MY_SCN_LIB_API__ 
#   endif

// for my res module
#   ifdef __MY_USE_RES_LIB__           // use library from client
#       define __MY_RES_LIB_API__   __MY_IMPORT__
#   elif defined __MY_BUILD_RES_LIB__  // building library
#       define __MY_RES_LIB_API__   __MY_EXPORT__
#   else                               // some other case (buid static library for example)
#       define __MY_RES_LIB_API__ 
#   endif

// for my wrap module
#   ifdef __MY_USE_WRAP_LIB__           // use library from client
#       define __MY_WRAP_LIB_API__   extern "C" __MY_IMPORT__
#   elif defined __MY_BUILD_WRAP_LIB__  // building library
#       define __MY_WRAP_LIB_API__   extern "C" __MY_EXPORT__
#   else                               // some other case (buid static library for example)
#       define __MY_WRAP_LIB_API__ 
#   endif

// for robogame module
#   ifdef __MY_USE_ROBOGAME_LIB__           // use library from client
#       define __MY_ROBOGAME_LIB_API__   extern "C" __MY_IMPORT__
#   elif defined __MY_BUILD_ROBOGAME_LIB__  // building library
#       define __MY_ROBOGAME_LIB_API__   extern "C" __MY_EXPORT__
#   else                               // some other case (buid static library for example)
#       define __MY_ROBOGAME_LIB_API__ 
#   endif

// Win32 compilers use _DEBUG for specifying debug builds.
#   ifdef _DEBUG
#       define MY_DEBUG_MODE 1
#   else
#       define MY_DEBUG_MODE 0
#   endif

#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32

//----------------------------------------------------------------------------
// Linux/Apple Settings
#if MY_PLATFORM == MY_PLATFORM_LINUX || MY_PLATFORM == MY_PLATFORM_APPLE

// Linux compilers don't have symbol import/export directives.
#   define __MY_SCN_LIB_API__

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
#   ifdef DEBUG
#       define MY_DEBUG_MODE 1
#   else
#       define MY_DEBUG_MODE 0
#   endif

#endif // #if MY_PLATFORM == MY_PLATFORM_LINUX || MY_PLATFORM == MY_PLATFORM_APPLE

//----------------------------------------------------------------------------
// Compiler settings

#if MY_COMPILER == MY_COMPILER_MSVC

#   if MY_DEBUG_MODE       
#       pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#       pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
#       pragma comment(linker, "/NODEFAULTLIB:msvcrt.lib")
#       pragma comment(linker, "/NODEFAULTLIB:libcd.lib ")
#       pragma comment(linker, "/NODEFAULTLIB:libcmtd.lib")       
#   else
#       pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#       pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
#       pragma comment(linker, "/NODEFAULTLIB:libcd.lib")
#       pragma comment(linker, "/NODEFAULTLIB:libcmtd.lib ")
#       pragma comment(linker, "/NODEFAULTLIB:msvcrtd.lib")
#   endif    

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4503)

// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception 
// specifications in the ANSI C++ draft.
#   pragma warning( disable : 4290 )

// disable: "no suitable definition provided for explicit template 
// instantiation request" Occurs in VC7 for no justifiable reason on all 
// #includes of Singleton
#   pragma warning( disable: 4661)

// disable: deprecation warnings when using CRT calls in VC8 
// These show up on all C runtime lib code in VC8, disable since they clutter
// the warnings with things we may not be able to do anything about (e.g. 
// generated code from nvparse etc). I doubt very much that these calls
// will ever be actually removed from VC anyway, it would break too much code.
#   pragma warning( disable: 4996)

// disable: "no definition for inline function"
#   pragma warning( disable: 4506)

// disable: warning 'this' : used in base member initializer list
#   pragma warning( disable: 4355)

#   undef _DEFINE_DEPRECATED_HASH_CLASSES
#   if MY_COMP_VER > 1300
#       define _DEFINE_DEPRECATED_HASH_CLASSES 0
#   else
#      define _DEFINE_DEPRECATED_HASH_CLASSES 1
#   endif

// See if we can use __forceinline or if we need to use __inline instead 
#   if MY_COMP_VER >= 1200
#       define MY_FORCEINLINE	__forceinline
#       define MY_INLINE		__inline
#	else
#		define MY_FORCEINLINE	__inline
#       define MY_INLINE		__inline
#   endif

#   define   swprintf   _snwprintf

#else 

#   if MY_COMPILER == MY_COMPILER_BORL

		inline float sqrtf( float x )
		{return (float)sqrt((double)x);}

		inline int floorf( float x )
		{return (int)floor((double)x);}

		inline int ceilf( float x )
		{return (int)ceil((double)x);}

#   endif

#	define MY_FORCEINLINE	__inline
#	define MY_INLINE		__inline

#endif

#if MY_DEBUG_MODE       
#	define MY_ASSERT(exp) assert(exp)
#else
#	define MY_ASSERT(exp) (void)0;
#endif    

//----------------------------------------------------------------------------
// core types, templates library

#include "core/core.h"

//----------------------------------------------------------------------------
// some constants

#define NONAME_SCENENODE_NAME      "noname scene node"
#define NONAME_GAMESCENE_FILE_NAME "noname game scene"
#define NONAME_FILE_NAME           "null"
#define NONAME                     "noname"

#include <dev/ConstDevice.h>
#include <dyn/ConstDynamic.h>
#include <scn/ConstScene.h>
#include <game/ConstGame.h>
#include <os/ConstOS.h>
#include <vid/ConstVideo.h>
#include <io/ConstInOut.h>
#include <res/ConstResources.h>
#include <scr/ConstScript.h>
#include <mm/ConstMultimed.h>
#include <img/ConstImgLib.h>

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

//! Enumeration for all index types there are.
enum E_INDEX_TYPE
{
	//! 16 bit index type
    EIT_16_BIT = 0, 

    //! 32 bit index type
    EIT_32_BIT,

	E_INDEX_TYPE_COUNT,

	E_INDEX_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! index types as string
static const c8* IndexTypeStr[E_INDEX_TYPE_COUNT] = 
{
	"EIT_16_BIT",
	"EIT_32_BIT"
};

//----------------------------------------------------------------------------
} // end namespace my
//----------------------------------------------------------------------------

// most important implementations

#include <core/CoreDynamic.h>
#include <core/CoreGame.h>
#include <core/CoreInOut.h>
#include <core/CoreScene.h>
#include <core/CoreVideo.h>

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

// struct for holding all singleton objects pointers
struct SSingletonsTable
{
	SSingletonsTable()
	{
		memset(&Ptrs, 0, sizeof(SPtrs));
	}

	struct SPtrs
	{
		dev::IDevice				*Device;
		dev::IPluginManager			*PluginManager;
		dev::IProfiler				*Profiler;
		io::ILogger					*Logger;
		io::IFileSystem				*FileSystem;
		io::IInputDispatcher		*InputDispatcher;
		io::ICursorControl			*CursorControl;    
		os::ITimerMain				*TimerMain;
		os::ITimerSecond			*TimerSecond;
		mm::ISoundDriver			*SoundDriver;        
		scr::ICoreScriptManager		*CoreScriptManager;
		scr::IScriptManager			*ScriptManager;
		dyn::IDynamicManager		*DynamicManager;                      
		scn::ISceneManager			*SceneManager;
		scn::IPathFinderManager		*PathFinderManager;
		scn::ILightGridManager		*LightGridManager;
		scn::IMaterialsManager		*MaterialsManager;
		scn::ICullingSystem			*CullingSystem;
		img::IImageLibrary			*ImageLibrary;
		vid::IVideoDriver			*VideoDriver;
		game::IGameManager			*GameManager;
		game::IGameAIModule			*GameAIModule;
		game::IGameEventsDispatcher	*GameEventsDispatcher;
		game::IGameTasksManager		*GameTasksManager;  
		res::IResourceManager		*ResourceManager;
	}
	Ptrs;
};

//----------------------------------------------------------------------------

//! return global singleton table
__MY_CORE_LIB_API__ const SSingletonsTable& getSingletonsTable();

//----------------------------------------------------------------------------

//! return a printable formatted string with global singleton table
__MY_CORE_LIB_API__ const c8* getSingletonsTableAsPrintableString();

//----------------------------------------------------------------------------

//! singletons table initialization
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
	res::IResourceManager		***ResourceManager_ms_Singleton);

//----------------------------------------------------------------------------

//! unique IDs types
enum E_UNIQUE_ID_TYPE
{
	EUID_SCENE_NODE_ID = 0,
	EUID_UNIT_FORMATION_ID,

	EUID_UNIQUE_ID_TYPE_COUNT
};

//----------------------------------------------------------------------------

//! add new ID to the ID-Factory
__MY_CORE_LIB_API__ bool addUniqueID(E_UNIQUE_ID_TYPE uid_type, s32 id);

//! remove ID from the ID-Factory
__MY_CORE_LIB_API__ bool removeUniqueID(E_UNIQUE_ID_TYPE uid_type, s32 id);

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------- 

#endif // my_incHPP
