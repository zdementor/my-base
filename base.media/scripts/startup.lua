
require "global"

MyDevice	= nil
MyProfiler	= nil
MyDriver	= nil
MyDynMgr	= nil
MyResMgr	= nil
MyCursor	= nil
MyLogger	= nil
MyScript	= nil
MyScnMgr	= nil
MyGameMgr	= nil
MyLogger	= nil
MyMatMgr	= nil
MyInpDisp	= nil
MyFileSys	= nil
MyMatMgr	= nil
MyPluginMgr	= nil
MyImgLib	= nil
MyCullSys	= nil
MySoundDrv	= nil
MyGameTasksMgr	= nil
MyGameEventDisp	= nil
MyGameAI	= nil
MyTimer		= nil
MyMemStat	= nil

CEGUISystem		= nil
CEGUIRenderer	= nil
CEGUISchemeMgr	= nil
CEGUIImgsetMgr	= nil
CEGUIWinMgr		= nil
CEGUICursor		= nil
CEGUIFontMgr	= nil

function RereadSingletons()
	MyDevice	= dev.IDevice:getSingleton()
	MyProfiler	= dev.IProfiler:getSingleton()
	MyDriver	= vid.IVideoDriver:getSingleton()
	MyDynMgr	= dyn.IDynamicManager:getSingleton()
	MyResMgr	= res.IResourceManager:getSingleton()
	MyCursor	= io.ICursorControl:getSingleton()
	MyLogger	= io.ILogger:getSingleton()
	MyScript	= scr.IScriptManager:getSingleton()
	MyScnMgr	= scn.ISceneManager:getSingleton()
	MyGameMgr	= game.IGameManager:getSingleton()
	MyLogger	= io.ILogger:getSingleton()
	MyMatMgr	= scn.IMaterialsManager:getSingleton()
	MyInpDisp	= io.IInputDispatcher:getSingleton()
	MyFileSys	= io.IFileSystem:getSingleton()
	MyMatMgr	= scn.IMaterialsManager:getSingleton()
	MyPluginMgr	= dev.IPluginManager:getSingleton()
	MyImgLib	= img.IImageLibrary:getSingleton()
	MyCullSys	= scn.ICullingSystem:getSingleton()
	MySoundDrv	= mm.ISoundDriver:getSingleton()
	MyGameTasksMgr	= game.IGameTasksManager:getSingleton()
	MyGameEventDisp = game.IGameEventsDispatcher:getSingleton()
	MyGameAI	= game.IGameAIModule:getSingleton()
	MyTimer		= os.ITimerMain:getSingleton()
	MyMemStat	= os.IMemStatus:getSingleton()

	CEGUISystem		= CEGUI.System:getSingleton()
	CEGUIRenderer	= CEGUISystem:getRenderer()
	CEGUISchemeMgr	= CEGUI.SchemeManager:getSingleton()
	CEGUIImgsetMgr	= CEGUI.ImagesetManager:getSingleton()
	CEGUIWinMgr		= CEGUI.WindowManager:getSingleton()
	CEGUICursor		= CEGUI.MouseCursor:getSingleton()
	CEGUIFontMgr	= CEGUI.FontManager:getSingleton()
end

function SetupResources()
	for k, v in ipairs(OPTIONS.MediaDirectories) do
		MyResMgr:setMediaDirRel(v.media, v.dir)
	end
	for k, v in ipairs(OPTIONS.CommonMediaDirectories) do
		MyResMgr:setCommonMediaDirRel(v.media, v.dir)
	end
end

function CreateDevice(driverType, winWidth, winHeight, bits, texFilter, flags)
	dev.createDevice(driverType, winWidth, winHeight, bits, texFilter, flags)
	if MyCEGUI.create(OPTIONS.CEGUIOptionsFileName) == false then
		io.ILogger:getSingleton():logErr(
			"Can't initialize CEGUI System, MyEngine will exit now...")
		dev.destroyDevice()
		os.exit(1)
	end	
	RereadSingletons()

	MyLogger:logInfo(string.format("---------------------Lua script info:----------------------------"))
	local ver = os.getversion()
	MyLogger:logInfo(string.format("OS version {%d.%d.%d (%s)}", 
	   ver.majorversion, ver.minorversion, ver.revision,
	   ver.description))
	MyLogger:logInfo(string.format("Current directory {%s}",
	   os.getcwd()))
	MyLogger:logInfo(string.format("-----------------------------------------------------------------"))

	SetupResources()
	MyLogger:setLogLevel(OPTIONS.LogLevel) 
	MyDevice:setWindowIcon(
		OPTIONS.Window.Icon.FileName,
		OPTIONS.Window.Icon.Width,
		OPTIONS.Window.Icon.Height)
	MyDevice:setWindowCaption(OPTIONS.Window.Caption)
	MyDriver:useMultiThreadRendering(OPTIONS.UseMultiThreadRendering)

end

function DestroyDevice()
	for i = 0, scr.ESCT_SCRIPT_CALLBACK_TYPE_COUNT - 1 do
		MyScript:setScriptCallback(i, nil)
	end
	MyCEGUI.destroy()
	dev.destroyDevice()
end

require "setup"

