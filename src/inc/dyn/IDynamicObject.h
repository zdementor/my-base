//|-------------------------------------------------------------------------
//| File:        IDynamicObject.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IDynamicObjectHPP
#define IDynamicObjectHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <scn/ISceneNode.h>
#include <scn/IMesh.h>

//----------------------------------------------------------------------------
namespace my {
namespace dyn {
//----------------------------------------------------------------------------

//! dynamic object parameters
struct __MY_DYN_LIB_API__ SDynamicObjectParams
{
	//! constructor 1
	SDynamicObjectParams () : 
        Type(EDOT_NONE) , GeomForm(EDOGF_BOX), MassValue(5), 
        DynamicConstraints(EDTC_ALL_TRANSF),
		DynamicIgnoreFilter(EDCF_NONE_COLLID),
        ControledByPlayer(false),
		SurfMu(1.0f), SurfBounce(0.01f),	SurfBounceVel(0.1f), SurfSoft(0.000001f)
		{}

	//! constructor 2
    SDynamicObjectParams(const SDynamicObjectParams& params)
    {
		set(params);
	}

    void set(const SDynamicObjectParams &params)
    {
		Type				= params.Type;
        GeomForm			= params.GeomForm;
        MassValue			= params.MassValue;
        ControledByPlayer	= params.ControledByPlayer; 
        DynamicConstraints	= params.DynamicConstraints; 
		SurfMu				= params.SurfMu;
		SurfBounce			= params.SurfBounce;
		SurfBounceVel		= params.SurfBounceVel;
		SurfSoft			= params.SurfSoft;
		DynamicIgnoreFilter	= params.DynamicIgnoreFilter;
    }

    //! operator =
    inline SDynamicObjectParams& operator=(const SDynamicObjectParams& params)
    {
		set(params);
		return *this;
    } 

	//! dynamic type of this ode-object
    E_DYNAMIC_OBJECT_TYPE Type;

	//! geom form of this ode-object
    E_DYN_OBJ_GEOM_FORM GeomForm;

	//! mass value of this ode-object
    f32  MassValue;

	//! if player controlling this dynamic object, 
	//! this flag must be true, otherwise - false
    bool ControledByPlayer;

	//! dynamic constraints
    s32 DynamicConstraints;

	//! dynamic ignore filter
	s32 DynamicIgnoreFilter;

    //! Coulomb friction coefficient (0..1)
    //! 0 - frictionless contact
	//! 1 - maximum friction
    f32  SurfMu;

    //! Restitution parameter (0..1)
    //! 0 - surfaces are not bouncy at all, 1 - maximum bouncyness
    f32  SurfBounce;

	//! The minimum incoming velocity necessary for bounce (in m/s). 
    f32  SurfBounceVel;

    //! Contact normal 'softness' parameter
    f32 SurfSoft;
};

//----------------------------------------------------------------------------

struct __MY_DYN_LIB_API__ SCollisionPoint
{
	SCollisionPoint()
		: Pos(0,0,0), Normal(0,0,0), DynObj(0), DynObj2(0) {}
	SCollisionPoint(const core::vector3df &pos,
		const core::vector3df &normal,
		IDynamicObject *dobj, IDynamicObject *dobj2 = NULL)
		: Pos(pos), Normal(normal),
	DynObj(dobj), DynObj2(dobj2) {}
	SCollisionPoint(f32 posX, f32 posY, f32 posZ,
		f32 nrmX, f32 nrmY, f32 nrmZ,
		IDynamicObject *dobj, IDynamicObject *dobj2 = NULL)
		: Pos(posX, posY, posZ), Normal(nrmX, nrmY, nrmZ),
		DynObj(dobj), DynObj2(dobj2) {}

	core::vector3df Pos;
	core::vector3df Normal;
    IDynamicObject* DynObj;
	IDynamicObject* DynObj2;
};

//----------------------------------------------------------------------------

struct __MY_DYN_LIB_API__ SCollisionPoints
{   
private:
	
	core::array<SCollisionPoint> Points;
    
public:

	SCollisionPoints() {}
    ~SCollisionPoints() { clear(); }

	//! add new collision point to buffer
    void add(const SCollisionPoint &point)
    {
        Points.push_back(point);
    }

	//! clear collision points
    void clear() 
    {
        Points.set_used(0);
    }

