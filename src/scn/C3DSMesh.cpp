//|-------------------------------------------------------------------------
//| File:        C3DSMesh.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "C3DSMesh.h"

#include <io/IReadFile.h>
#include <img/SColor.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <vid/IRenderBuffer.h>
#include <vid/IVideoDriver.h>
#include <dev/IDevice.h>
#include <scn/mesh_buffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

C3DSMesh::C3DSMesh():
	Vertices(0), Indices(0), SmoothingGroups(0), TCoords(0),
	CountVertices(0), CountFaces(0), CountTCoords(0)
{
#if MY_DEBUG_MODE 
	C3DSMesh::setClassName("C3DSMesh IAnimatedMesh");
#endif  
}

//---------------------------------------------------------------------------

C3DSMesh::~C3DSMesh()
{
	cleanUp();
}

//---------------------------------------------------------------------------

bool C3DSMesh::readPercentageChunk(
	io::IReadFile* file, ChunkData* chunk, f32& percentage)
{
	ChunkData data;
	readChunkData(file, data);

	short intpercentage;
	float fpercentage;

	switch(data.header.id)
	{
	case C3DS_PERCENTAGE_I:
		{
			// read short
			file->read(&intpercentage, 2);
			percentage=intpercentage/100.0f;
			data.read += 2;
		}
		break;
	case C3DS_PERCENTAGE_F:
		{
			// read float
			file->read(&fpercentage, sizeof(float));
			data.read += sizeof(float);
			percentage = (f32)fpercentage;
		}
		break;
	default:
		{
			// unknown percentage chunk
			LOGGER.logWarn("Unknown percentage chunk in 3Ds file.");
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}
	}

	chunk->read += data.read;

	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readColorChunk(
	io::IReadFile* file, ChunkData* chunk,
	img::SColor& out)
{
	ChunkData data;
	readChunkData(file, data);

	u8 c[3];
	f32 cf[3];

	switch(data.header.id)
	{
	case C3DS_COL_TRU:
	case C3DS_COL_LIN_24:
		{
			// read 8 bit data
			file->read(c, sizeof(c));
			out.set(255, c[0], c[1], c[2]);
			data.read += sizeof(c);
		}
		break;
	case C3DS_COL_RGB:
	case C3DS_COL_LIN_F:
		{
			// read float data
			file->read(cf, sizeof(cf));
			out.set(255, (s32)(cf[0]*255.0f), (s32)(cf[1]*255.0f), (s32)(cf[2]*255.0f));
			data.read += sizeof(cf);
		}
		break;
	default:
		{
			// unknown color chunk size
			LOGGER.logWarn("Unknown size of color chunk in 3Ds file.");
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}
	}

	chunk->read += data.read;

	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readMaterialChunk(io::IReadFile* file, ChunkData* parent)
{
	u16 matSection=0;

	vid::SRenderPass &pass = CurrentMaterial.Material.getPass(0);

	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_MATNAME:
			{
				c8* c = new c8[data.header.length - data.read];
				file->read(c, data.header.length - data.read);

				if (strlen(c))
					CurrentMaterial.Name = c;

				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_MATAMBIENT:
			{
				img::SColor color;
				readColorChunk(file, &data, color);
				pass.setAmbientColor(color);
			}
			break;
		case C3DS_MATDIFFUSE:
			{
				img::SColor color;
				readColorChunk(file, &data, color);
				pass.setDiffuseColor(color);
			}
			break;
		case C3DS_MATSPECULAR:
			{
				img::SColor color;
				readColorChunk(file, &data, color);
				pass.setSpecularColor(color);
			}
			break;
		case C3DS_MATSHININESS:
			{
				f32 shi;
				readPercentageChunk(file, &data, shi);
				shi = (1.f-shi)*128.f;
				pass.setShininess(shi);
			}
			break;
		case C3DS_TRANSPARENCY:
			{
				f32 percentage;
				readPercentageChunk(file, &data, percentage);
				if (percentage>0.0f)
				{
					u32 alpha = 255.0f * (1.0f - percentage);

					pass.setAlphaGen(vid::EAGT_CUSTOM);

					img::SColor dc = pass.getDiffuseColor();
					img::SColor ac = pass.getAmbientColor();
					img::SColor sc = pass.getSpecularColor();
					img::SColor ec = pass.getEmissiveColor();

					dc.setAlpha(alpha);
					ac.setAlpha(alpha);
					sc.setAlpha(alpha);
					ec.setAlpha(alpha);

					pass.setDiffuseColor(dc);
					pass.setAmbientColor(ac);
					pass.setSpecularColor(sc);
					pass.setEmissiveColor(ec);

					pass.setFlag(vid::EMF_BLENDING, true);
					pass.setBlendFuncs(
						vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);
				}
			}
			break;
		case C3DS_WIRE:
			break;
		case C3DS_TWO_SIDE:
			pass.setFlag(vid::EMF_BACK_FACE_CULLING, false);
			break;
		case C3DS_SHADING:
			{
				s16 flags;
				file->read(&flags, 2);
				switch (flags)
				{
					case 0:
						break;
					case 1:
						pass.setFlag(vid::EMF_GOURAUD_SHADING, false);
						break;
					case 2:
						pass.setFlag(vid::EMF_GOURAUD_SHADING, true);
						break;
					default:
						// phong and metal missing
						break;
				}
				data.read += data.header.length - data.read;
			}
			break;
		case C3DS_MATTEXMAP:
		case C3DS_MATSPECMAP:
		case C3DS_MATOPACMAP:
		case C3DS_MATREFLMAP:
		case C3DS_MATBUMPMAP:
			{
				matSection=data.header.id;
				// Should contain a percentage chunk, but does
				// not always have it
				s16 testval;
				const long pos = file->getPos();
				file->read(&testval, 2);
				file->seek(pos, false);
				if ((testval == C3DS_PERCENTAGE_I) ||
					(testval == C3DS_PERCENTAGE_F))
				switch (matSection)
				{
				case C3DS_MATTEXMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[0]);
				break;
				case C3DS_MATSPECMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[1]);
				break;
				case C3DS_MATOPACMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[2]);
				break;
				case C3DS_MATBUMPMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[4]);
				break;
				}
			}
			break;
		case C3DS_MATMAPFILE:
			{
				// read texture file name
				c8* c = new c8[data.header.length - data.read];
				file->read(c, data.header.length - data.read);
				switch (matSection)
				{
				case C3DS_MATTEXMAP:
					CurrentMaterial.Filename[0] = c;
				break;
				case C3DS_MATSPECMAP:
					CurrentMaterial.Filename[1] = c;
				break;
				case C3DS_MATOPACMAP:
					CurrentMaterial.Filename[2] = c;
				break;
				case C3DS_MATREFLMAP:
					CurrentMaterial.Filename[3] = c;
				break;
				case C3DS_MATBUMPMAP:
					CurrentMaterial.Filename[4] = c;
				break;
				}
				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_MAT_TEXTILING:
			{
				s16 flags;
				file->read(&flags, 2);
				data.read += 2;
			}
			break;
		case C3DS_MAT_USCALE:
		case C3DS_MAT_VSCALE:
		case C3DS_MAT_UOFFSET:
		case C3DS_MAT_VOFFSET:
			{
				f32 value;
				file->read(&value, 4);
				u32 i=0;
				if (matSection != C3DS_MATTEXMAP)
					i=1;
				u32 j=0,k=0;
				if (data.header.id == C3DS_MAT_VSCALE)
				{
					j=1;
					k=1;
				}
				else if (data.header.id == C3DS_MAT_UOFFSET)
				{
					j=2;
					k=0;
				}
				else if (data.header.id == C3DS_MAT_VOFFSET)
				{
					j=2;
					k=1;
				}
				core::matrix4 mat = pass.Layers[i].getTexMatrix();
				mat(j, k) = value;
				pass.Layers[i].setTexMatrix(mat);

				data.read += 4;
			}
			break;
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	Materials.push_back(CurrentMaterial);
	CurrentMaterial.clear();

	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readTrackChunk(
	io::IReadFile* file, ChunkData& data,
	IMeshBuffer* mb, const core::vector3df& pivot)
{
	u16 flags;
	u32 flags2;
	// Track flags
	file->read(&flags, 2);
	file->read(&flags2, 4);
	file->read(&flags2, 4);
	// Num keys
	file->read(&flags2, 4);
	file->read(&flags2, 4);
	// TCB flags
	file->read(&flags, 2);
	data.read += 20;

	f32 angle=0.0f;
	if (data.header.id== C3DS_ROT_TRACK_TAG)
	{
		// Angle
		file->read(&angle, sizeof(f32));
		data.read += sizeof(f32);
	}
	core::vector3df vec;
	file->read(&vec.X, sizeof(f32));
	file->read(&vec.Y, sizeof(f32));
	file->read(&vec.Z, sizeof(f32));
	data.read += 12;
	vec-=pivot;

	// apply transformation to mesh buffer
	if (false)//mb)
	{
		if (data.header.id==C3DS_POS_TRACK_TAG)
		{
			for (s32 i=0; i < mb->getVertexCount(); ++i)
				mb->getVertex(i).Pos += vec;
		}
		else if (data.header.id==C3DS_ROT_TRACK_TAG)
		{
			//TODO
		}
		else if (data.header.id==C3DS_SCL_TRACK_TAG)
		{
			//TODO
		}
	}
	// skip further frames
	file->seek(data.header.length - data.read, true);
	data.read += data.header.length - data.read;
	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readFrameChunk(io::IReadFile* file, ChunkData* parent)
{
	ChunkData data;

	//KF_HDR is always at the beginning
	readChunkData(file, data);
	if (data.header.id != C3DS_KF_HDR)
		return false;
	else
	{
		u16 version;
		file->read(&version, 2);
		core::stringc name;
		readString(file, data, name);
		u32 flags;
		file->read(&flags, 4);

		data.read += 4;
		parent->read += data.read;
	}
	data.read=0;

	IMeshBuffer* mb=0;
	core::vector3df pivot,bboxCenter;
	while(parent->read < parent->header.length)
	{
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_OBJECT_TAG:
			{
				mb=0;
				pivot.set(0.0f, 0.0f, 0.0f);
			}
			break;
		case C3DS_KF_SEG:
			{
				u32 flags;
				file->read(&flags, 4);
				file->read(&flags, 4);
				data.read += 8;
			}
			break;
		case C3DS_KF_NODE_HDR:
			{
				s16 flags;
				c8* c = new c8[data.header.length - data.read-6];
				file->read(c, data.header.length - data.read-6);

				// search mesh buffer to apply these transformations to
				for (u32 i=0; i<MeshBufferNames.size(); ++i)
				{
					if (MeshBufferNames[i]==c)
					{
						mb = Mesh.getMeshBuffer(i);
						break;
					}
				}

				file->read(&flags, 2);
				file->read(&flags, 2);
				file->read(&flags, 2);
				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_KF_CURTIME:
			{
				u32 flags;
				file->read(&flags, 4);
				data.read += 4;
			}
			break;
		case C3DS_NODE_ID:
			{
				u16 flags;
				file->read(&flags, 2);
				data.read += 2;
			}
			break;
		case C3DS_PIVOTPOINT:
			{
				file->read(&pivot.X, sizeof(f32));
				file->read(&pivot.Y, sizeof(f32));
				file->read(&pivot.Z, sizeof(f32));
				data.read += 12;
			}
			break;
		case C3DS_BOUNDBOX:
			{
				core::aabbox3df bbox;
				// abuse bboxCenter as temporary variable
				file->read(&bboxCenter.X, sizeof(f32));
				file->read(&bboxCenter.Y, sizeof(f32));
				file->read(&bboxCenter.Z, sizeof(f32));
				bbox.reset(bboxCenter);
				file->read(&bboxCenter.X, sizeof(f32));
				file->read(&bboxCenter.Y, sizeof(f32));
				file->read(&bboxCenter.Z, sizeof(f32));
				bbox.addInternalPoint(bboxCenter);
				bboxCenter=bbox.getCenter();
				data.read += 24;
			}
			break;
		case C3DS_MORPH_SMOOTH:
			{
				f32 flag;
				file->read(&flag, 4);
				data.read += 4;
			}
			break;
		case C3DS_POS_TRACK_TAG:
		case C3DS_ROT_TRACK_TAG:
		case C3DS_SCL_TRACK_TAG:
			readTrackChunk(file, data, mb, bboxCenter-pivot);
			break;
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
		data.read=0;
	}

	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readObjectChunk(io::IReadFile* file, ChunkData* parent)
{
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_OBJTRIMESH:
			readObjectChunk(file, &data);
			break;

		case C3DS_TRIVERT:
			readVertices(file, data);
			break;

		case C3DS_POINTFLAGARRAY:
			{
				u16 numVertex, flags;
				file->read(&numVertex, sizeof(u16));
				for (u16 i=0; i<numVertex; ++i)
				{
					file->read(&flags, sizeof(u16));
				}
				data.read += (numVertex+1)*sizeof(u16);
			}
			break;

		case C3DS_TRIFACE:
			readIndices(file, data);
			readObjectChunk(file, &data); // read smooth and material groups
			break;

		case C3DS_TRIFACEMAT:
			readMaterialGroup(file, data);
			break;

		case C3DS_TRIUV: // getting texture coordinates
			readTextureCoords(file, data);
			break;

		case C3DS_TRIMATRIX:
			{
				f32 mat[4][3];
				file->read(&mat, 12*sizeof(f32));
				TransformationMatrix.makeIdentity();
				for (int i=0; i<4; ++i)
				{
					for (int j=0; j<3; ++j)
					{
						TransformationMatrix(i,j)=mat[i][j];
					}
				}
				data.read += 12*sizeof(f32);
			}
			break;
		case C3DS_MESHCOLOR:
			{
				u8 flag;
				file->read(&flag, sizeof(u8));
				++data.read;
			}
			break;
		case C3DS_TRISMOOTH: // TODO
			{
				SmoothingGroups = new u32[CountFaces];
				file->read(SmoothingGroups, CountFaces*sizeof(u32));
				data.read += CountFaces*sizeof(u32);
			}
			break;

		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	return true;
}

//---------------------------------------------------------------------------

bool C3DSMesh::readChunk(io::IReadFile* file, ChunkData* parent)
{
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_VERSION:
			{
				u16 version;
				file->read(&version, sizeof(u16));
				file->seek(data.header.length - data.read - 2, true);
				data.read += data.header.length - data.read;
				if (version != 0x03)
					LOGGER.logErr("3ds file version is other than 3.");
			}
			break;
		case C3DS_EDIT_MATERIAL:
			readMaterialChunk(file, &data);
			break;
		case C3DS_KEYF3DS:
			readFrameChunk(file, &data);
			break;
		case C3DS_EDIT3DS:
			break;
		case C3DS_MESHVERSION:
		case 0x01:
			{
				u32 version;
				file->read(&version, sizeof(u32));
				data.read += sizeof(u32);
			}
			break;
		case C3DS_EDIT_OBJECT:
			{
				core::stringc name;
				readString(file, data, name);
				readObjectChunk(file, &data);
				if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
					composeObject<vid::S3DVertex1TCoordsTBN>(file, name);
				else
					composeObject<vid::S3DVertex1TCoords>(file, name);
			}
			break;

		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}
		parent->read += data.read;
	}

	return true;
}

//---------------------------------------------------------------------------

void C3DSMesh::loadMaterials(io::IReadFile* file, vid::E_VERTEX_TYPE vtype)
{
	// create a mesh buffer for every material
	core::stringc modelFilename = file->getFileName();

	if (Materials.empty())
		LOGGER.logInfo("No materials found in 3ds file.");

	MeshBufferNames.reallocate(Materials.size());
	for (u32 i=0; i<Materials.size(); ++i)
	{
		MeshBufferNames.push_back("");
		IMeshBuffer* m = mesh_buffer_factory::create(vtype);
		Mesh.addMeshBuffer(m);

		m->getMaterial() = Materials[i].Material;
		if (Materials[i].Filename[0].size())
		{
			LOGGER.logWarn("Could not load a texture for entry in 3ds file",
				Materials[i].Filename[0].c_str());
		}

		if (Materials[i].Filename[1].size())
		{
			LOGGER.logWarn("Could not load a texture for entry in 3ds file",
				Materials[i].Filename[1].c_str());
		}

		if (Materials[i].Filename[2].size())
		{
			LOGGER.logWarn("Could not load a texture for entry in 3ds file",
				Materials[i].Filename[2].c_str());
		}

		if (Materials[i].Filename[3].size())
		{
			LOGGER.logWarn("Could not load a texture for entry in 3ds file",
				Materials[i].Filename[3].c_str());
		}

		if (Materials[i].Filename[4].size())
		{
			LOGGER.logWarn("Could not load a texture for entry in 3ds file",
				Materials[i].Filename[4].c_str());
		}

		m->drop();
	}
}

//---------------------------------------------------------------------------

template <class TVert>
void C3DSMesh::composeObject(io::IReadFile* file, const core::stringc& name)
{
	if (Mesh.getMeshBuffersCount() != Materials.size())
		loadMaterials(file, TVert::Type);

	if (MaterialGroups.empty())
	{
		// no material group, so add all
		SMaterialGroup group;
		group.faceCount = CountFaces;
		group.faces = new u16[group.faceCount];
		for (u32 i=0; i<group.faceCount; ++i)
			group.faces[i] = i;
		MaterialGroups.push_back(group);

		// if we've got no material, add one without a texture
		if (Materials.empty())
		{
			SCurrentMaterial m;
			Materials.push_back(m);
			IMeshBuffer* mb = mesh_buffer_factory::create(TVert::Type);
			Mesh.addMeshBuffer(mb);
			mb->getMaterial() = Materials[0].Material;
			mb->drop();
			// add an empty mesh buffer name
			MeshBufferNames.push_back("");
		}
	}

	for (u32 i=0; i<MaterialGroups.size(); ++i)
	{
		IMeshBuffer* mb = 0;
		vid::SMaterial* mat=0;
		u32 mbPos;
		// -3 because we add three vertices at once
		u32 maxPrimitives = core::math::Min(
			VIDEO_DRIVER.getMaximalPrimitiveCount(),
			(u32)((1<<16)-1))-3; // currently hardcoded s16 max value for index pointers

		// find mesh buffer for this group
		for (mbPos=0; mbPos<Materials.size(); ++mbPos)
		{
			if (MaterialGroups[i].MaterialName == Materials[mbPos].Name)
			{
				mb = Mesh.getMeshBuffer(mbPos);
				mat=&Materials[mbPos].Material;
				MeshBufferNames[mbPos]=name;
				break;
			}
		}

		if (mb != 0)
		{
			// add geometry to the buffer.

			TVert vtx;
			core::vector3df vec;
			vtx.Normal.set(0,0,0);

			u32 vtxCountInit = mb->getVertexCount();
			u32 idxCountInit = mb->getIndexCount();

			for (s32 f = 0; f<MaterialGroups[i].faceCount; ++f)
			{
				u32 vtxCount = mb->getVertexCount();
				if (vtxCount>maxPrimitives)
				{
					IMeshBuffer* tmp = mesh_buffer_factory::create(TVert::Type);
					Mesh.addMeshBuffer(mb);
					mb->drop();
					Mesh.MeshBuffers[mbPos] = Mesh.MeshBuffers.get_last();
					Mesh.MeshBuffers[Mesh.MeshBuffers.size()-1] = tmp;
					mb->getMaterial() = tmp->getMaterial();
					vtxCount=0;
				}

				for (s32 v=0; v<3; ++v)
				{
					s32 idx = Indices[MaterialGroups[i].faces[f]*4 +v];

					if (CountVertices > idx)
					{
						vtx.Pos.X = Vertices[idx*3 + 0];
						vtx.Pos.Z = Vertices[idx*3 + 1];
						vtx.Pos.Y = Vertices[idx*3 + 2];
						//TransformationMatrix.transformVect(vtx.Pos);
					}

					if (CountTCoords > idx)
					{
						vtx.TCoords.X = TCoords[idx*2 + 0];
						vtx.TCoords.Y = 1.0f -TCoords[idx*2 + 1];
					}

					mb->addVertex(vtx, TVert::Type);
				}

				// add indices

				mb->addIndex(vtxCount);
				mb->addIndex(vtxCount+2);
				mb->addIndex(vtxCount+1);
			}

			// calculating and smoothing normals, according to smooth groups

			TVert* vertices = (TVert*)mb->getVertices();
			u16* indices = (u16*)mb->getIndices() + idxCountInit;

			u32 idxCnt = mb->getIndexCount() - idxCountInit;
			mesh_buffer_manipulator::SAdjFace<u16> *adj =
				mesh_buffer_manipulator::create_vertex_adjacency_data<u16>(
					TVert::Type, vertices, indices, idxCnt);
			for (u32 j = 0; j < idxCnt; j += 3)
			{
				TVert *vtx[] =
				{
					&vertices[indices[j]],
					&vertices[indices[j+1]],
					&vertices[indices[j+2]]
				};
				core::plane3d<f32> pl(vtx[0]->Pos, vtx[1]->Pos, vtx[2]->Pos);

				for (s32 v = 0; v < 3; ++v)
				{
					vtx[v]->Normal = pl.Normal;
					for (u32 k = 0; k < 32; k++)
					{
						u32 sg = 1 << k;
						mesh_buffer_manipulator::SAdjFace<u16> *adjv = &adj[j + v];
						while (adjv->Index != (u16)-1)
						{
							u32 adjFace = (adjv->Index) / 3;
							if (SmoothingGroups[adjFace] == sg)
							{
								u32 vtxIdx0 = (adjFace) * 3;
								core::plane3d<f32> p(
									vertices[indices[vtxIdx0]].Pos,
									vertices[indices[vtxIdx0 + 1]].Pos,
									vertices[indices[vtxIdx0 + 2]].Pos);
								if (pl.Normal.getAngleDeg(p.Normal) < 30)
									vtx[v]->Normal += p.Normal;
							}
							adjv = adjv->Next;
						}
					}
					vtx[v]->Normal.normalize();
				}
			}
			delete []adj;
		}
		else
			LOGGER.logWarn("Found no matching material for Group in 3ds file.");
	}

	cleanUp();
}

//---------------------------------------------------------------------------

void C3DSMesh::cleanUp()
{
	delete [] Vertices;
	CountVertices = 0;
	Vertices = 0;
	delete [] Indices;
	Indices = 0;
	CountFaces = 0;
	delete [] SmoothingGroups;
	SmoothingGroups = 0;
	delete [] TCoords;
	TCoords = 0;
	CountTCoords = 0;

	MaterialGroups.clear();
}

//---------------------------------------------------------------------------

void C3DSMesh::readTextureCoords(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountTCoords, sizeof(CountTCoords));
	data.read += sizeof(CountTCoords);

	s32 tcoordsBufferByteSize = CountTCoords * sizeof(f32) * 2;

	if (data.header.length - data.read != tcoordsBufferByteSize)
	{
		LOGGER.logWarn("Invalid size of tcoords found in 3ds file.");
		return;
	}

	TCoords = new f32[CountTCoords * 3];
	file->read(TCoords, tcoordsBufferByteSize);
	data.read += tcoordsBufferByteSize;
}

//---------------------------------------------------------------------------

void C3DSMesh::readMaterialGroup(io::IReadFile* file, ChunkData& data)
{
	SMaterialGroup group;

	readString(file, data, group.MaterialName);

	file->read(&group.faceCount, sizeof(group.faceCount));
	data.read += sizeof(group.faceCount);

	// read faces
	group.faces = new u16[group.faceCount];
	file->read(group.faces, sizeof(u16) * group.faceCount);
	data.read += sizeof(u16) * group.faceCount;

	MaterialGroups.push_back(group);
}

//---------------------------------------------------------------------------

void C3DSMesh::readIndices(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountFaces, sizeof(CountFaces));
	data.read += sizeof(CountFaces);

	s32 indexBufferByteSize = CountFaces * sizeof(u16) * 4;

	// Indices are u16s.
	// After every 3 Indices in the array, there follows an edge flag.
	Indices = new u16[CountFaces * 4];
	file->read(Indices, indexBufferByteSize);
	data.read += indexBufferByteSize;
}

