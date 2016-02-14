//|-------------------------------------------------------------------------
//| File:        SFog.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SFogHPP
#define SFogHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

#include <img/SColor.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

//! Fog properties
struct SFog
{
    SFog()
	{	
		set(EFT_LINEAR, img::SColor(255,0,0,0), 1400.0f, 1600.0f, 0.01f);
	}

    SFog(E_FOG_TYPE _Type, img::SColor _Color, f32 _Start, f32 _End, f32 _Density)
    {   
        set(_Type, _Color, _Start, _End, _Density );    
    } 
    void set(const SFog &_FogParams)
    {
        set(_FogParams.Type, _FogParams.Color, 
            _FogParams.Start, _FogParams.End, _FogParams.Density            
            );
    }
    void set (
        E_FOG_TYPE _Type, img::SColor _Color, f32 _Start, f32 _End, f32 _Density
        )
    {
		Type   = _Type;
        Color  = _Color;		
        Start  = _Start;
        End    = _End;
        Density= _Density;
    }

	bool operator == (const SFog &other) const
	{
		return !(this->operator != (other));
	}

	bool operator != (const SFog &other) const
	{
		return
			Type != other.Type ||
			Color != other.Color ||
			Start != other.Start ||
			End != other.End ||
			Density != other.Density;
	}

	//! Fog type
	E_FOG_TYPE Type;

    //! Color of the fog
    img::SColor Color;

    //! Only used in linear fog mode. Specifies where fog starts.
    f32 Start;

    //! Only used in linear fog mode. Specifies where fog ends.
    f32 End;

    //! Only used in expotential fog mode. Must be a value between 0 and 1.
    f32 Density;
};

//----------------------------------------------------------------------------
} // end namespace video
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef SFogHPP

