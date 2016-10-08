
require "scened"

Console =
{
	update,
	getState,
	onLog,
	changeState,
	isVisible,
	setStateChangeCallback,
	registerForRendering
}

CONSOLE_STATE_APPEARING		= 0
CONSOLE_STATE_VISIBLE		= 1
CONSOLE_STATE_DISAPPEARING	= 2
CONSOLE_STATE_INVISIBLE		= 3

local _ConsoleOK = false
local _ConsoleInitialized = false

local _ConsoleRoot = nil
local _ConsoleLog = nil
local _ConsoleCmdLine = nil
local _ConsoleLogoImage = nil
local _ConsoleRendererTypeText = nil

local _ItemLogLevelColor =
{
	[io.ELL_INFORMATION]= CEGUI.colour:new_local(0.6, 0.6, 0.6, 1),
	[io.ELL_WARNING]	= CEGUI.colour:new_local(0.8, 0.6, 0.6, 1),
	[io.ELL_ERROR]		= CEGUI.colour:new_local(1.0, 0.2, 0.2, 1),
}
local _ItemLogExpandColor = CEGUI.colour:new_local(0.3, 0.3, 0.3, 1)
local _ItemLogCmdColor = CEGUI.colour:new_local(0.6, 0.6, 0.0, 1)

local _ConsoleState = CONSOLE_STATE_INVISIBLE
local _ConsoleStateNext = CONSOLE_STATE_INVISIBLE
local _ConsoleAppearTimeMs = 500
local _ConsoleStartStateChangingTimeMs = 0
local _ConsoleStateChangeProgrees = 0

local _ConsoleStateChangeCallback = nil

local _LogCount = 0
local _Log = {}

local _ConsoleInfoFont = nil

function _ConsoleInit()
	if _ConsoleInitialized == true then
		return _ConsoleOK
	end
	LOG_INFO("Initializing Console...")
	MyLogger:increaseFormatLevel()
	_ConsoleInitialized = true
	
	CEGUISchemeMgr:create(OPTIONS.ConsoleGUI.Scheme, OPTIONS.ConsoleGUI.ResourceGroup)
	CEGUISchemeMgr:create(OPTIONS.ConsoleGUI.SchemeAliases, OPTIONS.ConsoleGUI.ResourceGroup)
	CEGUIImgsetMgr:create("MyEngineLogos.imageset", "CommonImagesets")
	
	_ConsoleRoot = CEGUIWinMgr:loadWindowLayout("myengine_console.layout", "", "CommonLayouts")
	CEGUISystem:getGUISheet():addChildWindow(_ConsoleRoot)
	_ConsoleRoot:setVisible(false)
	
	_ConsoleLog = tolua.cast(CEGUIWinMgr:getWindow("Console.Log"), "CEGUI::Listbox")
	
	_ConsoleLogoImage = CEGUIWinMgr:getWindow("Console.LogoImage")
	
	_ConsoleRendererTypeText = CEGUIWinMgr:getWindow("Console.RendererTypeText")
	_ConsoleLogoImage:setProperty("Image", "set:MyEngineLogos image:MyEngineLogoImage")
	_ConsoleRendererTypeText:setProperty("Text",
		string.format("%s renderer", vid.getDriverTypeReadableName(MyDriver:getDriverType())))
	
	_ConsoleCmdLine = tolua.cast(CEGUIWinMgr:getWindow("Console.CmdLine"), "CEGUI::Editbox")
	
	_ConsoleCmdLine:subscribeEvent("Activated", "_ConsoleCmdLineActivated")
	_ConsoleCmdLine:subscribeEvent("TextAccepted", "_ConsoleCmdTextAccepted")
	_ConsoleCmdLine:subscribeEvent("KeyUp", "_ConsoleCmdKeyUp")
	
	local font_fname = string.format("%s%s",
		MyResMgr:getMediaDirFull(res.EMT_FONTS), OPTIONS.ConsoleGUI.InfoFont)
	_ConsoleInfoFont = MyDriver:getFont(font_fname)
	_ConsoleInfoFont:setSize(14)
	_ConsoleInfoFont:setMonospaceMode(true)

	MyLogger:decreaseFormatLevel()
	LOG_INFO("Console initialized")

	_ConsoleOK = true
	return _ConsoleOK
