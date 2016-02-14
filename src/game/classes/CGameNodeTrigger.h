//|-------------------------------------------------------------------------
//| File:        CGameNodeTrigger.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameNodeTriggerHPP
#define CGameNodeTriggerHPP
//----------------------------------------------------------------------------

#include <game/IGameNodeTrigger.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Node Class used as base class for all game nodes
class CGameNodeTrigger : public IGameNodeTrigger
{

public:

	CGameNodeTrigger(scn::ISceneNode* n, SGameNodeParams &params);
	virtual ~CGameNodeTrigger();

	virtual bool activate(IGameNode* activator);
	virtual bool deactivate();

	virtual void setActivateScriptFileName(const c8* filename);
	virtual const c8* getActivateScriptFileName();

	virtual void setActivateScriptText(const c8* script_text);
	virtual const c8* getActivateScriptText();

	virtual void setDeactivateScriptFileName(const c8* filename);
	virtual const c8* getDeactivateScriptFileName();

	virtual void setDeactivateScriptText(const c8* script_text);
	virtual const c8* getDeactivateScriptText();

	//!
	virtual const c8* getGameCorePropertiesXMLString();

protected:

	core::stringc m_ActivateScriptFileName;
	core::stringc m_ActivateScriptText;
	core::stringc m_DeactivateScriptFileName;
	core::stringc m_DeactivateScriptText;

	bool m_CurrActivated;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameNodeTriggerHPP