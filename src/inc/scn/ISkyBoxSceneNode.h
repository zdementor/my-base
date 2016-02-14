//|-------------------------------------------------------------------------
//| File:        ISkyBoxSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISkyBoxSceneNodeHPP
#define ISkyBoxSceneNodeHPP
//-----------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//-----------------------------------------------------------------------------
namespace my{
namespace scn{
//-----------------------------------------------------------------------------

//! interfase for using Sky Box scene node
class __MY_SCN_LIB_API__ ISkyBoxSceneNode : public ISceneNode
{	
public:
	//! constructor
	ISkyBoxSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id) 
		: ISceneNode(type, parent, id) {}

	//! destructor
	virtual ~ISkyBoxSceneNode()  {}

	//! setting box sides
	virtual void setSides (
		vid::ITexture* top, vid::ITexture* bottom, vid::ITexture* left,
        vid::ITexture* right, vid::ITexture* front, vid::ITexture* back
        ) = 0;

	//! getting box sides textures
	virtual vid::ITexture* getTopSideTexture() = 0;		
	virtual vid::ITexture* getBottomSideTexture() = 0; 
	virtual vid::ITexture* getLeftSideTexture() = 0;
    virtual vid::ITexture* getRightSideTexture() = 0; 
	virtual vid::ITexture* getFrontSideTexture() = 0; 
	virtual vid::ITexture* getBackSideTexture() = 0;
};

//-----------------------------------------------------------------------------
} // namespace scn
} // namespace my
//-----------------------------------------------------------------------------

#endif // #ifndef ISkyBoxSceneNodeHPP