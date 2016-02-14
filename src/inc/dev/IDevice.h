//|-------------------------------------------------------------------------
//| File:        IDevice.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IDeviceHPP
#define IDeviceHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h> 
#include <dev/SExposedDeviceData.h>
#include <dev/SDeviceCreationParameters.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

// MyEngine device. You can create it with createDevice().
// This is the most important class of the MyEngine. You can access everything
//in the engine if you have a pointer to an instance of this class.
class __MY_DEV_LIB_API__ IDevice :
	public IUnknown, public ISingleton <IDevice>
{
public:

    //! destructor
    virtual ~IDevice() {};

	//!
	static bool isDebug() 
	{ 
#if MY_DEBUG_MODE
		return true;	
#endif
		return false;
	}

    //! Runs the device. Returns false if device wants to be deleted. Use it in this way:
    //! \code
    //! while(device->run())
    //! {
    //!     // draw everything here
    //! }
    //! \endcode
    virtual bool run() = 0;

    //! Sets the caption of the window.
    //! \param text: New text of the window caption.
    virtual void setWindowCaption ( const wchar_t* text ) = 0;
	virtual void setWindowCaption ( const c8* text ) = 0;

    //! \return Returns true if window is active. If the window is inactive,
    //! nothing need to be drawn. So if you don't want to draw anything when
    //! the window is inactive, create your drawing loop this way:
    //! \code
    //! while(device->run())
    //!     if (device->isWindowActive())
    //!     {
    //!         // draw everything here
    //!     }
    //! \endcode
    virtual bool isWindowActive() = 0;

	//! Show device main window
	virtual void show() = 0;

	//! Hide device main window
	virtual void hide() = 0;

    //! Notifies the device that it should close itself.
    //! IDevice::run() will always return false after close() was called.
    virtual void close() = 0;

    //! Returns the version of the engine. The returned string
    //! will look like this: "1.2.3" or this: "1.2". 
    virtual const c8* getVersionString() = 0;

    //! Sets a new event receiver to receive events.
    virtual void setEventReceiver(io::IEventReceiver* receiver) = 0;

	//! Notifies the device, that it has been resized
    virtual void OnResize() = 0;

    //! Sets if the window should be resizeable in windowed mode. The default
    //! is false. This method only works in windowed mode.
    virtual void setResizeAble(bool resize=false) = 0;

    //! return engine startup directory (full path)
    virtual core::stringc getStartupDir() = 0;

	//! return true if device is in dropping process
	virtual bool isDropping() = 0;

	//! return engine description string
	virtual const c8* getDescriptionString() = 0;

	//! getting current device time (ms)
	virtual u32 getDeviceTime() = 0;

	//! getting current device system time (ms)
	virtual u32 getDeviceSystemTime() = 0;

	//! return device flag value
	virtual bool getDeviceFlagValue(E_DEVICE_CREATION_FLAG flag) = 0;

	//! returning data, containing all information about it
	virtual const SExposedDeviceData& getExposedDeviceData() = 0;

	//! Set window icon
	virtual void setWindowIcon(const c8 *iconFileName, u32 width, u32 height) = 0;

	//! getting monitor brightness
	virtual f32 getMonitorBrightness() = 0;

	//! setting monitor brightness
	virtual bool setMonitorBrightness(f32 brightness) = 0;	

	//! switchs monitor to fullscreen
	virtual bool switchMonitorToFullScreen() = 0;

	//! restore monotor settings (brighteness/resolution) to initial values
	virtual void restoreMonitorSettings() = 0;

	virtual void setMinimalRunPeriod(u32 msec) = 0;
	virtual u32 getMinimalRunPeriod() = 0;

	virtual void onScriptDestroy() = 0;

	//!	Return singleton object
	static IDevice& getSingleton();

	//! Return pointer to singleton object
	static IDevice* getSingletonPtr();
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------
#endif

