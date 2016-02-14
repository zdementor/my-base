//|-------------------------------------------------------------------------
//| File:        CPathFinderAStar.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CPathFinderAStar.h"

#include <scn/ISceneManager.h>
#include <scn/ITerrainSceneNode.h>
#include <img/IImageLibrary.h>
#include <os/ITimer.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

core::array<IPathFinder*> CPathFinder::PathFinders;

const s32 CPathFinderAStar::CPathfinder::notfinished = 0; 
const s32 CPathFinderAStar::CPathfinder::notStarted = 0;
const s32 CPathFinderAStar::CPathfinder::found = 1;
const s32 CPathFinderAStar::CPathfinder::nonexistent = 2; 
const s32 CPathFinderAStar::CPathfinder::walkable = 0;
const s32 CPathFinderAStar::CPathfinder::unwalkable = 1;

//----------------------------------------------------------------------------

//! Constructor
CPathFinderAStar::CPathFinderAStar(ISceneNode* parent, s32 id) 
	: CPathFinder(parent, id), 
Positions(0), WalkabilitySizeX(0), WalkabilitySizeY(0), NoPathCounter(0)
{
#if MY_DEBUG_MODE 
	setClassName("CPathFinderAStar");
#endif
}

//----------------------------------------------------------------------------

//! Destructor
CPathFinderAStar::~CPathFinderAStar()
{
	// free allocated memory

	deinitialize();		
}

//---------------------------------------------------------------------------

//! is called on check visibility of this node
void CPathFinderAStar::OnCheckCulling()
{     
	if (!m_CullingChecked)
	{
		m_Culled = !IsVisible;

		ISceneNode::OnCheckCulling();
	}
}

//----------------------------------------------------------------------------

//! pre render event
void CPathFinderAStar::OnPreRender(u32 timeMs)
{
	ISceneNode::OnPreRender(timeMs);
}

//----------------------------------------------------------------------------

//! post render event
void CPathFinderAStar::OnPostRender(u32 timeMs)
{
	ISceneNode::OnPostRender(timeMs);

	Pathfinder.swapWalkabilityBuffers();
	
	NoPathCounter = 0;

	TransformedBBox = getTransformedBoundingBox();
}

//----------------------------------------------------------------------------

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CPathFinderAStar::getBoundingBox() const
{
	return BBox;
}

//---------------------------------------------------------------------------

//! Returns the material based on the zero based index i.
vid::SMaterial& CPathFinderAStar::getMaterial(s32 i)
{
    return Material;
}
//---------------------------------------------------------------------------

//! Returns amount of materials used by this scene node.
s32 CPathFinderAStar::getMaterialsCount()
{
    return 1;
}

//----------------------------------------------------------------------------

//! // Allocates memory for the pathfinder.
bool CPathFinderAStar::initialize(ITerrainSceneNode* terrain, img::IImage* walkabilitymap)
{
	bool res = false;

	if (!terrain)
		return res;	
	
	if (!(terrain->getHeightFieldSize()>0))
		return res;

	s32 TerrainSize = terrain->getHeightFieldSize();

	if (TerrainSize<=0)
		return res;

	if (walkabilitymap && (
			walkabilitymap->getDimension().Width!=walkabilitymap->getDimension().Height ||
			walkabilitymap->getDimension().Width != TerrainSize
			)
		)
		return res;	

	// free allocated memory

	deinitialize();	

	// initializing astar pathfinder

	LOGGER.log("Calculating walkability points for terrain . . .");		

	WalkabilitySizeX = TerrainSize;
	WalkabilitySizeY = TerrainSize;

	BBox = terrain->getBoundingBox();

	MinX = BBox.MinEdge.X;
	MinZ = BBox.MinEdge.Z;

	MaxX = BBox.MaxEdge.X;
	MaxZ = BBox.MaxEdge.Z;

	TileSizeX = (MaxX - MinX) / WalkabilitySizeX;
	TileSizeZ = (MaxZ - MinZ) / WalkabilitySizeY;

	DimX = MaxX - MinX;
	DimZ = MaxZ - MinZ;
	DimY = BBox.MaxEdge.Y - BBox.MinEdge.Y;

	core::vector3df vert;
	s32 i=0, j=0, cc=0;	

	// allocating memory

	Positions = new core::vector3df* [WalkabilitySizeX];

	for (i=0; i<WalkabilitySizeX; i++)
	{
		Positions[i] = new core::vector3df [WalkabilitySizeY];
	}

	Pathfinder.InitializePathfinder(WalkabilitySizeX, WalkabilitySizeY);

	// filling walkabilities arrays

	f32 maxH = 0;
	f32 minH = 0;

	for	(i=0; i<TerrainSize; i++)
	{
		for (j=0; j<TerrainSize; j++)
		{
			f32 h = terrain->getCellHeight(i, j);

			if ((i==0 && j==0 ) || h>maxH)
				maxH = h;

			if ((i==0 && j==0 ) || h<minH)
				minH = h;
		}
	}

	f32 hDiap = maxH-minH;

	for	(i=0; i<WalkabilitySizeX; i++)
	{
		for (j=0; j<WalkabilitySizeY; j++)
		{  		
			s32 xi = ( f32(i)/f32(WalkabilitySizeX-1) ) * f32(TerrainSize-1);
			s32 yj = ( f32(j)/f32(WalkabilitySizeY-1) ) * f32(TerrainSize-1);			

			core::vector3df pos = terrain->getCellPosition(xi, yj);  
			f32 h = core::math::Abs( pos.Y - minH ) / hDiap;

			Positions[i][j] = pos;

			if (walkabilitymap)
			{
				Pathfinder.walkability[i][j] = (
					walkabilitymap->getPixel( i, WalkabilitySizeY-j-1 ).getRed()==0 ? 0 : 1
					);
			}
			else
			{
				Pathfinder.walkability[i][j] = 0;
			}

			Pathfinder.influence[i][j] = 1;
			Pathfinder.heightinfluence[i][j] = 50 * h*h +1;
		}        
	} 
		
	res = true;

	c8 msg[255];

	if (res)
	{		
		sprintf(msg, "Walkability points calculated successfully ( %d x %d points).", WalkabilitySizeX, WalkabilitySizeY);
	}
	else
	{
		sprintf(msg, "Walkability points calculation finished with errors.");
	}

	LOGGER.log(msg);

	return res;
}

