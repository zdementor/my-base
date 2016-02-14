//|-------------------------------------------------------------------------
//| File:        CQ2LevelMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CQ3LevelMeshHPP
#define CQ3LevelMeshHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IBSPLevelMesh.h>
#include <io/IReadFile.h>
#include <io/IFileSystem.h>
#include <scn/SMesh.h>
#include <scn/SAnimatedMesh.h>
#include <vid/IVideoDriver.h>
#include <scn/ILightGrid.h>
#include <scn/mesh_buffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SMeshBufferLightMap;
class CQ3Entity;
/*
//---------------------------------------------------------------------------

class CQ2LevelMesh : public IBSPLevelMesh
{
	friend class ILightGrid;

public:

    //! constructor
    CQ2LevelMesh();

    //! destructor
    virtual ~CQ2LevelMesh();	

    //! returns the amount of frames in milliseconds. 
	//! If the amount is 1, it is a static (=non animated) mesh.
    virtual s32 getOveralFramesCount();

	//! Returns the IMesh interface according to the current animation
    virtual IMesh* getMesh();

	//! Returns the IMesh interface according to the specified key frame
	virtual IMesh* getMesh( s32 frame);

    //! Returns the type of the animated mesh.
    virtual E_MESH_TYPE getAnimatedMeshType() const;

	//! rescale normals
	virtual void rescaleAnimatedMeshNormals(core::vector3df &newScale);

	//! calc bounding box
    virtual void recalculateBoundingBox();

	//! return Entities count	
	virtual s32 getEntitiesCount(E_BSP_ENTITY_TYPE type);

	//! return Entity by its type and index
	virtual IBSPEntity& getEntity(E_BSP_ENTITY_TYPE type, s32 idx);

    //! loads a level from a .bsp-File.
    virtual bool loadFromFile(io::IReadFile* file);

	virtual void freeUnusedData();

    //! saving a level to a .bsp-File
    virtual bool saveToFile(const c8* filename);	

	virtual vid::SLight getLightFromPos(core::vector3df _pos);

	virtual bool isLightGridOK();

	virtual void removePatches();

	virtual void collapseVertices();

	virtual void generateMaxImportScript(const c8* filename, f32 light_intens_mul, f32 light_radius_mul);

	virtual void rewriteLightMapsFrom(const c8* lightmaps_path);	

	//! loads the texture
	static vid::ITexture* getQ3Texture(const c8 *texname);

	//! loads the texture from filename with extention
	static vid::ITexture* getQ3TextureWithExt(const c8 *texname);

	static vid::ITexture* getQ3NormalMapTexture(const c8 *texname);

	//! returns white texture
	static vid::ITexture* getQ3WhiteImage();

	//! return true for caulk, hint and so on textures
	static bool isRadiantHintTexture(c8* texname);	

public:

	//! --------------------------------------------------------------------------------------------------
	//!					Unofficial Quake 3 BSP Format
	//!					Author:     Ben "Digiben" Humphrey 
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! --------------------------------------------------------------------------------------------------
	//! Introduction
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! This document was created as an aid to the Quake3 BSP tutorial series featured on
	//! www.GameTutorials.com.  The information is what I have found, and it's possibly that it's
	//! incorrect or just blatantly wrong.  I suggest you use this as a reference and a guide, not the end
	//! all file format doc for the Quake3 .bsp file format.  With that out of the way, let's load some
	//! sweet levels! 
	//! 
	//! The Quake3 level format, .bsp, stores most of the information about the level.  There are other
	//! files such as .shader, .arena and .aas, which store bot and texture shader information.  The .bsp
	//! file is stored in what is called a IBSP format.  That means that the length and offsets of different 
	//! sections in the file are stored in what's know as lumps.  The older version of Quake use this
	//! same lump format, but different information is stored in each version of Quake.  If you can read 
	//! in Quake 3 levels, it's not a lot of changes to write a Quake 2 level loader. 
	//! 
	//! If you don't know what BSP stands for yet, it means Binary Space Partition(ing).  You would 
	//! create a BSP tree.  That means that there is a parent node, and at most, 2 children attached to 
	//! each parent.  These children are called the front and back children.  I won't attempt to teach you 
	//! how to create or manage a BSP tree here, but there is a BSP FAQ that SGI put out floating 
	//! around the internet somewhere that has a ton of information.  Better yet, I suggest you take the 
	//! BSP class at www.GameInstitute.com.  I personally took this class and was quite satisfied.  It 
	//! teaches all you need to know about BSP trees. 

	//! --------------------------------------------------------------------------------------------------
	//! Lumps
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! Like we mentioned before, lumps hold the length in bytes and offset into the file for a given 
	//! section.  Below is an enum eLumps that holds all the lumps and their order in the file: 

    enum eLumps
    {
        kEntities = 0,     // Stores player/object positions, etc...
        kTextures,         // Stores texture information
        kPlanes,           // Stores the splitting planes
        kNodes,            // Stores the BSP nodes
        kLeafs,            // Stores the leafs of the nodes
        kLeafFaces,        // Stores the leaf's indices into the faces
        kLeafBrushes,      // Stores the leaf's indices into the brushes
        kModels,           // Stores the info of world models
        kBrushes,          // Stores the brushes info (for collision)
        kBrushSides,       // Stores the brush surfaces info
        kVertices,         // Stores the level vertices
        kMeshVerts,        // Stores the model vertices offsets
        kShaders,          // Stores the shader files (blending, anims..)
        kFaces,            // Stores the faces for the level
        kLightmaps,        // Stores the lightmaps for the level
        kLightVolumes,     // Stores extra world lighting information
        kVisData,          // Stores PVS and cluster info (visibility)  
		
        kMaxLumps          // A constant to store the number of lumps
    };

    //! Each on of these sections has a offset and a size in bytes that need to be read in.  In the next 
	//! sections we will examine the structures needed to read in each lump. 
	//! Here is a lump structure.  The offset is the position into the file that is the starting point of the 
	//! current section.  The length is the number of bytes that this lump stores. 

    struct tBSPLump
    {
        s32 offset;
        s32 length;
    }
	Lumps[kMaxLumps];

	//! Let's give an example of reading in the vertices (kVertices) for the level.  Once the lumps are 
	//! read in, to find the number of vertices in the level we do this: 
	//! 
	//! numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex); 
	//! 
	//! We index the lumps[] array with the kVertices constant, then divide that lumps length by the 
	//! size of the tBSPVertex structure in bytes, which we will define later on.  It just so happens it's 
	//! 44 bytes.  If the length is 3388, then 3388 / 44 = 77.  We now know there is 77 vertices in the 
	//! .bsp file.  We then need to position the file pointer to the lump's offset, and start reading in 77 
	//! tBSPVertex structures into our dynamically allocated vertex array.  I use fread() and fseek() for 
	//! the file manipulation.  This is of course, ONLY if you are not reading from the .zip file.  I am 
	//! strictly speaking of loading the .bsp file unzipped. 	  
	//! 		
	//! Now that we understand the basics of lumps, let's move on to the header structure, along with the 
	//! rest of the structures for each lump read in.

	//! --------------------------------------------------------------------------------------------------
	//! BSP Header
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The very first thing that needs to be read in for the .bsp file is the header.  The header contains a 
	//! 4 character ID, then an integer that holds the version. 

    struct tBSPHeader
    {
        s32 strID;    // This should always be 'IBSP'
        s32 version;  // This should be 0x2e for Quake 3 files
    }
	Header; 

	//! --------------------------------------------------------------------------------------------------
	//! Vertices
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! This structure stores the vertex information.  There is a position, texture and lightmap 
	//! coordinates, the vertex normal and color.  To calculate the number of vertices in the lump you 
	//! divide the length of the lump by the sizeof(tBSPVertex). 

    struct tBSPVertex
    {
        f32 vPosition[3];      // (x, y, z) position. 
        f32 vTextureCoord[2];  // (u, v) texture coordinate
        f32 vLightmapCoord[2]; // (u, v) lightmap coordinate
        f32 vNormal[3];        // (x, y, z) normal vector
        u8 color[4];           // RGBA color for the vertex 
    }
	*Vertices;
    
	s32 NumVertices;

	//! --------------------------------------------------------------------------------------------------
	//! Faces
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! This structure holds the face information for each polygon of the level.  It mostly holds indices 
	//! into all the vertex and texture arrays.  To calculate the number of faces in the lump you divide 
	//! the length of the lump by the sizeof(tBSPFace). 

    struct tBSPFace
    {
        s32 textureID;         // The index into the texture array 
        s32 effect;            // The index for the effects (or -1 = n/a) 
        s32 type;              // 1=polygon, 2=patch, 3=mesh, 4=billboard 
        s32 vertexIndex;       // The index into this face's first vertex 
        s32 numOfVerts;        // The number of vertices for this face 
        s32 meshVertIndex;     // The index into the first meshvertex 
        s32 numMeshVerts;      // The number of mesh vertices 
        s32 lightmapID;        // The texture index for the lightmap 
		s32	lMapX, lMapY;          // The face's lightmap corner in the image 	
		s32	lMapWidth, lMapHeight; // The size of the lightmap section 
        f32 lMapOrigin[3];     // The 3D origin of lightmap. 
        f32 lMapVecs[3][3];    // The 3D space for s, t and normal vectors. 
        s32 patchWidth;        // The bezier patch dimensions width. 
		s32 patchHeight;       // The bezier patch dimensions height. 
    }
	*Faces;

    s32 NumFaces;

	//! If the face type is 1 (normal polygons), the vertexIndex and numOfVerts can be used to index 
	//! into the vertex array to render triangle fans.
	//! 
	//! If the face type is 2 (bezier path), the vertexIndex and numOfVerts act as a 2D grid of control 
	//! points, where the grid dimensions are described by the size[2] array.  You can render the bezier 
	//! patches with just the vertices and not fill in the curve information, but it looks horrible and 
	//! blocky.   
	//! 
	//! The point of the curved surfaces are to be able to create a more defined surface, depending on the 
	//! specs of the computer that is running that application.  Some computers with horrible speed and 
	//! video cards would make the smallest amount of polygons from the curve, where as the fast 
	//! computers using Geforce cards could use the highest amount of polygons to form a perfect 
	//! curve.
	//! 
	//! If the face type is 3 (mesh vertices), the vertexIndex and numOfVerts also work the same as if 
	//! the type is 1
	//! 
	//! If the face type is 4, the vertexIndex is the position of the billboard.  The billboards are used for 
	//! light effects such as flares, etc... 
	
	//! --------------------------------------------------------------------------------------------------
	//! Textures
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The texture structure stores the name of the texture, along with some surface information which 
	//! are associated with the brush, brush sides and faces.  To calculate the number of textures in the 
	//! lump you divide it's  length by the sizeof(tBSPTexture). 

    struct tBSPTexture
    {
        c8 strName[64];   // The name of the texture w/o the extension 
        u32 flags;        // The surface flags (unknown) 
        u32 contents;     // The content flags (unknown)
    }
	*Textures;

    s32 NumTextures;

	//! --------------------------------------------------------------------------------------------------
	//! Lightmaps
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! Unlike the textures, the lightmaps are stored in the .bsp file as 128x128 RGB images.  Many 
	//! faces share the same lightmap with their own section of it stored in the lightmap UV 
	//! coordinates.  Once you read in the lightmaps, you will want to create textures from them.  To 
	//! calculate the number of lightmaps  in the lump you divide the length of the lump by the 
	//! sizeof(tBSPLightmap). 

    struct tBSPLightmap
    {
        u8 imageBits[128][128][3];   // The RGB data in a 128x128 image
    }
	*LightMaps;

    s32 NumLightMaps;

	//! --------------------------------------------------------------------------------------------------
	//! Nodes
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The node structure holds the nodes that make up the BSP tree.  The BSP is not used for 
	//! rendering so much in Quake3, but for collision detection.  The node holds the splitter plane 
	//! index, the front and back index, along with the bounding box for the node.  If the front or back 
	//! indices are negative, then it's an index into the leafs array.  Since negative numbers can't 
	//! constitute an array index, you need to use the ~ operator or -(index  + 1) to find the correct 
	//! index.  This is because 0 is the starting index.  To calculate the number of nodes in the lump you 
	//! divide the length of the lump by the sizeof(tBSPNode). 

    struct tBSPNode
    {
        s32 plane;      // The index into the planes array 
        s32 front;      // The child index for the front node 
        s32 back;       // The child index for the back node 
        s32 mins[3];    // The bounding box min position. 
        s32 maxs[3];    // The bounding box max position. 
    }
	*Nodes;

    s32 NumNodes; 

	//! --------------------------------------------------------------------------------------------------
	//! Leafs
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The leafs, like the faces, are a very important part of the BSP information.  They store the  
	//! visibility cluster, the area portal, the leaf bounding box, the index into the faces, the number of 
	//! leaf faces, the index into the brushes for collision, and finally,  the number of leaf brushes.  To 
	//! calculate the number of leafs in the lump you divide the length of the lump by the 
	//! sizeof(tBSPLeaf). 

    struct tBSPLeaf
    {
        s32 cluster;           // The visibility cluster 
        s32 area;              // The area portal 
        s32 mins[3];           // The bounding box min position 
        s32 maxs[3];           // The bounding box max position 
        s32 leafface;          // The first index (in the Leaf Faces array) for into the face array 
        s32 numOfLeafFaces;    // The number of faces for this leaf 
        s32 leafBrush;         // The first index (in the Leaf Brushes array) for into the brushes 
        s32 numOfLeafBrushes;  // The number of brushes for this leaf 
    }
	*Leafs;

    s32 NumLeafs; 

	//! --------------------------------------------------------------------------------------------------
	//! Leaf Faces
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The leaf faces are used to index into tBSPFaces array.  You might at first think this is strange to 
	//! have the tBSPLeaf structure have an index into the pLeafFaces array, which in turn is just an 
	//! index into the tBSPFaces array.  This is because it's set up to start with a starting point (leafface) 
	//! and a count to go from there for each face (numOfLeafFaces).  The faces array is not 
	//! contiguous (in a row) according to each leaf.  That is where the leaf faces array comes into play.  
	//! It's kinda like the same concept of model loaders where they store the vertices and then have 
	//! faces that store the indices into the vertex array for that face.  To calculate the number of leaf 
	//! faces in the lump you divide the length of the lump by the sizeof(int).  
	
	s32 *LeafFaces;           // The index into the face array 

	s32 LeafFacesSize;

	//! --------------------------------------------------------------------------------------------------
    //! Planes
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The plane structure stores the normal to the plane and it's distance to the origin.  We use this as 
	//! the splitter plane for the BSP tree.  When rendering or testing collision, we can test the camera 
	//! position against the planes to see which plane we are in front of.  To calculate the number of 
	//! planes in the lump you divide the length of the lump by the sizeof(tBSPPlane). 

    struct tBSPPlane
    {
        f32 vNormal[3];     // Plane normal. 
        f32 d;              // The plane distance from origin 
    }
	*Planes;

    s32 NumPlanes;

	//! --------------------------------------------------------------------------------------------------
	//! Visibility Data
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The visibility information is comprised of a bunch of bitsets that store a bit for every cluster.  
	//! This is because the information is so massive that this way makes it faster to access and a smaller 
	//! memory footprint.  There is only one instance of this structure, but you calculate how much 
	//! needs to be read in bytes by either: numOfClusters * bytesPerCluster, or minus the size of 2 
	//! integers from this lumps length.  The pBitsets is then dynamically allocated and stores the 
	//! calculate bytes.  This is probably one of the most confusing parts about the .bsp file format, the 
	//! visibilty.  I will try and explain the important parts of it and give some code. 

    struct tBSPVisData
    {
        s32 numOfClusters;   // The number of clusters
        s32 bytesPerCluster; // Bytes (8 bits) in the cluster's bitset
        c8 *pBitsets;        // Array of bytes holding the cluster vis.
    }
	VisData; 

	//! To demonstrate what a cluster is and what we need to do with it, let's start with a simple 
	//! example.  When rendering, first we want to find which leaf we are in.  Once again, a leaf is an 
	//! end node of the BSP tree that holds a bunch of information about the faces, brushes and the 
	//! cluster it's in.  Once that leaf is founding by checking the camera position against all of the 
	//! planes, we then want to go through all of the leafs and check if their cluster is visible from the 
	//! current cluster we are in.  If it is, that means that we need to check if that leaf's bounding box is 
	//! inside of our frustum before we draw it.   
	//! 
	//! Say we have cluster A, B and C.  Each cluster is stored as a bit in bitset.  A bitset is just a huge 
	//! list of binary numbers next to each other.  Each cluster has their own list of bits that store a 1 or a 
	//! 0 to tell if the cluster in that bit is visible (1) or not visible (0).  Since there is most likely more 
	//! than 32 clusters in a level, you can't just use an integer (32-bits) to store the bits for all the 
	//! clusters.  This is why there are many bytes assigned to each cluster.  So, here is how it works: 
	//! 
	//! With cluster A, B and C, here is how they would be represented in binary (a bitset): 
	//! ABC
	//! 000 
	//! 
	//! Each 0 represents a slot that is assigned to a cluster.  Let's assume that: 
	//! - Cluster A can see cluster B and not C
	//! - Cluster B can see cluster A and C
	//! - Cluster C can see cluster B and not A 
	//! 
	//! Below is a representation of each one of their bitsets: 
	//! - A     110
	//! - B     111
	//! - C     011 
	//! 
	//! Does that make sense?  Notice for A there is a 1 in the first slot which means it can see itself, and 
	//! also in the second slot which means it can see cluster B.  The last slot is a 0, which tells us that 
	//! cluster A can not see what's in cluster C because some walls or whatever are blocking it.  This is 
	//! where the good spatial partition speed comes in.  If you are in the very bottom corner of the level 
	//! in a small little room, you just cut out probably 95% percent of the polygons that need to be 
	//! rendered because you can only most likely see the cluster that is right outside of that room. 
	//! 
	//! To test if a cluster is visible from another cluster, there is obviously going to have to be some bit-
	//! shifting and other binary math involved.  The basic algorithm to test a cluster against another 
	//! cluster is as follows: 
	//! int visible = pBitsets[currentCluster*bytesPerCluster + ( testCluster/8 )] & (1 << (testCluster & 7))  
	//! 
	//! If the result of visible isn't 0, then the testCluster can be seen from the currentCluster.  We 
	//! divide and % (mod) by 8 because we are using bytes which are 8 bits.  Basically, the first part is 
	//! indexing into the array of clusters to find the correct bitset, then we do a binary & (and) with the 
	//! cluster we are testing to get the result. 
	//! 
	//! Here is some basic code to do a cluster to cluster test: 
	//! 
	//! inline int IsClusterVisible(tBSPVisData *pPVS, int current, int test)
	//! {
	//!		if(!pPVS->pBitsets || current < 0) return 1;
	//! 
	//! 	byte visSet = pPVS->pBitsets[(current*pPVS->bytesPerCluster) + (test/8)];
	//! 	int result = visSet & (1 << (test & 7));
	//! 
	//!		return ( result );
	//!	} 

	//! --------------------------------------------------------------------------------------------------
	//! Entities
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The entity lump just stores a huge string with each line delimited by '\n'.  Some types of things 
	//! stored in this string is the deathmatch positions for the players in the beginning, weapon 
	//! positions, sky box information, light positions, etc...  I suggest you save it off to a file so that you 
	//! can get a good idea on how to parse it.  Be careful when writing a parser, things aren't always 
	//! saved in the same order.  Player positions usually store the 3D position on the map, along with 
	//! the orientation described by a rotation angle.  To find the length of the entity string to read in, 
	//! just divide the entity lump's length by sizeof(char). 

	c8 *StrEntities;              // This stores a huge string of all the entities in the level 

	s32 EntitiesSize;

	//! --------------------------------------------------------------------------------------------------
    //! Brushes
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The brushes store information about a convex volume, which are defined by the brush sides.  
	//! Brushes are used for collision detection.  This allows the level editor to decide what is collidable 
	//! and what can be walked through, such as trees, bushes or certain corners.

    struct tBSPBrush 
    {
        s32 brushSide;           // The starting brush side for the brush 
        s32 numOfBrushSides;     // Number of brush sides for the brush
        s32 textureID;           // The texture index for the brush
    }
	*Brushes;

    s32 NumBrushes; 

	//! --------------------------------------------------------------------------------------------------
	//! Leaf Brushes
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! Like the leaf faces, leaf brushes are used to index into the tBSPBrush array.  Once again, 
	//! brushes are used for collision detection.  To calculate the number of leaf brushes in the lump you 
	//! divide the length of the lump by the sizeof(int).  

	s32 *LeafBrushes;           // The index into the brush array 

	s32 LeafBrushesSize;

	//! --------------------------------------------------------------------------------------------------
    //! Brush Sides
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The brush sides lump stores information about the brush bounding surface.  To calculate the 
	//! number of brush sides, just divide the lumps length by sizeof(tBSPBrushSides). 

    struct tBSPBrushSide 
    {
        s32 plane;              // The plane index
        s32 textureID;          // The texture index
    }
	*BrushSides; 

	s32 NumBrushSides;

	//! --------------------------------------------------------------------------------------------------
    //! Models
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The model structure stores the face and brush information, along with the bounding box of the 
	//! object.  These objects can be movable such as doors, platforms, etc...  To calculate the number of 
	//! models in this lump, just divide the length of the lump by sizeof(tBSPModel). 

    struct tBSPModel 
    {
        f32 min[3];           // The min position for the bounding box
        f32 max[3];           // The max position for the bounding box. 
        s32 faceIndex;        // The first face index in the model 
        s32 numOfFaces;       // The number of faces in the model 
        s32 brushIndex;       // The first brush index in the model 
        s32 numOfBrushes;     // The number brushes for the model
    }
	*Models;

	s32 NumModels;

    SMesh *Mesh;              // mesh for holding Model[0] geometry
	SAnimatedMesh **AMeshes;  // meshes for holding all other models geom (NumModels-1 count)	

	//! --------------------------------------------------------------------------------------------------
	//! Mesh Vertices
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! This stores a list of vertex offsets that are used to create a triangle mesh.  To calculate the 
	//! number of mesh vertices in the lump you divide the length of the lump by the sizeof(int).  
	
	s32 *MeshVerts;           // The vertex offsets for a mesh 

    s32 NumMeshVerts;

	//! --------------------------------------------------------------------------------------------------
    //! Shaders
	//! --------------------------------------------------------------------------------------------------
	//! 
	//! The shader structure basically gives you the file name, for a *.shader file.  The .shader file then 
	//! stores all of the information about blending, animating and such.  The shader files can be found 
	//! usually in the scripts\ folder, assuming that there are any textures in the level that use shaders of 
	//! course.  To calculate the number of shaders for this lump, just divide the length of the lump by 
	//! sizeof(tBSPShader). 

    struct tBSPShader
    {
        c8 strName[64];     // The name of the shader file 
        s32 brushIndex;     // The brush index for this shader 
        s32 unknown;        // This is 99% of the time 5
    }
	*Shaders; 

	s32 NumShaders;

	//! --------------------------------------------------------------------------------------------------
	//! Light Volumes
	//! --------------------------------------------------------------------------------------------------
	//! 	
	//! Not everything in Quake3 is lit by lightmaps.  There are other lights that have their own special 
	//! properties.  I believe you can get the rest of the light information from the entities lump.  To 
	//! calculate the number of lights in the lump, just divide the length of the lump by 
	//! sizeof(SLightVolume).
	//! 
  	//! struct SLightVolume
	//! {
	//! 	u8 ambient[3];     // This is the ambient color
	//! 	u8 directional[3]; // This is the directional color in RGB
	//! 	u8 direction[2];   // The direction of the light: [phi,theta] 
	//! }; 

	SLightVolume* LightVols;
	
    s32 NumLightVols;

	//! The light data makes up a 3-Dimensional grid with dimensions of: 
	//! 
	//! x = floor(models[0].max[0] / 64)  - ceil(models[0].min[0] / 64)  + 1
	//! y = floor(models[0].max[1] / 64)  - ceil(models[0].min[1] / 64)  + 1
	//! z = floor(models[0].max[2] / 128) - ceil(models[0].min[2] / 128) + 1 

private:

    void loadTextures   (tBSPLump* l, io::IReadFile* file);     // Load the textures
    void loadLightmaps  (tBSPLump* l, io::IReadFile* file);     // Load the lightmaps
    void loadVerts      (tBSPLump* l, io::IReadFile* file);     // Load the vertices
    void loadFaces      (tBSPLump* l, io::IReadFile* file);     // Load the faces
    void loadPlanes     (tBSPLump* l, io::IReadFile* file);     // Load the Planes of the BSP
    void loadNodes      (tBSPLump* l, io::IReadFile* file);     // load the Nodes of the BSP
    void loadLeafs      (tBSPLump* l, io::IReadFile* file);     // load the Leafs of the BSP
    void loadLeafFaces  (tBSPLump* l, io::IReadFile* file);     // load the Faces of the Leafs of the BSP
    void loadLeafBrushes(tBSPLump* l, io::IReadFile* file);     // load the brushes of the leaf
	void loadVisData    (tBSPLump* l, io::IReadFile* file);     // load the visibility data of the clusters
    void loadEntities   (tBSPLump* l, io::IReadFile* file);     // load the entities
    void loadModels     (tBSPLump* l, io::IReadFile* file);     // load the models
    void loadMeshVerts  (tBSPLump* l, io::IReadFile* file);     // load the mesh vertices
    void loadBrushes    (tBSPLump* l, io::IReadFile* file);     // load the brushes of the BSP
    void loadBrushSides (tBSPLump* l, io::IReadFile* file);     // load the brushsides of the BSP    
	void loadLightVols  (tBSPLump* l, io::IReadFile* file);     // Load the light volumes
	void loadShaders    (tBSPLump* l, io::IReadFile* file); 
	
	void loadData(io::IReadFile* file);
	void freeData();
    void constructMesh();
	void parseEntities();
	void initLightGrid();	
	
	struct SModelEntityEntry
	{
		SModelEntityEntry()
			: Type(EBSPET_UNKNOWN), Index(-1) {}

		SModelEntityEntry(E_BSP_ENTITY_TYPE type, s32 idx)
			: Type(type), Index(idx) {}

		E_BSP_ENTITY_TYPE Type;
		s32 Index;
	};

	struct SModelMeshBuffer
	{
		SModelMeshBuffer() : MeshBuffer(0) {}

		IMeshBuffer     *MeshBuffer;
		core::array<s32> BSPFaces;
	};
	
	core::array<CQ3Entity> Q3Entities[E_BSP_ENTITY_TYPE_COUNT];
	
    core::stringc LevelName, LevelPath;
	SLightGridData LightGridData;
	core::aabbox3df BBox, LightGridBBox;	

	void printBspInfo(eLumps ltype);

	core::array<SModelEntityEntry> ModelEntities;

	bool LightGridOK;
};
*/
//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif