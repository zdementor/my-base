//|-------------------------------------------------------------------------
//| File:        IScriptManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IScriptManagerHPP
#define IScriptManagerHPP
//----------------------------------------------------------------------------

#include <scr/ICoreScriptManager.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

struct SScriptFunctor
{
	SScriptFunctor()
		: FunctionName(""), NeedLookup(true),
		ResultBool(false), ResultString(""),
		ResultFloat(0.f), ResultInt(0) {}
	SScriptFunctor(const c8* func)
		: FunctionName(func), NeedLookup(true),
		ResultBool(false), ResultString(""),
		ResultFloat(0.f), ResultInt(0){}

	core::stringc	FunctionName;
	bool			NeedLookup;
	s32				FunctionIndex;

	bool			ResultBool;
	core::stringc	ResultString;
	f32				ResultFloat;
	s32				ResultInt;
};

//----------------------------------------------------------------------------

//! Script Manager. Used as wrapper for script engine
class __MY_SCR_LIB_API__ IScriptManager :
	public ICoreScriptManagerInterface, public ISingleton <IScriptManager>
{
public:

	//! parsing script text
	virtual bool runScriptText(const c8* script_text) = 0;

	//! running script callbacks, with different types of arguments

	virtual bool runScriptFunction(SScriptFunctor *f) = 0;

	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0	) = 0;

	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		void		*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		void		*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		void		*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		void		*a1	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	bool		a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	s32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	u32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	f32			a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	const c8	*a1	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	void		*a1	) = 0;

	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		void		*a2	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		void		*a2	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		void		*a2	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		void		*a2	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		void		*a2	) = 0;
  	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	void		*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	bool		a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	s32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	u32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	f32			a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	const c8	*a2	) = 0;
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	void		*a2	) = 0;

	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, bool *result)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type]);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultBool;
		return r;
	}
	template <class T0>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, bool *result, T0 a0)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultBool;
		return r;
	}
	template <class T0, class T1>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, bool *result, T0 a0, T1 a1)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultBool;
		return r;
	}
	template <class T0, class T1, class T2>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, bool *result, T0 a0, T1 a1, T2 a2)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1, a2);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultBool;
		return r;
	}

	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, s32 *result)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type]);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultInt;
		return r;
	}
	template <class T0>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, s32 *result, T0 a0)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultInt;
		return r;
	}
	template <class T0, class T1>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, s32 *result, T0 a0, T1 a1)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultInt;
		return r;
	}
	template <class T0, class T1, class T2>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, s32 *result, T0 a0, T1 a1, T2 a2)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1, a2);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultInt;
		return r;
	}

	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, f32 *result)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type]);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultFloat;
		return r;
	}
	template <class T0>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, f32 *result, T0 a0)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultFloat;
		return r;
	}
	template <class T0, class T1>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, f32 *result, T0 a0, T1 a1)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultFloat;
		return r;
	}
	template <class T0, class T1, class T2>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, f32 *result, T0 a0, T1 a1, T2 a2)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1, a2);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultFloat;
		return r;
	}

	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 **result)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type]);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultString.c_str();
		return r;
	}
	template <class T0>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 **result, T0 a0)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultString.c_str();
		return r;
	}
	template <class T0, class T1>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 **result, T0 a0, T1 a1)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultString.c_str();
		return r;
	}
	template <class T0, class T1, class T2>
	bool runScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 **result, T0 a0, T1 a1, T2 a2)
	{
		bool r = runScriptFunction(m_ScriptCallbacks[type], a0, a1, a2);
		if (r && result) *result = m_ScriptCallbacks[type]->ResultString.c_str();
		return r;
	}

	//! set script callback
	virtual void setScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 *func) = 0;

	//!	Return singleton object
	static IScriptManager& getSingleton();

	//! Return pointer to singleton object
	static IScriptManager* getSingletonPtr();

protected:

	SScriptFunctor *m_ScriptCallbacks[ESCT_SCRIPT_CALLBACK_TYPE_COUNT];
};

//----------------------------------------------------------------------------

__MY_SCR_LIB_API__ IScriptManager* createMyScriptManager();

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IScriptManagerHPP