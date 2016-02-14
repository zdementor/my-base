//|-------------------------------------------------------------------------
//| File:        CD3D9HardwareOcclusionQuery.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CD3D9HardwareOcclusionQuery.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

CD3D9HardwareOcclusionQuery::CD3D9HardwareOcclusionQuery(IDirect3DDevice9* pD3DDevice) : 
	mpQuery(0), mpDevice(pD3DDevice)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9HardwareOcclusionQuery");
#endif

	// create the occlusion query
	if ( SUCCEEDED(mpDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &mpQuery)) ) 
	{	
		m_IsOK = true;
	}
}

//---------------------------------------------------------------------------

CD3D9HardwareOcclusionQuery::~CD3D9HardwareOcclusionQuery()
{
	SAFE_RELEASE(mpQuery);
}

//---------------------------------------------------------------------------

//! Starts the hardware occlusion query
void CD3D9HardwareOcclusionQuery::beginOcclusionQuery()
{
	mpQuery->Issue(D3DISSUE_BEGIN); 
    mIsQueryResultStillOutstanding = true;
    mPixelCount = 0;
}

//---------------------------------------------------------------------------

//! Ends the hardware occlusion test
void CD3D9HardwareOcclusionQuery::endOcclusionQuery()
{
	mpQuery->Issue(D3DISSUE_END);
}

//---------------------------------------------------------------------------

//! Pulls the hardware occlusion query too see if there is a result.
bool CD3D9HardwareOcclusionQuery::pullOcclusionQuery(u32* num_of_fragments)
{
	// in case you didn't check if query arrived and want the result now.
	if (mIsQueryResultStillOutstanding)
    {
		// Loop until the data becomes available
        DWORD pixels;
        const size_t dataSize = sizeof( DWORD );
		while (1)
        {
			const HRESULT hr = mpQuery->GetData((void *)&pixels, dataSize, D3DGETDATA_FLUSH);

            if  (hr == S_FALSE)
				continue;

            if  (hr == S_OK)
            {
				mPixelCount = pixels;
                *num_of_fragments = pixels;
                break;
            }
            if (hr == D3DERR_DEVICELOST)
            {
				*num_of_fragments = 100000;
                mPixelCount = 100000;
                mpDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &mpQuery);
                break;
            }
        } 
        mIsQueryResultStillOutstanding = false;
    }
    else
    {
        // we already stored result from last frames.
        *num_of_fragments = mPixelCount;
    }		
	
	return true;
}

//---------------------------------------------------------------------------

//! Lets you know when query is done, or still be processed by the Hardware
bool CD3D9HardwareOcclusionQuery::isStillOutstanding(void)
{
	// in case you already asked for this query
    if (!mIsQueryResultStillOutstanding)
		return false;

    DWORD pixels;
    const HRESULT hr = mpQuery->GetData( (void *) &pixels, sizeof( DWORD ), D3DGETDATA_FLUSH);

    if (hr  == S_FALSE)
		return true;

    if (hr == D3DERR_DEVICELOST)
    {
        mPixelCount = 100000;
		mpDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &mpQuery);
    }

    mPixelCount = pixels;
    mIsQueryResultStillOutstanding = false;

	return false;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