end

function _ConsoleCmdLineActivated(arg)
	_ConsoleLogoImage:moveToFront()
	_ConsoleLog:clearAllSelections()
end

-- // 
-- void life(CGUIEngineConsole* caller, core::stringw params)
-- {
	-- if (caller)
    -- {          
        -- game::IGameNodeMainPlayer *player = 
			-- GAME_MANAGER.getMainPlayerGameNode();
        -- if (!player) return;

		-- core::stringc str = params.c_str();

		-- f32 life = atof(str.c_str());		
		
		-- player->setMaxLife(life);
		-- player->setLife(life);
	-- }
-- }

-- //----------------------------------------------------------------------------

-- // 
-- void getpos(CGUIEngineConsole* caller, core::stringw params)
-- {
	-- if (caller)
    -- {
        -- scn::ISceneNode *player = 
			-- GAME_MANAGER.getMainPlayerSceneNode();
        -- if (!player) return;

        -- core::vector3df pos = player->getPosition();
        
        -- char ch[255];
        -- sprintf(ch, "Current position X=%.3f Y=%.3f Z=%.3f", pos.X, pos.Y, pos.Z);        
        -- LOGGER.log(ch);          
    -- }
-- }

-- //----------------------------------------------------------------------------

-- // 
-- void setpos(CGUIEngineConsole* caller, core::stringw params)
-- {
	-- if (caller)
    -- {   
        -- scn::ISceneNode *player =
			-- GAME_MANAGER.getMainPlayerSceneNode();
        -- if (!player) return;
        -- player->setPosition(player->getPosition() + core::vector3df(0, 2000, 0));  
    -- }
-- }

local _ShowFPS = true
local _ShowInfo = false

function _CmdList()
	local ll = MyLogger:getLogLevel()
	_LastUpdateTimeMs = 0 -- to immediate update
	MyLogger:setLogLevel(io.ELL_INFORMATION)
	LOG_INFO("Commands list:")
	for key, value in CONSOLE_COMMANDS do
		LOG_INFO(string.format("    %s - %s", key, value.Info))
	end
	MyLogger:setLogLevel(ll)
end

function _CmdShowFPS()
	_ShowFPS = not _ShowFPS
end

function _CmdShowInfo()
	_ShowInfo = not _ShowInfo
end

function _CmdProfile()
	if MyProfiler:isProfiling() then
		MyProfiler:suspendProfiling()
	else
		MyProfiler:resumeProfiling()
	end
end

function _CmdOctTree()
	MyScnMgr:setOctTreeCullingEnabled(				
		not MyScnMgr:isOctTreeCullingEnabled())
	if MyScnMgr:isOctTreeCullingEnabled() then
		LOG_INFO("OctTree culling enabled")
	else
		LOG_INFO("OctTree culling disabled")
	end
end

function _CmdOcclude()
	MyScnMgr:setOccludersCullingEnabled(				
		not MyScnMgr:isOccludersCullingEnabled())
	if MyScnMgr:isOccludersCullingEnabled() then
		LOG_INFO("Occluders culling enabled")
	else
		LOG_INFO("Occluders culling disabled")
	end
end

function _CmdClearScr()
	MyScript:clearScriptMemory()
end

function _CmdExit()
	MyDevice:close()
end

function _CmdStartPlugin(params)
	if params == "scened" then
		Scened.start()
	else
		MyPluginMgr:startPlugin(params)
	end
end

function _CmdShutPlugin(params)
	if params == "scened" then
		Scened.stop()
	else
		MyPluginMgr:shutPlugin(params)
	end
end

