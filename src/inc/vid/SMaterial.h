//|-------------------------------------------------------------------------
//| File:        SMaterial.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SMaterialHPP
#define SMaterialHPP
//---------------------------------------------------------------------------

#include <img/SColor.h>
#include <vid/ITexture.h>
#include <vid/IGPUProgram.h>
#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

void setHashDirty	(SMaterial		*material);
void setHashFFPDirty(SMaterial		*material);
void setHashGPUDirty(SMaterial		*material);
void setHashDirty	(SRenderPass	*pass);
void setHashFFPDirty(SRenderPass	*pass);
void setHashGPUDirty(SRenderPass	*pass);
void setHashFFPDirty(STextureLayer	*layer);

//---------------------------------------------------------------------------

struct STexCoordModifier
{
	friend struct STextureLayer;

public:

	STexCoordModifier()
	{ defaultInit(ETCMT_ROTATE); }

	STexCoordModifier(const STexCoordModifier &other) : Layer(NULL)
	{ (*this).set(other, false); }

	STexCoordModifier(E_TEXCOORD_MOD_TYPE type) 
	{ defaultInit(type); }

	void defaultInit(E_TEXCOORD_MOD_TYPE type)
	{
		Layer = NULL;
		Type = type;
		Matrix00 = Matrix11 = 1.f;
		Matrix01 = Matrix10 = 0.f;
		Translate0 = Translate1 = 0.f;
		Scale0 = Scale1 = 1.f;
		Scroll0 = Scroll1 = 0.f;
		RotateSpeed = 0.f;
		Turbulent.defaultInit();
		HashFFPDirty = true;
	}

	void set(const STexCoordModifier &m, bool check_equal = true)
	{
		if (check_equal && this->operator == (m))
			return;
		Type = m.Type;
		Matrix00 = m.Matrix00;
		Matrix11 = m.Matrix11;
		Matrix01 = m.Matrix01;
		Matrix10 = m.Matrix10;
		Translate0 = m.Translate0;
		Translate1 = m.Translate1;
		Scale0 = m.Scale0;
		Scale1 = m.Scale1;
		Scroll0 = m.Scroll0;
		Scroll1 = m.Scroll1;
		RotateSpeed = m.RotateSpeed;
		Turbulent.set(m.Turbulent);

		setHashFFPDirty(m.HashFFPDirty || HashFFP != m.HashFFP);
		HashFFP = m.HashFFP;
	}

	//! operator =
    STexCoordModifier& operator = (const STexCoordModifier& ml)
    {
		set(ml);
		return *this;
    } 

    //! Compare operator <
    bool operator < (const STexCoordModifier& tcm) const
    {
		if (Type != tcm.Type)
			return Type < tcm.Type;

		if (Type == ETCMT_TRANSFORM)
		{
			for (u32 i = 0; i < 4; i++)
				if (Matrix[i] != tcm.Matrix[i])
					return Matrix[i] < tcm.Matrix[i];
			for (u32 i = 0; i < 2; i++)
				if (Translate[i] != tcm.Translate[i])
					return Translate[i] < tcm.Translate[i];
		}
		else
		if (Type == ETCMT_TURBULENT)
			if (Turbulent != tcm.Turbulent)
				return Turbulent < tcm.Turbulent;
		else
		if (Type == ETCMT_SCROLL)
			for (u32 i = 0; i < 2; i++)
				if (Scroll[i] != tcm.Scroll[i])
					return Scroll[i] < tcm.Scroll[i];
		else
		if (Type == ETCMT_SCALE)
			for (u32 i = 0; i < 2; i++)
				if (Scale[0] != tcm.Scale[0])
					return Scale[i] < tcm.Scale[i];
		else
		if (Type==ETCMT_ROTATE)
			if (RotateSpeed != tcm.RotateSpeed)
				return RotateSpeed < tcm.RotateSpeed;

		return false;					
    } 

    //! Compare operator ==
    bool operator ==(const STexCoordModifier& tcm) const
    {
		bool res = Type == tcm.Type;
		if (!res)
			return res;

		if (Type==ETCMT_TRANSFORM)
			res = 
				Matrix[0]==tcm.Matrix[0] && Matrix[1]==tcm.Matrix[1] &&
				Matrix[2]==tcm.Matrix[2] && Matrix[3]==tcm.Matrix[3] &&
				Translate[0]==tcm.Translate[0] && Translate[1]==tcm.Translate[1];
		else
		if (Type == ETCMT_TURBULENT)
			res = Turbulent == tcm.Turbulent;
		else
		if (Type == ETCMT_SCROLL)
			res = Scroll[0]==tcm.Scroll[0] && Scroll[1]==tcm.Scroll[1];
		else
		if (Type == ETCMT_SCALE)
			res = Scale[0]==tcm.Scale[0] && Scale[1]==tcm.Scale[1];	
		else
		if (Type == ETCMT_ROTATE)
			res = RotateSpeed==tcm.RotateSpeed;

		return res;					
    } 	

    //! Compare operator !=
    bool operator != (const STexCoordModifier& tcm) const
    { return !((*this)==tcm); } 

	E_TEXCOORD_MOD_TYPE getType() const { return Type; }
	void setType(E_TEXCOORD_MOD_TYPE type)
	{
		if (Type == type)
			return;
		Type = type;
		setHashFFPDirty();
	}

	// used for ETCMT_TURBULENT
	const core::wave& getTurbulent() const { return Turbulent; }
	void setTurbulent(const core::wave &turb)
	{
		if (Turbulent == turb)
			return;
		Turbulent = turb;
		if (Type == ETCMT_TURBULENT)
			setHashFFPDirty();
	}

	// used for ETCMT_TRANSFORM
	//  s' = s * matrix[0][0] + t * matrix[1][0] + translate[0]
	//  t' = s * matrix[0][1] + t * matrix[0][1] + translate[1]
	f32 getMatrix00() const { return Matrix00; }
	f32 getMatrix01() const { return Matrix01; }
	f32 getMatrix10() const { return Matrix10; }
	f32 getMatrix11() const { return Matrix11; }
	void setMatrix(f32 m00, f32 m01, f32 m10, f32 m11)
	{
		if (core::math::Equals(Matrix00, m00) &&
				core::math::Equals(Matrix01, m01) &&
				core::math::Equals(Matrix10, m10) &&
				core::math::Equals(Matrix11, m11))
			return;
		Matrix00 = m00;
		Matrix01 = m01;
		Matrix10 = m10;
		Matrix11 = m11;
		if (Type == ETCMT_TRANSFORM)
			setHashFFPDirty();
	}
	f32 getTranslate0() const { return Translate0; }
	f32 getTranslate1() const { return Translate1; }
	void setTranslate(f32 t0, f32 t1)
	{
		if (core::math::Equals(Translate0, t0) &&
				core::math::Equals(Translate1, t1))
			return;
		Translate0 = t0;
		Translate1 = t1;
		if (Type == ETCMT_TRANSFORM)
			setHashFFPDirty();
	}

