
local _Ctrls =
{
	Root = {},
	Frame = {},
	Image = {},
	Buttons =
	{
		SaveButton			= {Name="Helper.ImageEditor.SaveButton", ImageName = "set:TangoGUIIcons image:actions.document-save"				},
		OpenButton			= {Name="Helper.ImageEditor.OpenButton", ImageName = "set:TangoGUIIcons image:actions.document-open"				},
		ReloadButton		= {Name="Helper.ImageEditor.ReloadButton", ImageName = "set:TangoGUIIcons image:actions.view-refresh"				},
		IncScaleButton		= {Name="Helper.ImageEditor.IncScaleButton", ImageName = "set:TangoGUIIcons image:actions.image-zoomin"				},
		DecScaleButton		= {Name="Helper.ImageEditor.DecScaleButton", ImageName = "set:TangoGUIIcons image:actions.image-zoomout"				},
		FullscreenButton	= {Name="Helper.ImageEditor.FullscreenButton", ImageName = "set:TangoGUIIcons image:actions.view-fullscreen"			},
		WindowedButton		= {Name="Helper.ImageEditor.WindowedButton", ImageName = "set:TangoGUIIcons image:actions.view-windowed"				},
		BottomAlignButton	= {Name="Helper.ImageEditor.BottomAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-bottom-aligned"		},
		TopAlignButton		= {Name="Helper.ImageEditor.TopAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-top-aligned"			},
		LeftAlignButton		= {Name="Helper.ImageEditor.LeftAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-left-aligned"			},
		RightAlignButton	= {Name="Helper.ImageEditor.RightAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-right-aligned"			},
		IdentScaleButton	= {Name="Helper.ImageEditor.IdentScaleButton", ImageName = nil },
	},
	ScaleScroll = {Name="Helper.ImageEditor.ScaleScroll"},
}

local _Callbacks =
{
	Open	= nil,
	Reload	= nil,
	Save	= nil,
	Close	= nil,
}

local _Resources =
{
	Textures =
	{
		ImageEmpty = nil,
		Image = nil,
	},
	Imagesets =
	{
		ImageEmpty = nil,
		Image = nil,
	},
}

local IMAGE_EDITOR_VIEW_SCROLL_IDENT = 3.0
local IMAGE_EDITOR_VIEW_SCROLL_STEP = 1.0

local _ImageEditorCurScale = 1.0
local IMAGE_EDITOR_VIEW_SCALE_MIN  = math.pow(2.0, -IMAGE_EDITOR_VIEW_SCROLL_IDENT)
local IMAGE_EDITOR_VIEW_SCALE_MAX  = math.pow(2.0, IMAGE_EDITOR_VIEW_SCROLL_IDENT)

local _ImageEditorCaption = "Image Editor:"
local _ImageEditorFileName = ""

local IMAGE_EDITOR_STATE_FULLSCREEN		= 0
local IMAGE_EDITOR_STATE_WINDOWED		= 1
local IMAGE_EDITOR_STATE_BOTTOM_ALIGNED	= 2
local IMAGE_EDITOR_STATE_TOP_ALIGNED	= 3
local IMAGE_EDITOR_STATE_LEFT_ALIGNED	= 4
local IMAGE_EDITOR_STATE_RIGHT_ALIGNED	= 5

local _ImageEditorState = IMAGE_EDITOR_STATE_WINDOWED
local _ImageEditorFlags = 0
local _ImageSize = core.dimension2di(255, 255)
local _ImageEditorUserData = nil
	
local function _ImageEditorSetScrollScale(scale)
	if scale <= IMAGE_EDITOR_VIEW_SCALE_MIN then
		scale = IMAGE_EDITOR_VIEW_SCALE_MIN
	end
	if scale >= IMAGE_EDITOR_VIEW_SCALE_MAX then
		scale = IMAGE_EDITOR_VIEW_SCALE_MAX
	end
	_ImageEditorCurScale = scale
end

local function _ImageEditorUpdateScaleControls()
	if _ImageEditorCurScale <= IMAGE_EDITOR_VIEW_SCALE_MIN then
		_Ctrls.Buttons.DecScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.DecScaleButton.Ctrl:enable()
	end
	if _ImageEditorCurScale >= IMAGE_EDITOR_VIEW_SCALE_MAX then
		_Ctrls.Buttons.IncScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.IncScaleButton.Ctrl:enable()
	end
	if _ImageEditorCurScale == 1.0 then
		_Ctrls.Buttons.IdentScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.IdentScaleButton.Ctrl:enable()
	end
	
	local img_sz =  CEGUI.UVector2(
		CEGUI.UDim(0.0, _ImageSize.Width * _ImageEditorCurScale),
		CEGUI.UDim(0.0, _ImageSize.Height * _ImageEditorCurScale))

	local img_area = CEGUI.URect(_Ctrls.Image.Ctrl:getArea().min, _Ctrls.Image.Ctrl:getArea().max)
	img_area.max.x.offset = img_area.min.x.offset + img_sz.x.offset
	img_area.max.y.offset = img_area.min.y.offset + img_sz.y.offset

	_Ctrls.Image.Ctrl:setArea(img_area)
	_Ctrls.Image.Ctrl:setMaxSize(img_sz)
		
