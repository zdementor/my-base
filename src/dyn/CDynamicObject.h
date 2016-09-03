//|-------------------------------------------------------------------------
//| File:        CDynamicObject.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDynamicObjectHPP
#define CDynamicObjectHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <dyn/IDynamicObject.h>

#include <ode/ode.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//----------------------------------------------------------------------------

class CDynamicObject : public IDynamicObject
{   
	friend class CDynamicManager;

private:

    // ODE stuff
    dWorldID World;   // the world to which the object will belong to
    dSpaceID Space;   // the objects space
    dBodyID  Body;    // body data of the object
	core::array<dGeomID> Geometry;
    dMass    Mass;    // mass of the object	
	dTriMeshDataID TriMeshData; // trimesh data

	dVector3 *vertices; // vertex array for trimesh geom
	int *indices;       // index array for trimesh geom
	int vertexcount;    // number of vertices in the vertex array
	int indexcount ;    // number of indices in the index array

    // Irrlicht stuff
	scn::ISceneNode* Node;  // visualizing node

	// displace and turnoff between ode-object and irr-node    
    core::vector3df ODETurnoff;    

	// frees all geom data 
	void freeGeomData(); 

    // parameters of the ODE object
    SDynamicObjectParams Parameters;

    // last My position and rotation
    core::vector3df LastMyPos;
    core::vector3df LastMyRot;

    // last Dynamic position and rotation
	core::vector3df  LastDynPos;
    core::quaternion LastDynRot;

    // Dynamic velocity vector
    core::vector3df DynVelocity;

    // falling ray
    core::line3d<f32> falling_ray;

    // признак падения
	bool isFalling;    
    bool isFallingOnLastCalc;

	core::vector3df MovingForce;
    core::vector3df ode_base_point;
    core::vector3df ode_high_point;
	f32             HeightOverGround;
	s32             FallingStartTime;

	core::array< core::line3d<f32> > TestRays;

	E_DYN_OBJ_STATE State;

	core::aabbox3df MyInitialBoundingBox;
	core::vector3df MyInitialBoundingBoxCenter;

	bool AutoUpdateGeomParams;

	dReal FirstDynPositionForInterpolation[3], SecondDynPositionForInterpolation[3];
    dReal FirstDynRotationForInterpolation[4], SecondDynRotationForInterpolation[4];  

	core::vector3df FirstPositionForInterpolation, SecondPositionForInterpolation;
	core::vector3df FirstRotationForInterpolation, SecondRotationForInterpolation;

	bool Moved;

	s32 m_CollisionFilter;

    // a list of all object contained in the simulation
    static core::list<IDynamicObject*> DynamicObjects;
	static core::list<IDynamicObject*> DynamicObjectsByType[E_DYNAMIC_OBJECT_TYPE_COUNT];

	core::list<IDynamicObject*>::iterator DynamicObjectsIterator;
	core::list<IDynamicObject*>::iterator DynamicObjectsByTypeIterator[E_DYNAMIC_OBJECT_TYPE_COUNT];	

	void clearDynObjectEntries();
	void createDynObjectEntries();

public:

    // Bounceable constructor, creates a CDynamicObject object 
    // belongs to a world and a collision space 
    CDynamicObject(
		dWorldID w, dSpaceID s, scn::ISceneNode* n, SDynamicObjectParams &params 
        );    

    //! Bounceable destructor clean up all object specific memory    
    virtual ~CDynamicObject();   

    //! возвращает элемент цены, за которым закреплен этот ODE обьект
    virtual scn::ISceneNode* getSceneNode();
       
	//! возвращает идентификатор тела этого ODE обьекта
    virtual void* getBodyID();
    
	//! возвращает идентификатор геометрии этого ODE обьекта
    virtual void* getGeomID();

    //! return My bounding box
    virtual core::aabbox3d<f32> getMyBoundingBox();

    //! return Dynamic bounding box
    virtual core::aabbox3d<f32> getDynBoundingBox();
    
	//! return My last position
    virtual core::vector3df& getMyLastPos();
    
	//! return My last rotation
    virtual core::vector3df& getMyLastRot();

    //! last Dynamic position
	virtual core::vector3df& getDynLastPos();

	//! last Dynamic rotation
    virtual core::quaternion& getDynLastRot();

    //! position displace between dynamic object and node
    virtual core::vector3df getDynDisplace();

	//! rotation turnoff between dynamic object and node
    virtual core::vector3df getDynTurnoff();

    //! состояние обьекта
    virtual E_DYN_OBJ_STATE& getState();

	//! get ODE object parameters
	virtual const SDynamicObjectParams& getParameters();
	
	//! set ODE object parameters
	virtual void setParameters(SDynamicObjectParams params);

	//! returning moving force applayed from player controlls to the dynamic object
	virtual core::vector3df& getMovingForce();

	//! returning base point of this dynamic object
	virtual core::vector3df& getBasePoint();

	//! returning high point of this dynamic object
	virtual core::vector3df& getHighPoint();

	virtual core::array< core::line3d<f32> > &getTestRays();

	//! returns height between ground and base point of this dynamic object
	//! ( works only for main player dynamic object )
	virtual f32& getHeightOverGround();

	//! returns time at wich this dynamic object starts to falling down (milisecs)
	virtual s32& getFallingStartTime();
	
	//! returning falling variable
	virtual bool& getIsFalling();

	//! returning falling on last calc variable
	virtual bool& getIsFallingOnLastCalc();

	//! update geometry data using current parameters
	virtual void updateGeomData(); 

	//! update geometry params for the current geometry data
	virtual void updateGeomParams();

	//! return AutoUpdateGeomParams flag value
	virtual bool& getAutoUpdateGeomParams();

	virtual void storeFirstDynTransformationForInterpolation();
	virtual void restoreFirstDynTransformationForInterpolation();
	virtual void storeSecondDynTransformationForInterpolation();
	virtual void restoreSecondDynTransformationForInterpolation();
	virtual void interpolateDynTransformation(f32 factor);

	virtual bool isMoved();
	virtual void setMoved(bool value);

	virtual void addForce(const core::vector3df& force);

	virtual s32 getCollisionFilter();
};

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------

#endif //CDynamicObjectHPP


