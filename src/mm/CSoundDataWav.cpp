//|-------------------------------------------------------------------------
//| File:        CSoundDataWav.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSoundDataWav.h"

#include <io/ILogger.h>
#include <io/IReadFile.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

#pragma pack (push, 1)
struct WavFmt
{
	unsigned short encoding;
	unsigned short channels;
	unsigned int   frequency;
	unsigned int   byterate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
};
struct WavFmtEx : public WavFmt
{
	unsigned short unknown_data;
};
#pragma pack (pop)

//-----------------------------------------------------------------------------

CSoundDataWav :: CSoundDataWav ( const c8* filename ) : CSoundData ( filename )
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSoundDataWav");
#endif

	Type = ESDT_WAV;

	if (!isOk()) return;

	readfile->seek(0);

	s32	magic;
	s32	length;
	s32	magic2;

	readfile ->read((void*)&magic, 4);
	readfile ->read((void*)&length, 4);
	readfile ->read((void*)&magic2, 4);

	if ( magic != RIFF || magic2 != WAVE )
	{
		ok = false;
		return;
	}

	for ( ; readfile -> getPos () < readfile->getSize(); )
	{
		readfile ->read((void*)&magic, 4);
		readfile ->read((void*)&length, 4);

		if ( magic == FMT)
		{
			if (length==sizeof(WavFmt))
			{
				WavFmt	format;

				readfile ->read( &format, sizeof ( WavFmt ) );

				if ( format.encoding != 1 )
				{
					ok = false;

					return;
				}

				numChannels   = format.channels;
				frequency     = format.frequency;
				bitsPerSample = format.bitsPerSample;
			}
			else
			if (length==sizeof(WavFmtEx))
			{
				WavFmtEx	format;

				readfile ->read( &format, sizeof ( WavFmtEx ) );

				if ( format.encoding != 1 )
				{
					ok = false;

					return;
				}

				numChannels   = format.channels;
				frequency     = format.frequency;
				bitsPerSample = format.bitsPerSample;
			}
			else
			{
				static core::stringc msg;
				msg.sprintf("Wrong wav-file format '%s'", filename);
				LOGGER.log(msg,io::ELL_ERROR);
				ok=false;
				break;
			}
		}
		else
		if ( magic == DATA )
		{
			dataOffs   = readfile->getPos ();
			dataLength = length;

			break;
		}
		else 
		if (!readfile->seek(length , SEEK_CUR))
		{			
			static core::stringc msg;
			msg.sprintf("Wrong wav-file format '%s'", filename);
			LOGGER.log(msg,io::ELL_ERROR);
			ok=false;
			break;
		}
	}

	if ( numChannels < 1 || frequency == 0 || dataOffs == 0 || dataLength == 0 )
		ok = false;	
}

//-----------------------------------------------------------------------------

CSoundDataWav :: ~CSoundDataWav ()
{
}

//-----------------------------------------------------------------------------

s32	CSoundDataWav :: getSize () const
{
	return dataLength;
}

//-----------------------------------------------------------------------------

// fill buffer with next audio data
s32 CSoundDataWav :: read ( c8* buf, s32 size )
{
	int	bytesLeft = dataLength - (readfile->getPos () - dataOffs);

	if ( size < 0 || size > bytesLeft )
		size = bytesLeft;

	return readfile->read( buf, size );
}

//-----------------------------------------------------------------------------		
								
// position in sound (in seconds)
bool CSoundDataWav :: seek ( float time )
{
	readfile->seek( dataOffs + (int)(time * numChannels * frequency*bitsPerSample/2) );	

	return true;
}

//-----------------------------------------------------------------------------------

// return position in sound (in seconds)
f32 CSoundDataWav::getTime ()
{
	// dont now how to calc current sound time for Wav
	return 0.0f;
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------