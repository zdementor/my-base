//|-------------------------------------------------------------------------
//| File:        CDynamicManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CDynamicManager.h"
#include "CDynamicObject.h"

#include <scn/ICameraSceneNode.h>
#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>
#include <game/IGameTasksManager.h>
#include <os/ICriticalSection.h>
#include <os/ITimer.h>
#include <os/ITimer.h>
#include <io/ILogger.h>
#include <os/IEvent.h>
#include <io/IFileSystem.h>
#include <io/IWriteFile.h>
#include <game/IGameNodeAI.h>
#include <game/IGameEventsDispatcher.h>

#include "ODEHelper.h"

//---------------------------------------------------------------------------
#ifdef _MY_USE_ODE_DYNAMIC_
//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//---------------------------------------------------------------------

CDynamicManager::CDynamicManager() 
	: 
theWorld(0),theSpace(0), theJointGroup(0), StoppingCalcDynamic(false), 
EnabledBodies(0), m_DynamicDeltaTimeSec(0)
{	
#if MY_DEBUG_MODE  
	setClassName("CDynamicManager");    
#endif

	dInitODE();
  
    theWorld = dWorldCreate();
	//theSpace = dHashSpaceCreate(0);
	theSpace = dSimpleSpaceCreate(0);

    theJointGroup = dJointGroupCreate(0);   

	// create ray
    ray_geom = dCreateRay (theSpace, 100);	

	// some initialization
	m_ForceInterolator.init(0.0333f, 7500, 0.0666f, 3500);   

	// default world params
	SDynamicWorldParams dyn_params;
	dyn_params.Gravity = core::vector3df(0.0f, -9.8f*80.0f, 0.0f);
	dyn_params.WorldCFM = 0.1f;
	dyn_params.WorldERP = 0.8f;

	setParams(dyn_params);
}

//---------------------------------------------------------------------

CDynamicManager::~CDynamicManager()
{
	removeAllObjects();

	dGeomDestroy(ray_geom);
    dJointGroupDestroy(theJointGroup);
    dSpaceDestroy(theSpace);
    dWorldDestroy(theWorld);

    dCloseODE();      
}

//---------------------------------------------------------------------

s32 CDynamicManager::getEnabledDynamicObjectsCount()
{
	return EnabledBodies;
}

//---------------------------------------------------------------------

s32 CDynamicManager::getDynamicObjectsCount()
{
	return CDynamicObject::DynamicObjects.size();
}

//---------------------------------------------------------------------

// функция, вызывается ODE, когда он решит, что между обьектами
// скоро произойдет столкновение
void CDynamicManager::nearCollisionCallback(
	void* data, // указатель на менеджера динамики
    dGeomID o1, // первый обьект, учавствующий в столкновении 
    dGeomID o2  // второй обьект, учавствующий в столкновении
    )
{  
    //менеджер ODE
    CDynamicManager* odeMngr = (CDynamicManager*)data;  

	// ODE bodies
    dBodyID b1=dGeomGetBody(o1);
    dBodyID b2=dGeomGetBody(o2);	

	// ODE dynamic objects
    IDynamicObject *obj1 = (IDynamicObject*)dGeomGetData(o1);
    IDynamicObject *obj2 = (IDynamicObject*)dGeomGetData(o2);

	if ((!obj1 || !obj2) ||
		(!b1 && !b2) || 
		(!b1 && dBodyIsEnabled(b2)==0) ||
		(!b2 && dBodyIsEnabled(b1)==0) ||
		(b1 && b2 && dBodyIsEnabled(b1)==0 && dBodyIsEnabled(b2)==0) ||
		(b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact))
		)
		return; 

	scn::ISceneNode  *snode =  (scn::ISceneNode*)obj1->getSceneNode();
    scn::ISceneNode  *snode2 = (scn::ISceneNode*)obj2->getSceneNode();
	
	// game nodes
    game::IGameNode  *game_node  = snode ? snode->getGameNode() : 0;
    game::IGameNode  *game_node2 = snode2 ? snode2->getGameNode() : 0;

	const SDynamicObjectParams& obj1_par = obj1->getParameters();  
	const SDynamicObjectParams& obj2_par = obj2->getParameters();  	

	if (!(obj1->getCollisionFilter()&DynamicObjectCollidFilter[obj2_par.Type][0])||
		!(obj2->getCollisionFilter()&DynamicObjectCollidFilter[obj1_par.Type][0])		
		)
		return;	

	static dContact contact[32];

	s32 MAX_CONTACTS = 16;

	if ((game_node && game_node->isMainPlayer()) || 
		(game_node2 && game_node2->isMainPlayer())
		)
	{
		MAX_CONTACTS = 32;
	}
	else if (
		game_node && game_node2 &&
		game_node->getGameNodeType()==game::EGNT_PERSON && 
		game_node2->getGameNodeType()==game::EGNT_PERSON
		)
	{
		MAX_CONTACTS = 1;
	}	

    dReal SurfMu = (dReal)(dInfinity*obj1_par.SurfMu);

    for(s32 i=0; i<MAX_CONTACTS; i++)
    {
        // contact settings
		if (i==0)
		{
			contact[i].surface.mode       = dContactBounce | dContactSoftCFM;
			contact[i].surface.mu         = SurfMu;
			contact[i].surface.bounce     = (dReal)(obj1_par.SurfBounce);
			contact[i].surface.bounce_vel = (dReal)(obj1_par.SurfBounceVel);
			contact[i].surface.soft_cfm   = (dReal)(obj1_par.SurfSoft);
		}
		else
		{
			memcpy(&contact[i], &contact[0], sizeof(dContact));
		}
    }

    // ask ode to find collision points
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));    

    if(numc>0)
    {
		if (game_node && game_node2
				&& obj1_par.Type != EDOT_MONOLITH
				&& obj2_par.Type != EDOT_MONOLITH)
		{        
			const core::aabbox3df& bb = game_node->getTransformedBBox();
			const core::aabbox3df& bb2= game_node2->getTransformedBBox();

			game::SGameEvent *event  = new game::SGameEvent( game::EGET_PHYSIC, game_node,  game_node2, bb );
			game::SGameEvent *event2 = new game::SGameEvent( game::EGET_PHYSIC, game_node2, game_node,  bb2);

			GAME_EVENTS_DISPATCHER.registerGameEvent(event);
			GAME_EVENTS_DISPATCHER.registerGameEvent(event2);	
		}

		if (obj1_par.Type == EDOT_SENSELESS || obj2_par.Type == EDOT_SENSELESS)
		{
			return;
		} 

        // так как есть точки контакта, то мы уже не падаем

		obj1->getIsFalling() = false;
		obj2->getIsFalling() = false;    

        // создаем связи
        for(s32 i=0;i<numc;i++)
        {   
            dJointID joint = dJointCreateContact(
                odeMngr->theWorld, odeMngr->theJointGroup, &contact[i]
				);
            dJointAttach(joint, b1, b2);
        }   
    }
} 

