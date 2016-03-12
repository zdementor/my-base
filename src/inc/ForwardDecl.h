//|-------------------------------------------------------------------------
//| File:        ForwardDecl.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ForwardDeclHPP
#define ForwardDeclHPP
//---------------------------------------------------------------------------

//! Forward declarations of commonly used classes of MyEngine

//---------------------------------------------------------------------------
namespace my {
//---------------------------------------------------------------------------

class IUnknown;
class IRunnable;

//---------------------------------------------------------------------------

//! Input/Output System Classes

namespace io {

class ILogger;
class IInputDispatcher;
class IFileSystem;
class IReadFile;
class IWriteFile;
class IFileList;
class IXMLReader;
class IXMLWriter;
struct SKeyMap;
class ICursorControl;
class IEventReceiver;
class IEventCapturer;
struct SEvent;
class ITextReader;

} // end namespace io

//---------------------------------------------------------------------------

//! Operating System Classes

namespace os {

class ITimer;
class ITimerMain;
class ITimerSecond;
class ICriticalSection;
class IEvent;
class IThread;
class ILibraryModule;
struct SExposedOSData;
class IMemStatus;

} // end namespace os

//---------------------------------------------------------------------------

//! 2D Image Classes

namespace img {

class IImageLoader;
class IImageLibrary;
class IImageSaver;
class IImage;
struct SColor;
struct SColorf;

} // end namespace img

//---------------------------------------------------------------------------

//! 3D Graphic Classes

namespace vid {

class IVideoDriver;
struct S3DVertexSimple;
struct S3DVertex1TCoords;
struct S3DVertex1TCoordsColoured;
struct S3DVertex2TCoords;
struct S3DVertex2TCoordsColoured;
struct S3DVertex1TCoordsTBN;
struct S3DVertex1TCoordsTBNColoured;
struct S3DVertex2TCoordsTBN;
struct S3DVertex2TCoordsTBNColoured;
class IVideoModeList;
class ITexture;
class ITextureAnim;
struct SMaterial;
struct SRenderPass;
struct STextureLayer;
struct SAnimatedTextureParams;
class IHardwareOcclusionQuery;
struct SExposedVideoData;
struct SFog;
class IRenderBuffer;
class IRenderTarget;
struct SLight;

class IFont;
class IGPUProgram;

} // end namespace vid

//---------------------------------------------------------------------------

//! Scene classes

namespace scn {

class ISceneManager;
struct SMesh;
class IMesh;
class IAnimatedMesh;
class ISceneNode;
class ICameraSceneNode;
class ISceneNodeAnimator;
class ISceneNodeAnimatorCollisionResponse;
class ILightSceneNode;
class IBillboardSceneNode;
class ITerrainSceneNode;
class IMeshLoader;
class IParticleSystemSceneNode;
class IDummyTransformationSceneNode;
class IAnimatedMeshSceneNode;
class IMeshManipulator;
struct SParticleSystemParams;
class ISkyBoxSceneNode;
class ILensFlareSceneNode;
struct SLens;
class IShadow;
class ITestSceneNode;
class IPathFinder;
class IPathFinderManager;
struct SSceneNodeCommonParams;
struct SSceneNodeParameters;
struct SSceneNodeTransformation;
struct SSceneNodeMaterial;
class ILightGridManager;
class ILightGrid;
struct SLightVolume;
struct SLightGridData;
class IMaterialsManager;
class ICullingSystem;
class IAnimationEndCallBack;
class IMeshAnimationEndCallBack;
class IParticleEmitter;
class IParticleAffector;
struct SImposterParameters;
struct SImposterCreationParameters;
class IKTreeSceneNode;
class ISkyBoxSceneNode; 
class ITextSceneNode;
struct SPathCell;
class ICameraController;
struct STileSetParams;

} // end namespace scn

//---------------------------------------------------------------------------

// Device Classes

namespace dev {

class IDevice;
class IPluginManager;
struct SExposedDeviceData;
struct SDeviceCreationParameters;
struct SProfileParameter;
class IProfiler;

} // end namespace dev

//---------------------------------------------------------------------------

//! Game Classes

namespace game {

class IGameNode;
struct SGameNodeCommonParams;
struct SGameNodeParams;
struct SGameTask;
struct SGameEvent;
class IGameUnitsFormation;
class IGameNodeDoor;
class IGameNodeTrigger;
class IGameNodeAI;
class IGameNodeWeapon;
class IGameNodeMainPlayer;
class IGameNodePerson;
class IGameNodeButton;
class IGameInventory;
class IGameNodeItem;
struct SWeaponParameters;
class IGameManager;
class CGameManager;
class IGameAIModule;
class IGameEventsDispatcher;
class IGameTasksManager;
class IPathFindTask;
class IGameNodeLevelMap;
struct SGameSceneProps;
struct SGameCameraStyle;
struct SCameraState;
struct SCameraZoom;

} // end namespace game

//---------------------------------------------------------------------------

//! Dynamic System Classes

namespace dyn {

class IDynamicManager;
class IDynamicObject;
struct SDynamicObjectParams;
struct SCollisionPoints;

}// end namespace dyn

//---------------------------------------------------------------------------

//! Script System Classes

namespace scr {

class IScriptManager;
class ICoreScriptManager;
struct SScriptFunctor;

}// end namespace src

//---------------------------------------------------------------------------

//! Multimedia System Classes

namespace mm {

class ISoundDriver;
class ISound;
class ISoundData;
class ISoundObject;

}// end namespace mm

//---------------------------------------------------------------------------

// Resource Classes

namespace res {

class IResourceManager;

} // end namespace res

//---------------------------------------------------------------------------
} // end namespace my
//---------------------------------------------------------------------------

namespace CEGUI {

class String;

} // end namespace CEGUI

//---------------------------------------------------------------------------

#endif // #ifndef ForwardDeclHPP