	// used for ETCMT_SCALE
	//  s *= scale[0]
	//  t *= scale[1]
	f32 getScale0() const { return Scale0; }
	f32 getScale1() const { return Scale1; }
	void setScale(f32 scale0, f32 scale1)
	{
		if (core::math::Equals(Scale0, scale0) &&
				core::math::Equals(Scale1, scale1))
			return;
		Scale0 = scale0;
		Scale1 = scale1;
		if (Type == ETCMT_SCALE)
			setHashFFPDirty();
	}

	// used for ETCMT_SCROLL
	//  s' = s + scroll[0] * time
	//  t' = t + scroll[1] * time
	f32 getScroll0() const { return Scroll0; }
	f32 getScroll1() const { return Scroll1; }
	void setScroll(f32 scroll0, f32 scroll1)
	{
		if (core::math::Equals(Scroll0, scroll0) &&
				core::math::Equals(Scroll1, scroll1))
			return;
		Scroll0 = scroll0;
		Scroll1 = scroll1;
		if (Type == ETCMT_SCROLL)
			setHashFFPDirty();
	}

	// used for ETCMT_ROTATE
	//  + = clockwise
	//  - = counterclockwise
	f32 getRotateSpeed() const { return RotateSpeed; }
	void setRotateSpeed(f32 rot)
	{
		if (core::math::Equals(RotateSpeed, rot))
			return;
		RotateSpeed = rot;
		if (Type == ETCMT_ROTATE)
			setHashFFPDirty();
	}

	u32 getHashFFP() const;

private:

	void setHashFFPDirty(bool dirty = true)
	{
		HashFFPDirty = dirty;
		if (dirty && Layer)
			vid::setHashFFPDirty(Layer);
	}

	E_TEXCOORD_MOD_TYPE Type;

	// ETCMT_TURBULENT
	core::wave Turbulent;

	// ETCMT_TRANSFORM
	union
	{
		struct { f32 Matrix[4]; };
		struct { f32 Matrix00, Matrix01, Matrix10, Matrix11; };
	};
	union
	{
		struct { f32 Translate[2]; };
		struct { f32 Translate0, Translate1; };
	};

	// ETCMT_SCALE
	union
	{
		struct { f32 Scale[2]; };
		struct { f32 Scale0, Scale1; };
	};

	// ETCMT_SCROLL
	union
	{
		struct { f32 Scroll[2]; };
		struct { f32 Scroll0, Scroll1; };
	};

	// ETCMT_ROTATE
	f32 RotateSpeed;

	core::matrix4& getTexMatrixModification(u32 curr_time_ms)
	{
		static core::matrix4 translate_05(
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			-0.5f,	-0.5f,	0.0f,	1.0f);
		static core::matrix4 translate_05_back(
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			0.5f,	0.5f,	0.0f,	1.0f);

		static u32 first_curr_time_ms = curr_time_ms;

		u32 dtms = curr_time_ms - first_curr_time_ms;
		f32 dt = 0.01f * f32(dtms);

		TexMatrixModification.makeIdentity();

		if (Type==ETCMT_TRANSFORM)
		{
			TexMatrixModification[0] = Matrix[0];
			TexMatrixModification[1] = Matrix[1];
			TexMatrixModification[4] = Matrix[2];
			TexMatrixModification[5] = Matrix[3];
			TexMatrixModification[12] = Translate[0];
			TexMatrixModification[13] = Translate[1];
		}
		else
		if (Type==ETCMT_TURBULENT)
		{
			core::vector3df trans(
				Turbulent.get_value(dtms),
				Turbulent.get_value(dtms * 2),
				0);
			TexMatrixModification.setTranslation(trans);
		}
		else
		if (Type == ETCMT_SCROLL)
		{
			f32 scroll_x = Scroll[0]*dt;
			f32 scroll_y = Scroll[1]*dt;

			scroll_x = scroll_x-(s32)(scroll_x);
			scroll_y = scroll_y-(s32)(scroll_y);

			if (scroll_x<0) scroll_x += 1;
			if (scroll_y<0) scroll_y += 1;		

			core::vector3df trans( scroll_x, scroll_y, 0 );
			TexMatrixModification.setTranslation(trans);
		}
		else
		if (Type==ETCMT_SCALE)
		{
			TexMatrixModification.setScale(core::vector3df(Scale[0], Scale[1], 0));
		}
		else
		if (Type==ETCMT_ROTATE)
		{
			static core::matrix4 rotate;
			rotate.setRotationDegrees(core::vector3df(0, 0, -RotateSpeed*dt));
			TexMatrixModification = translate_05_back*rotate*translate_05;
		}
		return TexMatrixModification;  
	}

	core::matrix4 TexMatrixModification;

	STextureLayer *Layer;

	mutable bool HashFFPDirty;
	mutable u32 HashFFP;
};

//---------------------------------------------------------------------------

//! struct for holding a material
struct STextureLayer
{
	friend struct SRenderPass;

	STextureLayer()
		: Pass(NULL), m_LayerListNode(this)
	{
		defaultInit();		
		vid::onCreateTextureLayer(&m_LayerListNode);
	}

	STextureLayer(const STextureLayer& ml)
		: Pass(NULL), Texture(NULL), m_LayerListNode(this)
    {
		set(ml, false);
		vid::onCreateTextureLayer(&m_LayerListNode);
	}	

	virtual ~STextureLayer()
	{
		SAFE_DROP(Texture);
		vid::onDestroyTextureLayer(&m_LayerListNode);
	}

	void defaultInit()
	{
		Texture = NULL;
		ProjWidth = 50.f;
		ProjHeight = 50.f;
		ProjPos.set(0.f, 0.f, 0.f);
		ProjDir.set(0.f, -1.f, 0.f);
		ProjUp.set(1.f, 0.f, 0.f);
		TexCoordGen = ETCGT_MANUAL_MAPPING;
		TexMatrix.makeIdentity();
		TexMatrixCurrent = TexMatrix;
		TexCoordModifiers.clear();
		TexRGBScale = 1;
		TexCoordAddr = ETCAM_WRAP;
		TexCoordChannel=0;
		Type = ETLT_DIFFUSE_MAP;
		FilterEnabled = true;

		NeedToSetupTexMatrixForProjection = false;

		setHashDirty();
	}