//---------------------------------------------------------------------------

//! find collisions of this ray with world
bool CDynamicManager::findCollisionsWithRay(
    core::line3d<f32> ray, s32 collid_filter, SCollisionPoints &collisions,
    core::array<scn::ISceneNode*> *excludeNodesList
    )
{ 
	bool collided = false; 	

    // Set the length of the given ray. 
    dGeomRaySetLength (ray_geom, (dReal)ray.getLength());

    // Set the starting position and direction of the given ray.
    core::vector3df dir = ray.getVector();
    dir.normalize();
    dGeomRaySet( 
        ray_geom, 
        (dReal)ray.start.X, (dReal)ray.start.Y, (dReal)ray.start.Z, 
        (dReal)dir.X, (dReal)dir.Y, (dReal)dir.Z
        );   	
	collisions.clear();	

	for (s32 f=0; f<E_DYNAMIC_OBJECT_TYPE_COUNT; f++)
	{
		E_DYNAMIC_OBJECT_TYPE  dtype   = E_DYNAMIC_OBJECT_TYPE(f);
		s32             dfilter = DynamicObjectCollidFilter[f][0];

		if ((dfilter&collid_filter)==0)
			continue;

		core::list<IDynamicObject*>::iterator iter = 
			CDynamicObject::DynamicObjectsByType[dtype].begin();   

		for	(; iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter)
		{
			IDynamicObject  *dobj  = *iter;   
			scn::ISceneNode *snode = NULL;	
			game::IGameNode *gnode = NULL;

			if (!(snode=dobj->getSceneNode()) || !(gnode=snode->getGameNode()) || 
				!snode->isVisible() 
				) 
				continue;
    
			const SDynamicObjectParams& dpar = dobj->getParameters();
				
			if (excludeNodesList && excludeNodesList->binary_search(snode)!=-1 )
				continue;

			dGeomID g = (dGeomID)dobj->getGeomID();
       
			const s32 MAX_CONTACTS = 16; 

			static dContact contact[MAX_CONTACTS];    

			int numc=dCollide(ray_geom, g, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));       

			if (numc>0) 
			{
				collided = true;            
        
				// store collision points

				static core::vector3df collisionPoint, collisionNormal;
				
				for (s32 c=0; c<numc; c++)
				{
					collisionPoint.set(
						contact[c].geom.pos[0], contact[c].geom.pos[1], contact[c].geom.pos[2]
						);
					collisionNormal.set(
						contact[c].geom.normal[0], contact[c].geom.normal[1], contact[c].geom.normal[2]
						); 
					collisions.add(SCollisionPoint(collisionPoint, collisionNormal, dobj));
				}
			}		
		}
	}
	
    return collided;
}

//---------------------------------------------------------------------------

//! find collisions of this ray with dynamic object

bool CDynamicManager::findCollisionsWithRay(
	IDynamicObject *obj, core::line3d<f32> ray, SCollisionPoints &collisions
    )
{	
	bool collided = false; 	

	IDynamicObject  *o   = obj;   
	scn::ISceneNode *node= NULL;

    if (!o || !(node=o->getSceneNode()) || !node->isVisible()) 
		return collided;

    // Set the length of the given ray. 
    dGeomRaySetLength (ray_geom, (dReal)ray.getLength());

    // Set the starting position and direction of the given ray.
    core::vector3df dir = ray.getVector();
    dir.normalize();
    dGeomRaySet( 
        ray_geom, 
        (dReal)ray.start.X, (dReal)ray.start.Y, (dReal)ray.start.Z, 
        (dReal)dir.X, (dReal)dir.Y, (dReal)dir.Z
        );   
	
	collisions.clear();
   
	dGeomID g = (dGeomID)o->getGeomID();
       
	const s32 MAX_CONTACTS = 16; 

	static dContact contact[MAX_CONTACTS];    

    // находим точки столкновения
	int numc = dCollide(ray_geom, g, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));       

    if (numc>0) 
    {
		collided = true;            
        
        // storing collision points
        core::vector3df collisionPoint, collisionNormal;
        for (s32 c=0; c<numc; c++)
        {
			collisionPoint = core::vector3df(
				contact[c].geom.pos[0], contact[c].geom.pos[1], contact[c].geom.pos[2]
                );
			collisionNormal = core::vector3df(
				contact[c].geom.normal[0], contact[c].geom.normal[1], contact[c].geom.normal[2]
				); 
            collisions.add(SCollisionPoint(collisionPoint, collisionNormal, o));
        }		
    } 
        
    return collided;
}

//---------------------------------------------------------------------------

//! check intersections of this ray with world
bool CDynamicManager::checkIntersectionWithRay(
    core::line3d<f32> ray, s32 collid_filter, 
	core::array<scn::ISceneNode*> *excludeNodesList
    )
{   
    // Set the length of the given ray. 
    dGeomRaySetLength (ray_geom, (dReal)ray.getLength());

    // Set the starting position and direction of the given ray.

	core::vector3df dir = ray.getVector();
	dir.normalize();

    dGeomRaySet( 
        ray_geom, 
        (dReal)ray.start.X, (dReal)ray.start.Y, (dReal)ray.start.Z, 
        (dReal)dir.X, (dReal)dir.Y, (dReal)dir.Z
        );    

	for (s32 f=0; f<E_DYNAMIC_OBJECT_TYPE_COUNT; f++)
	{
		E_DYNAMIC_OBJECT_TYPE  dtype   = E_DYNAMIC_OBJECT_TYPE(f);
		s32             dfilter = DynamicObjectCollidFilter[f][0];

		if ((dfilter&collid_filter)==0)
			continue;

		core::list<IDynamicObject*>::iterator iter = 
			CDynamicObject::DynamicObjectsByType[dtype].begin();   

		for	(; iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter)
		{
			
			IDynamicObject  *dobj  = *iter;   
			scn::ISceneNode *snode = NULL;	
			game::IGameNode *gnode = NULL;

			if (!dobj || !(snode=dobj->getSceneNode()) || !(gnode=snode->getGameNode()) || 
				!snode->isVisible()
				) 
				continue;	

			if ( excludeNodesList && excludeNodesList->binary_search(snode)!=-1 )	
			{
				continue;
			}		
		
			const SDynamicObjectParams& dpar = dobj->getParameters();

			dGeomID g = (dGeomID)dobj->getGeomID(); 

			const int MAX_CONTACTS = 1; 
			static dContact contact[MAX_CONTACTS];    

			int numc=dCollide( ray_geom, g, MAX_CONTACTS, &contact[0].geom, sizeof(dContact) );

			if (numc>0) 
			{
				return true;
			}
		}
    }  	
        
    return false;
}

//---------------------------------------------------------------------------

//! translate ODE-object through this dynamic-world by the force
void CDynamicManager::translateObjectThroughTheWorld(
	IDynamicObject *obj, core::vector3df force
    )
{
	if (!obj || force.getLength() <= 0.0f)
		return;		

	//force.Y =0;

	scn::ISceneNode *node = obj->getSceneNode(); 	

    if (!node) 
		return;

	game::IGameNode *gnodeai = SAFE_CAST_TO_GAME_NODE_AI(node->getGameNode()); 

	f32 koeff_attenuation_common = 1.0f;
	
	if (gnodeai)
	{
		f32 koeff_attenuation_0 = 1.0f;

		core::vector3df orig = 
			obj->getBasePoint() + ( obj->getHighPoint() - obj->getBasePoint() )*0.333f;

		if (obj->getIsFalling() && obj->getHeightOverGround()>25.0f)
		{
			s32 now = TIMER.getTime();

			f32 fall_time = core::math::Abs(obj->getFallingStartTime()-now);

			koeff_attenuation_0 = 1.0f - fall_time/250;

			CHECK_RANGE(koeff_attenuation_0, 0, 1);
		}	

		if (koeff_attenuation_0 == 0)
			return;
	
		f32 koeff_attenuation_1 = 1.0f;

		if (node->isMainPlayer())
		// calculaing koeff of attenuation (depending on test rays)	
		{
			core::vector3df force_xz_proj(force.X,0,force.Z);

			// perfoming testing on test rays

			core::array< core::line3d<f32> > &test_rays = obj->getTestRays();
			
			f32 test_angle_step = 11;
			f32 min_test_angle = -((test_rays.size()/2)*test_angle_step);

			u32 i=0;

			core::matrix4 m;
			core::vector3df rot_vec;			
			
			u32 nMonolith = 0;

			f32 LimitAngle = 30;

			for (i=0; i<test_rays.size(); i++)
			{
				m.setRotationDegrees( core::vector3df(0,180+min_test_angle+i*test_angle_step,0) ); 
				rot_vec = force_xz_proj;
				m.rotateVect(rot_vec);
				rot_vec.normalize();
				rot_vec *= 1000;
				test_rays[i].start = orig;
				test_rays[i].end   = orig + rot_vec;
				
				SCollisionPoints collisions;
				
				// collisions with monolith objects

				if (findCollisionsWithRay(test_rays[i], EDCF_MONOLITH_COLLID, collisions))
				{
					s32 idx = collisions.getNearestTo(orig);
					test_rays[i].end = collisions.getPoints()[idx].Pos;
					core::vector3df ray = test_rays[i].end - obj->getBasePoint();
					f32 angle = (f32)ray.getAngleDeg(test_rays[i].getVector());
					if (angle > LimitAngle) 
						nMonolith++;
					collisions.clear();
				}
			}
			
			koeff_attenuation_1 = 1.0f - (f32)nMonolith/(f32)test_rays.size();
			
			CHECK_RANGE(koeff_attenuation_1, 0.0f, 1.0f);
		}		
		
		koeff_attenuation_common = 	
			koeff_attenuation_0 * koeff_attenuation_1;

		if (koeff_attenuation_common == 0)
			return;
	}
	
	// apply our force

	f32 k = m_ForceInterolator.getValue(m_DynamicDeltaTimeSec);

	force *= k * koeff_attenuation_common;	 

	obj->addForce(force);
}

