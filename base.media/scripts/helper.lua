
Helper =
{
	GUI =
	{
		FileDialog =
		{
			--------------------------------------------------------------------------------
			-- INFO: Shows up File Open/Save Dialog
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   mode            - dialog mode, one of the FILE_DIALOG_MODE enum values
			--   flags           - not used for now, can be 0
			--   caption         - text, shown on the title bar
			--   start_dir       - current directory, from which we starting from
			--   default_file    - default file to open/save, can be nil
			--   files_filter    - table with supported file types, must looks like shown bellow
            --     {
            --       [1]     = { Descr = "TGA images",  Value = "tga" },
            --       [2]     = { Descr = "JPEG images", Value = "jpg" },
            --       ["tga"] = { Descr = "TGA images",  Value = "tga" },
            --       ["jpg"] = { Descr = "JPEG images", Value = "jpg" },
            --     }
			--   accept_callback - entry to call, when dialog action performed
			--   user_data       - user data to pass in accept callback
			--
			-- CALLBACKS DECLS:
			--   accept_callback(full_file_name, rel_full_file_name, user_data)
			--------------------------------------------------------------------------------
			show,
		},
		TextEditor = 
		{
			--------------------------------------------------------------------------------
			-- INFO: Shows up Text Editor Dialog
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   mode            - text editor mode, not used for now, can be 0
			--   flags           - must be a composition of TEXT_EDITOR_FLAGS flags
			--   caption         - text, shown on the title bar
			--   file_name       - file to load into Text Editor and to Edit to
			--   open_callback   - entry to call, when Open action performed
			--   reload_callback - entry to call, when Reload action performed
			--   save_callback   - entry to call, when Save action performed
			--   close_callback  - entry to call, when Close action performed
			--
			-- CALLBACKS DECLS:
			--   open_callback  (file_name)
			--   reload_callback(file_name)
			--   save_callback  (file_name)
			--   close_callback ()
			--------------------------------------------------------------------------------
			show,

			setFileName,
			setText,
			getText,
		},
		ImageEditor = 
		{
			--------------------------------------------------------------------------------
			-- INFO: Shows up Image Editor Dialog
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   mode            - text editor mode, not used for now, can be 0
			--   flags           - must be a composition of IMAGE_EDITOR_FLAGS flags (OPEN | SAVE | RELOAD | USE_IMAGE_ALPHA)
			--   caption         - text, shown on the title bar
			--   file_name       - image file to load and to edit
			--   open_callback   - entry to call, when Open action performed
			--   reload_callback - entry to call, when Reload action performed
			--   save_callback   - entry to call, when Save action performed
			--   close_callback  - entry to call, when Close action performed
			--   user_data       - user data to pass in open/reload/save callbacks
			--
			-- CALLBACKS DECLS:
			--   open_callback  (file_name, rel_full_file_name, user_data)
			--   reload_callback(file_name, rel_full_file_name, user_data)
			--   save_callback  (file_name, rel_full_file_name, user_data)
			--   close_callback ()
			--------------------------------------------------------------------------------
			show,

			--------------------------------------------------------------------------------
			-- INFO: Set current editing Image
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   image_name     - image name to save
			--   save_file_name - file name to ave image to
			--------------------------------------------------------------------------------
			setImage,
		},
		TypeSelectionDialog =
		{
			--------------------------------------------------------------------------------
			-- INFO: Type Selection Dialog
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   mode             - dialog mode, not used for now, can be 0
			--   flags            - dialog flags, not used for now, can be 0
			--   caption          - text, shown on the title bar
			--   types_table      - table with types to select from, must looks like shown bellow
            --     {
            --       [1]     = { Name = "TypeName0", Value = TypeValue0 },
            --       [2]     = { Name = "TypeName1", Value = TypeValue1 },
			--       ...
            --       [N]     = { Name = "TypeNameN-1", Value = TypeValueN-1 },
            --     }
			--   accept_callback  - entry to call, when type selection accepted
			--
			-- CALLBACKS DECLS:
			--   accept_callback(sel_type)
			--------------------------------------------------------------------------------
			show,
		},
		ColorSelectionDialog =
		{
			show,
		},
		WaveSetupDialog =
		{
			show,
		},
		MaterialSelectionDialog =
		{
			show,
		},
		TextEnterDialog =
		{
			show,
		},
		MessageBox =
		{
			--------------------------------------------------------------------------------
			-- INFO: Message Box Dialog
			--------------------------------------------------------------------------------
			-- PARAMETERS:
			--   mode             - dialog mode, not used for now, can be 0
			--   flags            - dialog flags, not used for now, can be 0
			--   caption          - text, shown on the title bar
			--   msg_text         - message text
			--------------------------------------------------------------------------------
			show,
		},
		Scened =
		{
			ScenePropsDialog =
			{
				show,
			},
			NormalMapConverterDialog =
			{
				show,
			},
			ShaderEditor = 
			{
				--------------------------------------------------------------------------------
				-- INFO: Shows up Shader Editor Dialog
				--------------------------------------------------------------------------------
				-- PARAMETERS:
				--   mode            - not used
				--   flags           - not used
				--   caption         - text, shown on the title bar
				--   file_name       - file to load into Shader Editor and to edit (can be nil)
				--   sources         - program sources
				--     {
				--       Uniforms,
				--       LightsCount,
				--		Tag,
				--       Vertex = { Source, Ver},
				--       Pixel  = { Source, Ver},
				--     }
				--   open_callback   - entry to call, when Open action performed
				--   reload_callback - entry to call, when Reload action performed
				--   save_callback   - entry to call, when Save action performed
				--   accept_callback  - entry to call, when Accept action performed
				--   cancel_callback  - entry to call, when Cancel action performed
				--
				-- CALLBACKS DECLS:
				--   open_callback(file_name)
				--   reload_callback(file_name)
				--   save_callback(file_name)
				--   close_callback()
				--------------------------------------------------------------------------------
				show,
				setFileName,
				setFileName,
				setSources,
				getSources,
			},
		},
		freeCEGUIResources,
		initButtons,
		initImages,
		setImageTexture,
		setButtonImage,
		setCenteredWindowPosition,
		addComboboxItem,
		addListboxItem,
		addMultiColumnListItem,
		fillCEGUITextureWithColor,
		setupCurrentScreenResolution,
	},
	copyToClipboard,
	pasteFromClipboard,
	getClampedValue,
	getRelFileName,
	getPassLayerIndex,
	getMaterialPassIndex,
	getSelectedMaterial,

	--------------------------------------------------------------------------------
	-- INFO: Returns XML text for Scene Node
	--------------------------------------------------------------------------------
	-- PARAMETERS:
	--   scene_node   - node XML text returns for
	--   scene_only   - true if want scene tags only
	getSceneNodeFileText,

	makeScreenShot,
}