	//! Compare operator !=
    bool operator < (const STextureLayer& other) const
    {
		if(this->operator ==(other))
			return false;

		if (Texture	!= other.Texture)
			return Texture < other.Texture;
		if (Type != other.Type)
			return Type < other.Type;
		if (TexCoordGen	!= other.TexCoordGen)
			return TexCoordGen < other.TexCoordGen;
		if (TexRGBScale	!= other.TexRGBScale)
			return TexRGBScale < other.TexRGBScale;
		if (TexMatrix != other.TexMatrix)
			return TexMatrix < other.TexMatrix;
		if (TexCoordAddr != other.TexCoordAddr)
			return TexCoordAddr < other.TexCoordAddr;
		if (TexCoordChannel != other.TexCoordChannel)
			return TexCoordChannel < other.TexCoordChannel;
		if (FilterEnabled != other.FilterEnabled)
			return FilterEnabled < other.FilterEnabled;
		return false;
	}

    //! Compare operator !=
    bool operator !=(const STextureLayer& other) const
    { return !((*this)==other); }
	
    //! Compare operator ==
    bool operator ==(const STextureLayer& other) const
    {
		return (getHashFFP()!= other.getHashFFP() ||
			getHashGPU()!= other.getHashGPU()) ? false : true;
    } 	
	
    //! operator =
    STextureLayer& operator = (const STextureLayer& ml)
    {
		set(ml);
		return *this;
    } 

	void set(const STextureLayer& ml, bool check_equal = true)
	{
		if (check_equal && this->operator == (ml))
			return;

		setTexture(
			((STextureLayer&)ml).Texture,
			((STextureLayer&)ml).Type,
			((STextureLayer&)ml).TexCoordChannel);

		ProjWidth	= ml.ProjWidth;
		ProjHeight	= ml.ProjHeight;
		ProjPos		= ml.ProjPos;
		ProjDir		= ml.ProjDir;
		ProjUp		= ml.ProjUp;
		NeedToSetupTexMatrixForProjection = ml.NeedToSetupTexMatrixForProjection;

		TexCoordModifiers = ml.TexCoordModifiers;
		u32 tcmcnt = TexCoordModifiers.size();
		for (u32 i = 0; i < tcmcnt; i++)
			TexCoordModifiers[i].Layer = this;

		TexCoordGen			= ml.TexCoordGen;
		TexRGBScale			= ml.TexRGBScale;
		TexMatrix			= ml.TexMatrix;
		TexMatrixCurrent	= ml.TexMatrixCurrent;
		TexCoordAddr		= ml.TexCoordAddr;
		TexCoordChannel		= ml.TexCoordChannel;

		FilterEnabled		= ml.FilterEnabled;

		setHashFFPDirty(ml.HashFFPDirty || HashFFP != ml.HashFFP);
		HashFFP = ml.HashFFP;

		setHashGPUDirty(ml.HashGPUDirty || HashGPU != ml.HashGPU);
		HashGPU = ml.HashGPU;
	}

	ITexture* getTexture() const
	{ return Texture; }

	void setTexture(ITexture *t, E_TEXTURE_LAYER_TYPE type, s32 tc_channel = 0) 
	{ 
		if (Type != type || TexCoordChannel != tc_channel ||
			(Texture == NULL && t != NULL) || (Texture != NULL && t == NULL))
		{
			Type = type;
			TexCoordChannel = tc_channel;
			setHashGPUDirty();
		}
		if (t != Texture)
		{
			SAFE_DROP(Texture);
			Texture = t; 
			SAFE_GRAB(Texture);
			setHashFFPDirty();
		}
	}

	void setTexture(ITexture *t) 
	{ 
		if ((Texture == NULL && t != NULL) ||
			(Texture != NULL && t == NULL))
		{
			setHashGPUDirty();
		}
		if (t != Texture)
		{
			SAFE_DROP(Texture);
			Texture = t; 
			SAFE_GRAB(Texture);
			setHashFFPDirty();
		}
	}

	void setType(E_TEXTURE_LAYER_TYPE type)
	{
		if(Type == type)
			return;
		Type = type;
		setHashGPUDirty();
	}

	E_TEXTURE_LAYER_TYPE getType() const
	{ return Type; }

	void* getHardwareTexture() const
	{ return Texture ? Texture->getHardwareTexture():0; }

	void update(u32 curr_time_ms)
	{
		u32 tcmsize = TexCoordModifiers.size();
		if (tcmsize == 0)
			return;
		TexMatrixCurrent = TexMatrix;
		for (u32 m = 0; m < tcmsize; m++) TexMatrixCurrent =
			TexCoordModifiers[m].getTexMatrixModification(curr_time_ms) * TexMatrixCurrent;
	}

	void setProjectionParams(f32 width_area, f32 height_area,
		const core::vector3df &pos, const core::vector3df &dir, const core::vector3df &up)
	{
		ProjWidth = width_area;
		ProjHeight = height_area;
		ProjPos = pos;
		ProjDir = dir;
		ProjUp = up;

		NeedToSetupTexMatrixForProjection = true;

		if (TexCoordGen == vid::ETCGT_PROJECTED_MAPPING)
			setupTexMatrixForProjection();
	}

	f32 getProjectionWidth() const
	{ return ProjWidth; }

	f32 getProjectionHeight() const
	{ return ProjHeight; }

	const core::vector3df& getProjectionPos() const
	{ return ProjPos; }

	const core::vector3df& getProjectionDir() const
	{ return ProjDir; }

	const core::vector3df& getProjectionUp() const
	{ return ProjUp; }

	void setTexMatrix(const core::matrix4 &m)
	{	
		if (TexCoordGen == vid::ETCGT_PROJECTED_MAPPING)
			return;
		TexMatrix = m;
		TexMatrixCurrent = TexMatrix;
		setHashFFPDirty();
	}

	const core::matrix4& getTexMatrix() const
	{ return TexMatrix; }

	const core::matrix4& getTexMatrixCurrent() const
	{ return TexMatrixCurrent; }

	//! Get texture coords gen mode
	E_TEXCOORD_GEN_TYPE getTexCoordGen() const
	{ return TexCoordGen; }

	//! Set texture coords gen mode
	void setTexCoordGen(E_TEXCOORD_GEN_TYPE tgt)
	{
		if (TexCoordGen == tgt)
			return;
		if (TexCoordGen == vid::ETCGT_PROJECTED_MAPPING)
		{
			TexMatrix.makeIdentity();
			TexMatrixCurrent = TexMatrix;
			setHashFFPDirty();
		}
		else
		if (tgt == vid::ETCGT_PROJECTED_MAPPING)
		{
			if (NeedToSetupTexMatrixForProjection)
				setupTexMatrixForProjection();
		}
		TexCoordGen = tgt;
		setHashGPUDirty();
	}

	//! Get texture coords modifiers stack
	const core::array<STexCoordModifier>& getTexCoordModifiers() const
	{ return TexCoordModifiers; }

	u32 getTexCoordModifiersCount() const
	{ return TexCoordModifiers.size(); }

