//|-------------------------------------------------------------------------
//| File:        CTerrainLibMiniSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CTerrainLibMiniSceneNode.h" 

#include <scn/ICameraSceneNode.h>
#include <vid/S3DVertex.h>
#include <vid/SFog.h>
#include <os/ITimer.h>
#include <scn/ICullingSystem.h>
#include <scn/ISceneManager.h>
#include <img/IImageLibrary.h>

//-----------------------------------------------------------------------
namespace my { 
namespace scn { 
//----------------------------------------------------------------------------

s32 WaterGreenlandsColor[]={
     47,  86, 106,
     38, 110, 143,
     52, 133, 138,
     49, 100,  75,
     57, 120,  40,
     47, 114,  71,
    156, 146,  92,
    163, 156, 124,
    151, 151, 140
    };
s32 WoodlandsMountainsColor[]={
    113,  83,  28,
    152, 117,  20,
     28,  93,  10,
     70, 145,  53,
    100, 143,  66,
    127, 140,  97,
    128, 140, 100,
    201, 202, 201,
    248, 247, 248
    };
s32 DesertColor[]={
     45,  80,   0,
     69,  44,   0,
     93,  28,   0,
    122,  45,   0,
    156,  63,   7,
    169, 111,  25,
    190, 172,  49,
    225, 201, 105,
    225, 208, 176
    };
//-----------------------------------------------------------------------

CTerrainLibMiniSceneNode* CTerrainLibMiniSceneNode::ms_Self=0; 

//-----------------------------------------------------------------------
// globals for libMini callbacks 
//-----------------------------------------------------------------------

// prepares a buffer if the buffer is nonempty draws and then empties the buffer 
// this function needs to be called after m_Stub->draw one more time 
void CTerrainLibMiniSceneNode::_BeginFan() 
{ 
    ms_Self->BeginFan(); 
} 

//-----------------------------------------------------------------------

// fills the buffer with vertecis 
void CTerrainLibMiniSceneNode::_FanVertex(float i,float y,float j) 
{ 
    ms_Self->FanVertex(i,y,j); 
} 

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::_PrismEdge(float x,float y,float yf,float z)
{
    if(ms_Self) ms_Self->PrismEdge(x, y, yf, z); 
}

//-----------------------------------------------------------------------
// other stuff
//-----------------------------------------------------------------------

CTerrainLibMiniSceneNode::CTerrainLibMiniSceneNode(ISceneNode* parent, s32 id) 
	: CTerrainSceneNode(parent, id), 
	m_Stub(0), ColorArrayPtr(0), ColorScheme(ETCS_WATER_GREENLANDS)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CTerrainLibMiniSceneNode");
#endif

    AbsoluteTransformation.getInversed(m_InvAbsoluteTransform); 

	m_FanStartIdx = m_FanVertsCnt =  m_IndicesToRender = m_VerticesToRender = 0;

	m_RenderVertices.set_used(4092);
	m_RenderIndices.set_used(1024);

	setMaps(NULL, NULL, 128.f);
} 

//-----------------------------------------------------------------------

CTerrainLibMiniSceneNode::~CTerrainLibMiniSceneNode() 
{
    SAFE_DELETE(m_Stub); 
} 

//-----------------------------------------------------------------------

bool CTerrainLibMiniSceneNode::setMaps(
	const c8* hmap_filename, const c8* fogmap_filename,
    f32 gridPointSpacing) 
{
	img::IImage *heightImage = NULL;

	if (!_setMaps(hmap_filename, fogmap_filename,
			gridPointSpacing, &heightImage))
		return false;

	detail_minus_2 = m_TileRepeatNumber-2.0f;
    size_minus_1 = m_HeightFieldSize-1; 
	size_minus_2 = m_HeightFieldSize-2; 
	size_plus_1 = m_HeightFieldSize+1;
    detail_div_size_1 = (float)((float)m_TileRepeatNumber/size_minus_1);

    if ( m_TerrainHeight <= 0)  m_TerrainHeight=0.000001f;

    switch(ColorScheme)
    {
    case ETCS_DESERT:
        SampleColorsNum = sizeof(DesertColor)/(3*sizeof(s32));
        ColorArrayPtr = DesertColor;
        break;
    case ETCS_WOODLANDS_MOUNTAINS:
        SampleColorsNum = sizeof(WoodlandsMountainsColor)/(3*sizeof(s32));;
        ColorArrayPtr = WoodlandsMountainsColor;
        break;
    case ETCS_WATER_GREENLANDS:
        SampleColorsNum = sizeof(WaterGreenlandsColor)/(3*sizeof(s32));;        
        ColorArrayPtr = WaterGreenlandsColor;
        break;
    default:
        ColorScheme = ETCS_NONE;
        SampleColorsNum = 0;        
        ColorArrayPtr = NULL;
        break;
    }

    deltaCoeff = 1.0f/((f32)SampleColorsNum-1);

	SAFE_DELETE(m_Stub); 
	
	m_Resolution = 1000000.0f;

	const float lambda=0.15f;
	const float displace=1.0f;
	const float emission=0.01f;
	const float attenuation=0.33f;

	void (*prismedge)(float x,float y,float yf,float z)=0;

	//prismedge = CTerrainLibMiniSceneNode::_PrismEdge;

	s16 *hmap = (s16 *)heightImage->getData();

    m_Stub = new ministub(
		hmap, &m_HeightFieldSize, // height map
		&m_GridPointSpacing, 1.f /*scale*/, 1.0f/*cellaspect*/, // grid definition
		0.0f, 0.0f, 0.0f, // grid center
        CTerrainLibMiniSceneNode::_BeginFan, CTerrainLibMiniSceneNode::_FanVertex,
        0, 0, 0, 
		NULL, 0, // fog map
		lambda, displace, attenuation,
        prismedge);

	heightImage->drop();

	setHeightScale(m_HeightScale);

	LOGGER.logInfo("LibMini terrain initialized (resolution=%.f, size=%dx%d(%dx%d), width=%.f, length=%.f, height=%.f)", 
		m_Resolution, m_HeightFieldSize, m_HeightFieldSize, m_HeightFieldSizeValue, m_HeightFieldSizeValue,
		m_TerrainWidth,  m_TerrainLength,  m_TerrainHeight);

    return true; 
}  

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::setHeightScale(f32 scale)
{
	CTerrainSceneNode::setHeightScale(scale);
	if (m_Stub)
	{
		m_Stub->setrelscale(getHeightScale());
	}
}

//-----------------------------------------------------------------------

f32 CTerrainLibMiniSceneNode::getCellHeight(u32 cell_x, u32 cell_y)
{
	if (!m_Stub || cell_x >= (u32)m_HeightFieldSize || cell_y >= (u32)m_HeightFieldSize)
		return 0;
	return m_Stub->getheight((s32)cell_x, (s32)cell_y);
}

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::BeginFan() 
{    
	m_FanStartIdx = m_VerticesToRender;
	m_FanVertsCnt = 0;

	if (m_RenderVertices.size() <= m_VerticesToRender + 24)
		m_RenderVertices.set_used(m_VerticesToRender * 2);

	if (m_RenderIndices.size() <= m_IndicesToRender + 24)
		m_RenderIndices.set_used(m_IndicesToRender * 2);
} 

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::FanVertex(f32 i, f32 y, f32 j) 
{
	if (m_FanVertsCnt >= 24)
		return;

	static S3DVertex2TCoords vertex;

	// vertex position

	f32 x = m_GridPointSpacing * i - m_SizeDiv2Dim;
    f32 z = m_SizeDiv2Dim - m_GridPointSpacing * j;  

	vertex.Pos.set(x, y * m_HeightScale, z);

	// vertex normal

	m_Stub->getnormal(x, z,
		&vertex.Normal.X, &vertex.Normal.Y, &vertex.Normal.Z); 

	// texture coordinates

	f32 t2u = (i)/m_HeightFieldSize;
    f32 t2v = (j)/m_HeightFieldSize;

	f32 t1u = t2u*m_TileRepeatNumber;
    f32 t1v = t2v*m_TileRepeatNumber;     
	
    vertex.TCoords.set (t1u,t1v); 
	vertex.TCoords2.set(t2u,t2v); 

	if (true)
	{
		// store calculated vertex

		m_RenderVertices[m_VerticesToRender] = vertex;
		
		u32 idxCnt = 0;
		u16 vtxIdx = m_VerticesToRender;

		// store correspondent fan indices

		while (true)
		{
			m_RenderIndices[m_IndicesToRender] = vtxIdx;

			m_IndicesToRender++;
			idxCnt++;

			if (m_FanVertsCnt < 3 || idxCnt >=3)
				break;
			else
			{
				vtxIdx = (idxCnt) == 1 ? m_FanStartIdx : (m_VerticesToRender - 1);
			}
		}

		m_VerticesToRender++;
	}
	else
	{
		u32 cnt = 0;

		while (true)
		{
			m_RenderVertices[m_VerticesToRender] = vertex;
		
			m_VerticesToRender++;
			cnt++;

			if (m_FanVertsCnt < 3 || cnt >=3)
				break;
			else
			{
				u32 vtxIdx = (cnt) == 1 ? m_FanStartIdx : m_VerticesToRender - ((m_FanVertsCnt > 3) ? 5 : 3);
				vertex = m_RenderVertices[vtxIdx];
			}
		}
	}

	m_FanVertsCnt++;
} 

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::PrismEdge(float x,float y,float yf,float z)
{	
}


//---------------------------------------------------------------------------

void CTerrainLibMiniSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		if (IsVisible)
		{
			m_Culled = CULLING_SYSTEM.isCull(this);
		}
		else
		{
			m_Culled = true;		
		}

		ISceneNode::OnCheckCulling();
	}
}

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::OnPreRender(u32 timeMs) 
{ 	
	if (!m_Stub)
		return;

	scn::ISceneManager &smgr   = SCENE_MANAGER;	

	ICameraSceneNode* camera = smgr.getActiveCamera(); 

	if (!camera)
		return;

	AbsoluteTransformation.getInversed(m_InvAbsoluteTransform);

	fovy   = RAD2DEG*camera->getFOV(); 
	aspect = camera->getAspectRatio(); 
    nearp  = camera->getNearValue(); 
	farp   = camera->getFarValue();

	cam_pos.set(camera->getPosition()); 
    cam_tgt.set(camera->getTarget()); 
	cam_up.set(camera->getPosition() + camera->getUpVector()); 

	m_InvAbsoluteTransform.transformVect(cam_pos);
    m_InvAbsoluteTransform.transformVect(cam_tgt); 
	m_InvAbsoluteTransform.transformVect(cam_up); 

	cam_up-=cam_pos;
	cam_up.normalize();

	cam_dir=cam_tgt-cam_pos; 

    if(cam_dir.getLengthSQ()==0.0)    
	{ 
		cam_dir.Z=-1.0f; 
    }	
	cam_dir.normalize();

	if (cam_up==cam_dir)
		cam_dir += core::vector3df(0.001f,0.001f,0.001f); // or libMini can crash

	// to avoid unknown error, generated by LibMini in this case
	if (cam_dir.X==0 && cam_dir.Z==0)
	{
		cam_dir.X+=0.000001f;
		cam_dir.Z+=0.000001f;
	}	

	cam_height_over_terrain = cam_pos.Y - getHeight(cam_pos.X,cam_pos.Z);

	fog_height_above_camera = fog_height_at_camera_pos - cam_height_over_terrain;

   // make callbacks to this scenenode 
    ms_Self=this;  

	// Determine the camera rotation, based on the camera direction.
	core::line3d<f32> line;
	line.start = smgr.getActiveCamera()->getAbsolutePosition();
	line.end   = smgr.getActiveCamera()->getTarget();
	core::vector3df cameraRotation = line.getVector().getHorizontalAngle();
	core::vector3df cameraPosition = smgr.getActiveCamera()->getPosition();

	if (m_Reset
			|| fabs(cameraRotation.X - OldCameraRotation.X) > CameraRotationDelta
			|| fabs(cameraRotation.Y - OldCameraRotation.Y) > CameraRotationDelta
			|| fabs(cameraPosition.X - OldCameraPosition.X) > CameraMovementDelta
			|| fabs(cameraPosition.Y - OldCameraPosition.Y) > CameraMovementDelta
			|| fabs(cameraPosition.Z - OldCameraPosition.Z) > CameraMovementDelta)
	{
		m_VerticesToRender = m_IndicesToRender = 0;
		m_FanStartIdx = m_FanVertsCnt = 0;

		m_Stub->draw(
			m_Resolution, 
			cam_pos.X, cam_pos.Y, cam_pos.Z, 
			cam_dir.X, cam_dir.Y, cam_dir.Z, 
			cam_up.X,  cam_up.Y,  cam_up.Z, 
			fovy, aspect, nearp, farp);  
		BeginFan();	

		OldCameraPosition = cameraPosition;
		OldCameraRotation = cameraRotation;	 
	}

	const core::aabbox3df &tbb = getTransformedBoundingBox();

	m_EnabledLights = 0;

	if (m_Material.getPass(0).isLighting())
	{
		const core::array <vid::SLight> &dlights = m_VideoDriver.getDynamicLights();
		u32 dl_count = dlights.size();
	
		for (u32 i = 0; i < dl_count && i < 32; i++)
		{
			f32 klight = 0.0f;
			const vid::SLight *light = &dlights[i];
			if (i == 0 && !light->Enabled)
				continue;
			else 
			if (light->Type == vid::ELT_POINT)
			{
				bool enabled = dlights[i].LightedVolume.intersectsWithBox(tbb);
				if (enabled)
					m_EnabledLights |= (1 << i);
			}
			else
			{
				m_EnabledLights |= (1 << i);
			}
		}
	}

	m_VideoDriver.registerGeometryForRendering(
		vid::ERP_3D_SOLID_PASS,
		AbsoluteTransformation, NULL, m_EnabledLights,
		m_Material,
		m_RenderVertices.pointer(), m_VerticesToRender, vid::S3DVertex2TCoords::Type,
		m_RenderIndices.pointer(), m_IndicesToRender, EIT_16_BIT,
		vid::EDPT_TRIANGLE_LIST);

	bool drawDebug = isDebugDataVisible();
	bool drawNormals = smgr.getSceneRenderFlag(ESRF_RENDER_TERRAIN_NORMALS);

	if (drawDebug)
    {
		m_VideoDriver.register3DBoxForRendering(
			getAbsoluteTransformation(),
			getBoundingBox(), img::SColor(0xffffffff));
    }

	if (drawNormals)
    {
		for (u32 i = 0; i < m_VerticesToRender; i++)
		{
			vid::S3DVertex2TCoords &v = m_RenderVertices[i];
			m_VideoDriver.register3DLineForRendering(AbsoluteTransformation, v.Pos, v.Pos + v.Normal * 50.f);
		}
    }

	ISceneNode::OnPreRender(timeMs); 
}

