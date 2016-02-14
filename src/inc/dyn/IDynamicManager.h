//|-------------------------------------------------------------------------
//| File:        IDynamicManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IDynamicManagerHPP
#define IDynamicManagerHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <dyn/IDynamicObject.h>   
#include <ISingleton.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//----------------------------------------------------------------------------

//! dynamic world parameters
struct SDynamicWorldParams
{
	SDynamicWorldParams ()
    {
		defaultInit();
    }
	//! setting default values to dynamic world parameters
    void defaultInit()
    {
        Gravity  = core::vector3df(0.0f,-1000.0f,0.0f);  
        WorldCFM = 0.1f;
        WorldERP = 0.8f;
    }
	//! setting dynamic world parameters
    void set(SDynamicWorldParams &params)
    {
        Gravity  = params.Gravity;
        WorldCFM = params.WorldCFM;
        WorldERP = params.WorldERP;
    }

    // Gravity
    core::vector3df Gravity;

    // Global CFM (constraint force mixing) value. (10-9 - 1)
    f32 WorldCFM;

    // Global ERP (error reduction parameter) value. (0..1)
    f32 WorldERP;   
};

//----------------------------------------------------------------------------

//! interface class for dynamic manager
class __MY_DYN_LIB_API__ IDynamicManager :
	public IUnknown, public ISingleton<IDynamicManager>
{
	friend class CDynamicManager; 
	friend bool destroyDynamicManager(IDynamicManager *mngr);
	friend void destroyAllDynamicManagers();

private: 

    //! destructor
    virtual ~IDynamicManager() {};

public:

	virtual s32 getEnabledDynamicObjectsCount() = 0;

	virtual s32 getDynamicObjectsCount() = 0;

    //! add new dynamic object to this ode-world
	//! \param n - scene node
	//! \param m - scene node mesh
	//! \param params - ode-object parameters
    virtual IDynamicObject* addObjectForSceneNode(
        scn::ISceneNode* n, SDynamicObjectParams &params = SDynamicObjectParams()
        )=0;

    //! destroy ODE object for scene node
	//! (return true on success, otherwise false)
    virtual bool removeObjectForSceneNode(scn::ISceneNode* n) = 0;

    //! destroy all dynamic objects
    virtual void removeAllObjects() = 0;

	//! pre dynamic call
    virtual void preDynamic() = 0;

    //! dynamic calculation 
    virtual void calcDynamic(f64 delta_time_sec) = 0;

    //! post dynamic call
    virtual void postDynamic() = 0;

    //! let all bouncables have the new position and rotation after 
    // physics have been applied    
    virtual void updateGeometryFromDynamic() = 0;   

    //! find collisions of this ray with world
    //! \param ray - ray (nothing more to say)
    //! \param mode - ode-dynamic calculation mode
	//! \param collisions - ref to store all finded collisions
    //! \param excludeNodesList - pointer to array of the pointers 
    //! to objects with wich we don't want to calc collisions
    virtual bool findCollisionsWithRay(
        core::line3d<f32> ray, s32 collid_filter, 
		SCollisionPoints &collisions,
        core::array<scn::ISceneNode*> *excludeNodesList=0
        )=0;

	//! find collisions of this ray with dynamic object
    //! \param obj - dynamic object to find collisions with
    //! \param ray - ray (nothing more to say)
	//! \param collisions - ref to store all finded collisions
    virtual bool findCollisionsWithRay(
		IDynamicObject *obj, core::line3d<f32> ray, SCollisionPoints &collisions
        )=0;

	//! check intersections of this ray with world
	//! \param ray - ray (nothing more to say)
	//! \param mode - ode-dynamic calculation mode
	//! \param excludeNodesList - pointer to array of the pointers 
	//! to objects with wich de don't want to check intersections
	virtual bool checkIntersectionWithRay(
		core::line3d<f32> ray, s32 collid_filter, 
		core::array<scn::ISceneNode*> *excludeNodesList=0
		)=0; 

	//! translate ODE-object through this dynamic-world by the force
	virtual void translateObjectThroughTheWorld(
		IDynamicObject *obj, core::vector3df force
		)=0;	

    //! set ODE World Params
    virtual void setParams(SDynamicWorldParams &params) =0;

	//! getting dynamic world gravity
	virtual core::vector3df getGravity()=0;

	//! setting dynamic world gravity
	virtual void setGravity(core::vector3df grav)=0;

	//! stopping calculating dynamic
	virtual void stopCalcDynamic()=0;

	//! resuming calculating dynamic
	virtual void resumeCalcDynamic()=0;

	//! is calculating dynamic or not
	virtual bool isCalcDynamic()=0;

	// moving dynamic object
	virtual void setDynamicObjectPosition(
		IDynamicObject* dynobj, const core::vector3df &pos, bool controlled_by_ai=false
		)=0;

	// rotating dynamic object
	virtual void setDynamicObjectRotation(
		IDynamicObject* dynobj,const core::vector3df &rot, bool controlled_by_ai=false
		)=0;

	// scaling dynamic object
	virtual void setDynamicObjectScale(IDynamicObject* dynobj,const core::vector3df &scale)=0;

	// returns dynamic oject axis aligned bounding box
	virtual core::aabbox3d<f32> getDynamicObjectAABBox(IDynamicObject* dynobj)=0;

	virtual void storeFirstObjectsTransformationsForInterpolation() = 0;
	virtual void restoreFirstObjectsTransformationsForInterpolation() = 0;
	virtual void storeSecondObjectsTransformationsForInterpolation() = 0;
	virtual void restoreSecondObjectsTransformationsForInterpolation() = 0;
	virtual void interpolateObjectsTransformations(f32 factor) = 0;

	//!	Return singleton object
	static IDynamicManager& getSingleton();

	//! Return pointer to singleton object
	static IDynamicManager* getSingletonPtr();
};

//---------------------------------------------------------------------------

__MY_DYN_LIB_API__ IDynamicManager* createDynamicManager();

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------

#endif //IDynamicManagerHPP