	STexCoordModifier& getTexCoordModifier(u32 idx)
	{ return TexCoordModifiers[idx]; }
	const STexCoordModifier& getTexCoordModifier(u32 idx) const
	{ return TexCoordModifiers[idx]; }

	//! Add new texture coords modifier
	void addTexCoordModifier(vid::E_TEXCOORD_MOD_TYPE type)
	{
		addTexCoordModifier(STexCoordModifier(type));
	}
	void addTexCoordModifier(const STexCoordModifier &tcm)
	{
		if (TexCoordModifiers.size() == 0)
			setHashGPUDirty();
		TexCoordModifiers.push_back(tcm);
		TexCoordModifiers[TexCoordModifiers.size() - 1].Layer = this;
	}

	void removeTexCoordModifier(u32 idx)
	{
		TexCoordModifiers.erase(idx);
		if (TexCoordModifiers.size() == 0)
		{
			TexMatrixCurrent = TexMatrix;
			setHashGPUDirty();
		}
	}

	void setTexCoordModifier(u32 idx, const STexCoordModifier &tcm)
	{
		TexCoordModifiers[idx].set(tcm);
	}

	bool swapTexCoordModifiers(u32 idx1, u32 idx2)
	{
		u32 sz = TexCoordModifiers.size();
		if (idx1 == idx2 || idx1 >= sz || idx2 >= sz)
			return false;
		STexCoordModifier tmp(TexCoordModifiers[idx1]);
		TexCoordModifiers[idx1].set(TexCoordModifiers[idx2]);
		TexCoordModifiers[idx2].set(tmp);
		return true;
	}

	//! Return true if tex coords animated via Texture Matrix
	bool isTexCoordAnimated() const
	{ return TexCoordModifiers.size() > 0; }

	//! Get texture RGB color scale
	s32 getTexRGBScale() const
	{ return TexRGBScale; }

	//! Set texture RGB color scale
	void setTexRGBScale(s32 scl)
	{
		if(TexRGBScale == scl)
			return;
		TexRGBScale = scl;
		setHashGPUDirty();
	}

	//! Get texture coords addresing mode
	E_TEXCOORD_ADDRESS_MODE getTexCoordAddr() const
	{ return TexCoordAddr; }

	//! Set texture coords addresing mode
	void setTexCoordAddr(E_TEXCOORD_ADDRESS_MODE am)
	{
		if(TexCoordAddr == am)
			return;
		TexCoordAddr = am;
		setHashGPUDirty();
	}

	//! Get texture coords channel indes
	s32 getTexCoordChannel() const
	{ return TexCoordChannel; }

	//! Get texture coords channel indes
	void setTexCoordChannel(s32 tch)
	{
		if(TexCoordChannel == tch)
			return;
		TexCoordChannel = tch;
		setHashGPUDirty();
	}

	//! Enable/disable texture filter
	void enableFilter(bool enable)
	{
		if(FilterEnabled == enable)
			return;
		FilterEnabled = enable;
		setHashFFPDirty();
	}

	bool isFilterEnabled() const
	{ return FilterEnabled; }

	void setHashDirty(bool dirty = true)
	{
		HashFFPDirty = HashGPUDirty = dirty;
		if (dirty && Pass)
			vid::setHashDirty(Pass);
	}

	void setHashFFPDirty(bool dirty = true)
	{
		HashFFPDirty = dirty;
		if (dirty && Pass)
			vid::setHashFFPDirty(Pass);
	}

	void setHashGPUDirty(bool dirty = true)
	{
		HashGPUDirty = dirty;
		if (dirty && Pass)
			vid::setHashGPUDirty(Pass);
	}

	u16 getHashFFP() const;
	u16 getHashGPU() const;

private:

	void setupTexMatrixForProjection()
	{
		f32 width_area = ProjWidth;
		f32 height_area = ProjHeight;

		core::vector3df left = ProjUp.getCrossProduct(ProjDir);
		left.normalize();

		core::vector3df posoffs(ProjPos);
		posoffs -= left * width_area / 2.0f;
		posoffs -= ProjUp * height_area / 2.0f;	

		width_area *= 2.0f;
		height_area *= 2.0f;

		static core::matrix4 proj, view;	

		proj.buildProjectionMatrixOrthoLH(width_area, height_area, 0.1, 1.0f);							
		view.buildViewMatrixLH(posoffs, posoffs + ProjDir, ProjUp);

		TexMatrix = proj * view;
		TexMatrixCurrent = TexMatrix;

		setHashFFPDirty();
	}

	ITexture				*Texture;
	E_TEXTURE_LAYER_TYPE	Type;
	core::matrix4			TexMatrix, TexMatrixCurrent;

	f32 ProjWidth, ProjHeight;
	core::vector3df ProjPos, ProjDir, ProjUp;
	bool NeedToSetupTexMatrixForProjection;

	E_TEXCOORD_GEN_TYPE					TexCoordGen;	
	core::array < STexCoordModifier >	TexCoordModifiers;
	s32									TexRGBScale;
	E_TEXCOORD_ADDRESS_MODE				TexCoordAddr;
	s32									TexCoordChannel;

	mutable bool HashFFPDirty, HashGPUDirty;

	mutable u16 HashFFP, HashGPU;

	bool FilterEnabled;

	SRenderPass *Pass;

	core::list_node<STextureLayer*> m_LayerListNode;
};

//---------------------------------------------------------------------------

//! struct for holding a render material
struct SRenderPass
{
	friend struct SMaterial;

	//! material texture layers
	STextureLayer Layers [MY_MATERIAL_MAX_LAYERS];
  
	SRenderPass()
		: Material(NULL), GPUProgram(NULL), m_RenderPassListNode(this)
    {
		defaultInit();
		for(u32 tl = 0; tl < MY_MATERIAL_MAX_LAYERS; tl++)
			Layers[tl].Pass = this;
		vid::onCreateRenderPass(&m_RenderPassListNode);
	}

	SRenderPass(const SRenderPass& m)
		: Material(NULL), GPUProgram(NULL), m_RenderPassListNode(this)
    {
		set(m, false);
		for(u32 tl = 0; tl < MY_MATERIAL_MAX_LAYERS; tl++)
			Layers[tl].Pass = this;
		vid::onCreateRenderPass(&m_RenderPassListNode);
	}

	virtual ~SRenderPass()
	{
		SAFE_DROP(GPUProgram);
		vid::onDestroyRenderPass(&m_RenderPassListNode);
	}

