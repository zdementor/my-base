//|-------------------------------------------------------------------------
//| File:        CSoundData.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	CSoundDataHPP
#define	CSoundDataHPP
//---------------------------------------------------------------------

#include <my_inc.h>
#include <mm/ISoundData.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//---------------------------------------------------------------------

class CSoundData : public ISoundData
{

protected:

	io::IReadFile *readfile;

	s32		numChannels;
	s32		frequency;
	s32		bitsPerSample;
	bool	ok;

	E_SOUND_DATA_TYPE Type;

public:

	CSoundData ( const c8* filename );
	virtual	~CSoundData ();


	s32	getNumChannels () const
	{
		return numChannels;
	}

	s32	getFrequency () const
	{
		return frequency;
	}

	s32	getBitsPerSample () const
	{
		return bitsPerSample;
	}
	
	virtual	bool isOk () const;

	// get size in bytes of unpacked audio data
	virtual	s32	getSize () const = 0;

	// fill buffer with next audio data
	virtual	s32	read ( c8* buf, s32 size = -1 ) = 0;

	// position in sound (in seconds)
	virtual	bool seek ( f32 time ) = 0;

	// return position in sound (in seconds)
	virtual	f32 getTime ();
	
	// whether we're at end of sound data
	virtual	bool atEnd () const;

	// returns sound data file name
	virtual	const c8* getFileName () const;

	//! return sound type
	E_SOUND_DATA_TYPE getType();
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	CSoundDataHPP
