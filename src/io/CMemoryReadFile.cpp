//|-------------------------------------------------------------------------
//| File:        CMemoryReadFile.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMemoryReadFile.h"

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

CMemoryReadFile::CMemoryReadFile(
	void* memory, s32 len, const c8* fileName, bool d
	) : 
Buffer(memory), Len(len), Pos(0), deleteMemoryWhenDropped(d)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CReadFile");
#endif

    Filename = fileName;

	FilePath = core::extractFilePath(Filename); 
}

//---------------------------------------------------------------------------

CMemoryReadFile::~CMemoryReadFile()
{
    if (deleteMemoryWhenDropped)
        delete [] (c8*)Buffer;
}

//---------------------------------------------------------------------------

//! returns how much was read
s32 CMemoryReadFile::read(void* buffer, s32 sizeToRead)
{
    s32 amount = sizeToRead;
    if (Pos + amount > Len)
        amount -= Pos + amount - Len;

    if (amount < 0)
        amount = 0;

    c8* p = (c8*)Buffer;
    memcpy(buffer, p + Pos, amount);
    
    Pos += static_cast<u32> ( amount );

    return amount;
}

//---------------------------------------------------------------------------

//! ������ ������� �� ����� (���� �� �������� 0)
//! returns how much was read
s32 CMemoryReadFile::readString(void* buffer, s32 maxBuffSize)
{   
	s32 strLen =0;
    s32 readLen=0;   
    c8* p = (c8*)Buffer;
    c8* b = (c8*)buffer;
    
    p += Pos;

    while(true)
    {   if (*(p+readLen)==0)
        // ����� ����� ������
            break;

        if ( readLen<(maxBuffSize-1) && (Pos+readLen)<Len)
        // ����� � �����, ������ ���� � ��� ���������� �����
        // � ����� ����� ��� �� ���������
        {   b[strLen]=*(p+strLen);
            strLen++;
        }
        readLen++;
    }

    // ������ ����� ������
    b[strLen] =0;

    // ��������� ������� �� ��������� ����������� ������
    Pos += strLen + 1;

    // ���������� ����� ����������� ��������
    return strLen+1;
}

//---------------------------------------------------------------------------

//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CMemoryReadFile::seek(s32 finalPos, bool relativeMovement)
{
    if (relativeMovement)
    {
        if (Pos + finalPos > Len)
            return false;

        Pos += finalPos;
    }
    else
    {
        if ( (unsigned) finalPos > Len)
            return false;
        
        Pos = finalPos;
    }

    return true;
}

//---------------------------------------------------------------------------

//! returns size of file
s32 CMemoryReadFile::getSize()
{
    return Len;
}

//---------------------------------------------------------------------------

//! returns where in the file we are.
s32 CMemoryReadFile::getPos()
{
    return Pos;
}

//---------------------------------------------------------------------------

//! returns name of file
const c8* CMemoryReadFile::getFileName()
{
    return Filename.c_str();
}

//---------------------------------------------------------------------------

//! returns file path
const c8* CMemoryReadFile::getFilePath()
{
    return FilePath.c_str();
}

//---------------------------------------------------------------------------

IReadFile* createMemReadFile(
	void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped
	)
{
    CMemoryReadFile* file = new CMemoryReadFile(
		memory, size, fileName, deleteMemoryWhenDropped
		);
    return file;
}

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------