//|-------------------------------------------------------------------------
//| File:        CSound.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	CSoundHPP
#define	CSoundHPP
//------------------------------------------------------------------------

#include <my_inc.h>
#include <mm/ISound.h>
#include <al.h>
#include <alc.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

const s32 MAX_OAL_SOURCES = 32;

//------------------------------------------------------------------------

class CSound : public ISound
{
	friend class CSoundDriver;

protected:     	

	ALuint      m_Source;          // id's of OpenAL source

	ISoundData	*m_DataSource;
	bool		 m_Looping;
	bool		 m_Streaming;
	ALuint		 m_Format;    	
	ALuint	     m_Buffer[2];		// id's of OpenAL buffers
	s32		     m_CurrentBufferIdx;
	c8          *m_DataBuffer;		// buffer for CSound data
	f32          m_Position[3];		// CSound source position
	f32          m_Velocity[3];		// CSound source m_Velocity
	f32			 m_Pitch;
	f32			 m_Gain;
	f32		     m_Time;
	bool         m_AutoDelete;

	bool _update ();

	void _addSource();
	void _removeSource();

	s32 m_SourceIdx;

public: 

	struct SSoundSourceEntry
	{
		SSoundSourceEntry() : Sound(NULL), Source(0) {}
		SSoundSourceEntry(ALuint source) : Sound(NULL), Source(source) {}

		CSound* Sound;
		ALuint  Source;
	};

	static SSoundSourceEntry m_SoundSources[MAX_OAL_SOURCES];          // id's of OpenAL sources
	static s32 m_SourcesUsed;

	f32 gainToVolume(f32 m_Gain) const;
	f32 volumeToGain(f32 volume) const;

	CSound(ISoundData *data);
	CSound(ISoundData *data, bool theLoop, bool auto_delete);
	virtual ~CSound();

	virtual core::vector3df getPos () const;
	virtual core::vector3df getVelocity () const;
	virtual f32 getGain () const;
	virtual f32 getVolume() const;
	virtual f32 getPitch () const;
	virtual bool isLooping () const;
	virtual bool isPlaying () const;
	virtual bool atEnd     () const;
	virtual bool isAutoDelete() const;	

	virtual void moveTo( const core::vector3df& newPos );
	virtual void moveBy( const core::vector3df& delta  );

	virtual void setVelocity ( const core::vector3df& newVel );
	virtual void setPitch    ( f32 newPitch );
	virtual void setGain     ( f32 newGain  );
	virtual void setVolume   ( f32 newVolume  );
	virtual void setLooping  ( bool flag    );

	virtual void play   ();
	virtual void pause  ();
	virtual void stop   ();
	virtual void rewind ();

	virtual ISoundData* getSoundData();

	enum
	{
		BUFFER_SIZE = 2*32000
	};

	static const f32 SOUND_SPACE_SCALING;
	static const f32 SOUND_SPACE_SCALING_INV;

	struct SSoundDataEntry
	{
		SSoundDataEntry() 
			: FileName(""), SoundData(NULL) {}

		SSoundDataEntry(core::stringc _filename, ISoundData *_sound_data) 
			: FileName(_filename), SoundData(_sound_data) {}

		core::stringc  FileName;
		ISoundData    *SoundData;
	};

	core::list<ISound *>::iterator	    m_SoundIterator;

	static core::list <ISound *>	    Sounds;
	static core::list <SSoundDataEntry>	SoundDatas;

	enum { VOLUME_STEPS = 11 };

	static f64 GainSteps[VOLUME_STEPS+1];

	bool m_AtEnd;
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	CSoundHPP
