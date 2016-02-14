//|-------------------------------------------------------------------------
//| File:        CSurfaceLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSurfaceLoaderHPP
#define CSurfaceLoaderHPP
//-----------------------------------------------------------------------------

#include <img/ISurfaceLoader.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! class wich is able to load image data from all kind of image formats
class CSurfaceLoader : public ISurfaceLoader
{
public:
    //! constructor
    CSurfaceLoader();

    //! destructor
    virtual ~CSurfaceLoader();

    //! creates a surface from the file
    virtual IImage* loadImageFromFile(io::IReadFile* file);

    //! creates a surface from the file
    virtual IImage* loadImageFromFile(const c8* fileName);

	//! Adds an external surface loader to the array of loaders
	virtual void addSurfaceLoader(img::IImageLoader* loader);

private:

	core::array<img::IImageLoader*> SurfaceLoader;

};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

