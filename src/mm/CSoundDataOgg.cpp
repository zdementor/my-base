//|-------------------------------------------------------------------------
//| File:        CSoundDataOgg.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSoundDataOgg.h"
#include <io/IReadFile.h>
#include <io/ILogger.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

CSoundDataOgg :: CSoundDataOgg  (  const c8* filename  ) : CSoundData ( filename )
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSoundDataOgg");
#endif

	if (!isOk()) return;

	readfile->seek(0);

	ov_callbacks	cb;

	cb.read_func  = readFunc;
	cb.seek_func  = seekFunc;
	cb.close_func = closeFunc;
	cb.tell_func  = tellFunc;

	if ( ov_open_callbacks ( this, &file, NULL, 0, cb ) < 0 )
	{
		ok = false;

		return;
	}

	info          = ov_info ( &file, -1 );
	numChannels   = info -> channels;
	frequency     = info -> rate;
	bitsPerSample = 16;

	Type = ESDT_OGG;
}

//-----------------------------------------------------------------------------------

CSoundDataOgg :: ~CSoundDataOgg ()
{
	if (isOk())
		ov_clear ( &file );
}

//-----------------------------------------------------------------------------------

int	CSoundDataOgg :: getSize () const
{
	double	totalTime = ov_time_total ( const_cast <OggVorbis_File *> ( &file ), -1 );

	return (int)( totalTime + 0.5) * numChannels * frequency * 2;
}

//-----------------------------------------------------------------------------------

// fill buffer with next audio data
s32	CSoundDataOgg :: read ( c8* buf, s32 size )
{
	if ( size < 0 )
		size = getSize ();

	int	curSection;
	int	bytesRead = 0;

	while ( bytesRead < size )
	{
		int	res = ov_read ( &file, (char *)(buf + bytesRead), size - bytesRead, 0, 2, 1, &curSection );

		if ( res <= 0 )
			break;

		bytesRead += res;
	}

	return bytesRead;
}

//-----------------------------------------------------------------------------------

// position in sound (in seconds)
bool	CSoundDataOgg :: seek ( float time )
{
	ov_time_seek ( &file, time );

	return true;
}

//-----------------------------------------------------------------------------------

// return position in sound (in seconds)
f32 CSoundDataOgg::getTime ()
{
	return (f32)ov_time_tell(&file);
}

//-----------------------------------------------------------------------------------

size_t CSoundDataOgg::readFunc ( void * ptr, size_t size, size_t nmemb, void * datasource )
{
	CSoundDataOgg * dt   = (CSoundDataOgg *) datasource;

	if ( dt->readfile->getPos() >= dt->readfile->getSize() )
  		return 0;

  	return (size_t) dt->readfile->read ( ptr, s32(size * nmemb) );
}

//-----------------------------------------------------------------------------------

int CSoundDataOgg::seekFunc ( void * datasource, ogg_int64_t offset, int whence )
{
	CSoundDataOgg * dt   = (CSoundDataOgg *) datasource;

	if ( whence == SEEK_SET )
  		dt->readfile->seek( (int) offset , SEEK_SET);
	else
	if ( whence == SEEK_CUR )
  		dt->readfile->seek( (int) offset , SEEK_CUR);
	else
	if ( whence == SEEK_END )
  		dt->readfile->seek( dt->readfile->getSize() + (int) offset , SEEK_SET);

  	return dt->readfile->getPos ();
}

//-----------------------------------------------------------------------------------

int CSoundDataOgg::closeFunc ( void * datasource )
{
	return 0;
}

//-----------------------------------------------------------------------------------

long CSoundDataOgg::tellFunc ( void * datasource )
{
	CSoundDataOgg * dt   = (CSoundDataOgg *) datasource;

	return dt->readfile->getPos ();
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------