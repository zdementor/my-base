//|-------------------------------------------------------------------------
//| File:        CGameSceneLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameSceneLoaderHPP
#define CGameSceneLoaderHPP
//----------------------------------------------------------------------------
#include <game/IGameNode.h>
#include <io/IXMLReader.h>
//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------
class IGameManager;
//----------------------------------------------------------------------------
//! Game Manager. Used as manager of all game scene nodes and games stuff
class CGameSceneLoader : public IUnknown
{
public:

	//! constructor
	CGameSceneLoader();

	//! destructor
	virtual ~CGameSceneLoader();

	//! load scene to xml-file
	bool loadGameScene(core::stringc xmlFileName);

	//! loads game node common parameters from the opened xml-file
	SGameNodeCommonParams loadGameNodeCommonParams(
		io::IXMLReader* xml, core::stringw node_name)
		;

private:
	
	struct SParentConnectionEntry
	{
		SParentConnectionEntry() : child(NULL), parent_id(-1) {;}
		SParentConnectionEntry(
			scn::ISceneNode *_child, s32 _parent_id
			) : child(_child), parent_id(_parent_id) {;}
		scn::ISceneNode *child;
		s32 parent_id;
	};

};
//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
#endif // #ifndef CGameSceneLoaderHPP