//-----------------------------------------------------------------------

void CTerrainLibMiniSceneNode::OnPostRender(u32 timeMs) 
{
	ISceneNode::OnPostRender(timeMs);
} 

//-----------------------------------------------------------------------

f32 CTerrainLibMiniSceneNode::getHeight(float x,float z) 
{ 
    if (m_Stub) 
		return m_Stub->getheight(x,z); 
    else 
        return 0; 
} 

//-----------------------------------------------------------------------

vector3df CTerrainLibMiniSceneNode::getNormal(float x,float z) 
{ 
    vector3df n(0,1,0); 
    if (m_Stub) 
    { 
        f32 nx,ny,nz; 
        m_Stub->getnormal(x,z,&nx,&ny,&nz); 
        n.set(nx,ny,nz); 
    } 
    return n; 
} 

//----------------------------------------------------------------------------

void CTerrainLibMiniSceneNode::getHeightColor(f32 y, s32 &R, s32 &G, s32 &B)
{
    R =255; G =255; B =255;

    if (ColorScheme==ETCS_NONE || SampleColorsNum<2 || !ColorArrayPtr) return;
    
    f32 heightCoeff = (y - m_HeightMin) / m_TerrainHeight;
    
    if (heightCoeff>1) heightCoeff=1; else if (heightCoeff<0) heightCoeff=0;    

    f32 minR=0, minG=0, minB=0, maxR=0, maxG=0, maxB=0, minCoeff=0, maxCoeff=1;   
    f32 kR=1, kG=1, kB=1, cR=1, cG=1, cB=1;

    s32 coeffStep = (s32)(heightCoeff/deltaCoeff);

    if (coeffStep>=(SampleColorsNum-1)) coeffStep = SampleColorsNum-2; 

    s32 offset = coeffStep*3;

    minR = (f32)ColorArrayPtr[  offset];
    minG = (f32)ColorArrayPtr[++offset];
    minB = (f32)ColorArrayPtr[++offset];
    maxR = (f32)ColorArrayPtr[++offset];
    maxG = (f32)ColorArrayPtr[++offset];
    maxB = (f32)ColorArrayPtr[++offset];

    minCoeff = coeffStep*deltaCoeff;
    maxCoeff = (coeffStep+1)*deltaCoeff;    

    f32 maxCoeff_minCoeff = maxCoeff-minCoeff;
    
    if (maxCoeff_minCoeff==0) maxCoeff_minCoeff=0.000001f;

    f32 _1_div_maxCoeff_minCoeff = 1.0f/maxCoeff_minCoeff;

    kR = (f32)(maxR-minR)*_1_div_maxCoeff_minCoeff;
    cR = (f32)(minR - kR*minCoeff);

    kG = (f32)(maxG-minG)*_1_div_maxCoeff_minCoeff;
    cG = (f32)(minG - kG*minCoeff);

    kB = (f32)(maxB-minB)*_1_div_maxCoeff_minCoeff;
    cB = (f32)(minB - kB*minCoeff);

    R = (s32)(kR*heightCoeff+cR); 
	G = (s32)(kG*heightCoeff+cG); 
	B = (s32)(kB*heightCoeff+cB);
}

//-----------------------------------------------------------------------

u32 CTerrainLibMiniSceneNode::getCompatibleHeightFieldSize(u32 origSize)
{
	u32 size = 0;

	// Calculation size to be compatible with LibMini
	for (size = 3; size < origSize; size = 2*size-1);

	return size;
}

//-----------------------------------------------------------------------
} // end namespace scn
} // end namespace my


