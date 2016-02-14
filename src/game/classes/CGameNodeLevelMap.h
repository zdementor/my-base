//|-------------------------------------------------------------------------
//| File:        CGameNodeLevelMap.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeLevelMapHPP
#define CGameNodeLevelMapHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeLevelMap.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SLevelMapToLoadEntry
{
	core::vector3df Origin;
	core::stringc   XMLFileName;

	SLevelMapToLoadEntry() {}
	SLevelMapToLoadEntry(core::vector3df origin, const c8 *xml_fname) 
		: Origin(origin), XMLFileName(xml_fname) {}
};

//----------------------------------------------------------------------------

struct SLevelPart
{
	SLevelPart () 
		: SceneNode(0), TargetedByIdx(-1), Occluder(false),
		OpasDetected(false), WasTransparent(false),
		OpasWanted(1.0f), OpasCurrent(1.0f), OpasityChangeTimeMs(0),
		TargetKey(""), TargetNameKey(""), TeamKey(""), TranspDist(0.001f),
		BlendingAlwaysRequired(false) {}

	core::stringc TargetKey, TargetNameKey, TeamKey;

	core::array<s32> TeamIdxs;
	s32 TargetedByIdx;
	f32 TranspDist;
	scn::ISceneNode* SceneNode;

	bool BlendingAlwaysRequired;
	bool OpasDetected, WasTransparent;
	f32 OpasWanted, OpasCurrent;
	u32 OpasityChangeTimeMs;
	bool Occluder;
};

//----------------------------------------------------------------------------

//! Game Node LevelMap Class
class CGameNodeLevelMap : public IGameNodeLevelMap
{

public:

	//! constructor
	CGameNodeLevelMap(scn::ISceneNode* n, SGameNodeParams &params);

	//! destructor
	virtual ~CGameNodeLevelMap();

	//! calling every time before doing all game stuff (dynamic, scriptin, etc.)
	virtual void OnPreDynamic(f64 delta_time_sec);

    //! calling every time after doing all game stuff (dynamic, scriptin, etc.)
    virtual void OnPostDynamic(f64 delta_time_sec);

	//!
	virtual void initializeLevelParts();

protected:

private:

	//!
	void addLevelPart(const SLevelPart& level_part);

	//!
	void connectLevelParts();

	//!
	void clearLevelParts();

	core::array<SLevelPart> m_LevelParts;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeLevelMapHPP