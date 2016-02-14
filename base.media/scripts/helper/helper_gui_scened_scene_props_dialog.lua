
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn		= {},
		CancelBtn	= {},
	},
	TabControl = {},
	Lighting =
	{
		Root = {},
		Buttons =
		{
			DiffuseBtn = {},
			AmbientBtn = {},
			SpecularBtn= {},
			GlobalAmbientBtn= {},
			ShadowColorBtn= {},
		},
		Editboxes =
		{
			DirXEdit = {},
			DirYEdit = {},
			DirZEdit = {},
		},
		ColorImages =
		{
			DiffuseImage = {},
			AmbientImage = {},
			SpecularImage = {},
			GlobalAmbientImage = {},
			ShadowImage = {},
		},
		CastShadowsCheck = {},
	},
	Fog =
	{
		Root = {},
		Buttons =
		{
			FogColorBtn = {},
		},
		ColorImages =
		{
			FogColorImage = {},
		},
		FogStartSpinner = {},
		FogEndSpinner = {},
	},
	LODs =
	{
		Root = {},
		LODNearestSpinner = {},
		LODNearSpinner = {},
		LODFarSpinner = {},
	},
}

local _Resources =
{
	Textures =
	{
		ColorImages =
		{
			DiffuseImage = nil,
			AmbientImage = nil,
			SpecularImage = nil,
			GlobalAmbientImage = nil,
			ShadowImage = nil,
			FogColorImage = nil,
		},
	},
}

local _ColorTbl =
{
	DiffuseImage = img.SColor(),
	AmbientImage = img.SColor(),
	SpecularImage = img.SColor(),
	GlobalAmbientImage = img.SColor(),
	ShadowImage = img.SColor(),
	FogColorImage = img.SColor(),
}

local _GameScenePropsInit = game.SGameSceneProps()
local _GameScenePropsNew = game.SGameSceneProps()

local function _ScenedScenePropsColorUpdate()
	_ColorTbl.DiffuseImage:set(_GameScenePropsNew.Video.GlobalLight.DiffuseColor)
	_ColorTbl.AmbientImage:set(_GameScenePropsNew.Video.GlobalLight.AmbientColor)
	_ColorTbl.SpecularImage:set(_GameScenePropsNew.Video.GlobalLight.SpecularColor)
	_ColorTbl.GlobalAmbientImage:set(MyDriver:getGlobalAmbientColor())
	_ColorTbl.ShadowImage:set(MyDriver:getShadowColor())
	local fog = MyDriver:getFog()
	_ColorTbl.FogColorImage:set(fog.Color)
	for key, value in pairs(_Resources.Textures.ColorImages) do
		if _ColorTbl[key] ~= nil then
			Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.ColorImages[key], _ColorTbl[key])
		else
			MyLogger:logInfo(string.format("Incorrect ColorImage in Scene Props (key='%s')", key))
		end
	end
end

local function _ScenedScenePropsDirectionUpdate()
	_Ctrls.Lighting.Editboxes.DirXEdit.Ctrl:setText(string.format("%.3f", -_GameScenePropsNew.Video.GlobalLight.Position.X))
	_Ctrls.Lighting.Editboxes.DirYEdit.Ctrl:setText(string.format("%.3f", -_GameScenePropsNew.Video.GlobalLight.Position.Y))
	_Ctrls.Lighting.Editboxes.DirZEdit.Ctrl:setText(string.format("%.3f", -_GameScenePropsNew.Video.GlobalLight.Position.Z))
end

