
local WinWidth	= 360
local WinHeight	= 420

CreateDevice(vid.EDT_OPENGL11, WinWidth, WinHeight, 8, vid.ETF_NONE,
	bit.bor(dev.EDCF_ALWAYS_ON_TOP, dev.EDCF_NO_SYSTEM_MENU, dev.EDCF_NO_TITLE_BAR,
		dev.EDCF_ROUNDED_FRAME_RECTANGLE, dev.EDCF_DRAG_USING_TOP_AREA,
		dev.EDCF_USE_FFP, dev.EDCF_SOFTWARE_RASTERIZER))
		
local Title = string.format("Setup MyEngine (v.%s)", MyDevice:getVersionString())

MyDevice:setWindowCaption(Title)

MyCursor:resetGraphicCursor()
MyCursor:setCursor(io.ECT_ARROW)
MyCursor:setVisible(true)

CEGUISchemeMgr:create("AquaLook.scheme", "CommonSchemes")
CEGUISchemeMgr:create("AquaLookSetupAliases.scheme", "CommonSchemes")
CEGUIImgsetMgr:create("MyEngineZDimitorLogo.imageset", "CommonImagesets")

local MainWnd = CEGUIWinMgr:loadWindowLayout("myengine_setup.layout", "", "CommonLayouts")
CEGUISystem:setGUISheet(MainWnd)
MainWnd:setVisible(true)

CEGUIWinMgr:getWindow("Setup.MainFrame"):setText(Title)

local scr_sz = CEGUI.Size(WinWidth, WinHeight)
local cegui_fonts = CEGUIFontMgr:getIterator()
cegui_fonts:toStart()
while cegui_fonts:isAtEnd() == false do
	cegui_fonts:value():setNativeResolution(scr_sz)
	cegui_fonts:next()
end

CEGUIWinMgr:getWindow("MyEngineLogo"):setProperty("Image",
	"set:MyEngineZDimitorLogo image:MyEngineZDimitorLogoImage")

CEGUISystem:setDefaultMouseCursor("AquaLook", "MouseArrow")
CEGUISystem:setDefaultFont("BlueHighway-16")
CEGUICursor:hide()

local ButtonRun = CEGUIWinMgr:getWindow("ButtonRun")
local ButtonExit = CEGUIWinMgr:getWindow("ButtonExit")
local CheckVSync =
	tolua.cast(CEGUIWinMgr:getWindow("CheckVSync"),"CEGUI::Checkbox")
local CheckFullscreen =
	tolua.cast(CEGUIWinMgr:getWindow("CheckFullscreen"),"CEGUI::Checkbox")
local CheckShadows =
	tolua.cast(CEGUIWinMgr:getWindow("CheckShadows"),"CEGUI::Checkbox")
local CheckDirt =
	tolua.cast(CEGUIWinMgr:getWindow("CheckDirt"),"CEGUI::Checkbox")
local ComboDriverType = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboDriverType"),"CEGUI::Combobox")
local ComboResolution = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboResolution"),"CEGUI::Combobox")
local ComboRenderPath = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboRenderPath"),"CEGUI::Combobox")
local ComboTexFilter = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboTexFilter"),"CEGUI::Combobox")
local ComboGeomDetail = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboGeomDetail"),"CEGUI::Combobox")
local ComboLighting = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboLighting"),"CEGUI::Combobox")
local ComboAnimation = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboAnimation"),"CEGUI::Combobox")
local ComboGraphicDetail = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboGraphicDetail"),"CEGUI::Combobox")
local ComboShaders = 
	tolua.cast(CEGUIWinMgr:getWindow("ComboShaders"),"CEGUI::Combobox")
local LabelShadersQuality = 
	tolua.cast(CEGUIWinMgr:getWindow("LabelShadersQuality"),"CEGUI::Combobox")

StartupDriverIndex = 1
SETUP_SETTINGS = {}

