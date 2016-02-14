//|-------------------------------------------------------------------------
//| File:        CGameSceneSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameSceneSaverHPP
#define CGameSceneSaverHPP
//----------------------------------------------------------------------------

#include <game/IGameNode.h>
#include <io/IXMLWriter.h>
#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

class IGameManager;

//----------------------------------------------------------------------------

//! Game Manager. Used as manager of all game scene nodes and games stuff
class CGameSceneSaver : public IUnknown
{
public:

	//! constructor
	CGameSceneSaver();

	//! destructor
	virtual ~CGameSceneSaver();

	//! saving scene to xml-file
	void saveGameScene(core::stringc xmlFileName);

	//! save game node common parameters to the opened xml-file
	void saveGameNodeCommonParams( io::IXMLWriter* xml, IGameNode* game_node );

private:

	s32 SceneSaveWarnings;

};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameSceneSaverHPP