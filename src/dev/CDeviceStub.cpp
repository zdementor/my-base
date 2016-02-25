//|-------------------------------------------------------------------------
//| File:        CDeviceStub.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CDeviceStub.h"

#include <scn/ISceneManager.h>
#include <io/IFileSystem.h>
#include <io/IEventReceiver.h>
#include <game/IGameManager.h>
#include <scn/ICameraSceneNode.h>
#include <os/ICriticalSection.h>
#include <io/IWriteFile.h>
#include <io/ICursorControl.h>
#include <os/ITimer.h>
#include <os/IMemStat.h>
#include <io/ILogger.h>
#include <os/IEvent.h>
#include <dev/IPluginManager.h>
#include <os/IThread.h>
#include <res/IResourceManager.h>
#include <scr/IScriptManager.h>
#include <scn/IMaterialsManager.h>  
#include <dyn/IDynamicManager.h>
#include <mm/ISoundDriver.h>

#include "CProfiler.h"

#include "../MyVersion.h"

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

//! constructor
CDeviceStub::CDeviceStub(
	io::IEventReceiver* resv, SDeviceCreationParameters params
	) : SceneManager(0),
VideoDriver(0),	FileSystem(0), GameManager(0), TimerMain(0), TimerSecond(0), 
Logger(0),	InputDispatcher(0), CriticalSection(0), FPSSynchroEvent(0), 
ImageLibrary(0), PluginManager(0), ResourceManager(0), 
IsDropping(false), FPSSynchronizationThread(0), DeviceSystemTime(0), DeviceTime(0),
CreationParams(params),
m_SigletonsCreated(false), m_MonitorBrightness(0.0f), m_Visible(false),
m_ConsoleReseiver(this)
{
	os::createMemStatus();

#if MY_DEBUG_MODE 
	IDevice::setClassName("CDeviceStub IDevice");
	IRunnable::setClassName("CDeviceStub IRunnable");
#endif

	UserReceiver = resv;

	// create logger
	Logger = LOGGER_PTR;
	if (!Logger)
		Logger = io::createLogger(UserReceiver);
	else
		Logger->grab();

	m_Profiler = new CProfiler;

	core::stringc ver = getVersionString();
	m_DescrString = "";
	m_DescrString.sprintf("MyEngine v.%s -\n3D game engine created by\n", ver.c_str() );
	m_DescrString.append (" Zhuk Dmitry (ZDimitor aka zdementor),\n");
	m_DescrString.append (" zdimitor@gmail.com, zdimitor@pochta.ru\n");
	m_DescrString.append (" Krasnoyarsk-Moscow 2004-2012.\n");
	m_DescrString.append ("Sources based on:\n");
	m_DescrString.append (" Irrlicht-v.0.6 (http://irrlicht.sourceforge.net),\n");
	m_DescrString.append (" ODE (http://ode.org),\n");
	m_DescrString.append (" CEGUI (http://www.cegui.org.uk),\n");
	m_DescrString.append (" Cal3D (http://cal3d.sourceforge.net),\n");
	m_DescrString.append (" FreeType2 (ftp://ftp.freetype.org),\n");
	m_DescrString.append (" ogg-vorbis (http://www.xiph.org/),\n");
	m_DescrString.append (" OpenAL (http://wwww.openal.org/),\n");
	m_DescrString.append (" Lua (http://www.lua.org/),\n");
	m_DescrString.append (" Tolua++ (http://www.codenix.com/~tolua/),\n");
	m_DescrString.append (" Libjpeg (ftp://ftp.uu.net/graphics/jpeg/),\n");
	m_DescrString.append (" Libpng (http://libpng.sourceforge.net),\n");
	m_DescrString.append (" MiniLib (http://www.stereofx.org/ terrain.html),\n");
	m_DescrString.append (" zlib (http://www.zlib.org)");

    // create filesystem
	FileSystem = FILE_SYSTEM_PTR;
	if (!FileSystem)
		FileSystem = io::createFileSystem();
	else
		FileSystem->grab();

	StartupDirectory = FileSystem->getWorkingDirectory();

	Logger->setEventReceiver(&m_ConsoleReseiver);

	LOGGER.logInfo("Startup Directory: %s", StartupDirectory.c_str());

	LOGGER.logInfo("-------------------------------------------------------");
    LOGGER.logInfo(m_DescrString.c_str());
	LOGGER.logInfo("-------------------------------------------------------");

	LOGGER.logInfo("Creating MyDevice . . .");
    
	bool swrast = getDeviceFlagValue(EDCF_SOFTWARE_RASTERIZER);

	vid::E_DRIVER_TYPE     driverType    = CreationParams.DriverType;
	core::dimension2d<s32> windowSize    = CreationParams.WindowSize;
	u32                    bits          = CreationParams.Bits;
	vid::E_TEXTURE_FILTER  textureFilter = CreationParams.TextureFilter;

	if (swrast)
	{
		CreationParams.Flags &= ~(EDCF_USE_SHADERS);
		CreationParams.Flags &= ~(EDCF_SHADOWS);
		CreationParams.Flags &= ~(EDCF_ANTIALIASING);
	}

	if (!getDeviceFlagValue(EDCF_USE_SHADERS))
	{
		CreationParams.Flags &= ~(EDCF_SHADERS_HIGH_QUALITY);
		CreationParams.Flags &= ~(EDCF_CACHE_SHADERS);
	}

	if (!getDeviceFlagValue(EDCF_USE_SHADERS))
		CreationParams.Flags |= EDCF_USE_FFP;

	bool fullscreen		= getDeviceFlagValue(EDCF_FULLSCREEN);
	bool antialiasing	= getDeviceFlagValue(EDCF_ANTIALIASING);
	bool verticalsync	= getDeviceFlagValue(EDCF_VERTICAL_SYNC);
	bool shadows		= getDeviceFlagValue(EDCF_SHADOWS);
	bool lighting_medim	= getDeviceFlagValue(EDCF_LIGHTING_MEDIUM_QUALITY);
	bool lighting_high	= getDeviceFlagValue(EDCF_LIGHTING_HIGH_QUALITY);
	bool perpixlighting	= getDeviceFlagValue(EDCF_SHADERS_HIGH_QUALITY);
	bool hianimqual		= getDeviceFlagValue(EDCF_HI_ANIMATION_QUALITY);
	bool shaders		= getDeviceFlagValue(EDCF_USE_SHADERS);	
	bool ffp			= getDeviceFlagValue(EDCF_USE_FFP);	
	bool dirt			= getDeviceFlagValue(EDCF_DIRT_ON_LEVEL);

	core::stringc msg;
	
	msg.sprintf(
		" MyDevice options:\n"
		"  Video driver type : %s\n"
		"  Window size       : %d x %d\n"
		"  Color bits        : %d\n"
		"  Texture filtering : %s\n"
		"  Fullscreen        : %s\n"
		"  Antialiasing      : %s\n"
		"  Vertical sync.    : %s\n"
		"  Shadows           : %s\n"
		"  Lighting quality  : %s\n"
		"  Per pixel lighting: %s\n"
		"  Smooth animation  : %s\n"
		"  Shader effects    : %s\n"
		"  Fixed pipeline    : %s\n"
		"  Dirt on level     : %s\n"
		"  Hardware accel.   : %s"		
		,
		vid::DriverTypeReadableName[CreationParams.DriverType],
		CreationParams.WindowSize.Width, CreationParams.WindowSize.Height,
		CreationParams.Bits,
		vid::TextureFilterReadableName[CreationParams.TextureFilter],
		fullscreen?"true":"false",
		antialiasing?"true":"false",
		verticalsync?"true":"false",
		shadows?"true":"false",
		lighting_high?"high":(lighting_medim?"medium":"low"),
		perpixlighting?"true":"false",
		hianimqual?"true":"false",
		shaders?"true":"false",
		ffp?"true":"false",
		dirt?"true":"false",
		swrast?"false":"true");

	LOGGER.log(msg.c_str());

    // create timer
    TimerMain   = os::createMainTimer();
	TimerSecond = os::createSecondTimer();

	// creating plugin manager
	PluginManager = createPluginManager();	

    // create image library
    ImageLibrary = img::createImageLibrary();    

	// creating device synchronization events 
	FPSSynchroEvent  = os::createEvent(true);

	ScriptManager = scr::createMyScriptManager();

	// creating resource manager
	ResourceManager = res::createResourceManager();	

	// creating thread for fps speed breake
	FPSSynchronizationThread = os::createTimerThread(this, 100);
	FPSSynchronizationThread->run();

	setMinimalRunPeriod(0);

	m_Profiler->startProfiling();
}

