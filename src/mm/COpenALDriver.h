//|-------------------------------------------------------------------------
//| File:        COpenALDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef	COpenALDriverHPP
#define	COpenALDriverHPP
//------------------------------------------------------------------------

#include "CSoundDriver.h"
#include <al.h>
#include <alc.h>

//------------------------------------------------------------------------
namespace my {
namespace mm {
//------------------------------------------------------------------------

class COpenALDriver : public CSoundDriver
{

protected:

	// device & context for working with OpenAL
	ALCdevice  * device;					
	ALCcontext * context;

public:

	COpenALDriver  ();
	virtual ~COpenALDriver ();	

	virtual void moveTo( const core::vector3df& newPos );
	virtual void setVelocity( const core::vector3df& newVel );
	virtual void setOrientation( const core::vector3df& view, const core::vector3df& up );	

	virtual bool isOk () const;

	virtual E_SOUND_DRIVER_TYPE getSoundDriverType();
};

//------------------------------------------------------------------------
} // end namespace mm
} // end namespace my
//------------------------------------------------------------------------

#endif // #ifndef	COpenALDriverHPP