//----------------------------------------------------------------------------

//! Allocates memory for the pathfinder.
bool CPathFinderAStar::initialize(img::IImage* walkabilitymap)
{
	bool res = false;

	if (!walkabilitymap || !Parent)
		return res;	
	
	if (walkabilitymap->getDimension().Width<0 ||
		walkabilitymap->getDimension().Height<0)
		return res;	

	// free allocated memory

	deinitialize();	

	// initializing astar pathfinder

	LOGGER.log("Calculating walkability points . . .");		

	WalkabilitySizeX = walkabilitymap->getDimension().Width;
	WalkabilitySizeY = walkabilitymap->getDimension().Height;

	BBox = Parent->getBoundingBox();

	MinX = BBox.MinEdge.X;
	MinZ = BBox.MinEdge.Z;

	MaxX = BBox.MaxEdge.X;
	MaxZ = BBox.MaxEdge.Z;

	TileSizeX = (MaxX - MinX) / WalkabilitySizeX;
	TileSizeZ = (MaxZ - MinZ) / WalkabilitySizeY;

	DimX = MaxX - MinX;
	DimZ = MaxZ - MinZ;
	DimY = BBox.MaxEdge.Y - BBox.MinEdge.Y;

	f32 maxH = BBox.MaxEdge.Y;
	f32 minH = BBox.MinEdge.Y;

	core::vector3df vert;
	s32 i=0, j=0, cc=0;	

	// allocating memory

	Positions = new core::vector3df* [WalkabilitySizeX];

	for (i=0; i<WalkabilitySizeX; i++)
	{
		Positions[i] = new core::vector3df [WalkabilitySizeY];
	}

	Pathfinder.InitializePathfinder(WalkabilitySizeX, WalkabilitySizeY);

	// filling walkabilities arrays

	f32 hDiap = maxH-minH;

	f32 xdiap = BBox.getExtend().X;
	f32 ydiap = BBox.getExtend().Y;
	f32 zdiap = BBox.getExtend().Z;

	f32 xstep = (f32)xdiap/(f32)WalkabilitySizeX;
	f32 zstep = (f32)zdiap/(f32)WalkabilitySizeY;

	for	(i=0; i<WalkabilitySizeX; i++)
	{
		for (j=0; j<WalkabilitySizeY; j++)
		{  	
			f32 r = walkabilitymap->getPixel(i,WalkabilitySizeY-j).getRed();
			f32 g = walkabilitymap->getPixel(i,WalkabilitySizeY-j).getGreen();
			f32 h = g / 255.0f;

			core::vector3df pos(
				BBox.MinEdge.X + i*xstep,
				BBox.MinEdge.Y + h*hDiap,
				BBox.MinEdge.Z + j*zstep
				);			

			Positions[i][j] = pos;
			Pathfinder.walkability[i][j] = r==255 ? 1 : 0;
			Pathfinder.influence[i][j] = 1;
			Pathfinder.heightinfluence[i][j] = 1;		
		}        
	} 
	
	res = true;

	if (res)
		LOGGER.logInfo("Walkability points calculated successfully ( %d x %d points).",
			WalkabilitySizeX, WalkabilitySizeY);		
	else
		LOGGER.logErr("Walkability points calculation finished with errors.");

	return res;
}

//----------------------------------------------------------------------------