local function _ScenedScenePropsUpdate()
	_ScenedScenePropsColorUpdate()
	_ScenedScenePropsDirectionUpdate()

	_Ctrls.Fog.FogStartSpinner.Ctrl:setCurrentValue(_GameScenePropsNew.Video.DistanceFog.Start)
	_Ctrls.Fog.FogEndSpinner.Ctrl:setCurrentValue(_GameScenePropsNew.Video.DistanceFog.End)
	
	_Ctrls.LODs.LODNearestSpinner.Ctrl:setCurrentValue(_GameScenePropsNew.Scene.LODDistances[scn.ELL_LOD_NEAREST])
	_Ctrls.LODs.LODNearSpinner.Ctrl:setCurrentValue(_GameScenePropsNew.Scene.LODDistances[scn.ELL_LOD_NEAR])
	_Ctrls.LODs.LODFarSpinner.Ctrl:setCurrentValue(_GameScenePropsNew.Scene.LODDistances[scn.ELL_LOD_FAR])

	_Ctrls.Lighting.CastShadowsCheck.Ctrl:setSelected(_GameScenePropsNew.Video.GlobalLight.CastShadows)
end

local function _ScenedScenePropsSetDiffuseColor(color)
	_GameScenePropsNew.Video.GlobalLight.DiffuseColor:set(color)
	MyGameMgr:setGameSceneProps(_GameScenePropsNew)
	--MyDriver:setGlobalLightParams(_GameScenePropsNew.Video.GlobalLight)
	_ScenedScenePropsColorUpdate()
end

local function _ScenedScenePropsSetAmbientColor(color)
	_GameScenePropsNew.Video.GlobalLight.AmbientColor:set(color)
	MyDriver:setGlobalLightParams(_GameScenePropsNew.Video.GlobalLight)
	_ScenedScenePropsColorUpdate()
end

local function _ScenedScenePropsSetSpecularColor(color)
	_GameScenePropsNew.Video.GlobalLight.SpecularColor:set(color)
	MyDriver:setGlobalLightParams(_GameScenePropsNew.Video.GlobalLight)
	_ScenedScenePropsColorUpdate()
end

local function _ScenedScenePropsSetGlobalAmbientColor(color)
	_GameScenePropsNew.Video.GlobalAmbientColor:set(color)
	MyDriver:setGlobalAmbientColor(_GameScenePropsNew.Video.GlobalAmbientColor)
	_ScenedScenePropsColorUpdate()
end

local function _ScenedScenePropsSetShadowColor(color)
	_GameScenePropsNew.Video.ShadowColor:set(color)
	MyDriver:setShadowColor(_GameScenePropsNew.Video.ShadowColor)
	_ScenedScenePropsColorUpdate()
end

local function _ScenedScenePropsSetFogColor(color)
	_GameScenePropsNew.Video.DistanceFog.Color:set(color)
	_GameScenePropsNew.Video.DistanceFog.Color:setAlpha(255)
	MyDriver:setFog(_GameScenePropsNew.Video.DistanceFog)
	_ScenedScenePropsColorUpdate()
end

local _TmpColor = img.SColor()