//---------------------------------------------------------------------------

//! pre dynamic call
void CDynamicManager::preDynamic()
{
	if (StoppingCalcDynamic) 
		return;		
    
    IDynamicObject    *obj;
    dBodyID            body=0;
	scn::ISceneNode   *node=0;

    dReal              *ode_pos;
    dReal              *ode_rot;

    dReal              last_ode_pos[3];

    core::vector3df    euler, last_euler;
	
	static f32 start_falling=0;

	static bool falling_before;
	static bool falling_after;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];

		core::list<IDynamicObject*>::iterator iter = 
			CDynamicObject::DynamicObjectsByType[dtype].begin();

		for	(; iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter)
		{
			obj=*iter;			

			if (!(body=((dBodyID)obj->getBodyID())) || 
				!(node = obj->getSceneNode()) ||			
				!dBodyIsEnabled(body)
				)
				continue;

			dReal* lin_vel = (dReal*)dBodyGetLinearVel(body);			
		
			if (node->isMainPlayer())
				falling_before = obj->getIsFalling();

			obj->getHeightOverGround() = 0;

			// ODE object params

			const SDynamicObjectParams &par = obj->getParameters();

			// remember ODE position and rotation

			ode_pos = (dReal*)dBodyGetPosition(body);        
			ode_rot = (dReal*)dBodyGetQuaternion(body);

			last_ode_pos[0] = obj->getDynLastPos().X;
			last_ode_pos[1] = obj->getDynLastPos().Y;
			last_ode_pos[2] = obj->getDynLastPos().Z;
       
			core::aabbox3d<f32> ode_aabbox = getDynamicObjectAABBox (obj);
        
			obj->getBasePoint().X = ode_pos[0];
			obj->getBasePoint().Y = ode_aabbox.MinEdge.Y;
			obj->getBasePoint().Z = ode_pos[2];

			obj->getHighPoint().X = ode_pos[0];
			obj->getHighPoint().Y = ode_aabbox.MaxEdge.Y;
			obj->getHighPoint().Z = ode_pos[2];	
			
			obj->getDynLastPos().X = ode_pos[0];
			obj->getDynLastPos().Y = ode_pos[1];
			obj->getDynLastPos().Z = ode_pos[2];

			obj->getDynLastRot().W = ode_rot[0];
			obj->getDynLastRot().X = ode_rot[1];
			obj->getDynLastRot().Y = ode_rot[2];
			obj->getDynLastRot().Z = ode_rot[3];

			// slow down linear velosity (to avoid continuous sliping)

			game::IGameNode* game_node = node->getGameNode();			
				
			if (game_node && (
					game_node->getGameNodeType()==game::EGNT_PERSON || 
					game_node->getGameNodeType()==game::EGNT_MAIN_PLAYER 
					)
				)
			{
				lin_vel[0] *= 0.75;
				lin_vel[2] *= 0.75;
			}
			else
			{
				lin_vel[0] *= 0.985;
				lin_vel[2] *= 0.985;
			}

			// check if can ODE rotate objects or not

			if (!(par.DynamicConstraints&EDTC_MOVX)) lin_vel[0] = (dReal)0;
			if (!(par.DynamicConstraints&EDTC_MOVY)) lin_vel[1] = (dReal)0;
			if (!(par.DynamicConstraints&EDTC_MOVZ)) lin_vel[2] = (dReal)0;
            
			dBodySetLinearVel( body, lin_vel[0], lin_vel[1], lin_vel[2] );
        
			// controlling linear velosity        

			dReal* ang_vel = (dReal*)dBodyGetAngularVel(body);  
			
			// slow down angular velosity (to avoid continuous sliping)

			if (game_node && 
				game_node->getGameNodeType()!=game::EGNT_PERSON && 
				game_node->getGameNodeType()!=game::EGNT_MAIN_PLAYER 
				)
			{
				ang_vel[0] *= 0.995;
				ang_vel[1] *= 0.9;
				ang_vel[2] *= 0.995;  
			}

			f32 amax=5, amin=-5;

			if (ang_vel[0]>amax) ang_vel[0]=amax; else if (ang_vel[0]<amin) ang_vel[0]=amin;
			if (ang_vel[1]>amax) ang_vel[1]=amax; else if (ang_vel[1]<amin) ang_vel[1]=amin;
			if (ang_vel[2]>amax) ang_vel[2]=amax; else if (ang_vel[2]<amin) ang_vel[2]=amin;

			// check if can ODE rotate objects or not
        
			if (!(par.DynamicConstraints&EDTC_ROTX)) ang_vel[0] = (dReal)0.0f;
			if (!(par.DynamicConstraints&EDTC_ROTY)) ang_vel[1] = (dReal)0.0f;
			if (!(par.DynamicConstraints&EDTC_ROTZ)) ang_vel[2] = (dReal)0.0f;
       
			dBodySetAngularVel(body, ang_vel[0], ang_vel[1], ang_vel[2]);           
        
			// setting initial variables values

			obj->getIsFalling() = true;    		
		}
    }	
}

//---------------------------------------------------------------------------

//! dynamic calculation
void CDynamicManager::calcDynamic(f64 delta_time_sec)
{	
	if (StoppingCalcDynamic) 
		return;	

	m_DynamicDeltaTimeSec = delta_time_sec;

	// run the simulation

	// build the collision joints for all objects in 'theSpace'
	dSpaceCollide(theSpace, this, &nearCollisionCallback);  

	// make a simulation step for 'theWorld'
	dWorldQuickStep (theWorld, m_DynamicDeltaTimeSec);
	
	// clean up all joints from collisions;
	dJointGroupEmpty(theJointGroup);
}

//---------------------------------------------------------------------------

//! post dynamic call
void CDynamicManager::postDynamic()
{
	if (StoppingCalcDynamic) 
		return;	

	s32 now = TIMER.getTime();
	
	IDynamicObject    *obj;
    dBodyID            body=0;
	scn::ISceneNode   *node=0;

	dReal              *ode_pos;
    dReal              *ode_rot;

	dReal              last_ode_pos[3];
    dReal              last_ode_rot[4];

	core::vector3df    euler, last_euler;
    dMatrix3           quater;  	

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];

		core::list<IDynamicObject*>::iterator iter = 
			CDynamicObject::DynamicObjectsByType[dtype].begin();
		
		for	(; iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter)
		{
			obj=*iter;

			if (!(body=((dBodyID)obj->getBodyID())) || 				
				!(node = obj->getSceneNode()) ||
				!dBodyIsEnabled(body)
				)
				continue;

			const dReal* lin_vel = (dReal*)dBodyGetLinearVel(body);	
 
			// параметры обьекта ODE
			SDynamicObjectParams par = obj->getParameters();

			// remember ODE position and rotation
			ode_pos = (dReal*)dBodyGetPosition(body);        
			ode_rot = (dReal*)dBodyGetQuaternion(body);

			last_ode_pos[0] = obj->getDynLastPos().X;
			last_ode_pos[1] = obj->getDynLastPos().Y;
			last_ode_pos[2] = obj->getDynLastPos().Z;

			last_ode_rot[0] = obj->getDynLastRot().W;
			last_ode_rot[1] = obj->getDynLastRot().X;
			last_ode_rot[2] = obj->getDynLastRot().Y;
			last_ode_rot[3] = obj->getDynLastRot().Z;

			// определяем состояние

			if      (!obj->getIsFalling()  &&  obj->getIsFallingOnLastCalc()) obj->getState() = EDOS_STOP_FALLING;
			else if ( obj->getIsFalling()  && !obj->getIsFallingOnLastCalc()) obj->getState() = EDOS_START_FALLING;
			else if ( obj->getIsFalling())                                    obj->getState() = EDOS_FALLING;
			else                                                              obj->getState() = EDOS_WALKING;

			core::vector3df speed(
				ode_pos[0]-last_ode_pos[0], ode_pos[1]-last_ode_pos[1], ode_pos[2]-last_ode_pos[2]
				);

			f32 speed_length = speed.getLength();

			if (speed_length<0.25f || ode_pos[1]<-1000.0f)
			// to avoid slow sliping over ground and unstability
			{
				ode_pos[0] = last_ode_pos[0];
				ode_pos[1] = last_ode_pos[1];
				ode_pos[2] = last_ode_pos[2];
			}

			// check if we can move objects or not 

			if (!(par.DynamicConstraints&EDTC_MOVX)) ode_pos[0] = last_ode_pos[0];
			if (!(par.DynamicConstraints&EDTC_MOVY)) ode_pos[1] = last_ode_pos[1];
			if (!(par.DynamicConstraints&EDTC_MOVZ)) ode_pos[2] = last_ode_pos[2];  

			dBodySetPosition(body, ode_pos[0], ode_pos[1], ode_pos[2]); 
        
			// check if we can rotate objects or not	

			if (!(par.DynamicConstraints&EDTC_ROTX) || 
				!(par.DynamicConstraints&EDTC_ROTY) ||
				!(par.DynamicConstraints&EDTC_ROTZ)
				)
			{
				last_euler = obj->getMyLastRot() + obj->getDynTurnoff();

				ode::EulerToODEQuaternion (last_euler, quater);
        
				dBodySetQuaternion(body, quater); 
			}

			// чтобы игрок не подскакивал как шарик
			if (obj->getState() == EDOS_FALLING || 
				obj->getState() == EDOS_STOP_FALLING || 
				obj->getState() == EDOS_START_FALLING			 
				) 
			{             
				if (lin_vel[1]>0) 
				{
					dBodySetLinearVel(body, lin_vel[0], 0, lin_vel[2]);  
				}
			}  	
			
			if (obj->getIsFalling() && !obj->getIsFallingOnLastCalc())
			{
				obj->getFallingStartTime() = TIMER.getTime();
			}			
			
			if (node->isMainPlayer() && obj->getIsFalling())
			{
				SCollisionPoints collisions;
				core::line3d<f32> ray;

				ray.start = obj->getBasePoint();
				ray.end = ray.start + core::vector3df(0.0f ,-1.0f, 0.0f)*1000000.0f;
				
				// collisions with monolith objects
				f32 h = 0.f;
				if (findCollisionsWithRay(ray,
						EDCF_MONOLITH_COLLID | EDCF_PSEUDO_DYNAMIC_COLLID, collisions))
				{
					s32 idx = collisions.getNearestTo(ray.start);
					h = ray.start.Y - collisions.getPoints()[idx].Pos.Y;
				}
				if (h > 0)
					obj->getHeightOverGround() = h;
			}
			else
			{
				obj->getHeightOverGround() = 0;
			}

			obj->getIsFallingOnLastCalc() = obj->getIsFalling();
		}
	}
}

