//|-------------------------------------------------------------------------
//| File:        CSoundDataOgg.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	CSoundDataOggHPP
#define	CSoundDataOggHPP
//------------------------------------------------------------------------

#include "CSoundData.h"
#include <vorbis/vorbisfile.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class CSoundDataOgg : public CSoundData
{
	OggVorbis_File	file;
	vorbis_info	  * info;

public:

	CSoundDataOgg  ( const c8* filename );
	virtual ~CSoundDataOgg ();

	virtual	int	getSize () const;

	// fill buffer with next audio data
	virtual	int	read ( c8* buf, int size = -1 );

	// position in sound (in seconds)
	virtual	bool seek ( float time );

	// return position in sound (in seconds)
	virtual	f32 getTime ();

protected:

	static	size_t readFunc  ( void * ptr, size_t size, size_t nmemb, void * datasource );
	static	int    seekFunc  ( void * datasource, ogg_int64_t offset, int whence );
	static	int    closeFunc ( void * datasource );
	static	long   tellFunc  ( void * datasource );

};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	CSoundDataOggHPP

