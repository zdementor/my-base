//|-------------------------------------------------------------------------
//| File:        CSound.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSound.h"
#include "CSoundData.h"
#include "CSoundDriver.h"
#include <mm/ISoundData.h>
#include <io/ILogger.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

CSound::SSoundSourceEntry CSound::m_SoundSources[MAX_OAL_SOURCES];          // id's of OpenAL sources
s32 CSound::m_SourcesUsed=0;

const f32 CSound::SOUND_SPACE_SCALING = 0.01f;
const f32 CSound::SOUND_SPACE_SCALING_INV = 1.0f / SOUND_SPACE_SCALING;

core::list <ISound *> CSound::Sounds;
core::list <CSound::SSoundDataEntry> CSound::SoundDatas;

f64 CSound::GainSteps[VOLUME_STEPS+1] = 
{
	0.0009765625,
	0.001953125,		
	0.00390625,
	0.0078125,		
	0.015625,
	0.03125,	
	0.0625,
	0.125,
	0.25,
	0.5,
	1.0,
	2.0
};

//------------------------------------------------------------------------

CSound::CSound (ISoundData * data)
: m_Time(0.0f), m_AtEnd(false), m_SoundIterator(0),
m_Looping(false), m_AutoDelete(false), m_Source(0), m_DataBuffer(NULL), m_SourceIdx(-1)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSound");
#endif

	m_DataSource = data;

	m_Buffer[0]=0;
	m_Buffer[1]=0;

	m_DataSource->grab();

	Sounds.push_back( this );
	m_SoundIterator = Sounds.get_last();
}

//------------------------------------------------------------------------

CSound::CSound ( ISoundData * data, bool theLoop, bool auto_delete ) 
: m_Time(0.0f), m_AtEnd(false), m_SoundIterator(0),
m_Looping(theLoop), m_AutoDelete(auto_delete), m_Source(0), m_DataBuffer(NULL), m_SourceIdx(-1)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CSound");
#endif

	m_DataSource = data;

	m_Buffer[0]=0;
	m_Buffer[1]=0;

	m_Position[0]=0;
	m_Position[1]=0;
	m_Position[2]=0;

	m_Velocity[0]=0;
	m_Velocity[1]=0;
	m_Velocity[2]=0;

	setVolume(1.0f);
	setPitch (1.0f);

	static core::stringc msg;

	if ( m_DataSource == NULL )
	{
		msg.sprintf("Cant create OpenAL sound from null sound data!");
		LOGGER.log(msg, io::ELL_ERROR);

		return;
	}

	m_DataSource->seek(0);

	m_Streaming  = true;

	if ( m_DataSource -> getNumChannels () == 2 )		
	// stereo
	{
		if ( m_DataSource -> getBitsPerSample () == 16 )
			m_Format = AL_FORMAT_STEREO16;
		else
			m_Format = AL_FORMAT_STEREO8;
	}
	else
	// mono
	{
		if ( m_DataSource -> getBitsPerSample () == 16 )
			m_Format = AL_FORMAT_MONO16;
		else
			m_Format = AL_FORMAT_MONO8;
	}

	if ( m_DataSource -> getSize () < BUFFER_SIZE)
		m_Streaming = false;

	if ( m_Streaming )
	{
		m_DataBuffer = new c8 [BUFFER_SIZE];

		alGenBuffers ( 2, m_Buffer  );
		if (m_Buffer[0]==0 || m_Buffer[1]==0)
		{
			msg.sprintf("Cant create OpenAL sound buffer for '%s'!", m_DataSource->getFileName());
			LOGGER.log(msg, io::ELL_ERROR);
		}
	}
	else
	{
		alGenBuffers( 1, m_Buffer );
		if (m_Buffer[0]==0)
		{
			msg.sprintf("Cant create OpenAL sound buffer for '%s'!", m_DataSource->getFileName());
			LOGGER.log(msg, io::ELL_ERROR);
		}

		c8* buff = new c8 [m_DataSource->getSize()];	

		if (m_Buffer[0]!=0)
		{
			int size = m_DataSource->read ( buff );
			alBufferData( m_Buffer[0], m_Format, buff, size, m_DataSource -> getFrequency () );
		}

		delete buff;
	}

	m_Time = m_DataSource->getTime();	

	m_DataSource->grab();

	Sounds.push_back( this );
	m_SoundIterator = Sounds.get_last();
}

//----------------------------------------------------------------------------------

CSound :: ~CSound ()
{
	SAFE_DELETE(m_DataBuffer);

	if ( m_Streaming )
		if (m_Buffer[0]!=0 && m_Buffer[1]!=0)
			alDeleteBuffers ( 2, m_Buffer );
	else
		if (m_Buffer[0]!=0)
			alDeleteBuffers ( 1, m_Buffer );

	SAFE_DROP(m_DataSource);

	_removeSource();

	if (m_SoundIterator!=0) 
	{
		Sounds.erase(m_SoundIterator);
		m_SoundIterator=0;
	} 	
}

//----------------------------------------------------------------------------------

