//|-------------------------------------------------------------------------
//| File:        ITextSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITextSceneNodeHPP
#define ITextSceneNodeHPP
//-------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//-------------------------------------------------------------------------
namespace my {
namespace scn {
//-------------------------------------------------------------------------

//! A scene node for displaying 2d text at a position in three dimensional space
class ITextSceneNode : public ISceneNode
{
public:

	// constructor
	ITextSceneNode(ISceneNode *parent, s32 id)
		: ISceneNode(ESNT_TEXT_SCENE_NODE, parent, id) {}

	//! sets the font of the text
	virtual void setFont(vid::IFont *font) = 0;

	//! returns the font of the text
	virtual vid::IFont* getFont () const = 0;

	//! sets the text string
	virtual void setText(const c8* text) = 0;

	//! returns the text string
	virtual const c8* getText() const = 0;

	//! sets the color of the text
	virtual void setTextColor(const img::SColor &color) = 0;

	//! returns the color of the text
	virtual const img::SColor& getTextColor() const = 0;
};

//-------------------------------------------------------------------------
} // end namespace scn
} // end namespace my   
//-------------------------------------------------------------------------

#endif // #ifndef ITextSceneNodeHPP

