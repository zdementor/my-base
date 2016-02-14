//|-------------------------------------------------------------------------
//| File:        C3DSMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef C3DSMeshHPP
#define C3DSMeshHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IAnimatedMesh.h>
#include <io/IReadFile.h>
#include <vid/S3DVertex.h>
#include <scn/SMesh.h>

#include "3DSHelper.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class C3DSMesh : public IAnimatedMesh
{
public:

    C3DSMesh();
    virtual ~C3DSMesh();

    bool loadFromFile(io::IReadFile* file);

	//////////////////////////////////////
	//      IAnimatedMesh
	//////////////////////////////////////

    virtual IMesh* getMesh();
	virtual IMesh* getMesh(f32 frame);

    virtual void recalculateBoundingBox();

    virtual E_MESH_TYPE getAnimatedMeshType() const
	{ return EMT_3DS; }

	virtual void rescaleAnimatedMeshNormals(const core::vector3df &newScale);

	virtual u32 getOveralFramesCount()
	{ return 1; }

private:

	bool readChunk(io::IReadFile* file, ChunkData* parent);
	bool readMaterialChunk(io::IReadFile* file, ChunkData* parent);
	bool readFrameChunk(io::IReadFile* file, ChunkData* parent);
	bool readTrackChunk(io::IReadFile* file, ChunkData& data,
		IMeshBuffer* mb, const core::vector3df& pivot);
	bool readObjectChunk(io::IReadFile* file, ChunkData* parent);
	bool readPercentageChunk(io::IReadFile* file, ChunkData* chunk, f32& percentage);
	bool readColorChunk(io::IReadFile* file, ChunkData* chunk, img::SColor& out);

	void readChunkData(io::IReadFile* file, ChunkData& data);
	void readString(io::IReadFile* file, ChunkData& data, core::stringc& out);
	void readVertices(io::IReadFile* file, ChunkData& data);
	void readIndices(io::IReadFile* file, ChunkData& data);
	void readMaterialGroup(io::IReadFile* file, ChunkData& data);
	void readTextureCoords(io::IReadFile* file, ChunkData& data);

	template <class TVert>
	void composeObject(io::IReadFile* file, const core::stringc& name);
	void loadMaterials(io::IReadFile* file, vid::E_VERTEX_TYPE vtype);
	void cleanUp();

	SMesh Mesh;

	f32* Vertices;
	u16* Indices;
	u32* SmoothingGroups;
	core::array<u16> TempIndices;
	f32* TCoords;
	u16 CountVertices;
	u16 CountFaces; // = CountIndices/4
	u16 CountTCoords;

	struct SCurrentMaterial
	{
		void clear() {
			Material = vid::SMaterial();
			Name = "";
			Filename[0] = "";
			Filename[1] = "";
			Filename[2] = "";
			Filename[3] = "";
			Filename[4] = "";
			Strength[0] = 0.f;
			Strength[1] = 0.f;
			Strength[2] = 0.f;
			Strength[3] = 0.f;
			Strength[4] = 0.f;
		}

		vid::SMaterial Material;
		core::stringc Name;
		core::stringc Filename[5];
		f32 Strength[5];
	};

	struct SMaterialGroup
	{
		SMaterialGroup() : faceCount(0), faces(0) {};

		SMaterialGroup(const SMaterialGroup& o)
		{
			*this = o;
		}

		~SMaterialGroup()
		{
			clear();
		}

		void clear()
		{
			delete [] faces;
			faces = 0;
			faceCount = 0;
		}

		void operator =(const SMaterialGroup& o)
		{
			MaterialName = o.MaterialName;
			faceCount = o.faceCount;
			faces = new u16[faceCount];
			for (u32 i=0; i<faceCount; ++i)
				faces[i] = o.faces[i];
		}

		core::stringc MaterialName;
		u16 faceCount;
		u16* faces;
	};

	core::array<SMaterialGroup> MaterialGroups;
	SCurrentMaterial CurrentMaterial;
	core::array<SCurrentMaterial> Materials;
	core::array<core::stringc> MeshBufferNames;
	core::matrix4 TransformationMatrix;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