	void defaultInit()
	{	
		Updated = false;

		LightingMode = ELM_NONE;

		AmbientColor.set(255,0,0,0);
		DiffuseColor.set(255,255,255,255);
		EmissiveColor.set(255,0,0,0);
		SpecularColor.set(255,255,255,255);
		Shininess = 0.0f;

		DiffuseColorCurrent = DiffuseColor;
		EmissiveColorCurrent = EmissiveColor;

		SrcBlendFunc = ESBF_ONE;
		DstBlendFunc = EDBF_ONE;
		BlendEquation = EBE_ADD;

		Flags = 0;

		setFlag(EMF_ZWRITE_ENABLE,		true);
		setFlag(EMF_BACK_FACE_CULLING,	true);
		setFlag(EMF_FRONT_FACE_CCW,		true);
		setFlag(EMF_GOURAUD_SHADING,	true);
		setFlag(EMF_FOG_ENABLE,			false);
		setFlag(EMF_MIP_MAP_OFF,		false);
		setFlag(EMF_BLENDING,			false);

		ColorGen = ECGT_VERTEX;
		AlphaGen = EAGT_VERTEX;

		DepthTest	= ECT_LESS;
		AlphaTest	= EAT_ALWAYS;

		Number = 0;

		for(s32 tl=0; tl<MY_MATERIAL_MAX_LAYERS; tl++)
		{
			Layers[tl].defaultInit();
			if(tl == 1)
				Layers[tl].setTexCoordChannel(1);
		}

		setHashDirty();
	}

	//! Get ambient color(how much ambient light(a global light) is reflected by this material)
    //! The default is full white, meaning objects are completely globally illuminated.
    //! Reduce this if you want to see diffuse or specular light effects, or change
    //! the blend of colours to make the object have a base colour other than white.
    const img::SColor& getAmbientColor() const
	{ return AmbientColor; }

	//! Set ambient color
    void setAmbientColor(const img::SColor & col)
	{
		if(AmbientColor == col)
			return;
		AmbientColor = col;
		setHashFFPDirty();
	}

    //! Get diffuse color(how much diffuse light coming from a light source is reflected)
    //! The default is full white.
    const img::SColor & getDiffuseColor() const
	{ return DiffuseColor; }

	//! Set diffuse color
	void setDiffuseColor(const img::SColor & col)
	{
		if (DiffuseColor == col)
			return;
		DiffuseColor = col;
		DiffuseColorCurrent = col;
		setHashFFPDirty();
	}

    //! Get emissive color(light emitted by this material)
	//! Default is to emitt no light.
    const img::SColor& getEmissiveColor() const
	{ return EmissiveColor; }

	//! Set emissive color
	void setEmissiveColor(const img::SColor & col)
	{
		if (EmissiveColor == col)
			return;
		EmissiveColor = col;
		EmissiveColorCurrent = col;
		setHashFFPDirty();
	}

    //! Get specular color(how much highlights from a light is reflected)
    //! The default is to reflect no specular light. 
    const img::SColor & getSpecularColor() const
	{ return SpecularColor; }

	//! Set specular color
    void setSpecularColor(const img::SColor & col)
	{
		if(SpecularColor == col)
			return;
		SpecularColor = col;
		setHashFFPDirty();
	}

    //! Get shininess(value affecting the size of specular highlights)
    f32 getShininess() const
	{ return Shininess; }

	//! Set shininess
	void setShininess(f32 sh)
	{
		CHECK_RANGE(sh, 0.0f, 128.0f);
		if(Shininess == sh)
			return;
		Shininess = sh;
		setHashFFPDirty();
	}

    //! Determinates if the material is somehow transparent.
    //! \return Returns true if material is transparent, false if not.
    bool isTransparent() const
    { return getFlag(EMF_BLENDING); }

    //! Return true if this material allow some lighting mode
    bool isLighting() const
	{ return LightingMode != ELM_NONE; }

	//! Get Lighting Mode
	E_LIGHTING_MODE getLightingMode() const
	{ return LightingMode;}

	//! Set Lighting Mode
	void setLightingMode(E_LIGHTING_MODE mode)
	{
		if(LightingMode == mode)
			return;
		LightingMode = mode;
		setHashGPUDirty();
	}

    //! Compare operator <
    bool operator < (const SRenderPass &b) const
    {
		if (this->operator ==(b))
			return false;
		if (GPUProgram != b.GPUProgram)
			return GPUProgram < b.GPUProgram;
		if (AmbientColor != b.AmbientColor)
			return AmbientColor < b.AmbientColor;
		if (DiffuseColor != b.DiffuseColor)
			return DiffuseColor < b.DiffuseColor;
		if (EmissiveColor != b.EmissiveColor)
			return EmissiveColor < b.EmissiveColor;
		if (SpecularColor != b.SpecularColor)
			return SpecularColor < b.SpecularColor;
		if (Shininess != b.Shininess)
			return Shininess < b.Shininess;
		if (LightingMode != b.LightingMode)
			return LightingMode < b.LightingMode;
		if (SrcBlendFunc != b.SrcBlendFunc)
			return SrcBlendFunc < b.SrcBlendFunc;
		if (DstBlendFunc != b.DstBlendFunc)
			return DstBlendFunc < b.DstBlendFunc;
		if (BlendEquation != b.BlendEquation)
			return BlendEquation < b.BlendEquation;
		if (ColorFunc != b.ColorFunc)
			return ColorFunc < b.ColorFunc;
		if (AlphaFunc != b.AlphaFunc)
			return AlphaFunc < b.AlphaFunc;
		if (DepthTest != b.DepthTest)
			return DepthTest < b.DepthTest;
		if (AlphaTest != b.AlphaTest)
			return AlphaTest < b.AlphaTest;
		if (ColorGen != b.ColorGen)
			return ColorGen < b.ColorGen;
		if (AlphaGen != b.AlphaGen)
			return AlphaGen < b.AlphaGen;
		if (Number != b.Number)
			return Number < b.Number;

		if (Flags != b.Flags)
			return Flags < b.Flags;

		for (u32 tl = 0; tl < MY_MATERIAL_MAX_LAYERS; tl++)
			if (Layers[tl] != b.Layers[tl])
				return Layers[tl] < b.Layers[tl];

        return false;	
	} 

    //! Compare operator !=
    bool operator != (const SRenderPass& b) const
    { return !((*this)==b); } 
	
    //! Compare operator ==
    bool operator == (const SRenderPass& b) const
    { 	
		return (getHashFFP()!= b.getHashFFP() ||
			getHashGPU()!= b.getHashGPU()) ? false : true;
	} 

    //! operator =
    SRenderPass& operator = (const SRenderPass& p)
    {
		set(p);
		return *this;
    } 

