//|-------------------------------------------------------------------------
//| File:        CVideoModeList.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CVideoModeList.h"     

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! constructor
CVideoModeList::CVideoModeList()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CVideoModeList");
#endif

    Desktop.depth = 0;
    Desktop.size = core::dimension2d<s32>(0,0);
}

//---------------------------------------------------------------------------

//! destructor
CVideoModeList::~CVideoModeList()
{
}

//---------------------------------------------------------------------------

void CVideoModeList::setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize)
{
    Desktop.depth = desktopDepth;
    Desktop.size = desktopSize;
}

//---------------------------------------------------------------------------

//! Gets amount of video modes in the list.
s32 CVideoModeList::getVideoModeCount() const
{
    return (s32)VideoModes.size();
}

//---------------------------------------------------------------------------

//! Returns the screen size of a video mode in pixels.
core::dimension2d<s32> CVideoModeList::getVideoModeResolution(s32 modeNumber) const
{
    if (modeNumber < 0 || modeNumber > (s32)VideoModes.size())
        return core::dimension2d<s32>(0,0);

    return VideoModes[modeNumber].size;
}

//---------------------------------------------------------------------------

//! Returns the pixel depth of a video mode in bits.
s32 CVideoModeList::getVideoModeDepth(s32 modeNumber) const
{
    if (modeNumber < 0 || modeNumber > (s32)VideoModes.size())
        return 0;

    return VideoModes[modeNumber].depth;
}

//---------------------------------------------------------------------------

//! Returns current desktop screen resolution.
core::dimension2d<s32> CVideoModeList::getDesktopResolution() const
{
    return Desktop.size;
}

//---------------------------------------------------------------------------

//! Returns the pixel depth of a video mode in bits.
s32 CVideoModeList::getDesktopDepth() const
{
    return Desktop.depth;
}

//---------------------------------------------------------------------------

//! adds a new mode to the list
void CVideoModeList::addMode(const core::dimension2d<s32>& size, s32 depth)
{
    SVideoMode m;
    m.depth = depth;
    m.size = size;

    for (s32 i=0; i<(s32)VideoModes.size(); ++i)
    {
        if (VideoModes[i] == m)
            return;
    }

    VideoModes.push_back(m);
    VideoModes.sort(); // TODO: could be replaced by inserting into right place
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

