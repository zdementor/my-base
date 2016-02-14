//|-------------------------------------------------------------------------
//| File:        IImagePresenter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IImagePresenterHPP
#define IImagePresenterHPP
//----------------------------------------------------------------------------
#include <img/IImage.h>
//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

// Interface for a class which is able to present an IImage 
// an the Screen. Usually only implemented by an IrrDevice for
// presenting Software Device Rendered images.
// This class should be used internal only.
class IImagePresenter
{
public:
    //! presents a surface in the client area
    virtual void present(img::IImage* surface) = 0;
};

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------
#endif

