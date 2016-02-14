//|-------------------------------------------------------------------------
//| File:        CShadowFake.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CShadowFakeHPP
#define CShadowFakeHPP
//---------------------------------------------------------------------------

#include <scn/IShadow.h>
#include <vid/SLight.h>
#include <io/ILogger.h>
#include <dev/IDevice.h>
#include <scn/ISceneManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SDynamicLightEntry
{
	enum ELIGHT_ACTION_TYPE
	{
		laNone = 0, laAppearing, laDisappearing
	} 
	ActionType;

	f32 ActionStartTime, ActionEndTime, ActionLifeTime;

	bool CastShadowsNow;
	f32  Distance;
	img::SColor Color;
	bool Updated;
	vid::SLight Light;
	core::vector3df AbsDirection, LocalDirection;
	core::vector3df AbsPosition, LocalPosition;
	u32 Index;

	SDynamicLightEntry()
	{
		LocalDirection.set(0,0,0);
		AbsDirection.set(0,0,0);
		CastShadowsNow = false;
		ActionType = laNone;
		ActionLifeTime =500;
		ActionStartTime = ActionEndTime = -1;
		Index = 0;

		Updated = false;

		Color.set(255, 255, 255, 255);
	}

	SDynamicLightEntry(const vid::SLight& dl)
	{
		Light = dl;

		LocalDirection.set(0,0,0);
		AbsDirection.set(0,0,0);
		CastShadowsNow = false;
		ActionType = laNone;		
		ActionLifeTime = 500;
		ActionStartTime = ActionEndTime = -1;

		Updated = true;

		Color = Light.DiffuseColor.toSColor();
		AbsPosition = Light.Position;
	}

	virtual ~SDynamicLightEntry() {}

	bool operator < (const SDynamicLightEntry& other) const
    { return Light.Type < other.Light.Type; }
};

//---------------------------------------------------------------------------

class CLightShadowGround : public SLightShadow
{
public:

	CLightShadowGround() 
		: m_Light(NULL), m_LightPos(0,0,0), m_LightPosAbs(0,0,0),
		m_LightIdx(0) {}

	virtual E_SHADOW_TYPE getShadowType() { return ESHT_SHADOW_FAKE; }

	void onLightShadowCasted(
		const SDynamicLightEntry &dl, scn::E_SHADOW_TYPE type,
		const scn::ISceneNode *owner, const img::SColor &color)
	{
		SCENE_MANAGER.onLightShadowCasted(dl.Light, type,
			dl.Index, owner, color, m_GroundRect, m_GroundY, m_ZOrder);
	}

	void setLight(const SDynamicLightEntry &dl)
	{
		m_Light			= &dl;
		m_LightPos		= dl.LocalPosition;
		m_LightPosAbs	= dl.AbsPosition;
		m_LightIdx		= dl.Light.Identificator;
	}

	const SDynamicLightEntry	*m_Light;
	core::vector3df				m_LightPos;
	core::vector3df				m_LightPosAbs;
	void						*m_LightIdx;

	core::rectf					m_GroundRect;
	f32							m_GroundY;
	f32							m_ZOrder;
};

//---------------------------------------------------------------------------

//! Scene node for rendering a shadow volume into a stencil buffer.
class CShadowFake : public IShadow
{
public:

    CShadowFake(ISceneNode* parent);
    virtual ~CShadowFake();

    virtual E_SHADOW_TYPE getShadowType()
	{ return ESHT_SHADOW_FAKE; }

	virtual void registerForRendering(
		bool fake_shadow, u32 nowMs,
		const vid::SLight &light0, u64 enabled_lights,
		IAnimatedMesh *amesh, IMesh *mesh, bool render_debug);

	virtual bool isMainPlayerShadow()
	{ return m_Parent ? m_Parent->isMainPlayer() : false; }

protected:

	void _updateShadowGround(
		const SDynamicLightEntry &dl, CLightShadowGround *svp);

