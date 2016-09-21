//|-------------------------------------------------------------------------
//| File:        CDynamicObject.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CDynamicObject.h"

#include <dev/IDevice.h>
#include <scn/ITerrainSceneNode.h>
#include <os/ICriticalSection.h>
#include <dyn/IDynamicManager.h>
#include <io/ILogger.h>
#include <scn/ISceneNode.h>
#include <scn/ISceneManager.h>
#include <res/IResourceManager.h>

#include "ODEHelper.h"

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//---------------------------------------------------------------------------

#define DENSITY 0.000005f

//---------------------------------------------------------------------------

core::list<IDynamicObject*> CDynamicObject::DynamicObjects;
core::list<IDynamicObject*> CDynamicObject::DynamicObjectsByType[E_DYNAMIC_OBJECT_TYPE_COUNT];

//---------------------------------------------------------------------------

CDynamicObject::CDynamicObject(
    dWorldID w, dSpaceID s, scn::ISceneNode* n, SDynamicObjectParams &params 
    ) : 
World(w), Space(s), Node(n), isFalling(false), isFallingOnLastCalc(false), 
Body(NULL), m_TriMeshData(NULL), m_HeightfieldData(NULL),
DynVelocity(core::vector3df(0,0,0)), MovingForce(core::vector3df(0,0,0)), 
vertices(0), indices(0), vertexcount(0), indexcount(0),
HeightOverGround(0), AutoUpdateGeomParams(false), Moved(true),
ODETurnoff(core::vector3df(0,0,0))
{
#if MY_DEBUG_MODE  
	setClassName("CDynamicObject");
#endif

	DynamicObjectsIterator=0;

	for (s32 i=0; i<E_DYNAMIC_OBJECT_TYPE_COUNT; i++)
	{
		DynamicObjectsByTypeIterator[i]=0;
	}

	Parameters.set(params);

	State = EDOS_UNKNOWN;
    
    if(Node)
    {
        Node->DynObject=this;

		updateGeomData();
    }

	TestRays.push_back( core::line3d<f32>() );
	TestRays.push_back( core::line3d<f32>() );
	TestRays.push_back( core::line3d<f32>() );	

	TestRays.push_back( core::line3d<f32>() );
	TestRays.push_back( core::line3d<f32>() );	
}

//---------------------------------------------------------------------------

CDynamicObject::~CDynamicObject()
{
	freeGeomData();

	if(Node) 
	{ 
		Node->DynObject=NULL; 
		Node=NULL; 
	}
}

//---------------------------------------------------------------------------

// frees all geom data 
void CDynamicObject::freeGeomData()
{
	// free ode data

	// destroy ode bodies
	if (Body) 
	{ 
		dBodyDestroy(Body); 
		Body=0; 
	}

	if (m_TriMeshData) 
	{ 
		dGeomTriMeshDataDestroy(m_TriMeshData); 
		m_TriMeshData = 0;
	}
	if (m_HeightfieldData) 
	{ 
		dGeomHeightfieldDataDestroy(m_HeightfieldData);
		m_HeightfieldData = 0; 
	}

	// destroy ode geoms
	for (u32 i=0; i<Geometry.size(); i++)
	{
		if (Geometry[i])
		{
			dGeomDestroy(Geometry[i]); Geometry[i]=0;
		}
	}

	Geometry.clear();
    
	// free geometry

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	clearDynObjectEntries();
}

//---------------------------------------------------------------------------

dReal heightfield_callback( void* pUserData, int x, int z )
{
	scn::ITerrainSceneNode* terrain = 
		reinterpret_cast<scn::ITerrainSceneNode*>(pUserData);

	return terrain->getCellHeight(x, z);
}

//---------------------------------------------------------------------------

