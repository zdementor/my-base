//|-------------------------------------------------------------------------
//| File:        CMyScriptManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMyScriptManagerHPP
#define CMyScriptManagerHPP
//----------------------------------------------------------------------------

#include <scr/IScriptManager.h>

#ifndef TOLUA_STATIC
#	define TOLUA_STATIC
#endif
#include <tolua++.h>

//----------------------------------------------------------------------------
namespace my {
namespace scr {
//----------------------------------------------------------------------------

#define MY_LUA_CALL_0() \
do { \
	if (!_luaLookup(f)) return false; \
	return _luaCall(f, 0); \
} while (0)

#define MY_LUA_CALL_1(type0, a0) \
do { \
	if (!_luaLookup(f)) return false; \
	tolua_push##type0(m_LuaState, a0); \
	return _luaCall(f, 1); \
} while (0)

#define  MY_LUA_CALL_2(type0, type1, a0, a1) \
do { \
	if (!_luaLookup(f)) return false; \
	tolua_push##type0(m_LuaState, a0); \
	tolua_push##type1(m_LuaState, a1); \
	return _luaCall(f, 2); \
} while (0)

#define  MY_LUA_CALL_3(type0, type1, type2, a0, a1, arg2) \
do { \
	if (!_luaLookup(f)) return false; \
	tolua_push##type0(m_LuaState, a0); \
	tolua_push##type1(m_LuaState, a1); \
	tolua_push##type2(m_LuaState, arg2); \
	return _luaCall(f, 3); \
} while (0)

//----------------------------------------------------------------------------

//! Script Manager. Used as wrapper for script engine
class CMyScriptManager : public IScriptManager
{
public:

	CMyScriptManager();
	virtual ~CMyScriptManager();

	//! implementation of the IScriptManager interfaces

	virtual bool runScriptText(const c8* script_text);

	virtual bool runScriptFunction(SScriptFunctor *f) { MY_LUA_CALL_0(); }

	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0	) { MY_LUA_CALL_1(boolean,	a0); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0	) { MY_LUA_CALL_1(number,	a0); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0	) { MY_LUA_CALL_1(number,	a0); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0	) { MY_LUA_CALL_1(number,	a0); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0	) { MY_LUA_CALL_1(string,	a0); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0	) { MY_LUA_CALL_1(userdata, a0); }

	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		bool		a1	) { MY_LUA_CALL_2(boolean,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		s32			a1	) { MY_LUA_CALL_2(boolean,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		u32			a1	) { MY_LUA_CALL_2(boolean,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		f32			a1	) { MY_LUA_CALL_2(boolean,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		const c8	*a1	) { MY_LUA_CALL_2(boolean,	string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool		a0,		void		*a1	) { MY_LUA_CALL_2(boolean,	userdata,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		bool		a1	) { MY_LUA_CALL_2(number,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		s32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		u32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		f32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		const c8	*a1	) { MY_LUA_CALL_2(number,	string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32		a0,		void		*a1	) { MY_LUA_CALL_2(number,	userdata,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		bool		a1	) { MY_LUA_CALL_2(number,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		s32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		u32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		f32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		const c8	*a1	) { MY_LUA_CALL_2(number,	string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, u32		a0,		void		*a1	) { MY_LUA_CALL_2(number,	userdata,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		bool		a1	) { MY_LUA_CALL_2(number,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		s32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		u32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		f32			a1	) { MY_LUA_CALL_2(number,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		const c8	*a1	) { MY_LUA_CALL_2(number,	string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32		a0,		void		*a1	) { MY_LUA_CALL_2(number,	userdata,	a0, a1); }
  	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1	) { MY_LUA_CALL_2(string,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1	) { MY_LUA_CALL_2(string,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1	) { MY_LUA_CALL_2(string,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1	) { MY_LUA_CALL_2(string,	number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1	) { MY_LUA_CALL_2(string,	string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1	) { MY_LUA_CALL_2(string,	userdata,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	bool		a1	) { MY_LUA_CALL_2(userdata,	boolean,	a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	s32			a1	) { MY_LUA_CALL_2(userdata, number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	u32			a1	) { MY_LUA_CALL_2(userdata, number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	f32			a1	) { MY_LUA_CALL_2(userdata, number,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	const c8	*a1	) { MY_LUA_CALL_2(userdata, string,		a0, a1); }
	virtual bool runScriptFunction(SScriptFunctor *f, void		*a0,	void		*a1	) { MY_LUA_CALL_2(userdata, userdata,	a0, a1); }

	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		bool		a2	) { MY_LUA_CALL_3(boolean,	boolean,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		s32			a2	) { MY_LUA_CALL_3(boolean,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		u32			a2	) { MY_LUA_CALL_3(boolean,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		f32			a2	) { MY_LUA_CALL_3(boolean,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		const c8	*a2	) { MY_LUA_CALL_3(boolean,	boolean,	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	bool		a1,		void		*a2	) { MY_LUA_CALL_3(boolean,	boolean,	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		bool		a2	) { MY_LUA_CALL_3(boolean,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		s32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		u32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		f32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		const c8	*a2	) { MY_LUA_CALL_3(boolean,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	s32			a1,		void		*a2	) { MY_LUA_CALL_3(boolean,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		bool		a2	) { MY_LUA_CALL_3(boolean,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		s32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		u32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		f32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		const c8	*a2	) { MY_LUA_CALL_3(boolean,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	u32			a1,		void		*a2	) { MY_LUA_CALL_3(boolean,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		bool		a2	) { MY_LUA_CALL_3(boolean,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		s32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		u32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		f32			a2	) { MY_LUA_CALL_3(boolean,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		const c8	*a2	) { MY_LUA_CALL_3(boolean,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	f32			a1,		void		*a2	) { MY_LUA_CALL_3(boolean,	number,		userdata,	a0, a1, a2); }
  	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	bool		a2	) { MY_LUA_CALL_3(boolean,	string,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	s32			a2	) { MY_LUA_CALL_3(boolean,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	u32			a2	) { MY_LUA_CALL_3(boolean,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	f32			a2	) { MY_LUA_CALL_3(boolean,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	const c8	*a2	) { MY_LUA_CALL_3(boolean,	string,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	const c8	*a1,	void		*a2	) { MY_LUA_CALL_3(boolean,	string,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	bool		a2	) { MY_LUA_CALL_3(boolean,	userdata,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	s32			a2	) { MY_LUA_CALL_3(boolean,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	u32			a2	) { MY_LUA_CALL_3(boolean,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	f32			a2	) { MY_LUA_CALL_3(boolean,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	const c8	*a2	) { MY_LUA_CALL_3(boolean,	userdata, 	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, bool	a0,	void		*a1,	void		*a2	) { MY_LUA_CALL_3(boolean,	userdata, 	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		bool		a2	) { MY_LUA_CALL_3(number,	boolean,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		s32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		u32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		f32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	boolean,	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	bool		a1,		void		*a2	) { MY_LUA_CALL_3(number,	boolean,	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	s32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	u32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	f32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
  	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	bool		a2	) { MY_LUA_CALL_3(number,	string,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	s32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	u32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	f32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	const c8	*a2	) { MY_LUA_CALL_3(number,	string,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	const c8	*a1,	void		*a2	) { MY_LUA_CALL_3(number,	string,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	bool		a2	) { MY_LUA_CALL_3(number,	userdata,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	s32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	u32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	f32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	const c8	*a2	) { MY_LUA_CALL_3(number,	userdata, 	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, s32	a0,	void		*a1,	void		*a2	) { MY_LUA_CALL_3(number,	userdata, 	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		bool		a2	) { MY_LUA_CALL_3(number,	boolean,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		s32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		u32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		f32			a2	) { MY_LUA_CALL_3(number,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	boolean,	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	bool		a1,		void		*a2	) { MY_LUA_CALL_3(number,	boolean,	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	s32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	u32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		bool		a2	) { MY_LUA_CALL_3(number,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		s32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		u32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		f32			a2	) { MY_LUA_CALL_3(number,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		const c8	*a2	) { MY_LUA_CALL_3(number,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	f32			a1,		void		*a2	) { MY_LUA_CALL_3(number,	number,		userdata,	a0, a1, a2); }
  	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	bool		a2	) { MY_LUA_CALL_3(number,	string,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	s32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	u32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	f32			a2	) { MY_LUA_CALL_3(number,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	const c8	*a2	) { MY_LUA_CALL_3(number,	string,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	const c8	*a1,	void		*a2	) { MY_LUA_CALL_3(number,	string,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	bool		a2	) { MY_LUA_CALL_3(number,	userdata,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	s32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	u32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	f32			a2	) { MY_LUA_CALL_3(number,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	const c8	*a2	) { MY_LUA_CALL_3(number,	userdata, 	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, f32	a0,	void		*a1,	void		*a2	) { MY_LUA_CALL_3(number,	userdata, 	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		bool		a2	) { MY_LUA_CALL_3(string,	boolean,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		s32			a2	) { MY_LUA_CALL_3(string,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		u32			a2	) { MY_LUA_CALL_3(string,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		f32			a2	) { MY_LUA_CALL_3(string,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		const c8	*a2	) { MY_LUA_CALL_3(string,	boolean,	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	bool		a1,		void		*a2	) { MY_LUA_CALL_3(string,	boolean,	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		bool		a2	) { MY_LUA_CALL_3(string,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		s32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		u32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		f32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		const c8	*a2	) { MY_LUA_CALL_3(string,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	s32			a1,		void		*a2	) { MY_LUA_CALL_3(string,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		bool		a2	) { MY_LUA_CALL_3(string,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		s32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		u32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		f32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		const c8	*a2	) { MY_LUA_CALL_3(string,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	u32			a1,		void		*a2	) { MY_LUA_CALL_3(string,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		bool		a2	) { MY_LUA_CALL_3(string,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		s32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		u32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		f32			a2	) { MY_LUA_CALL_3(string,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		const c8	*a2	) { MY_LUA_CALL_3(string,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	f32			a1,		void		*a2	) { MY_LUA_CALL_3(string,	number,		userdata,	a0, a1, a2); }
  	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	bool		a2	) { MY_LUA_CALL_3(string,	string,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	s32			a2	) { MY_LUA_CALL_3(string,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	u32			a2	) { MY_LUA_CALL_3(string,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	f32			a2	) { MY_LUA_CALL_3(string,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	const c8	*a2	) { MY_LUA_CALL_3(string,	string,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	const c8	*a1,	void		*a2	) { MY_LUA_CALL_3(string,	string,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	bool		a2	) { MY_LUA_CALL_3(string,	userdata,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	s32			a2	) { MY_LUA_CALL_3(string,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	u32			a2	) { MY_LUA_CALL_3(string,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	f32			a2	) { MY_LUA_CALL_3(string,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	const c8	*a2	) { MY_LUA_CALL_3(string,	userdata, 	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, const c8	*a0,	void		*a1,	void	*a2	) { MY_LUA_CALL_3(string,	userdata, 	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		bool		a2	) { MY_LUA_CALL_3(userdata,	boolean,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		s32			a2	) { MY_LUA_CALL_3(userdata,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		u32			a2	) { MY_LUA_CALL_3(userdata,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		f32			a2	) { MY_LUA_CALL_3(userdata,	boolean,	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		const c8	*a2	) { MY_LUA_CALL_3(userdata,	boolean,	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	bool		a1,		void		*a2	) { MY_LUA_CALL_3(userdata,	boolean,	userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		bool		a2	) { MY_LUA_CALL_3(userdata,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		s32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		u32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		f32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		const c8	*a2	) { MY_LUA_CALL_3(userdata,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	s32			a1,		void		*a2	) { MY_LUA_CALL_3(userdata,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		bool		a2	) { MY_LUA_CALL_3(userdata,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		s32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		u32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		f32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		const c8	*a2	) { MY_LUA_CALL_3(userdata,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	u32			a1,		void		*a2	) { MY_LUA_CALL_3(userdata,	number,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		bool		a2	) { MY_LUA_CALL_3(userdata,	number,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		s32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		u32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		f32			a2	) { MY_LUA_CALL_3(userdata,	number,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		const c8	*a2	) { MY_LUA_CALL_3(userdata,	number,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	f32			a1,		void		*a2	) { MY_LUA_CALL_3(userdata,	number,		userdata,	a0, a1, a2); }
  	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	bool		a2	) { MY_LUA_CALL_3(userdata,	string,		boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	s32			a2	) { MY_LUA_CALL_3(userdata,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	u32			a2	) { MY_LUA_CALL_3(userdata,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	f32			a2	) { MY_LUA_CALL_3(userdata,	string,		number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	const c8	*a2	) { MY_LUA_CALL_3(userdata,	string,		string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	const c8	*a1,	void		*a2	) { MY_LUA_CALL_3(userdata,	string,		userdata,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	bool		a2	) { MY_LUA_CALL_3(userdata,	userdata,	boolean,	a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	s32			a2	) { MY_LUA_CALL_3(userdata,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	u32			a2	) { MY_LUA_CALL_3(userdata,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	f32			a2	) { MY_LUA_CALL_3(userdata,	userdata, 	number,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	const c8	*a2	) { MY_LUA_CALL_3(userdata,	userdata, 	string,		a0, a1, a2); }
	virtual bool runScriptFunction(SScriptFunctor *f, void	*a0,	void		*a1,	void		*a2	) { MY_LUA_CALL_3(userdata,	userdata, 	userdata,	a0, a1, a2); }

	virtual void setScriptCallback(E_SCRIPT_CALLBACK_TYPE type, const c8 *func);

	//! implementation of the ICoreScriptManagerInterface interfaces

	virtual bool runScript(const c8* filename)
	{ return m_CoreScripter ? m_CoreScripter->runScript(filename) : 0; }

	virtual void clearScriptMemory()
	{ m_CoreScripter ? m_CoreScripter->clearScriptMemory() : 0; }

	virtual u32 getScriptMemoryKBytes()
	{ return m_CoreScripter ? m_CoreScripter->getScriptMemoryKBytes() : 0; }

	virtual void setScriptMaxMemoryKBytes(u32 max_kbytes)
	{ m_CoreScripter ? m_CoreScripter->setScriptMaxMemoryKBytes(max_kbytes) : 0; }

	virtual void* getCoreScripterPtr()
	{ return m_CoreScripter ? m_CoreScripter->getCoreScripterPtr() : 0; }

	virtual void setScriptsFullDir(const c8* dir)
	{ m_CoreScripter ? m_CoreScripter->setScriptsFullDir(dir) : 0; }
	
	virtual const c8* getScriptsFullDir()
	{ return m_CoreScripter ? m_CoreScripter->getScriptsFullDir() : 0; }

protected:

	inline bool _luaLookup(SScriptFunctor *functor);	
	inline bool _luaCall(SScriptFunctor *functor, s32 nargs);

	lua_State *m_LuaState;

	ICoreScriptManager *m_CoreScripter;
};

//----------------------------------------------------------------------------
} // end namespace scr
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CMyScriptManagerHPP 