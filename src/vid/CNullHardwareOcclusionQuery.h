//|-------------------------------------------------------------------------
//| File:        CNullHardwareOcclusionQuery.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullHardwareOcclusionQueryHPP
#define CNullHardwareOcclusionQueryHPP

//---------------------------------------------------------------------------

#include <vid/IHardwareOcclusionQuery.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! This is a abstract class that that provides the interface for the query class for 
 //! hardware occlusion.
class CNullHardwareOcclusionQuery : public IHardwareOcclusionQuery
{

public:

	//! Default object constructor
	CNullHardwareOcclusionQuery();

	//! Object destructor
	virtual ~CNullHardwareOcclusionQuery();

	//! return true if hardware supports occlusion querry
	bool isOK();

	//! Starts the hardware occlusion query
	virtual void beginOcclusionQuery();

	//! Ends the hardware occlusion test
	virtual void endOcclusionQuery();

	//! Pulls the hardware occlusion query too see if there is a result.
	virtual bool pullOcclusionQuery(u32* num_of_fragments);

	//! Let's you get the last pixel count with out doing the hardware occlusion test
	virtual u32 getLastQuerysPixelcount() const;

	//! Lets you know when query is done, or still be processed by the Hardware
	virtual bool isStillOutstanding(void); 

protected :

	// numbers of visible pixels determined by last query
    u32 mPixelCount;

    // is query hasn't yet returned a result.
	bool mIsQueryResultStillOutstanding;

	bool m_IsOK;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CNullHardwareOcclusionQueryHPP