CONSOLE_COMMANDS =
{
	["listcmd"]		= {Handler = _CmdList, 			Info = "List of all available console commands"},
	["showfps"]		= {Handler = _CmdShowFPS,		Info = "Show/hide FPS counter"},
	["showinfo"]	= {Handler = _CmdShowInfo,		Info = "Show/hide different information counters"},
	["profile"]		= {Handler = _CmdProfile,		Info = "Start/stop profiling counters"},
	["octtree"]		= {Handler = _CmdOctTree,		Info = "On/off octree geometry culling"},
	["occlude"]		= {Handler = _CmdOcclude,		Info = "On/off occluders geometry culling"},
	["clearscr"]	= {Handler = _CmdClearScr,		Info = "Clear script memory"},
	["exit"]		= {Handler = _CmdExit,			Info = "Exit game"},
	["start"]		= {Handler = _CmdStartPlugin,	Info = "Start plugin"},
	["shut"]		= {Handler = _CmdShutPlugin,	Info = "Shut plugin"},
}

local _CmdLog = {}
local _CmdLogCount = 0
local _CmdLogCursor = 0

function _ConsoleCmdKeyUp(args)
	local keyEvent = CEGUI.toKeyEventArgs(args)
	if _CmdLogCount <= 0 then
		return
	end
	if keyEvent.scancode == CEGUI.Key.ArrowUp then
		_CmdLogCursor = _CmdLogCursor - 1
	elseif keyEvent.scancode == CEGUI.Key.ArrowDown then
		_CmdLogCursor = _CmdLogCursor + 1
	else
		return
	end
	if _CmdLogCursor < 1 then
		_CmdLogCursor = 1
	elseif _CmdLogCursor > _CmdLogCount then
		_CmdLogCursor = _CmdLogCount + 1
		_ConsoleCmdLine:setText("")
		return
	end
	_ConsoleCmdLine:setText(_CmdLog[_CmdLogCursor])
	_ConsoleCmdLine:setCaratIndex(string.len(_ConsoleCmdLine:getText()))
end

function _ConsoleCmdTextAccepted(args)
	local cmdname = core.Utf8ToAnsi(string.lower(_ConsoleCmdLine:getText()))
	if string.len(cmdname) <= 0 then
		return
	end
	
	local cmd = nil
	local params = nil
	for w in string.gfind(cmdname, "%a+") do
		if cmd == nil then
			cmd = w
		elseif params == nil then
			params = w
		end
	end
	
	cmd = CONSOLE_COMMANDS[cmd]
	if cmd == nil then
		local ll = MyLogger:getLogLevel()
		_LastUpdateTimeMs = 0 -- to immediate update
		MyLogger:setLogLevel(io.ELL_INFORMATION)
		LOG_WARN(string.format("Unknown command '%s'", cmdname))
		MyLogger:setLogLevel(ll)
		_ConsoleCmdLine:setText("")
		return
	end

	table.insert(_CmdLog, cmdname)
	_CmdLogCount = _CmdLogCount + 1
	_CmdLogCursor = _CmdLogCount + 1
	
	local ll = MyLogger:getLogLevel()
	_LastUpdateTimeMs = 0 -- to immediate update
	MyLogger:setLogLevel(io.ELL_INFORMATION)
	LOG_INFO(string.format(">> %s", cmdname))
	MyLogger:setLogLevel(ll)
	_ConsoleCmdLine:setText("")
	cmd.Handler(params)
end

local function _ConsoleOutputMessage(id, logInfo)
	if logInfo.Item ~= nil then
		return
	end
	local newItem = CEGUI.createListboxTextItem(
		"", id, nil, false, true)
	MyCEGUI.setListboxItemTextUtf8(newItem, core.AnsiToUtf8(logInfo.Text))
	newItem:setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush")
	newItem:setTextColours(_ItemLogLevelColor[logInfo.LogLevel])
	_ConsoleLog:addItem(newItem)
	logInfo.Item = newItem
end

local _curYPos = CEGUI.UDim(0.0, 0.0)
local _LastUpdateTimeMs = 0