//---------------------------------------------------------------------------

// let all bouncables have the new position and rotation after 
// physics have been applied    
void CDynamicManager::updateGeometryFromDynamic()
{	
	if (StoppingCalcDynamic) 
		return;	

	core::list<IDynamicObject*>::iterator iter=NULL;
	IDynamicObject  *obj;
	core::vector3df  irr_pos;
	core::vector3df  irr_rot;
	dReal           *ode_pos;
	dReal           *ode_rot;    
	dBodyID          body=0;
	scn::ISceneNode *node=0;
	core::vector3df  ode_vel;	

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	EnabledBodies = 0;

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];

		core::list<IDynamicObject*>::iterator iter = 
			CDynamicObject::DynamicObjectsByType[dtype].begin();
		
		for	(; iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || 				
				!(node = obj->getSceneNode()) ||			
				!dBodyIsEnabled(body)
				)
				continue;			

			// get the new position of the ODE body
			ode_pos = (dReal*)dBodyGetPosition(body);
			// get the new rotation of the ODE body    
			ode_rot = (dReal*)dBodyGetQuaternion(body);

			// convert ODE transformations to Irrlicht
			irr_pos.set((f32)ode_pos[0],(f32)ode_pos[1],(f32)ode_pos[2]);            
			ode::ODEQuaternionToEuler(ode_rot, irr_rot);
	        
			// next Irrlicht orientation
			core::vector3df next_rot = irr_rot - obj->getDynTurnoff();     
			core::vector3df next_pos = irr_pos - obj->getDynDisplace(); 

			const SDynamicObjectParams& par   = obj->getParameters();
			core::vector3df last_euler = obj->getMyLastRot();

			if (!(par.DynamicConstraints&EDTC_ROTX) ||
				!(par.DynamicConstraints&EDTC_ROTY) ||
				!(par.DynamicConstraints&EDTC_ROTZ)
				)
			{
				next_rot = last_euler;
			}

			// set the node rotation 
			node->RelativeRotation     = next_rot; 			
			obj->getMyLastRot()        = next_rot;

			// set the position at the scenenode
			node->RelativeTranslation    = next_pos;
			obj->getMyLastPos()          = next_pos;

			obj->getDynLastPos().X = ode_pos[0];
			obj->getDynLastPos().Y = ode_pos[1];
			obj->getDynLastPos().Z = ode_pos[2];

			obj->getDynLastRot().W = ode_rot[0];
			obj->getDynLastRot().X = ode_rot[1];
			obj->getDynLastRot().Y = ode_rot[2];
			obj->getDynLastRot().Z = ode_rot[3];
			
			node->m_TransformationChanged = true;
			node->updateAbsoluteTransformation();
				
			if (obj->getAutoUpdateGeomParams())
			{
				obj->updateGeomParams();
			}

			obj->setMoved(false);	
			
			dBodySetAutoDisableFlag(body, 1);
			dBodySetAutoDisableLinearThreshold(body,  1.0f);
			dBodySetAutoDisableAngularThreshold(body, 0.1f);
			dBodySetAutoDisableSteps(body, 1);	
			
			const dReal* lin_vel = (dReal*)dBodyGetLinearVel(body);
			
			if (core::vector3df(lin_vel[0], lin_vel[1], lin_vel[2]).getLength()<1.0f)
			{
				dBodyDisable(body);
			}

			EnabledBodies++;
		}
	}
}

//---------------------------------------------------------------------------

// add ODE object
IDynamicObject* CDynamicManager::addObjectForSceneNode(
	scn::ISceneNode* n, SDynamicObjectParams &params
    )
{
	if (params.Type==EDOT_NONE)
	{
		return NULL;
	}

    CDynamicObject* b = new CDynamicObject(theWorld, theSpace, n, params);    	

    return (IDynamicObject*)b;
}

//---------------------------------------------------------------------------

//! destroy ODE object
bool CDynamicManager::removeObjectForSceneNode(scn::ISceneNode *n)
{
	bool res = false;

	if (!n) return res;	

	IDynamicObject *obj = n->getDynamicObject();

	if (obj) res = obj->drop();	

	return res;
}

//---------------------------------------------------------------------------