local SETUP_DRIVER_DIRECTX9	= 1
local SETUP_DRIVER_OPENGL12	= 2
local SETUP_DRIVER_OPENGL21	= 3
local SETUP_DRIVER_TYPE = {
	[SETUP_DRIVER_DIRECTX9]	= {
		name	= vid.getDriverTypeReadableName(vid.EDT_DIRECTX9),
		id		= vid.EDT_DIRECTX9},
	[SETUP_DRIVER_OPENGL12] = {
		name	= vid.getDriverTypeReadableName(vid.EDT_OPENGL12),
		id		= vid.EDT_OPENGL12},
	[SETUP_DRIVER_OPENGL21] = {
		name	= vid.getDriverTypeReadableName(vid.EDT_OPENGL21),
		id		= vid.EDT_OPENGL21},}

local SETUP_RENDER_USE_FFP		= 1
local SETUP_RENDER_USE_SHADERS	= 2
local SETUP_RENDER_PATH = {
	[SETUP_RENDER_USE_FFP]		= {name = "Fixed pipeline",	id = true	},
	[SETUP_RENDER_USE_SHADERS]	= {name = "Shaders",id = false	}}

local SETUP_TEX_NONE		= 1
local SETUP_TEX_BILINEAR	= 2
local SETUP_TEX_TRILINEAR	= 3
local SETUP_TEX_ANISOTROPIC	= 4
local SETUP_TEX_FILTER = {
	[SETUP_TEX_NONE]		= {name = "None",		id = vid.ETF_NONE		},
	[SETUP_TEX_BILINEAR]	= {name = "Bilinear",	id = vid.ETF_BILINEAR	},
	[SETUP_TEX_TRILINEAR]	= {name = "Trilinear",	id = vid.ETF_TRILINEAR	},
	[SETUP_TEX_ANISOTROPIC]	= {name = "Anisotropic",id = vid.ETF_ANISOTROPIC}}

local SETUP_GEOM_LOW	= 1
local SETUP_GEOM_HIGH	= 2
local SETUP_GEOM_DETAIL = {
	{name = "Low",	id = false	},
	{name = "High",	id = true	}}

local SETUP_LIGHTING_LOW	= 1
local SETUP_LIGHTING_MEDIUM	= 2
local SETUP_LIGHTING_HIGH	= 3
local SETUP_LIGHTING_QUALITY = {
	[SETUP_LIGHTING_LOW]	= {name = "Low",	id = 0									},
	[SETUP_LIGHTING_MEDIUM]	= {name = "Medium",	id = dev.EDCF_LIGHTING_MEDIUM_QUALITY	},
	[SETUP_LIGHTING_HIGH]	= {name = "High",	id = dev.EDCF_LIGHTING_HIGH_QUALITY		}}
	
local SETUP_SHADERS_LOW		= 1
local SETUP_SHADERS_HIGH	= 2
local SETUP_SHADERS_QUALITY = {
	[SETUP_SHADERS_LOW]		= {name = "Low",				id = 0								},
	[SETUP_SHADERS_HIGH]	= {name = "High (perpixel)",	id = dev.EDCF_SHADERS_HIGH_QUALITY	}}

local SETUP_ANIM_LOW	= 1
local SETUP_ANIM_HIGH	= 2
local SETUP_ANIM_QUALITY = {
	[SETUP_ANIM_LOW]	= {name = "Low",			id = false	},
	[SETUP_ANIM_HIGH]	= {name = "High (smooth)",	id = true	}}

local SETUP_GRAPHIC_LOW		= 1
local SETUP_GRAPHIC_MEDIUM	= 2
local SETUP_GRAPHIC_HIGH	= 3
local SETUP_GRAPHIC_HIGHEST	= 4
local SETUP_GRAPHIC_CUSTOM	= 5
local SETUP_GRAPHIC_LEVEL = {
	[SETUP_GRAPHIC_LOW]		= {name = "Low",	id = 0},
	[SETUP_GRAPHIC_MEDIUM]	= {name = "Medium",	id = 1},
	[SETUP_GRAPHIC_HIGH]	= {name = "High",	id = 2},
	[SETUP_GRAPHIC_HIGHEST]	= {name = "Highest",id = 3},
	[SETUP_GRAPHIC_CUSTOM]	= {name = "Custom",	id = 4}}
	
-- load options from file
function LoadStartupOptions()
	local file = io.open (OPTIONS.StartupOptionsFileName, "r")
	if file ~= nil then
		i = 0
		for line in file:lines() do 
			if i==0 then    
				StartupDriverIndex = tonumber(line)
			else 
				local settings = {}
				for token in string.gfind(line, "%d+") do
					table.insert(settings, tonumber(token))
				end
				table.insert(SETUP_SETTINGS, {
					DriverIndex			= settings[1],
					ResolutionX			= settings[2],
					ResolutionY			= settings[3],
					ColorBits			= 32,
					TexFilter			= settings[5],
					Fullscreen			= settings[6],
					Antialiasing		= settings[7],
					VerticalSync		= settings[8],
					Shadows				= settings[9],
					DynamicLighting		= settings[10],
					HiMapsDetail		= settings[11],
					HiAnimQuality		= settings[12],
					Shaders				= settings[13],
					ShadersHighQuality	= settings[14],
					DirtOnLevel			= settings[15]})
	        end
			i = i + 1
	    end
		file:close()
	else
	-- setting settings to default values
		StartupDriverIndex = SETUP_DRIVER_OPENGL21
		for i = SETUP_DRIVER_DIRECTX9, SETUP_DRIVER_OPENGL21 do
			table.insert(SETUP_SETTINGS, {
				DriverIndex			= i,
				ResolutionX			= 800,
				ResolutionY			= 600,
				ColorBits			= 32,
				TexFilter			= SETUP_TEX_BILINEAR,
				Fullscreen			= 0,
				Antialiasing		= 0,
				VerticalSync		= 0,
				Shadows				= 0,
				DynamicLighting		= SETUP_LIGHTING_LOW,
				HiMapsDetail		= 0,
				HiAnimQuality		= 0,
				Shaders				= 0,
				ShadersHighQuality	= 0,
				DirtOnLevel			= 0})
		end
	end
end

-- write options to file
function SaveStartupOptions()
	local file = io.open (OPTIONS.StartupOptionsFileName, "w")
	if file ~= nil then
		file:write(string.format("%d\n", StartupDriverIndex))
		for k, v in pairs(SETUP_SETTINGS) do
			file:write(string.format("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				v.DriverIndex,
				v.ResolutionX,
				v.ResolutionY,
				v.ColorBits,
				v.TexFilter,
				v.Fullscreen,
				v.Antialiasing,
				v.VerticalSync,
				v.Shadows,
				v.DynamicLighting,
				v.HiMapsDetail,
				v.HiAnimQuality,
				v.Shaders,
				v.ShadersHighQuality,
				v.DirtOnLevel))
		end
		file:close()
	end
end

function AddListboxItem(listbox, name, id)
	local new_item = CEGUI.createListboxTextItem(name, id, nil, false, true)   
	new_item:setSelectionBrushImage("AquaLook", "MultiLineEditboxSelectionBrush")
	new_item:setTextColours(CEGUI.colour(0.0, 0.0, 0.0, 1.0))
	listbox:addItem(new_item)
end

function InitControls()
	ComboDriverType:resetList()
	for k, v in pairs(SETUP_DRIVER_TYPE) do
		AddListboxItem(ComboDriverType, v.name, v.id)
	end
	ComboGeomDetail:resetList()
	for k, v in pairs(SETUP_GEOM_DETAIL) do
		AddListboxItem(ComboGeomDetail, v.name, v.id)
	end
	ComboAnimation:resetList()
	for k, v in pairs(SETUP_ANIM_QUALITY) do
		AddListboxItem(ComboAnimation, v.name, v.id)
	end
	ComboGraphicDetail:resetList()
	for i = 1, SETUP_GRAPHIC_CUSTOM - 1 do
		local name	= SETUP_GRAPHIC_LEVEL[i].name
		local id	= SETUP_GRAPHIC_LEVEL[i].id
		AddListboxItem(ComboGraphicDetail, name, id)
	end
	UpdateControls()
