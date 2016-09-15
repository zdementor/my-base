//|-------------------------------------------------------------------------
//| File:        CTerrainSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CTerrainSceneNode.h"

#include <res/IResourceManager.h>
#include <scn/ISceneManager.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CTerrainSceneNode::CTerrainSceneNode(ISceneNode* parent, s32 id)
	: ITerrainSceneNode(ESNT_TERRAIN_SCENE_NODE, parent, id),
	m_VideoDriver(VIDEO_DRIVER), m_EnabledLights(0), m_Reset(false)
{
	m_HeightFieldSize = m_HeightFieldSizeValue = 0; 

	m_TileRepeatNumber = 10.f;

	m_HeightMapFileName = "";
	m_FogMapFileName = "";

	OldCameraPosition = core::vector3df(-99999.9f, -99999.9f, -99999.9f );
	OldCameraRotation = core::vector3df(-99999.9f, -99999.9f, -99999.9f );
	CameraMovementDelta = 25.0f;
	CameraRotationDelta = 2.0f;

	m_HeightMapTextureName = "";

	m_HeightScale = 1.f;
}

//---------------------------------------------------------------------------

CTerrainSceneNode::~CTerrainSceneNode()
{
	_clearMaps();
	_clearTiles();
}

//---------------------------------------------------------------------------

void CTerrainSceneNode::_clearMaps()
{
	m_HeightFieldSize = m_HeightFieldSizeValue = 0; 

	m_HeightMapTextureName = "";
}

//---------------------------------------------------------------------------

void CTerrainSceneNode::_clearTiles()
{
	m_Tiles.set_used(0);
	m_Material = vid::SMaterial();

	for (u32 t = 0; t < m_TilesTextures.size(); t++)
	{
		for (u32 tt = 0; tt < m_TilesTextures[t].size(); tt++)
			SAFE_DROP(m_TilesTextures[t][tt]);
		m_TilesTextures[t].set_used(0);
	}
	m_TilesTextures.set_used(0);
}

//---------------------------------------------------------------------------

void CTerrainSceneNode::_resetMaterial()
{
	m_Material = vid::SMaterial();

	u32 passesCnt = 0;

	for (u32 t = 0; t < m_TilesTextures.size(); t++)
	{
		TTexArr &texArr = m_TilesTextures[t];

		if (!VIDEO_DRIVER.isUseShaders())
		{
			for (u32 tt = 0; tt < texArr.size() && texArr.size() >= 2; tt += 2)
			{
				vid::ITexture *tileTex = texArr[tt];
				vid::ITexture *maskTex = texArr[tt + 1];

				if (passesCnt > 0)
					m_Material.addPass(vid::SRenderPass());

				vid::SRenderPass &p = m_Material.getPass(m_Material.getPassesCount() - 1);		
				
				p.setBlendFuncs(vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);

				p.setFlag ( vid::EMF_BLENDING, (m_Material.getPassesCount() > 1) );

				p.setLightingMode(vid::ELM_8_LIGHTS);

				p.setFlag(vid::EMF_FOG_ENABLE, true);

				p.setDiffuseColor ( img::SColor (255,255,255,255) );
				p.setAmbientColor ( img::SColor (255,50,50,50) );
				p.setSpecularColor ( img::SColor (0,0,0,0) );
				p.setShininess ( 0 );

				p.Layers[0].setTexRGBScale ( 1 );
				p.Layers[1].setTexRGBScale ( 1 );
				p.Layers[2].setTexRGBScale ( 1 );
				p.Layers[3].setTexRGBScale ( 1 );

				p.Layers[0].setTexture(tileTex);
				p.Layers[1].setTexture(maskTex, vid::ETLT_DIFFUSE_MAP, 1);

				passesCnt++;
			}
		}
		else
		{
			for (u32 tt = 0; tt < texArr.size() && texArr.size() >= 4; tt += 4)
			{
				vid::ITexture *tileTex[] = { texArr[tt], texArr[tt + 1], texArr[tt + 2] };
				vid::ITexture *splatTex = texArr[tt + 3];

				if (passesCnt)
					m_Material.addPass(vid::SRenderPass());

				vid::SRenderPass &p = m_Material.getPass(m_Material.getPassesCount() - 1);		
				
				p.setFlag(vid::EMF_BLENDING, passesCnt > 0 );
				if (passesCnt > 0)
					p.setBlendFuncs(vid::ESBF_ONE, vid::EDBF_ONE_MINUS_SRC_ALPHA);

				p.setLightingMode(vid::ELM_8_LIGHTS);

				p.setFlag(vid::EMF_FOG_ENABLE, true);

				p.setDiffuseColor ( img::SColor (255,255,255,255) );
				p.setAmbientColor ( img::SColor (255,50,50,50) );
				p.setSpecularColor ( img::SColor (0,0,0,0) );
				p.setShininess ( 0 );

				p.Layers[0].setTexRGBScale ( 1 );
				p.Layers[1].setTexRGBScale ( 1 );
				p.Layers[2].setTexRGBScale ( 1 );
				p.Layers[3].setTexRGBScale ( 1 );

				vid::ITexture *texUnits[] = { tileTex[0], tileTex[1], tileTex[2], splatTex };
				for (u32 tu = 0; tu < sizeof(texUnits) / sizeof(*texUnits); tu++)
				{
					p.Layers[tu].setTexture(texUnits[tu],
						texUnits[tu] == splatTex ? vid::ETLT_SPLATTING_MAP : vid::ETLT_DIFFUSE_MAP,
						texUnits[tu] == splatTex ? 1 : 0);
				}

				passesCnt++;
			}
		}
	}
}