	void set(const SRenderPass& p, bool check_equal = true)
	{
		if (check_equal && this->operator ==(p))
			return;

		Flags = p.Flags;

		LightingMode = p.LightingMode;

		DiffuseColor  = p.DiffuseColor;
		AmbientColor  = p.AmbientColor;
		EmissiveColor = p.EmissiveColor;

		if(p.Updated)
		{
			DiffuseColorCurrent  = p.DiffuseColorCurrent;
			EmissiveColorCurrent  = p.EmissiveColorCurrent;
		}
		else
		{
			DiffuseColorCurrent  = p.DiffuseColor;
			EmissiveColorCurrent  = p.EmissiveColor;
		}
        
        SpecularColor = p.SpecularColor;
        Shininess     = p.Shininess;   		
		
		SrcBlendFunc = p.SrcBlendFunc;
		DstBlendFunc = p.DstBlendFunc;
		BlendEquation = p.BlendEquation;

		ColorFunc = p.ColorFunc;
		AlphaFunc = p.AlphaFunc;
		ColorGen = p.ColorGen;
		AlphaGen = p.AlphaGen;

		DepthTest = p.DepthTest;
		AlphaTest = p.AlphaTest;

		Number = p.Number;

		setGPUProgram(p.getGPUProgram());

		for(s32 tl=0; tl<MY_MATERIAL_MAX_LAYERS; tl++)
			Layers[tl] = p.Layers[tl];

		setHashFFPDirty(p.HashFFPDirty || HashFFP != p.HashFFP);
		HashFFP = p.HashFFP;

		setHashGPUDirty(p.HashGPUDirty || HashGPU != p.HashGPU);
		HashGPU = p.HashGPU;
	}

	//! Get material flags
	u16 getFlags() const
	{ return Flags; }

	//! Get material flag
	bool getFlag(vid::E_MATERIAL_FLAG flag) const
	{ return(Flags & MaterialFlagBit[flag]) != 0; }

	//! Set material flag value
	void setFlag(vid::E_MATERIAL_FLAG flag, bool value)
	{
		bool fvalue = getFlag(flag);
		if(fvalue == value)
			return;
		if(value)
			Flags |= MaterialFlagBit[flag];
		else
			Flags &= ~MaterialFlagBit[flag];
		(flag == EMF_FOG_ENABLE) ?
			setHashGPUDirty() : setHashFFPDirty();
	}

	//! Get Color / Alpha change functions
	const core::wave & getColorFunc() const
	{ return ColorFunc; }
	const core::wave & getAlphaFunc() const
	{ return AlphaFunc; }

	//! Set Color / Alpha generation functions
	void setColorFunc(const core::wave & func)
	{
		if(ColorFunc == func)
			return;
		ColorFunc = func;
		setHashFFPDirty();
	}
	void setAlphaFunc(const core::wave & func)
	{
		if(AlphaFunc == func)
			return;
		AlphaFunc = func;
		setHashFFPDirty();
	}

	//! Get Color / Alpha generation type
	E_COLOR_GEN_TYPE getColorGen() const
	{ return ColorGen; }
	E_ALPHA_GEN_TYPE getAlphaGen() const
	{ return AlphaGen; }

	//! Set Color / Alpha generation type
	void setColorGen(E_COLOR_GEN_TYPE gen)
	{
		if(ColorGen == gen)
			return;
		ColorGen = gen;
		setHashGPUDirty();
	}
	void setAlphaGen(E_ALPHA_GEN_TYPE gen)
	{
		if(AlphaGen == gen)
			return;
		AlphaGen = gen;
		setHashGPUDirty();
	}

	//! Get Depth / Alpha test functions
	E_COMPARE_TEST getDepthTest() const
	{ return DepthTest; }
	E_ALPHA_TEST getAlphaTest() const
	{ return AlphaTest; }

	//! Set Depth / Alpha test functions
	void setDepthTest(E_COMPARE_TEST dt)
	{
		if(DepthTest == dt)
			return;
		DepthTest = dt;
		setHashFFPDirty();
	}
	void setAlphaTest(E_ALPHA_TEST at)
	{
		if(AlphaTest == at)
			return;
		AlphaTest = at;
		setHashFFPDirty();
	}

	//! Get Blending factor for source objects in the scene.
	E_SRC_BLEND_FUNCTION getSrcBlendFunc() const
	{ return SrcBlendFunc; }

	//! Get Blending factor for destination objects in the scene.
	E_DST_BLEND_FUNCTION getDstBlendFunc() const
	{ return DstBlendFunc; }

	//! Set Blending factor for source and dest objects in the scene.
	void setBlendFuncs(
		E_SRC_BLEND_FUNCTION srcblend, E_DST_BLEND_FUNCTION dstblend)
	{	
		if (SrcBlendFunc == srcblend && DstBlendFunc == dstblend)
			return;
		SrcBlendFunc = srcblend;
		DstBlendFunc = dstblend;
		setHashFFPDirty();
	}

	//! Get Blending equation
	E_BLEND_EQUATION getBlendEquation() const
	{ return BlendEquation; }

	//! Set Blending equation
	void setBlendEquation(E_BLEND_EQUATION eq)
	{
		if(BlendEquation == eq)
			return;
		BlendEquation = eq;
		setHashFFPDirty();
	}

	void update(u32 curr_time_ms)
	{
		// updating material layers
		for (s32 tl=0; tl<MY_MATERIAL_MAX_LAYERS; tl++)
			Layers[tl].update(curr_time_ms);

		// update material colors and alpha
		if (ColorGen == ECGT_FUNCTION)
		{
			f32 glow = ColorFunc.get_value(curr_time_ms);
			CHECK_RANGE(glow, 0.0f, 1.0f);
			EmissiveColorCurrent.set(255,
				EmissiveColor.getRed() * glow,
				EmissiveColor.getGreen() * glow,
				EmissiveColor.getBlue() * glow);
		}
		else
			EmissiveColorCurrent.color = EmissiveColor.color;

		if (AlphaGen == EAGT_FUNCTION)
		{
			f32 glow = AlphaFunc.get_value(curr_time_ms);
			CHECK_RANGE(glow, 0, 1);  		
			int alpha = 255 * glow;
			DiffuseColorCurrent.setAlpha(alpha);
		}
		else
			DiffuseColorCurrent.setAlpha(DiffuseColor.getAlpha());

		Updated = true;
	}
	
	const img::SColor& getDiffuseColorCurrent() const
	{ return DiffuseColorCurrent; }

	const img::SColor& getEmissiveColorCurrent() const
	{ return EmissiveColorCurrent; }

	s32 getNumber() const
	{ return Number; }

	void setGPUProgram(IGPUProgram* gpu)
	{
		if (GPUProgram == gpu)
			return;
		SAFE_DROP(GPUProgram);
		GPUProgram = gpu;
		SAFE_GRAB(GPUProgram);
		setHashFFPDirty();
	}

	IGPUProgram* getGPUProgram() const
	{ return GPUProgram; }

	//! swap two layers
	bool swapLayers(unsigned l1, unsigned l2)
	{
		if (l1 >= MY_MATERIAL_MAX_LAYERS ||
				l2 >= MY_MATERIAL_MAX_LAYERS ||
				l1 == l2)
			return false;
		STextureLayer tmp;
		STextureLayer &layer1 = Layers[l1];
		STextureLayer &layer2 = Layers[l2];
		tmp.set(layer1);
		layer1.set(layer2);
		layer2.set(tmp);
		setHashDirty();
		return true;
	}

