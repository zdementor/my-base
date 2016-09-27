
ROOT_DIR = ".."

dofile(ROOT_DIR.."/deps/premake_common.lua")

local MY_PRJ_DIR = ROOT_DIR.."/build"
local MY_PRJ_NAME = "MyEngine_Sources"

local APPS_DIR = ROOT_DIR.."/base/apps"
local SRC_DIR  = ROOT_DIR.."/base/src"

local BASE_INC_DIR = { SRC_DIR.."/inc", DEPS_DIR, SRC_DIR.."/vid/gl", }
local BASE_LIB_DIR = { DEPS_DIR.."/lib/static/"..ARCH, DEPS_DIR.."/lib/dynamic/"..ARCH, LIBS_DIR }

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyCore", "c++", "dll", "",
	{},
		{ ZLIB_DEP, PNG_DEP, JPEG_DEP, DEVIL_DEP},
			{},
	{	
		"__MY_BUILD_CORE_LIB__",
		"__MY_BUILD_OS_LIB__",
		"__MY_BUILD_IO_LIB__",
		"__MY_BUILD_IMG_LIB__",
		"IL_STATIC_LIB"
	}, {}, {},
	{
		SRC_DIR.."/inc/IUnknown.h",
		SRC_DIR.."/inc/ForwardDecl.h",
		SRC_DIR.."/inc/my.h",
		SRC_DIR.."/inc/my_inc.h",
		SRC_DIR.."/inc/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/inc/**.h",
		SRC_DIR.."/core/*.cpp",
		SRC_DIR.."/core/*.h",
		SRC_DIR.."/inc/os/*.h",
		SRC_DIR.."/os/*.cpp",
		SRC_DIR.."/os/*.h",
		SRC_DIR.."/inc/io/*.h",
		SRC_DIR.."/io/*.cpp",
		SRC_DIR.."/io/*.h",
		SRC_DIR.."/inc/img/*.h",
		SRC_DIR.."/img/*.cpp",
		SRC_DIR.."/img/*.h"
	},
		{},
	{ BASE_INC_DIR, ZLIB_INC_DIR, PNG_INC_DIR, JPEG_INC_DIR, DEVIL_INC_DIR },
		{ BASE_LIB_DIR}
)

local GL_DEPS = {}

if os.is("windows") then
	GL_DEPS = { "opengl32", "glu32" }
end

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyVideoGL21", "c++", "dll", "",
	{ "MyCore", },
		{ FTYPE_DEP, },
			{ GL_DEPS },
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_GL21_LIB__", "GLEW_STATIC"}, {}, {},
	{
		SRC_DIR.."/inc/*.h",
		SRC_DIR.."/inc/vid/*.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/vid/*.cpp",
		SRC_DIR.."/vid/*.h",
		SRC_DIR.."/vid/gl/**.cpp",
		SRC_DIR.."/vid/gl/**.h",
		SRC_DIR.."/vid/gl/glew.c",
	},
		{},
	{ BASE_INC_DIR, FTYPE_INC_DIR },
		{ BASE_LIB_DIR }
)

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyVideoGL11", "c++", "dll", "",
	{"MyCore", },
		{ FTYPE_DEP,},
			{ GL_DEPS },
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_GL11_LIB__", "GLEW_STATIC"}, {}, {},
	{
		SRC_DIR.."/inc/*.h",
		SRC_DIR.."/inc/vid/*.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/vid/*.cpp",
		SRC_DIR.."/vid/*.h",
		SRC_DIR.."/vid/gl/**.cpp",
		SRC_DIR.."/vid/gl/**.h",
		SRC_DIR.."/vid/gl/glew.c",
	},
		{},
	{ BASE_INC_DIR, FTYPE_INC_DIR },
		{ BASE_LIB_DIR }
)

if os.is("windows") then
	local DX9_DEPS = { "d3dx9" }
	InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
		"MyVideoDX9", "c++", "dll", "",
		{"MyCore", },
			{ FTYPE_DEP, },
				{ DX9_DEPS },
		{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_DX9_LIB__"}, {}, {},
		{	
			SRC_DIR.."/inc/*.h",
			SRC_DIR.."/inc/vid/*.h",
			SRC_DIR.."/MyDllEntry.cpp",
			SRC_DIR.."/MySingletons.cpp",
			SRC_DIR.."/vid/*.cpp",
			SRC_DIR.."/vid/*.h",
			SRC_DIR.."/vid/dx9/**.cpp",
			SRC_DIR.."/vid/dx9/**.h",
		},
			{},
		{ BASE_INC_DIR, DEPS_DIR.."/d3d/"..ARCH, FTYPE_INC_DIR },
			{ BASE_LIB_DIR },
		{}, {"/NODEFAULTLIB:d3dx8.lib"}, nil, nil,
		nil, nil
	)
end

local MY_ENGINE_PRJDEPS = { "MyCore", "MyVideoGL11", "MyVideoGL21", }
local MY_ENGINE_DEPS    = { VORB_DEP, OGG_DEP, CAL3D_DEP, MINI_DEP, ODE_DEP, LUA_DLL_DEP, TOLUA_DLL_DEP, }
local MY_ENGINE_SYSDEPS = {}

if os.is("windows") then
	table.insert(MY_ENGINE_PRJDEPS, "MyVideoDX9")
	table.insert(MY_ENGINE_SYSDEPS, "openal32")
	for key, value in pairs({ "openal32.dll", "d3dx9_36.dll"}) do
		local dstpath = ROOT_DIR.."/bin/"..ARCH
		local dst = dstpath.."/"..value
		local src = DEPS_DIR.."/lib/dynamic/"..ARCH.."/"..value
		if not os.isdir(dstpath) then
			os.mkdir(dstpath)
		end
		if not os.isfile(dst) then
			io.write(string.format("Copy %s to %s.\n", src, dst))
			os.copyfile(src, dst)
		end
	end
