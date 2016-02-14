 //|-------------------------------------------------------------------------
//| File:        BillHelper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef BillHelper_h
#define BillHelper_h
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SBillAnimationFrame : public SAnimationFrame
{
	SBillAnimationFrame()
		: animation(0), direction(0), index(0),
		TCoordsPix(0,0,0,0), TOffsetPix(0,0) {}
	SBillAnimationFrame(const SBillAnimationFrame & other)
		: SAnimationFrame(other.Texture, other.TCoords, other.TOffset),
		animation(other.animation), direction(other.direction),
		index(other.index), params(other.params),
		TCoordsPix(other.TCoordsPix), TOffsetPix(other.TOffsetPix) {}
	SBillAnimationFrame(u32 a, u32 d, u32 i, const SAnimationFrameParameters & p,
		vid::ITexture * t, const core::rectf &tc, const core::recti &tci)
		: SAnimationFrame(t, tc, core::vector2df(0.0f,0.0f)),
		animation(a), direction(d), index(i), params(p),
		TCoordsPix(tci), TOffsetPix(0,0) {}

	u32 animation, direction, index;
	SAnimationFrameParameters params;
	core::recti TCoordsPix;
	core::vector2di TOffsetPix;

	bool operator < (const SBillAnimationFrame & other)
	{
		if (animation != other.animation)
			return animation < other.animation;
		if (direction != other.direction)
			return direction < other.direction;
		return index < other.index;
	}
};

//---------------------------------------------------------------------------

struct SBillTexture
{
	SBillTexture() : loaded(false), image(NULL), rtt(NULL) {}
	SBillTexture(const c8 * _fname, vid::ITexture * _rtt )
		: fname(_fname), loaded(false), image(NULL), rtt(_rtt) {}
	SBillTexture(const c8 * _fname, vid::ITexture * _rtt, bool _loaded)
		: fname(_fname), loaded(_loaded), image(NULL), rtt(_rtt) {}
	SBillTexture(const SBillTexture & other)
	{ operator = (other); }
	virtual ~SBillTexture()
	{
		SAFE_DROP(image);
		SAFE_DROP(rtt);
	}
	SBillTexture & operator = (const SBillTexture & other)
	{
		fname = other.fname;
		loaded = other.loaded;

		SAFE_DROP(image);
		image = other.image;
		SAFE_GRAB(image);

		SAFE_DROP(rtt);
		rtt = other.rtt;
		SAFE_GRAB(rtt);

		frames = other.frames;

		return *this;
	}
	void createImage(const core::dimension2di & sz, img::E_COLOR_FORMAT fmt)
	{
		SAFE_DROP(image);
		image = IMAGE_LIBRARY.createEmptyImage(sz, fmt);
	}	
	void createImageFromRTT()
	{
		if (!rtt)
			return;
		SAFE_DROP(image);
		img::IImage * img_rtt = rtt->lock();
		if (img_rtt)
		{
			image = IMAGE_LIBRARY.createImageFromAnother(
				img_rtt, img_rtt->getColorFormat());
		}
		rtt->unlock();
		SAFE_DROP(rtt);
	}
	core::stringc fname;
	bool loaded;
	img::IImage * image;
	vid::ITexture * rtt;
	core::array < SBillAnimationFrame > frames;
};

//---------------------------------------------------------------------------

bool compressBillfoardsTextures(bool		compress,
	const c8 *								base_tex_name,
	core::dimension2di &					max_tex_size,
	core::array < SBillTexture > &			bill_tex,
	core::array < SBillTexture > *			bill_tex_out,
	core::array < SBillAnimationFrame > *	bill_fr_out);

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