	void setHashDirty(bool dirty = true)
	{
		HashFFPDirty = HashGPUDirty = dirty;
		if (dirty && Material)
			vid::setHashDirty(Material);
	}

	void setHashFFPDirty(bool dirty = true)
	{
		HashFFPDirty = dirty;
		if (dirty && Material)
			vid::setHashFFPDirty(Material);
	}

	void setHashGPUDirty(bool dirty = true)
	{
		HashGPUDirty = dirty;
		if (dirty && Material)
			vid::setHashGPUDirty(Material);
	}

	u64 getHash() const
	{ return getHashGPU() ^ getHashFFP(); }

	u64 getHashFFP() const;
	u64 getHashGPU() const;

private:

	mutable bool HashFFPDirty, HashGPUDirty;

	mutable u64 HashFFP, HashGPU;

	img::SColor AmbientColor;        
    img::SColor DiffuseColor;
    img::SColor EmissiveColor;   
    img::SColor SpecularColor;   

    f32 Shininess;

	//! material flags
	u16 Flags;

	E_LIGHTING_MODE LightingMode;

	core::wave ColorFunc, AlphaFunc;

	E_COLOR_GEN_TYPE ColorGen;
	E_ALPHA_GEN_TYPE AlphaGen;

	//! Depth / Alpha test functions
	E_COMPARE_TEST DepthTest;
	E_ALPHA_TEST AlphaTest;

	//! Blending factor for source/destination objects with the scene.
	E_SRC_BLEND_FUNCTION SrcBlendFunc;
	E_DST_BLEND_FUNCTION DstBlendFunc;

	//! Blending equations for objects with the scene.
	E_BLEND_EQUATION BlendEquation;
	
	img::SColor DiffuseColorCurrent, EmissiveColorCurrent;

	bool Updated;

	s32 Number;

	SMaterial *Material;

	IGPUProgram	*GPUProgram;

	core::list_node<SRenderPass*> m_RenderPassListNode;
};

//---------------------------------------------------------------------------

struct __MY_VID_LIB_API__ SMaterial
{
	SMaterial() : HashFFP(0), HashGPU(0)
	{
		// default render pass
		addPass(SRenderPass());
	}

	SMaterial(const SRenderPass &pass) : HashFFP(0), HashGPU(0)
	{
		// default render pass
		addPass(SRenderPass());
		Passes[0] = pass;
	}

	SMaterial(const SMaterial &other) : HashFFP(0), HashGPU(0)
	{ this->operator =(other); }	

	void set(const SMaterial &other)
	{
		Passes = other.Passes;
		u32 pcnt = other.getPassesCount();
		for(u32 p = 0; p < pcnt; p++)
			Passes[p].Material = this;
		HashFFPDirty = other.HashFFPDirty;
		HashGPUDirty = other.HashGPUDirty;
		HashFFP = other.HashFFP;
		HashGPU = other.HashGPU;
	}

    //! Compare operator <
    bool operator < (const SMaterial& other) const
    {
		if(this->operator ==(other))
			return false;
		u32 size1 = Passes.size();
		u32 size2 = other.Passes.size();
		for(u32 p = 0; p < size1 && p < size2; p++)
			if(Passes[p] != other.Passes[p])
				return Passes[p] < other.Passes[p];
		return size1 < size2;
	} 
	
    //! Compare operator !=
    bool operator != (const SMaterial& b) const
    { return !((*this) == b); } 
	
    //! Compare operator ==
    bool operator ==(const SMaterial& b) const
    { 
		return(getHashFFP() != b.getHashFFP() ||
			getHashGPU()!= b.getHashGPU()) ? false : true;
	}

	void operator = (const SMaterial & other)
	{ set(other); }

	void setFlag(E_MATERIAL_FLAG flag, bool value)
	{
		s32 size = Passes.size();
		if(size == 1 ||(
				flag != vid::EMF_MIP_MAP_OFF && flag != vid::EMF_BLENDING && 
				flag != vid::EMF_ZWRITE_ENABLE))
		{
			for(s32 r=0; r<size; r++)
			{
				if(flag == vid::EMF_BACK_FACE_CULLING && Passes[r].isTransparent())
					Passes[r].setFlag(vid::EMF_BACK_FACE_CULLING, false);
				else
				{
					if(flag == vid::EMF_BLENDING && value == true)
						Passes[r].setFlag(vid::EMF_BACK_FACE_CULLING, false);
					Passes[r].setFlag(flag, value);
				}
			}
		}
	}

	void setLightingMode(E_LIGHTING_MODE mode)
	{
		s32 size = Passes.size();
		if (size == 1)
			Passes[0].setLightingMode(mode);
	}

	bool isTransparent() const
	{
		s32 size = Passes.size();
		for(s32 r=0; r<size; r++)
			if(!Passes[r].isTransparent())
				return false;
		return true;
	}

	void update(u32 curr_time_ms)
	{
		for(u32 r = 0; r < Passes.size(); r++)
			Passes[r].update(curr_time_ms);
	}

	void defaultInit()
	{
		for(u32 r = 0; r < Passes.size(); r++)
			Passes[r].defaultInit();
	}

	SRenderPass& getPass(u32 p) 
	{ return Passes[p]; }

	const SRenderPass& getPass(u32 p) const
	{ return Passes[p]; }

	u32 getPassesCount() const
	{ return Passes.size(); }

	u32 addPass()
	{
		SRenderPass pass;
		return addPass(pass);
	}

	u32 addPass(const SRenderPass &pass)
	{
		Passes.push_back(pass);
		s32 number = Passes.size()-1;
		Passes[number].Number = number;
		if (number > 0)
		{
			Passes[number].setDepthTest(ECT_EQUAL);
			Passes[number].setFlag(EMF_BLENDING, true);
		}
		Passes[number].Material = this;
		setHashDirty();
		return number;
	}

	bool removePass(u32 p)
	{
		if (Passes.size() == 1)
			return false;
		Passes.erase(p);
		for (u32 r = p; r < Passes.size(); r++)
			Passes[r].Number = r;
		setHashDirty();
		return true;
	}

	void setHashDirty(bool dirty = true)
	{ HashFFPDirty = HashGPUDirty = dirty; }

	void setHashFFPDirty(bool dirty = true)
	{ HashFFPDirty = dirty; }

	void setHashGPUDirty(bool dirty = true)
	{ HashGPUDirty = dirty; }

	u64 getHash() const
	{ return getHashGPU() ^ getHashFFP(); }

	u64 getHashFFP() const;
	u64 getHashGPU() const;

	bool loadFromFile(const c8 *filename);
	bool saveToFile(const c8 *filename) const;

private:

