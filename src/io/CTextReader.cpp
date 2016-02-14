//|-------------------------------------------------------------------------
//| File:        CTextReader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CTextReader.h"
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

CTextReader::CTextReader(IReadFile* file)
: TextData(0), TextDataSize(0), CData(0), CDataSize(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CTextReader");
#endif

    readFile(file);
}

//----------------------------------------------------------------------------

//! Destructor
CTextReader::~CTextReader()
{
    delete [] CData;
}
//----------------------------------------------------------------------------
//! Returns pointer to read text buffer
wchar_t* CTextReader::getText()
{
    return TextData;
}
//----------------------------------------------------------------------------
//! returns size of text buffer
s32 CTextReader::getTextSize() const
{
    return TextDataSize;
}
//----------------------------------------------------------------------------
//! reads the complete file into the buffer
void CTextReader::readFile(IReadFile* file)
{
    if (!file)
        return;

	u32 pos = file->getPos();
	file->seek(0);

	FileName = file->getFileName();
	FilePath = file->getFilePath();

    CDataSize = file->getSize();

    if (!CDataSize)
        return;

    CDataSize+=2; // We need two terminating 0's at the end.
                 // for ASCII we need 1 0's, for UTF-16 2.

    CData = new c8[CDataSize];

    if (!file->read(CData, CDataSize-2))
    {
        delete [] CData;
        CData = 0;
        CDataSize = 0;
    }
    else
    {

        CData[CDataSize-1] = 0;
        CData[CDataSize-2] = 0;
        convertToUTF16();
    }

	file->seek(pos);
}

//----------------------------------------------------------------------------

//! converts the read text data to utf16 text.
void CTextReader::convertToUTF16()
{
    wchar_t* header = (wchar_t*)CData;

    if (CDataSize>=2 && 
        (header[0] == 0xFFFE || header[0] == 0xFEFF))
    {
        // We've got UTF-16

        TextData = &header[1];
        TextDataSize = (CDataSize - 1) / (sizeof(wchar_t) / sizeof(c8)); // header 

        if (header[0] == 0xFFFE)
        {
            // This is Big Endian, 
            // convert to little endian.

            for (u32 i=0; i<CDataSize; ++i)
                TextData[i] = convertBEtoLE(TextData[i]);
        }
    }
    else
    {
        // ASCII, convert to UTF-16

        TextDataSize = CDataSize;
        convertASCIIToUTF16(CData, CDataSize, &TextData);
        delete [] CData; 
        CData = (c8*)TextData;
        return;
    }
}

//----------------------------------------------------------------------------

//! converts ASCII to an UTF-16, output pointer should be deleted by caller
void CTextReader::convertASCIIToUTF16(const c8* characters, u32 len, wchar_t** out)
{
    (*out) = new wchar_t[len];
    for (u32 i=0; i<len; ++i)
        (*out)[i] = characters[i];
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

