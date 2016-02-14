//|-------------------------------------------------------------------------
//| File:        CNullHardwareOcclusionQueryNull.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CNullHardwareOcclusionQuery.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

CNullHardwareOcclusionQuery::CNullHardwareOcclusionQuery() : 
	mPixelCount(0), mIsQueryResultStillOutstanding(false), m_IsOK(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CNullHardwareOcclusionQuery");
#endif
}

//---------------------------------------------------------------------------

CNullHardwareOcclusionQuery::~CNullHardwareOcclusionQuery()
{
}

//---------------------------------------------------------------------------

//! return true if hardware supports occlusion querry
bool CNullHardwareOcclusionQuery::isOK()
{
	return m_IsOK;
}

//---------------------------------------------------------------------------

//! Starts the hardware occlusion query
void CNullHardwareOcclusionQuery::beginOcclusionQuery()
{
}

//---------------------------------------------------------------------------

//! Ends the hardware occlusion test
void CNullHardwareOcclusionQuery::endOcclusionQuery()
{
}

//---------------------------------------------------------------------------

//! Pulls the hardware occlusion query too see if there is a result.
bool CNullHardwareOcclusionQuery::pullOcclusionQuery(u32* num_of_fragments)
{
	return false;
}

//---------------------------------------------------------------------------

//! Let's you get the last pixel count with out doing the hardware occlusion test
u32 CNullHardwareOcclusionQuery::getLastQuerysPixelcount() const 
{ 
	return mPixelCount; 
}

//---------------------------------------------------------------------------

//! Lets you know when query is done, or still be processed by the Hardware
bool CNullHardwareOcclusionQuery::isStillOutstanding(void)
{
	return false;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------


