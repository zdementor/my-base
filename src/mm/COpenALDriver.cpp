//|-------------------------------------------------------------------------
//| File:        COpenALDriver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenALDriver.h"
#include "CSoundDataOgg.h"
#include "CSoundDataWav.h"
#include "CSound.h"
#include <io/ILogger.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

COpenALDriver::COpenALDriver() 
	: device(NULL), context(NULL)
{	
#if MY_DEBUG_MODE  
	IUnknown::setClassName("COpenALDriver");
#endif

	LOGGER.log("Sound initialization:", io::ELL_INFORMATION);
	LOGGER.log(" OpenAL driver activation...", io::ELL_INFORMATION);

	try
	{
		device = alcOpenDevice ( NULL );
	}
	catch(...)
	{
		device = NULL;
	}
	
	if ( device == NULL )
	{
		LOGGER.log(" Can't open OpenAL device!", io::ELL_ERROR);
		return;
	}

	LOGGER.log(" OAL device opened", io::ELL_INFORMATION);

	context = alcCreateContext ( device, NULL );

	if ( context == NULL )
	{
		LOGGER.log(" Can't create OpenAL context!", io::ELL_ERROR);
		return;
	}

	LOGGER.log(" OAL context created", io::ELL_INFORMATION);

	alcMakeContextCurrent ( context );
	
	// set listener's properties
	moveTo         ( core::vector3df(0.0f, 0.0f, 0.0f) );
	setVelocity    ( core::vector3df(0.0f, 0.0f, 0.0f) );
	setOrientation ( core::vector3df(1.0f, 0.0f, 0.0f), core::vector3df(0.0f, 0.0f, 1.0f) );

	// print renderer information
    const ALubyte* renderer = alGetString(AL_RENDERER);
    const ALubyte* vendor   = alGetString(AL_VENDOR);

    core::stringc logstr;
    logstr = " Sound card model: ";
    logstr.append((const c8*)renderer);
	logstr.append(" ");
    logstr.append((const c8*)vendor);

    if (renderer && vendor)
        LOGGER.log(logstr.c_str(), io::ELL_INFORMATION);

	core::stringc alVer = (c8*)alGetString(AL_VERSION);
    core::stringc alVerStr = " OAL driver version: ";
    alVerStr = alVerStr + alVer;
	LOGGER.log(alVerStr.c_str(), io::ELL_INFORMATION);

	const ALubyte* extentions = alGetString(AL_EXTENSIONS);

	core::stringc glExt = (c8*)extentions;
    core::stringc glExtStr = " Supported OpenAL extentions: \n  ";
    glExtStr=glExtStr+glExt;
    LOGGER.log(glExtStr.c_str(), io::ELL_INFORMATION);

	for (s32 s=0; s<MAX_OAL_SOURCES; s++)
	{
		ALuint source;

		alGenSources ( 1, &source );
		if (source==0)
		{
			logstr.sprintf("Cant create OpenAL %d sound source!", s);
			LOGGER.log(logstr, io::ELL_ERROR);
		}
		else
		{
			CSound::m_SoundSources[s].Source = source; 
		}
	}  

	LOGGER.log("Sound OK", io::ELL_INFORMATION);
}

//---------------------------------------------------------------------

COpenALDriver::~COpenALDriver ()
{
	clearSoundCache();

	for (s32 s=0; s<MAX_OAL_SOURCES; s++)
	{
		alDeleteSources ( 1, &CSound::m_SoundSources[s].Source );

		CSound::m_SoundSources[s].Source=0;
	} 

	alcDestroyContext ( context );
	alcCloseDevice    ( device  );

	LOGGER.log("OpenAL sound deactivated", io::ELL_INFORMATION);
}

//---------------------------------------------------------------------

void COpenALDriver::moveTo ( const core::vector3df& newPos )
{
	CSoundDriver::moveTo(newPos);

	alListenerfv ( AL_POSITION, pos );
}

//---------------------------------------------------------------------

void COpenALDriver::setVelocity ( const core::vector3df& newVel )
{
	CSoundDriver::setVelocity(newVel);

	alListenerfv ( AL_VELOCITY, velocity );
}

//---------------------------------------------------------------------

void COpenALDriver::setOrientation ( const core::vector3df& view, const core::vector3df& up )
{
	CSoundDriver::setOrientation(view, up);

	float	orient [6];

	orient [0] =  viewDir.X;
	orient [1] =  viewDir.Y;
	orient [2] = -viewDir.Z;
	orient [3] =  upDir.X;
	orient [4] =  upDir.Y;
	orient [5] = -upDir.Z;

	alListenerfv ( AL_ORIENTATION, orient );
}

//---------------------------------------------------------------------

bool COpenALDriver::isOk() const
{
	return device != NULL && context != NULL;
}

//------------------------------------------------------------------------

E_SOUND_DRIVER_TYPE COpenALDriver::getSoundDriverType()
{
	return ESDT_OPENAL;
}

//---------------------------------------------------------------------

mm::ISoundDriver* createOpenALDriver()
{
	return new COpenALDriver();
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------
