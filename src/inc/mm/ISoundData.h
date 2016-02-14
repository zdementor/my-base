//|-------------------------------------------------------------------------
//| File:        ISoundData.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	ISoundDataHPP
#define	ISoundDataHPP
//---------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//---------------------------------------------------------------------

class __MY_MM_LIB_API__ ISoundData : public IUnknown 
{

public:

	virtual	s32	getNumChannels () const = 0;
	virtual	s32	getFrequency () const = 0;
	virtual	s32	getBitsPerSample () const = 0;	
	virtual	bool isOk () const = 0;

	// get size in bytes of unpacked audio data
	virtual	s32	getSize () const = 0;

	// fill buffer with next audio data
	virtual	s32	read ( c8* buf, s32 size = -1 ) = 0;

	// position in sound (in seconds)
	virtual	bool seek ( f32 time ) = 0;

	// return position in sound (in seconds)
	virtual	f32 getTime () = 0;
	
	// whether we're at end of sound data
	virtual	bool atEnd () const = 0;

	// returns sound data file name
	virtual	const c8* getFileName () const = 0;

	//! return sound type
	virtual	E_SOUND_DATA_TYPE getType() = 0;
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	ISoundDataHPP