end

function UpdateControls()
	ComboDriverType:setItemSelectState(StartupDriverIndex-1, true)
	UpdateVideoModes()
	UpdateLightingModes()
	UpdateShadersQuality()
	UpdateTexFilter()
	UpdateRenderPath()
	CheckFullscreen:setSelected(SETUP_SETTINGS[StartupDriverIndex].Fullscreen	== 1)
	CheckVSync:		setSelected(SETUP_SETTINGS[StartupDriverIndex].VerticalSync	== 1)
	CheckShadows:	setSelected(SETUP_SETTINGS[StartupDriverIndex].Shadows		== 1)
	CheckDirt:		setSelected(SETUP_SETTINGS[StartupDriverIndex].DirtOnLevel 	== 1)
	ComboRenderPath:setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].Shaders,				true)
	ComboTexFilter:	setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].TexFilter - 1,		true)
	ComboGeomDetail:setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].HiMapsDetail,			true)
	ComboLighting:	setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].DynamicLighting - 1,  true)
	ComboAnimation:	setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].HiAnimQuality,		true)
	ComboShaders:	setItemSelectState(SETUP_SETTINGS[StartupDriverIndex].ShadersHighQuality,	true)
	UpdateGraphicLevel()
end
	
function UpdateVideoModes()
	local depth = 32
	local _4_div_3 = 4.0/ 3.0;
	local  _4_div_3_min = _4_div_3 - 0.1;
	local  _4_div_3_max = _4_div_3 + 0.1;
	local  _16_div_10 = 16.0/ 10.0;
	local  _16_div_10_min = _16_div_10 - 0.1;
	local  _16_div_10_max = _16_div_10 + 0.1;
	local  _16_div_9 = 16.0/ 9.0;
	local  _16_div_9_min = _16_div_9 - 0.1;
	local  _16_div_9_max = _16_div_9 + 0.1;
	ComboResolution:resetList()
	local vm_list = MyDriver:getVideoModeList()
	local vm_cnt = vm_list:getVideoModeCount()
	for vm = 0, vm_cnt - 1 do
		local _res = vm_list:getVideoModeResolution(vm)
		local _depth = vm_list:getVideoModeDepth(vm)
		if depth == _depth  and _res.Width >= 640 and _res.Height >= 480 then
			local aspect = _res.Width/_res.Height;
			local aspect_str = ""
			if aspect==_4_div_3 then
				aspect_str = " (4/3)"
			elseif aspect>_4_div_3_min and aspect<_4_div_3_max then
				aspect_str = " (4/3)"
			elseif aspect==_16_div_10 then
				aspect_str = " (16/10)"
			elseif aspect>_16_div_10_min and aspect<_16_div_10_max then
				aspect_str = " (16/10)"
			elseif aspect==_16_div_9 then
				aspect_str = " (16/9)"
			elseif aspect>_16_div_9_min and aspect<_16_div_9_max then
				aspect_str = " (16/9)"
			end
			local id	= bit.bor(bit.lshift(_res.Width, 16), _res.Height)
			local name	= string.format(
				"%4d x %4d %d bit%s", _res.Width, _res.Height, _depth, aspect_str)
			AddListboxItem(ComboResolution, name, id)
		end
	end
	local resX = SETUP_SETTINGS[StartupDriverIndex].ResolutionX
	local resY = SETUP_SETTINGS[StartupDriverIndex].ResolutionY
	local resID = bit.bor(bit.lshift(resX, 16), resY)
	for i = 0, ComboResolution:getItemCount() - 1 do
		local item = ComboResolution:getListboxItemFromIndex(i)
		local id = item:getID()
		if id == resID then
			ComboResolution:setItemSelectState(i, true)
			break
		end
	end
end

function UpdateLightingModes()
	ComboLighting:resetList()
	for k, v in pairs(SETUP_LIGHTING_QUALITY) do
		AddListboxItem(ComboLighting, v.name, v.id)
	end
end

function UpdateShadersQuality()
	ComboShaders:hide()
	LabelShadersQuality:hide()
	ComboShaders:resetList()
	for k, v in pairs(SETUP_SHADERS_QUALITY) do
		AddListboxItem(ComboShaders, v.name, v.id)
	end
	if SETUP_SETTINGS[StartupDriverIndex].Shaders ~= 0 then
		ComboShaders:show()
		LabelShadersQuality:show()		
	end	
end

function UpdateTexFilter()
	local idx = StartupDriverIndex
	ComboTexFilter:resetList()
	for k, v in pairs(SETUP_TEX_FILTER) do
		if idx ~= SETUP_DRIVER_OPENGL12 or v.id ~= vid.ETF_ANISOTROPIC then
			AddListboxItem(ComboTexFilter, v.name, v.id)
		end
	end
end

function UpdateRenderPath()
	local idx = StartupDriverIndex
	ComboRenderPath:resetList()
	for k, v in pairs(SETUP_RENDER_PATH) do
		if idx ~= SETUP_DRIVER_OPENGL12 or v.id == true then
			AddListboxItem(ComboRenderPath, v.name, v.id)
		end
	end
end
	
function SetGraphicLevel(level)
	local idx = StartupDriverIndex
	if level == SETUP_GRAPHIC_LOW then
		SETUP_SETTINGS[idx].TexFilter			= SETUP_TEX_BILINEAR
		SETUP_SETTINGS[idx].Shadows				= 0
		SETUP_SETTINGS[idx].DynamicLighting		= SETUP_LIGHTING_LOW
		SETUP_SETTINGS[idx].HiMapsDetail		= 0
		SETUP_SETTINGS[idx].HiAnimQuality		= 0
		SETUP_SETTINGS[idx].DirtOnLevel			= 0
		SETUP_SETTINGS[idx].ShadersHighQuality	= 0
	elseif level == SETUP_GRAPHIC_MEDIUM then
		SETUP_SETTINGS[idx].TexFilter			= SETUP_TEX_TRILINEAR
		SETUP_SETTINGS[idx].Shadows				= 1
		SETUP_SETTINGS[idx].DynamicLighting		= SETUP_LIGHTING_MEDIUM
		SETUP_SETTINGS[idx].HiMapsDetail		= 0
		SETUP_SETTINGS[idx].HiAnimQuality		= 1
		SETUP_SETTINGS[idx].DirtOnLevel			= 0
		SETUP_SETTINGS[idx].ShadersHighQuality	= 0
	elseif level == SETUP_GRAPHIC_HIGH then
		SETUP_SETTINGS[idx].TexFilter			= SETUP_TEX_TRILINEAR
		SETUP_SETTINGS[idx].Shadows				= 1
		SETUP_SETTINGS[idx].DynamicLighting		= SETUP_LIGHTING_HIGH
		SETUP_SETTINGS[idx].HiMapsDetail		= 0
		SETUP_SETTINGS[idx].HiAnimQuality		= 1
		SETUP_SETTINGS[idx].DirtOnLevel			= 1
		SETUP_SETTINGS[idx].ShadersHighQuality	= 1
	elseif level == SETUP_GRAPHIC_HIGHEST then
		if idx == SETUP_DRIVER_OPENGL12 then
			SETUP_SETTINGS[idx].TexFilter			= SETUP_TEX_TRILINEAR
		else
			SETUP_SETTINGS[idx].TexFilter			= SETUP_TEX_ANISOTROPIC
		end		
		SETUP_SETTINGS[idx].Shadows				= 1
		SETUP_SETTINGS[idx].DynamicLighting		= SETUP_LIGHTING_HIGH
		SETUP_SETTINGS[idx].HiMapsDetail		= 1
		SETUP_SETTINGS[idx].HiAnimQuality		= 1
		SETUP_SETTINGS[idx].DirtOnLevel			= 1
		SETUP_SETTINGS[idx].ShadersHighQuality	= 1
	end
	SETUP_SETTINGS[idx].Antialiasing = 0
	UpdateGraphicLevel()
	UpdateControls()
