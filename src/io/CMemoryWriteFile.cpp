//|-------------------------------------------------------------------------
//| File:        CMemoryWriteFile.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMemoryWriteFile.h"

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

CMemoryWriteFile::CMemoryWriteFile(const c8* fileName, core::stringw& out_buffer) 
	: Pos(0), BufferCPtr(0), BufferWPtr(&out_buffer)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CMemoryWriteFile");
#endif

    Filename = fileName;
	FilePath = core::extractFilePath(Filename); 
}

//---------------------------------------------------------------------------

CMemoryWriteFile::CMemoryWriteFile(const c8* fileName, core::stringc& out_buffer) 
	: Pos(0), BufferCPtr(&out_buffer), BufferWPtr(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CMemoryWriteFile");
#endif

    Filename = fileName;
	FilePath = core::extractFilePath(Filename); 
}

//---------------------------------------------------------------------------

CMemoryWriteFile::~CMemoryWriteFile()
{
}

//---------------------------------------------------------------------------

//! Reads an amount of bytes from the file.
u32 CMemoryWriteFile::write(const void* buffer, u32 sizeToWrite, bool flush)
{
	if (BufferCPtr)
	{
		core::stringc& Buffer = (*BufferCPtr);

		if (Buffer.size()<(s32)(Pos+sizeToWrite))
			Buffer.reserve(Pos+sizeToWrite+1);

		for (u32 i=0; i<sizeToWrite; i++)
			Buffer[Pos++] = ((c8*)buffer)[i];

		return sizeToWrite;

	}
	else
	if (BufferWPtr)
	{
		core::stringw& Buffer = (*BufferWPtr);

		/*
		if (Buffer.size()*2<(s32)(Pos+sizeToWrite))
			Buffer.reserve(Pos+(sizeToWrite)/2.0f+0.5f+1);

		for (s32 i=0; i<sizeToWrite; i++)
			Buffer[Pos++] = ((wchar_t*)buffer)[i];
			*/

		u32 size = (sizeToWrite)/2.0f+0.5f;

		if (Buffer.size()<(s32)(Pos+size))
			Buffer.reserve(Pos+size+1);

		for (u32 i=0; i<size; i++)
			Buffer[Pos++] = ((wchar_t*)buffer)[i];

		return sizeToWrite;
	}

	return 0;	
}

//---------------------------------------------------------------------------

//! Changes position in file, returns true if successful.
bool CMemoryWriteFile::seek(s32 finalPos, bool relativeMovement)
{
	if (BufferCPtr)
	{
		core::stringc& Buffer = (*BufferCPtr);

		if (relativeMovement)
			Pos = Pos + finalPos;
		else
			Pos = finalPos;

		if ((s32)Pos<Buffer.size())
			return true;
	}
	else
	if (BufferWPtr)
	{
		core::stringw& Buffer = (*BufferWPtr);

		if (relativeMovement)
			Pos = Pos + finalPos;
		else
			Pos = finalPos;

		if ((s32)Pos<Buffer.size())
			return true;		
	}

	Pos=0;

	return false;
}

//---------------------------------------------------------------------------

//! Returns the current position in the file.
s32 CMemoryWriteFile::getPos()
{
	return Pos;
}

//---------------------------------------------------------------------------

IWriteFile* createMemWriteFile(const c8* fileName, core::stringc& out_buffer)
{
    CMemoryWriteFile* file = new CMemoryWriteFile(fileName, out_buffer);
    return file;
}

//---------------------------------------------------------------------------

IWriteFile* createMemWriteFile(const c8* fileName, core::stringw& out_buffer)
{
    CMemoryWriteFile* file = new CMemoryWriteFile(fileName, out_buffer);
    return file;
}

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------