// Frees memory used by the pathfinder.
void CPathFinderAStar::deinitialize()
{
	for (s32 s=0; s<WalkabilitySizeX; s++)
		SAFE_DELETE_ARRAY(Positions[s]);
	SAFE_DELETE_ARRAY(Positions);

	Pathfinder.DeinitializePathfinder();

	WalkabilitySizeX = 0;
	WalkabilitySizeY = 0;

	BBox.reset(0,0,0);	
}

//----------------------------------------------------------------------------

//! finding path from start to destination point, return true on success
bool CPathFinderAStar::findPath(
	core::vector3df start_pos, core::vector3df dest_pos, 
    core::array< core::vector3df >& output_path,
	bool force_recalc)
{	
	bool res = false;	

	core::matrix4& absmatr = AbsoluteTransformation;

	SPathCell start_cell = getPathCellFromPosition(start_pos);
	SPathCell dest_cell = getPathCellFromPosition(dest_pos);

	if (!TransformedBBox.isPointInside(start_pos) ||
		!TransformedBBox.isPointInside(dest_pos) ||
		!start_cell.InsideWalkmap ||
		!dest_cell.InsideWalkmap)
		return res;
	
	int startingX = start_cell.Indices.X;
	int startingY = start_cell.Indices.Y;

	int targetX = dest_cell.Indices.X;
	int targetY = dest_cell.Indices.Y;

	if (!isWalkableCell(startingX, startingY) ||
		!isWalkableCell(targetX, targetY))
	// there is no any chance to find way,
	// so lets CPU make some other important work :)
		return false;

	dest_pos.Y = dest_cell.Position.Y;
	
	if (startingX == targetX && startingY == targetY)
	{
		output_path.clear();
		output_path.push_back( start_pos );	
		output_path.push_back( dest_pos  );	
		return true;
	}
	
	if (core::math::Abs(startingX-targetX) == 1 &&
		core::math::Abs(startingY-targetY) == 1 )
	{
		output_path.clear();
		if (Pathfinder.isWalkable(targetX, targetY))
		{
			output_path.push_back( start_pos );	
			output_path.push_back( dest_pos  );	
			res = true;
		}
		return res;
	}

	s32 output_size = output_path.size();
	if (!force_recalc && output_size>=2)
	{
		s32 output_size_1  = output_size - 1;
		s32 i;
		for (i=0; i<output_size_1; i++)
		{
			core::vector3df v = output_path[i]-start_pos;
			if (v.getLength() < TileSizeX)
				break;
		}
		for (s32 j=0; j<i; j++)
			output_path.erase(0);
		if (output_path.size()>=2)
			return true;
	}

	if (NoPathCounter==0)
	{
		res = Pathfinder.FindPath(
			startingX, startingY, targetX, targetY) == CPathfinder::found;
		if (!res)
			NoPathCounter++;
	}

	output_path.clear();

	if (res)
	{
		// storing finded path
		core::vector3df pos;
		for (s32 p=0; p<2*Pathfinder.pathLength; p+=2)
		{
			if (Pathfinder.pathBank[p] == targetX &&
				Pathfinder.pathBank[p+1] == targetY)
				break;
			pos  = Positions[Pathfinder.pathBank[p]][Pathfinder.pathBank[p+1]];
			absmatr.transformVect(pos);
			output_path.push_back( pos );
		}	
	}

	return res;	
}

//----------------------------------------------------------------------------

//! return path finder type
E_PATH_FINDER_TYPE CPathFinderAStar::getPathFinderType()
{
	return EPFT_ASTAR;
}

//-----------------------------------------------------------------------------

void CPathFinderAStar::mapUnitPosition(const core::vector3df& pos)
{
	const SPathCell &cell = getPathCellFromPosition(pos);

	s32 x = cell.Indices.X;
	s32 y = cell.Indices.Y;

	if (!cell.InsideWalkmap)
		return;

	s32 infl = 1;

	Pathfinder.addInfluence( x-1, y-1,   infl );
	Pathfinder.addInfluence( x-1, y,     infl );
	Pathfinder.addInfluence( x-1, y+1,   infl );

	Pathfinder.addInfluence( x,   y-1,   infl );
	Pathfinder.addInfluence( x,   y,     infl );
	Pathfinder.addInfluence( x,   y+1,   infl );

	Pathfinder.addInfluence( x+1, y-1,   infl );
	Pathfinder.addInfluence( x+1, y,     infl );
	Pathfinder.addInfluence( x+1, y+1,   infl );
}

//-----------------------------------------------------------------------------
	
void CPathFinderAStar::unmapUnitPosition(const core::vector3df &pos)
{
	const SPathCell &cell = getPathCellFromPosition(pos);

	s32 x = cell.Indices.X;
	s32 y = cell.Indices.Y;

	if (!cell.InsideWalkmap)
		return;

	s32 infl = -1;

	Pathfinder.addInfluence( x-1, y-1,   infl );
	Pathfinder.addInfluence( x-1, y,     infl );
	Pathfinder.addInfluence( x-1, y+1,   infl );

	Pathfinder.addInfluence( x,   y-1,   infl );
	Pathfinder.addInfluence( x,   y,     infl );
	Pathfinder.addInfluence( x,   y+1,   infl );

	Pathfinder.addInfluence( x+1, y-1,   infl );
	Pathfinder.addInfluence( x+1, y,     infl );
	Pathfinder.addInfluence( x+1, y+1,   infl );
}