	void _registerGeometryFromLight(
		f32 now, SDynamicLightEntry &dlight, bool render_debug);

	core::array <SDynamicLightEntry*> m_DLights;

	void _clearDLights()
	{
		u32 lsize = m_DLights.size();
		for (u32 i=0; i < lsize;)
		{
			SDynamicLightEntry *dl = m_DLights[i];
			if (!dl->Updated || (
					dl->ActionType == SDynamicLightEntry::laNone &&
					!dl->CastShadowsNow))
			{
				m_DLightsHash.erase_value(dl->Light.Identificator);
				m_DLights.erase(i);
				lsize--;
			}
			else
			{
				dl->Updated = false;
				i++;
			}
		}
	}

	bool _addDLight(const vid::SLight &light, u32 idx, f32 action_lifetime = 500)
	{
		if (light.Identificator==0 || !light.CastShadows)
			return false;			
		
		// transform light position into local coord system

		core::vector3df lposabs = light.Position;
		if (light.Type == vid::ELT_DIRECTIONAL)
		{
			lposabs.normalize();
			lposabs = m_TopParentCenter + lposabs * 1000.0f;
		}

		core::vector3df lpos = lposabs;
		m_ParentAbsoluteTransformationInv.transformVect(lpos);

		core::vector3df ldirabs;
		ldirabs =  m_TopParentCenter - lposabs;	

		core::vector3df ldir = ldirabs;
		core::vector3df orig(0,0,0);
		m_ParentAbsoluteTransformationInv.transformVect(ldir);
		m_ParentAbsoluteTransformationInv.transformVect(orig);
		ldir -= orig;
		ldir.normalize();

		SDynamicLightEntry *dl;
		dl = m_DLightsHash.get_value(light.Identificator);
		if (dl)
		{
			dl->Light.Radius	= light.Radius;
			dl->AbsPosition		= lposabs;
			dl->AbsDirection	= ldirabs;
			dl->LocalPosition	= lpos;
			dl->LocalDirection	= ldir;
			dl->Updated			= true;
			dl->Color			= light.DiffuseColor.toSColor();
			dl->Index			= idx;
			return false;
		}

		dl = new SDynamicLightEntry(light);
		dl->ActionLifeTime	= action_lifetime;
		dl->AbsPosition		= lposabs;
		dl->AbsDirection	= ldirabs;
		dl->LocalPosition	= lpos;
		dl->LocalDirection	= ldir;
		dl->Updated			= true;
		dl->Index			= idx;
		m_DLights.push_back(dl);
		m_DLightsHash.set_value(dl->Light.Identificator, dl);
		return true;
	}

	core::hash_table <void*, SDynamicLightEntry*, 32> m_DLightsHash;

	void _applyLights(f32 now, const vid::SLight &light0, u64 enabled_lights);

	virtual void _updateLights(f32 now);

	core::plane3df m_GroundPlane;

	bool m_GroundPlaneValid;

	core::matrix4 m_ParentAbsoluteTransformation;
	core::vector3df m_ParentAbsolutePosition;

	core::aabbox3df m_ParentTransformedBBox, m_TopParentTransformedBBox, m_ParentBBox;
	core::vector3df m_TopParentCenter;

	IBillboardSceneNode			*m_ParentImposter;
	core::matrix4               m_ParentAbsoluteTransformationInv;

	static core::vector3df m_BlurOffsets[3];

	ISceneNode *m_Parent, *m_TopParent;

	core::array <CLightShadowGround*> m_FakeShadowLights;
    s32 m_FakeShadowLightsUsed;

	vid::SMaterial m_Material;

	f32 m_AlphaInterpolation;

	static u32 s_ProfileShadowFake;

	vid::IVideoDriver	&m_Driver;
	dev::IProfiler		&m_Profiler;
	scn::ISceneManager	&m_SceneManager;

	bool m_LightingHighQuality;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CShadowFakeHPP

