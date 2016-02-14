//|-------------------------------------------------------------------------
//| File:        IPathFindTask.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IPathFindTaskHPP
#define IPathFindTaskHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

class __MY_GAME_LIB_API__ IPathFindTask : public IUnknown
{
public:

	//! Constructor
	IPathFindTask() {}

	//! Destructor
    virtual ~IPathFindTask() {}

	//! initializes path find task data
    virtual void initTaskData(core::vector3df &start, core::vector3df &target) = 0;

	//! must be called every frame
    virtual void performTask(bool force_recalc) = 0;

	//! 
    virtual void setPerformed() = 0;

	//! return finded path
	virtual core::array< core::vector3df >& getFindedPath() = 0;

	//! setting task perform period
	virtual void setMinPerformPeriodMs(s32 period) = 0;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IPathFindTaskHPP