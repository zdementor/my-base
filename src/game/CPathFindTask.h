//|-------------------------------------------------------------------------
//| File:        CPathFindTask.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CPathFindTaskHPP
#define CPathFindTaskHPP
//----------------------------------------------------------------------------

#include <game/IPathFindTask.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

class __MY_GAME_LIB_API__ CPathFindTask : public IPathFindTask
{
public:

	//! Constructor
	CPathFindTask();

	//! Destructor
    virtual ~CPathFindTask();

	//! initializes path find task data
    virtual void initTaskData(core::vector3df &start, core::vector3df &target);

	//! must be called every frame
    virtual void performTask(bool force_recalc);

	//! 
    virtual void setPerformed();

	//! return finded path
	virtual core::array< core::vector3df >& getFindedPath();

	//! setting task perform period
	virtual void setMinPerformPeriodMs(s32 period);

private:

	core::array< core::vector3df > m_FindedPath;
	s32 m_NextPerformTimeMs;
	s32 m_MinPerformPeriodMs;

	core::vector3df m_StartPos, m_TargetPos;

	static core::queue<CPathFindTask*> m_TasksQueue;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CPathFindTaskHPP