end

function GetGraphicLevel()
	local level = SETUP_GRAPHIC_CUSTOM
	local idx = StartupDriverIndex
	if
		SETUP_SETTINGS[idx].TexFilter				== SETUP_TEX_BILINEAR
		and SETUP_SETTINGS[idx].Shadows				== 0
		and SETUP_SETTINGS[idx].DynamicLighting		== SETUP_LIGHTING_LOW
		and SETUP_SETTINGS[idx].HiMapsDetail		== 0
		and SETUP_SETTINGS[idx].HiAnimQuality		== 0
		and SETUP_SETTINGS[idx].DirtOnLevel			== 0
		and SETUP_SETTINGS[idx].ShadersHighQuality	== 0
		then
		level = SETUP_GRAPHIC_LOW
	elseif
		SETUP_SETTINGS[idx].TexFilter				== SETUP_TEX_TRILINEAR
		and SETUP_SETTINGS[idx].Shadows				== 1
		and SETUP_SETTINGS[idx].DynamicLighting		== SETUP_LIGHTING_MEDIUM
		and SETUP_SETTINGS[idx].HiMapsDetail		== 0
		and SETUP_SETTINGS[idx].HiAnimQuality		== 1
		and SETUP_SETTINGS[idx].DirtOnLevel			== 0
		and SETUP_SETTINGS[idx].ShadersHighQuality	== 0
		then
		level = SETUP_GRAPHIC_MEDIUM
	elseif
		(	(	idx == SETUP_DRIVER_OPENGL12 and
				SETUP_SETTINGS[idx].TexFilter == SETUP_TEX_TRILINEAR
			) or
			(SETUP_SETTINGS[idx].TexFilter == SETUP_TEX_ANISOTROPIC)
		)
		and SETUP_SETTINGS[idx].Shadows				== 1
		and SETUP_SETTINGS[idx].DynamicLighting		== SETUP_LIGHTING_HIGH
		and SETUP_SETTINGS[idx].HiMapsDetail		== 1
		and SETUP_SETTINGS[idx].HiAnimQuality		== 1
		and SETUP_SETTINGS[idx].DirtOnLevel			== 1
		and SETUP_SETTINGS[idx].ShadersHighQuality	== 1
		then
		level = SETUP_GRAPHIC_HIGHEST	
	elseif
		SETUP_SETTINGS[idx].Antialiasing			== 0
		and SETUP_SETTINGS[idx].TexFilter			== SETUP_TEX_TRILINEAR
		and SETUP_SETTINGS[idx].Shadows				== 1
		and SETUP_SETTINGS[idx].DynamicLighting		== SETUP_LIGHTING_HIGH
		and SETUP_SETTINGS[idx].HiMapsDetail		== 0
		and SETUP_SETTINGS[idx].HiAnimQuality		== 1
		and SETUP_SETTINGS[idx].DirtOnLevel			== 1
		and SETUP_SETTINGS[idx].ShadersHighQuality	== 1
		then
		level = SETUP_GRAPHIC_HIGH

	end
	return level
end

function UpdateGraphicLevel()
	local level = GetGraphicLevel()
	if level ~= SETUP_GRAPHIC_CUSTOM then
		if ComboGraphicDetail:getItemCount() >= SETUP_GRAPHIC_CUSTOM then
			local item = ComboGraphicDetail:getListboxItemFromIndex(SETUP_GRAPHIC_CUSTOM - 1)
			ComboGraphicDetail:removeItem(item)
		end
	else
		if ComboGraphicDetail:getItemCount() < SETUP_GRAPHIC_CUSTOM then
			local name	= SETUP_GRAPHIC_LEVEL[SETUP_GRAPHIC_CUSTOM].name
			local id	= SETUP_GRAPHIC_LEVEL[SETUP_GRAPHIC_CUSTOM].id
			AddListboxItem(ComboGraphicDetail, name, id)
		end
	end
	ComboGraphicDetail:setItemSelectState(level - 1, true)
end

local RunEngine = false

function ButtonHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "ButtonRun" then
		RunEngine = true
		MyDevice:close()
	elseif name == "ButtonExit" then
		MyDevice:close()
	end
end

function ComboHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "ComboDriverType" then
		local item = ComboDriverType:getSelectedItem()
		local idx = ComboDriverType:getItemIndex(item)
		StartupDriverIndex = idx + 1
		UpdateControls()
	elseif name == "ComboResolution" then
		local item = ComboResolution:getSelectedItem()
		local id = item:getID()
		SETUP_SETTINGS[StartupDriverIndex].ResolutionX = bit.rshift(id, 16)
		SETUP_SETTINGS[StartupDriverIndex].ResolutionY = bit.band(id, 65535)
	elseif name == "ComboRenderPath" then
		local item = ComboRenderPath:getSelectedItem()
		local idx = ComboRenderPath:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].Shaders = idx
		UpdateControls()
	elseif name == "ComboTexFilter" then
		local item = ComboTexFilter:getSelectedItem()
		local idx = ComboTexFilter:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].TexFilter = idx + 1
	elseif name == "ComboGeomDetail" then
		local item = ComboGeomDetail:getSelectedItem()
		local idx = ComboGeomDetail:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].HiMapsDetail = idx
	elseif name == "ComboLighting" then
		local item = ComboLighting:getSelectedItem()
		local idx = ComboLighting:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].DynamicLighting = idx + 1
	elseif name == "ComboAnimation" then
		local item = ComboAnimation:getSelectedItem()
		local idx = ComboAnimation:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].HiAnimQuality = idx
	elseif name == "ComboShaders" then
		local item = ComboShaders:getSelectedItem()
		local idx = ComboShaders:getItemIndex(item)
		SETUP_SETTINGS[StartupDriverIndex].ShadersHighQuality = idx
	elseif name == "ComboGraphicDetail" then
		local item = ComboGraphicDetail:getSelectedItem()
		local idx = ComboGraphicDetail:getItemIndex(item)
		SetGraphicLevel(idx + 1)
	end
	UpdateGraphicLevel()
end

function CheckHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "CheckVSync" then
		if CheckVSync:isSelected() then
			SETUP_SETTINGS[StartupDriverIndex].VerticalSync = 1
		else
			SETUP_SETTINGS[StartupDriverIndex].VerticalSync = 0
		end
	elseif name == "CheckFullscreen" then
		if CheckFullscreen:isSelected() then
			SETUP_SETTINGS[StartupDriverIndex].Fullscreen = 1
		else
			SETUP_SETTINGS[StartupDriverIndex].Fullscreen = 0
		end
	elseif name == "CheckDirt" then
		if CheckDirt:isSelected() then
			SETUP_SETTINGS[StartupDriverIndex].DirtOnLevel = 1
		else
			SETUP_SETTINGS[StartupDriverIndex].DirtOnLevel = 0
		end
	elseif name == "CheckShadows" then
		if CheckShadows:isSelected() then
			SETUP_SETTINGS[StartupDriverIndex].Shadows = 1
		else
			SETUP_SETTINGS[StartupDriverIndex].Shadows = 0
		end
	end
	UpdateGraphicLevel()
end

local needToRender = false

function OnEvent(args)
	local event = tolua.cast(args,"const io::SEvent")   
	if event.EventType == io.EET_PAINT_EVENT then
		needToRender = true
		return
	elseif event.EventType ~= io.EET_LOG_TEXT_EVENT and
			MyCEGUI.onEvent(event) then
		needToRender = true
		return
	elseif event.EventType == io.EET_KEY_INPUT_EVENT then
		if event.KeyInput.Event == io.EKIE_KEY_PRESSED_UP then
			if event.KeyInput.Key == io.EKC_ESCAPE then
				MyDevice:close()
			end
		end
	end
end