end

local function _ImageEditorUpdateControls()

	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl:enable()
	end
	_Ctrls.Buttons.SaveButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.SAVE) ~= 0)
	_Ctrls.Buttons.OpenButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.OPEN) ~= 0)
	_Ctrls.Buttons.ReloadButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.RELOAD) ~= 0)
	
	_Ctrls.Frame.Ctrl:setSizingEnabled(false)
	
	if _ImageEditorState == IMAGE_EDITOR_STATE_FULLSCREEN then
		_Ctrls.Buttons.FullscreenButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_WINDOWED then
		_Ctrls.Buttons.WindowedButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setSizingEnabled(true)
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_BOTTOM_ALIGNED then
		_Ctrls.Buttons.BottomAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_TOP_ALIGNED then
		_Ctrls.Buttons.TopAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_LEFT_ALIGNED then
		_Ctrls.Buttons.LeftAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_RIGHT_ALIGNED then
		_Ctrls.Buttons.RightAlignButton.Ctrl:disable()
	end

	_ImageEditorUpdateScaleControls()
end

local function _ImageEditorOpenImage(full_file_name)

	local cegui_tex = _Resources.Textures.Image
	local my_tex = MyDriver:findTexture(full_file_name)
	local image = nil

	image = MyImgLib:getImage(full_file_name)
	if image ~= nil then
		if my_tex == nil then
			my_tex = MyDriver:addTexture(full_file_name, image)
		end
	end

	MyCEGUI.setTexture(cegui_tex, my_tex)

	if my_tex ~= nil then
		local imageset = _Resources.Imagesets.Image
		imageset:undefineAllImages()
		imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0))
		_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.Image image:Image")
		_ImageSize:set(cegui_tex:getSize().width, cegui_tex:getSize().height)
	else
		_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.ImageEmpty image:Image")
		_ImageSize:set(256, 256)
		MyLogger:logErr("Can't open image from file '"..tostring(full_file_name).."'")
	end

	_ImageEditorFileName = full_file_name
	_Ctrls.Frame.Ctrl:setText(
		_ImageEditorCaption..": "..core.extractFileName(full_file_name))

	_ImageEditorUpdateScaleControls()

	if my_tex ~= nil then		
		return true
	end
	return false
end


local function _ImageEditorOpenImageAccepted(full_file_name, rel_full_file_name, user_data)
	local my_tex = _ImageEditorOpenImage(full_file_name)
	if my_tex ~= nil then	
		if _Callbacks.Open ~= nil then
			_Callbacks.Open(full_file_name, rel_full_file_name, user_data)
		end
	end
end

local function _ImageEditorSaveImageAccepted(full_file_name, rel_full_file_name, user_data)
	local cegui_tex = _Resources.Textures.Image
	local imageset = _Resources.Imagesets.Image
	local my_tex = MyCEGUI.getTexture(cegui_tex)
	local tex_file_name = MyDriver:findTextureName(my_tex)
	local image = nil
	if tex_file_name ~= nil then
		image = MyImgLib:getImage(tex_file_name)
	end
	if image ~= nil then
		MyImgLib:saveImageToFile(image, full_file_name)
		if _Callbacks.Save ~= nil then
			_Callbacks.Save(full_file_name, rel_full_file_name, user_data)
		end
	else
		MyLogger:logInfo("Can't save image to "..full_file_name)
	end
end

local function _ImageEditorSetFileName(file_name)
	if not _HelperInit() then
		return false
	end
	_ImageEditorOpenImage(file_name)
	_ImageEditorUpdateControls()
	return true
end