//----------------------------------------------------------------------------

CDeviceStub::~CDeviceStub()
{
	IsDropping = true;

	m_Profiler->stopProfiling();

	LOGGER.setLogLevel(io::ELL_INFORMATION);

	LOGGER.log("Starting My Engine cleanup . . .");

	Logger->increaseFormatLevel();

	GameManager->stopGame();
	
	SAFE_CALL(FPSSynchronizationThread, stop());
	SAFE_DROP(FPSSynchronizationThread);

	SAFE_DROP(PluginManager);
	DYNAMIC_MANAGER.drop();
	SOUND_DRIVER.drop();
	SAFE_DROP(GameManager);
	SAFE_DROP(SceneManager);	
	SAFE_DROP(ScriptManager);
	SAFE_DROP(VideoDriver);	
    SAFE_DROP(FileSystem);		
	SAFE_DROP(InputDispatcher);
    SAFE_DROP(TimerMain);
	SAFE_DROP(TimerSecond);

	SAFE_DROP(FPSSynchroEvent);
	SAFE_DROP(ImageLibrary);		
	SAFE_DROP(ResourceManager);	

	os::restoreMonitorSettings();

	Logger->decreaseFormatLevel();

	LOGGER.log("Cleanup completed");

	Logger->setEventReceiver(NULL);
	SAFE_DROP(Logger);

	m_Profiler->drop();

	MEM_STATUS.drop();
}