void CSound::_addSource()
{
	if (!m_Source && m_SourcesUsed<MAX_OAL_SOURCES)
	{   
		m_SourceIdx=m_SourcesUsed; 

		m_SoundSources[m_SourceIdx].Sound = this;
		m_Source = m_SoundSources[m_SourceIdx].Source;
		
		m_SourcesUsed++;   
	}
}

//----------------------------------------------------------------------------------

void CSound::_removeSource()
{
	if (m_Source && m_SourcesUsed>0)
	{     
		s32 last_idx = m_SourcesUsed-1;

		m_SoundSources[m_SourceIdx].Sound  = m_SoundSources[last_idx].Sound;
		m_SoundSources[m_SourceIdx].Source = m_SoundSources[last_idx].Source;

		m_SoundSources[m_SourceIdx].Sound->m_SourceIdx = m_SourceIdx;    
		m_SoundSources[last_idx].Sound  = 0;
		m_SoundSources[last_idx].Source = m_Source;

		m_Source = 0;  
		m_SourceIdx=-1;

		m_SourcesUsed--;     		
	}
}

//----------------------------------------------------------------------------------

core::vector3df CSound::getPos() const
{
	return core::vector3df(-m_Position[0], m_Position[1], -m_Position[2]) * SOUND_SPACE_SCALING_INV;
}

//----------------------------------------------------------------------------------

core::vector3df CSound::getVelocity() const
{
	return core::vector3df(m_Velocity[0], m_Velocity[1], m_Velocity[2]);
}

//----------------------------------------------------------------------------------

f32	CSound::getGain () const
{
	return m_Gain;
}

//----------------------------------------------------------------------------------

f32	CSound::getVolume() const
{
	return gainToVolume( getGain() );
}

//----------------------------------------------------------------------------------

f32 CSound::getPitch () const
{
	return m_Pitch;
}

//----------------------------------------------------------------------------------

bool CSound::isLooping () const
{
	return m_Looping;
}

//----------------------------------------------------------------------------------

bool CSound::isPlaying () const
{
	return m_Source!=0;
}

//----------------------------------------------------------------------------------
			
bool CSound :: atEnd () const
{
	return m_AtEnd;	
}

//----------------------------------------------------------------------------------

bool CSound::isAutoDelete() const
{
	return m_AutoDelete;
}

//----------------------------------------------------------------------------------

void CSound :: moveTo ( const core::vector3df& newPos )
{
	m_Position[0] = -newPos.X * SOUND_SPACE_SCALING;
	m_Position[1] =  newPos.Y * SOUND_SPACE_SCALING;
	m_Position[2] = -newPos.Z * SOUND_SPACE_SCALING;

	if (m_Source==0)
		return;

	alSourcefv ( m_Source, AL_POSITION, m_Position );
}

//----------------------------------------------------------------------------------

void CSound::moveBy ( const core::vector3df& delta )
{
	core::vector3df newpos = getPos() + delta;

	moveTo(newpos);
}

//----------------------------------------------------------------------------------

void CSound::setVelocity ( const core::vector3df& newVel )
{
	m_Velocity[0] = newVel.X;
	m_Velocity[1] = newVel.Y;
	m_Velocity[2] = newVel.Z;

	if (m_Source==0)
		return;

	alSourcefv ( m_Source, AL_VELOCITY, m_Velocity );
}

//----------------------------------------------------------------------------------

void CSound::setPitch ( float newPitch )
{
	m_Pitch = newPitch;

	if (m_Source==0)
		return;

	alSourcef ( m_Source, AL_PITCH, m_Pitch );
}

//----------------------------------------------------------------------------------

void CSound::setGain ( float newGain )
{
	m_Gain = newGain;

	if (m_Source==0)
		return;

	alSourcef ( m_Source, AL_GAIN, m_Gain );
}

//----------------------------------------------------------------------------------

void CSound::setVolume( float newVolume )
{
	setGain( volumeToGain( newVolume ) );
}

//----------------------------------------------------------------------------------

void CSound::setLooping ( bool flag )
{
	m_Looping = flag;

	if (m_Source==0)
		return;

	if (!m_Streaming)
		alSourcei ( m_Source, AL_LOOPING, m_Looping ? AL_TRUE : AL_FALSE );
}

//----------------------------------------------------------------------------------

void CSound::play()
{
	if (m_Source==0 && m_DataSource)
	{		 
		_addSource();

		if (m_Source)
		{
			m_AtEnd = false;

			if ( m_Streaming )
			{	  
				m_CurrentBufferIdx = 0;

				if (m_Buffer[0]!=0 && m_Buffer[1]!=0)
				{
					int size = 0;

					size = m_DataSource -> read ( m_DataBuffer, BUFFER_SIZE );
					alBufferData( m_Buffer[0], m_Format, m_DataBuffer, size, m_DataSource -> getFrequency () );

					size = m_DataSource -> read ( m_DataBuffer, BUFFER_SIZE );
					alBufferData( m_Buffer[1], m_Format, m_DataBuffer, size, m_DataSource -> getFrequency () );	
				}

				if (m_Buffer[0]!=0 && m_Buffer[1]!=0)
					alSourceQueueBuffers( m_Source, 2, m_Buffer );	
			}
			else
			{
				if (m_Buffer[0]!=0)
					alSourcei( m_Source, AL_BUFFER, m_Buffer[0] );	
			}

			moveTo     (getPos()     );
			setVelocity(getVelocity());
			setPitch   (getPitch()   );
			setVolume  (getVolume()  );
			setLooping (isLooping()  );
		}
	}

	if (m_Source)
	{
		alSourcePlay ( m_Source );
	}
}