local function _ScenedScenePropsWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.Scened.SceneProps.OKBtn" then
		MyGameMgr:setGameSceneProps(_GameScenePropsNew)
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Helper.Scened.SceneProps.CancelBtn" or name == "Helper.Scened.SceneProps.Frame" then
		MyGameMgr:setGameSceneProps(_GameScenePropsInit)
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Helper.Scened.SceneProps.Lighting.DiffuseBtn" then
		_TmpColor:set(_GameScenePropsNew.Video.GlobalLight.DiffuseColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, _TmpColor, _ScenedScenePropsSetDiffuseColor)
	elseif name == "Helper.Scened.SceneProps.Lighting.AmbientBtn" then
		_TmpColor:set(_GameScenePropsNew.Video.GlobalLight.AmbientColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Ambient Color", 0, _TmpColor, _ScenedScenePropsSetAmbientColor)
	elseif name == "Helper.Scened.SceneProps.Lighting.SpecularBtn" then
		_TmpColor:set(_GameScenePropsNew.Video.GlobalLight.SpecularColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Specular Color", 0, _TmpColor, _ScenedScenePropsSetSpecularColor)
	elseif name == "Helper.Scened.SceneProps.Lighting.GlobalAmbientBtn" then
		_TmpColor:set(_GameScenePropsNew.Video.GlobalAmbientColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Scene Ambient Color", 0, _TmpColor, _ScenedScenePropsSetGlobalAmbientColor)
	elseif name == "Helper.Scened.SceneProps.Lighting.ShadowColorBtn" then
		Helper.GUI.ColorSelectionDialog.show(
			"Shadow Color", 0, _GameScenePropsNew.Video.ShadowColor, _ScenedScenePropsSetShadowColor)
	elseif name == "Helper.Scened.SceneProps.Fog.FogColorBtn" then
		_GameScenePropsNew.Video.DistanceFog.Color:setAlpha(255)
		Helper.GUI.ColorSelectionDialog.show(
			"Fog Color", 0, _GameScenePropsNew.Video.DistanceFog.Color, _ScenedScenePropsSetFogColor)
	end
end

local function _ScenedScenePropsTextAccepted(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.Scened.SceneProps.Lighting.DirXEdit"
			or name == "Helper.Scened.SceneProps.Lighting.DirYEdit"
			or name == "Helper.Scened.SceneProps.Lighting.DirZEdit" then
		local x = tonumber(_Ctrls.Lighting.Editboxes.DirXEdit.Ctrl:getText())
		local y = tonumber(_Ctrls.Lighting.Editboxes.DirYEdit.Ctrl:getText())
		local z = tonumber(_Ctrls.Lighting.Editboxes.DirZEdit.Ctrl:getText())
		_GameScenePropsNew.Video.GlobalLight.Position:set(-x, -y, -z)
		MyDriver:setGlobalLightParams(_GameScenePropsNew.Video.GlobalLight)
		_Ctrls.Lighting.Editboxes.DirXEdit.Ctrl:getText()
		_ScenedScenePropsDirectionUpdate()
	end
end

local function _ScenedScenePropsValueChanged(args)
	local pass = _ScenedMaterialGetSelectedPass()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.Scened.SceneProps.Fog.FogStartSpinner" then
		_GameScenePropsNew.Video.DistanceFog:set(MyDriver:getFog())
		_GameScenePropsNew.Video.DistanceFog.Start = _Ctrls.Fog.FogStartSpinner.Ctrl:getCurrentValue()
		MyDriver:setFog(_GameScenePropsNew.Video.DistanceFog)
	elseif name == "Helper.Scened.SceneProps.Fog.FogEndSpinner" then
		_GameScenePropsNew.Video.DistanceFog:set(MyDriver:getFog())
		_GameScenePropsNew.Video.DistanceFog.End = _Ctrls.Fog.FogEndSpinner.Ctrl:getCurrentValue()
		MyDriver:setFog(_GameScenePropsNew.Video.DistanceFog)
	elseif name == "Helper.Scened.SceneProps.LODs.LODNearestSpinner" then
		MyScnMgr:setLODDistance(scn.ELL_LOD_NEAREST,
			_Ctrls.LODs.LODNearestSpinner.Ctrl:getCurrentValue())
	elseif name == "Helper.Scened.SceneProps.LODs.LODNearSpinner" then
		MyScnMgr:setLODDistance(scn.ELL_LOD_NEAR,
			_Ctrls.LODs.LODNearSpinner.Ctrl:getCurrentValue())
	elseif name == "Helper.Scened.SceneProps.LODs.LODFarSpinner" then
		MyScnMgr:setLODDistance(scn.ELL_LOD_FAR,
			_Ctrls.LODs.LODFarSpinner.Ctrl:getCurrentValue())
	end
end

local function _ScenedScenePropsCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.Scened.SceneProps.Lighting.CastShadowsCheck" then
		_GameScenePropsNew.Video.GlobalLight.CastShadows = _Ctrls.Lighting.CastShadowsCheck.Ctrl:isSelected()
		MyDriver:setGlobalLightParams(_GameScenePropsNew.Video.GlobalLight)
	end
end

function _ScenedScenePropsInit()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_scened_scene_props.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Frame")

	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _ScenedScenePropsWidgetClicked)
	
	Helper.GUI.initButtons("Helper.Scened.SceneProps.",
		_Ctrls.Buttons, _ScenedScenePropsWidgetClicked)
	
	_Ctrls.Lighting.Root.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Lighting.Root")
	
	Helper.GUI.initButtons("Helper.Scened.SceneProps.Lighting.",
		_Ctrls.Lighting.Buttons, _ScenedScenePropsWidgetClicked)
	
	for key, value in pairs(_Ctrls.Lighting.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Helper.Scened.SceneProps.Lighting."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedScenePropsTextAccepted)
	end
	
	for key, value in pairs(_Ctrls.Lighting.ColorImages) do
		value.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Lighting."..key)
		local cegui_tex = nil
		local imageset = nil
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		_Resources.Textures.ColorImages[key] = cegui_tex
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		imageset = CEGUIImgsetMgr:create("Helper.Scened.SceneProps.Lighting."..key, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.Ctrl:setProperty("Image", "set:Helper.Scened.SceneProps.Lighting."..key.." image:ColorImage")
	end

	_Ctrls.Fog.Root.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Fog.Root")
	
	Helper.GUI.initButtons("Helper.Scened.SceneProps.Fog.",
		_Ctrls.Fog.Buttons, _ScenedScenePropsWidgetClicked)
		
	for key, value in pairs(_Ctrls.Fog.ColorImages) do
		value.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Fog."..key)
		local cegui_tex = nil
		local imageset = nil
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		_Resources.Textures.ColorImages[key] = cegui_tex
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		imageset = CEGUIImgsetMgr:create("Helper.Scened.SceneProps.Fog."..key, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.Ctrl:setProperty("Image", "set:Helper.Scened.SceneProps.Fog."..key.." image:ColorImage")
	end

	_Ctrls.Fog.FogStartSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Fog.FogStartSpinner"), "CEGUI::Spinner")
	_Ctrls.Fog.FogStartSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedScenePropsValueChanged)

	_Ctrls.Fog.FogEndSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Fog.FogEndSpinner"), "CEGUI::Spinner")
	_Ctrls.Fog.FogEndSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedScenePropsValueChanged)
	
	_Ctrls.LODs.Root.Ctrl = CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.LODs.Root")
	
	_Ctrls.LODs.LODNearestSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.LODs.LODNearestSpinner"), "CEGUI::Spinner")
	_Ctrls.LODs.LODNearestSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedScenePropsValueChanged)

	_Ctrls.LODs.LODNearSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.LODs.LODNearSpinner"), "CEGUI::Spinner")
	_Ctrls.LODs.LODNearSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedScenePropsValueChanged)

	_Ctrls.LODs.LODFarSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.LODs.LODFarSpinner"), "CEGUI::Spinner")
	_Ctrls.LODs.LODFarSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedScenePropsValueChanged)
	
	_Ctrls.Lighting.CastShadowsCheck.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.Scened.SceneProps.Lighting.CastShadowsCheck"), "CEGUI::Checkbox")
	_Ctrls.Lighting.CastShadowsCheck.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedScenePropsCheckStateChanged)
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _ScenedScenePropsShow()

	if not _HelperInit() then
		return false
	end
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()

	_GameScenePropsInit:set(MyGameMgr:getGameSceneProps())
	_GameScenePropsNew:set(_GameScenePropsInit)

	_TmpColor:set(_GameScenePropsNew.Video.GlobalLight.DiffuseColor)
	
	_ScenedScenePropsUpdate()
	
	return true
end
Helper.GUI.Scened.ScenePropsDialog.show = _ScenedScenePropsShow
