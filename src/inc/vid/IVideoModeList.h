//|-------------------------------------------------------------------------
//| File:        IVideoModeList.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IVideoModeListHPP
#define IVideoModeListHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! A list of all available video modes.
//!  You can get a list via IDevice::getVideoModeList(). If you are confused 
//! now, because you think you have to create an Irrlicht Device with a video
//! mode before being able to get the video mode list, let me tell you that
//! there is no need to start up an Irrlicht Device with DT_DIRECTX9, DT_OPENGL or
//! DT_SOFTWARE: For this (and for lots of other reasons) the null device,
//! DT_NULL exists.
class __MY_VID_LIB_API__ IVideoModeList : public IUnknown
{
public:

    //! destructor
    virtual ~IVideoModeList() {}

    //! Gets amount of video modes in the list.
    //! \return Returns amount of video modes.
    virtual s32 getVideoModeCount() const = 0;

    //! Returns the screen size of a video mode in pixels.
    //! \param modeNumber: zero based index of the video mode.
    //! \return Returns size of screen in pixels of the specified video mode.
    virtual core::dimension2d<s32> getVideoModeResolution(s32 modeNumber) const = 0;

    //! Returns the pixel depth of a video mode in bits.
    //! \param modeNumber: zero based index of the video mode.
    //! \return Returns the size of each pixel of the specified video mode in bits.
    virtual s32 getVideoModeDepth(s32 modeNumber) const = 0;

    //! Returns current desktop screen resolution.
    //! \return Returns size of screen in pixels of the current desktop video mode.
    virtual core::dimension2d<s32> getDesktopResolution() const = 0;

    //! Returns the pixel depth of a video mode in bits.
    //! \return Returns the size of each pixel of the current desktop video mode in bits.
    virtual s32 getDesktopDepth() const = 0;

	//! adds a new mode to the list
    virtual void addMode(const core::dimension2d<s32>& size, s32 depth) = 0;

    virtual void setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize) = 0;
};

//---------------------------------------------------------------------------
} // end namespace video
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IVideoModeListHPP

