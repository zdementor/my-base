//|-------------------------------------------------------------------------
//| File:        ISurfaceLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISurfaceLoaderHPP
#define ISurfaceLoaderHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>
#include <img/SColor.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! class wich is able to load image data from all kind of image formats
class __MY_IMG_LIB_API__ ISurfaceLoader : public IUnknown
{
public:

    //! constructor
    ISurfaceLoader() {;}

    //! destructor
    virtual ~ISurfaceLoader() {;}

    //! creates a surface from the file
    virtual IImage* loadImageFromFile(io::IReadFile* file) = 0;

    //! creates a surface from the file
    virtual IImage* loadImageFromFile(const c8* fileName) = 0;

	//! Adds an external surface loader to the array of loaders
	virtual void addSurfaceLoader(IImageLoader* loader) = 0;

};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

