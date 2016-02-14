//|-------------------------------------------------------------------------
//| File:        CLimitReadFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CLimitReadFileHPP
#define CLimitReadFileHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <io/IReadFile.h>

//----------------------------------------------------------------------------

namespace my {
class CUnicodeConverter;
namespace io {

//----------------------------------------------------------------------------

//! this is a read file, which is limited to some boundaries, 
//! so that it may only start from a certain file position
//! and may only read until a certain file position.
//! This can be useful, for example for reading uncompressed files
//! in an archive (zip).
class CLimitReadFile : public IReadFile
{
public:
    
    CLimitReadFile(IReadFile* alreadyOpenedFile, s32 areaSize, const c8* name);
    virtual ~CLimitReadFile();

    virtual s32 read(void* buffer, s32 sizeToRead);

    virtual bool seek(s32 finalPos, bool relativeMovement = false);

    virtual s32 getSize();

    virtual s32 getPos();

    virtual const c8* getFileName();
    virtual const c8* getFilePath();

    virtual const void* getHandle() { return (const void*)File;}

private:

    void init();

    core::stringc Filename, FilePath;
    s32 AreaSize;
    s32 AreaStart;
    s32 AreaEnd;
    IReadFile* File;
};
//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------
#endif

