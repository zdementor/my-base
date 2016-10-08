
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

MyRT = nil

CEGUISystem		= nil
CEGUIRenderer	= nil
CEGUISchemeMgr	= nil
CEGUIImgsetMgr	= nil
CEGUIWinMgr		= nil
CEGUICursor		= nil
CEGUIFontMgr	= nil

MAIN_COLOR_RTT_NAME = "MainColorRenderTargetTexture"
MAIN_DEPTH_RTT_NAME = "MainDepthRenderTargetTexture"

function LOG_INFO(...)
	io.ILogger:getSingleton():logInfo("<Lua> "..string.format(...))
end

function LOG_WARN(...)
	io.ILogger:getSingleton():logWarn("<Lua> "..string.format(...))
end

function LOG_ERR(...)
	io.ILogger:getSingleton():logErr("<Lua> "..string.format(...))
end

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

	local ver = os.getversion()
	local arch = "32 bit"
	if os._is64bit() then
		arch = "64 bit"
	end

	LOG_INFO("Creating Device...")
	LOG_INFO("-------------------------------------------------------")
	LOG_INFO(string.format(" OS version           : %d.%d.%d (%s)",
		ver.majorversion, ver.minorversion, ver.revision, ver.description))
	LOG_INFO(string.format(" Current directory    : %s",
		os.getcwd()))
	LOG_INFO(string.format(" MyEngine binary arch : %s",
		arch))
	LOG_INFO("-------------------------------------------------------")
	io.ILogger:getSingleton():increaseFormatLevel()	
	dev.createDevice(driverType, winWidth, winHeight, bits, texFilter, flags)
	if MyCEGUI.create(OPTIONS.CEGUIOptionsFileName) == false then
		LOG_ERR("Can't initialize CEGUI System, MyEngine will exit now...")
		dev.destroyDevice()
		os.exit(1)
	end	
	io.ILogger:getSingleton():decreaseFormatLevel()	
	LOG_INFO("Device created.")

	RereadSingletons()

	if MyDriver:queryFeature(vid.EVDF_RENDER_TO_TARGET) then
		LOG_INFO("Creating Render Target...")
		MyLogger:increaseFormatLevel()	
		local rtOK = false
		local colorRT = MyDriver:addRenderTargetTexture(MAIN_COLOR_RTT_NAME,
			winWidth, winHeight, img.ECF_A8R8G8B8)
		local depthRT = nil
		if colorRT ~= nil then
			depthRT = MyDriver:addRenderTargetTexture(MAIN_DEPTH_RTT_NAME,
				winWidth, winHeight, img.ECF_DEPTH24_STENCIL8)
		end
		if depthRT ~= nil and colorRT ~= nil then
			MyRT = MyDriver:addRenderTarget()
			if MyRT == nil then
				LOG_INFO("Can not add Render Target!")
			end
		end
		if MyRT ~= nil then
			rtOK = MyRT:bindColorTexture(0, colorRT, false)
			if rtOK then
				rtOK = MyRT:bindDepthTexture(depthRT, false)
			end
			if rtOK then
				rtOK = MyRT:rebuild()
			end
			if rtOK then
				local rt = MyDriver:getRenderTarget()
				rtOK = MyDriver:setRenderTarget(MyRT)
				MyDriver:setRenderTarget(rt)
			end
		end
		if not rtOK then
			if MyRT ~= nil then
				MyDriver:removeRenderTarget(MyRT)
				MyRT = nil
			end
			if colorRT ~= nil then
				MyDriver:removeTexture(colorRT)
				colorRT = nil
			end
			if depthRT ~= nil then
				MyDriver:removeTexture(depthRT)
				depthRT = nil
			end
			MyLogger:decreaseFormatLevel()	
			LOG_INFO("Can not create Render Target, fallback to the default render buffer.")
		else
			MyLogger:decreaseFormatLevel()	
			LOG_INFO("Render Target created.")
		end
	end
	SetupResources()
	MyLogger:setLogLevel(OPTIONS.LogLevel) 
	MyDevice:setWindowIcon(
		OPTIONS.Window.Icon.FileName,
		OPTIONS.Window.Icon.Width,
		OPTIONS.Window.Icon.Height)
	MyDevice:setWindowCaption(OPTIONS.Window.Caption)
end

function DestroyDevice()
	LOG_INFO("Destroy Device...")
	io.ILogger:getSingleton():increaseFormatLevel()	
	for i = 0, scr.ESCT_SCRIPT_CALLBACK_TYPE_COUNT - 1 do
		MyScript:setScriptCallback(i, nil)
	end
	MyCEGUI.destroy()
	MyRT = nil -- will be destroyed by device
	dev.destroyDevice()
	io.ILogger:getSingleton():decreaseFormatLevel()	
	LOG_INFO("Device destroyed.")
end

require "setup"