//---------------------------------------------------------------------------

bool CTerrainSceneNode::_setMaps(
	const c8* hmap_filename, const c8* fogmap_filename,
    f32 gridPointSpacing, f32 heightScale, img::IImage **heightImageOut)
{
	img::IImage *heightmap = IMAGE_LIBRARY.getImage(hmap_filename);
	if (!heightmap)
		hmap_filename = NULL;

	m_HeightMapFileName = hmap_filename ? hmap_filename : NONAME_FILE_NAME;
	m_FogMapFileName = fogmap_filename ? fogmap_filename : NONAME_FILE_NAME;

	m_GridPointSpacing = m_GridPointSpacingValue = gridPointSpacing;
	m_HeightScale = heightScale;

	_clearMaps();

	const u32 defaultImgSize = 16;

	m_HeightFieldSize = m_HeightFieldSizeValue = heightmap ? heightmap->getDimension().Width : defaultImgSize;

	m_HeightMin = 10000000; 
    m_HeightMax =-10000000; 

	m_SizeDiv2Dim = m_GridPointSpacing * (m_HeightFieldSizeValue / 2.0f - 0.5);

	img::IImage* heightImage = IMAGE_LIBRARY.createEmptyImage(
		core::dimension2di(m_HeightFieldSize, m_HeightFieldSize), img::ECF_LUMINANCE16);

	if (heightmap)
	{
		u16 *p = (u16 *)heightImage->getData();

		for (s32 i = 0; i < m_HeightFieldSize; i++) 
		{   
			for (s32 j = 0; j < m_HeightFieldSize; j++) 
			{   
				p[i + j * m_HeightFieldSize] = (0x7fff * (heightmap->getPixel(i, m_HeightFieldSize - 1 - j).getRed() / 255.f));
	            
				if (m_HeightMin > p[i + j * m_HeightFieldSize]) 
					m_HeightMin = p[i + j * m_HeightFieldSize]; 
				if (m_HeightMax < p[i + j * m_HeightFieldSize]) 
					m_HeightMax = p[i + j * m_HeightFieldSize];
			} 
		}
	}
	else
	{
		u16 *p = (u16 *)heightImage->getData();

		for (s32 i = 0; i < m_HeightFieldSize; i++) 
		{   
			f32 x = 0.5f + 0.5f * core::math::Sin(core::TWO_PI * 2.f * (f32)i / (f32)(defaultImgSize - 1.f));

			for (s32 j=0; j < m_HeightFieldSize; j++) 
			{   
				f32 y = 0.5f + 0.5f * core::math::Sin(core::TWO_PI * 2.f * (f32)j / (f32)(defaultImgSize - 1.f));

				p[i + j * m_HeightFieldSizeValue] = (0x7fff * (u8(0xff & (u32((x * y) * 255.0f + 0.5f))) / 255.f));
	            
				if (m_HeightMin > p[i + j * m_HeightFieldSize]) 
					m_HeightMin = p[i + j * m_HeightFieldSize];
				if (m_HeightMax < p[i + j * m_HeightFieldSize]) 
					m_HeightMax = p[i + j * m_HeightFieldSize]; 
			} 
		}
	}

	m_HeightMin *= m_HeightScale;
	m_HeightMax *= m_HeightScale;

	m_HeightMapTextureName.sprintf("heightmap.texture.%d.%p", m_HeightFieldSizeValue, this);

	*heightImageOut = heightImage;

	return true;
}