ButtonRun:subscribeEvent	("Clicked",					"ButtonHandler"	)
ButtonExit:subscribeEvent	("Clicked",					"ButtonHandler"	)
CheckVSync:subscribeEvent		("CheckStateChanged",		"CheckHandler"	)
CheckFullscreen:subscribeEvent	("CheckStateChanged",		"CheckHandler"	)
CheckDirt:subscribeEvent		("CheckStateChanged",		"CheckHandler"	)
CheckShadows:subscribeEvent		("CheckStateChanged",		"CheckHandler"	)
ComboDriverType:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboResolution:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboRenderPath:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboTexFilter:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboGeomDetail:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboLighting:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboAnimation:subscribeEvent		("ListSelectionAccepted",	"ComboHandler"	)
ComboGraphicDetail:subscribeEvent	("ListSelectionAccepted",	"ComboHandler"	)
ComboShaders:subscribeEvent			("ListSelectionAccepted",	"ComboHandler"	)

MyScript:setScriptCallback(scr.ESCT_ON_IO_EVENT, "OnEvent")

LoadStartupOptions()

InitControls()

-- don't want to load CPU at 100%
MyDevice:setMinimalRunPeriod(50)

MyDevice:show()
while MyDevice:run() do
	if needToRender then
		MyCEGUI.registerForRendering()
		if MyDriver:beginRendering() then
			MyDriver:renderAll()
			MyDriver:endRendering()
		end
		needToRender = false
	end
end

DestroyDevice()

SaveStartupOptions()

function RunGeneralEngine()
	local driverType= SETUP_DRIVER_TYPE[StartupDriverIndex].id
	local winWidth	= SETUP_SETTINGS[StartupDriverIndex].ResolutionX
	local winHeight	= SETUP_SETTINGS[StartupDriverIndex].ResolutionY
	local bits		= 32
	local texFilter	= SETUP_TEX_FILTER[SETUP_SETTINGS[StartupDriverIndex].TexFilter].id
	local flags		= dev.EDCF_SOUND_ON
	if SETUP_SETTINGS[StartupDriverIndex].Fullscreen == 1 then
		flags = bit.bor(flags, dev.EDCF_FULLSCREEN)
	end
	if SETUP_SETTINGS[StartupDriverIndex].Antialiasing == 1 then
		--flags = bit.bor(flags, dev.EDCF_ANTIALISING)
	end
	if SETUP_SETTINGS[StartupDriverIndex].VerticalSync == 1 then
		flags = bit.bor(flags, dev.EDCF_VERTICAL_SYNC)
	end
	if SETUP_SETTINGS[StartupDriverIndex].Shadows == 1 then
		flags = bit.bor(flags, dev.EDCF_SHADOWS)
	end
	flags = bit.bor(flags,
		SETUP_LIGHTING_QUALITY[SETUP_SETTINGS[StartupDriverIndex].DynamicLighting].id)
	if SETUP_SETTINGS[StartupDriverIndex].HiMapsDetail == 1 then
		flags = bit.bor(flags, dev.EDCF_HI_MAPS_DETAIL)
	end
	if SETUP_SETTINGS[StartupDriverIndex].HiAnimQuality == 1 then
		flags = bit.bor(flags, dev.EDCF_HI_ANIMATION_QUALITY)
	end
	if SETUP_SETTINGS[StartupDriverIndex].Shaders == 1 then
		flags = bit.bor(flags, dev.EDCF_USE_SHADERS)
	else
		flags = bit.bor(flags, dev.EDCF_USE_FFP)
	end
	if SETUP_SETTINGS[StartupDriverIndex].ShadersHighQuality == 1 then
		flags = bit.bor(flags, dev.EDCF_SHADERS_HIGH_QUALITY)
	end
	if SETUP_SETTINGS[StartupDriverIndex].DirtOnLevel == 1 then
		flags = bit.bor(flags, dev.EDCF_DIRT_ON_LEVEL)
	end
	if OPTIONS.CacheShaders == true then
		flags = bit.bor(flags, dev.EDCF_CACHE_SHADERS)
	end
	CreateDevice(driverType, winWidth, winHeight, bits, texFilter, flags)
	require "main"
	DestroyDevice()
end

if RunEngine == true then
	RunGeneralEngine()
end
