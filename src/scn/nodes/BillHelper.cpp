 //|-------------------------------------------------------------------------
//| File:        BillHelper.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "BillHelper.h"

#include <vid/IVideoDriver.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct CTreeNode
{
public:
	CTreeNode(const core::recti & bound)
		: m_LNode(NULL), m_RNode(NULL), m_Data(NULL), m_Bound(bound) {}
	virtual ~CTreeNode()
	{
		delete m_LNode;
		delete m_RNode;
	}

	bool insert(void * data,
		const core::recti & bound, core::recti * bound_out)
	{
		u32 bw = m_Bound.getWidth(), bh = m_Bound.getHeight();
		u32 _bw = bound.getWidth(), _bh = bound.getHeight();

		if (bw < _bw || bh < _bh)
			return false;

		if (!m_Data)
		{
			m_Data = data;

			bound_out->set(m_Bound.UpperLeftCorner.X,
				m_Bound.UpperLeftCorner.Y,
				m_Bound.UpperLeftCorner.X + _bw,
				m_Bound.UpperLeftCorner.Y + _bh);

			core::recti r_bound, l_bound;
			//if (bw < bh)
			{
				r_bound.UpperLeftCorner.set(m_Bound.UpperLeftCorner.X + _bw + 1,
					m_Bound.UpperLeftCorner.Y);
				r_bound.LowerRightCorner.set(m_Bound.LowerRightCorner.X,
					m_Bound.UpperLeftCorner.Y + _bh);
				l_bound.UpperLeftCorner.set(m_Bound.UpperLeftCorner.X,
					m_Bound.UpperLeftCorner.Y + _bh + 1);
				l_bound.LowerRightCorner = m_Bound.LowerRightCorner;
			}
			//else
			/*
			{
				r_bound.UpperLeftCorner.set(m_Bound.UpperLeftCorner.X + _bw,
					m_Bound.UpperLeftCorner.Y);
				r_bound.LowerRightCorner = m_Bound.LowerRightCorner;
				l_bound.UpperLeftCorner.set(m_Bound.UpperLeftCorner.X,
					m_Bound.UpperLeftCorner.Y + _bh);
				l_bound.LowerRightCorner.set(m_Bound.UpperLeftCorner.X + _bw,
					m_Bound.LowerRightCorner.Y );
			}*/

			if (l_bound.LowerRightCorner.X >= l_bound.UpperLeftCorner.X &&
				l_bound.LowerRightCorner.Y >= l_bound.UpperLeftCorner.Y)
				m_LNode = new CTreeNode(l_bound);
			if (r_bound.LowerRightCorner.X >= r_bound.UpperLeftCorner.X &&
				r_bound.LowerRightCorner.Y >= r_bound.UpperLeftCorner.Y)			
				m_RNode = new CTreeNode(r_bound);

			return true;
		}
		return (m_RNode ? m_RNode->insert(data, bound, bound_out) : false) ||
			(m_LNode ? m_LNode->insert(data, bound, bound_out) : false);
	}
private:
	CTreeNode *	m_LNode, * m_RNode;
	core::recti m_Bound;
	void *		m_Data;
};

//---------------------------------------------------------------------------

class CTree
{
public:
	CTree(const core::recti & bound)
	{ m_Root = new CTreeNode(bound); }
	virtual ~CTree()
	{ delete m_Root; }

	bool insert(void * data,
		const core::recti & bound, core::recti * bound_out)
	{ return m_Root->insert(data, bound, bound_out); }
private:
	CTreeNode * m_Root;
};

//---------------------------------------------------------------------------

struct SBillAnimationFrameExt
{
	SBillAnimationFrameExt() : frame(NULL), image(NULL) {}

	SBillAnimationFrameExt(SBillAnimationFrame * f, img::IImage * img)
		: frame(f), image(img) {}

	SBillAnimationFrame * frame;
	img::IImage * image;

	bool operator < (const SBillAnimationFrameExt & other)
	{
		if (frame->TCoordsPix != other.frame->TCoordsPix)
			return frame->TCoordsPix.getHeight() >
				other.frame->TCoordsPix.getHeight();
		return false;
	}
};

//---------------------------------------------------------------------------

bool compressBillfoardsTextures(bool		compress,
	const c8 *								base_tex_name,
	core::dimension2di &					max_tex_size,
	core::array < SBillTexture > &			bill_tex,
	core::array < SBillTexture > *			bill_tex_out,
	core::array < SBillAnimationFrame > *	bill_fr_out)
{
	u32 tcnt = bill_tex.size();

	bill_tex_out->set_used(0);
	bill_fr_out->set_used(0);

	if (compress)
	{
		core::array < SBillAnimationFrameExt > bill_fr;

		for (u32 t = 0; t < tcnt; t++)
		{
			core::array < SBillAnimationFrame > & frames = bill_tex[t].frames;
			u32 frcnt = frames.size();
			for (u32 f = 0; f < frcnt; f++)
				bill_fr.push_back(SBillAnimationFrameExt(&frames[f], bill_tex[t].image));		
		}
		bill_fr.sort();

		core::array < core::array < SBillAnimationFrameExt > > bill_fr_tex_map;

		while (bill_fr.size())
		{
			u32 frcnt = bill_fr.size();
			f32 tex_area = 0;
			for (u32 f = 0; f < frcnt; f++)
			{
				tex_area += (bill_fr[f].frame->TCoordsPix.getWidth() + 1) *
					(bill_fr[f].frame->TCoordsPix.getHeight() + 1);
			}

			f32 tex_avg_res = (f32)core::math::Sqrt(tex_area);

			s32 f0w = bill_fr[0].frame->TCoordsPix.getWidth() + 1;
			s32 f0h = bill_fr[0].frame->TCoordsPix.getHeight() + 1;

			CHECK_MIN_RANGE(tex_avg_res, f0w);
			CHECK_MIN_RANGE(tex_avg_res, f0h);

			s32 tex_w = core::math::GetNearestPowerOfTwo((s32)tex_avg_res);
			CHECK_MAX_RANGE(tex_w, max_tex_size.Width);

			s32 tex_h = core::math::GetNearestPowerOfTwo((s32)(tex_area / tex_w + 0.5f));
			if (tex_h < f0h)
				tex_h = core::math::GetNearestPowerOfTwo(f0h);
			CHECK_MAX_RANGE(tex_h, max_tex_size.Height);
			if (tex_h > 256 && f0h < tex_h / 2 &&
				tex_w * tex_h * 0.9 > tex_avg_res * tex_avg_res)
				tex_h /= 2;

			core::stringc texname;
			texname.sprintf("%s_%.3d.dds", base_tex_name, (*bill_tex_out).size());

			(*bill_tex_out).push_back(SBillTexture(texname.c_str(), NULL));
			bill_fr_tex_map.push_back(core::array < SBillAnimationFrameExt >());

			(*bill_tex_out).get_last().createImage(
				core::dimension2di(tex_w, tex_h), img::ECF_A8R8G8B8);

			CTree tree(core::recti(0, 0, tex_w - 1, tex_h - 1));
			while (tex_w > 1 && tex_h > 1 && bill_fr.size())
			{
				SBillAnimationFrame fr = *bill_fr[0].frame;
				if (tree.insert((void*)true, bill_fr[0].frame->TCoordsPix, &fr.TCoordsPix))
				{
					fr.TCoords.UpperLeftCorner.X = (f32)fr.TCoordsPix.UpperLeftCorner.X / f32(tex_w);
					fr.TCoords.UpperLeftCorner.Y = (f32)fr.TCoordsPix.UpperLeftCorner.Y / f32(tex_h);
					fr.TCoords.LowerRightCorner.X = (f32)fr.TCoordsPix.LowerRightCorner.X / f32(tex_w);
					fr.TCoords.LowerRightCorner.Y = (f32)fr.TCoordsPix.LowerRightCorner.Y / f32(tex_h);

					fr.TOffset.X = (f32)fr.TOffsetPix.X / f32(tex_w);
					fr.TOffset.Y = (f32)fr.TOffsetPix.Y / f32(tex_h);

					SBillTexture & bt = bill_tex_out->get_last();
					bt.frames.push_back(fr);
					bill_fr_tex_map.get_last().push_back(bill_fr[0]);
					bill_fr.erase(0);
				}
				else
					break;
			}
		}
		
		tcnt = (*bill_tex_out).size();
		for (u32 t = 0; t < tcnt; t++)
		{
			u32 frcnt = (*bill_tex_out)[t].frames.size();
			img::IImage * img_dst = (*bill_tex_out)[t].image;
			if (!img_dst)
				continue;
			for (u32 f = 0; f < frcnt; f++)
			{
				SBillAnimationFrame & fr_dst = (*bill_tex_out)[t].frames[f];
				SBillAnimationFrame * fr_src = bill_fr_tex_map[t][f].frame;
				img::IImage * img_src = bill_fr_tex_map[t][f].image;
				if (!img_src)
					continue;
				img_src->copyTo(img_dst, fr_dst.TCoordsPix.UpperLeftCorner,
					fr_src->TCoordsPix);
			}
		}
	}
	else
		(*bill_tex_out) = bill_tex;

	tcnt = (*bill_tex_out).size();
	for (u32 t = 0; t < tcnt; t++)
	{
		u32 frcnt = (*bill_tex_out)[t].frames.size();
		img::IImage * image = (*bill_tex_out)[t].image;
		if (!image)
			continue;

		vid::ITexture * tex = VIDEO_DRIVER.addTexture(
			(*bill_tex_out)[t].fname.c_str(), image);

		for (u32 f = 0; f < frcnt; f++)
		{
			SBillAnimationFrame & frame = (*bill_tex_out)[t].frames[f];
			frame.Texture = tex;
			bill_fr_out->push_back(frame);
		}
	}
	bill_fr_out->sort();

	return true;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------



 