//---------------------------------------------------------------------------

void C3DSMesh::readVertices(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountVertices, sizeof(CountVertices));
	data.read += sizeof(CountVertices);

	const s32 vertexBufferByteSize = CountVertices * sizeof(f32) * 3;

	if (data.header.length - data.read != vertexBufferByteSize)
	{
		LOGGER.logErr("Invalid size of vertices found in 3ds file %d",
			CountVertices);
		return;
	}

	Vertices = new f32[CountVertices * 3];
	file->read(Vertices, vertexBufferByteSize);
	data.read += vertexBufferByteSize;
}

//---------------------------------------------------------------------------

void C3DSMesh::readChunkData(io::IReadFile* file, ChunkData& data)
{
	file->read(&data.header, sizeof(ChunkHeader));
	data.read += sizeof(ChunkHeader);
}

//---------------------------------------------------------------------------

void C3DSMesh::readString(io::IReadFile* file, ChunkData& data, core::stringc& out)
{
	c8 c = 1;
	out = "";
	while (c)
	{
		file->read(&c, sizeof(c8));
		if (c)
			out.append(c);
	}
	data.read+=out.size()+1;
}

//---------------------------------------------------------------------------

bool C3DSMesh::loadFromFile(io::IReadFile* file)
{
    if (!file)
        return false;

	ChunkData data;

	readChunkData(file, data);

	if (data.header.id != C3DS_MAIN3DS )
		return 0;

	CurrentMaterial.clear();
	Materials.clear();
	MeshBufferNames.clear();
	cleanUp();

	if (!readChunk(file, &data))
		return false;

	// success

	for (s32 i = 0; i < Mesh.getMeshBuffersCount(); ++i)
	{
		IMeshBuffer *mb = Mesh.getMeshBuffer(i);
		// drop empty buffers
		if (mb->getIndexCount() == 0 || mb->getVertexCount() == 0)
		{
			Mesh.MeshBuffers.erase(i--);
			mb->drop();
		}
		else
			mesh_buffer_manipulator::calculate_tangents(mb);
	}

	recalculateBoundingBox();

    return true;
}

//---------------------------------------------------------------------------

void C3DSMesh::recalculateBoundingBox()
{
	for (s32 i = 0; i < Mesh.getMeshBuffersCount(); ++i)
	{
		IMeshBuffer *mb = Mesh.getMeshBuffer(i);
		mb->recalculateBoundingBox();
	}
	Mesh.recalculateBoundingBox();
	m_BoundingBox = Mesh.getBoundingBox();
}

//---------------------------------------------------------------------------

IMesh* C3DSMesh::getMesh()
{
    return &Mesh;
}

//---------------------------------------------------------------------------

IMesh* C3DSMesh::getMesh(f32 frame)
{
	return getMesh();
}

//----------------------------------------------------------------------------

void C3DSMesh::rescaleAnimatedMeshNormals(const core::vector3df &newScale) 
{
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

