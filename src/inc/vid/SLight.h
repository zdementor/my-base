//|-------------------------------------------------------------------------
//| File:        SLight.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SLightHPP
#define SLightHPP
//---------------------------------------------------------------------------

#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! structure for holding data describing a dynamic point light.
//! ambient light and point lights are the only light supported 
//! by the irrlicht engine.
struct SLight
{
    SLight() : Type(ELT_POINT),
		AmbientColor(0.0f,0.0f,0.0f), DiffuseColor(1.0f, 1.0f, 1.0f), 
        SpecularColor(1.0f,1.0f,1.0f), Position(0.0f, 0.0f, 0.0f), Radius(100.0f),
        CastShadows(true), Identificator(0), Enabled(false), Dynamic(false),
		IntensityWaveEnabled(false), ScreenPosition(0, 0)
    {}

    SLight(const SLight &other)
    { set(other); }

	virtual ~SLight() {}

	void set(const SLight &other)
	{ (*this) = other; }

    //! Ambient color emitted by the light
    img::SColorf AmbientColor; 

    //! Diffuse color emitted by the light. This is the primary color you
    //! might want to set.
    img::SColorf DiffuseColor; 

    //! Specular color emitted by the light. 
    img::SColorf SpecularColor; 

    //! Position of the light in the world space
    core::vector3df Position; 

    //! Radius of light. Everything within this radius be be lighted.
    f32 Radius; 

    //! Does the light cast shadows?
    bool CastShadows;

	//! Unique Identificator
	void* Identificator;

	//! Type of the light. Default: ELT_POINT
    E_LIGHT_TYPE Type;

	//! Enabled/Disabled Flag
	bool Enabled;

	//! Dynamic light if true, or static if false
	bool Dynamic;

	//! bounding box (in the world space)
	core::aabbox3df BoundingBox;

	//! Lighting intesity wave
	core::wave IntensityWave;

	//! Enable/Disable intensity changing
	bool IntensityWaveEnabled;

	//! ligted volume (in the world space)
	core::aabbox3df LightedVolume;
	core::vector3df LightedVolumeCenter, LightedVolumeSize;	

	//! position in screen space
	core::vector2di ScreenPosition;

	//! screen rectangle lighted with this light (depends on LightedVolume)
	core::recti ScreenRect;

	bool operator == ( const SLight & other) const
	{ return !((*this) != other); }

	bool operator != ( const SLight & other ) const
	{
		return (this != &other) ||
			AmbientColor != other.AmbientColor ||
			DiffuseColor != other.DiffuseColor || 
			SpecularColor != other.SpecularColor ||
			Dynamic != other. Dynamic ||
			Position != other.Position ||
			Radius != other.Radius ||
			CastShadows != other.CastShadows ||
			Type != other.Type ||
			BoundingBox != other.BoundingBox;
	}

	bool operator < ( const SLight & other ) const
	{
		if (Type != other.Type)
			return Type < other.Type;
		if (Radius != other.Radius)
			return Radius < other.Radius;
		return (*((u32*)this) < *((u32*)&other));
	}
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef SLightHPP

