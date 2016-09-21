//|-------------------------------------------------------------------------
//| File:        CSoundDriver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSoundDriver.h"
#include "CSoundDataOgg.h"
#include "CSoundDataWav.h"
#include "CSound.h"
#include <io/ILogger.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

mm::ISoundDriver* createOpenALDriver();

//------------------------------------------------------------------------

CSoundDriver::CSoundDriver() 
{	
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSoundDriver");
#endif
}

//---------------------------------------------------------------------

CSoundDriver::~CSoundDriver ()
{
	clearSoundCache();
}

//---------------------------------------------------------------------

void CSoundDriver::moveTo ( const core::vector3df& newPos )
{
	pos[0] = -newPos.X * CSound::SOUND_SPACE_SCALING;
	pos[1] =  newPos.Y * CSound::SOUND_SPACE_SCALING;
	pos[2] = -newPos.Z * CSound::SOUND_SPACE_SCALING;
}

//---------------------------------------------------------------------

void CSoundDriver::moveBy ( const core::vector3df& delta  )
{
	core::vector3df newpos = getPos() + delta;

	moveTo ( newpos );
}

//---------------------------------------------------------------------

void CSoundDriver::setVelocity ( const core::vector3df& newVel )
{
	velocity[0] = newVel.X;
	velocity[1] = newVel.Y;
	velocity[2] = newVel.Z;
}

//---------------------------------------------------------------------

void CSoundDriver::setOrientation ( const core::vector3df& view, const core::vector3df& up )
{
	viewDir = view;
	upDir = up;
}

//---------------------------------------------------------------------

void CSoundDriver::updateSounds ()
{   
	for (s32 s=0; s<CSound::m_SourcesUsed;)
	{
		s32 wasUsed = CSound::m_SourcesUsed;
		CSound* sound = CSound::m_SoundSources[s].Sound;

		sound->_update();

		if (sound->atEnd())
		{
			if (sound->isAutoDelete())
				sound->drop();
		}
		if (wasUsed == CSound::m_SourcesUsed)
			s++;
	}
}

//---------------------------------------------------------------------

ISound* CSoundDriver::cloneSound( ISound* snd )
{
	if (!snd)
		return NULL;
	
	ISoundData* snddata = (snd->getSoundData());	

	if (snddata && snddata->isOk ())
	{
		if (getSoundDriverType() == mm::ESDT_OPENAL)
		{
			return new CSound ( snddata, snd->isLooping(), snd->isAutoDelete() );	
		}
		else
		{
			return new CSound (snddata);	
		}
	}

	return NULL;
}

//---------------------------------------------------------------------

ISound* CSoundDriver::getSound( const char * fileName, bool loop, bool auto_delete )
{
	ISoundData *snd = NULL;	
	
	core::list<CSound::SSoundDataEntry>::iterator it = CSound::SoundDatas.begin();

	for (; it != CSound::SoundDatas.end (); ++it )
	{
		if ((*it).FileName == core::stringc(fileName))
		{
			snd = (*it).SoundData;
			break;
		}			
	}
		
	if (snd==NULL)
	{		
		snd = loadSoundDataFromFile(fileName, loop);

		if (snd && snd->isOk ())
		{
			CSound::SoundDatas.push_back(
				CSound::SSoundDataEntry(core::stringc(fileName), snd)
				);
		}
	}
		
	if ( snd == NULL )
		return NULL;
		
	if ( snd -> isOk () )
	{
		if ( getSoundDriverType()==mm::ESDT_OPENAL )
		{
			return new CSound ( snd, loop, auto_delete );	
		}
		else
		{
			return new CSound (snd);	
		}
	}
		
	SAFE_DROP(snd);
	
	return NULL;	
}

//---------------------------------------------------------------------

ISoundData* CSoundDriver::loadSoundDataFromFile( const char * fileName, bool loop )
{
	const c8 * ext = strrchr ( fileName, '.' );
	
	ISoundData * snd = NULL;	

	if ( ext && !strcmp ( ext + 1, "ogg" ) )
		snd = new CSoundDataOgg ( fileName );
	else
	if ( ext && !strcmp ( ext + 1, "wav" ) )
		snd = new CSoundDataWav ( fileName );

	if (snd && snd->isOk())
	{	
		c8 ch[255];	
			
		s32 numChannels   = snd->getNumChannels();
		s32 frequency     = snd->getFrequency(); 
		s32 bitsPerSample = snd->getBitsPerSample();

		sprintf(
			ch, "%s (%d channels, %d freq, %d bps)",
			fileName, numChannels, frequency, bitsPerSample
			);

		LOGGER.log("Loaded sound", ch, io::ELL_INFORMATION);
	}
	else
	{
		SAFE_DROP(snd);
		LOGGER.log("Could not load sound data", fileName, io::ELL_ERROR);
	}

	return snd;
}

//---------------------------------------------------------------------

//! looks for the file name of sound if it's already loaded
core::stringc CSoundDriver::findSoundFileName(ISound* snd_ptr)
{
	core::stringc filename = findSoundFullFileName(snd_ptr);

	core::stringc fname =
		core::extractFileName( filename );

	filename = fname;

	if (filename.size()==0) filename=NONAME_FILE_NAME;

    return filename;
}

//---------------------------------------------------------------------

