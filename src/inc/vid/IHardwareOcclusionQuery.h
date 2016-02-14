//|-------------------------------------------------------------------------
//| File:        IHardwareOcclusionQuery.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IIHardwareOcclusionQueryHPP
#define IIHardwareOcclusionQueryHPP

//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! This is a abstract class that that provides the interface for the query class for 
//! hardware occlusion.
class __MY_VID_LIB_API__ IHardwareOcclusionQuery : public IUnknown
{
public:

	//! return true if hardware supports occlusion querry
	virtual bool isOK() = 0;

	/**
	  * Starts the hardware occlusion query
	  * @Remarks
	  *
	  *		In the rendering loop:
	  *     startOcclusionQuery();
	  *			Draw all occluders
	  *		endOcclusionQuery();
	  *
	  *		Results must be pulled using:
	  *		UINT	m_uintNumberOfPixelsVisable;
	  *		pullOcclusionQuery( &m_dwNumberOfPixelsVisable );
	  */
	virtual void beginOcclusionQuery() = 0;

	/**
	  * Ends the hardware occlusion test
	  */
	virtual void endOcclusionQuery() = 0;

	/**
      * Pulls the hardware occlusion query too see if there is a result.
      * @retval NumOfFragments will get the resulting number of fragments.
      * @return True if success or false if not.
      */
	virtual bool pullOcclusionQuery(u32* num_of_fragments) = 0;

	/**
	  * Let's you get the last pixel count with out doing the hardware occlusion test
	  * @return The last fragment count from the last test.
	  * Remarks This function won't give you new values, just the old value.
	  */
	virtual u32 getLastQuerysPixelcount() const = 0;

	/**
	  * Lets you know when query is done, or still be processed by the Hardware
	  * @return true if query isn't finished.
	  */
	 virtual bool IHardwareOcclusionQuery::isStillOutstanding(void) = 0; 
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IIHardwareOcclusionQueryHPP

