//|-------------------------------------------------------------------------
//| File:      ILensFlareSceneNode.h
//|
//| Descr:     This file is a part of the 'My Engine' v.0.1
//| Author(s): Zhuck 'zdementor' Dmitri (aka ZDimitor)
//| Email:     zdimitor@pochta.ru, sibergames@nm.ru
//|
//| Copyright (c) 2004-2009 by SiberGames, Krasnoyarsk - Moscow. All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILensFlareSceneNodeHPP
#define ILensFlareSceneNodeHPP
//-----------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//-----------------------------------------------------------------------------
namespace my{
namespace scn{
//-----------------------------------------------------------------------------

struct SLens
{
	SLens()
		: texture(0), spaceConst(0)
	{}

	SLens(const SLens& lens)
		: texture(0), spaceConst(0) 
	{
		set(lens);
	}

	SLens(vid::ITexture *tex, f32 space)
		: texture(0), spaceConst(0) 
	{
		setTexture(tex);
		spaceConst = space;
	}

	~SLens()
	{
		SAFE_DROP(texture);
	}

    SLens& operator = (const SLens &lens)
    {
		set(lens);
		return *this;
    }

	void set(const SLens &lens)
    {
		setTexture(lens.texture);
		spaceConst = lens.spaceConst;
    }

	vid::ITexture* getTexture() const
	{ return texture; }

	void setTexture(vid::ITexture *tex)
	{
		SAFE_DROP(texture);
		texture = tex;
		SAFE_GRAB(texture);
	}

	f32 spaceConst;

private:

	vid::ITexture* texture;
};

//-----------------------------------------------------------------------------

//! interfase for using Lens Flare scene node
class __MY_SCN_LIB_API__ ILensFlareSceneNode : public ISceneNode
{	
public:
	//! constructor
	ILensFlareSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id) 
		: ISceneNode(type, parent, id) {}

	//! destructor
	virtual ~ILensFlareSceneNode()  {}

	//! getting lens count
	virtual u32 getLensCount() = 0;

	//! setting lens, return true on success
	virtual bool setLens(u32 idx, const SLens &lens) = 0;
	virtual bool setLens(u32 idx, vid::ITexture *tex, f32 space) = 0;

	//! adding lens
	virtual void addLens(const SLens &lens) = 0;
	virtual void addLens(vid::ITexture *tex, f32 space) = 0;

	//! getting lens
	virtual const SLens& getLens(u32 idx) const = 0;

	//! removing lens
	virtual void removeLens(u32 idx) = 0;

	//! clearing all lens
	virtual void clearLens() = 0;

	//! setting corona
	virtual void setCorona(vid::ITexture *t) = 0;

	//! getting corona
	virtual vid::ITexture* getCorona() const = 0;

	//! setting general lens color
	virtual void setColor(const img::SColor &color) = 0;

	//! getting general lens color
	virtual const img::SColor& getColor() = 0;

	//! setting etalon screen size (in which scale of the lens is 1.0)
	virtual void setEtalonScreenSize(const core::dimension2di &scrSize) = 0;
	virtual void setEtalonScreenSize(u32 scrWidth, u32 scrHeight) = 0;

	//! getting etalon screen size (in which scale of the lens is 1.0)
	virtual const core::dimension2di& getEtalonScreenSize() const = 0;

	//! return screen position of this lensflare
	virtual const core::position2di& getScreenPosition() const = 0;
};

//-----------------------------------------------------------------------------
} // namespace scn
} // namespace my
//-----------------------------------------------------------------------------

#endif // #ifndef ILensFlareSceneNodeHPP