//-----------------------------------------------------------------------------

void CPathFinderAStar::clearUnitMappings()
{
	Pathfinder.swapWalkabilityBuffers();
}

//-----------------------------------------------------------------------------

core::dimension2df CPathFinderAStar::getCellSize()
{
	return core::dimension2df(TileSizeX, TileSizeZ);
}

//-----------------------------------------------------------------------------

core::dimension2di CPathFinderAStar::getCellsDimension()
{
	return core::dimension2di(WalkabilitySizeX, WalkabilitySizeY);
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isWalkableCell(s32 x, s32 y)
{
	const SPathCell &cell = getPathCell(x, y);

	if (!cell.InsideWalkmap)
		return false;

	return Pathfinder.isWalkable(x, y);
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isWalkablePosition(const core::vector3df &pos)
{
	const SPathCell &cell = getPathCellFromPosition(pos);

	s32 x = cell.Indices.X;
	s32 y = cell.Indices.Y;

	if (!cell.InsideWalkmap)
		return false;

	return Pathfinder.isWalkable(x, y);
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isWalkableLine(const core::vector3df &src, const core::vector3df &dst)
{
	SPathCell src_cell = getPathCellFromPosition(src);
	SPathCell dst_cell = getPathCellFromPosition(dst);

	if (!src_cell.InsideWalkmap ||
		!dst_cell.InsideWalkmap)
		return false;

	core::list <core::vector2di> pixels;
	core::math::draw_line(
		src_cell.Position.X, src_cell.Position.Z,
		dst_cell.Position.X, dst_cell.Position.Z,
		pixels);

	core::list<core::vector2di>::iterator iter = pixels.begin();
	for (; iter!=pixels.end(); ++iter)
	{
		core::vector2di pixel = *iter;
		if (!Pathfinder.isWalkable( pixel.X, pixel.Y ))
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isViewableLine(
	const core::vector3df &src, const core::vector3df &dst)
{
	return isWalkableLine(src, dst);
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isPointInside(const core::vector3df &point)
{
	if (TransformedBBox.isPointInside(point))
		return true;
	return false;
}

//-----------------------------------------------------------------------------

core::vector3df CPathFinderAStar::getPositionOffsetFor(const core::vector3df &pos)
{
	return getPathCellFromPosition(pos).Position - pos;
}

//----------------------------------------------------------------------------

void CPathFinderAStar::validatePositionToGround(core::vector3df &position)
{	
	position.Y = getPathCellFromPosition(position).Position.Y;
}

//----------------------------------------------------------------------------

const SPathCell& CPathFinderAStar::getPathCellFromPosition(const core::vector3df &position)
{
	static SPathCell cell;

	core::vector3df pos = position;
	core::matrix4 abs_inv = AbsoluteTransformation.getInversed();

	// transform into local space
	abs_inv.transformVect(pos);

	// detecting cell indices
	s32 x = s32((pos.X - BBox.MinEdge.X) / TileSizeX);
	s32 y = s32((pos.Z - BBox.MinEdge.Z) / TileSizeZ);

	cell = getPathCell(x, y);

	return cell;
}

//----------------------------------------------------------------------------

const SPathCell& CPathFinderAStar::getPathCell(s32 x, s32 y)
{
	static SPathCell cell;

	cell.Indices.set(x, y);

	if (x >= 0 && y >= 0 &&
		x < Pathfinder.mapWidth &&
		y < Pathfinder.mapHeight)
	{
		cell.InsideWalkmap	= true;
		cell.Walkable		= Pathfinder.isWalkable(x, y);
		cell.Position		= Positions[x][y];		
	}
	else
	{
		cell.InsideWalkmap	= false;
		cell.Walkable		= false;
		cell.Position.set(
			BBox.MinEdge.X + x * TileSizeX,
			TransformedBBox.MaxEdge.Y,
			BBox.MinEdge.Z + y * TileSizeZ);
	}

	// transform to global space
	AbsoluteTransformation.transformVect(cell.Position);

	return cell;
}

//-----------------------------------------------------------------------------

core::vector2di CPathFinderAStar::getNearestWalkableCell(const core::vector3df &pos)
{
	const SPathCell &cell = getPathCellFromPosition(pos);

	s32 x = cell.Indices.X;
	s32 y = cell.Indices.Y;

	if (cell.InsideWalkmap)
	{
		s32 _x[8] = { x-1, x+1, x,   x,   x-1, x+1, x-1, x+1 };
		s32 _y[8] = { y,   y,   y-1, y+1, y-1, y+1, y+1, y-1 };

		s32 choosed_cell = -1;
		f32 min_dist = -1.0f;

		for (s32 i=0; i<8; i++)
		{
			if (Pathfinder.walkability[_x[i]][_y[i]]==0)
			{
				f32 dist = core::vector3df(pos-Positions[_x[i]][_y[i]]).getLength();
				if (min_dist>dist || min_dist==-1.0f)
				{
					min_dist = dist;
					choosed_cell = i;
				}
			}
		}

		if (choosed_cell != -1)
		{
			x = _x[choosed_cell];
			y = _y[choosed_cell];
		}
	}

	return core::vector2di(x, y);
}

//-----------------------------------------------------------------------------

u8 CPathFinderAStar::getInfluence(s32 x, s32 y)
{
	return Pathfinder.getInfluence(x, y);
}

//-----------------------------------------------------------------------------

u16 CPathFinderAStar::getHeightInfluence(s32 x, s32 y)
{
	return Pathfinder.getHeightInfluence(x, y);
}

//-----------------------------------------------------------------------------

bool CPathFinderAStar::isBusy(const core::vector3df& pos)
{
	SPathCell cell = getPathCellFromPosition(pos);
	s32 x = cell.Indices.X;
	s32 y = cell.Indices.Y;
	if (!cell.InsideWalkmap)
		return true;
	return Pathfinder.isBusy(x, y);
}

//-----------------------------------------------------------------------------

// Allocates memory for the pathfinder.
void CPathFinderAStar::CPathfinder::InitializePathfinder(int width, int height)
{
	if (!(width>0 &&  height>0))
		return;

	// free old memory

	DeinitializePathfinder();

	// allocate new memory

	mapWidth  = width;
	mapHeight = height;

	int x = 0, y=0;

	walkability     = new u8* [mapWidth];
	influence       = new u8* [mapWidth];
	heightinfluence = new u16*[mapWidth];

	for (x=0; x<mapWidth; x++)
	{		
		walkability[x]     = new u8 [mapHeight];
		influence[x]       = new u8 [mapHeight];
		heightinfluence[x] = new u16[mapHeight];

		for (y=0; y<mapHeight; y++)
		{
			walkability[x][y] = 0;
			influence  [x][y] = 1;	
			heightinfluence[x][y] = 1;
		}
	}

	openList = new int [mapWidth*mapHeight+2];
	openX    = new int [mapWidth*mapHeight+2];
	openY    = new int [mapWidth*mapHeight+2];

	for (x=0; x<(mapWidth*mapHeight+2); x++)
	{
		openList[x]=0;
		openX[x]=0;
		openY[x]=0;
	}

	whichList = new int* [mapWidth+1];

	for (x=0; x<(mapWidth+1); x++)
	{
		whichList[x] = new int [mapHeight+1];

		for (y=0; y<(mapHeight+1); y++)
		{
			whichList[x][y] = 0;
		}
	}	

	parentX = new int* [mapWidth+1];
	parentY = new int* [mapWidth+1];

	for (x=0; x<(mapWidth+1); x++)
	{
		parentX[x] = new int [mapHeight+1];
		parentY[x] = new int [mapHeight+1];

		for (y=0; y<(mapHeight+1); y++)
		{
			parentX[x][y] = 0;
			parentY[x][y] = 0;	
		}
	}

	Fcost = new int [mapWidth*mapHeight+2];

	Gcost = new int* [mapWidth+1];

	for (x=0; x<(mapWidth+1); x++)
	{
		Gcost[x] = new int [mapHeight+1];
	}

	Hcost = new int [mapWidth*mapHeight+2];

	pathBank = new int [4];
}

//-----------------------------------------------------------------------------

// Frees memory used by the pathfinder.
void CPathFinderAStar::CPathfinder::DeinitializePathfinder()
{
	if (!(mapWidth>0 && mapHeight>0))
		return;

	int x = 0;

	for (x=0; x<mapWidth; x++)
	{
		SAFE_DELETE_ARRAY(walkability[x]);
		SAFE_DELETE_ARRAY(influence[x]);
		SAFE_DELETE_ARRAY(heightinfluence[x]);
	}
	SAFE_DELETE_ARRAY(walkability);
	SAFE_DELETE_ARRAY(influence);
	SAFE_DELETE_ARRAY(heightinfluence);

	SAFE_DELETE_ARRAY(openList);
	
	for (x=0; x<(mapWidth+1); x++)
		SAFE_DELETE_ARRAY(whichList[x]);
	SAFE_DELETE_ARRAY(whichList);

	SAFE_DELETE_ARRAY(openX);
	SAFE_DELETE_ARRAY(openY);

	for (x=0; x<(mapWidth+1); x++)
	{
		SAFE_DELETE_ARRAY(parentX[x]);
		SAFE_DELETE_ARRAY(parentY[x]);
	}
	SAFE_DELETE_ARRAY(parentX);
	SAFE_DELETE_ARRAY(parentY);

	SAFE_DELETE_ARRAY(Fcost);
	
	for (x=0; x<(mapWidth+1); x++)
		SAFE_DELETE_ARRAY(Gcost[x]);
	SAFE_DELETE_ARRAY(Gcost);

	SAFE_DELETE_ARRAY(Hcost);

	SAFE_DELETE_ARRAY(pathBank);

	mapWidth = mapHeight = 0;
}

//-----------------------------------------------------------------------------

// Finds a path using A*
int CPathFinderAStar::CPathfinder::FindPath(
	int startingX, int startingY, int targetX, int targetY
	)
{
	int onOpenList=0, parentXval=0, parentYval=0,
		a=0, b=0, m=0, u=0, v=0, temp=0, corner=0, numberOfOpenListItems=0,
		addedGCost=0, tempGcost = 0, path = 0,
		tempx, pathX, pathY, cellPosition,
		newOpenListItemID=0;

	int startX = startingX;
	int startY = startingY;	

	//2.Quick Path Checks: Under the some circumstances no path needs to
	//	be generated ...

	//	If starting location and target are in the same location...
	if (startX == targetX && startY == targetY && pathLocation > 0)
		return found;
	if (startX == targetX && startY == targetY && pathLocation == 0)
		return nonexistent;

	//	If target square is unwalkable, return that it's a nonexistent path.
	if (walkability[targetX][targetY] == unwalkable)
		goto noPath;

	//3.Reset some variables that need to be cleared
	if (onClosedList > 1000000) //reset whichList occasionally
	{
		for (int x = 0; x < mapWidth;x++) 
		{
			for (int y = 0; y < mapHeight;y++)
				whichList [x][y] = 0;
		}
		onClosedList = 10;	
	}
	onClosedList = onClosedList+2; //changing the values of onOpenList and onClosed list is faster than redimming whichList() array
	onOpenList = onClosedList-1;
	pathLength = notStarted;//i.e, = 0
	pathLocation = notStarted;//i.e, = 0
	Gcost[startX][startY] = 0; //reset starting square's G value to 0

	//4.Add the starting location to the open list of squares to be checked.
	numberOfOpenListItems = 1;
	openList[1] = 1;//assign it as the top (and currently only) item in the open list, which is maintained as a binary heap (explained below)
	openX[1] = startX ; openY[1] = startY;

	do
	//5.Do the following until a path is found or deemed nonexistent.
	{
	
		if (numberOfOpenListItems != 0)
		//6.If the open list is not empty, take the first cell off of the list.
		//	This is the lowest F cost cell on the open list.
		{
			//7. Pop the first item off the open list.
			parentXval = openX[openList[1]];
			parentYval = openY[openList[1]]; //record cell coordinates of the item
			whichList[parentXval][parentYval] = onClosedList;//add the item to the closed list

			//	Open List = Binary Heap: Delete this item from the open list, which
			//  is maintained as a binary heap. For more information on binary heaps, see:
			//	http://www.policyalmanac.org/games/binaryHeaps.htm
			numberOfOpenListItems = numberOfOpenListItems - 1;//reduce number of open list items by 1	
				
			//	Delete the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
			openList[1] = openList[numberOfOpenListItems+1];//move the last item in the heap up to slot #1
			v = 1;

			//	Repeat the following until the new item in slot #1 sinks to its proper spot in the heap.
			do
			{
				u = v;		
				if (2*u+1 <= numberOfOpenListItems) //if both children exist
				{
	 				//Check if the F cost of the parent is greater than each child.
					//Select the lowest of the two children.
					if (Fcost[openList[u]] >= Fcost[openList[2*u]]) 
						v = 2*u;
					if (Fcost[openList[v]] >= Fcost[openList[2*u+1]]) 
						v = 2*u+1;		
				}
				else
				{
					if (2*u <= numberOfOpenListItems) //if only child #1 exists
					{
	 					//Check if the F cost of the parent is greater than child #1	
						if (Fcost[openList[u]] >= Fcost[openList[2*u]]) 
							v = 2*u;
					}
				}

				if (u != v) //if parent's F is > one of its children, swap them
				{
					temp = openList[u];
					openList[u] = openList[v];
					openList[v] = temp;			
				}
				else
					break; //otherwise, exit loop				
			}
			while (true);//reorder the binary heap

			//7.Check the adjacent squares. (Its "children" -- these path children
			//	are similar, conceptually, to the binary heap children mentioned
			//	above, but don't confuse them. They are different. Path children
			//	are portrayed in Demo 1 with grey pointers pointing toward
			//	their parents.) Add these adjacent child squares to the open list
			//	for later consideration if appropriate (see various if statements
			//	below).
			for (b = parentYval-1; b <= parentYval+1; b++)
			{
				for (a = parentXval-1; a <= parentXval+1; a++)
				{
					//	If not off the map (do this first to avoid array out-of-bounds errors)
					if (a != -1 && b != -1 && a != mapWidth && b != mapHeight)
					{

						//	If not already on the closed list (items on the closed list have
						//	already been considered and can now be ignored).			
						if (whichList[a][b] != onClosedList) 
						{ 						
							//	If not a wall/obstacle square.
							if (walkability [a][b] != unwalkable) 
							{ 								
								//	Don't cut across corners
								corner = walkable;	
								if (a == parentXval-1) 
								{
									if (b == parentYval-1)
									{
										if (walkability[parentXval-1][parentYval] == unwalkable
											|| walkability[parentXval][parentYval-1] == unwalkable) 
											corner = unwalkable;
									}
									else if (b == parentYval+1)
									{
										if (walkability[parentXval][parentYval+1] == unwalkable
											|| walkability[parentXval-1][parentYval] == unwalkable) 
											corner = unwalkable; 
									}
								}
								else if (a == parentXval+1)
								{
									if (b == parentYval-1)
									{
										if (walkability[parentXval][parentYval-1] == unwalkable 
											|| walkability[parentXval+1][parentYval] == unwalkable) 
											corner = unwalkable;
									}
									else if (b == parentYval+1)
									{
										if (walkability[parentXval+1][parentYval] == unwalkable 
											|| walkability[parentXval][parentYval+1] == unwalkable)
											corner = unwalkable; 
									}
								}	

								if (corner == walkable) 
								{						
								//	If not already on the open list, add it to the open list.			
									if (whichList[a][b] != onOpenList) 
									{	
										//Create a new open list item in the binary heap.
										newOpenListItemID = newOpenListItemID + 1; //each new item has a unique ID #
										m = numberOfOpenListItems+1;
										openList[m] = newOpenListItemID;//place the new open list item (actually, its ID#) at the bottom of the heap
										openX[newOpenListItemID] = a;
										openY[newOpenListItemID] = b;//record the x and y coordinates of the new item

										//Figure out its G cost
										if (abs(a-parentXval) == 1 && abs(b-parentYval) == 1)
											addedGCost = 14;//cost of going to diagonal squares	
										else	
											addedGCost = 10;//cost of going to non-diagonal squares											

										Gcost[a][b] = 
											Gcost[parentXval][parentYval] + 
											addedGCost * influence[a][b] * heightinfluence[a][b];

										//Figure out its H and F costs and parent
										Hcost[openList[m]] = 10*(abs(a - targetX) + abs(b - targetY));
										Fcost[openList[m]] =  ( Gcost[a][b] + Hcost[openList[m]] );
										parentX[a][b] = parentXval ; parentY[a][b] = parentYval;	

										//Move the new open list item to the proper place in the binary heap.
										//Starting at the bottom, successively compare to parent items,
										//swapping as needed until the item finds its place in the heap
										//or bubbles all the way to the top (if it has the lowest F cost).
										while (m != 1) //While item hasn't bubbled to the top (m=1)	
										{
											//Check if child's F cost is < parent's F cost. If so, swap them.	
											if (Fcost[openList[m]] <= Fcost[openList[m/2]])
											{
												temp = openList[m/2];
												openList[m/2] = openList[m];
												openList[m] = temp;
												m = m/2;
											}
											else
												break;
										}
										numberOfOpenListItems = numberOfOpenListItems+1;//add one to the number of items in the heap

										//Change whichList to show that the new item is on the open list.
										whichList[a][b] = onOpenList;
									}								
									else //If whichList(a,b) = onOpenList
									//8.If adjacent cell is already on the open list, check to see if this 
									//	path to that cell from the starting location is a better one. 
									//	If so, change the parent of the cell and its G and F costs.	
									{									
										//Figure out the G cost of this possible new path
										if (abs(a-parentXval) == 1 && abs(b-parentYval) == 1)
											addedGCost = 14;//cost of going to diagonal tiles	
										else	
											addedGCost = 10;//cost of going to non-diagonal tiles
										
										tempGcost = 
											Gcost[parentXval][parentYval] + 
											addedGCost * influence[a][b] * heightinfluence[a][b];
										
										//If this path is shorter (G cost is lower) then change
										//the parent cell, G cost and F cost. 		
										if ( tempGcost < Gcost[a][b]) //if G cost is less,
										{
											parentX[a][b] = parentXval; //change the square's parent
											parentY[a][b] = parentYval;
											Gcost[a][b] = tempGcost;//change the G cost			

											//Because changing the G cost also changes the F cost, if
											//the item is on the open list we need to change the item's
											//recorded F cost and its position on the open list to make
											//sure that we maintain a properly ordered open list.
											for (int x = 1; x <= numberOfOpenListItems; x++) //look for the item in the heap
											{
												if (openX[openList[x]] == a && openY[openList[x]] == b) //item found
												{
													Fcost[openList[x]] = ( Gcost[a][b] + Hcost[openList[x]] );//change the F cost
													
													//See if changing the F score bubbles the item up from it's current location in the heap
													m = x;
													while (m != 1) //While item hasn't bubbled to the top (m=1)	
													{
														//Check if child is < parent. If so, swap them.	
														if (Fcost[openList[m]] < Fcost[openList[m/2]])
														{
															temp = openList[m/2];
															openList[m/2] = openList[m];
															openList[m] = temp;
															m = m/2;
														}
														else
															break;
													} 
													break; //exit for x = loop

												} //If openX(openList(x)) = a

											} //For x = 1 To numberOfOpenListItems

										}//If tempGcost < Gcost(a,b)

									}//else If whichList(a,b) = onOpenList	

								}//If not cutting a corner

							}//If not a wall/obstacle square.

						}//If not already on the closed list 

					}//If not off the map

				}//for (a = parentXval-1; a <= parentXval+1; a++){

			}//for (b = parentYval-1; b <= parentYval+1; b++){

		}//if (numberOfOpenListItems != 0)	
		else
		//9.If open list is empty then there is no path.	
		{
			path = nonexistent; break;
		}  

		//If target is added to open list then path has been found.
		if (whichList[targetX][targetY] == onOpenList)
		{
			path = found; break;
		}

	}
	while (1);//Do until path is found or deemed nonexistent	
	
	if (path == found)
	//10. Save the path if it exists.
	{
		//a.Working backwards from the target to the starting location by checking
		//	each cell's parent, figure out the length of the path.
		pathX = targetX; pathY = targetY;
		do
		{
			//Look up the parent of the current cell.	
			tempx = parentX[pathX][pathY];		
			pathY = parentY[pathX][pathY];
			pathX = tempx;

			//Figure out the path length
			pathLength = pathLength + 1;
		}
		while (pathX != startX || pathY != startY);

		//b.Resize the data bank to the right size in bytes
		SAFE_DELETE_ARRAY(pathBank);
		pathBank = new int [pathLength*8];

		//c. Now copy the path information over to the databank. Since we are
		//	working backwards from the target to the start location, we copy
		//	the information to the data bank in reverse order. The result is
		//	a properly ordered set of path data, from the first step to the
		//	last.
		pathX = targetX ; pathY = targetY;
		cellPosition = pathLength*2;//start at the end	
		do
		{
			cellPosition = cellPosition - 2;//work backwards 2 integers
			pathBank[cellPosition] = pathX;
			pathBank[cellPosition+1] = pathY;

			//d.Look up the parent of the current cell.	
			tempx = parentX[pathX][pathY];		
			pathY = parentY[pathX][pathY];
			pathX = tempx;

			//e.If we have reached the starting square, exit the loop.	
		}
		while (pathX != startX || pathY != startY);	

		//11.Read the first path step into xPath/yPath arrays
		xPath = pathBank[0];
		yPath = pathBank[1];

	} // if (path == found)

	return path;

noPath:	

	//12.If there is no path to the selected target, set the pathfinder's
	//	xPath and yPath equal to its current location and return that the
	//	path is nonexistent.

	xPath = startingX;
	yPath = startingY;

	return nonexistent;
}

//----------------------------------------------------------------------------

void CPathFinderAStar::CPathfinder::swapWalkabilityBuffers()
{
	s32 line_size = sizeof(u8)*mapHeight;

	s32 x=0;

	for (x=0; x<mapWidth; x++)
	{	
		memset(influence[x], (u8)1,  line_size);
	}
}

//----------------------------------------------------------------------------

void CPathFinderAStar::CPathfinder::addInfluence(s32 x, s32 y, s32 infl)
{
	if (x>=0 && y>=0 && x<mapWidth && y<mapHeight)
	{
		s32 newinfl = influence[x][y] + infl;

		CHECK_RANGE(newinfl, 1, 255);

		influence[x][y] = newinfl;
	}
}

//----------------------------------------------------------------------------

bool CPathFinderAStar::CPathfinder::isWalkable(s32 x, s32 y)
{
	if (x>=0 && y>=0 && x<mapWidth && y<mapHeight)
	{
		return ( walkability[x][y] == 0 );
	}

	return false;
}

//----------------------------------------------------------------------------

bool CPathFinderAStar::CPathfinder::isBusy(s32 x, s32 y)
{
	if (x>=0 && y>=0 && x<mapWidth && y<mapHeight)
	{
		return influence[x][y]>1;
	}

	return true;
}

//----------------------------------------------------------------------------

u8 CPathFinderAStar::CPathfinder::getInfluence(s32 x, s32 y)
{
	if (x>=0 && y>=0 && x<mapWidth && y<mapHeight)
	{
		return  influence[x][y];
	}

	return 1;
}

//----------------------------------------------------------------------------

u16 CPathFinderAStar::CPathfinder::getHeightInfluence(s32 x, s32 y)
{
	if (x>=0 && y>=0 && x<mapWidth && y<mapHeight)
	{
		return  heightinfluence[x][y];
	}

	return 0;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------