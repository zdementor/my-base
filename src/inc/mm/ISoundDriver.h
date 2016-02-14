//|-------------------------------------------------------------------------
//| File:        ISoundDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	ISoundDriverHPP
#define	ISoundDriverHPP
//------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class __MY_MM_LIB_API__ ISoundDriver :
	public IUnknown, public ISingleton <ISoundDriver>
{

public:

	virtual bool isOk () const = 0;
	virtual const core::vector3df getPos() const = 0;
	virtual const core::vector3df getVelocity() const = 0;
	virtual const core::vector3df getViewDir() const = 0;
	virtual const core::vector3df getUpDir() const = 0;

	virtual void moveTo(const core::vector3df& newPos ) = 0;
	virtual void moveBy(const core::vector3df& delta ) = 0;
	virtual void setVelocity(const core::vector3df& newVel) = 0;
	virtual void setOrientation(
		const core::vector3df& view, const core::vector3df& up) = 0;
	
	virtual ISound* cloneSound( ISound* snd ) = 0;

	virtual ISound* getSound(
		const char * fileName, bool loop, bool auto_delete=false) = 0;

	//! looks for the file name of sound if it's already loaded
	virtual core::stringc findSoundFileName(ISound* snd_ptr) = 0;

    //! looks for the full file name of sound if it's already loaded
    virtual core::stringc findSoundFullFileName(ISound* snd_ptr) = 0;
	
	virtual void clearUnusedSoundCache() = 0;

	virtual void updateSounds() = 0;

	virtual s32 getPlayingSoundsCount() = 0;
	virtual s32 getSoundsCount() = 0;

	virtual E_SOUND_DRIVER_TYPE getSoundDriverType() = 0;

	//!	Return singleton object
	static ISoundDriver& getSingleton();

	//! Return pointer to singleton object
	static ISoundDriver* getSingletonPtr();	
};

//------------------------------------------------------------------------

__MY_MM_LIB_API__ mm::ISoundDriver* createSoundDriver(
	E_SOUND_DRIVER_TYPE sound_type);

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	ISoundDriverHPP

