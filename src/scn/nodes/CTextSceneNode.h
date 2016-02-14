//|-------------------------------------------------------------------------
//| File:        CTextSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTextSceneNodeHPP
#define CTextSceneNodeHPP
//-------------------------------------------------------------------------

#include <scn/ITextSceneNode.h> 

//-------------------------------------------------------------------------
namespace my {
namespace scn {
//-------------------------------------------------------------------------

class CTextSceneNode : public ITextSceneNode
{
public:

	CTextSceneNode(
		ISceneNode *parent, s32 id,
		vid::IFont *font, const c8* text = 0,
		img::SColor color = img::SColor(100,0,0,0) );

	virtual ~CTextSceneNode();

	virtual void OnCheckCulling();

    virtual void OnPreRender(u32 timeMs);

	virtual const core::aabbox3df& getBoundingBox() const
	{ return m_BoundingBox; }

	virtual s32 getMaterialsCount()
	{ return 0; }

	virtual void setFont(vid::IFont *font);

	virtual vid::IFont* getFont () const
	{ return m_Font; }

	virtual void setText(const c8* text);

	virtual const c8* getText () const
	{ return m_Text.c_str(); }

	virtual void setTextColor(const img::SColor & color);

	virtual const img::SColor& getTextColor () const
	{ return m_TextColor; }

	virtual bool set(ISceneNode *other);

	virtual const c8* getSceneCorePropertiesXMLString();
	
private:

	core::stringc m_Text;
	img::SColor m_TextColor;
	vid::IFont* m_Font;

	core::aabbox3d<f32> m_BoundingBox;
};  	

//-------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//-------------------------------------------------------------------------

#endif

