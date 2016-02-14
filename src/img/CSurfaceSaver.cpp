//|-------------------------------------------------------------------------
//| File:        CSurfaceSaver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSurfaceSaver.h"

#include <img/IImageSaver.h>
#include <io/IFileSystem.h>
#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

IImageSaver* createImageSaverTGA();
IImageSaver* createImageSaverBMP();
IImageSaver* createImageSaverJPG();
IImageSaver* createImageSaverDevIL();

//-----------------------------------------------------------------------------

CSurfaceSaver::CSurfaceSaver()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CSurfaceSaver");
#endif

	SurfaceSaver.push_back(createImageSaverDevIL());
	SurfaceSaver.push_back(createImageSaverTGA());
    SurfaceSaver.push_back(createImageSaverBMP());
    SurfaceSaver.push_back(createImageSaverJPG());
}

//-----------------------------------------------------------------------------

CSurfaceSaver::~CSurfaceSaver()
{
    for (u32 i=0; i<SurfaceSaver.size(); ++i)
        SAFE_DROP(SurfaceSaver[i]);
	SurfaceSaver.clear();
}

//-----------------------------------------------------------------------------

bool CSurfaceSaver::saveImageToFile(IImage* image, io::IWriteFile* file)
{
	if (!image || !file) return false;
	
    // try to save file
    for (u32 i=0; i<SurfaceSaver.size(); ++i)
	{
        if (SurfaceSaver[i]->isASaveableFileExtension(file->getFileName()))
            return SurfaceSaver[i]->saveImage(image, file);
	}	

	core::stringc msg;

	msg = "Cant save image to file '";
	msg.append(file->getFileName());
	msg.append("' (format is seems to be unsupported).");
	
	LOGGER.log("CSurfaceSaver",msg.c_str(),io::ELL_ERROR);

    return false;
}

//-----------------------------------------------------------------------------

bool CSurfaceSaver::saveImageToFile(IImage* image, const c8* fileName)
{
	if (!image) return false;

	io::IWriteFile *file = FILE_SYSTEM.createAndWriteFile(fileName);
    bool res = saveImageToFile(image, file);
    SAFE_DROP(file);

    return res;
}

//---------------------------------------------------------------------------

void CSurfaceSaver::addSurfaceSaver(img::IImageSaver* saver)
{
    if (!saver)
        return;

    saver->grab();
    SurfaceSaver.push_back(saver);
}

//---------------------------------------------------------------------------

ISurfaceSaver* createSurfaceSaver()
{
    return new CSurfaceSaver();
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

