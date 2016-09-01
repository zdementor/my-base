//|-------------------------------------------------------------------------
//| File:        CSkyBoxSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSkyBoxSceneNodeHPP
#define CSkyBoxSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISkyBoxSceneNode.h>
#include <vid/S3DVertex.h>
#include <scn/mesh_buffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CSkyBoxSceneNode : public ISkyBoxSceneNode
{
public:

    CSkyBoxSceneNode(ISceneNode* parent, s32 id);
    virtual ~CSkyBoxSceneNode();

    //! set material
    virtual void setMaterial(vid::SMaterial& mat)
    {
		s32 mat_count = getMaterialsCount();

		for (s32 m=0; m < mat_count; m++)
		{
			vid::SMaterial& Mat = getMaterial(m);

			vid::ITexture* tex = Mat.getPass(0).Layers[0].getTexture();

			Mat = mat;
			
			Mat.getPass(0).Layers[0].setTexture(tex);			
		}
	}

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);

    virtual const core::aabbox3df& getBoundingBox() const;

    virtual vid::SMaterial& getMaterial(s32 i);
    virtual s32 getMaterialsCount();

	virtual void setSides (
		vid::ITexture* top, vid::ITexture* bottom, vid::ITexture* left,
        vid::ITexture* right, vid::ITexture* front, vid::ITexture* back);

	virtual vid::ITexture* getTopSideTexture();		
	virtual vid::ITexture* getBottomSideTexture(); 
    virtual vid::ITexture* getRightSideTexture(); 
	virtual vid::ITexture* getLeftSideTexture();
	virtual vid::ITexture* getFrontSideTexture(); 
	virtual vid::ITexture* getBackSideTexture();

	virtual const c8* getSceneCorePropertiesXMLString();

	virtual bool set(ISceneNode* other);

private:

    core::aabbox3df Box;
    u16 Indices[6];
    vid::S3DVertex1TCoords Vertices[4*6];
    vid::SMaterial Material[6];
	vid::ITexture *m_DefaultTexture;
};
//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
#endif

