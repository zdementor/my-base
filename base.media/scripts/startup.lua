
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

function CreateRT(width, height, colorRTFmts, depthFmt)

	local newRT = nil

	local rtOK = true

	local colorRTs = {}
	local depthRT = nil

	for no = 1, table.getn(colorRTFmts) do
		colorRTs[no] = MyDriver:addRenderTargetTexture(nil,
			width, height, colorRTFmts[no])
		if colorRTs[no] == nil then
			rtOK = false
			break
		end
	end
	if rtOK then
		depthRT = MyDriver:addRenderTargetTexture(nil,
			width, height, depthFmt)
		if depthRT == nil then
			rtOK = false
		end
	end

	if rtOK then
		newRT = MyDriver:addRenderTarget()
		if newRT == nil then
			rtOK = false
		end
	end
	if rtOK then
		for no = 1, table.getn(colorRTs) do
			rtOK = newRT:bindColorTexture(no-1, colorRTs[no], false)
			if not rtOK then
				break
			end
		end
	end
	if rtOK then
		rtOK = newRT:bindDepthTexture(depthRT, false)
	end
	if rtOK then
		rtOK = newRT:rebuild()
	end
	if rtOK then
		local oldRT = MyDriver:getRenderTarget()
		rtOK = MyDriver:setRenderTarget(newRT)
		MyDriver:setRenderTarget(oldRT)
	end
	if not rtOK then
		if newRT ~= nil then
			MyDriver:removeRenderTarget(newRT)
			newRT = nil
		end
		for no = 1, table.getn(colorRTs) do
			if colorRTs[no] ~= nil then
				MyDriver:removeTexture(colorRTs[no])
			end
		end
		colorRTs = {}
		if depthRT ~= nil then
			MyDriver:removeTexture(depthRT)
			depthRT = nil
		end
	end

	return newRT
end

function CreateMainRT(width, height)

	local colorRTFmts = {
		img.ECF_A8R8G8B8,      -- diffuse
--		img.ECF_A8R8G8B8,      -- normal
--		img.ECF_A32B32G32R32F, -- position
--		img.ECF_A8R8G8B8,      -- material
		}
	local depthFmt = img.ECF_DEPTH24_STENCIL8

	return CreateRT(width, height, colorRTFmts, depthFmt)
end

function IsDefferedRT(rt)
	if rt == nil then
		return false
	end
	local colCnt = 0
	for i = 0, vid.MY_MAX_COLOR_ATTACHMENTS - 1 do
		local colRT = rt:getColorTexture(i)
		if colRT ~= nil then
			colCnt = colCnt + 1
		end
	end
	if colCnt > 1 then
		return true
	end
	return false
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

	if MyDriver:queryFeature(vid.EVDF_RENDER_TO_TARGET)
			and not MyDevice:getDeviceFlagValue(dev.EDCF_USE_FFP) then

		LOG_INFO("Creating Main Render Target...")
		MyLogger:increaseFormatLevel()	

		MyRT = CreateMainRT(winWidth, winHeight)

		MyLogger:decreaseFormatLevel()
		if MyRT ~= nil then
			LOG_INFO("Main Render Target created.")
		else
			LOG_INFO("Can not create Main Render Target, fallback to the default render buffer.")
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

