//|-------------------------------------------------------------------------
//| File:        CDynamicManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDynamicManagerHPP
#define CDynamicManagerHPP
//---------------------------------------------------------------------------

#include <my_inc.h>

#include <dyn/IDynamicManager.h>  
#include <dev/IDevice.h>  
#include "CDynamicObject.h"

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//---------------------------------------------------------------------------

class CDynamicManager : public IDynamicManager
{
    
private:

    //! time delta (ms) from last ode dynamic calculation 
    static f32 deltaTime;

    //! The Collision callback gets called every time ODE decides that 2 geoms 
	//! are close enough to check for intersection points    
    static void nearCollisionCallback(void* data, dGeomID o1, dGeomID o2);  

    // world for objects 
    dWorldID theWorld; 

    // space for objects
    dSpaceID theSpace; 

    // the group to which all contact joint are added
    dJointGroupID theJointGroup;     
    
    //! dynamic manager parameters
    SDynamicWorldParams Parameters;  

	dGeomID ray_geom;	

	bool StoppingCalcDynamic;	

	s32 EnabledBodies;

	f64 m_DynamicDeltaTimeSec;

	core::math::li2df m_ForceInterolator;

public:

    //! constructor
    CDynamicManager();

    //! destructor
    virtual ~CDynamicManager();

    // add ODE object
    virtual IDynamicObject* addObjectForSceneNode(
        scn::ISceneNode* n, SDynamicObjectParams &params = SDynamicObjectParams()
        );

	virtual s32 getEnabledDynamicObjectsCount();

	virtual s32 getDynamicObjectsCount();

    //! destroy ODE object for scene node
	//! (return true on success, otherwise false)
    virtual bool removeObjectForSceneNode(scn::ISceneNode* n);

    //! destroy all ODE objects
    virtual void removeAllObjects();

    //! pre dynamic call
    virtual void preDynamic();

	//! dynamic calculation
    virtual void calcDynamic(f64 delta_time_sec);

    //! post dynamic call
    virtual void postDynamic();

    // let all bouncables have the new position and rotation after 
    // physics have been applied    
    virtual void updateGeometryFromDynamic();   

    //! find collisions of this ray with world
    //! \param ray - ray (nothing more to say)
    //! \param mode - ode-dynamic calculation mode
	//! \param collisions - ref to store all finded collisions
    //! \param excludeNodesList - pointer to array of the pointers 
    //! to objects with wich we don't want to calc collisions
    virtual bool findCollisionsWithRay(
        const core::line3df &ray, s32 collid_filter, SCollisionPoints &collisions,
        core::array<scn::ISceneNode*> *excludeNodesList=0
        );

	//! find collisions of this ray with dynamic object
    //! \param obj - dynamic object to find collisions with
    //! \param ray - ray (nothing more to say)
	//! \param collisions - ref to store all finded collisions
    virtual bool findCollisionsWithRay(
		IDynamicObject *obj, const core::line3df &ray, SCollisionPoints &collisions
        );

	//! check intersections of this ray with world
    //! \param ray - ray (nothing more to say)
    //! \param mode - ode-dynamic calculation mode
    //! \param excludeNodesList - pointer to array of the pointers 
    //! to objects with wich de don't want to check intersections
	virtual bool checkIntersectionWithRay(
		const core::line3df &ray, s32 collid_filter, 
		core::array<scn::ISceneNode*> *excludeNodesList=0
		);

	//! translate ODE-object through this dynamic-world by the force
	virtual void translateObjectThroughTheWorld(
		IDynamicObject *obj, const core::vector3df &force
		);	

    //! set ODE World Params
    virtual void setParams(SDynamicWorldParams &params);

	//! getting dynamic world gravity
	virtual core::vector3df getGravity();

	//! setting dynamic world gravity
	virtual void setGravity(core::vector3df grav);

	//! stopping calculating dynamic
	virtual void stopCalcDynamic();

	//! resuming calculating dynamic
	virtual void resumeCalcDynamic();

	//! is calculating dynamic or not
	virtual bool isCalcDynamic(); 

	// moving dynamic object
	virtual void setDynamicObjectPosition(
		IDynamicObject* dynobj, const core::vector3df &pos, bool controlled_by_ai=false
		);

	// rotating dynamic object
	virtual void setDynamicObjectRotation(
		IDynamicObject* dynobj,const core::vector3df &rot, bool controlled_by_ai=false
		);

	// scaling dynamic object
	virtual void setDynamicObjectScale(IDynamicObject* dynobj,const core::vector3df &scale);

	// returns dynamic oject axis aligned bounding box
	virtual core::aabbox3d<f32> getDynamicObjectAABBox(IDynamicObject* dynobj);

	virtual void storeFirstObjectsTransformationsForInterpolation();
	virtual void restoreFirstObjectsTransformationsForInterpolation();
	virtual void storeSecondObjectsTransformationsForInterpolation();
	virtual void restoreSecondObjectsTransformationsForInterpolation();
	virtual void interpolateObjectsTransformations(f32 factor);		
};

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------

#endif