//! destroy all ODE objects
void CDynamicManager::removeAllObjects()
{
	core::stringc msg;

	if (CDynamicObject::DynamicObjects.size() != 0)
	{
		LOGGER.log("Not all Dynamic Objects deleted!", io::ELL_ERROR);
	}
	
    core::list<IDynamicObject*>::iterator iter = CDynamicObject::DynamicObjects.begin();
	IDynamicObject* obj = NULL;

    for(; iter != CDynamicObject::DynamicObjects.end();)
    {
		obj=(*iter);

		if (!obj->drop())
		{			
			msg. sprintf("Can't drop CDynamicObject %d", obj);
			LOGGER.log(msg, io::ELL_ERROR);

			++iter;	
		}
		else
			iter = CDynamicObject::DynamicObjects.begin();
    }
}

//----------------------------------------------------------------------------

// set ODE World Params
void CDynamicManager::setParams(SDynamicWorldParams &params)
{   
    Parameters.set(params);

	// Set the global CFM (constraint force mixing) value. 
	// Typical values are in the range 10-9 -- 1. 
	// The default is 10-5 if single precision is being used, 
 	// or 10-10 if double precision is being used. 
    dWorldSetCFM (theWorld, Parameters.WorldCFM);

	// Set and get the global ERP value, that controls how much error 
	// correction is performed in each time step. Typical values are in the range 0.1--0.8. 
	// The default is 0.2. 
    dWorldSetERP (theWorld, Parameters.WorldERP);

	// Set the depth of the surface layer around all geometry objects. 
	// Contacts are allowed to sink into the surface layer up to the given depth 
	// before coming to rest. The default value is zero. 
	// Increasing this to some small value (e.g. 0.001) can help prevent 
	// jittering problems due to contacts being repeatedly made and broken
    dWorldSetContactSurfaceLayer (theWorld,0.25f); 

	// Reducing this value can help prevent "popping" of deeply embedded objects. 
	// ( The default value is infinity (i.e. no limit). )
	dWorldSetContactMaxCorrectingVel(theWorld, 1000.0f);

	// Set the number of iterations that the QuickStep method performs per step. 
	// More iterations will give a more accurate solution, but will take longer to compute. 
	// The default is 20 iterations.
    dWorldSetQuickStepNumIterations (theWorld, 20); 

	// Set the default auto-disable parameters for newly created bodies. 
	// The default parameters are: 
	// AutoDisableFlag = disabled 
	// AutoDisableLinearThreshold = 0.01 
	// AutoDisableAngularThreshold = 0.01 
	// AutoDisableSteps = 10 
	// AutoDisableTime = 0 
    dWorldSetAutoDisableFlag (theWorld,1); 

	// setting Dynamic World Gravity
	setGravity(Parameters.Gravity);	
} 

//----------------------------------------------------------------------------

//! getting dynamic world gravity
core::vector3df CDynamicManager::getGravity()
{
	return Parameters.Gravity;
}

//----------------------------------------------------------------------------

//! setting dynamic world gravity
void CDynamicManager::setGravity(core::vector3df grav)
{
	Parameters.Gravity = grav;

	dWorldSetGravity(theWorld, grav.X, grav.Y, grav.Z);	
}

//----------------------------------------------------------------------------

//! stopping calculating dynamic
void CDynamicManager::stopCalcDynamic()
{
	StoppingCalcDynamic = true;
}

//----------------------------------------------------------------------------

//! resuming calculating dynamic
void CDynamicManager::resumeCalcDynamic()
{
	core::list<IDynamicObject*>::iterator iter=NULL;
	dGeomID            geom = 0;
    dBodyID            body = 0;
	IDynamicObject    *obj  = NULL;

    for	(	iter = CDynamicObject::DynamicObjects.begin(); 
			iter != CDynamicObject::DynamicObjects.end(); ++iter
		)
    {
        if (!(obj=*iter) || 
			!(body=((dBodyID)obj->getBodyID())) || 
			!(geom=((dGeomID)obj->getGeomID()))
			) continue;

		dBodyEnable(body);
	}

	StoppingCalcDynamic = false;	
}

//----------------------------------------------------------------------------

//! is calculating dynamic or not
bool CDynamicManager::isCalcDynamic()
{
	return !StoppingCalcDynamic;
}

//---------------------------------------------------------------------------

// moving dynamic object
void CDynamicManager::setDynamicObjectPosition(
    IDynamicObject* dynobj, const core::vector3df &pos, bool controlled_by_ai
    )
{
	if(!dynobj)
		return;
			
	if (!controlled_by_ai)
	{   
		scn::ISceneNode* node = dynobj->getSceneNode();
		dBodyID body = (dBodyID)dynobj->getBodyID();  

		if (node)
		{        			
			dGeomID geom = (dGeomID)dynobj->getGeomID(); 

			core::vector3df new_pos = pos + dynobj->getDynDisplace();	
			
			if (geom) 
			{
				dGeomSetPosition(geom, new_pos.X, new_pos.Y, new_pos.Z);
			}

			if (body)
			{
				dBodySetPosition(body, new_pos.X, new_pos.Y, new_pos.Z);
				dBodyEnable(body);
			}

			dynobj->storeFirstDynTransformationForInterpolation();
			dynobj->storeSecondDynTransformationForInterpolation();

			dynobj->getMyLastPos()    = pos;

			node->RelativeTranslation = pos;
			node->m_TransformationChanged = true;
			node->updateAbsoluteTransformation();
		}
	}
	else
	{
		core::vector3df force = ( pos - dynobj->getMyLastPos() ); 

		translateObjectThroughTheWorld(dynobj, force);   		 			
	}
}

//---------------------------------------------------------------------------     

