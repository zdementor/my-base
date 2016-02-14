//|-------------------------------------------------------------------------
//| File:        SGameSceneProps.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SGameScenePropsHPP
#define SGameScenePropsHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/SFog.h>
#include <vid/SLight.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------
	
struct SSceneProps
{
	SSceneProps()
	{
		LODDistances[scn::ELL_LOD_NEAREST] = 600.0f;
		LODDistances[scn::ELL_LOD_NEAR]    = 800.0f;
		LODDistances[scn::ELL_LOD_FAR]     = 1400.0f;
		LODDistances[scn::ELL_LOD_FARTHEST]= 1000000.0f;
	}

	SSceneProps(const SSceneProps &p)
	{
		set(p);
	}

	inline SSceneProps& operator=(const SSceneProps& p)
	{
		set(p);

		return *this;
	} 

	inline void set(const SSceneProps& p)
	{
		for (s32 ld=0; ld<scn::E_LOD_LEVEL_COUNT; ld++)
			LODDistances[ld] = p.LODDistances[ld];
	}

	f32 LODDistances[scn::E_LOD_LEVEL_COUNT];
};

//----------------------------------------------------------------------------

struct SVideoProps
{
	SVideoProps() :
		GroundFogColor(255,255,255,255), ShadowColor(90,0,0,0)
	{
		GlobalAmbientColor = img::SColorf(img::SColor(255, 128, 128, 128));

		GlobalLight.Type = vid::ELT_DIRECTIONAL;
		GlobalLight.AmbientColor.set (0.4f, 0.4f, 0.4f);
		GlobalLight.DiffuseColor.set (0.6f, 0.6f, 0.6f);
		GlobalLight.SpecularColor.set(0.0f, 0.0f, 0.0f);
		GlobalLight.Position.set     (-1.000000, 1000.000000, -1.000000); 
		GlobalLight.CastShadows = false;
		GlobalLight.Radius = 10000;
		GlobalLight.Enabled = true;

		DistanceFog.set(vid::EFT_LINEAR, img::SColor(255,0,0,0), 2500.0f, 5000.0f, 0.01f);
	}

	SVideoProps(const SVideoProps &p)
	{
		set(p);
	}

	inline SVideoProps& operator=(const SVideoProps& p)
	{
		set(p);

		return *this;
	} 

	inline void set(const SVideoProps& p)
	{
		ShadowColor			= p.ShadowColor;
		DistanceFog			= p.DistanceFog;
		GroundFogColor		= p.GroundFogColor;
		GlobalAmbientColor	= p.GlobalAmbientColor;
		GlobalLight			= p.GlobalLight;
	}

	img::SColor		ShadowColor;
	vid::SFog		DistanceFog;
	img::SColor		GroundFogColor;
	img::SColorf	GlobalAmbientColor;
	vid::SLight		GlobalLight;
};

//----------------------------------------------------------------------------

struct SGameSceneProps
{
	SGameSceneProps()
	{
	}

	SGameSceneProps(const SGameSceneProps &p)
	{
		set(p);
	}

	inline SGameSceneProps& operator=(const SGameSceneProps& p)
	{
		set(p);

		return *this;
	} 

	inline void set(const SGameSceneProps& p)
	{
		Scene = p.Scene;
		Video = p.Video;
	}

	SSceneProps	Scene;  
	SVideoProps Video;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef SGameScenePropsHPP