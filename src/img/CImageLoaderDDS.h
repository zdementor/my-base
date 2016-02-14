//|-------------------------------------------------------------------------
//| File:        CImageLoaderDDS.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderDDSHPP
#define CImageLoaderDDSPHPP
//-----------------------------------------------------------------------------

#include "CImageLoader.h"

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//!  Surface Loader fow Windows bitmaps
class CImageLoaderDDS : public CImageLoader
{
public:

    CImageLoaderDDS();
    virtual ~CImageLoaderDDS();

    virtual bool isALoadableFileExtension(const c8* fileName);

    virtual bool isALoadableFileFormat(io::IReadFile* file);

    virtual IImage* loadImage(io::IReadFile* file);

    virtual IImage* loadImage(const c8* fileName);

private:

};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
#endif

