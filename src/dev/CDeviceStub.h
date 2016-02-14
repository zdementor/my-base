//|-------------------------------------------------------------------------
//| File:        CDeviceStub.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDeviceStubHPP
#define CDeviceStubHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <dev/IDevice.h>
#include <io/IInputDispatcher.h>
#include <img/IImageLibrary.h>
#include <IRunnable.h>
#include <os/os.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

//! Stub for an Irrlicht Device implementation
class CDeviceStub : public IDevice, public IRunnable
{
public:

    //! constructor
    CDeviceStub(io::IEventReceiver* resv, SDeviceCreationParameters params);

    //! destructor
    virtual ~CDeviceStub();

	//! IRunnable interface implementation

    virtual bool run();
	virtual void run(void* user_data);

	//! IDevice interface implementation

	virtual void show() { m_Visible = true; }
	virtual void hide() { m_Visible = false; }

    virtual const c8* getVersionString();

    virtual void setEventReceiver(io::IEventReceiver* receiver);
 
    virtual core::stringc getStartupDir()
	{ return StartupDirectory; }

	virtual bool isDropping()
	{ return IsDropping; }

	virtual const c8* getDescriptionString()
	{ return m_DescrString.c_str(); }

	virtual u32 getDeviceTime()
	{ return DeviceTime; }

	virtual u32 getDeviceSystemTime()
	{ return DeviceSystemTime; }

	virtual bool getDeviceFlagValue(E_DEVICE_CREATION_FLAG flag)
	{ return ((CreationParams.Flags & flag)==0 ? false : true); }

	virtual const SExposedDeviceData& getExposedDeviceData()
	{ return ExposedDeviceData; }

	virtual f32 getMonitorBrightness()
	{ return m_MonitorBrightness != 0.0f ? m_MonitorBrightness : os::getMonitorBrightness(); }

	virtual bool setMonitorBrightness(f32 brightness);	

	virtual bool switchMonitorToFullScreen();

	virtual void restoreMonitorSettings()
	{ return os::restoreMonitorSettings(); }

	virtual void setMinimalRunPeriod(u32 msec);
	virtual u32 getMinimalRunPeriod();

	virtual void onScriptDestroy();

	//! send the event to the right receiver
    void _postEventFromUser(const io::SEvent &event);

protected:

    void createSingletons();

    io::IFileSystem			*FileSystem;
    vid::IVideoDriver		*VideoDriver;
    scn::ISceneManager		*SceneManager;
    os::ITimer				*TimerMain;
	os::ITimer				*TimerSecond;
	io::IEventReceiver		*UserReceiver;
    io::ILogger				*Logger;        
	game::IGameManager		*GameManager;
	io::IInputDispatcher	*InputDispatcher;
	os::ICriticalSection	*CriticalSection;
	img::IImageLibrary		*ImageLibrary;
	IPluginManager			*PluginManager;
	res::IResourceManager	*ResourceManager;
	scr::IScriptManager		*ScriptManager;
	IProfiler				*m_Profiler;

	os::IEvent				*FPSSynchroEvent;

	class CConsoleReseiver : public io::IEventReceiver
	{
	public:
		CConsoleReseiver(CDeviceStub *device)
			: m_Device(device) {}
		~CConsoleReseiver() {}

		virtual bool OnEvent(const io::SEvent &event)
		{
			m_Device->_postEventFromUser(event);
			return true;
		}
	private:
		CDeviceStub *m_Device;
	}
	m_ConsoleReseiver;

    core::stringc StartupDirectory;

	bool IsDropping;

	os::IThread* FPSSynchronizationThread;

	u32 DeviceSystemTime, DeviceTime;

	SExposedDeviceData ExposedDeviceData;
	SDeviceCreationParameters CreationParams;

	scr::SScriptFunctor* m_ScriptCallbackOnEvent;

	core::stringc m_DescrString;

	bool m_SigletonsCreated;

	f32 m_MonitorBrightness;

	bool m_Visible;

	u32 m_MinimalRunPeriod;
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------

#endif

