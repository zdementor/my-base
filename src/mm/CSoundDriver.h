//|-------------------------------------------------------------------------
//| File:        CSoundDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	CSoundDriverHPP
#define	CSoundDriverHPP
//------------------------------------------------------------------------

#include <my_inc.h>
#include <mm/ISoundDriver.h>
#include <al.h>
#include <alc.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class CSoundDriver : public ISoundDriver
{

protected:

	// listener's position
	f32 pos[3];	

	// listeners velocity (for Doppler effect)
	f32 velocity[3];
	
	// listener's orientation - view and up vectors
	core::vector3df viewDir;
	core::vector3df upDir;

public:

	CSoundDriver  ();
	virtual ~CSoundDriver ();

	virtual bool isOk () const;
	virtual const core::vector3df getPos() const;
	virtual const core::vector3df getVelocity() const;
	virtual const core::vector3df getViewDir() const;
	virtual const core::vector3df getUpDir() const;
	virtual void moveTo( const core::vector3df& newPos );
	virtual void moveBy( const core::vector3df& delta  );
	virtual void setVelocity( const core::vector3df& newVel );
	virtual void setOrientation( const core::vector3df& view, const core::vector3df& up );	

	virtual ISound* cloneSound( ISound* snd );

	virtual ISound* getSound( const char * fileName, bool loop, bool auto_delete=false );

	//! looks for the file name of sound if it's already loaded
	virtual core::stringc findSoundFileName(ISound* snd_ptr);

    //! looks for the full file name of sound if it's already loaded
    virtual core::stringc findSoundFullFileName(ISound* snd_ptr);

	virtual void clearUnusedSoundCache();	
	virtual void clearSoundCache();	

	virtual void updateSounds();

	virtual s32 getPlayingSoundsCount();
	virtual s32 getSoundsCount();

	virtual E_SOUND_DRIVER_TYPE getSoundDriverType();

private:

	ISoundData* loadSoundDataFromFile( const char * fileName, bool loop );
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	CSoundDriverHPP

