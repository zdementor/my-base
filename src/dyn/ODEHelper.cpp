//|-------------------------------------------------------------------------
//| File:        ODEHelper.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "ODEHelper.h"
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
namespace ode {
//---------------------------------------------------------------------------

//! convert an ODE style quaternion to irrlicht euler angle rotation 
//! (angels are returned in degrees to match the 
//! ISceneNode::setRotation(vector3df &r) convention) adapted from
//! http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
void ODEQuaternionToEuler(const dQuaternion quaternion,core::vector3df &euler)
{
	dReal w,x,y,z;

    w=quaternion[0]; 
	x=quaternion[1]; 
	y=quaternion[2]; 
	z=quaternion[3];

    dReal sqw = w*w, sqx = x*x, sqy = y*y, sqz = z*z; 

    // heading
	f32 arg1 = 2.0f * (x*y + z*w);
	f32 arg2 = (sqx - sqy - sqz + sqw);

	if (arg2==0) arg2=0.000001f;

    f32 radZ = (f32)atan2(arg1, arg2);
	euler.Z = (f32)core::RAD2DEG*(radZ);

    // attitude
    f32 arg = -2.0f * (x*z - y*w);

    if (arg<=-1) arg=-0.999999f; else if (arg>=1) arg=0.999999f;

    f32 radY = (f32)asin(arg);
	euler.Y = (f32)core::RAD2DEG*(radY);

    // bank
	arg1 = 2.0f * (y*z + x*w);
	arg2 = (-sqx - sqy + sqz + sqw);

	if (arg2==0) arg2=0.000001f;

    f32 radX = (f32)atan2(arg1, arg2);  
	euler.X = (f32)core::RAD2DEG*(radX);
}

//---------------------------------------------------------------------------

//! convert euler rotation angles to ODE style quaternions 
//! (angles are assumed to be in degrees to match the ISceneNode::getRotation() 
//! convention)  adapted from 
//! http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm   
void EulerToODEQuaternion(const core::vector3df &euler, dQuaternion quaternion)
{
    dReal _heading = (dReal)core::DEG2RAD*(euler.Z)/(dReal)2.0f;
    dReal _attitude= (dReal)core::DEG2RAD*(euler.Y)/(dReal)2.0f;
    dReal _bank    = (dReal)core::DEG2RAD*(euler.X)/(dReal)2.0f;

    dReal c1 = (dReal)cos(_heading);    
    dReal s1 = (dReal)sin(_heading);    
    dReal c2 = (dReal)cos(_attitude);    
    dReal s2 = (dReal)sin(_attitude);    
    dReal c3 = (dReal)cos(_bank);    
    dReal s3 = (dReal)sin(_bank);
    dReal c1c2 = c1*c2;    
    dReal s1s2 = s1*s2;     
    quaternion[0]=(dReal) (c1c2*c3  + s1s2*s3);  //w
    quaternion[1]=(dReal) (c1c2*s3  - s1s2*c3);  //x    
    quaternion[2]=(dReal) (c1*s2*c3 + s1*c2*s3); //y    
    quaternion[3]=(dReal) (s1*c2*c3 - c1*s2*s3); //z
}

//---------------------------------------------------------------------------

//! conversion ODE quaternion to transformation matrix
void ODEQuaternionToMatrix(const dQuaternion &q, core::matrix4 &m)
{
	f32 quat[4];

	quat[0] = q[1]; // x
	quat[1] = q[2]; // x
	quat[2] = q[3]; // x
	quat[3] = q[0]; // x

    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = quat[0] + quat[0];
    y2 = quat[1] + quat[1];
    z2 = quat[2] + quat[2];
    xx = quat[0] * x2;   xy = quat[0] * y2;   xz = quat[0] * z2;
    yy = quat[1] * y2;   yz = quat[1] * z2;   zz = quat[2] * z2;
    wx = quat[3] * x2;   wy = quat[3] * y2;   wz = quat[3] * z2;

    m(0,0)=1.0f-(yy+zz); m(0,1)=xy-wz;        m(0,2)=xz+wy;
    m(1,0)=xy+wz;        m(1,1)=1.0f-(xx+zz); m(1,2)=yz-wx;
    m(2,0)=xz-wy;        m(2,1)=yz+wx;        m(2,2)=1.0f-(xx+yy);

    m(0,3) = m(1,3) = m(2,3) = 0;
    m(3,0) = m(3,1) = m(3,2) = 0;
    m(3,3) = 1;
}

//---------------------------------------------------------------------------

//! slerp interpolation two ODE quaternions
void ODEQuaternionSlerp(const dQuaternion &quat1, const dQuaternion &quat2, dQuaternion &quat_out, f32 factor)
{
	f32 w1 = quat1[0]; 
	f32 x1 = quat1[1]; 
	f32 y1 = quat1[2]; 
	f32 z1 = quat1[3];

	f32 w2 = quat2[0]; 
	f32 x2 = quat2[1]; 
	f32 y2 = quat2[2]; 
	f32 z2 = quat2[3];

	core::quaternion q1(x1,y1,z1,w1);
	core::quaternion q2(x2,y2,z2,w2);

	core::quaternion q_out;
	
	q_out.slerp(q1, q2, factor);

	quat_out[0] = q_out.W;
	quat_out[1] = q_out.X;
	quat_out[2] = q_out.Y;
	quat_out[3] = q_out.Z;
}

//---------------------------------------------------------------------------
} // end namespace ode
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------
