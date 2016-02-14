//|-------------------------------------------------------------------------
//| File:        CD3D9HardwareOcclusionQuery.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9HardwareOcclusionQueryHPP
#define CD3D9HardwareOcclusionQueryHPP

//---------------------------------------------------------------------------

#include "../CNullHardwareOcclusionQuery.h"
#include <d3d/d3d9.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! If you use multiple rendering passes you can test only the first pass and 
//! all other passes don't have to be rendered 
//! if the first pass results has too few pixels visible.

//! Be sure to render all occluder first and whats out so the RenderQue don't switch places on 
//! the occluding objects and the tested objects because it thinks it's more effective..

//! This is a class that is the DirectX9 implementation of 
//! hardware occlusion testing.
class CD3D9HardwareOcclusionQuery : public CNullHardwareOcclusionQuery
{
public:

	//! Default object constructor
	CD3D9HardwareOcclusionQuery( IDirect3DDevice9* pD3DDevice );

	//! Object destructor
	virtual ~CD3D9HardwareOcclusionQuery();

	//! Starts the hardware occlusion query
	virtual void beginOcclusionQuery();

	//! Ends the hardware occlusion test
	virtual void endOcclusionQuery();

	//! Pulls the hardware occlusion query too see if there is a result.
	virtual bool pullOcclusionQuery(u32* num_of_fragments);

	//! Lets you know when query is done, or still be processed by the Hardware
	virtual bool isStillOutstanding(void); 
	
private:
	
	IDirect3DQuery9*	mpQuery;
	IDirect3DDevice9*   mpDevice;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CD3D9HardwareOcclusionQueryHPP
