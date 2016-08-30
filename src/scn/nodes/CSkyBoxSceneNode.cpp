//|-------------------------------------------------------------------------
//| File:        CSkyBoxSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSkyBoxSceneNode.h"
#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <scn/ICameraSceneNode.h>
#include <vid/S3DVertex.h>
#include <res/IResourceManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CSkyBoxSceneNode::CSkyBoxSceneNode(ISceneNode* parent, s32 id) 
	: ISkyBoxSceneNode(ESNT_SKY_BOX_SCENE_NODE, parent, id)
{
#if MY_DEBUG_MODE 
	setClassName("CSkyBoxSceneNode");
#endif

    // create indices
    Indices[0] = 0;
    Indices[1] = 1; 
    Indices[2] = 2; 
    Indices[3] = 0; 
    Indices[4] = 2; 
    Indices[5] = 3; 
}

//---------------------------------------------------------------------------

CSkyBoxSceneNode::~CSkyBoxSceneNode()
{
}

//---------------------------------------------------------------------------

const core::aabbox3df& CSkyBoxSceneNode::getBoundingBox() const
{
    return Box;
}

//---------------------------------------------------------------------------

void CSkyBoxSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		if (IsVisible)  		
		{
			m_Culled = false;		
		}

		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CSkyBoxSceneNode::OnPreRender(u32 timeMs)
{
	core::matrix4 view = VIDEO_DRIVER.getTransform(vid::ETS_VIEW);
	view.makeInversed();
	view.setRotationDegrees(core::vector3df(0, 0, 0));

	for (u32 i = 0; i < 6; i++)
	{
		vid::SRenderPass &pass = Material[i].getPass(0);

		pass.setLightingMode(vid::ELM_NONE);

		pass.setFlag(vid::EMF_ZWRITE_ENABLE, false);
		pass.setFlag(vid::EMF_GOURAUD_SHADING, false);

		pass.setDepthTest(vid::ECT_ALWAYS);

		pass.Layers[0].setTexCoordAddr(vid::ETCAM_CLAMP);
		pass.Layers[0].enableFilter(false);

		VIDEO_DRIVER.registerGeometryForRendering(
			vid::ERP_3D_SKY_PASS,
			view, NULL, 0,
			Material[i],
			&Vertices[i * 4], 4, Vertices->Type,
			&Indices[0], 6, EIT_16_BIT,
			vid::EDPT_TRIANGLE_LIST);
	}

	bool drawDebug = isDebugDataVisible();

	if (drawDebug)
    {
    }

    ISceneNode::OnPreRender(timeMs);
}

//---------------------------------------------------------------------------

vid::SMaterial& CSkyBoxSceneNode::getMaterial(s32 i)
{
    return Material[i];
}

//---------------------------------------------------------------------------

s32 CSkyBoxSceneNode::getMaterialsCount()
{
    return 6;
}

//---------------------------------------------------------------------------

