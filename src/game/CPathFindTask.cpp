//|-------------------------------------------------------------------------
//| File:        CPathFindTask.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CPathFindTask.h"
#include <scn/IPathFinderManager.h>
#include <os/ITimer.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

core::queue<CPathFindTask*> CPathFindTask::m_TasksQueue;

//----------------------------------------------------------------------------

//! Constructor
CPathFindTask::CPathFindTask() :
	m_NextPerformTimeMs(0), m_MinPerformPeriodMs(500)
{	
}

//----------------------------------------------------------------------------

//! Destructor
CPathFindTask::~CPathFindTask()
{
	setPerformed();
}

//----------------------------------------------------------------------------

//! initializes path find task data
void CPathFindTask::initTaskData(core::vector3df &start, core::vector3df &target)
{
	m_StartPos = start;
	m_TargetPos = target;
}

//----------------------------------------------------------------------------

//! must be called every frame
void CPathFindTask::performTask(bool force_recalc)
{
	s32 now = TIMER.getTime();

	scn::IPathFinderManager &pathmngr = PATH_FINDER_MANAGER;		

	m_TasksQueue.push_back(this);

	if (!m_TasksQueue.is_ready(this))
	{
		return;
	}

	if (now < m_NextPerformTimeMs || pathmngr.getPathFindCalcCount()>1)
	{
		return;
	}

	if ( now > m_NextPerformTimeMs)
	{
		m_NextPerformTimeMs = now + m_MinPerformPeriodMs;
	}

	m_TasksQueue.switch_next();	

	core::array< core::vector3df > new_path;

	bool finded = pathmngr.findPath(m_StartPos, m_TargetPos, new_path, force_recalc);

	if (finded)
	{
		m_FindedPath = new_path;		
	}
	else
	{
		// use old path		
		if (m_FindedPath.size()>3)
		{
			f32 mindist = 100;				
			s32 mindistcell = 0;

			if (m_FindedPath.size()>2)
			{
				for (u32 p=0; p<m_FindedPath.size(); p++)
				{
					f32 dist = core::vector3df(m_FindedPath[p]-m_StartPos).getLength();

					if (dist<mindist || p==0)
					{
						mindist = dist;
						mindistcell = p;
					}
				}
					
				if (mindistcell>0)
				{
					m_FindedPath.erase(0, mindistcell);
				}

				if (m_FindedPath.size()<2)
				{
					m_FindedPath = new_path;
				}
			}
			else
			{
				m_FindedPath = new_path;
			}
		}
		else
		{
			m_FindedPath = new_path;
		}			
	}
}

//----------------------------------------------------------------------------

//! 
void CPathFindTask::setPerformed()
{
	m_TasksQueue.erase(this);
}

//----------------------------------------------------------------------------

//! return finded path
core::array< core::vector3df >& CPathFindTask::getFindedPath()
{
	return m_FindedPath;
}

//----------------------------------------------------------------------------

//! setting task perform period
void CPathFindTask::setMinPerformPeriodMs(s32 period)
{
	m_MinPerformPeriodMs = period;
}

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------