local function _ConsoleUpdate()
	if not _ConsoleInit() then
		return
	end

	local nowMs = MyDevice:getDeviceSystemTime()
	
	if _ConsoleStateNext ~= _ConsoleState then
		_ConsoleState = _ConsoleStateNext
		if _ConsoleStateChangeCallback ~= nil then
			_ConsoleStateChangeCallback(_ConsoleState)
		end
	end
	if _ConsoleState == CONSOLE_STATE_APPEARING or
			_ConsoleState == CONSOLE_STATE_DISAPPEARING then
		_ConsoleStateChangeProgrees = (
			nowMs - _ConsoleStartStateChangingTimeMs) / _ConsoleAppearTimeMs
		if _ConsoleStateChangeProgrees >= 1.0 then
			_ConsoleStateChangeProgrees = 1.0
		end
		if _ConsoleState == CONSOLE_STATE_APPEARING then
			_curYPos.scale = -1.0 + _ConsoleStateChangeProgrees
			if not _ConsoleRoot:isVisible() then
				_ConsoleRoot:setVisible(true)
				_ConsoleRoot:setAlwaysOnTop(true)
			end
		else
			_curYPos.scale = -_ConsoleStateChangeProgrees
		end
		_ConsoleRoot:setYPosition(_curYPos)
		if _ConsoleStateChangeProgrees == 1.0 then
			if _ConsoleState == CONSOLE_STATE_APPEARING then
				_ConsoleCmdLine:activate()
				_ConsoleCmdLine:enable()
				_ConsoleCmdLine:activate()
				_ConsoleState = CONSOLE_STATE_VISIBLE
			else
				_ConsoleRoot:setVisible(false)
				_ConsoleState = CONSOLE_STATE_INVISIBLE
			end
			_LastUpdateTimeMs = 0 -- to immediate update
		end
	end
	if _ConsoleStateNext ~= _ConsoleState then
		_ConsoleStateNext = _ConsoleState
		if _ConsoleStateChangeCallback ~= nil then
			_ConsoleStateChangeCallback(_ConsoleState)
		end
	end
	
	if (nowMs - _LastUpdateTimeMs) < 1000 then
		return
	end
	_LastUpdateTimeMs = nowMs

	local startIdx = 0
	local itemCount = _ConsoleLog:getItemCount()
	if itemCount > 0 then
		local lastItem = _ConsoleLog:getListboxItemFromIndex(itemCount - 1)
		if lastItem ~= nil then
			startIdx = lastItem:getID()
		end
	end
	startIdx = startIdx + 1
	
	local logCnt = 0
	local maxLogCnt = 100
	if (_LogCount - startIdx) > maxLogCnt then
		local newStartIdx = _LogCount - maxLogCnt
		local newItem = CEGUI.createListboxTextItem(
			string.format("<<<--- Skiped %d lines. --->>>", newStartIdx - startIdx),
			0, nil, false, true)   
		newItem:setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush")
		newItem:setTextColours(_ItemLogExpandColor)
		_ConsoleLog:addItem(newItem)
		startIdx = newStartIdx
	end
	while startIdx <= _LogCount do
		_ConsoleOutputMessage(startIdx, _Log[startIdx])
		startIdx = startIdx + 1
		logCnt = logCnt + 1
	end
	if logCnt > 0 then
		local scroll = _ConsoleLog:getVertScrollbar()
		scroll:setScrollPosition(scroll:getDocumentSize())
	end

end
Console.update = _ConsoleUpdate

local function _ConsoleGetState()
	return _ConsoleState
end
Console.getState = _ConsoleGetState

local function _ConsoleOnLog(logEvent)
	if logEvent.Level < OPTIONS.ConsoleLogLevel then
		return
	end
	_LogCount = _LogCount + 1
	_Log[_LogCount] = {Item = nil, Text = logEvent.Text, LogLevel = logEvent.Level}
end
Console.onLog = _ConsoleOnLog

