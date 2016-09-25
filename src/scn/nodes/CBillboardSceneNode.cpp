//|-------------------------------------------------------------------------
//| File:        CBillboardSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CBillboardSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <scn/ICameraSceneNode.h>
#include <os/ITimer.h>
#include <dev/IDevice.h>
#include <scn/ICullingSystem.h>
#include <res/IResourceManager.h>

//--------------------------------------------------------------------------
namespace my {
namespace scn {
//--------------------------------------------------------------------------

u16 CBillboardSceneNode::m_Indices[] = { 0, 2, 1, 0, 3 , 2 };

//--------------------------------------------------------------------------

CBillboardSceneNode::CBillboardSceneNode(ISceneNode* parent, s32 id) 
	: IBillboardSceneNode(ESNT_BILLBOARD_SCENE_NODE, parent, id),
	m_AnimationTexturesDirectionOrderCCW(true), 
	m_StartTime(TIMER.getTime()), m_CurrentAnimation(0),
	m_FrameOffset(0.0f, 0.0f), m_FrameScale(1.0f, 1.0f),
	m_GL(VIDEO_DRIVER.getDriverFamily() == vid::EDF_OPENGL)
{
#if MY_DEBUG_MODE 
	IBillboardSceneNode::setClassName("CBillboardSceneNode");
#endif 

	// default size
	setSize(core::dimension2d<f32>(200,200));    

	m_Vertices[0].TCoords.set(1.0f, 1.0f);
	m_Vertices[1].TCoords.set(1.0f, 0.0f);
	m_Vertices[2].TCoords.set(0.0f, 0.0f);
	m_Vertices[3].TCoords.set(0.0f, 1.0f);

	for ( u32 i = 0; i < 4; i++ )
	{
		m_Vertices[i].Color.color = 0xffffffff;
		m_Vertices[i].Normal.set(0.0f, 1.0f, 0.0f);
	}
}

//--------------------------------------------------------------------------

CBillboardSceneNode::~CBillboardSceneNode()
{
	for (u32 a=0; a<m_Animators.size(); a++)
		SAFE_DROP(m_Animators[a]);
	m_Animators.clear();

	setAnimationCallback(NULL);
}

//---------------------------------------------------------------------------

void CBillboardSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{		
		m_Culled = !IsVisible || CULLING_SYSTEM.isCull(this);
		ISceneNode::OnCheckCulling();
	}
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::OnPreRender(u32 timeMs)
{
	scn::ISceneManager &smgr = SCENE_MANAGER;

	ICameraSceneNode* camera = smgr.getActiveCamera();
	if (!camera)
		return;

	vid::IVideoDriver & driver = VIDEO_DRIVER;

	// make billboard look to Camera

	core::vector3df campos = camera->getAbsolutePosition();
    core::vector3df target = getAbsolutePosition();

	_animate(campos);

	vid::S3DVertex1TCoordsColoured * v = m_Vertices;
	
	getCornersCoordinatesForView(
		campos, target, v[0].Pos, v[1].Pos, v[2].Pos, v[3].Pos );
	driver.registerGeometryForRendering(
		vid::ERP_3D_TRANSP_2ND_PASS,
		core::matrix4(), NULL, NULL,
		m_Material,
		v, 4, vid::EVT_1TCOORDS_COLOURED,
		m_Indices, 6, EIT_16_BIT,
		vid::EDPT_TRIANGLE_LIST );

	if (isDebugDataVisible())
	{
		vid::SMaterial m;
		m.getPass(0).setFlag(vid::EMF_ZWRITE_ENABLE, false);
		driver.registerGeometryForRendering(
			vid::ERP_3D_TRANSP_1ST_PASS,
			core::matrix4(), NULL, NULL,
			m,
			v, 4, vid::EVT_1TCOORDS_COLOURED,
			m_Indices, 6, EIT_16_BIT,
			vid::EDPT_TRIANGLE_LIST);

		driver.register3DBoxForRendering(
			vid::ERP_3D_SOLID_PASS, getAbsoluteTransformation(),
			getBoundingBox(), 0xffffffff,
			true);
	}

    ISceneNode::OnPreRender(timeMs);
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::setSize(const core::dimension2d<f32>& size)
{
	m_Params.Size = size;    
	f32 lsize = size.Height>size.Width ? size.Height/2:size.Width/2;    
	m_BBox = core::aabbox3d<f32>(-lsize,-lsize,-lsize,lsize,+lsize,+lsize);
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::setColor(const img::SColor& color) 
{
	if (m_GL)
	{
		for ( u32 i = 0; i < 4; i++ )
			m_Vertices[i].Color = color.toOpenGLColor();
	}
	else
	{
		for ( u32 i = 0; i < 4; i++ )
			m_Vertices[i].Color = color;
	}
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::setBillboardParameters(const SBillboardParams & bill_params)
{
	m_Params = bill_params;

	if (!m_Params.FlipWidth && !m_Params.FlipHeight)
	{
		m_Vertices[0].TCoords.set(1.0f, 1.0f);
		m_Vertices[1].TCoords.set(1.0f, 0.0f);
		m_Vertices[2].TCoords.set(0.0f, 0.0f);
		m_Vertices[3].TCoords.set(0.0f, 1.0f);
	}
	else 
	if (m_Params.FlipWidth && !m_Params.FlipHeight)
	{
		m_Vertices[0].TCoords.set(0.0f, 1.0f);
		m_Vertices[1].TCoords.set(0.0f, 0.0f);
		m_Vertices[2].TCoords.set(1.0f, 0.0f);
		m_Vertices[3].TCoords.set(1.0f, 1.0f);
	}
	else
	if (m_Params.FlipHeight && !m_Params.FlipWidth)
	{
		m_Vertices[0].TCoords.set(1.0f, 0.0f);
		m_Vertices[1].TCoords.set(1.0f, 1.0f);
		m_Vertices[2].TCoords.set(0.0f, 1.0f);
		m_Vertices[3].TCoords.set(0.0f, 0.0f);
	}
	else 
	{
		m_Vertices[0].TCoords.set(0.0f, 0.0f);
		m_Vertices[1].TCoords.set(0.0f, 1.0f);
		m_Vertices[2].TCoords.set(1.0f, 1.0f);
		m_Vertices[3].TCoords.set(1.0f, 0.0f);
	}
}

//--------------------------------------------------------------------------

bool CBillboardSceneNode::setCurrentAnimationProgress(f32 progress)
{
	if (m_CurrentAnimation >= m_Animators.size() || m_Animators[m_CurrentAnimation])
		return false;
	CSceneNodeAnimatorTexture *anim = m_Animators[m_CurrentAnimation];
	u32 dir_cnt = anim->getDirectionSetsCount();
	if (dir_cnt>0)
	{
		const SAnimationFrameParameters &anim_par = 
			anim->getTextureAnimatorParameters();
		const core::array<SAnimationFrame> &dir_set = 
			anim->getFramesDirectionSet(0);
			
		u32 timeMs = DEVICE.getDeviceTime();

		m_StartTime = timeMs -
			s32(progress * f32(dir_set.size()-1) * (f32)anim_par.TimePerFrame);

		anim->setAnimationStartTime(m_StartTime);
	}
	return true;
}

//--------------------------------------------------------------------------

f32 CBillboardSceneNode::getCurrentAnimationProgress()
{
	f32 progress = 0.0f;
	if (m_CurrentAnimation <m_Animators.size() &&
			m_Animators[m_CurrentAnimation])
	{
		CSceneNodeAnimatorTexture *anim =
			m_Animators[m_CurrentAnimation];
		s32 dir_cnt = anim->getDirectionSetsCount();
		if (dir_cnt>0)
		{
			const core::array<SAnimationFrame> &dir_set = 
				anim->getFramesDirectionSet(0);
			progress = (f32)(anim->getCurrentFrameIndex() + 1) /
				(f32)dir_set.size();
		}
		CHECK_RANGE(progress, 0.0f, 1.0f);
	}
	return progress;
}

//--------------------------------------------------------------------------

bool CBillboardSceneNode::setCurrentAnimation(u32 anim, f32 delay)
{
	m_CurrentAnimation = anim;
	return true;
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::setAnimationDirectionSetFrames( 
	u32 animation_idx, u32 direction_idx,
	const core::array <SAnimationFrame> &frames,
	const SAnimationFrameParameters &params)
{
	if (!m_Params.FrameResolution.Width
			&& !m_Params.FrameResolution.Height
			&& frames.size() && frames[0].Texture)
	{
		core::dimension2di texsz = frames[0].Texture->getSize();
		m_Params.FrameResolution.Width = texsz.Width * frames[0].TCoords.getWidth();
		m_Params.FrameResolution.Height = texsz.Height * frames[0].TCoords.getHeight();
	}

	if (animation_idx >= m_Animators.size())
	{
		for (u32 a = m_Animators.size(); a <= animation_idx; a++)
		{
			m_Animators.push_back(a == animation_idx ?
				new CSceneNodeAnimatorTexture(m_StartTime, params) : NULL);
		}
	}
	else if (!m_Animators[animation_idx])	
		m_Animators[animation_idx] = new CSceneNodeAnimatorTexture(m_StartTime, params);
	
	m_Animators[animation_idx]->setFramesDirectionSet(direction_idx, frames);		

	setAnimationFramesDirectionOrderCCW(m_AnimationTexturesDirectionOrderCCW);
}

//---------------------------------------------------------------------------

const core::array<SAnimationFrame>& CBillboardSceneNode::getAnimationDirectionSetFrames(
	u32 animation_idx, u32 direction_idx)
{
	if (animation_idx >= m_Animators.size() || !m_Animators[animation_idx])
		return m_EmptyTextures;
	return m_Animators[animation_idx]->getFramesDirectionSet(direction_idx);
}

//---------------------------------------------------------------------------

const SAnimationFrame& CBillboardSceneNode::getAnimationDirectionSetFrame(
	u32 animation_idx, u32 direction_idx, u32 frame_idx)
{
	u32 fr_cnt = getAnimationDirectionSetFramesCount(animation_idx, direction_idx);
	if (fr_cnt <= frame_idx)
		return m_EmptyFrame;
	const core::array<SAnimationFrame> &frames =
		m_Animators[animation_idx]->getFramesDirectionSet(direction_idx);
	return frames[frame_idx];
}

//---------------------------------------------------------------------------

const SAnimationFrameParameters& CBillboardSceneNode::getAnimationFramesParameters(
	u32 animation_idx)
{
	if (animation_idx >= m_Animators.size() || !m_Animators[animation_idx])
		return m_EmptyTextureAnimatorParameters;
	return m_Animators[animation_idx]->getTextureAnimatorParameters();
}

//---------------------------------------------------------------------------

u32 CBillboardSceneNode::getAnimationDirectionSetsCount(
	u32 animation_idx)
{
	if (animation_idx >= m_Animators.size()
			|| !m_Animators[animation_idx])
		return 0;
	return m_Animators[animation_idx]->getDirectionSetsCount();
}

//---------------------------------------------------------------------------

u32 CBillboardSceneNode::getAnimationDirectionSetFramesCount(
	u32 animation_idx, u32 direction_idx)
{
	if (animation_idx >= m_Animators.size()
			|| !m_Animators[animation_idx]
			|| m_Animators[animation_idx]->getDirectionSetsCount() <= direction_idx)
		return 0;
	return m_Animators[animation_idx]->getDirectionSetFramesCount(direction_idx);
}


//--------------------------------------------------------------------------

void CBillboardSceneNode::setRectColor(
	img::SColor &c1, img::SColor &c2, img::SColor &c3, img::SColor &c4)
{
	if (m_GL)
	{
		m_Vertices[0].Color = c1.toOpenGLColor();
		m_Vertices[1].Color = c2.toOpenGLColor();
		m_Vertices[2].Color = c3.toOpenGLColor();
		m_Vertices[3].Color = c4.toOpenGLColor();
	}
	else
	{	
		m_Vertices[0].Color = c1;
		m_Vertices[1].Color = c2;
		m_Vertices[2].Color = c3;
		m_Vertices[3].Color = c4;
	}
}

//--------------------------------------------------------------------------

vid::ITexture* CBillboardSceneNode::getCurrentAnimationTextureForCameraPosition(
	const core::vector3df &cam_pos)
{
	_animate(cam_pos);
	return m_Material.getPass(0).Layers[0].getTexture();
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::getCornersCoordinatesForView(
	core::vector3df  view_pos, core::vector3df view_tgt,
	core::vector3df& right_down_corner,
	core::vector3df& right_up_corner,
	core::vector3df& left_up_corner,
	core::vector3df& left_down_corner)
{
    core::vector3df pos = getAbsolutePosition();
    core::vector3df up = m_Params.UpAxis;//(0.0f, 1.0f, 0.0f);	

	core::vector3df vertical, horizontal, view;

	if (m_Params.AxisAligmentType == EAAT_NONE ||
		m_Params.AlignAxis == core::vector3df(0,0,0))
	{
		view  = view_tgt - view_pos;
		view.normalize();

		horizontal = up.getCrossProduct(view);
		horizontal.normalize();

		core::vector3df v1 = view.getCrossProduct(horizontal);
		v1.normalize();		

		vertical= v1;
	}
	else
	{
		core::vector3df v2 = m_Params.AlignAxis;

		if (m_Params.AxisAligmentType == EAAT_BLEND_CAM_TGT || 
			m_Params.AxisAligmentType == EAAT_STRETCH_CAM_TGT ||
			m_Params.AxisAligmentType == EAAT_SHARP_CAM_TGT)
			view   = view_tgt - view_pos;
		else
			view   = pos - view_pos;

		view.normalize();

		horizontal = up.getCrossProduct(view);
		horizontal.normalize();

		core::vector3df v1 = view.getCrossProduct(horizontal);
		v1.normalize();	

		if (m_Params.AxisAligmentType == EAAT_BLEND_CAM_TGT || 
			m_Params.AxisAligmentType == EAAT_BLEND_CAM_POS)
		{
			vertical= (v1+v2)*0.5f;
			horizontal = vertical.getCrossProduct(view);
			horizontal.normalize();
		}
		else if (
			m_Params.AxisAligmentType == EAAT_STRETCH_CAM_TGT || 
			m_Params.AxisAligmentType == EAAT_STRETCH_CAM_POS)
		{
			vertical= v2 - pos;
			horizontal = vertical.getCrossProduct(view);
			horizontal.normalize();

			m_Params.Size.Height = vertical.getLength();
		}
		else if (
			m_Params.AxisAligmentType == EAAT_SHARP_CAM_TGT || 
			m_Params.AxisAligmentType == EAAT_SHARP_CAM_POS)		
		{
			vertical= v2;
			horizontal = vertical.getCrossProduct(view);
			horizontal.normalize();
		}
		else if (m_Params.AxisAligmentType == EAAT_NORMAL_AXIS)
		{
			horizontal = m_Params.AlignAxis.getCrossProduct(up);
			horizontal.normalize();
			vertical = up;
		}
	}	

	vertical.normalize();

	m_Normal = (vertical).getCrossProduct(horizontal);
    m_Normal.normalize();

	f32 width  = RelativeScale.X * m_Params.Size.Width;
	f32 height = RelativeScale.Y * m_Params.Size.Height;
	f32 width_scl  = width * m_FrameScale.X;
	f32 height_scl = height * m_FrameScale.Y;

	core::vector3df hoffs = horizontal * 0.5f * width;
	core::vector3df voffs = vertical;
	core::vector3df hoffs_scl = horizontal * 0.5f * width_scl;
	core::vector3df voffs_scl = vertical;

	core::vector3df frame_offs =
		horizontal * (RelativeScale.X * m_Params.Size.Width * m_FrameOffset.X) -
		vertical * (RelativeScale.Y * m_Params.Size.Height * m_FrameOffset.Y);

	core::vector3df back_offs;

	if (m_Params.AxisAligmentType == EAAT_STRETCH_CAM_TGT ||
		m_Params.AxisAligmentType == EAAT_STRETCH_CAM_POS)
	{
		voffs *= height;
		voffs_scl *= height_scl;
		right_down_corner = hoffs_scl;
		right_up_corner   = voffs_scl + hoffs_scl;
		left_up_corner    = voffs_scl - hoffs_scl;
		left_down_corner  = hoffs_scl * (-1.0f);

		back_offs = left_up_corner - (voffs - hoffs);
	}
	else
	{
		voffs *= 0.5f * height;
		voffs_scl *= 0.5f * height_scl;
		right_down_corner = hoffs_scl - voffs_scl;
		right_up_corner   = hoffs_scl + voffs_scl;
		left_up_corner    = voffs_scl           - hoffs_scl;
		left_down_corner  = voffs_scl * (-1.0f) - hoffs_scl;

		
	}
	back_offs = (voffs - hoffs) - left_up_corner;
	pos += back_offs + frame_offs;
	right_down_corner += pos;
	right_up_corner   += pos;
	left_up_corner    += pos;
	left_down_corner  += pos;
}

//---------------------------------------------------------------------------

void CBillboardSceneNode::_animate(core::vector3df cam_pos)
{
	u32 timeMs = DEVICE.getDeviceTime();

	m_CurrentAnimation = getCurrentAnimation();

	if (m_CurrentAnimation < m_Animators.size() &&
			m_Animators[m_CurrentAnimation])
	{
		CSceneNodeAnimatorTexture* animator = m_Animators[m_CurrentAnimation];

		s32 texset_idx_old = animator->getActiveFramesDirectionSetIndex();
		s32 texset_idx_new = 0;
		f32 texset_idx_old_f = texset_idx_old;
		f32 texset_idx_new_f = 0;

		s32 texset_count = animator->getDirectionSetsCount();

		if (texset_count > 1)
		{
			core::vector3df node_pos = getAbsolutePosition();			
			core::vector3df node_look_at;
				
			getAbsoluteTransformation().transformVect(
				core::vector3df(100,0,0), node_look_at );

			node_pos.Y = 0;
			cam_pos.Y = 0;
			node_look_at.Y = 0;

			core::vector3df node_look = node_look_at - node_pos;
			core::vector3df node_cam_dir = cam_pos - node_pos;

			f32 angle = node_look.getAngleDeg(node_cam_dir);

			core::triangle3df tri( node_look_at, cam_pos, node_pos );      
        
			if (!tri.isFrontFacing(core::vector3df(0,-1,0)))
			// CCW
				angle = 360-angle;

			texset_idx_new_f = f32(angle / 360.0f) * f32(texset_count);
			texset_idx_new   = texset_idx_new_f + 0.5f;

			// make histeresis
			if (!(texset_idx_new > texset_idx_old && texset_idx_new_f > (texset_idx_old_f + 0.4f))
					&& !(texset_idx_new < texset_idx_old && texset_idx_new_f < (texset_idx_old_f - 0.4f)))
				texset_idx_new = texset_idx_old;

			if (m_AnimationTexturesDirectionOrderCCW)
				texset_idx_new = texset_count-texset_idx_new;

			// final checks
			if (texset_idx_new>=texset_count)
				texset_idx_new = 0;
			else
				CHECK_MIN_RANGE(texset_idx_new, 0);	
		}
		animator->activateFramesDirectionSet(texset_idx_new);
		animator->animateNode(this, timeMs);

		const SAnimationFrame &frame = animator->getCurrentFrame();
		vid::ITexture         *tex   = frame.Texture;
		const core::vector2df &toffs = frame.TOffset;
		const core::rectf     &tc    = frame.TCoords;

		if (tex)
		{
			m_FrameScale.set(
				(tex->getSize().Width * tc.getWidth()) /
					(f32)m_Params.FrameResolution.Width,
				(tex->getSize().Height * tc.getHeight()) /
					(f32)m_Params.FrameResolution.Height);
			m_FrameOffset.set(
				(tex->getSize().Width * toffs.X) /
					(f32)m_Params.FrameResolution.Width,
				(tex->getSize().Height * toffs.Y) /
					(f32)m_Params.FrameResolution.Height);
		}
	}
}

//--------------------------------------------------------------------------

void CBillboardSceneNode::setMaterial(vid::SMaterial& mat)
{
	ISceneNode::setMaterial(mat);

	m_Vertices[0].Color = m_GL ?
		getMaterial(0).getPass(0).getDiffuseColor().toOpenGLColor() :
		getMaterial(0).getPass(0).getDiffuseColor();
	m_Vertices[1].Color.color = m_Vertices[2].Color.color =
		m_Vertices[3].Color.color = m_Vertices[0].Color.color;
}

//---------------------------------------------------------------------------

//!
bool CBillboardSceneNode::set(ISceneNode* other)
{
	if (!other || this == other
			|| other->getSceneNodeType()!=ESNT_BILLBOARD_SCENE_NODE)
		return false;

	IBillboardSceneNode *other_bill = SAFE_CAST_TO_BILLBOARD_SCENE_NODE(other);

	setBillboardParameters(other_bill->getBillboardParameters());

	u32 anim_cnt = other_bill->getAnimationsCount();
	for (u32 a = 0; a < anim_cnt; a++)	
	{
		SAnimationFrameParameters params =
			other_bill->getAnimationFramesParameters(a);
		u32 dir_cnt = other_bill->getAnimationDirectionSetsCount(a);
		for (u32 d = 0; d < dir_cnt; d++)	
		{
			const core::array<SAnimationFrame> &frames = 
				other_bill->getAnimationDirectionSetFrames(a, d);
			setAnimationDirectionSetFrames(a, d, frames, params);
		}
	}
	return true;
}

//---------------------------------------------------------------------------

const c8* CBillboardSceneNode::getSceneCorePropertiesXMLString()
{
	res::IResourceManager &resmgr = RESOURCE_MANAGER;
	core::stringc str;
	static core::stringc xmlstr;
	xmlstr = "";

	if (getMaterialsCount() == 1)
	{
		const c8* mat_xml = SCENE_MANAGER.getMaterialXMLText(getMaterial(0));
		xmlstr.append(mat_xml);
	}

	const SBillboardParams & billparams = getBillboardParameters();

	str.sprintf("<Size width=\"%.2f\" height=\"%.2f\" />\n",
		billparams.Size.Width, billparams.Size.Height);
	xmlstr.append(str);

	str.sprintf("<Flip width=\"%d\" height=\"%d\" />\n",
		billparams.FlipWidth, billparams.FlipHeight);
	xmlstr.append(str);

	str.sprintf("<UpAxis value=\"%.2f,%.2f,%.2f\" />\n",
		billparams.UpAxis.X, billparams.UpAxis.Y, billparams.UpAxis.Z);
	xmlstr.append(str);

	str.sprintf("<AxisAligment type=\"%s\" x=\"%.2f\" y=\"%.2f\" z=\"%.2f\" />\n",
		scn::AxisAligmentTypeStr[billparams.AxisAligmentType],
		billparams.AlignAxis.X, billparams.AlignAxis.Y, billparams.AlignAxis.Z);
	xmlstr.append(str);

	str.sprintf("<FrameResolution width=\"%d\" height=\"%d\" />\n",
		billparams.FrameResolution.Width, billparams.FrameResolution.Height);
	xmlstr.append(str);

	xmlstr.append("<SpriteAnimations dir_order_ccw=\"1\">\n");

	u32 anim_cnt = getAnimationsCount();

	core::array < SBillTexName > textures_fnames;

	for (u32 a = 0; a < anim_cnt; a++)
	{
		u32 dir_cnt = getAnimationDirectionSetsCount(a);
		for (u32 d = 0; d < dir_cnt; d++)	
		{
			const core::array<SAnimationFrame>& frames = 
				getAnimationDirectionSetFrames(a, d);
			u32 fr_cnt = frames.size();
			for (u32 f = 0; f < fr_cnt; f++)	
			{
				const c8 *fname =
					resmgr.getTextureFileNameRel(frames[f].Texture);
				if (textures_fnames.binary_search(
						SBillTexName(fname, NULL)) == -1)
					textures_fnames.push_back(
						SBillTexName(fname, frames[f].Texture));
			}
		}
	}
	textures_fnames.sort();

	u32 tcnt = textures_fnames.size();
	for (u32 t = 0; t < tcnt; t++)	
	{
		str.sprintf("<TextureFileName value=\"%s\" />\n",
			textures_fnames[t].fname.c_str());
		xmlstr.append(str);
	}
	
	for (u32 a = 0; a < anim_cnt; a++)
	{
		const SAnimationFrameParameters & params =
			getAnimationFramesParameters(a);

		str.sprintf("<AnimationSet index=\"%d\" loop=\"%d\" time_per_frame=\"%d\">\n",
			a, params.Loop, params.TimePerFrame);
		xmlstr.append(str);
		u32 dir_cnt = getAnimationDirectionSetsCount(a);
		for (u32 d = 0; d < dir_cnt; d++)	
		{
			const core::array<SAnimationFrame>& frames = 
				getAnimationDirectionSetFrames(a, d);

			xmlstr.append("<DirectionSet>\n");
			u32 fr_cnt = frames.size();
			for (u32 f = 0; f < fr_cnt; f++)	
			{
				core::dimension2di texsz = frames[f].Texture ?
					frames[f].Texture->getSize() : (core::dimension2di(0, 0));
				core::recti tci(
					frames[f].TCoords.UpperLeftCorner.X * (f32)texsz.Width + 0.5f,
					frames[f].TCoords.UpperLeftCorner.Y * (f32)texsz.Height + 0.5f,
					frames[f].TCoords.LowerRightCorner.X * (f32)texsz.Width + 0.5f,
					frames[f].TCoords.LowerRightCorner.Y * (f32)texsz.Height + 0.5f);
				core::vector2di toffsi(
					frames[f].TOffset.X * (f32)texsz.Width + 0.5f,
					frames[f].TOffset.Y * (f32)texsz.Height + 0.5f);

				str.sprintf("<Frame tex_index=\"%d\" tc_left=\"%d\" "
					"tc_top=\"%d\" tc_right=\"%d\" tc_bottom=\"%d\" "
					"toffs_x=\"%d\" toffs_y=\"%d\" />\n",
					textures_fnames.linear_search(frames[f].Texture),
					tci.UpperLeftCorner.X, tci.UpperLeftCorner.Y,
					tci.LowerRightCorner.X, tci.LowerRightCorner.Y,
					toffsi.X, toffsi.Y);
				xmlstr.append(str);
			}
			xmlstr.append("</DirectionSet>\n");
		}
		xmlstr.append("</AnimationSet>\n");
	}

	xmlstr.append("</SpriteAnimations>\n");

	return xmlstr.c_str();
}

//--------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//--------------------------------------------------------------------------
