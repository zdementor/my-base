
#include <my.h>

#if MY_DEBUG_MODE
#	ifdef VLD_DEBUG
#		undef VLD_DEBUG
#	endif
//#	define VLD_DEBUG	
#	ifndef VLD_DEBUG
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	else
#		include "vld.h"
#	endif
#endif

#include <vid/SMaterial.h>

using namespace my;

const c8 *caption = "Test01 - Dynamic Lighting";

u32 GPUProgCur = 5;

const c8 *GPUProgNames[] =
{
	"Fixed Pipeline",
	"vertex_lighting.gpu",
	"perpixel_paralax_1_lighting.gpu",
	"perpixel_paralax_2_lighting.gpu",
	"perpixel_paralax_3_lighting.gpu",
	"perpixel_paralax_4_lighting.gpu",
};

vid::IGPUProgram *GPUProg[sizeof(GPUProgNames)/sizeof(*GPUProgNames)];

u32 CameraCur = 0;
scn::ICameraSceneNode *Cameras[] =
{NULL, NULL, NULL};

scn::ILightSceneNode *Lights[] =
{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

scn::ISceneNode *Nodes[] = { NULL, NULL};

struct SMediaEntry
{
	res::E_MEDIA_TYPE	type;
	c8					*dir;
};

SMediaEntry MediaEntry[res::E_MEDIA_TYPE_COUNT] =
{
	{res::EMT_ROOT,            "../../base/apps/test.media/"  },
	{res::EMT_TEXTURES,        "textures/"       },
	{res::EMT_FONTS,           "gui/fonts/"      },
	{res::EMT_MESHES,          "models/"         },
	{res::EMT_XML_SCRIPTS,     "xml/"            },
	{res::EMT_GAME_SCRIPTS,    "scripts/"        },
	{res::EMT_SOUND_TRACKS,    "sounds/tracks/"  },
	{res::EMT_SOUND_EFFECTS,   "sounds/effects/" },
	{res::EMT_TEMP_DATA,       "temp/"           },
	{res::EMT_MATERIALS,       "materials/"      },
	{res::EMT_GPU_PROGRAMS,    "materials/gpu/"  },
};

//-------------------------------------------------------------------------

scn::ILightSceneNode* addRotatedLightWithLensFlare(
	const img::SColorf &lColor, f32 lRadius,
	const core::vector3df &rotPos, f32 rotRadius, f32 rotSpeed)
{
	scn::ILightSceneNode *light =
		SCENE_MANAGER.addLightSceneNode();
	vid::SLight lparams = light->getLightParameters();
	lparams.Radius = lRadius;
	lparams.Type = vid::ELT_POINT;
	lparams.Enabled = true;
	lparams.AmbientColor.set(0.0f, 0.0f, 0.0f);
	lparams.DiffuseColor = lColor;
	lparams.SpecularColor = lColor;
	lparams.Dynamic = true;
	light->setLightParameters(lparams);
	scn::ISceneNodeAnimator *anim = SCENE_MANAGER.createFlyCircleAnimator(
		rotPos, rotRadius, rotSpeed);
	light->addAnimator(anim);
	anim->drop();

	vid::ITexture *texture = VIDEO_DRIVER.getTexture(
		core::stringc().sprintf("%s%s",
			RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
			"corona_white.jpg").c_str());
	scn::ILensFlareSceneNode *lens = SCENE_MANAGER.addLensFlareSceneNode(light);
	lens->setCorona(texture);
	lens->setEtalonScreenSize(core::dimension2di(320, 240));
	u32 mcnt = lens->getMaterialsCount();
	for (u32 i = 0; i < mcnt; i++)
	{
		vid::SMaterial &mat = lens->getMaterial(i);
		for (u32 j = 0; j < mat.getPassesCount(); j++)
		{
			vid::SRenderPass &pass = mat.getPass(j);
			pass.setDiffuseColor(lparams.DiffuseColor.toSColor());
		}
	}
	return light;
}

//-------------------------------------------------------------------------

void setGPUProg(scn::ISceneNode *node, vid::IGPUProgram *gpu_prog)
{
	u32 mcnt = node->getMaterialsCount();
	for (u32 i = 0; i < mcnt; i++)
	{
		vid::SMaterial &mat = node->getMaterial(i);
		for (u32 j = 0; j < mat.getPassesCount(); j++)
		{
			vid::SRenderPass &pass = mat.getPass(j);
			pass.setGPUProgram(gpu_prog);
		}
	}
}

//-------------------------------------------------------------------------

class MyEventReceiver : public io::IEventReceiver
{
public:
    virtual bool OnEvent(const io::SEvent &event)
	{	
		if(event.EventType==io::EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.Event == io::EKIE_KEY_PRESSED_UP)
			{				
				if (event.KeyInput.Key == io::EKC_ESCAPE)
				{
					DEVICE.close();
					return true;
				}
				if (event.KeyInput.Key >= io::EKC_1 && event.KeyInput.Key <= io::EKC_0)
				{
					u32 nlights = (event.KeyInput.Key == io::EKC_0) ?
						0 : (event.KeyInput.Key - io::EKC_1 + 1);

					for (u32 i = 0; i < sizeof(Lights) / sizeof(*Lights); i++)
					{
						if (i < nlights)
							Lights[i]->setVisible(true);
						else
							Lights[i]->setVisible(false);
					}
				}
				if (event.KeyInput.Key == io::EKC_C)
				{
					scn::ICameraSceneNode *cam_old = Cameras[CameraCur++];
					CameraCur = CameraCur % (sizeof(Cameras)/sizeof(*Cameras));
					scn::ICameraSceneNode *cam_new = Cameras[CameraCur];;

					cam_new->setPosition(cam_old->getPosition());
					cam_new->setTarget(cam_old->getTarget());
					cam_new->updateAbsoluteTransformation();

					SCENE_MANAGER.setActiveCamera(cam_new);
				}
				if (event.KeyInput.Key == io::EKC_A)
				{
					for (u32 i = 0; i < sizeof(Lights) / sizeof(*Lights); i++)
					{
						scn::ILightSceneNode *light = Lights[i];
						if (!light)
							continue;
						const core::list<scn::ISceneNodeAnimator*>& animators = light->getAnimators();
						core::list<scn::ISceneNodeAnimator*>::iterator it = animators.begin();
						for (; it != animators.end(); ++it)
						{
							scn::ISceneNodeAnimator *anim = *it;
							anim->setEnabled(!anim->isEnabled());
						}						
					}
				}
				if (event.KeyInput.Key == io::EKC_F)
				{
					VIDEO_DRIVER.setPolygonFillMode((vid::E_POLYGON_FILL_MODE)(
						((s32)VIDEO_DRIVER.getPolygonFillMode() + 1) %
							(s32)vid::E_POLYGON_FILL_MODE_COUNT));
				}
				if (event.KeyInput.Key == io::EKC_S ||
					event.KeyInput.Key == io::EKC_LBRACKET ||
					event.KeyInput.Key == io::EKC_RBRACKET)
				{
					if (event.KeyInput.Key == io::EKC_LBRACKET)
					{
						if (GPUProgCur > 0)
							GPUProgCur--;
						else
							GPUProgCur = sizeof(GPUProg)/sizeof(*GPUProg) - 1;
					}
					else
						GPUProgCur++;
					GPUProgCur = GPUProgCur % (sizeof(GPUProg)/sizeof(*GPUProg));

					for (u32 i = 0; i < sizeof(Nodes) / sizeof(*Nodes); i++)
						setGPUProg(Nodes[i], GPUProg[GPUProgCur]);
				}
				if (event.KeyInput.Key == io::EKC_R)
				{
					VIDEO_DRIVER.loadGPUProgramsFromDir(
						RESOURCE_MANAGER.getMediaDirFull(res::EMT_MATERIALS));
				}
			}			
		}
		return false;
    }
}
receiver;