// Set ODE geometry data using the mesh structure   
void CDynamicObject::updateGeomData() 
{	
    if(Node==NULL) return;  	

	freeGeomData();

	E_DYN_OBJ_GEOM_FORM &gf = Parameters.GeomForm;

	scn::IMesh *Mesh = 0;

	scn::IAnimatedMesh *phys_mesh = Node->getPhysicalMesh();
	if (phys_mesh)
	{
		Mesh = phys_mesh->getMesh(0);
	}
	if (!Mesh)
	{
		scn::IAnimatedMeshSceneNode *animated_mesh_scene_node =
			SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(Node);
		if (animated_mesh_scene_node)
			Mesh = animated_mesh_scene_node->getMesh(0);
	}

    core::vector3df pos    = Node->getPosition();
    core::vector3df rot    = Node->getRotation();
    core::vector3df scl    = Node->getScale();

    core::aabbox3d<f32> box = Node->getBoundingBox();
	core::vector3df extend  = box.getExtend();
	core::vector3df center  = box.getCenter(); 

	MyInitialBoundingBox = box;
	
    int i,j,ci,cif,cv;      

    ODETurnoff.set(0,0,0);	

    if (Parameters.Type == EDOT_MONOLITH) 
		center.set(0,0,0);

    // creating body for the ode object

    if (Parameters.Type == EDOT_MONOLITH ||
			Parameters.Type == EDOT_SENSELESS)  
    {   
		// monolith object has no body
        Body=0;
    }
    else
    {   
		// create a body for this object
        Body = dBodyCreate (World); 		

        // ������������� ������ ��� ����
        dBodySetData(Body,(void*)this);

        // set mass shape
        if (gf==EDOGF_XCCYLINDER || gf==EDOGF_YCCYLINDER || 
            gf==EDOGF_ZCCYLINDER
            )
        {			
            dReal radius,length;
           
            if (gf==EDOGF_XCCYLINDER)        
            {
				radius   = (dReal)core::math::Max( extend.Y*scl.Y, extend.Z*scl.Z )*0.5f;
                length   = (dReal)extend.X*scl.X;
            }
			else if (gf==EDOGF_YCCYLINDER) 
            {
				radius   = (dReal)core::math::Max( extend.X*scl.X, extend.Z*scl.Z )*0.5f;
                length   = (dReal)extend.Y*scl.Y;
            }
			else if (gf== EDOGF_ZCCYLINDER) 
            {
				radius   = (dReal)core::math::Max( extend.X*scl.X, extend.Y*scl.Y )*0.5f;
                length   = (dReal)extend.Z*scl.Z;
            }

            length -=2*radius;

            // ������� ����� ��������
            dMassSetCappedCylinder (&Mass, DENSITY,3,radius,length>0?length:0.000001f);
        }
		else 
		if (gf == EDOGF_SPHERE)
        {   // �����
            dMassSetSphere (
				&Mass, DENSITY, 
				(dReal)core::math::Max( extend.X*scl.X, extend.Y*scl.Y, extend.Z*scl.Z )*0.5f
				);
        }
		else
        {   // �����
            dMassSetBox (
				&Mass, DENSITY,
				(dReal)extend.X*scl.X, (dReal)extend.Y*scl.Y, (dReal)extend.Z*scl.Z
				);
        }
        
        // adjust mass weight
        dMassAdjust(&Mass, (dReal)Parameters.MassValue);             
    }	
    
    if ((Mesh || Node->getSceneNodeType() == scn::ESNT_TERRAIN_SCENE_NODE) && gf==EDOGF_MESH)
    // creating treemesh data for ode
    {
		vertexcount = 0;
		indexcount  = 0;

		scn::ITerrainSceneNode* terrain = NULL;

		if (Node->getSceneNodeType() == scn::ESNT_TERRAIN_SCENE_NODE)
		{
			terrain = static_cast<scn::ITerrainSceneNode*>(Node);
		}

		if (terrain)
		{
			s32 size = terrain->getHeightFieldSize();

	        // build structure for ode trimesh geom    
			
			indexcount = (size-1)*(size-1)*6;
			vertexcount = size*size;

		    indices =new int[indexcount];
			vertices=new dVector3[vertexcount];

			core::vector3df vert;
			s32 j=0;	
			ci=0; cv=0;

			for	(i=0; i<size; i++)
			{
				for (j=0; j<size; j++, cv++)
		        {   
			        vert = terrain->getCellPosition(i, j);  

					vertices[cv][0] = vert.X;
					vertices[cv][1] = vert.Y;
					vertices[cv][2] = vert.Z;
				}        
			}    

		    s32 i_size_j, i_1_size_j, i_1_size_j_1;

			for (i=0; i<size-1; i++)
			{
				for (j=0; j<size-1; j++)
		        {   
				    i_size_j     = i*size+j;
					i_1_size_j   = (i+1)*size+j;
					i_1_size_j_1 = i_1_size_j + 1;

					indices[ci++] = i_size_j;
					indices[ci++] = i_1_size_j_1;
					indices[ci++] = i_1_size_j;	
					
					indices[ci++] = i_size_j;
					indices[ci++] = i_size_j+1;
					indices[ci++] = i_1_size_j_1;
				}
		    }		
		}
		else
		{
	        for(i=0;i<Mesh->getMeshBuffersCount();i++)
		    {
				scn::IMeshBuffer* mb = Mesh->getMeshBuffer(i);

				if (!mb || mb->getMaterial().isTransparent()) 
					continue;		
			
				indexcount += mb->getIndexCount();
				vertexcount+= mb->getVertexCount();
			}
	        // build structure for ode trimesh geom        
		    indices =new int[indexcount];
			vertices=new dVector3[vertexcount];
       
	        // fill trimesh geom
		    ci=0; cif=0; cv=0;
			s32 mb_count = Mesh->getMeshBuffersCount();
	        for (i=0; i<mb_count; i++)
		    {
				scn::IMeshBuffer* mb = Mesh->getMeshBuffer(i); 
	
				if (!mb || mb->getMaterial().isTransparent()) 
					continue;		

			    // fill indices

				s32 ind_cnt = mb->getIndexCount();

				for (j=0;j<ind_cnt;j++)
	            {   
					indices[ci]=cif+mb->getIndex(j);
			        ci++;
				}
	            cif=cif+mb->getVertexCount();

		        // fill vertices				

				for (j=0;j<mb->getVertexCount();j++)
		        {
					vid::S3DBaseVertex& v = mb->getVertex(j); 

					vertices[cv][0] = (v.Pos.X-center.X)*scl.X;
			        vertices[cv][1] = (v.Pos.Y-center.Y)*scl.Y;
					vertices[cv][2] = (v.Pos.Z-center.Z)*scl.Z;
					cv++;
	            }  			   
			}
        } 
        
		// creates a dTriMeshData object which is used to store mesh data.
        m_TriMeshData = dGeomTriMeshDataCreate();
        
		// build the trimesh data
        dGeomTriMeshDataBuildSimple(
			m_TriMeshData, (dReal*)vertices, vertexcount, (unsigned int*)indices, indexcount
			);  		
       
        // creating transform geometry
		Geometry.push_back(dCreateGeomTransform(Space));
		dGeomTransformSetCleanup(Geometry[Geometry.size()-1],1);   

		// attaching simple geometry to the trasform geometry
		dGeomTransformSetGeom(
			Geometry[Geometry.size()-1], 
			dCreateTriMesh(
				0, m_TriMeshData, 0, 0, 0
				)
			);  
    }
	else if (gf == EDOGF_HEIGHTMAP)
	// creating geometry, based on heightmaps array
	{		
		scn::ITerrainSceneNode* terrain = 
			SAFE_CAST_TO_TERRAIN_SCENE_NODE(Node);	

		if (terrain)
		{
			// our heightfield floor
			dHeightfieldDataID heightid = m_HeightfieldData = dGeomHeightfieldDataCreate();

			// Create heightfield.

			dGeomHeightfieldDataBuildCallback( 
				heightid, terrain, heightfield_callback,
				box.getExtend().X, box.getExtend().Z,
				terrain->getHeightFieldSize(), terrain->getHeightFieldSize(),
				REAL( 1.0 ), REAL( 0.0 ), REAL( 0.0 ), 0 
				);

			// Give some very bounds which, while conservative,
			// makes AABB computation more accurate than +/-INF.
			dGeomHeightfieldDataSetBounds( 
				heightid, box.MinEdge.Y, box.MaxEdge.Y 
				);

			// creating transform geometry
			Geometry.push_back(dCreateGeomTransform(Space));
			dGeomTransformSetCleanup(Geometry[Geometry.size()-1],1);   

			// attaching simple geometry to the transform geometry
			dGeomTransformSetGeom(
				Geometry[Geometry.size()-1], 
				dCreateHeightfield ( 0, heightid, 1 )
				);
		}
	}
	else if (
		gf==EDOGF_XCCYLINDER || gf==EDOGF_YCCYLINDER || gf==EDOGF_ZCCYLINDER 
        )
    // creating capped cylinder  geometry (along axis X,Y or Z)
    {		
		dGeomID geom, geomtr;
		
		// creating transform geometry

		geomtr = dCreateGeomTransform(Space);		
		dGeomTransformSetCleanup(geomtr,1);  

		Geometry.push_back(geomtr);	

		// creating simple geometry
		geom = dCreateCapsule ( 0, 1, 1);

		// attaching simple geometry to the transform geometry
		dGeomTransformSetGeom( geomtr, geom ); 	
    }
	else if (gf == EDOGF_SPHERE)
    // creating sphere geometry
    {
		// creating transform geometry
		Geometry.push_back(dCreateGeomTransform(Space));
		dGeomTransformSetCleanup(Geometry[Geometry.size()-1],1);   

		// attaching simple geometry to the transform geometry
		dGeomTransformSetGeom(
			Geometry[Geometry.size()-1], 
			dCreateSphere ( 0, 1 )
			);  
    }
	else
    // creating cube geometry
    {
		// creating transform geometry
		Geometry.push_back(dCreateGeomTransform(Space));
		dGeomTransformSetCleanup(Geometry[Geometry.size()-1],1);   

		// attaching simple geometry to the transform geometry
		dGeomTransformSetGeom(
			Geometry[Geometry.size()-1], 
			dCreateBox( 
				0, 1, 1, 1 
				)
			);  
    } 

	for (i=0; i<(s32)Geometry.size(); i++)
	{
		// lets have a pointer to our bounceable, 
		dGeomSetData(Geometry[i], (void*)this);

		// attach the body to the geometry
		if (Body) 
			dGeomSetBody(Geometry[i],Body);  
	}

    // make the body have a mass
    if (Body) dBodySetMass(Body,&Mass);  

	updateGeomParams();	

	clearDynObjectEntries();
	createDynObjectEntries();
}

//---------------------------------------------------------------------------

//! update geometry params for the current geometry data
void CDynamicObject::updateGeomParams()
{
    if(Node==NULL) return;  	

	E_DYN_OBJ_GEOM_FORM &gf = Parameters.GeomForm;

    core::vector3df pos    = Node->getPosition();
    core::vector3df rot    = Node->getRotation();
    core::vector3df scl    = Node->getScale();

    core::aabbox3d<f32> box = Node->getBoundingBox();
	core::vector3df extend  = box.getExtend();
	core::vector3df center  = box.getCenter(); 

	MyInitialBoundingBox       = box;	
	MyInitialBoundingBoxCenter = center;

    ODETurnoff.set(0,0,0);	

    if (Parameters.Type == EDOT_MONOLITH) 
		center.set(0,0,0);

    if (gf==EDOGF_MESH)
    {		
		// nothing to do
    }
	else if (gf == EDOGF_HEIGHTMAP)
	{
		// nothing to do		
	}
	else if (
		gf==EDOGF_XCCYLINDER || gf==EDOGF_YCCYLINDER || gf==EDOGF_ZCCYLINDER 
        )
	// updating capped cylinder geometry params
    {
		dReal radius,length;

        if (gf==EDOGF_XCCYLINDER)
        {
			radius   = (dReal)core::math::Max( extend.Y*scl.Y, extend.Z*scl.Z )*0.5f;
            length   = (dReal)extend.X*scl.X;
			ODETurnoff.set(0.0f,90.0f,0.0f);
        }
		else if (gf==EDOGF_YCCYLINDER)
        {
			radius   = (dReal)core::math::Max( extend.X*scl.X, extend.Z*scl.Z )*0.5f;
            length   = (dReal)extend.Y*scl.Y;
			ODETurnoff.set(90.0f,0.0f,0.0f);
        }
		else if (gf==EDOGF_ZCCYLINDER)
        {
			radius   = (dReal)core::math::Max( extend.X*scl.X, extend.Y*scl.Y )*0.5f;
            length   = (dReal)extend.Z*scl.Z;
			ODETurnoff.set(0.0f,0.0f,0.0f);
        }

        length -=2*radius;    
		
		for (s32 g=0; g<(s32)Geometry.size(); g++)
		{
			dGeomID ccylinder = dGeomTransformGetGeom(Geometry[g]);
			
			dGeomCCylinderSetParams(ccylinder, radius, length>0?length:0.000001f);
		}		
    }
	else if (gf == EDOGF_SPHERE)
	// updating sphere geometry params
    {
		for (s32 g=0; g<(s32)Geometry.size(); g++)
		{
			dGeomID sphere = dGeomTransformGetGeom(Geometry[g]);

			dGeomSphereSetRadius (sphere, (dReal)core::math::Max( extend.X*scl.X, extend.Y*scl.Y, extend.Z*scl.Z )*0.5f);
		}
    }
	else
    // updating cube geometry params
    {
		for (s32 g=0; g<(s32)Geometry.size(); g++)
		{
			dGeomID box = dGeomTransformGetGeom(Geometry[g]);

			dGeomBoxSetLengths (box, (dReal)extend.X*scl.X, (dReal)extend.Y*scl.Y, (dReal)extend.Z*scl.Z );
		}
    } 		

    // transforming ode object	

	DYNAMIC_MANAGER.setDynamicObjectRotation(this, rot);
    DYNAMIC_MANAGER.setDynamicObjectPosition(this, pos);	
	
	storeFirstDynTransformationForInterpolation();
	storeSecondDynTransformationForInterpolation();

	m_CollisionFilter = 
		DynamicObjectCollidFilter[Parameters.Type][1] ^ 
		Parameters.DynamicIgnoreFilter;

	Moved = true;
}

//---------------------------------------------------------------------------

void CDynamicObject::clearDynObjectEntries()
{
	if (DynamicObjectsIterator!=0)
	{
		DynamicObjects.erase(DynamicObjectsIterator);
		DynamicObjectsIterator=0;
	}

	for (s32 i=0; i<E_DYNAMIC_OBJECT_TYPE_COUNT; i++)
	{
		if (DynamicObjectsByTypeIterator[i]!=0)
		{
			DynamicObjectsByType[i].erase(DynamicObjectsByTypeIterator[i]);
			DynamicObjectsByTypeIterator[i]=0;
		}
	}
}

//---------------------------------------------------------------------------

void CDynamicObject::createDynObjectEntries()
{
	DynamicObjects.push_back(this);	
	DynamicObjectsIterator = DynamicObjects.get_last();

	DynamicObjectsByType[Parameters.Type].push_back(this);	
	DynamicObjectsByTypeIterator[Parameters.Type] = 
		DynamicObjectsByType[Parameters.Type].get_last();	
}

//---------------------------------------------------------------------------

scn::ISceneNode* CDynamicObject::getSceneNode()
{
	return Node;
}

//---------------------------------------------------------------------------

void* CDynamicObject::getBodyID()
{
	return (void*)Body;
}

//---------------------------------------------------------------------------

//! ���������� ������������� ��������� ����� ODE �������
void* CDynamicObject::getGeomID()
{
	if (Geometry.size() > 0)
		return (void*)Geometry[0];
	else
		return (void*)0;
}

//---------------------------------------------------------------------------

//! return bounding box
core::aabbox3d<f32> CDynamicObject::getMyBoundingBox()
{   
	return MyInitialBoundingBox;
}

//---------------------------------------------------------------------------

//! return bounding box
core::aabbox3d<f32> CDynamicObject::getDynBoundingBox()
{   
	core::aabbox3d<f32> ode_box;

	dReal aabb[6];    

	dGeomGetAABB ( (dGeomID)getGeomID(), aabb );      

	return core::aabbox3d<f32>(aabb[0],aabb[2],aabb[4],aabb[1],aabb[3],aabb[5]);;
}

//---------------------------------------------------------------------------

//! return My last position
core::vector3df& CDynamicObject::getMyLastPos()
{   	
	return LastMyPos;
}

//---------------------------------------------------------------------------

//! return My last rotation
core::vector3df& CDynamicObject::getMyLastRot()
{   
	return LastMyRot;
}

//---------------------------------------------------------------------------

//! last Dynamic position
core::vector3df& CDynamicObject::getDynLastPos()
{
	return LastDynPos; 
}

//---------------------------------------------------------------------------

//! last Dynamic rotation
core::quaternion& CDynamicObject::getDynLastRot()
{
	return LastDynRot;
}

//---------------------------------------------------------------------------

//! position displace between dynamic object and node
core::vector3df CDynamicObject::getDynDisplace()
{ 
	core::vector3df ODEDisplace(0,0,0);

	if (Parameters.Type != EDOT_MONOLITH)
    {
		ODEDisplace = MyInitialBoundingBoxCenter;

		core::vector3df abs_pos = Node->getAbsolutePosition();
		core::vector3df parent_offset = abs_pos-Node->getPosition();
	        
		Node->getAbsoluteTransformation().transformVect(ODEDisplace);
	        
		ODEDisplace = ODEDisplace - abs_pos + parent_offset;
	}

    return ODEDisplace;     
}

//----------------------------------------------------------------------------

//! rotation turnoff between dynamic object and node
core::vector3df CDynamicObject::getDynTurnoff()
{   
	return ODETurnoff;       
}   

//---------------------------------------------------------------------------

// ��������� �������
E_DYN_OBJ_STATE& CDynamicObject::getState()
{   
	return State;
}

//---------------------------------------------------------------------------

//! get ODE object parameters
const SDynamicObjectParams& CDynamicObject::getParameters()
{
	return Parameters;
}

//---------------------------------------------------------------------------

//! set ODE object parameters
void CDynamicObject::setParameters(SDynamicObjectParams params)
{
	SDynamicObjectParams old_params = Parameters;

	CHECK_RANGE(params.SurfMu, 0.0f, 1.0f);

	Parameters = params;	

	if (old_params.Type != Parameters.Type ||
		old_params.GeomForm    != Parameters.GeomForm ||
		old_params.MassValue   != Parameters.MassValue 
		)
	{
		updateGeomData();
	}
	
	m_CollisionFilter = 
		DynamicObjectCollidFilter[Parameters.Type][1] ^ 
		Parameters.DynamicIgnoreFilter;
}

//---------------------------------------------------------------------------

//! returning moving force applayed from player controlls to the dynamic object
core::vector3df& CDynamicObject::getMovingForce()
{
	return MovingForce;
}

//---------------------------------------------------------------------------

//! returning base point of this dynamic object
core::vector3df& CDynamicObject::getBasePoint()
{
	return ode_base_point;    
}

//---------------------------------------------------------------------------

//! returning high point of this dynamic object
core::vector3df& CDynamicObject::getHighPoint()
{
	return ode_high_point;
}

