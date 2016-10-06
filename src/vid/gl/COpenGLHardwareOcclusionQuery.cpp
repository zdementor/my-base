//|-------------------------------------------------------------------------
//| File:        COpenGLHardwareOcclusionQuery.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLHardwareOcclusionQuery.h"
#include "COpenGLDriver.h"

#if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! Default object constructor
COpenGLHardwareOcclusionQuery::COpenGLHardwareOcclusionQuery(COpenGLDriver* driver) : 
	m_Driver(driver), mQueryID(0)
{ 
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLHardwareOcclusionQuery");
#endif

	// Check for hardware occlusion support
    if (driver->queryFeature(EVDF_OCCLUSION_QUERY))
	{
		glGenQueries(1, &mQueryID );	
		if (mQueryID)
			m_IsOK = true;
	}
}

//------------------------------------------------------------------

//! Object destructor
COpenGLHardwareOcclusionQuery::~COpenGLHardwareOcclusionQuery() 
{ 
	if (mQueryID)
		glDeleteQueries(1, &mQueryID);  
}

//------------------------------------------------------------------

void COpenGLHardwareOcclusionQuery::beginOcclusionQuery() 
{ 
	glBeginQuery(GL_SAMPLES_PASSED, mQueryID);
}

//------------------------------------------------------------------

void COpenGLHardwareOcclusionQuery::endOcclusionQuery() 
{ 
    glEndQuery(GL_SAMPLES_PASSED);
}

//------------------------------------------------------------------

bool COpenGLHardwareOcclusionQuery::pullOcclusionQuery( u32* num_of_fragments ) 
{
	glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT, (GLuint*)num_of_fragments);
	
	mPixelCount = *num_of_fragments;

	return true;
}

//------------------------------------------------------------------

bool COpenGLHardwareOcclusionQuery::isStillOutstanding(void)
{    
	GLuint available;

    glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT_AVAILABLE, &available);
      
	return !(available == GL_TRUE);  
} 

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #if __MY_BUILD_GL_VER__ >= MY_DRIVER_TYPE_OPENGL21
