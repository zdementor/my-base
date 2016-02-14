//|-------------------------------------------------------------------------
//| File:        ISound.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	ISoundHPP
#define	ISoundHPP
//------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class __MY_MM_LIB_API__ ISound : public IUnknown
{

public:

	virtual core::vector3df getPos() const = 0;
	virtual core::vector3df getVelocity() const = 0;
	virtual f32 getGain() const = 0;
	virtual f32 getVolume() const = 0;
	virtual f32 getPitch() const = 0;
	virtual bool isLooping() const = 0;
	virtual bool isPlaying() const = 0;
	virtual bool atEnd() const = 0;	
	virtual bool isAutoDelete() const = 0;	

	virtual void moveTo( const core::vector3df& newPos ) = 0;
	virtual void moveBy( const core::vector3df& delta  ) = 0;

	virtual void setVelocity ( const core::vector3df& newVel ) = 0;
	virtual void setPitch    ( f32 newPitch ) = 0;
	virtual void setGain     ( f32 newGain  ) = 0;
	virtual void setVolume   ( f32 newVolume) = 0;
	virtual void setLooping  ( bool flag    ) = 0;

	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual void rewind () = 0;

	virtual ISoundData* getSoundData() = 0;
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	ISoundHPP
