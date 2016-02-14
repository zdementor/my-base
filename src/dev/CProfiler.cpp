//|-------------------------------------------------------------------------
//| File:        CProfiler.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CProfiler.h"
#include <dev/IDevice.h>
#include <io/ILogger.h>
#include <os/ITimer.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

CProfiler::SProfileNode *CProfiler::SProfileNode::s_CurrentProfileNode = NULL;

//----------------------------------------------------------------------------

CProfiler::CProfiler() : m_Suspended(false)
{
	allocProfileId("Overal time");

	suspendProfiling();
}

//----------------------------------------------------------------------------

CProfiler::~CProfiler()
{
	u32 cnt = m_ProfileNodes.size();
	for (u32 i = 0; i < cnt; i++)
		delete m_ProfileNodes[i];
	m_ProfileNodes.clear();
}

//----------------------------------------------------------------------------

void CProfiler::suspendProfiling()
{
	if (!m_Suspended)
	{
		m_Suspended = true;
		LOGGER.logInfo("Profiling suspended.");
	}
}

//----------------------------------------------------------------------------

void CProfiler::resumeProfiling()
{
	if (m_Suspended)
	{
		m_Suspended = false;
		LOGGER.logInfo("Profiling resumed.");
	}
}

//----------------------------------------------------------------------------

bool CProfiler::startProfiling(u32 profileId)
{
	if (profileId == 0)
	{
		u32 cnt = m_ProfileNodes.size();
		for (u32 i = 0; i < cnt; i++)
			m_ProfileNodes[i]->flushResult();
	}
	else
	{
		if (m_Suspended)
			return false;
	}

	SProfileNode *node = NULL;
	if (profileId < m_ProfileNodes.size())
		node = m_ProfileNodes[profileId];
	if (!node || node->Profiling)
	{
		LOGGER.logErr("startProfiling: Profiling error for profileId=%d name='%s'",
			profileId, node ? node->Parameter.Name.c_str() : NULL);
		return false;
	}
	if (node->Parent &&
		node->Parent != SProfileNode::s_CurrentProfileNode)
	{
		LOGGER.logErr("startProfiling: Profiling error for profileId=%d name='%s', wrong parent",
			profileId, node->Parameter.Name.c_str());
		return false;
	}

	if (!node->Parent)
	{
		SProfileNode *parent =
			node->Parent = SProfileNode::s_CurrentProfileNode;
		if (parent)
		{
			SProfileNode *child = parent->LastChild;
			if (child)
				child = child->Next = parent->LastChild = node;
			else
				child = parent->FirstChild = parent->LastChild = node;
			child->ParentDepth = parent->ParentDepth + 1;
		}
	}

	f64 timeSec = TIMER.getSystemTimeSec();

	node->Profiling = true;
	node->CurrentTimeSec = timeSec;

	SProfileNode::s_CurrentProfileNode = node;

	return true;
}

//----------------------------------------------------------------------------

u32 CProfiler::allocProfileId(const c8 *profileName)
{
	u32 profileId = m_ProfileNodes.size();
	m_ProfileNodes.push_back(new SProfileNode(profileName));
	m_ProfileNodes[profileId]->Parameter.Id = profileId;
	return profileId;
}

//----------------------------------------------------------------------------

bool CProfiler::stopProfiling(u32 profileId, const c8 *info)
{
	if (profileId != 0)
	{
		if (m_Suspended)
			return false;
	}

	f64 timeSec = TIMER.getSystemTimeSec();

	SProfileNode *node = NULL;
	if (profileId < m_ProfileNodes.size())
		node = m_ProfileNodes[profileId];
	if (!node || !node->Profiling)
	{
		LOGGER.logErr("stopProfiling: Profiling error for profileId=%d name='%s'",
			profileId, node ? node->Parameter.Name.c_str() : NULL);
		return false;
	}

	node->Profiling = false;
	node->Info = info ? info : "";
	
	f64 deltaSec = timeSec - node->CurrentTimeSec;

	node->TimeSec[node->ProfileCount % MY_PROFILE_AVERAGE_COUNT] += deltaSec;
	
	if (profileId == 0)
	{
		u32 cnt = m_ProfileNodes.size();
		for (u32 i = 0; i < cnt; i++)
			m_ProfileNodes[i]->calcAverage();
	}

	SProfileNode::s_CurrentProfileNode = node->Parent;

	return true;
}

//----------------------------------------------------------------------------

bool CProfiler::setProfileInfo(u32 profileId, const c8 *info)
{
	SProfileNode *node = NULL;
	if (profileId < m_ProfileNodes.size())
		node = m_ProfileNodes[profileId];
	if (!node)
	{
		LOGGER.logErr("setProfileInfo: Profiling error for profileId=%d",
			profileId);
		return false;
	}

	node->Info = info ? info : "";

	return true;
}

//----------------------------------------------------------------------------

const SProfileParameter* CProfiler::getFirstProfileParameter()
{ return &m_ProfileNodes[0]->Parameter; }

//----------------------------------------------------------------------------

const SProfileParameter* CProfiler::getNextProfileParameter(u32 profileId)
{
	if (profileId >= m_ProfileNodes.size())
		return NULL;
	SProfileNode *node = m_ProfileNodes[profileId];
	if (node->OldFirstChild)
		return &node->OldFirstChild->Parameter;
	else if (node->OldNext)
		return &node->OldNext->Parameter;
	CProfiler::SProfileNode* parent = NULL;
	while ((parent = node->OldParent) != NULL)
	{
		if (parent->OldNext)
			return &parent->OldNext->Parameter;
		node = parent;
	}
	return NULL;
}

//----------------------------------------------------------------------------

const SProfileParameter* CProfiler::getProfileParameter(u32 profileId)
{
	if (profileId >= m_ProfileNodes.size())
		return NULL;
	return &m_ProfileNodes[profileId]->Parameter;
}

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------