//---------------------------------------------------------------------------

const c8* CTerrainSceneNode::getHeightMapFileName()
{
	return m_HeightMapFileName.c_str();
}

//---------------------------------------------------------------------------

f32  CTerrainSceneNode::getHeightScale()
{
	return m_HeightScale;
}

//---------------------------------------------------------------------------

f32  CTerrainSceneNode::getGridPointSpacing()
{
	return m_GridPointSpacingValue;
}

//---------------------------------------------------------------------------

void CTerrainSceneNode::setTileSets(const core::array<STileSetParams> &tiles)
{
	_clearTiles();

	for (u32 t = 0; t < tiles.size(); t++)
	{
		_addTileSet(
			tiles[t].getTile0FileName(),
			tiles[t].getTile1FileName(),
			tiles[t].getTile2FileName(),
			tiles[t].getSplatMapFileName());
	}

	_resetMaterial();
}
//---------------------------------------------------------------------------

bool CTerrainSceneNode::_addTileSet(
	const c8 *tile0tex_file_name,
	const c8 *tile1tex_file_name,
	const c8 *tile2tex_file_name,
	const c8 *splatmap_file_name)
{
	m_Tiles.push_back(STileSet());
	m_TilesTextures.push_back(TTexArr());

	u32 idx = m_Tiles.size() - 1;

	bool res = _setTileSet(
		idx,
		tile0tex_file_name,
		tile1tex_file_name,
		tile2tex_file_name,
		splatmap_file_name);

	if (!res)
	{
		m_Tiles.erase(idx);
		for (u32 t = 0; t < m_TilesTextures[idx].size(); t++)
			SAFE_DROP(m_TilesTextures[idx][t]);
		m_TilesTextures.erase(idx);
	}

	return res;
}

//---------------------------------------------------------------------------

bool CTerrainSceneNode::addTileSet(
	const c8 *tile0tex_file_name,
	const c8 *tile1tex_file_name,
	const c8 *tile2tex_file_name,
	const c8 *splatmap_file_name)
{
	bool res = _addTileSet(
		tile0tex_file_name,
		tile1tex_file_name,
		tile2tex_file_name,
		splatmap_file_name);

	if (res)
		_resetMaterial();

	return res;
}

//---------------------------------------------------------------------------