	//! render passes
	core::array < SRenderPass > Passes;

	mutable bool HashFFPDirty, HashGPUDirty;
	mutable u64 HashFFP, HashGPU;
};

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashDirty(SMaterial *material)
{ material->setHashDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashFFPDirty(SMaterial *material)
{ material->setHashFFPDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashGPUDirty(SMaterial *material)
{ material->setHashGPUDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashDirty(SRenderPass *pass)
{ pass->setHashDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashFFPDirty(SRenderPass *pass)
{ pass->setHashFFPDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashGPUDirty(SRenderPass *pass)
{ pass->setHashGPUDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE void setHashFFPDirty(STextureLayer *layer)
{ layer->setHashFFPDirty(); }

//---------------------------------------------------------------------------

MY_FORCEINLINE u32 STexCoordModifier::getHashFFP() const
{
	if (HashFFPDirty)
	{
		HashFFPDirty = false;
		u32 hash = (Type << 28) & 0xf0000000;
		if (Type == ETCMT_TRANSFORM)
		{
			for (u32 i = 0; i < 4; i++)
				hash ^= ((u32)(Matrix[i] * 10000.f)) & 0xffffff;
			for (u32 i = 0; i < 2; i++)
				hash ^= ((u32)(Translate[i] * 10000.f)) & 0xffffff;
		}
		else
		if (Type == ETCMT_TURBULENT)
			hash ^= Turbulent.get_hash();
		else
		if (Type == ETCMT_SCROLL)
			for (u32 i = 0; i < 2; i++)
				hash ^= ((u32)(Scroll[i] * 10000.f)) & 0xffffff;
		else
		if (Type == ETCMT_SCALE)
			for (u32 i = 0; i < 2; i++)
				hash ^= ((u32)(Scale[i] * 10000.f)) & 0xffffff;
		else
		if (Type==ETCMT_ROTATE)
			hash ^= ((u32)(RotateSpeed * 10000.f)) & 0xffffff;
		HashFFP = hash;
	}
	return HashFFP;
} 

//---------------------------------------------------------------------------

MY_FORCEINLINE u16 STextureLayer::getHashFFP() const
{
	if (HashFFPDirty)
	{
		HashFFPDirty = false;
		HashFFP = 0;
		if(!Texture)
			return HashFFP;
		u32 hash =  FilterEnabled ? 0x1 : 0x0;
		
		hash <<= 2;
		hash ^= TexCoordAddr; // max 3

		hash <<= 3;
		hash ^= TexRGBScale; // max 4

		hash ^= core::hash_key(Texture, 32, 0xffffffff);

		if (!TexMatrix.isIdentity())
			for(u32 i = 0; i < 4; i++)
				for(u32 j = 0; j < 4; j++)
					hash ^= *((u32*)&(TexMatrix(i, j)));
		u32 tcmcnt = TexCoordModifiers.size();
		for (u32 i = 0; i < tcmcnt; i++)
			hash ^= TexCoordModifiers[i].getHashFFP();
		HashFFP = 0xffff & ((hash >> 16) ^ hash);
	}
	return HashFFP;
}

//---------------------------------------------------------------------------

MY_FORCEINLINE u16 STextureLayer::getHashGPU() const
{
	if(HashGPUDirty)
	{
		HashGPUDirty = false;
		u16 hash = 0;
		if (Texture)
		{
			hash ^= Texture->getFormat(); // max 3

			hash <<= 3;
			hash ^= Type;  // max 5

			hash <<= 2;
			hash ^= TexCoordGen; // max 2

			hash <<= 3;
			hash ^= TexRGBScale; // max 4

			hash <<= 1;
			hash ^= TexCoordChannel; // max 1

			hash <<= 1;
			hash ^= TexCoordModifiers.size() ? 1 : 0;
		}
		HashGPU = hash;
	}
	return HashGPU;
}

//---------------------------------------------------------------------------

MY_FORCEINLINE u64 SRenderPass::getHashFFP() const
{
	if(HashFFPDirty)
	{
		HashFFPDirty = false;

		u64 hash = (u64)GPUProgram;

		hash ^=((u64)AmbientColor.color);
		hash ^=((u64)DiffuseColor.color << 32);
		hash ^=((u64)EmissiveColor.color);
		hash ^=((u64)SpecularColor.color << 32);
		hash ^=((u64)Shininess << 28);

		if (Flags & EMF_BLENDING)
		{
			hash ^=((u64)SrcBlendFunc << 40);
			hash ^=((u64)DstBlendFunc << 20);
			hash ^=((u64)BlendEquation);
		}

		hash ^=((u64)ColorFunc.get_hash() << 32);
		hash ^=((u64)AlphaFunc.get_hash());

		hash ^=((u64)DepthTest);
		hash ^=((u64)AlphaTest << 8);

		hash ^=((u64)Number << 40);
		hash ^=((u64)(Flags & (~MaterialFlagBit[EMF_FOG_ENABLE])) << 48);

		for(u32 t = 0; t < MY_MATERIAL_MAX_LAYERS; t++)
			if (Layers[t].Texture)
				hash ^= ((u64)Layers[t].getHashFFP()) << (16 * t);

		HashFFP = hash;
	}
	return HashFFP;
}

//---------------------------------------------------------------------------

MY_FORCEINLINE u64 SRenderPass::getHashGPU() const
{
	if (HashGPUDirty)
	{
		HashGPUDirty = false;

		u64 hash = (u64)(LightingMode ? 0xffffffffffffffff : 0x0);

		hash ^=((u64)ColorGen << 16);
		hash ^=((u64)AlphaGen << 24);
		hash ^=((u64)(Flags & MaterialFlagBit[EMF_FOG_ENABLE]) << 32);

		for(u32 t = 0; t < MY_MATERIAL_MAX_LAYERS; t++)
			hash ^= ((u64)Layers[t].getHashGPU()) << (16 * t);

		HashGPU = hash;
	}
	return HashGPU;
}

//---------------------------------------------------------------------------

MY_FORCEINLINE u64 SMaterial::getHashFFP() const
{
	if (HashFFPDirty)
	{
		HashFFPDirty = false;
		HashFFP = 0;

		u32 passcnt = Passes.size();
		for(u32 p = 0; p < passcnt; p++)
			HashFFP ^= Passes[p].getHashFFP();
	}
	return HashFFP;
}

//---------------------------------------------------------------------------

MY_FORCEINLINE u64 SMaterial::getHashGPU() const
{
	if(HashGPUDirty)
	{
		HashGPUDirty = false;
		HashGPU = 0;

		u32 passcnt = Passes.size();
		for(u32 p = 0; p < passcnt; p++)
			HashGPU ^= Passes[p].getHashGPU();
	}
	return HashGPU;
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif  // #ifndef SRenderPassHPP
