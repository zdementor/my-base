//|-------------------------------------------------------------------------
//| File:        CPathFinderManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CPathFinderManager.h"
#include "CPathFinderAStar.h"

#include <scn/ISceneManager.h>
#include <scn/ITerrainSceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! constructor
CPathFinderManager::CPathFinderManager() 
: 
PathFindingEnabled(true), PathFindCalcCount(0)
{
#if MY_DEBUG_MODE 
	setClassName("CPathFinderManager");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CPathFinderManager::~CPathFinderManager()
{
}

//----------------------------------------------------------------------------

//! return path find calc count
u32 CPathFinderManager::getPathFindCalcCount()
{
	return PathFindCalcCount;
}

//----------------------------------------------------------------------------

//! zeroing path find calc counter
void CPathFinderManager::zeroPathFindCalcCounter()
{
	PathFindCalcCount = 0;
}

//----------------------------------------------------------------------------

//! finding path from start to destination point, return true on success
bool CPathFinderManager::findPath(
	core::vector3df start_pos, core::vector3df dest_pos, 
	core::array< core::vector3df >& output_path,
	bool force_recalc
	)
{
	PathFindCalcCount++;

	bool finded = false;

	if (PathFindingEnabled)
	{
		output_path.clear();	
		
		finded = true;

		for (u32 p=0; p < CPathFinder::PathFinders.size(); p++)	
		{
			IPathFinder *pfinder = CPathFinder::PathFinders[p];

			if (pfinder->isPointInside(start_pos) && pfinder->isPointInside(dest_pos))
			{
				SPathCell start_cell = pfinder->getPathCellFromPosition(start_pos);
				SPathCell dest_cell = pfinder->getPathCellFromPosition(dest_pos);

				s32 startx = start_cell.Indices.X;
				s32 starty = start_cell.Indices.Y;
				
				s32 destx = dest_cell.Indices.X;
				s32 desty = dest_cell.Indices.Y;
				
				if (!pfinder->isWalkableCell(startx, starty))
				// if starting cell is not walkable, trying to find nearest walkable 
				{
					core::vector2di walk_cell_beg =	pfinder->getNearestWalkableCell(start_pos);

					startx = walk_cell_beg.X;
					starty = walk_cell_beg.Y;
				}

				if (!pfinder->isWalkableCell(destx, desty))
				// if destination cell is not walkable, trying to find nearest walkable 
				{
					core::vector2di walk_cell_end =	pfinder->getNearestWalkableCell(dest_pos);

					destx = walk_cell_end.X;
					desty = walk_cell_end.Y;
				}	

				start_pos = pfinder->getPathCell(startx, starty).Position;
				dest_pos  = pfinder->getPathCell(destx, desty).Position;
				
				// fire pathfining

				finded = pfinder->findPath(
					start_pos, dest_pos, output_path, force_recalc
					);

				if (finded)
				{
					return finded;
				}
			}
		}

		output_path.clear();

		// there is no way, so lets return simple start-dest way

		output_path.push_back(start_pos);
		output_path.push_back(dest_pos);

		finded = false;
	}	

	return finded;
}

//----------------------------------------------------------------------------

//! adding new pathfinder to the pathfinders pool (A* pathfinder)
IPathFinder* CPathFinderManager::addAStarPathFinder(
	ITerrainSceneNode* terrain, img::IImage* walkability
	)
{
	if (!terrain || !walkability)
		return NULL;

	ISceneNode* parent = dynamic_cast<ISceneNode*>(terrain);
	s32 id = -1;

	CPathFinderAStar* pathfinder = new CPathFinderAStar(parent, id);
	pathfinder->drop();

	if (!pathfinder->initialize(terrain, walkability))
	{
		LOGGER.log("Can't initialize pathfinder!", io::ELL_ERROR);
	}			

	return pathfinder;
}

//----------------------------------------------------------------------------

//! adding new pathfinder to the pathfinders pool (A* pathfinder)
IPathFinder* CPathFinderManager::addAStarPathFinder(
	ISceneNode* parent, img::IImage* walkability
	)
{
	if (!parent || !walkability)
		return NULL;

	s32 id = -1;

	CPathFinderAStar* pathfinder = new CPathFinderAStar(parent, id);
	pathfinder->drop();

	if (!pathfinder->initialize(walkability))
	{
		LOGGER.log("Can't initialize pathfinder!", io::ELL_ERROR);
	}			

	return pathfinder;
}

//----------------------------------------------------------------------------

//! enabling/disabling our pathfinders, except default pathfinder, it always working
void CPathFinderManager::setPathFindingEnabled(bool enabled)
{
	PathFindingEnabled = enabled;
}

//----------------------------------------------------------------------------

//! return true if our pathfinders are enabled
bool CPathFinderManager::isPathFindingEnabled()
{
	return PathFindingEnabled;
}

//----------------------------------------------------------------------------

void CPathFinderManager::mapUnitPosition(const core::vector3df& pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
			CPathFinder::PathFinders[p]->mapUnitPosition(pos);
	}
}


//----------------------------------------------------------------------------

void CPathFinderManager::unmapUnitPosition(const core::vector3df& pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
			CPathFinder::PathFinders[p]->unmapUnitPosition(pos);
	}
}