end

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyEngine", "c++", "dll", "",
	{ MY_ENGINE_PRJDEPS, },
		{ MY_ENGINE_DEPS, },
			{ MY_ENGINE_SYSDEPS, },
	{
		"__MY_BUILD_VID_LIB__",
		"__MY_BUILD_VID_NULL_LIB__",
		"__MY_BUILD_MM_LIB__",
		"__MY_BUILD_SCN_LIB__",
		"__MY_BUILD_DYN_LIB__",
		"__MY_BUILD_GAME_LIB__",
		"__MY_BUILD_SCR_LIB__",
		"TOLUA_STATIC",
		"__MY_BUILD_RES_LIB__",
		"__MY_BUILD_DEV_LIB__",
	},  { "TOLUA_RELEASE" }, {},
	{	SRC_DIR.."/inc/*.h",
		SRC_DIR.."/MyVersion.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/inc/vid/*.h",
		SRC_DIR.."/vid/*.cpp",
		SRC_DIR.."/vid/*.h",
		SRC_DIR.."/inc/gui/*.h",
		SRC_DIR.."/inc/mm/*.h",
		SRC_DIR.."/mm/*.cpp",
		SRC_DIR.."/mm/*.h",
		SRC_DIR.."/inc/scn/*.h",
		SRC_DIR.."/scn/*.cpp",
		SRC_DIR.."/scn/*.h",
		SRC_DIR.."/scn/*.hpp",
		SRC_DIR.."/scn/nodes/*.cpp",
		SRC_DIR.."/scn/nodes/*.h",
		SRC_DIR.."/scn/nodes/*.hpp",
		SRC_DIR.."/inc/dyn/*.h",
		SRC_DIR.."/dyn/**.cpp",
		SRC_DIR.."/dyn/**.h",
		SRC_DIR.."/inc/game/*.h",
		SRC_DIR.."/game/**.cpp",
		SRC_DIR.."/game/**.h",
		SRC_DIR.."/inc/scr/*.h",
		SRC_DIR.."/scr/CMyScriptManager.cpp",
		SRC_DIR.."/scr/CMyScriptManager.h",
		SRC_DIR.."/inc/dev/*.h",
		SRC_DIR.."/dev/*.cpp",
		SRC_DIR.."/dev/*.h",
		SRC_DIR.."/inc/res/*.h",
		SRC_DIR.."/res/*.cpp",
		SRC_DIR.."/res/*.h",
	},
		{SRC_DIR.."/vid/CFontTTF.*"},
	{
		BASE_INC_DIR,
		VORB_INC_DIR,
		OGG_INC_DIR,
		OAL_INC_DIR,
		CAL3D_INC_DIR,
		MINI_INC_DIR,
		ODE_INC_DIR, ODE_SRC_DIR,
		LUA_INC_DIR,
		TOLUA_INC_DIR,
	},
		{ BASE_LIB_DIR }
)

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyCEGUI", "c++", "dll", "",
	{ "MyCore", },
		{ CEGUIBASE_DEP, CEGUILUA_DEP, CEGUIXML_DEP, CEGUIFAL_DEP, CEGUIIMG_DEP, },
			{},
	{"__MY_BUILD_GUI_LIB__", "MYRENDERER_EXPORTS"}, {}, {},
	{
		SRC_DIR.."/inc/*.h",
		SRC_DIR.."/inc/gui/*.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/gui/*.h",
		SRC_DIR.."/gui/*.cpp",
		SRC_DIR.."/gui/iniFile/*.cpp",
		SRC_DIR.."/gui/iniFile/*.h",
	},
		{},
	{ BASE_INC_DIR, CEGUI_INC_DIR, CEGUI_LUA_INC_DIR },
		{ BASE_LIB_DIR }
)

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"MyCoreScript", "c++", "dll", "",
	{ "MyCore", "MyEngine", "MyCEGUI", },
		{LUA_DLL_DEP, TOLUA_DLL_DEP, },
			{},
	{"__MY_BUILD_SCR_LIB__", "TOLUA_STATIC", "LUA_BUILD_AS_DLL"}, {"TOLUA_RELEASE"}, {},
	{
		SRC_DIR.."/inc/*.h",
		SRC_DIR.."/inc/scr/*.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/scr/CCoreScriptManager.cpp",
		SRC_DIR.."/scr/CCoreScriptManager.h",
		SRC_DIR.."/scr/MyLuaBind.cpp",
		SRC_DIR.."/scr/MyLuaBind.h",
		SRC_DIR.."/scr/package/**.pkg",
	},
		{},
	{ BASE_INC_DIR, TOLUA_INC_DIR, LUA_INC_DIR, LUA_ADD_INC_DIR, CEGUI_INC_DIR },
		{ BASE_LIB_DIR }
)
	
InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"Test01", "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		SRC_DIR.."/inc/**.h",
		APPS_DIR.."/Tests/Test01.cpp",
	},
		{},
	{ BASE_INC_DIR },
		{ BASE_LIB_DIR }
)
	
InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"Test02", "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		SRC_DIR.."/inc/**.h",
		APPS_DIR.."/Tests/Test02.cpp",
	},
		{},
	{ BASE_INC_DIR },
		{ BASE_LIB_DIR }
)

InitPackage(MY_PRJ_NAME, MY_PRJ_DIR,
	"Test03", "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		SRC_DIR.."/inc/**.h",
		APPS_DIR.."/Tests/Test03.cpp",
	},
		{},
	{ BASE_INC_DIR },
		{ BASE_LIB_DIR }
)