namespace test
{

}


//-------------------------------------------------------------------------

int main(int argc, char* argv[]) 
{
#if MY_DEBUG_MODE

	#ifndef VLD_DEBUG 

	//_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

	int tmpDbgFlag; 

	HANDLE hLogFile = CreateFile(
		"VS_log.txt",GENERIC_WRITE,FILE_SHARE_WRITE, 
		NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
		); 
	_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW | _CRTDBG_MODE_DEBUG); 
	_CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); 
	_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW | _CRTDBG_MODE_DEBUG); 

	_CrtSetReportFile(_CRT_ASSERT,hLogFile); 
	_CrtSetReportFile(_CRT_WARN,hLogFile); 
	_CrtSetReportFile(_CRT_ERROR,hLogFile); 

	tmpDbgFlag=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); 
	tmpDbgFlag|=_CRTDBG_ALLOC_MEM_DF; 
	tmpDbgFlag|=_CRTDBG_DELAY_FREE_MEM_DF; 
	tmpDbgFlag|=_CRTDBG_LEAK_CHECK_DF; 
	
	_CrtSetDbgFlag(tmpDbgFlag);

	//_CrtSetBreakAlloc(811031); 

	#endif
#endif

	//-------------------------------------------------------------------------
	// show MyEngine Setup
	//-------------------------------------------------------------------------
   	
	vid::E_DRIVER_TYPE driverType = vid::EDT_OPENGL21;
	core::dimension2d<s32> resolution(640, 480);
	s32 colorBit=32;
	s32 Flags=0;
	vid::E_TEXTURE_FILTER TexFilter = vid::ETF_NONE;

	//Flags |= dev::EDCF_SOUND_ON;
	//Flags |= dev::EDCF_FULLSCREEN;
	Flags |= dev::EDCF_USE_SHADERS;
	Flags |= dev::EDCF_USE_FFP;
	Flags |= dev::EDCF_LIGHTING_MEDIUM_QUALITY;

	dev::SDeviceCreationParameters creation_params; 

	creation_params.DriverType		= driverType;
	creation_params.WindowSize		= resolution;
	creation_params.Bits			= colorBit;
	creation_params.TextureFilter	= TexFilter;
	creation_params.Flags			= Flags;
	creation_params.EventReceiver	= &receiver;
	creation_params.WindowHandle	= 0;

	//----------------------------------------------------
	// create MyEngine device and exit if creation failed
	//----------------------------------------------------

	//if (!scr::createCoreScriptManager())
	{
		//printf("Error! Can not create CoreScriptManager.");
		//return 0;	
	}

	if (!dev::createDevice(creation_params)) 
	{
		printf("Error! Can not create MyEngine device.");
		return 0;	
	}

	LOGGER.setLogLevel(io::ELL_INFORMATION);

	CURSOR_CONTROL.setVisible(true);

	for (u32 i = 0 ; i < sizeof(MediaEntry) /sizeof(*MediaEntry); i++)
		RESOURCE_MANAGER.setMediaDirRel(
			MediaEntry[i].type, MediaEntry[i].dir);

	//------------------------------------------------------------------------
    // main loop
    //------------------------------------------------------------------------	

	DEVICE.setWindowCaption(caption);
	DEVICE.show();

	{
		scn::IAnimatedMeshSceneNode *node = SCENE_MANAGER.addAnimatedMeshSceneNode();
		//scn::IAnimatedMesh *mesh = SCENE_MANAGER.addSphereMesh(50.0f, 6);
		//scn::IAnimatedMesh *mesh = SCENE_MANAGER.addBoxMesh(core::vector3df(50.0f, 50.0f, 50.0f));
		
		scn::IAnimatedMesh *mesh = SCENE_MANAGER.getMesh(
			core::stringc().sprintf("%s%s",
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"room.3ds").c_str());
			
		node->setAnimatedMesh(mesh);
		node->setScale(core::vector3df(1.0f, 1.0f, 1.0f));
		node->setPosition(core::vector3df(0.0f,0.0f,0.0f));
		node->setOccluder(false);

		Nodes[0] = node;
	}
	
	{
		scn::IAnimatedMeshSceneNode *node =
			SCENE_MANAGER.addAnimatedMeshSceneNode();
		scn::IAnimatedMesh *mesh = SCENE_MANAGER.getMesh(
			core::stringc().sprintf("%s%s",
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"room.3ds").c_str());
		node->setAnimatedMesh(mesh);
		node->setScale(core::vector3df(1.0f, 1.0f, 1.0f));
		node->setPosition(core::vector3df(-1000.0f,0.0f,0.0f));
		node->setOccluder(false);

		Nodes[1] = node;
	}

	VIDEO_DRIVER.loadGPUProgramsFromDir(
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_MATERIALS));
	VIDEO_DRIVER.loadGPUProgramsFromDir(
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_GPU_PROGRAMS));

	for (u32 i = 1; i < sizeof(GPUProg)/sizeof(*GPUProg); i++)
	{
		if (i == 0)
			GPUProg[i] = NULL;
		else
		{
			const c8 *dir = RESOURCE_MANAGER.getMediaDirFull(res::EMT_MATERIALS);
			GPUProg[i] = VIDEO_DRIVER.getGPUProgram(
				core::stringc().sprintf("%s%s", dir, GPUProgNames[i]).c_str());
		}
	}

	{
		vid::ITexture *texture0 = VIDEO_DRIVER.getTexture(
			core::stringc().sprintf("%s%s",
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
				"rockwall.jpg").c_str());

		bool nmapflg =
			VIDEO_DRIVER.getTextureCreationFlag(vid::ETCF_CREATE_NORMAL_MAP);
		VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_CREATE_NORMAL_MAP, true);
		VIDEO_DRIVER.setNormalMapCreationAmplitude(9.0f);
		vid::ITexture *texture1 = VIDEO_DRIVER.getTexture(
			core::stringc().sprintf("%s%s",
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
				"rockwall_height.bmp").c_str());
		VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_CREATE_NORMAL_MAP, nmapflg);

		for (u32 i = 0; i < sizeof(Nodes) / sizeof(*Nodes); i++)
		{
			scn::ISceneNode *node = Nodes[i];
			u32 mcnt = node->getMaterialsCount();
			for (u32 i = 0; i < mcnt; i++)
			{
				vid::SMaterial &mat = node->getMaterial(i);
				for (u32 j = 0; j < mat.getPassesCount(); j++)
				{
					vid::SRenderPass &pass = mat.getPass(j);
					pass.setLightingMode(vid::ELM_8_LIGHTS);
					pass.Layers[0].setTexture(texture0);
					pass.Layers[3].setTexture(texture1, vid::ETLT_NORMAL_MAP);
					pass.setAmbientColor(0xff000000);
					pass.setDiffuseColor(0xffffffff);
					pass.setSpecularColor(0xff505050);
					pass.setEmissiveColor(0xff000000);
					pass.setShininess(127.0f);
				}
			}
		}
	}

	for (u32 i = 0; i < sizeof(Nodes) / sizeof(*Nodes); i++)
		setGPUProg(Nodes[i], GPUProg[GPUProgCur]);

	INPUT_DISPATCHER.mapKey(io::EKC_UP,		game::EGA_MOVE_FORWARD);
	INPUT_DISPATCHER.mapKey(io::EKC_DOWN,	game::EGA_MOVE_BACKWARD);
	INPUT_DISPATCHER.mapKey(io::EKC_LEFT,	game::EGA_MOVE_LEFT_STRAFE);
	INPUT_DISPATCHER.mapKey(io::EKC_RIGHT,	game::EGA_MOVE_RIGHT_STRAFE);

	Cameras[0] = SCENE_MANAGER.addCameraSceneNodeMaya		(NULL, 100.0, 100.0, 100.0);
	Cameras[1] = SCENE_MANAGER.addCameraSceneNodeFPS		(NULL, 100.0, 100.0, 100.0);
	Cameras[2] = SCENE_MANAGER.addCameraSceneNodeOverview	(NULL, 100.0, 100.0);

	scn::ICameraSceneNode *camera = Cameras[0];		
	SCENE_MANAGER.setActiveCamera(camera);

	Lights[0] = addRotatedLightWithLensFlare(img::SColorf(0.1f, 1.0f, 0.1f), 300.0f,
		core::vector3df(50,300,0),220.0f, -0.0015f);
	Lights[1] = addRotatedLightWithLensFlare(img::SColorf(1.0f, 0.1f, 0.1f), 300.0f,
		core::vector3df(0,150,0), 200.0f, 0.0005f);
	Lights[2] = addRotatedLightWithLensFlare(img::SColorf(0.1f, 0.1f, 1.0f), 300.0f,
		core::vector3df(0,100,0), 300.0f, -0.0005f);
	Lights[3] = addRotatedLightWithLensFlare(img::SColorf(0.1f, 1.0f, 1.0f), 300.0f,
		core::vector3df(0,70,0), 250.0f, -0.00025f);
	Lights[4] = addRotatedLightWithLensFlare(img::SColorf(1.0f, 0.5f, 1.0f), 300.0f,
		core::vector3df(0,60,0), 250.0f, 0.000125f);
	Lights[5] = addRotatedLightWithLensFlare(img::SColorf(1.0f, 0.1f, 1.0f), 300.0f,
		core::vector3df(0,50,0), 250.0f, 0.00125f);
	Lights[6] = addRotatedLightWithLensFlare(img::SColorf(1.0f, 1.0f, 0.1f), 300.0f,
		core::vector3df(0,40,0), 250.0f, -0.00125f);
	Lights[7] = addRotatedLightWithLensFlare(img::SColorf(0.5f, 1.0f, 0.5f), 300.0f,
		core::vector3df(50,200,0),220.0f, 0.0015f);

	VIDEO_DRIVER.setBackgroundColor(0xff333333);

	{
		vid::SLight lparams = VIDEO_DRIVER.getGlobalLightParams();
		lparams.Enabled = false;
		lparams.Position.set(0,1000,0);
		lparams.SpecularColor.set(1.0f, 1.0f, 1.0);
		lparams.DiffuseColor.set(1.0f, 1.0f, 1.0);
		VIDEO_DRIVER.setGlobalLightParams(lparams);
	}

	camera->setPosition(core::vector3df(-68.0f,100.0f,56.0f));
	camera->setTarget(core::vector3df(0,0,0));
	camera->updateAbsoluteTransformation();

	VIDEO_DRIVER.setPolygonFillMode(vid::EPFM_SOLID);
	VIDEO_DRIVER.setTextureFilter(vid::ETF_TRILINEAR);

	while (DEVICE.run())
	{
		GAME_MANAGER.preRenderFrame();
		SCENE_MANAGER.preRenderScene();
		if (VIDEO_DRIVER.beginRendering())
		{
			VIDEO_DRIVER.renderAll();
			VIDEO_DRIVER.endRendering();
		}
		GAME_MANAGER.doGame();
		SCENE_MANAGER.postRenderScene();
		GAME_MANAGER.postRenderFrame();

		u32 fps = VIDEO_DRIVER.getCurrentFPS();
		static u32 last_fps = fps;
		static u32 last_gpu = 0;
		if (last_fps != fps || last_gpu != GPUProgCur)
		{
			static core::stringc str, str2;
			if (GPUProgCur)
				str2.sprintf("Using GPU Program (%s)", GPUProgNames[GPUProgCur]);
			else
				str2 = "Using Fixed Pipeline";
			str.sprintf("%s (FPS=%d, tris=%d, DIPs=%d) %s", caption, fps,
				VIDEO_DRIVER.getRenderedTrianglesCount(),
				VIDEO_DRIVER.getRenderedDIPsCount(), str2.c_str());
			DEVICE.setWindowCaption(str.c_str());
			last_fps = fps;
			last_gpu = GPUProgCur;
		}
	}

	//------------------------------------------------------------------------
    // free memory
    //------------------------------------------------------------------------	

	DEVICE.drop();
	//CORE_SCRIPT_MANAGER.drop();
    
#if MY_DEBUG_MODE 
	// Memory audit
	core::stringc fnameAfter = "LeakObjects.txt";
	printAllocNamesList((char *)fnameAfter.c_str());
#endif // #if MY_DEBUG_MODE 

	return 0;    
}