// rotating dynamic object
void CDynamicManager::setDynamicObjectRotation(
    IDynamicObject* dynobj,const core::vector3df &rot, bool controlled_by_ai
    )
{   
	if(!dynobj)
		return;
	
	scn::ISceneNode* node = dynobj->getSceneNode();

	if (node)
	{
		dBodyID body = (dBodyID)dynobj->getBodyID(); 
		dGeomID geom = (dGeomID)dynobj->getGeomID();	

		static dQuaternion quaternion; 

		ode::EulerToODEQuaternion(
			rot + dynobj->getDynTurnoff(), quaternion
			);    

		if (geom) 
		{
			dGeomSetQuaternion(geom, quaternion);		
		} 
			
		if (body) 
		{
			dBodySetQuaternion(body, quaternion);
			dBodyEnable(body);
		}

		if (!controlled_by_ai)
		{
			dynobj->storeFirstDynTransformationForInterpolation();
			dynobj->storeSecondDynTransformationForInterpolation();  
		}

		dynobj->getMyLastRot() = rot;

		node->RelativeRotation = rot;
		node->m_TransformationChanged = true;
		node->updateAbsoluteTransformation();
	}	
}

//---------------------------------------------------------------------------     

// scaling dynamic object
void CDynamicManager::setDynamicObjectScale(IDynamicObject* dynobj,const core::vector3df &scale)
{
	if(!dynobj) return; 	

    dynobj->updateGeomData();	
}

//---------------------------------------------------------------------------

// returns dynamic oject axis aligned bounding box
core::aabbox3d<f32> CDynamicManager::getDynamicObjectAABBox(IDynamicObject* dynobj)
{
	core::aabbox3d<f32> ode_box;

	if (dynobj)
	{
		ode_box = dynobj->getDynBoundingBox();
	}
   
    return ode_box;
}

//----------------------------------------------------------------------------

void CDynamicManager::storeFirstObjectsTransformationsForInterpolation()
{
	core::list<IDynamicObject*>::iterator iter=NULL;
    IDynamicObject  *obj;
    dBodyID          body=0;
    scn::ISceneNode *node=0;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];
		 		
		for	(	iter = CDynamicObject::DynamicObjectsByType[dtype].begin(); 
				iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter
			)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || !dBodyIsEnabled(body))
				continue;

			obj->storeFirstDynTransformationForInterpolation();
		}
	}
}

//----------------------------------------------------------------------------

void CDynamicManager::restoreFirstObjectsTransformationsForInterpolation()
{	
	core::list<IDynamicObject*>::iterator iter=NULL;
    IDynamicObject  *obj;
    dBodyID          body=0;
    scn::ISceneNode *node=0;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];
		 		
		for	(	iter = CDynamicObject::DynamicObjectsByType[dtype].begin(); 
				iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter
			)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || !dBodyIsEnabled(body))
				continue;

			obj->restoreFirstDynTransformationForInterpolation();
		}
	}
}

//----------------------------------------------------------------------------

void CDynamicManager::storeSecondObjectsTransformationsForInterpolation()
{
    core::list<IDynamicObject*>::iterator iter=NULL;
    IDynamicObject  *obj;
    dBodyID          body=0;
    scn::ISceneNode *node=0;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];
		 		
		for	(	iter = CDynamicObject::DynamicObjectsByType[dtype].begin(); 
				iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter
			)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || !dBodyIsEnabled(body))
				continue;

			obj->storeSecondDynTransformationForInterpolation();
		}
	}
}

//----------------------------------------------------------------------------

void CDynamicManager::restoreSecondObjectsTransformationsForInterpolation()
{	
	core::list<IDynamicObject*>::iterator iter=NULL;
    IDynamicObject  *obj;
    dBodyID          body=0;
    scn::ISceneNode *node=0;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];
		 		
		for	(	iter = CDynamicObject::DynamicObjectsByType[dtype].begin(); 
				iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter
			)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || !dBodyIsEnabled(body))
				continue;

			obj->restoreSecondDynTransformationForInterpolation();
		}
	}
}

//----------------------------------------------------------------------------

void CDynamicManager::interpolateObjectsTransformations(f32 factor)
{	
	core::list<IDynamicObject*>::iterator iter=NULL;
    IDynamicObject  *obj;
    dBodyID          body=0;
    scn::ISceneNode *node=0;

	const E_DYNAMIC_OBJECT_TYPE dtypearr[] = { EDOT_DYNAMIC, EDOT_PSEUDO_DYNAMIC };
	const s32 dtypecnt = sizeof(dtypearr)/sizeof(E_DYNAMIC_OBJECT_TYPE);

	for (s32 i=0; i<dtypecnt; i++)
	{
		E_DYNAMIC_OBJECT_TYPE dtype = dtypearr[i];
		 		
		for	(	iter = CDynamicObject::DynamicObjectsByType[dtype].begin(); 
				iter!=CDynamicObject::DynamicObjectsByType[dtype].end(); ++iter
			)
		{
			obj = *iter;

			if (!(body=((dBodyID)obj->getBodyID())) || !dBodyIsEnabled(body))
				continue;

			obj->interpolateDynTransformation(factor);
		}
	}
}

//----------------------------------------------------------------------------

//! создает менеджера динамики ODE
__MY_EXPORT__ IDynamicManager* createDynamicManager()
{     
    return new CDynamicManager();
}

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------
#endif //_MY_USE_ODE_DYNAMIC_
//----------------------------------------------------------------------------