bool CTerrainSceneNode::_setTileSet(u32 idx,
	const c8 *tile0tex_file_name,
	const c8 *tile1tex_file_name,
	const c8 *tile2tex_file_name,
	const c8 *splatmap_file_name)
{
	if (idx >= m_Tiles.size() || !splatmap_file_name)
		return false;

	img::IImage *splatmap = IMAGE_LIBRARY.getImage(splatmap_file_name);
	if (!splatmap)
		return false;

	TTexArr &texArr = m_TilesTextures[idx];

	for (u32 t = 0; t < m_TilesTextures[idx].size(); t++)
		SAFE_DROP(m_TilesTextures[idx][t]);
	m_TilesTextures[idx].set_used(0);

	vid::ITexture *tileTex[3] =
	{
		(tile0tex_file_name ? VIDEO_DRIVER.getTexture(tile0tex_file_name) : NULL),
		(tile1tex_file_name ? VIDEO_DRIVER.getTexture(tile1tex_file_name) : NULL),
		(tile2tex_file_name ? VIDEO_DRIVER.getTexture(tile2tex_file_name) : NULL)
	};

	if (!VIDEO_DRIVER.isUseShaders())
	{
		vid::ITexture *maskTex[3] = { NULL, NULL, NULL };

		s32 sw = splatmap->getDimension().Width;
		s32 sh = splatmap->getDimension().Height;

		img::IImage* maskImg[sizeof(tileTex) / sizeof(*tileTex)] =
		{
			!tileTex[0] ? NULL : IMAGE_LIBRARY.createEmptyImage(splatmap->getDimension(), img::ECF_A8R8G8B8),
			!tileTex[1] ? NULL : IMAGE_LIBRARY.createEmptyImage(splatmap->getDimension(), img::ECF_A8R8G8B8),
			!tileTex[2] ? NULL : IMAGE_LIBRARY.createEmptyImage(splatmap->getDimension(), img::ECF_A8R8G8B8)
		};

		// splatting map normalization
		for (s32 i = 0; i < sw; i++)
		{
			for (s32 j=0; j<sh; j++)
			{   
				img::SColor color = splatmap->getPixel(i, j);

				f32 r = color.getRed();
				f32 g = color.getGreen();
				f32 b = color.getBlue();

				f32 rgb = ( r + g + b );
				
				CHECK_MIN_RANGE(rgb, 0.001f);
				f32 k = 255.0f / rgb;

				r *= k;
				g *= k;
				b *= k;

				CHECK_RANGE(r, 0, 255);
				CHECK_RANGE(g, 0, 255);
				CHECK_RANGE(b, 0, 255);			

				if (maskImg[0])
					maskImg[0]->setPixel(i, j, img::SColor(r,255,255,255));
				if (maskImg[1])
					maskImg[1]->setPixel(i, j, img::SColor(g,255,255,255));
				if (maskImg[2])
					maskImg[2]->setPixel(i, j, img::SColor(b,255,255,255));			
			}
		}

		for (s32 i = 0; i <  sizeof(tileTex) / sizeof(*tileTex); i++)
		{
			img::IImage *img = maskImg[i];
			if (!img)
				continue;
			core::stringc tex_name = "";

			tex_name.sprintf("%p.splattex.%d", this, i);

			maskTex[i] = VIDEO_DRIVER.addTexture(tex_name.c_str(), img);

			SAFE_DROP(img);
		}

		for (u32 t = 0; t < sizeof(tileTex) / sizeof(*tileTex); t++)
		{
			if (!tileTex[t] || !maskTex[t])
				continue;

			texArr.push_back(tileTex[t]); SAFE_GRAB(tileTex[t]);
			texArr.push_back(maskTex[t]); SAFE_GRAB(maskTex[t]);
		}
	}
	else
	{
		vid::ITexture *splatTex = NULL;

		s32 sw = splatmap->getDimension().Width;
		s32 sh = splatmap->getDimension().Height;

		img::IImage* splargb = IMAGE_LIBRARY.createEmptyImage(splatmap->getDimension(), img::ECF_A8R8G8B8);

		// splatting map normalization
		for (s32 i = 0; i < sw; i++)
		{
			for (s32 j=0; j<sh; j++)
			{   
				img::SColor color = splatmap->getPixel(i, j);

				f32 r = color.getRed();
				f32 g = color.getGreen();
				f32 b = color.getBlue();

				f32 rgb = ( r + g + b );
				
				CHECK_MIN_RANGE(rgb, 0.001f);
				f32 k = 255.0f / rgb;

				r *= k;
				g *= k;
				b *= k;

				CHECK_RANGE(r, 0, 255);
				CHECK_RANGE(g, 0, 255);
				CHECK_RANGE(b, 0, 255);

				splargb->setPixel(i, j, img::SColor(m_Tiles.size() > 1 ? (r + g + b) : 255, r, g, b));	
			}
		}

		splatTex = VIDEO_DRIVER.addTexture(splatmap_file_name, splargb);
		SAFE_DROP(splargb);

		texArr.push_back(tileTex[0]); SAFE_GRAB(tileTex[0]);
		texArr.push_back(tileTex[1]); SAFE_GRAB(tileTex[1]);
		texArr.push_back(tileTex[2]); SAFE_GRAB(tileTex[2]);
		texArr.push_back(splatTex);   SAFE_GRAB(splatTex);
	}

	m_Tiles[idx].setSplatMapFileName(splatmap_file_name);
	m_Tiles[idx].setTile0Texture(tileTex[0]);
	m_Tiles[idx].setTile1Texture(tileTex[1]);
	m_Tiles[idx].setTile2Texture(tileTex[2]);

	return true;
}

