//|-------------------------------------------------------------------------
//| File:        CTextSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CTextSceneNode.h"

#include <scn/ISceneManager.h>
#include <vid/IFont.h>
#include <scn/ICullingSystem.h>

//-------------------------------------------------------------------------
namespace my {
namespace scn {
//-------------------------------------------------------------------------
   
CTextSceneNode::CTextSceneNode(
	ISceneNode* parent, s32 id,
	vid::IFont* font, const c8* text, img::SColor color
	) : ITextSceneNode(parent, id),
m_Text(text), m_TextColor(color), m_Font(font) 	
{
#ifdef _DEBUG
	setClassName("CTextSceneNode");
#endif
	SAFE_GRAB(m_Font);
}

//-------------------------------------------------------------------------

CTextSceneNode::~CTextSceneNode()
{
	SAFE_DROP(m_Font);
}

//---------------------------------------------------------------------------

void CTextSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{
		if (IsVisible && m_Font)
			m_Culled = CULLING_SYSTEM.isCull(this);
		else
			m_Culled = true;		
		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CTextSceneNode::OnPreRender(u32 timeMs)
{
    scn::ISceneManager &smgr = SCENE_MANAGER;		

	core::position2d<s32> pos = 
		SCENE_MANAGER.getScreenCoordinatesFrom3DPosition(
			getAbsolutePosition()); 
	core::rect<s32> r(pos, core::dimension2d<s32>(1,1));
	m_Font->registerForRendering(
		m_Text.c_str(), m_Text.size(), r, m_TextColor, true, true);

    ISceneNode::OnPreRender(timeMs);
}

//-------------------------------------------------------------------------

void CTextSceneNode::setFont(vid::IFont *font)
{
	if (!font)
		return;
	SAFE_DROP(m_Font);
	m_Font = font;
	SAFE_GRAB(m_Font);
}

//-------------------------------------------------------------------------

void CTextSceneNode::setText(const c8 *text)
{
	m_Text = text;
}

//-------------------------------------------------------------------------

void CTextSceneNode::setTextColor(const img::SColor& color)
{
	m_TextColor = color;
}

//---------------------------------------------------------------------------

bool CTextSceneNode::set(ISceneNode* other)
{
	if (!other || this == other
			|| other->getSceneNodeType()!=ESNT_TEXT_SCENE_NODE)
		return false;

	setFont     (((ITextSceneNode*)other)->getFont()     );
	setText     (((ITextSceneNode*)other)->getText()     );
	setTextColor(((ITextSceneNode*)other)->getTextColor());  	

	return true;
}

//---------------------------------------------------------------------------

const c8* CTextSceneNode::getSceneCorePropertiesXMLString()
{
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0));
		xmlstr.append(mat_xml);
	}

	str.sprintf(
		"<Font filename=\"%s\" size=\"%d/\" />\n",
		m_Font->getFileName(), m_Font->getSize()		
		);
	xmlstr.append(str);

	str.sprintf(
		"<Text value=\"%s\" />\n",
		core::stringc(getText()).c_str()
		);
	xmlstr.append(str);

	img::SColor c = getTextColor();

	str.sprintf(
		"<TextColor value=\"%d,%d,%d,%d\" />\n",
		c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() 
		);
	xmlstr.append(str);

	return xmlstr.c_str();
}

//-------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//-------------------------------------------------------------------------
