
local _Ctrls =
{
	Root = {},
}

function _ScenedInitKTreeControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_ktree.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.KTree.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedKTreeUpdate,
		updateControls = _ScenedUpdateKTreeControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end

function _ScenedKTreeUpdate()
end

function _ScenedUpdateKTreeControls()
	_Ctrls.Root.Ctrl:setVisible(false)
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_KTREE_SCENE_NODE then
			_Ctrls.Root.Ctrl:setVisible(true)
		end
	end
end
