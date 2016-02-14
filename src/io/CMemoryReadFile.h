//|-------------------------------------------------------------------------
//| File:        CMemoryReadFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMemoryReadFileHPP
#define CMemoryReadFileHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <io/IReadFile.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Class for reading from memory.
class CMemoryReadFile : public IReadFile
{
public:
    
    CMemoryReadFile(void* memory, s32 len, const c8* fileName, bool deleteMemoryWhenDropped);
    virtual ~CMemoryReadFile();

    virtual s32 read(void* buffer, s32 sizeToRead);

    virtual s32 readString(void* buffer, s32 maxBuffSize);

    virtual bool seek(s32 finalPos, bool relativeMovement = false);

    virtual s32 getSize();

    virtual s32 getPos();

    virtual const c8* getFileName();
    virtual const c8* getFilePath();

    virtual const void* getHandle() { return NULL;}

private:

    core::stringc Filename, FilePath;
    void *Buffer;
    u32 Len;
    u32 Pos;
    bool deleteMemoryWhenDropped;
};
//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------
#endif