local function _ImageEditorClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ImageEditor.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
		if _Callbacks.Close ~= nil then
			_Callbacks.Close()
		end
	elseif name == _Ctrls.Buttons.DecScaleButton.Name then
		local scroll = _Ctrls.ScaleScroll.Ctrl:getScrollPosition() - IMAGE_EDITOR_VIEW_SCROLL_STEP
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(scroll)
	elseif name == _Ctrls.Buttons.IncScaleButton.Name then		
		local scroll = _Ctrls.ScaleScroll.Ctrl:getScrollPosition() + IMAGE_EDITOR_VIEW_SCROLL_STEP
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(scroll)
	elseif name == _Ctrls.Buttons.IdentScaleButton.Name then		
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)
	elseif name == _Ctrls.Buttons.SaveButton.Name then
		local cegui_tex = _Resources.Textures.Image
		local my_tex = MyCEGUI.getTexture(cegui_tex)
		local tex_file_name = MyDriver:findTextureName(my_tex)
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save image...",
			MyResMgr:getMediaDirFull(res.EMT_TEXTURES),
			tex_file_name,
			SCENED_IMAGE_SAVE_FILE_FILTER, _ImageEditorSaveImageAccepted, _ImageEditorUserData)
	elseif name == _Ctrls.Buttons.OpenButton.Name then
		local cegui_tex = _Resources.Textures.Image
		local my_tex = MyCEGUI.getTexture(cegui_tex)
		local tex_file_name = nil
		local start_dir = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if my_tex ~= nil then
			tex_file_name = MyDriver:findTextureName(my_tex)
			if string.len(tex_file_name) > 0 and string.sub(tex_file_name, 0, 1) == "#" then
				tex_file_name = nil
			else
				start_dir = core.extractFilePath(tex_file_name)
			end
		end						
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open image...",
			start_dir, tex_file_name,
			SCENED_IMAGE_OPEN_FILE_FILTER, _ImageEditorOpenImageAccepted, _ImageEditorUserData)
	elseif name == _Ctrls.Buttons.ReloadButton.Name then
		_ImageEditorSetFileName(_ImageEditorFileName)
		_Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)
		if _Callbacks.Reload ~= nil then
			_Callbacks.Reload(_ImageEditorFileName, nil, _ImageEditorUserData)
		end
	elseif name == _Ctrls.Buttons.FullscreenButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_FULLSCREEN
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{1.0,0},{1.0,0}}")
	elseif name == _Ctrls.Buttons.WindowedButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_WINDOWED
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.1,0},{0.2,0},{0.9,0},{0.8,0}}")
	elseif name == _Ctrls.Buttons.BottomAlignButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_BOTTOM_ALIGNED
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.5,0},{1.0,0},{1.0,0}}")
	elseif name == _Ctrls.Buttons.TopAlignButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_TOP_ALIGNED
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.,0},{0.0,0},{1.0,0},{0.5,0}}")
	elseif name == _Ctrls.Buttons.LeftAlignButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_LEFT_ALIGNED
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{0.66,0},{1.0,0}}")
	elseif name == _Ctrls.Buttons.RightAlignButton.Name then
		_ImageEditorState = IMAGE_EDITOR_STATE_RIGHT_ALIGNED
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.33,0},{0.0,0},{1.0,0},{1.0,0}}")
	end
	_ImageEditorUpdateControls()
end

local function _ImageEditorScrollPosChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.ScaleScroll.Name then
		local scale = math.pow(2, _Ctrls.ScaleScroll.Ctrl:getScrollPosition() - IMAGE_EDITOR_VIEW_SCROLL_IDENT)
		_ImageEditorSetScrollScale(scale)
		_ImageEditorUpdateScaleControls()
	end
end

function _ImageEditorInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_ImageEditor.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)

	_Ctrls.Frame.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ImageEditor.Frame"), "CEGUI::FrameWindow")

	_Ctrls.Image.Ctrl = CEGUIWinMgr:getWindow("Helper.ImageEditor.Image")

	local cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 40, 40, 40))
	_Resources.Textures.ImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Helper.ImageEditor.ImageEmpty", cegui_tex)
	imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.ImageEmpty = imageset
	
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.Image = cegui_tex
	imageset = CEGUIImgsetMgr:create("Helper.ImageEditor.Image", cegui_tex)
	imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.Image = imageset

	_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.ImageEmpty image:Image")

	_Ctrls.Image.Ctrl:setMouseCursor(OPTIONS.ScenedGUI.SchemeName, "MouseMoveCursor")
	
	Helper.GUI.initButtons("", _Ctrls.Buttons, _ImageEditorClick)

	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _ImageEditorClick)

	_Ctrls.ScaleScroll.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow(_Ctrls.ScaleScroll.Name), "CEGUI::Scrollbar")

	_Ctrls.ScaleScroll.Ctrl:setDocumentSize(IMAGE_EDITOR_VIEW_SCROLL_IDENT * 2. + 1.)
	_Ctrls.ScaleScroll.Ctrl:setPageSize(IMAGE_EDITOR_VIEW_SCROLL_STEP)
	_Ctrls.ScaleScroll.Ctrl:setStepSize(IMAGE_EDITOR_VIEW_SCROLL_STEP)

	_Ctrls.ScaleScroll.Ctrl:subscribeEvent("ScrollPosChanged", _ImageEditorScrollPosChanged)

	_Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)

	_ImageEditorState = IMAGE_EDITOR_STATE_WINDOWED
	_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.1,0},{0.2,0},{0.9,0},{0.8,0}}")

	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _ImageEditorShow(mode, flags, caption, file_name,
	file_open_callback, file_reload_callback, file_save_callback, close_callback, user_data)
	
	if not _HelperInit() then
		return false
	end
	
	_ImageEditorCaption = caption

	_ImageEditorFlags = flags

	_Callbacks.Open		= file_open_callback
	_Callbacks.Reload	= file_reload_callback
	_Callbacks.Save		= file_save_callback
	_Callbacks.Close	= close_callback
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
	
	_ImageEditorSetFileName(file_name)
	
	_ImageEditorUserData = user_data
	
	return true
end
Helper.GUI.ImageEditor.show = _ImageEditorShow
