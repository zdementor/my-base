//|-------------------------------------------------------------------------
//| File:        CLimitReadFile.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CLimitReadFile.h"

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

CLimitReadFile::CLimitReadFile(
	IReadFile* alreadyOpenedFile, s32 areaSize, const c8* name
	) : 
AreaSize(areaSize), File(alreadyOpenedFile), Filename(name)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CLimitReadFile");
#endif

    SAFE_GRAB(File);

	FilePath = core::extractFilePath(Filename);

    init();
}

//----------------------------------------------------------------------------

CLimitReadFile::~CLimitReadFile()
{
    SAFE_DROP(File);
}

//----------------------------------------------------------------------------

void CLimitReadFile::init()
{
    if (!File)
        return;

    AreaStart = File->getPos();
    AreaEnd = AreaStart + AreaSize;
}

//----------------------------------------------------------------------------

s32 CLimitReadFile::read(void* buffer, s32 sizeToRead)
{
    s32 pos = File->getPos();

    if (pos >= AreaEnd)
        return 0;

    if (pos + sizeToRead >= AreaEnd)
        sizeToRead = AreaEnd - pos;

    return File->read(buffer, sizeToRead);
}

//----------------------------------------------------------------------------

bool CLimitReadFile::seek(s32 finalPos, bool relativeMovement)
{
    s32 pos = File->getPos();

    if (relativeMovement)
    {
        if (pos + finalPos > AreaEnd)
            finalPos = AreaEnd - pos;
    }
    else
    {
        finalPos += AreaStart;
        if ((s32)finalPos > AreaEnd)
            return false;
    }

    return File->seek(finalPos, relativeMovement);  
}

//----------------------------------------------------------------------------

s32 CLimitReadFile::getSize()
{
    return AreaSize;
}

//----------------------------------------------------------------------------

s32 CLimitReadFile::getPos()
{
    return File->getPos() - AreaStart;
}

//----------------------------------------------------------------------------

const c8* CLimitReadFile::getFileName()
{
    return Filename.c_str();
}

//----------------------------------------------------------------------------

const c8* CLimitReadFile::getFilePath()
{
    return FilePath.c_str();
}

//----------------------------------------------------------------------------

IReadFile* createLimitedReadFile(
	const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize
	)
{
    return new CLimitReadFile(alreadyOpenedFile, areaSize, fileName);
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------