//|-------------------------------------------------------------------------
//| File:        CPathFinderAStar.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CPathFinderAStarHPP
#define CPathFinderAStarHPP
//----------------------------------------------------------------------------

#include <scn/IPathFinder.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CPathFinder : public IPathFinder
{
	friend class CPathFinderManager;

public:

	//! Constructor
	CPathFinder(ISceneNode* parent, s32 id)
		: IPathFinder(ESNT_PATH_FINDER_SCENE_NODE, parent, id) 
	{
		PathFinders.push_back(this);
	}

	//! Destructor
    virtual ~CPathFinder() 
	{
		for (u32 p=0; p<PathFinders.size(); p++)
		{
			if (PathFinders[p] == this)
			{
				PathFinders.erase(p);
				break;
			}
		}
	}
	
protected:

	static core::array<IPathFinder*> PathFinders;
};

//----------------------------------------------------------------------------

class CPathFinderAStar : public CPathFinder
{
public:

	CPathFinderAStar(ISceneNode* parent, s32 id);

    virtual ~CPathFinderAStar();

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);
	virtual void OnPostRender(u32 timeMs);

    virtual const core::aabbox3d<f32>& getBoundingBox() const;

    virtual vid::SMaterial& getMaterial(s32 i);

    virtual s32 getMaterialsCount();

    virtual bool findPath(
        core::vector3df start_pos, core::vector3df dest_pos, 
        core::array <core::vector3df> &output_path,
		bool force_recalc = false);

	virtual E_PATH_FINDER_TYPE getPathFinderType();

	bool initialize(ITerrainSceneNode* terrain, img::IImage* walkabilitymap);

	bool initialize(img::IImage* walkabilitymap);

	virtual void mapUnitPosition  (const core::vector3df& pos);
	virtual void unmapUnitPosition(const core::vector3df& pos);

	virtual void clearUnitMappings();

	virtual core::dimension2df getCellSize();

	virtual core::dimension2di getCellsDimension();

	virtual bool isWalkableCell(s32 x, s32 y);

	virtual bool isWalkablePosition(const core::vector3df &pos);
	
	virtual bool isWalkableLine(const core::vector3df &src, const core::vector3df &dst);

	virtual bool isViewableLine(const core::vector3df &src, const core::vector3df &dst);

	virtual bool isPointInside(const core::vector3df &point);

	virtual core::vector3df getPositionOffsetFor(const core::vector3df &pos);

	virtual void validatePositionToGround(core::vector3df &pos);

	virtual const SPathCell& getPathCellFromPosition(const core::vector3df &position);

	virtual const SPathCell& getPathCell(s32 x, s32 y);

	virtual core::vector2di getNearestWalkableCell(const core::vector3df &pos);

	virtual u8 getInfluence(s32 x, s32 y);	
	virtual u16 getHeightInfluence(s32 x, s32 y);	
	
	virtual bool isBusy(const core::vector3df& pos);

	//!
	virtual const c8* getSceneCorePropertiesXMLString()
	{
		static core::stringc xmlstr;
		xmlstr = "";
		xmlstr.append("\n");
		return xmlstr.c_str();
	}

private:

	// Frees memory used by the pathfinder.
	void deinitialize();

	s32 WalkabilitySizeX, WalkabilitySizeY;
	core::vector3df** Positions;

	vid::SMaterial Material;

	f32 MinX, MinZ, MaxX, MaxZ;
	f32 TileSizeX, TileSizeZ;
	f32 DimX, DimZ, DimY;	

	class CPathfinder
	{
	public:

		CPathfinder() 
			: mapWidth(0), mapHeight(0), onClosedList(10),
		walkability(NULL), influence(0), heightinfluence(0), openList(NULL), whichList(NULL), 
		openX(NULL), openY(NULL), parentX(NULL), parentY(NULL), 
		Fcost(NULL), Gcost(NULL), Hcost(NULL), pathBank(NULL) {}
			
		int mapWidth, mapHeight; 
		int onClosedList;

		static const s32 notfinished; 
		static const s32 notStarted;
		static const s32 found;
		static const s32 nonexistent; 
		static const s32 walkable;
		static const s32 unwalkable;

		// needed arrays
		u8  **walkability, **influence;
		u16 **heightinfluence;

		int *openList;    //1 dimensional array holding ID# of open list items
		int **whichList;  //2 dimensional array used to record 

		// whether a cell is on the open list or on the closed list.
		int *openX;    //1d array stores the x location of an item on the open list
		int *openY;    //1d array stores the y location of an item on the open list
		int **parentX; //2d array to store parent of each cell (x)
		int **parentY; //2d array to store parent of each cell (y)
		int *Fcost;	  //1d array to store F cost of a cell on the open list
		int **Gcost;   //2d array to store G cost for each cell.
		int *Hcost;    //1d array to store H cost of a cell on the open list	
		int *pathBank; //stores the founded path array

		int pathLength;   //stores length of the found path for critter
		int pathLocation; //stores current position along the chosen path for critter	

		//Path reading variables
		int pathStatus, xPath, yPath;

		// Allocates memory for the pathfinder.
		void InitializePathfinder(int width, int height);

		// Frees memory used by the pathfinder.
		void DeinitializePathfinder();

		// Finds a path using A*
		int FindPath( int startingX, int startingY, int targetX, int targetY );

		void swapWalkabilityBuffers();

		void addInfluence(s32 x, s32 y, s32 infl);		

		bool isWalkable(s32 x, s32 y);
		bool isBusy(s32 x, s32 y);

		u8 getInfluence(s32 x, s32 y);	
		u16 getHeightInfluence(s32 x, s32 y);		
	}
	Pathfinder;

	s32 NoPathCounter;

	core::aabbox3df TransformedBBox, BBox;	
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CPathFinderAStarHPP