//---------------------------------------------------------------------------

bool CTerrainSceneNode::setTileSet(u32 idx,
	const c8 *tile0tex_file_name,
	const c8 *tile1tex_file_name,
	const c8 *tile2tex_file_name,
	const c8 *splatmap_file_name)
{
	if (!_setTileSet(idx,
			tile0tex_file_name,
			tile1tex_file_name,
			tile2tex_file_name,
			splatmap_file_name))
	{
		removeTileSet(idx);
		return false;
	}

	_resetMaterial();

	return true;
}

//---------------------------------------------------------------------------

bool CTerrainSceneNode::removeTileSet(u32 idx)
{
	if (idx >= m_Tiles.size())
		return false;

	m_Tiles.erase(idx);
	for (u32 t = 0; t < m_TilesTextures[idx].size(); t++)
		SAFE_DROP(m_TilesTextures[idx][t]);
	m_TilesTextures.erase(idx);

	_resetMaterial();

	return true;
}

//---------------------------------------------------------------------------

const STileSet& CTerrainSceneNode::getTileSet(u32 idx)
{
	static STileSet ts;
	if (idx >= m_Tiles.size())
		return ts;
	return m_Tiles[idx];
}

//---------------------------------------------------------------------------

u32 CTerrainSceneNode::getTileSetsCount()
{
	return m_Tiles.size();
}

//---------------------------------------------------------------------------

u32 CTerrainSceneNode::getHeightFieldSize() 
{ 
	return m_HeightFieldSizeValue; 
} 

//------------------------------------------------------------------------------

const core::vector3df& CTerrainSceneNode::getCellPosition(u32 cell_x, u32 cell_y)
{	
	static core::vector3df v;

	f32 x = m_GridPointSpacing * (f32)cell_x - m_SizeDiv2Dim;
    f32 z = m_SizeDiv2Dim - m_GridPointSpacing * (f32)cell_y;  

	f32 height = getCellHeight(cell_x, cell_y);	

	v.set(x, height, z);
	return v;
}

//---------------------------------------------------------------------------

void CTerrainSceneNode::setTileRepeatNumber(u32 value) 
{
	if (m_TileRepeatNumber != value)
	{
		m_Reset = true;
		m_TileRepeatNumber = value;
	}
}

//---------------------------------------------------------------------------

u32 CTerrainSceneNode::getTileRepeatNumber()
{
	return m_TileRepeatNumber;
}

//-------------------------------------------------------------------------