//---------------------------------------------------------------------------

core::array< core::line3d<f32> >& CDynamicObject::getTestRays()
{
	return TestRays;
}

//---------------------------------------------------------------------------

//! returns height between ground and base point of this dynamic object
//! ( works only for main player dynamic object )
f32& CDynamicObject::getHeightOverGround()
{
	return HeightOverGround;
}

//---------------------------------------------------------------------------

//! returns time at wich this dynamic object starts to falling down (milisecs)
s32& CDynamicObject::getFallingStartTime()
{
	return FallingStartTime;
}

//---------------------------------------------------------------------------

//! returning falling variable
bool& CDynamicObject::getIsFalling()
{
	return isFalling;
}

//---------------------------------------------------------------------------

//! returning falling on last calc variable
bool& CDynamicObject::getIsFallingOnLastCalc()
{
	return isFallingOnLastCalc;
}

//---------------------------------------------------------------------------

//! return AutoUpdateGeomParams flag value
bool& CDynamicObject::getAutoUpdateGeomParams()
{
	return AutoUpdateGeomParams;
}

//---------------------------------------------------------------------------

void CDynamicObject::storeFirstDynTransformationForInterpolation()
{
	if (!Body || !Node)
		return;	

	// get the position of the ODE body

	dReal* pos = (dReal*)dBodyGetPosition(Body);

	FirstDynPositionForInterpolation[0] = pos[0];
	FirstDynPositionForInterpolation[1] = pos[1];
	FirstDynPositionForInterpolation[2] = pos[2];

	// get the rotation of the ODE body    

	dReal* rot = (dReal*)dBodyGetQuaternion(Body);

	FirstDynRotationForInterpolation[0] = rot[0];
	FirstDynRotationForInterpolation[1] = rot[1];
	FirstDynRotationForInterpolation[2] = rot[2];
	FirstDynRotationForInterpolation[3] = rot[3];

	FirstPositionForInterpolation = Node->RelativeTranslation;
	FirstRotationForInterpolation = Node->RelativeRotation;
}

//---------------------------------------------------------------------------

void CDynamicObject::restoreFirstDynTransformationForInterpolation()
{
	if (!Body || !Node)
		return;

	dBodySetPosition(
		Body, 
		FirstDynPositionForInterpolation[0], 
		FirstDynPositionForInterpolation[1], 
		FirstDynPositionForInterpolation[2]
		);

	dBodySetQuaternion(Body, FirstDynRotationForInterpolation);	
}

//---------------------------------------------------------------------------

void CDynamicObject::storeSecondDynTransformationForInterpolation()
{
	if (!Body || !Node)
		return;

	// get the position of the ODE body

	dReal* pos = (dReal*)dBodyGetPosition(Body);

	SecondDynPositionForInterpolation[0] = pos[0];
	SecondDynPositionForInterpolation[1] = pos[1];
	SecondDynPositionForInterpolation[2] = pos[2];

	// get the rotation of the ODE body    

	dReal* rot = (dReal*)dBodyGetQuaternion(Body);

	SecondDynRotationForInterpolation[0] = rot[0];
	SecondDynRotationForInterpolation[1] = rot[1];
	SecondDynRotationForInterpolation[2] = rot[2];
	SecondDynRotationForInterpolation[3] = rot[3];

	SecondPositionForInterpolation = Node->RelativeTranslation;
	SecondRotationForInterpolation = Node->RelativeRotation;
}

//---------------------------------------------------------------------------

void CDynamicObject::restoreSecondDynTransformationForInterpolation()
{
	if (!Body || !Node)
		return;

	dBodySetPosition(
		Body, 
		SecondDynPositionForInterpolation[0], 
		SecondDynPositionForInterpolation[1], 
		SecondDynPositionForInterpolation[2]
		);

	dBodySetQuaternion(Body, SecondDynRotationForInterpolation);
}

//---------------------------------------------------------------------------

