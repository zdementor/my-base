//|-------------------------------------------------------------------------
//| File:        CImageDevIL.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageDevILHPP
#define CImageDevILHPP
//----------------------------------------------------------------------------

#include "CImage.h"

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

class CImageDevIL : public CImage
{
public:       

	CImageDevIL();
	virtual ~CImageDevIL();

	bool imageFrom(IImage *image);
	bool loadFrom(io::IReadFile *file);
	bool saveTo(io::IWriteFile *file);

	static bool isALoadableFileExtension(const c8* fileName);
	static bool isALoadableFileFormat(io::IReadFile* file);
	static bool isASaveableFileExtension(const c8* fileName);

protected:

	class ilImage *m_ILImage;
};

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif

