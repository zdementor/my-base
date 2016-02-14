//|-------------------------------------------------------------------------
//| File:        CVideoModeList.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CVideoModeListHPP
#define CVideoModeListHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/IVideoModeList.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class CVideoModeList : public IVideoModeList
{
public:

    //! constructor
    CVideoModeList();

    //! destructor
    virtual ~CVideoModeList();

    //! Gets amount of video modes in the list.
    virtual s32 getVideoModeCount() const;

    //! Returns the screen size of a video mode in pixels.
    virtual core::dimension2d<s32> getVideoModeResolution(s32 modeNumber) const;

    //! Returns the pixel depth of a video mode in bits.
    virtual s32 getVideoModeDepth(s32 modeNumber) const;

    //! Returns current desktop screen resolution.
    virtual core::dimension2d<s32> getDesktopResolution() const;

    //! Returns the pixel depth of a video mode in bits.
    virtual s32 getDesktopDepth() const;

    //! adds a new mode to the list
    virtual void addMode(const core::dimension2d<s32>& size, s32 depth);

    virtual void setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize);

private:

    struct SVideoMode
    {
        core::dimension2d<s32> size;
        s32 depth;

        bool operator==(const SVideoMode& other) const
        {
            return size == other.size && depth == other.depth;
        }

        bool operator <(const SVideoMode& other) const
        {
            return (size.Width < other.size.Width ||
                (size.Width == other.size.Width && 
                size.Height < other.size.Height) ||
                (size.Width == other.size.Width && 
                size.Height == other.size.Height &&
                depth < other.depth));
        }
    };

    core::array<SVideoMode> VideoModes;
    SVideoMode Desktop;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

