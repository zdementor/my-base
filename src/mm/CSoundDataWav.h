//|-------------------------------------------------------------------------
//| File:        CSoundDataWav.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	CSoundDataWavHPP
#define	CSoundDataWavHPP
//------------------------------------------------------------------------

#include "CSoundData.h"

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class	CSoundDataWav : public CSoundData
{
	int	dataOffs;
	int	dataLength;

public:

	CSoundDataWav  ( const c8* filename );
	virtual ~CSoundDataWav ();

	virtual	int	getSize () const;

	// fill buffer with next audio data
	virtual	int	read ( c8* buf, s32 size = -1 );

	// position in sound (in seconds)
	virtual	bool	seek ( f32 time );

	// return position in sound (in seconds)
	virtual	f32 getTime ();

	enum
	{
		RIFF = 0x46464952,
		WAVE = 0x45564157,
		FMT  = 0x20746D66,
		DATA = 0x61746164,
	};
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	CSoundDataWavHPP