local function _ConsoleChangeState()
	if not _ConsoleInit() then
		return
	end
	_ConsoleStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
	if _ConsoleState == CONSOLE_STATE_INVISIBLE then
		_ConsoleStateNext = CONSOLE_STATE_APPEARING
	elseif _ConsoleState == CONSOLE_STATE_VISIBLE then
		_ConsoleStateNext = CONSOLE_STATE_DISAPPEARING
	elseif _ConsoleState == CONSOLE_STATE_APPEARING then
		_ConsoleStartStateChangingTimeMs = _ConsoleStartStateChangingTimeMs -
			_ConsoleAppearTimeMs * (1.0 - _ConsoleStateChangeProgrees)
		_ConsoleStateNext = CONSOLE_STATE_DISAPPEARING
	elseif _ConsoleState == CONSOLE_STATE_DISAPPEARING then
		_ConsoleStartStateChangingTimeMs = _ConsoleStartStateChangingTimeMs -
			_ConsoleAppearTimeMs * (1.0 - _ConsoleStateChangeProgrees)
		_ConsoleStateNext = CONSOLE_STATE_APPEARING
	end
end
Console.changeState = _ConsoleChangeState

local function _ConsoleIsVisible()
	return _ConsoleState ~= CONSOLE_STATE_INVISIBLE
end
Console.isVisible = _ConsoleIsVisible

local function _ConsoleSetStateChangeCallback(callback)
	_ConsoleStateChangeCallback = callback
end
Console.setStateChangeCallback = _ConsoleSetStateChangeCallback

local _Pos = core.vector3df(0,0,0)
local _Rot = core.vector3df(0,0,0)
local _PosRect = core.recti(0, 0, 0, 0)
local _ColorWhite = img.SColor(255, 255, 255, 255)
local _ColorBlack = img.SColor(255, 0, 0, 0)
local _ScrSize = nil
local _PadStr = ""
local _Text = ""

local function _ConsoleFontRegisterForRendering(text, posx, posy, width, height)
	local text_len = string.len(text)

	_PosRect:set(posx + 1, posy + 1, width, height)
	_ConsoleInfoFont:registerForRendering(
		_Text, text_len, _PosRect, _ColorBlack, false, false, nil)
		
	_PosRect:set(posx, posy, width, height)
	_ConsoleInfoFont:registerForRendering(
		_Text, text_len, _PosRect, _ColorWhite, false, false, nil)
end

