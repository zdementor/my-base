//|-------------------------------------------------------------------------
//| File:        CSoundData.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSoundData.h"
#include "../io/CMemoryReadFile.h"
#include <io/IFileSystem.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

CSoundData :: CSoundData ( const c8* filename )
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSoundData");
#endif

	ok = false;
	readfile = NULL;

	io::IReadFile *rfile = FILE_SYSTEM.createAndOpenFile(filename);	
	
	if (rfile)
	{
		c8* rdata = new c8 [rfile->getSize()];
		
		s32 rsize = rfile->read(rdata, rfile->getSize());

		readfile = FILE_SYSTEM.createMemoryReadFile(rdata, rsize, filename, true);	

		SAFE_DROP(rfile);

		numChannels   = 0;
		frequency     = 0;
		bitsPerSample = 8;
		ok            = true;
	}
}

//-----------------------------------------------------------------------

CSoundData :: ~CSoundData ()
{
	SAFE_DROP(readfile);
}

//-----------------------------------------------------------------------

bool	CSoundData :: isOk () const
{
	return readfile != NULL && ok;
}

//-----------------------------------------------------------------------

// return position in sound (in seconds)
f32 CSoundData::getTime ()
{
	return 0.0f;
}

//-----------------------------------------------------------------------

bool CSoundData::atEnd () const
{
	if (readfile)
	{
		return readfile->getPos () >= readfile->getSize();
	}

	return true;
}

//-----------------------------------------------------------------------

// returns sound data file name
const c8* CSoundData::getFileName () const
{
	return readfile ? readfile->getFileName() : NULL;
}

//-----------------------------------------------------------------------

//! return sound type
E_SOUND_DATA_TYPE CSoundData::getType()
{
	return Type;
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------