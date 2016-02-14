
rootdir = ".."

dopackage(rootdir.."/deps/premake_common.lua")

local MY_PRJ_DIR = rootdir.."/build/"

local APPS_DIR = rootdir.."/base/apps"

project.name = "MyEngine_Sources"
project.path = MY_PRJ_DIR

package = newpackage()
InitPackage("MyCore", MY_PRJ_DIR, "c++", "dll", "",
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
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/inc/core/*.h"),
		matchfiles(SRC_DIR.."/core/*.cpp"),
		matchfiles(SRC_DIR.."/core/*.h"),
		matchfiles(SRC_DIR.."/inc/os/*.h"),
		matchfiles(SRC_DIR.."/os/*.cpp"),
		matchfiles(SRC_DIR.."/os/*.h"),
		matchfiles(SRC_DIR.."/inc/io/*.h"),
		matchfiles(SRC_DIR.."/io/*.cpp"),
		matchfiles(SRC_DIR.."/io/*.h"),
		matchfiles(SRC_DIR.."/inc/img/*.h"),
		matchfiles(SRC_DIR.."/img/*.cpp"),
		matchfiles(SRC_DIR.."/img/*.h")
	},
		{},
	{BASE_INC_PATH, ZLIB_INC_DIR, PNG_INC_DIR, JPEG_INC_DIR, DEVIL_INC_DIR},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyVideoGL21", MY_PRJ_DIR, "c++", "dll", "",
	{ "MyCore", },
		{ FTYPE_DEP, },
			{},
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_GL21_LIB__", "GLEW_STATIC"}, {}, {},
	{
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/vid/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/vid/*.cpp"),
		matchfiles(SRC_DIR.."/vid/*.h"),
		matchrecursive(SRC_DIR.."/vid/gl/*.cpp"),
		matchrecursive(SRC_DIR.."/vid/gl/*.h"),
		SRC_DIR.."/vid/gl/glew.c",
	},
		{},
	{BASE_INC_PATH, FTYPE_INC_DIR},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyVideoGL12", MY_PRJ_DIR, "c++", "dll", "",
	{"MyCore", },
		{ FTYPE_DEP, },
			{},
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_GL12_LIB__", "GLEW_STATIC"}, {}, {},
	{
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/vid/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/vid/*.cpp"),
		matchfiles(SRC_DIR.."/vid/*.h"),
		matchrecursive(SRC_DIR.."/vid/gl/*.cpp"),
		matchrecursive(SRC_DIR.."/vid/gl/*.h"),
		SRC_DIR.."/vid/gl/glew.c",
	},
		{},
	{BASE_INC_PATH, FTYPE_INC_DIR},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyVideoGL11", MY_PRJ_DIR, "c++", "dll", "",
	{"MyCore", },
		{ FTYPE_DEP,},
			{},
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_GL11_LIB__", "GLEW_STATIC"}, {}, {},
	{
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/vid/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/vid/*.cpp"),
		matchfiles(SRC_DIR.."/vid/*.h"),
		matchrecursive(SRC_DIR.."/vid/gl/*.cpp"),
		matchrecursive(SRC_DIR.."/vid/gl/*.h"),
		SRC_DIR.."/vid/gl/glew.c",
	},
		{},
	{BASE_INC_PATH, FTYPE_INC_DIR},
		BASE_LIB_PATH)
		
package = newpackage()
InitPackage("MyVideoDX9", MY_PRJ_DIR, "c++", "dll", "",
	{"MyCore", },
		{ FTYPE_DEP, },
			{},
	{"__MY_BUILD_VID_LIB__", "__MY_BUILD_VID_DX9_LIB__"}, {}, {},
	{	
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/vid/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/vid/*.cpp"),
		matchfiles(SRC_DIR.."/vid/*.h"),
		matchrecursive(SRC_DIR.."/vid/dx9/*.cpp"),
		matchrecursive(SRC_DIR.."/vid/dx9/*.h"),
	},
		{},
	{BASE_INC_PATH, FTYPE_INC_DIR},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyEngine", MY_PRJ_DIR, "c++", "dll", "",
	{ "MyCore", "MyVideoDX9", "MyVideoGL11", "MyVideoGL12", "MyVideoGL21", },
		{ VORB_DEP, OGG_DEP, CAL3D_DEP, MINI_DEP, ODE_DEP, LUA_DLL_DEP, TOLUA_DLL_DEP, },
			{"openal32", "alut"},
	{
		"__MY_BUILD_VID_LIB__",
		"__MY_BUILD_VID_NULL_LIB__",
		"__MY_BUILD_MM_LIB__",
		"__MY_BUILD_SCN_LIB__",
		"__MY_BUILD_DYN_LIB__",
		"__MY_BUILD_GAME_LIB__",
		"__MY_BUILD_SCR_LIB__", "TOLUA_STATIC",
		"__MY_BUILD_RES_LIB__",
		"__MY_BUILD_DEV_LIB__"
	},  {"TOLUA_RELEASE"}, {},
	{	matchfiles(SRC_DIR.."/inc/*.h"),
		SRC_DIR.."/MyVersion.h",
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/inc/vid/*.h"),
		matchfiles(SRC_DIR.."/vid/*.cpp"),
		matchfiles(SRC_DIR.."/vid/*.h"),
		matchfiles(SRC_DIR.."/inc/gui/*.h"),
		matchfiles(SRC_DIR.."/inc/mm/*.h"),
		matchfiles(SRC_DIR.."/mm/*.cpp"),
		matchfiles(SRC_DIR.."/mm/*.h"),
		matchfiles(SRC_DIR.."/inc/scn/*.h"),
		matchfiles(SRC_DIR.."/scn/*.cpp"),
		matchfiles(SRC_DIR.."/scn/*.h"),
		matchfiles(SRC_DIR.."/scn/*.hpp"),
		matchfiles(SRC_DIR.."/scn/nodes/*.cpp"),
		matchfiles(SRC_DIR.."/scn/nodes/*.h"),
		matchfiles(SRC_DIR.."/scn/nodes/*.hpp"),
		matchfiles(SRC_DIR.."/inc/dyn/*.h"),
		matchrecursive(SRC_DIR.."/dyn/*.cpp"),
		matchrecursive(SRC_DIR.."/dyn/*.h"),
		matchfiles(SRC_DIR.."/inc/game/*.h"),
		matchrecursive(SRC_DIR.."/game/*.cpp"),
		matchrecursive(SRC_DIR.."/game/*.h"),
		matchfiles(SRC_DIR.."/inc/scr/*.h"),
		SRC_DIR.."/scr/CMyScriptManager.cpp",
		SRC_DIR.."/scr/CMyScriptManager.h",
		matchfiles(SRC_DIR.."/inc/dev/*.h"),
		matchfiles(SRC_DIR.."/dev/*.cpp"),
		matchfiles(SRC_DIR.."/dev/*.h"),
		matchfiles(SRC_DIR.."/inc/res/*.h"),
		matchfiles(SRC_DIR.."/res/*.cpp"),
		matchfiles(SRC_DIR.."/res/*.h"),
	},
		{matchfiles(SRC_DIR.."/vid/CFontTTF.*")},
	{
		BASE_INC_PATH,
		VORB_INC_DIR,
		OGG_INC_DIR,
		OAL_INC_DIR,
		CAL3D_INC_DIR,
		MINI_INC_DIR,
		ODE_INC_DIR,
		LUA_INC_DIR,
		TOLUA_INC_DIR,
	},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyCEGUI", MY_PRJ_DIR, "c++", "dll", "",
	{ "MyCore", },
		{ CEGUIBASE_DEP, CEGUILUA_DEP, CEGUIXML_DEP, CEGUIFAL_DEP, CEGUIIMG_DEP, },
			{},
	{"__MY_BUILD_GUI_LIB__", "MYRENDERER_EXPORTS"}, {}, {},
	{
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/gui/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		matchfiles(SRC_DIR.."/gui/*.h"),
		matchfiles(SRC_DIR.."/gui/*.cpp"),
		matchfiles(SRC_DIR.."/gui/iniFile/*.cpp"),
		matchfiles(SRC_DIR.."/gui/iniFile/*.h"),
	},
		{},
	{BASE_INC_PATH, CEGUI_INC_DIR, CEGUI_LUA_INC_DIR},
		BASE_LIB_PATH)

package = newpackage()
InitPackage("MyCoreScript", MY_PRJ_DIR, "c++", "dll", "",
	{ "MyCore", "MyEngine", "MyCEGUI", },
		{ LUA_DLL_DEP, TOLUA_DLL_DEP, },
			{},
	{"__MY_BUILD_SCR_LIB__", "TOLUA_STATIC"}, {"TOLUA_RELEASE"}, {},
	{
		matchfiles(SRC_DIR.."/inc/*.h"),
		matchfiles(SRC_DIR.."/inc/scr/*.h"),
		SRC_DIR.."/CompileConf.h",
		SRC_DIR.."/MyDllEntry.cpp",
		SRC_DIR.."/MySingletons.cpp",
		SRC_DIR.."/scr/CCoreScriptManager.cpp",
		SRC_DIR.."/scr/CCoreScriptManager.h",
		SRC_DIR.."/scr/MyLuaBind.cpp",
		SRC_DIR.."/scr/MyLuaBind.h",
		matchrecursive(SRC_DIR.."/scr/package/*.pkg"),
	},
		{},
	{BASE_INC_PATH, TOLUA_INC_DIR, LUA_INC_DIR, CEGUI_INC_DIR}, BASE_LIB_PATH)
	
package = newpackage()
InitPackage("Test01", MY_PRJ_DIR, "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		matchrecursive(SRC_DIR.."/inc/*.h"),
		matchrecursive(APPS_DIR.."/Tests/Test01.cpp"),
	},
		{},
	BASE_INC_PATH, BASE_LIB_PATH)
	
package = newpackage()
InitPackage("Test02", MY_PRJ_DIR, "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		matchrecursive(SRC_DIR.."/inc/*.h"),
		matchrecursive(APPS_DIR.."/Tests/Test02.cpp"),
	},
		{},
	BASE_INC_PATH, BASE_LIB_PATH)

package = newpackage()
InitPackage("Test03", MY_PRJ_DIR, "c++", "exe", "",
	{ "MyCore", "MyEngine", },
		{},
			{},
	{	"__MY_USE_CORE_LIB__", "__MY_USE_IO_LIB__", "__MY_USE_OS_LIB__",
		"__MY_USE_IMG_LIB__", "__MY_USE_SCN_LIB__", "__MY_USE_MM_LIB__",
		"__MY_USE_VID_LIB__", "__MY_USE_GAME_LIB__", "__MY_USE_DYN_LIB__",
		"__MY_USE_SCR_LIB__", "__MY_USE_GUI_LIB__", "__MY_USE_DEV_LIB__"
	}, {}, {},
	{
		matchrecursive(SRC_DIR.."/inc/*.h"),
		matchrecursive(APPS_DIR.."/Tests/Test03.cpp"),
	},
		{},
	BASE_INC_PATH, BASE_LIB_PATH)

os.execute("svn up")
os.execute("svn info > svninfo.txt")
local file = io.open ("svninfo.txt", "r")
if file~=nil then    
	for line in file:lines() do
		local subStr = "Revision:"
		local idx = string.find(line, subStr)
		if idx ~= nil then
			local revStr = string.sub(line, idx + string.len(subStr), string.len(line))
			local rev = nil
			 for w in string.gfind(line, "%d+") do
				rev= w
			end
			local fileOut = io.open (SRC_DIR.."/MyVersion.h", "w")
			if fileOut ~= nil then
				fileOut:write(string.format("#define MY_ENGINE_VERSION_STRING \"0.2.%s\"", rev))
				fileOut:close()
			end			
		end
	end
	file:close()
end
	