local function _ConsoleRegisterForRendering()
	if not _ConsoleInit() then
		return
	end
	local posx = 20
	local posy = 20
	local height1 = 20
	local height2 = 12
	
	if MyScnMgr:isSceneEditorStarted() then
		posy = posy + 30
	end
	
	local scrWidth = MyDriver:getScreenSize().Width

	if _ShowFPS then
		
		local param = MyProfiler:getFirstProfileParameter()
		local frameTimeSec = param.FilteredTimeSec
		local cur_fps	= MyDriver:getCurrentFPS()
		local tri		= MyDriver:getRenderedTrianglesCount()
		local dips		= MyDriver:getRenderedDIPsCount()

		_Text = string.format("%d fps, %.3f ms, %d tri/fr, %d dips/fr",
				cur_fps, frameTimeSec * 1000.0, tri, dips)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)
	end

	if _ShowInfo then
		local dyn_per_sec   = MyGameMgr:getDynamicPS()	
		local game_per_sec  = MyGameMgr:getGamePS()
		local input_per_sec = MyInpDisp:getInputPS()	
		local script_per_sec= MyGameMgr:getScriptPS()

		posy = posy + height1

		_Text = string.format("%#3d dyn/s, %#3d game/s, %#3d inp/s, %#3d scr/s",
			dyn_per_sec, game_per_sec, input_per_sec, script_per_sec)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

		local events        = MyGameEventDisp:getGameEventsCount()
		local aievents      = MyGameAI:getGameAIEventsCount()
		local tasks         = MyGameTasksMgr:getGameTasksCount()
		local lcount        = MyDriver:getDynamicLightsCount()
		local pscount       = MySoundDrv:getPlayingSoundsCount()
		local scount        = MySoundDrv:getSoundsCount()
		local occl			= MyCullSys:getOccludersCount()

		posy = posy + height2

		_Text = string.format("%#3d events, %#3d ai events, %#3d tasks, %#3d lights, %#3d (%#3d) sounds, %#3d occluders",
			events, aievents, tasks, lcount, pscount, scount, occl)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

		local visnodes		= MyScnMgr:getVisibleSceneNodesCount()
		local nodes			= MyScnMgr:getSceneNodesCount()	
		local enabled_dobj  = MyDynMgr:getEnabledDynamicObjectsCount()
		local dobj          = MyDynMgr:getDynamicObjectsCount()
		local act_game_nodes= MyGameMgr:getActiveGameNodesCount()
		local game_nodes    = MyGameMgr:getGameNodesCount()
		local collisions    = MyDynMgr:getCustomCollisionPointsCount() + MyDynMgr:getDynamicCollisionPointsCount()

		posy = posy + height2
		
		_Text = string.format("%#3d (%#3d) scn.nodes, %#3d (%#3d) dyn.obj., %#3d (%#3d) game nodes, %#5d collisions",
			visnodes, nodes, enabled_dobj, dobj, act_game_nodes, game_nodes, collisions)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

		local script_memory      = MyScript:getScriptMemoryKBytes() / 1024.0
		local alloc_mbytes       = (MyMemStat:getAllocatedBytes() / 1024.0) / 1024.0
		local alloc_sec          = MyMemStat:getAllocsPerSec()
		local dealloc_sec        = MyMemStat:getDeallocsPerSec()
		local alloc_kbytes_sec   = MyMemStat:getAllocBytesPerSec() / 1024.0
		local dealloc_kbytes_sec = MyMemStat:getDeallocBytesPerSec() / 1024.0

		posy = posy + height1

		_Text = string.format("Memory status: %.3fMb script, %.3fMb dynamic",
			script_memory, alloc_mbytes)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

		if alloc_kbytes_sec > 0 or alloc_sec > 0 or dealloc_kbytes_sec > 0 or dealloc_sec > 0 then
			posy = posy + height1
			_Text = string.format("dynamic alloc./dealloc. %.3fKb(%d)/%.3fKb(%d) per sec.",
				alloc_kbytes_sec, alloc_sec, dealloc_kbytes_sec, dealloc_sec)
			_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)
		end

		local player_gnode = MyGameMgr:getMainPlayerGameNode()
		local player = nil
		if player_gnode ~= nil then
			player = player_gnode:getSceneNode()
		end

		_Pos:set(0.0, 0.0, 0.0)
		_Rot:set(0.0, 0.0, 0.0)
		if player ~= nil and not Scened.isStarted() then
			_Pos:set(player:getPosition())
			_Rot:set(player:getRotation())
		else
			local camera = MyScnMgr:getActiveCamera()
			if camera ~= nil then
				_Pos:set(camera:getPosition())
				_Rot:set(camera:getRotation())				
			end
		end

		posy = posy + height1

		_Text = string.format("Position X=%8.2f, Y=%8.2f, Z=%8.2f",
			_Pos.X, _Pos.Y, _Pos.Z)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)
			
		posy = posy + height2

		_Text = string.format("Rotation X=%8.2f, Y=%8.2f, Z=%8.2f",
			_Rot.X, _Rot.Y, _Rot.Z)
		_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

		posy = posy + height1 - height2
			
		local param = MyProfiler:getFirstProfileParameter()
		while param ~= nil do
			_PadStr = ""
			for i = 0, param.ParentDepth - 1 do
				_PadStr = _PadStr.."    "
			end

			posy = posy + height2

			_Text = string.format("%s%45s: %6.2f%% (%7.3f ms) %s",
				_PadStr, param.Name:c_str(), param.Pers,
				param.FilteredTimeSec * 1000.0, param.Info:c_str())
			_ConsoleFontRegisterForRendering(_Text, posx, posy, scrWidth, posy + height1)

			param = MyProfiler:getNextProfileParameter(param.Id)
		end
	end

end
Console.registerForRendering = _ConsoleRegisterForRendering