void CSkyBoxSceneNode::setSides (	
	vid::ITexture* top,   vid::ITexture* bottom,	
	vid::ITexture* right, vid::ITexture* left,  
	vid::ITexture* front, vid::ITexture* back)
{	
    // create material    
    vid::SRenderPass pass;

    pass.setLightingMode(vid::ELM_NONE);

    pass.setFlag(vid::EMF_ZWRITE_ENABLE, false);
	pass.setFlag(vid::EMF_GOURAUD_SHADING, false);

	pass.setDepthTest(vid::ECT_ALWAYS);

	pass.Layers[0].setTexCoordAddr(vid::ETCAM_BORDER);
	pass.Layers[0].enableFilter(false);

	//
	//       -111         111
	//          /6--------/5        y
	//         /  |      / |        ^  z
	//        /   |   11-1 |        | /
	//  -11-1 3---------2  |        |/
	//        |   7- - -| -4 1-11   *---->x
	//        | -1-11   |  /       3-------|2
	//        |/        | /         |    //|
	//        0---------1/          |  //  |
	//     -1-1-1     1-1-1         |//    |
	//                             0--------1
    
    f32 onepixel = 0.0f;	

    f32 l = 100.0f;
    f32 t = 1.0f;
    f32 o = 0.0f;

	// create right side
	Material[0].getPass(0) = pass;
    Material[0].getPass(0).Layers[0].setTexture( left );
    Vertices[0] = vid::S3DVertex1TCoords(-l,-l,-l, 0,0,1, t, t);
    Vertices[1] = vid::S3DVertex1TCoords( l,-l,-l, 0,0,1, o, t);
    Vertices[2] = vid::S3DVertex1TCoords( l, l,-l, 0,0,1, o, o);
    Vertices[3] = vid::S3DVertex1TCoords(-l, l,-l, 0,0,1, t, o);

	// create front side
    Material[1].getPass(0) = pass;
    Material[1].getPass(0).Layers[0].setTexture( front );
    Vertices[4] = vid::S3DVertex1TCoords( l,-l,-l, -1,0,0, t, t);
    Vertices[5] = vid::S3DVertex1TCoords( l,-l, l, -1,0,0, o, t);
    Vertices[6] = vid::S3DVertex1TCoords( l, l, l, -1,0,0, o, o);
    Vertices[7] = vid::S3DVertex1TCoords( l, l,-l, -1,0,0, t, o);

	// create left side
	Material[2].getPass(0) = pass;
    Material[2].getPass(0).Layers[0].setTexture( right );
    Vertices[8]  = vid::S3DVertex1TCoords( l,-l, l, 0,0,-1, t, t);
    Vertices[9]  = vid::S3DVertex1TCoords(-l,-l, l, 0,0,-1, o, t);
    Vertices[10] = vid::S3DVertex1TCoords(-l, l, l, 0,0,-1, o, o);
    Vertices[11] = vid::S3DVertex1TCoords( l, l, l, 0,0,-1, t, o);

	// create back side
    Material[3].getPass(0) = pass;
    Material[3].getPass(0).Layers[0].setTexture( back );
    Vertices[12] = vid::S3DVertex1TCoords(-l,-l, l, 1,0,0, t, t);
    Vertices[13] = vid::S3DVertex1TCoords(-l,-l,-l, 1,0,0, o, t);
    Vertices[14] = vid::S3DVertex1TCoords(-l, l,-l, 1,0,0, o, o);
    Vertices[15] = vid::S3DVertex1TCoords(-l, l, l, 1,0,0, t, o);

	// create top side
    Material[4].getPass(0) = pass;
    Material[4].getPass(0).Layers[0].setTexture( top );
    Vertices[16] = vid::S3DVertex1TCoords( l, l, l, 0,-1,0, o, t);
    Vertices[17] = vid::S3DVertex1TCoords(-l, l, l, 0,-1,0, o, o);
    Vertices[18] = vid::S3DVertex1TCoords(-l, l,-l, 0,-1,0, t, o);
    Vertices[19] = vid::S3DVertex1TCoords( l, l,-l, 0,-1,0, t, t);

    // create bottom side
    Material[5].getPass(0) = pass;
    Material[5].getPass(0).Layers[0].setTexture( bottom );
    Vertices[20] = vid::S3DVertex1TCoords(-l,-l, l, 0,1,0, o, t);
    Vertices[21] = vid::S3DVertex1TCoords( l,-l, l, 0,1,0, o, o);
    Vertices[22] = vid::S3DVertex1TCoords( l,-l,-l, 0,1,0, t, o);
    Vertices[23] = vid::S3DVertex1TCoords(-l,-l,-l, 0,1,0, t, t); 
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getTopSideTexture()
{
	return Material[4].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getBottomSideTexture()
{
	return Material[5].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getRightSideTexture()
{
	return Material[0].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getLeftSideTexture()
{
	return Material[2].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getFrontSideTexture()
{
	return Material[1].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

vid::ITexture* CSkyBoxSceneNode::getBackSideTexture()
{
	return Material[3].getPass(0).Layers[0].getTexture();
}

//---------------------------------------------------------------------------

//!
const c8* CSkyBoxSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	core::stringc relfullfname, fullfname, fname, fpath, texpath, relpath;
	
	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getTopSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES);
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<TopSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getBottomSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<BottomSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getRightSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<RightSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getLeftSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<LeftSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getFrontSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<FrontSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////

	fullfname = VIDEO_DRIVER.findTextureName(getBackSideTexture());
	fname =  core::extractFileName(fullfname);
	fpath =  core::extractFilePath(fullfname);
	texpath = RESOURCE_MANAGER.getMediaDirFull ( res::EMT_TEXTURES );
	relpath = core::getRelativePathBetweenDirectories(texpath, fpath);	
	relfullfname = relpath.c_str();
	relfullfname.append(fname.c_str());

	str.sprintf("<BackSideTexture filename=\"%s\" />\n", relfullfname.c_str());
	xmlstr.append(str);

	//////////////////////////////	

	return xmlstr.c_str();
}

//---------------------------------------------------------------------------

//!
bool CSkyBoxSceneNode::set(ISceneNode* other)
{
	ISkyBoxSceneNode* sky_other = SAFE_CAST_TO_SKY_BOX_SCENE_NODE(other);

	if (!sky_other || this == other)
		return false;

	setSides(
		sky_other->getTopSideTexture(),
		sky_other->getBottomSideTexture(),	
		sky_other->getRightSideTexture(),
		sky_other->getLeftSideTexture(),		
		sky_other->getFrontSideTexture(),
		sky_other->getBackSideTexture());

	return true;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------


