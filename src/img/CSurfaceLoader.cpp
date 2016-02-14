//|-------------------------------------------------------------------------
//| File:        CSurfaceLoader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSurfaceLoader.h"
#include <img/IImageLoader.h>
#include <io/IFileSystem.h>
#include <io/ILogger.h>
//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

IImageLoader* createImageLoaderBMP();
IImageLoader* createImageLoaderJPG();
IImageLoader* createImageLoaderTGA();
IImageLoader* createImageLoaderPSD();
IImageLoader* createImageLoaderPCX();
IImageLoader* createImageLoaderPNG();
IImageLoader* createImageLoaderDDS();
IImageLoader* createImageLoaderDevIL();

//-----------------------------------------------------------------------------

CSurfaceLoader::CSurfaceLoader()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CSurfaceLoader");
#endif

    // create surface loaders
	SurfaceLoader.push_back(img::createImageLoaderDevIL());
    SurfaceLoader.push_back(img::createImageLoaderBMP());
    SurfaceLoader.push_back(img::createImageLoaderJPG());
    SurfaceLoader.push_back(img::createImageLoaderTGA());
    SurfaceLoader.push_back(img::createImageLoaderPSD());
    SurfaceLoader.push_back(img::createImageLoaderPCX());
	SurfaceLoader.push_back(img::createImageLoaderPNG());
	SurfaceLoader.push_back(img::createImageLoaderDDS());
}

//-----------------------------------------------------------------------------

CSurfaceLoader::~CSurfaceLoader()
{
    // delete surface loaders
    for (u32 i=0; i<SurfaceLoader.size(); ++i)
        SAFE_DROP(SurfaceLoader[i]);
	SurfaceLoader.clear();
}

//-----------------------------------------------------------------------------

IImage* CSurfaceLoader::loadImageFromFile(io::IReadFile* file)
{
	if (!file)
		return 0;

	img::IImage* image = 0;
    // try to load file based on file extension

    u32 i;

    for (i=0; i<SurfaceLoader.size(); ++i)
	{
        if (SurfaceLoader[i]->isALoadableFileExtension(file->getFileName()))
        {
            image = SurfaceLoader[i]->loadImage(file);
            if (image)
                break;
        }
	}

    // try to load file based on what is in it
    if (!image)
	{
        for (i=0; i<SurfaceLoader.size(); ++i)
        {
            if (i!=0)
                file->seek(0);

            if (SurfaceLoader[i]->isALoadableFileFormat(file))
            {
                file->seek(0);
                image = SurfaceLoader[i]->loadImage(file);
                if (image)
                    break;
            }
        }
	}

    return image;
}

//-----------------------------------------------------------------------------

IImage* CSurfaceLoader::loadImageFromFile(const c8* fileName)
{
	io::IReadFile *file = FILE_SYSTEM.createAndOpenFile(fileName);

	IImage* image = loadImageFromFile(file);
	
	SAFE_DROP(file);	

    return image;
}

//---------------------------------------------------------------------------
 
void CSurfaceLoader::addSurfaceLoader(img::IImageLoader* loader)
{
    if (!loader)
        return;

    loader->grab();
    SurfaceLoader.push_back(loader);
}

//---------------------------------------------------------------------------

ISurfaceLoader* createSurfaceLoader()
{
    return new CSurfaceLoader();
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