	//! find index of the nearest collision point
    s32 getNearestTo(core::vector3df p) const
    {
		if (Points.size() <= 0) return -1;

        f32 minLengtSQ =0, lengtSQ =0;
        s32 indOfMin = -1;

		s32 PointCnt = (s32)Points.size();

        for (s32 i=0; i < PointCnt; i++)
        {
			core::vector3df vec = Points[i].Pos - p;
            lengtSQ = (f32)vec.getLengthSQ();
            if ((i==0) || (lengtSQ < minLengtSQ))
            {   
				indOfMin = i;
                minLengtSQ = lengtSQ;
            }
        }  
		return indOfMin;
    }

	//! return points array
    const core::array<SCollisionPoint>& getPoints() const
    { return Points; }

	//! return point by its index
    const SCollisionPoint& getPoint(u32 idx) const
    { return Points[idx]; }

	//! find average collision point
	core::vector3df getAveragePos() const
    {
		core::vector3df avg(0,0,0);

		u32 PointCnt = Points.size();

		if (PointCnt<=0) return avg;		

		for (u32 i=0; i<PointCnt; i++)
        {
			if (i==0) avg =  Points[i].Pos;
			else      avg += Points[i].Pos;			
        }  

		avg /= (f32)PointCnt;

		return avg;
	}

	//! return true if there is no any collision points in buffer array
	bool isEmpty() const
	{
		return ( (Points.size()>0) ? false : true );
	}
};

//----------------------------------------------------------------------------

//! interface class for ode-object
class __MY_DYN_LIB_API__ IDynamicObject : public IUnknown
{

protected:
    
	//! destructor
    virtual ~IDynamicObject() {;};

public:

    //! returns scene node assotiated with this ode-object
    virtual scn::ISceneNode* getSceneNode() =0;

    // возвращает идентификатор тела этого ODE обьекта
    virtual void* getBodyID() =0;

    // возвращает идентификатор геометрии этого ODE обьекта
    virtual void* getGeomID() =0;

    //! return My bounding box
    virtual core::aabbox3d<f32> getMyBoundingBox()=0;

	//! return Dynamic bounding box
    virtual core::aabbox3d<f32> getDynBoundingBox()=0;
    
	//! return My last position
    virtual core::vector3df& getMyLastPos()=0;
    
	//! return My last rotation
    virtual core::vector3df& getMyLastRot()=0;

    //! last Dynamic position
	virtual core::vector3df& getDynLastPos()=0;

	//! last Dynamic rotation
    virtual core::quaternion& getDynLastRot()=0;

    //! position displace between dynamic object and node
    virtual core::vector3df getDynDisplace()=0;

    //! rotation turnoff between dynamic object and node
    virtual core::vector3df getDynTurnoff()=0;

    // состояние обьекта
    virtual E_DYN_OBJ_STATE& getState()=0;

	//! get ODE object parameters
	virtual const SDynamicObjectParams& getParameters()=0;

	//! set ODE object parameters
	virtual void setParameters(SDynamicObjectParams params)=0;

	//! returning moving force applayed from player controlls to the dynamic object
	virtual core::vector3df& getMovingForce()=0;

	//! returning base point of this dynamic object
	virtual core::vector3df& getBasePoint()=0;

	//! returning high point of this dynamic object
	virtual core::vector3df& getHighPoint()=0;

	virtual core::array< core::line3d<f32> > &getTestRays()=0;	

	//! returns height between ground and base point of this dynamic object
	//! ( works only for main player dynamic object )
	virtual f32& getHeightOverGround()=0;

	//! returns time at wich this dynamic object starts to falling down (milisecs)
	virtual s32& getFallingStartTime()=0;

	//! returning falling variable
	virtual bool& getIsFalling()=0;

	//! returning falling on last calc variable
	virtual bool& getIsFallingOnLastCalc()=0;

	//! update geometry data using current parameters
	virtual void updateGeomData()=0;

	//! update geometry params for the current geometry data
	virtual void updateGeomParams()=0;

	//! return AutoUpdateGeomParams flag value
	virtual bool& getAutoUpdateGeomParams() = 0;

	virtual void storeFirstDynTransformationForInterpolation() = 0;
	virtual void restoreFirstDynTransformationForInterpolation() = 0;
	virtual void storeSecondDynTransformationForInterpolation() = 0;
	virtual void restoreSecondDynTransformationForInterpolation() = 0;
	virtual void interpolateDynTransformation(f32 factor) = 0;

	virtual bool isMoved() = 0;
	virtual void setMoved(bool value) = 0;

	virtual void addForce(const core::vector3df& force) = 0;

	virtual s32 getCollisionFilter() = 0;
};

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//----------------------------------------------------------------------------

#endif //IDynamicObjectHPP
