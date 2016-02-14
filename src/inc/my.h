//|-------------------------------------------------------------------------
//| File:        my_inc.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef __MY__
#define __MY__
//----------------------------------------------------------------------------

// main includes

#include <my_inc.h>

// classes interfaces

#include <img/SColor.h>
#include <vid/SLight.h>
#include <scn/IAnimatedMesh.h>
#include <scn/IAnimatedMeshMD2.h>
#include <scn/ICameraSceneNode.h>
#include <scn/IDummyTransformationSceneNode.h>
#include <io/IEventReceiver.h>
#include <io/IFileList.h>
#include <io/IFileSystem.h>
#include <vid/IFont.h>
#include <dyn/IDynamicManager.h>
#include <dyn/IDynamicObject.h>
#include <scn/ILightSceneNode.h>
#include <io/ILogger.h>
#include <os/ITimer.h>
#include <os/ITimer.h>
#include <os/ITimer.h>
#include <scn/IMesh.h>
#include <scn/IMeshBuffer.h>
#include <io/IReadFile.h>
#include <dev/IDevice.h>
#include <scn/ISceneManager.h>
#include <scn/ISceneNode.h>
#include <scn/ISceneNodeAnimator.h>
#include <scn/IParticleSystemSceneNode.h>
#include <scn/ITerrainSceneNode.h>
#include <scn/IParticleEmitter.h>
#include <scn/IParticleAffector.h>
#include <scn/IBillboardSceneNode.h>
#include <vid/ITexture.h>
#include <IUnknown.h>
#include <vid/IVideoDriver.h>
#include <vid/IVideoModeList.h>
#include <io/IWriteFile.h>
#include <io/IXMLReader.h>
#include <io/IXMLWriter.h>
#include <game/IGameManager.h>
#include <vid/S3DVertex.h>
#include <scn/SAnimatedMesh.h>
#include <io/SKeyMap.h>
#include <vid/SMaterial.h>
#include <scn/SMesh.h>
#include <scn/ISkyBoxSceneNode.h>
#include <io/IInputDispatcher.h>
#include <scn/ITestSceneNode.h>
#include <scr/IScriptManager.h>
#include <img/IImageLibrary.h>
#include <mm/ISoundDriver.h>
#include <io/ICursorControl.h>
#include <IRunnable.h>
#include <os/ICriticalSection.h>
#include <scn/IPathFinderManager.h>
#include <dev/IPluginManager.h>
#include <res/IResourceManager.h>
#include <os/ILibraryModule.h>
#include <game/IPathFindTask.h>
#include <scn/IBSPLevelMesh.h>
#include <scn/IMaterialsManager.h>
#include <scn/ICullingSystem.h>
#include <dev/SDeviceCreationParameters.h>
#include <scn/ILensFlareSceneNode.h>
#include <vid/ITextureAnim.h>
#include <scn/IKTreeSceneNode.h> 
#include <scn/ISkyBoxSceneNode.h> 
#include <scn/mesh_buffer.h>

#include <game/IGameNodeButton.h>
#include <game/IGameNodeDecoration.h>
#include <game/IGameNodeDoor.h>
#include <game/IGameNodeItem.h>
#include <game/IGameNodeLevelMap.h>
#include <game/IGameNodeLight.h>
#include <game/IGameNodeMainPlayer.h>
#include <game/IGameNodePerson.h>
#include <game/IGameNodePlatform.h>
#include <game/IGameNodeSkyDome.h>
#include <game/IGameNodeTerrain.h>
#include <game/IGameNodeTrigger.h>
#include <game/IGameNodeWeapon.h>

//----------------------------------------------------------------------------
namespace my { 
namespace dev { 
//----------------------------------------------------------------------------

//! create myengine device
__MY_DEV_LIB_API__ IDevice* createDevice(
	//! Type of the device. 
	vid::E_DRIVER_TYPE driverType,
	//! Size of the window or the video mode in fullscreen mode.
	u32 windowWidth, u32 windowHeight,
	//! Bits per pixel in fullscreen mode. Ignored if windowed mode.
	u32 bits,
	//! Texture filter
	vid::E_TEXTURE_FILTER textureFilter,
	//! Device creation flags
	u32 flags);

//----------------------------------------------------------------------------

//! create myengine device
__MY_DEV_LIB_API__ IDevice* createDevice(
	// device creation parameters
	SDeviceCreationParameters params);

//----------------------------------------------------------------------------

//! create myengine device
extern "C" __MY_DEV_LIB_API__ IDevice * createDeviceEx(
	// device creation parameters
	SDeviceCreationParameters params);

//----------------------------------------------------------------------------

//! destroy myengine device
__MY_DEV_LIB_API__ bool destroyDevice();

//----------------------------------------------------------------------------
} //end namespace dev    
} //end namespace my        
//----------------------------------------------------------------------------

// MyEngine consist of folowing name spaces:
//	dev  - engine device
//	gui  - GUI system
//	img  - 2d-image library
//	vid  - 3d-graphic library
//	io   - i/o system
//	os   - os-functions
//	scn  - 3d scene
//	core - core types and template library
//	game - game stuff
//	dyn  - dynamic system
//  scr  - scripting system
//  mm   - multimedia system
//  res  - resources

#endif //__MY__