//----------------------------------------------------------------------------

void CPathFinderManager::clearUnitMappings()
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		CPathFinder::PathFinders[p]->clearUnitMappings();
	}
}

//----------------------------------------------------------------------------

bool CPathFinderManager::isWalkablePosition(core::vector3df pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos) && 
			CPathFinder::PathFinders[p]->isWalkablePosition(pos)
			)
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

bool CPathFinderManager::isWalkableLine(core::vector3df &src, core::vector3df &dst)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(src) && 
			CPathFinder::PathFinders[p]->isPointInside(dst) &&
			CPathFinder::PathFinders[p]->isWalkableLine(src, dst)
			)
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool CPathFinderManager::isViewableLine(core::vector3df &src, core::vector3df &dst)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(src) && 
			CPathFinder::PathFinders[p]->isPointInside(dst) &&
			CPathFinder::PathFinders[p]->isViewableLine(src, dst))
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------

void CPathFinderManager::getPathCellLine(
	const core::vector3df &start_pos, const core::vector3df &end_pos,
    core::array <SPathCell> &output_line)
{
	f32 path_length = (end_pos - start_pos).getLength();

	output_line.set_used(0);

	u32 pcnt = CPathFinder::PathFinders.size();
	SPathCell start_cell, end_cell;
	for (u32 p=0; p < pcnt; p++)	
	{
		IPathFinder *pfinder = CPathFinder::PathFinders[p];
		f32 max_path_length = pfinder->getBoundingBox().getExtend().getLength();

		if (!pfinder->isPointInside(start_pos) &&
			!pfinder->isPointInside(end_pos))
			continue;
		if (path_length > max_path_length)
		{
			LOGGER.logWarn("Path is to length for pathfinder %.3f (max=%.3f)",
				path_length, max_path_length);
			continue;
		}

		start_cell = pfinder->getPathCellFromPosition(start_pos);
		end_cell = pfinder->getPathCellFromPosition(end_pos);

		static core::array <core::vector2di> way;
		core::math::draw_line(
			start_cell.Indices.X, start_cell.Indices.Y,
			end_cell.Indices.X, end_cell.Indices.Y, way);
		u32 wcnt = way.size();

		output_line.set_used(wcnt);
		output_line.set_used(0);
		for (u32 i = 0; i < wcnt; i++)
			output_line.push_back(pfinder->getPathCell(way[i].X, way[i].Y));

		break;
	}
}

//----------------------------------------------------------------------------

core::vector3df CPathFinderManager::getPositionOffsetFor(core::vector3df pos)
{
	core::vector3df pos_offset(0,0,0);

	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
		{
			pos_offset = CPathFinder::PathFinders[p]->getPositionOffsetFor(pos);
			break;
		}
	}

	return pos_offset;
}

//----------------------------------------------------------------------------

void CPathFinderManager::validatePositionToGround(core::vector3df &pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
		{
			CPathFinder::PathFinders[p]->validatePositionToGround(pos);
			return;
		}
	}
}

//----------------------------------------------------------------------------

core::vector3df CPathFinderManager::getGroundPositionFor(const core::vector3df &pos)
{
	core::vector3df _pos = pos;
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(_pos))
		{
			CPathFinder::PathFinders[p]->validatePositionToGround(_pos);
			break;
		}
	}
	return _pos;
}

//----------------------------------------------------------------------------

f32 CPathFinderManager::getHeightOverGround(const core::vector3df &pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
		{
			core::vector3df _pos = pos;
			CPathFinder::PathFinders[p]->validatePositionToGround(_pos);
			
			return pos.Y-_pos.Y;
		}
	}

	return 0.0f;
}

//----------------------------------------------------------------------------

bool CPathFinderManager::isPointInsidePathFinding(const core::vector3df &pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
		{	
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------

bool CPathFinderManager::checkIntersectionWithRay(const core::line3df &ray)
{
	bool collided = false;

	core::array <scn::SPathCell> view_cell_line;
	core::array <core::vector3df> view_line;

	getPathCellLine(ray.start, ray.end, view_cell_line);

	s32 vccnt = view_cell_line.size();
	if (vccnt < 2)
		return false;

	f32 dh = (ray.end.Y - ray.start.Y) / f32(vccnt - 1.0f);
	view_line.set_used(vccnt);
	for (s32 i = 0; i < vccnt; i++)
	{
		view_line[i].set(
			view_cell_line[i].Position.X,
			view_cell_line[0].Position.Y + dh * i,
			view_cell_line[i].Position.Z);
	}

	for (s32 i = 0; i < vccnt; i++)
		if (view_line[i].Y < view_cell_line[i].Position.Y)
			return true;

	return false;
}

//----------------------------------------------------------------------------

bool CPathFinderManager::isBusy(const core::vector3df& pos)
{
	for (u32 p=0; p<CPathFinder::PathFinders.size(); p++)	
	{
		if (CPathFinder::PathFinders[p]->isPointInside(pos))
		{
			return CPathFinder::PathFinders[p]->isBusy(pos);
		}
	}

	return false;
}

//----------------------------------------------------------------------------

IPathFinderManager* createPathFinderManager()
{
	return new CPathFinderManager;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------