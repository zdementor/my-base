//|-------------------------------------------------------------------------
//| File:        IGameNodeTrigger.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeTriggerHPP
#define IGameNodeTriggerHPP
//----------------------------------------------------------------------------

#include "IGameNodeActivatable.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! GameNode Trigger class
class IGameNodeTrigger : public IGameNodeActivatable
{

public:

	//! constructor
	IGameNodeTrigger(scn::ISceneNode* n, SGameNodeParams &params)
		: IGameNodeActivatable(n, params) {}

	//! destructor
	virtual ~IGameNodeTrigger() {}

	virtual void setActivateScriptFileName(const c8* filename)=0;
	virtual const c8* getActivateScriptFileName()=0;

	virtual void setActivateScriptText(const c8* script_text)=0;
	virtual const c8* getActivateScriptText()=0;

	virtual void setDeactivateScriptFileName(const c8* filename)=0;
	virtual const c8* getDeactivateScriptFileName()=0;

	virtual void setDeactivateScriptText(const c8* script_text)=0;
	virtual const c8* getDeactivateScriptText()=0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeTriggerHPP