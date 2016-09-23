//|-------------------------------------------------------------------------
//| File:        CProfiler.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CProfilerHPP
#define CProfilerHPP
//----------------------------------------------------------------------------

#include <dev/IProfiler.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

#define MY_PROFILE_AVERAGE_COUNT 25

class CProfiler : public IProfiler
{
public:

	CProfiler();
	virtual ~CProfiler();

	virtual bool isProfiling()
	{ return !m_Suspended; }

	virtual void suspendProfiling();
	virtual void resumeProfiling();

	virtual bool startProfiling(u32 profileId = 0);
	virtual u32 allocProfileId(const c8 *profileName);
	virtual bool stopProfiling(u32 profileId = 0, const c8 *info = NULL);

	virtual bool setProfileInfo(u32 profileId, const c8 *info);

	virtual const SProfileParameter* getFirstProfileParameter();
	virtual const SProfileParameter* getNextProfileParameter(u32 profileId);
	virtual const SProfileParameter* getProfileParameter(u32 profileId);

private:

	IDevice *m_Device;

	struct SProfileNode
	{
	public:
		SProfileNode()
		{
			defaultInit();
		}
		SProfileNode(const c8 *name)
		{
			defaultInit();
			Parameter.Name = name;
		}
		void defaultInit()
		{
			for (u32 i = 0; i < MY_PROFILE_AVERAGE_COUNT; i++)
				TimeSec[i] = 0.0;
			Info= "";
			ProfileCount = 0;
			AverageTimeSec = ROUNDING_ERROR_64;
			FirstChild = LastChild = Next = Parent =
				OldFirstChild = OldLastChild = OldNext = OldParent = NULL;
			Parameter.ParentDepth		= 0;
			Parameter.FilteredTimeSec	= 0;
			ParentDepth = 0;
			Profiling = false;
		}
		void flushResult()
		{
			Parameter.Info				= Info;
			Parameter.FilteredTimeSec	= AverageTimeSec;
			Parameter.CurrentTimeSec	= TimeSec[ProfileCount % MY_PROFILE_AVERAGE_COUNT];
			Parameter.ParentDepth	= ParentDepth;
			Parameter.Pers			= 100.0f * (Parent ?
				(AverageTimeSec / Parent->AverageTimeSec) : 1.0f);
			TimeSec[ProfileCount % MY_PROFILE_AVERAGE_COUNT] = 0.0;
			OldFirstChild	= FirstChild;
			OldLastChild	= LastChild;
			OldNext		= Next;
			OldParent	= Parent;
			FirstChild = LastChild = Next = Parent = NULL;
			ParentDepth = 0;
			Profiling = false;
		}
		void calcAverage()
		{
			u32 valId = ProfileCount % MY_PROFILE_AVERAGE_COUNT;
			f64 avg = 0.0f;
			u32 avgCnt =  (ProfileCount >= MY_PROFILE_AVERAGE_COUNT) ?
				MY_PROFILE_AVERAGE_COUNT : (valId + 1);
			for (u32 i = 0; i < avgCnt; i++)
				avg += TimeSec[i];
			avg /= (f64)avgCnt;
			if (core::math::Abs(AverageTimeSec - avg) / AverageTimeSec > 0.1)
				AverageTimeSec = avg;
			if (AverageTimeSec <= 0.0)
				AverageTimeSec = ROUNDING_ERROR_64;
			ProfileCount++;
		}
		SProfileParameter Parameter;
		SProfileNode *FirstChild, *LastChild, *Next, *Parent;
		SProfileNode *OldFirstChild, *OldLastChild, *OldNext, *OldParent;
		f64 AverageTimeSec, FilteredAverageTimeSec;
		f64 CurrentTimeSec;
		f64 TimeSec[MY_PROFILE_AVERAGE_COUNT];
		u32 ProfileCount;
		s32 ParentDepth;
		bool Profiling;
		core::stringc Info;
		static SProfileNode *s_CurrentProfileNode;
	};

	core::array <SProfileNode*> m_ProfileNodes;

	bool m_Suspended;
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------
#endif

