//|-------------------------------------------------------------------------
//| File:        ODEHelper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ODEHelperHPP
#define ODEHelperHPP
//---------------------------------------------------------------------------

#include <my_inc.h>

#include <ode/ode.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
namespace ode {
//---------------------------------------------------------------------------

//! convert an ODE style quaternion to irrlicht euler angle rotation 
//! (angels are returned in degrees to match the 
//! ISceneNode::setRotation(vector3df &r) convention) adapted from
//! http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
void ODEQuaternionToEuler(const dQuaternion quaternion,core::vector3df &euler);

//! convert euler rotation angles to ODE style quaternions 
//! (angles are assumed to be in degrees to match the ISceneNode::getRotation() 
//! convention)  adapted from 
//! http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm   
void EulerToODEQuaternion(const core::vector3df &euler, dQuaternion quaternion);

//! conversion ODE quaternion to transformation matrix
void ODEQuaternionToMatrix(const dQuaternion &quat, core::matrix4 &m);

//! slerp interpolation two ODE quaternions
void ODEQuaternionSlerp(const dQuaternion &quat1, const dQuaternion &quat2, dQuaternion &quat_out, f32 factor);

//---------------------------------------------------------------------------
} // end namespace ode
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ODEHelperHPP