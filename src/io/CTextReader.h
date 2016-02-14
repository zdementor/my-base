//|-------------------------------------------------------------------------
//| File:        CTextReader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTextReaderHPP
#define CTextReaderHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <io/IReadFile.h>
#include <io/ITextReader.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Text Reader providing easy access text files, no matter if they are
//! in ASCII or UTF-16 format. UTF-8 is currently not supported yet,
//! but may be added later.
class CTextReader : public ITextReader
{
public:

    CTextReader(IReadFile* file);

    //! Destructor
    virtual ~CTextReader();

	//! Returns name of file from wich text reader reads text contaned in it
	virtual const c8* getFileName() 
	{
		return FileName.c_str();
	}

	//! Returns file path from wich text reader reads text contaned in it
	virtual const c8* getFilePath() 
	{
		return FilePath.c_str();
	}

    //! Returns pointer to read text buffer
    virtual wchar_t* getText();

    //! returns size of text buffer
    virtual s32 getTextSize() const;

private:

    //! reads the complete file into the buffer
    void readFile(IReadFile* file);

    //! converts the read text data to utf16 text.
    void convertToUTF16();

    //! converts ASCII to an UTF-16, output pointer should be deleted by caller
    void convertASCIIToUTF16(const c8* characters, u32 len, wchar_t** out);

    //! converts an big endian to little endian
    inline unsigned short convertBEtoLE (unsigned short be)
    {
        return (be >> 8) | (be << 8);
    }

    char* CData;                    // read data from file
    wchar_t* TextData;          // pointer to textdata
    unsigned int TextDataSize;  // size of wchat_t buffer without header
    unsigned int CDataSize;     // site of char buffer

	core::stringc FileName, FilePath;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