//----------------------------------------------------------------------------------

void CSound :: pause()
{
	if (m_Source==0)
		return;

	alSourcePause( m_Source );
}

//----------------------------------------------------------------------------------

void CSound::rewind()
{
	if (m_Source==0)
		return;

	m_AtEnd = false;

	m_DataSource->seek(0);

	alSourceRewind ( m_Source );
}

//----------------------------------------------------------------------------------

void CSound::stop ()
{
	if (m_Source==0)
		return;

	alSourceStop ( m_Source );

	m_Time=0;	

	if ( m_Streaming )
	{
		ALint queued;

		alGetSourcei ( m_Source,AL_BUFFERS_QUEUED,&queued );

		if ( queued > 0 )
			alSourceUnqueueBuffers ( m_Source, 2, m_Buffer );

		m_CurrentBufferIdx = 0;
	}

	_removeSource();

	m_AtEnd = true;
}

//----------------------------------------------------------------------------------

bool CSound::_update()
{
	if (m_Source==0 || m_AtEnd)
		return false;

	if ( m_Streaming )
	{
		ALint state;
		alGetSourcei( m_Source,  AL_SOURCE_STATE, &state );

		if (state!=AL_PLAYING)
			alSourcePlay ( m_Source );

		m_DataSource->seek(m_Time);

		ALint processed;

		alGetSourcei( m_Source, AL_BUFFERS_PROCESSED, &processed );

		if ( processed == 0 )
		{
			return true;
		}

		if ( processed == 1 )
		{
			alSourceUnqueueBuffers ( m_Source, 1, &m_Buffer [m_CurrentBufferIdx] );

			int size = m_DataSource->read( m_DataBuffer, BUFFER_SIZE );			

			if ( size > 0 || (size == 0 && m_Looping) )
			{
				alBufferData( 
					m_Buffer [m_CurrentBufferIdx], m_Format, m_DataBuffer, size,
				    m_DataSource -> getFrequency () 
					);

				alSourceQueueBuffers( m_Source, 1, &m_Buffer [m_CurrentBufferIdx] );

				if (size<BUFFER_SIZE && m_Looping)
				{
					m_DataSource -> seek( 0 );
				}
			}
			else
			{
				int queued;

				alGetSourcei( m_Source, AL_BUFFERS_QUEUED, &queued );

				if (queued==0)
				{
					m_DataSource->seek(0);
				}
			}

			m_CurrentBufferIdx = 1 - m_CurrentBufferIdx;
		}
		else if ( processed==2)
		{
			alSourceUnqueueBuffers ( m_Source, 2, m_Buffer );

			m_CurrentBufferIdx = 0;

			int		size;
			
			size = m_DataSource -> read ( m_DataBuffer, BUFFER_SIZE );
			alBufferData ( m_Buffer [0], m_Format, m_DataBuffer, size, m_DataSource -> getFrequency () );

			size = m_DataSource -> read ( m_DataBuffer, BUFFER_SIZE );		
			alBufferData ( m_Buffer [1], m_Format, m_DataBuffer, size, m_DataSource -> getFrequency () );

			alSourceQueueBuffers ( m_Source, 2, m_Buffer );				
		}

		m_Time = m_DataSource->getTime();	
		m_AtEnd = m_DataSource->atEnd();
	}
	else
	{
		ALint state;
		alGetSourcei( m_Source,  AL_SOURCE_STATE, &state );

		if (state!=AL_PLAYING)
			m_AtEnd = true;
	}

	if (m_AtEnd)
	{
		_removeSource();

		return false;
	}

	return true;
}

//------------------------------------------------------------------------

f32 CSound::gainToVolume(f32 m_Gain) const
{
	f32 volume = 0.0f;

	s32 s=0;

	for (; s<VOLUME_STEPS; s++)
	{
		if (GainSteps[s]<=m_Gain && m_Gain<GainSteps[s+1])
		{
			volume = 1.0f * ( f32(s)/f32(VOLUME_STEPS) );

			break;
		}
	}

	if (s>=VOLUME_STEPS)
		volume = 1.0f;

	return volume;
}

//------------------------------------------------------------------------

f32 CSound::volumeToGain(f32 volume) const
{
	s32 s = volume * f32(VOLUME_STEPS);

	CHECK_RANGE(s, 0, VOLUME_STEPS);

	return (f32)GainSteps[s];
}

//------------------------------------------------------------------------

ISoundData* CSound::getSoundData()
{
	return m_DataSource;
}

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------