void CDynamicObject::interpolateDynTransformation(f32 factor)
{
	dReal interp_dyn_pos[] = 
	{
		FirstDynPositionForInterpolation[0]+(SecondDynPositionForInterpolation[0]-FirstDynPositionForInterpolation[0])*factor,
		FirstDynPositionForInterpolation[1]+(SecondDynPositionForInterpolation[1]-FirstDynPositionForInterpolation[1])*factor,
		FirstDynPositionForInterpolation[2]+(SecondDynPositionForInterpolation[2]-FirstDynPositionForInterpolation[2])*factor
	};

	dReal interp_dyn_rot[4];   
    ode::ODEQuaternionSlerp(FirstDynRotationForInterpolation, SecondDynRotationForInterpolation, interp_dyn_rot, factor);

	dBodySetPosition(
		Body, 
		interp_dyn_pos[0], interp_dyn_pos[1], interp_dyn_pos[2]
		); 
	dBodySetQuaternion(Body, interp_dyn_rot);

	core::vector3df  irr_pos;
	core::vector3df  irr_rot;

	// convert ODE transformations to MyEngine
	irr_pos.set((f32)interp_dyn_pos[0],(f32)interp_dyn_pos[1],(f32)interp_dyn_pos[2]);            
	ode::ODEQuaternionToEuler(interp_dyn_rot, irr_rot);

	// next MyEngine orientation
	core::vector3df next_rot = irr_rot - getDynTurnoff();     
	core::vector3df next_pos = irr_pos - getDynDisplace();   
	
	const SDynamicObjectParams& par   = getParameters();
	core::vector3df last_euler        = getMyLastRot();

	if (!(par.DynamicConstraints&EDTC_ROTX) ||
		!(par.DynamicConstraints&EDTC_ROTY) ||
		!(par.DynamicConstraints&EDTC_ROTZ)
		)
	{
		next_rot = last_euler;
	} 

	// set the node rotation 
	Node->RelativeRotation  = next_rot; 			
	getMyLastRot()          = next_rot;

	// set the position at the scenenode
	Node->RelativeTranslation = next_pos;
	getMyLastPos()            = next_pos;

	getDynLastPos().X = interp_dyn_pos[0];
	getDynLastPos().Y = interp_dyn_pos[1];
	getDynLastPos().Z = interp_dyn_pos[2];

	getDynLastRot().W = interp_dyn_rot[0];
	getDynLastRot().X = interp_dyn_rot[1];
	getDynLastRot().Y = interp_dyn_rot[2];
	getDynLastRot().Z = interp_dyn_rot[3];

	Node->m_TransformationChanged = true;
	Node->updateAbsoluteTransformation();
}

//---------------------------------------------------------------------------

bool CDynamicObject::isMoved()
{
	return Moved;
}

//---------------------------------------------------------------------------

void CDynamicObject::setMoved(bool value)
{
	Moved = value;

	if (!Body)
		return;	

	dBodyEnable(Body);
}

//---------------------------------------------------------------------------

void CDynamicObject::addForce(const core::vector3df& force)
{
	if (!Body||force.getLength() <=0.0f)
		return;	
	
	dBodyAddForce(Body, force.X, force.Y, force.Z);
	MovingForce = force;	

	setMoved(true);

	// to start up moving

	dReal* lin_vel = (dReal*)dBodyGetLinearVel(Body);

	if (core::math::IsZero(lin_vel[0]) &&
		core::math::IsZero(lin_vel[1]) &&
		core::math::IsZero(lin_vel[2]) 
		)
	{
		core::vector3df add_vel(force);
		add_vel.normalize();
		add_vel*=10.0f;
		
		dBodySetLinearVel(Body, lin_vel[0]+add_vel.X, lin_vel[1]+add_vel.Y, lin_vel[2]+add_vel.Z);					
	}		
}

//---------------------------------------------------------------------------

s32 CDynamicObject::getCollisionFilter()
{
	return m_CollisionFilter;
}

//---------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------
