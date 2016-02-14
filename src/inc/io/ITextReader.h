//|-------------------------------------------------------------------------
//| File:        ITextReader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITextReaderHPP
#define ITextReaderHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Text Reader providing easy access text files, no matter if they are
//! in ASCII or UTF-16 format. UTF-8 is currently not supported yet,
//! but may be added later.
class ITextReader : public IUnknown
{
public:

	//! Returns name of file from wich text reader reads text contaned in it
	virtual const c8* getFileName() = 0;

	//! Returns file path from wich text reader reads text contaned in it
	virtual const c8* getFilePath() = 0;

    //! Returns pointer to read text buffer
    virtual wchar_t* getText() = 0;

    //! returns size of text buffer
    virtual s32 getTextSize() const = 0;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef ITextReaderHPP