FILE_DIALOG_MODE =
{
	NEW  = 1,
	SAVE = 2,
	OPEN = 3,
}

TEXT_EDITOR_FLAGS =
{
	OPEN   = 1,
	SAVE   = 2,
	RELOAD = 4,
}

IMAGE_EDITOR_FLAGS =
{
	OPEN            = 1,
	SAVE            = 2,
	RELOAD          = 4,
	USE_IMAGE_ALPHA = 8,
}

MULTICOLUMN_FLAGS =
{
	SORTING_ENABLED = 1,
	SIZING_DISABLED = 2,
}

require "helper/helper_gui_file_dialog"
require "helper/helper_gui_text_editor"
require "helper/helper_gui_image_editor"
require "helper/helper_gui_type_selection_dialog"
require "helper/helper_gui_wave_setup_dialog"
require "helper/helper_gui_color_selection_dialog"
require "helper/helper_gui_material_selection_dialog"
require "helper/helper_gui_text_enter_dialog"
require "helper/helper_gui_message_box"
require "helper/helper_gui_scened_shader_editor"
require "helper/helper_gui_scened_scene_props_dialog"
require "helper/helper_gui_scened_normalmap_converter_dialog"
require "helper/helper_gui_common"
require "helper/helper_common"

local _HelperInitialized = false
local _HelperOK = false

function _HelperInit()

	if _HelperInitialized then
		return _HelperOK
	end
	_HelperInitialized = true
	LOG_INFO("Initializing Helper . . .")
	MyLogger:increaseFormatLevel()

	CEGUISchemeMgr:create(OPTIONS.HelperGUI.Scheme, OPTIONS.HelperGUI.ResourceGroup)
	CEGUISchemeMgr:create(OPTIONS.HelperGUI.SchemeAliases, OPTIONS.HelperGUI.ResourceGroup)
	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	_FileDialogInit()
	_TextEditorInit()
	_ImageEditorInit()
	_TypeSelectionDialogInit()
	_ColorSelectionDialogInit()
	_WaveSetupDialogInit()
	_MaterialSelectionDialogInit()
	_TextEnterDialogInit()
	_MessageBoxInit()
	_ScenedShaderEditorInit()	
	_ScenedScenePropsInit()
	_ScenedNMapConvInit()
	
	LOG_INFO("Helper initialized")
	MyLogger:decreaseFormatLevel()
	_HelperOK = true
	return _HelperOK
end