//---------------------------------------------------------------------------

bool CDeviceStub::run()
{
	// to fps speed break
	if (m_MinimalRunPeriod != 0)
		FPSSynchroEvent->wait();
	FPSSynchroEvent->reset();
	
	DeviceTime       = TIMER.getTime();
	DeviceSystemTime = TIMER.getSystemTime();

	m_Profiler->stopProfiling();
	m_Profiler->startProfiling();

	return true;
}

//---------------------------------------------------------------------------

void CDeviceStub::run(void* user_data)
{
	FPSSynchroEvent->set();			
}

//----------------------------------------------------------------------------

void CDeviceStub::createSingletons()
{
	// creating sound driver	
	bool sound = DEVICE.getDeviceFlagValue(dev::EDCF_SOUND_ON);
	mm::createSoundDriver(!sound ? mm::ESDT_NULL : mm::ESDT_OPENAL);

    // create Scene manager
    SceneManager = scn::createSceneManager();

	// set user event recevier
    setEventReceiver(UserReceiver);

	// creating dynamic manager
    dyn::createDynamicManager();

	// create game manager
	GameManager = game::createGameManager();

	//LOGGER.log("Sinletons:");
	//LOGGER.log(core::getSingletonsTableAsPrintableString());

	m_SigletonsCreated = true;
}

//----------------------------------------------------------------------------

bool CDeviceStub::setMonitorBrightness(f32 brightness)
{
	m_MonitorBrightness = brightness;
	bool ret = false;
	// never overbright in windowed mode
	if (!ExposedDeviceData.Video.FullScreen) 
	{
		LOGGER.log( "Can't change brightness in window mode");
		return ret;
	}
	ret = os::setMonitorBrightness(brightness, ExposedDeviceData.Video.Bits);
	if (ret)
		m_MonitorBrightness = os::getMonitorBrightness();
	return ret;
}	

//----------------------------------------------------------------------------

bool CDeviceStub::switchMonitorToFullScreen()
{
	return os::switchMonitorToFullScreen(
		ExposedDeviceData.Video.WindowSize.Width,
		ExposedDeviceData.Video.WindowSize.Height,
		ExposedDeviceData.Video.Bits);
}

//----------------------------------------------------------------------------

void CDeviceStub::_postEventFromUser(const io::SEvent &event)
{
    bool adsorbed = false;

	if (!m_SigletonsCreated)
		return;

	// handle event by user defined event receiver
    if (UserReceiver)
        adsorbed = UserReceiver->OnEvent(event);  

	// script on event callback
	if (!adsorbed && SCRIPT_MANAGER_PTR)
	{
		bool res = false;
		if (SCRIPT_MANAGER.runScriptCallback(
				scr::ESCT_ON_IO_EVENT, &res, (void*)&event))
			adsorbed = res;
	}

	// pass event to camera controllers
	scn::ICameraSceneNode *camera = NULL;
	if (!adsorbed && SCENE_MANAGER_PTR && (
			camera = SCENE_MANAGER.getActiveCamera()))
		adsorbed = camera->getController().OnEvent(event);
}

//----------------------------------------------------------------------------

//! Sets a new event receiver to receive events
void CDeviceStub::setEventReceiver(io::IEventReceiver* receiver)
{
	UserReceiver = receiver;
}

//----------------------------------------------------------------------------

const c8* CDeviceStub::getVersionString()
{
	return MY_ENGINE_VERSION_STRING;
}

//----------------------------------------------------------------------------

void CDeviceStub::setMinimalRunPeriod(u32 msec)
{
	m_MinimalRunPeriod = msec;
	if (msec == 0)
		msec = 100; // don't wand to be loaded by synchro thread
	FPSSynchronizationThread->setRunPeriod(msec);
}

//----------------------------------------------------------------------------

u32 CDeviceStub::getMinimalRunPeriod()
{
	return m_MinimalRunPeriod;
}

//----------------------------------------------------------------------------

void CDeviceStub::onScriptDestroy()
{
	
}

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace irr
//----------------------------------------------------------------------------