vid::ITexture* CTerrainSceneNode::getHeightMapTexture()
{
	if (m_HeightMapTextureName.size() == 0)
		return NULL;

	vid::ITexture *tex = m_VideoDriver.findTexture(m_HeightMapTextureName.c_str());
	if (!tex)
	{
		tex = m_VideoDriver.addTexture(core::dimension2di(m_HeightFieldSizeValue, m_HeightFieldSizeValue),
			m_HeightMapTextureName.c_str(), img::ECF_LUMINANCE16);

		if (tex)
		{
			img::IImage *img = tex->lock();

			f32 k = (f32)0xffff / (f32)0x7fff;

			if (img && img->getBytesPerPixel() == sizeof(s16))
			{
				s32 iwidth = img->getDimension().Width;
				s32 iheight = img->getDimension().Height;

				s16 *p = (s16 *)img->getData();
				for (s32 i = 0; i < iwidth; i++)
				{
					f32 ii = ((f32)i / (f32)(m_HeightFieldSizeValue-1)) * (f32)iwidth;
					for (s32 j = 0; j < iheight; j++)
					{
						f32 jj = ((f32)j / (f32)(m_HeightFieldSizeValue-1)) * (f32)iheight;
						p[(i) * m_HeightFieldSizeValue + j] = k * getCellHeight(ii, jj) / m_HeightScale;
					}
				}
			}

			tex->unlock();
		}
	}

	return tex;
}

//---------------------------------------------------------------------------

//!
const c8* CTerrainSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	core::stringc fullfname, fname, fpath, texpath, relpath, relfullfname;

	/////////////////////////////

	for (u32 t=0; t < m_Tiles.size(); t++)
	{
		STileSet &tile = m_Tiles[t];

		vid::ITexture *tileTex[] =
		{
			tile.getTile0Texture(),
			tile.getTile1Texture(),
			tile.getTile2Texture()
		};

		str.sprintf("<TileSet>\n");
		xmlstr.append(str);

		for (u32 tt = 0 ; tt < sizeof(tileTex) / sizeof(*tileTex); tt++)
		{
			fullfname = VIDEO_DRIVER.findTextureName(tileTex[tt]);

			if (fullfname.size() > 0 && fullfname != NONAME_FILE_NAME)
			{
				fname =  core::extractFileName(fullfname);
				fpath =  core::extractFilePath(fullfname);
				texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
				relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
				relfullfname = relpath.c_str();
				relfullfname.append(fname.c_str());
			}
			else
			{
				relfullfname = NONAME_FILE_NAME;
			}

			str.sprintf("<Tile%dTextureFileName value=\"%s\" />\n", tt, relfullfname.c_str());
			xmlstr.append(str);
		}

		fullfname = tile.getSplatMapFileName();

		if (fullfname.size() > 0 && fullfname != NONAME_FILE_NAME)
		{
			fname =  core::extractFileName(fullfname);
			fpath =  core::extractFilePath(fullfname);
			texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
			relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
			relfullfname = relpath.c_str();
			relfullfname.append(fname.c_str());
		}
		else
		{
			relfullfname = NONAME_FILE_NAME;
		}

		str.sprintf("<SplattingMapFileName value=\"%s\" />\n", relfullfname.c_str());
		xmlstr.append(str);

		str.sprintf("</TileSet>\n");
		xmlstr.append(str);
	}

	/////////////////////////////

	str.sprintf("<TileRepeatNumber value=\"%d\" />\n", getTileRepeatNumber());
	xmlstr.append(str);

	/////////////////////////////

	fullfname = m_HeightMapFileName;

	if (fullfname.size()>0 && fullfname!=NONAME_FILE_NAME)
	{
		fullfname = m_HeightMapFileName;
		fname =  core::extractFileName(fullfname);
		fpath =  core::extractFilePath(fullfname);
		texpath = RESOURCE_MANAGER.getMediaDirFull( res::EMT_TEXTURES );
		relpath = core::getRelativePathBetweenDirectories(texpath, fpath);
		relfullfname = relpath.c_str();
		relfullfname.append(fname.c_str());
	}
	else
	{
		relfullfname = NONAME_FILE_NAME;
	}

	str.sprintf("<HeightMapFileName value=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);	

	/////////////////////////////

	str.sprintf("<HeightScale value=\"%.4f\" />\n", getHeightScale());
	xmlstr.append(str);

	/////////////////////////////

	str.sprintf("<GridPointSpacing value=\"%.2f\" />\n", getGridPointSpacing());
	xmlstr.append(str);

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------

} // end namespace scn
} // end namespace my

