//|-------------------------------------------------------------------------
//| File:        SGameCameraStyle.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SGameCameraStyleHPP
#define SGameCameraStyleHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SCameraState
{
	SCameraState() {}
	SCameraState(
		const core::vector3df &lookat_transl, const core::vector3df &pos_transl,
		f32 fov, f32 znear, f32 zfar, const core::dimension2df &viewvol	
		) 
	: LookatTranslation(lookat_transl), PositionTranslation(pos_transl), 
	FOVDegrees(fov), NearClip(znear), FarClip(zfar), ViewVolume(viewvol)	
	{}   

	SCameraState getInterpolated(const SCameraState& other, f32 d) const
	{
		f32 inv = 1.0f - d;        
		
		return SCameraState (  
			LookatTranslation.getInterpolated(other.LookatTranslation, d),
			PositionTranslation.getInterpolated(other.PositionTranslation, d),
			other.FOVDegrees*inv + FOVDegrees*d,
			other.NearClip*inv + NearClip*d, other.FarClip*inv + FarClip*d,
			core::dimension2df(
				other.ViewVolume.Width*inv + ViewVolume.Width*d,
				other.ViewVolume.Height*inv + ViewVolume.Height*d
				)
			); 
	}

	core::vector3df LookatTranslation, PositionTranslation;
	f32 FOVDegrees; 
	f32 NearClip, FarClip;
	core::dimension2df ViewVolume;  
};

//----------------------------------------------------------------------------

struct SCameraZoom
{
	SCameraZoom()
	{
		Value = 500.0f;
		RightTop = LeftTop = SCameraState(
			core::vector3df(275.0f, 20.0f, 0.0f), core::vector3df(-80.0f, 20.0f, 0.0f),
			80, 1, 3500, core::dimension2df(5000, 5000)
			);
		RightBottom = LeftBottom = SCameraState(
			core::vector3df(150.0f, 20.0f, 0.0f), core::vector3df(40.0f, 20.0f, 0.0f),
			80, 1, 3500, core::dimension2df(5000, 5000)
			); 
		Center = SCameraState(
			core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f),
			80, 1, 3500, core::dimension2df(5000, 5000)
			);
		UseCenter = true;
	}

	f32 Value; 
	SCameraState Center, LeftTop, RightTop, LeftBottom, RightBottom;  
	bool UseCenter;
};

//----------------------------------------------------------------------------

struct SGameCameraStyle
{
	core::stringc Name;			
	f32 RotationSpeed, MoveSpeed, ZoomSpeed; 
	core::vector3df UpVector;
	SCameraZoom FirstZoom, SecondZoom;     
	scn::E_CAMERA_PROJECTION_TYPE ProjType; 
	f32 MinZoomValue, MaxZoomValue;
	f32 MouseSenseX, MouseSenseY;

	SGameCameraStyle::SGameCameraStyle()
	{
		Name = "DefaultStyle";
		ProjType = scn::ECPT_PERSPECTIVE;
		RotationSpeed=9.0f;
		MoveSpeed=3.5f;
		ZoomSpeed=10.0f;
		UpVector.set(0,1,0);		
		MinZoomValue=300;
		MaxZoomValue=900;
		setCursorLockRect(core::rectf(0,0,1,1));
		setCursorActiveRectRel(core::rectf(0.5f,0.5f,0.5f,0.5f));
		setCursorActiveRectRel(core::rectf(0.5f,0.5f,0.5f,0.5f));
		MouseSenseX = MouseSenseY = 1.0f;
	}

	bool operator < (const SGameCameraStyle& other) const
	{
		return Name<other.Name;
	}

#if MY_COMPILER != MY_COMPILER_BORL

	SCameraState getState(
		const core::position2df& cursor_pos, f32 zoom_value,
		f32 centerInterpCoeff = 1.0f
		) const
	{        
		CHECK_RANGE(zoom_value, FirstZoom.Value, SecondZoom.Value);

		core::position2df active_center = CursorActiveRect.getCenter();

		f32 dr = core::vector2df(
			cursor_pos.X - active_center.X,
			cursor_pos.Y - active_center.Y).getLength() / 0.5f;
		f32 dh = f32(cursor_pos.Y-CursorLockRect.Top) / LockHeight;
		f32 dw = f32(cursor_pos.X-CursorLockRect.Left) / LockWidth;
		f32 dzoom = f32(zoom_value-FirstZoom.Value) /
			(SecondZoom.Value-FirstZoom.Value);

		CHECK_RANGE(dr,    0.0f, 1.0f);
		CHECK_RANGE(dh,    0.0f, 1.0f);
		CHECK_RANGE(dw,    0.0f, 1.0f);
		CHECK_RANGE(dzoom, 0.0f, 1.0f);

		SCameraState left_interpolated_min =
			FirstZoom.LeftBottom.getInterpolated(FirstZoom.LeftTop,  dh);
		SCameraState right_interpolated_min =
			FirstZoom.RightBottom.getInterpolated(FirstZoom.RightTop, dh); 		
		SCameraState left_interpolated_max =
			SecondZoom.LeftBottom.getInterpolated(SecondZoom.LeftTop,  dh);
		SCameraState right_interpolated_max =
			SecondZoom.RightBottom.getInterpolated(SecondZoom.RightTop, dh);

		SCameraState min_interpolated =
			right_interpolated_min.getInterpolated(left_interpolated_min, dw);
		SCameraState max_interpolated =
			right_interpolated_max.getInterpolated(left_interpolated_max, dw);

		SCameraState min_interpolated_center = min_interpolated;
		SCameraState max_interpolated_center = max_interpolated;
		if (FirstZoom.UseCenter) min_interpolated_center =
			min_interpolated_center.getInterpolated(FirstZoom.Center, dr);
		if (SecondZoom.UseCenter) max_interpolated_center =
			max_interpolated_center.getInterpolated(SecondZoom.Center, dr);
		
		min_interpolated =
			min_interpolated.getInterpolated(min_interpolated_center, centerInterpCoeff);
		max_interpolated =
			max_interpolated.getInterpolated(max_interpolated_center, centerInterpCoeff);

		return max_interpolated.getInterpolated(min_interpolated, dzoom);
	}

#endif // #if MY_COMPILER != MY_COMPILER_BORL

	const core::rectf& getCursorActiveRect() const
	{
		return CursorActiveRect; 
	}

	const core::rectf& getCursorLockRect() const
	{
		return CursorLockRect; 
	}

	void setCursorActiveRectRel(const core::rectf &cursor_dead_rect)
	{
		CursorActiveRectRel = cursor_dead_rect;
		_update();
	}

	void setCursorLockRect(const core::rectf &cursor_lock_rect)
	{
		CursorLockRect = cursor_lock_rect;
		LockWidth  = CursorLockRect.getWidth();
		LockHeight = CursorLockRect.getHeight();
		_update();
	}

private:

	void _update()
	{
#if MY_COMPILER != MY_COMPILER_BORL
		CursorActiveRect.Left	= CursorLockRect.Left	+ CursorActiveRectRel.Left	* LockWidth;
		CursorActiveRect.Right	= CursorLockRect.Left	+ CursorActiveRectRel.Right	* LockWidth;
		CursorActiveRect.Top	= CursorLockRect.Top	+ CursorActiveRectRel.Top	* LockHeight;
		CursorActiveRect.Bottom	= CursorLockRect.Top	+ CursorActiveRectRel.Bottom* LockHeight;
#endif // #if MY_COMPILER != MY_COMPILER_BORL
	}

	core::rectf CursorActiveRectRel;
	core::rectf CursorLockRect, CursorActiveRect;
	f32 LockWidth, LockHeight;         
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef SGameCameraStyleHPP