//! looks for the full file name of sound if it's already loaded
core::stringc CSoundDriver::findSoundFullFileName(ISound* snd_ptr)
{
	core::stringc filename=NONAME_FILE_NAME;

	if (!snd_ptr)
		return filename;

	ISoundData *snd_data = snd_ptr->getSoundData();

	if (!snd_data)
		return filename;

	core::list<CSound::SSoundDataEntry>::iterator it = CSound::SoundDatas.begin ();

	for (; it != CSound::SoundDatas.end (); ++it)
	{
		CSound::SSoundDataEntry &s_data = *it;

		if (s_data.SoundData == snd_data)
		{
			filename = s_data.FileName.c_str();

			break;
		}
	}

	filename.make_lower();

	if (filename.size()==0) filename=NONAME_FILE_NAME;

    return filename;
}

//---------------------------------------------------------------------

void CSoundDriver::clearUnusedSoundCache()
{
	s32 cleared_sounds = 0, cleared_snd_data = 0;		
	s32 all_sounds = CSound::Sounds.size(), all_sound_datas = CSound::SoundDatas.size();

	core::list<ISound *>::iterator it = CSound::Sounds.begin();

	for (; it != CSound::Sounds.end (); )
	{
		ISound *snd = *it;

		if ( snd && snd->getReferenceCounter()==1 )
		{
			if (!snd->drop())
			{
				c8 ch[255];
				sprintf(ch, "Can't drop CSound %p", snd);
				LOGGER.log(ch, io::ELL_ERROR);
			}
			else
			{
				cleared_sounds++;

				it = CSound::Sounds.begin();
			}			
		}
		else
		{
			++it;
		}
	}

	core::list<CSound::SSoundDataEntry>::iterator itsdata = CSound::SoundDatas.begin();

	for (; itsdata != CSound::SoundDatas.end (); )
	{
		if ( (*itsdata).SoundData->getReferenceCounter()==1 )
		{
			SAFE_DROP((*itsdata).SoundData);
			CSound::SoundDatas.erase(itsdata);

			cleared_snd_data++;
		}
		else
		{
			++itsdata;
		}
	}

	if (cleared_sounds>0 || cleared_snd_data>0)
	{
		core::stringc msg;

		msg.sprintf("Unused sound cache (%d sounds, %d sound data)(from %d, %d at all) has been cleared.", cleared_sounds, cleared_snd_data, all_sounds, all_sound_datas);
		LOGGER.log(msg.c_str());
	}
}

//---------------------------------------------------------------------

void CSoundDriver::clearSoundCache()
{
	s32 cleared_sounds = 0, cleared_snd_data = 0;

	core::list<ISound *>::iterator it = CSound::Sounds.begin();

	for (; it != CSound::Sounds.end (); )
	{
		ISound *snd = *it;

		if (snd)
		{
			if (!snd->drop())
			{
				c8 ch[255];
				sprintf(ch, "Can't drop CSound %p", snd);
				LOGGER.log(ch, io::ELL_ERROR);
			}
			else
			{
				cleared_sounds++;

				it = CSound::Sounds.begin();
			}			
		}
		else
		{
			++it;
		}
	}

	if (CSound::Sounds.size()!=0)
	{
		LOGGER.log("Not all Sounds deleted!", io::ELL_ERROR);
	}

	core::list<CSound::SSoundDataEntry>::iterator itsdata = CSound::SoundDatas.begin();

	for (; itsdata != CSound::SoundDatas.end (); ++itsdata)
	{
		if ( (*itsdata).SoundData->getReferenceCounter()==1 )
		{
			SAFE_DROP((*itsdata).SoundData);

			cleared_snd_data++;
		}
	}

	CSound::SoundDatas.clear();		

	if (cleared_sounds>0 || cleared_snd_data>0)
	{
		core::stringc msg;

		msg.sprintf("Sound cache (%d sounds, %d sound data) has been cleared.", cleared_sounds, cleared_snd_data);
		LOGGER.log(msg.c_str());
	} 
}

//---------------------------------------------------------------------

bool CSoundDriver::isOk() const
{
	return false;
}

//---------------------------------------------------------------------

const core::vector3df CSoundDriver::getPos() const
{
	return core::vector3df(-pos[0], pos[1], -pos[2]) * CSound::SOUND_SPACE_SCALING_INV;
}

//---------------------------------------------------------------------

const core::vector3df CSoundDriver::getVelocity () const
{
	return core::vector3df(velocity[0], velocity[1], velocity[2]);
}

//---------------------------------------------------------------------

const core::vector3df CSoundDriver::getViewDir () const
{
	return viewDir;
}

//---------------------------------------------------------------------

const core::vector3df CSoundDriver::getUpDir () const
{
	return upDir;
}

//------------------------------------------------------------------------

s32 CSoundDriver::getPlayingSoundsCount()
{
	return CSound::m_SourcesUsed;
}

//------------------------------------------------------------------------

s32 CSoundDriver::getSoundsCount()
{
	return CSound::Sounds.size();
}

//------------------------------------------------------------------------

E_SOUND_DRIVER_TYPE CSoundDriver::getSoundDriverType()
{
	return ESDT_NULL;
}

//------------------------------------------------------------------------

__MY_EXPORT__ mm::ISoundDriver* createSoundDriver(E_SOUND_DRIVER_TYPE sound_type)
{
	if (sound_type == mm::ESDT_OPENAL)
	{
		mm::ISoundDriver *snddrv = createOpenALDriver();
		if (snddrv->isOk())
			return snddrv;
		snddrv->drop();
	}

	// ESDT_NULL
	return new CSoundDriver();
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------
