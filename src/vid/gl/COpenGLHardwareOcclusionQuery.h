//|-------------------------------------------------------------------------
//| File:        COpenGLHardwareOcclusionQuery.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLHardwareOcclusionQueryHPP
#define COpenGLHardwareOcclusionQueryHPP
//---------------------------------------------------------------------------

#include "../CNullHardwareOcclusionQuery.h"
#include "COpenGLDriver.h"

#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

// If you use multiple rendering passes you can test only the first pass and all other passes don't have to be rendered 
// if the first pass result has too few pixels visible.

// Be sure to render all occluder first and whats out so the RenderQue don't switch places on 
// the occluding objects and the tested objects because it thinks it's more effective..

//! This is a class that is the base class of the query class for 
//! hardware occlusion.
class COpenGLHardwareOcclusionQuery : public CNullHardwareOcclusionQuery
{
public:

	//! Default object constructor
	COpenGLHardwareOcclusionQuery( COpenGLDriver* driver );

	//! Object destructor
	virtual ~COpenGLHardwareOcclusionQuery();

	//! Starts the hardware occlusion query
	virtual void beginOcclusionQuery();

	//! Ends the hardware occlusion test
	virtual void endOcclusionQuery();

	//! Pulls the hardware occlusion query too see if there is a result.
	virtual bool pullOcclusionQuery(u32* num_of_fragments);

	//! Lets you know when query is done, or still be processed by the Hardware
	virtual bool isStillOutstanding(void); 

private:

	GLuint		  mQueryID;
	COpenGLDriver *m_Driver;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
#endif // #ifndef COpenGLHardwareOcclusionQueryHPP
