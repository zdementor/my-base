//|-------------------------------------------------------------------------
//| File:        IProfiler.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IProfilerHPP
#define IProfilerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace dev {
//----------------------------------------------------------------------------

struct SProfileParameter
{
	SProfileParameter()
		: Id((u32)-1), Name("Unknown Parameter"),
		CurrentTimeSec(0.0), FilteredTimeSec(0.0), ParentDepth(0),
		Info(""), Pers(0.0f) {}
	u32				Id;
	core::stringc	Name;
	f64				CurrentTimeSec, FilteredTimeSec;
	u32				ParentDepth;
	core::stringc	Info;
	f32				Pers;
};

//----------------------------------------------------------------------------

class IProfiler :
	public IUnknown, public ISingleton <IProfiler>
{
public:

	virtual bool isProfiling() = 0;

	virtual void suspendProfiling() = 0;
	virtual void resumeProfiling() = 0;

	virtual bool startProfiling(u32 profileId = 0) = 0;
	virtual u32 allocProfileId(const c8 *profileName) = 0;
	virtual bool stopProfiling(u32 profileId = 0, const c8 *info = NULL) = 0;

	virtual bool setProfileInfo(u32 profileId, const c8 *info) = 0;

	virtual const SProfileParameter* getFirstProfileParameter() = 0;
	virtual const SProfileParameter* getNextProfileParameter(u32 profileId) = 0;
	virtual const SProfileParameter* getProfileParameter(u32 profileId) = 0;

	static IProfiler& getSingleton();
	static IProfiler* getSingletonPtr();
};

//----------------------------------------------------------------------------
} // end namespace dev
} // end namespace my
//----------------------------------------------------------------------------
#endif

