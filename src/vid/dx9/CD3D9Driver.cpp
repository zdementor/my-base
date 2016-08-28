//|-------------------------------------------------------------------------
//| File:        CD3D9Driver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CD3D9Driver.h"  

//----------------------------------------------------------------------------
#if MY_PLATFORM == MY_PLATFORM_WIN32
#ifdef _IRR_COMPILE_WITH_DIRECTX_9_
//----------------------------------------------------------------------------

#include "CD3D9Texture.h" 
#include "CD3D9GPUProgram.h"
#include "CD3D9RenderBuffer.h"

#include <scn/ISceneManager.h>
#include <io/ILogger.h>
#include <dev/IDevice.h>
#include <img/IImageLibrary.h>

#include <d3d/d3dx9math.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

#include "D3D9Helper.h"

//----------------------------------------------------------------------------

//! constructor
CD3D9Driver::CD3D9Driver(const core::dimension2d<s32>& screenSize) 
	: CNullDriver(screenSize), 
D3DLibrary(0), pID3DDevice(0), pID3D(0), DeviceLost(false),
MaxAnisotropyLevel(0), StencilFogTexture(0), pEventQuery(0),
m_D3D9HardwareOcclusionQuery(0), PrevRenderTarget(0)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Driver");    
#endif

    // init direct 3d is done in the factory function
    MaxLightDistance = sqrtf(FLT_MAX);

	memset(m_VertexDecls, 0, sizeof(m_VertexDecls));

	m_DriverType = EDT_DIRECTX9;
}

//----------------------------------------------------------------------------

//! destructor
CD3D9Driver::~CD3D9Driver()
{
	free();

	SAFE_DROP(m_D3D9HardwareOcclusionQuery);

    SAFE_RELEASE(StencilFogTexture);   
	SAFE_RELEASE(pEventQuery);

	for (u32 i = 0; i < E_VERTEX_TYPE_COUNT; i++)
		SAFE_RELEASE(m_VertexDecls[i]);

    // drop d3d8
    SAFE_RELEASE(pID3DDevice);
    SAFE_RELEASE(pID3D);

	LOGGER.log("Direct3D9 graphic deactivated");
}

//----------------------------------------------------------------------------

bool CD3D9Driver::_initDriver(SExposedVideoData &out_video_data)
{
    HRESULT hr;
    D3DLibrary = LoadLibrary( "d3d9.dll" );

	bool pureSoftware = false;

    if (!D3DLibrary)
    {
        LOGGER.log(" Error, could not load d3d9.dll.", io::ELL_ERROR);
        return false;
    }

    typedef IDirect3D9 * (__stdcall *D3DCREATETYPE)(UINT);
    D3DCREATETYPE d3dCreate = (D3DCREATETYPE) GetProcAddress(D3DLibrary, "Direct3DCreate9");

    if (!d3dCreate)
    {
        LOGGER.logErr(" Error, could not get proc adress of Direct3DCreate9.");
        return false;
    }

    //just like pID3D = Direct3DCreate8(D3D_SDK_VERSION);
    pID3D = (*d3dCreate)(D3D_SDK_VERSION);    

    if (!pID3D)
    {
        LOGGER.log(" Error initializing D3D.", io::ELL_ERROR);
        return false;
    }
	
    D3DDISPLAYMODE d3ddm;
    hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
    if (FAILED(hr))
    {
        LOGGER.log(" Error: Could not get Adapter Display mode.", io::ELL_ERROR);
        return false;
    }
	
    ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));	

	m_PresentParams.BackBufferCount             = 1;	
    m_PresentParams.EnableAutoDepthStencil      = TRUE;	
	//m_PresentParams.hDeviceWindow               = hwnd;    
	m_PresentParams.BackBufferWidth  = m_ScreenSize.Width;
    m_PresentParams.BackBufferHeight = m_ScreenSize.Height;   

	s32 bits = out_video_data.Bits;

	if (bits != 32 && bits !=16)
	{
		LOGGER.logErr("Unsupported color bits (%d)!", bits);
		return false;
	}

	D3DFORMAT adapterFormat = D3DFMT_X1R5G5B5;

	if (m_Fullscreen)
	{
		if (bits==32)
			adapterFormat = D3DFMT_X8R8G8B8;
		m_PresentParams.SwapEffect	= D3DSWAPEFFECT_FLIP;
		m_PresentParams.Windowed	= FALSE;
		m_PresentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		adapterFormat		= d3ddm.Format; // same as desktop default
		m_PresentParams.SwapEffect	= D3DSWAPEFFECT_COPY;
		m_PresentParams.Windowed	= TRUE;
	}

	m_PresentParams.BackBufferFormat = (bits == 32) ?
		D3DFMT_A8R8G8B8 : D3DFMT_A1R5G5B5;

	m_PresentParams.PresentationInterval = m_VerticalSync ?
		D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    // enable anti alias if possible and whished
    if (m_Antialiasing)
    {
        DWORD qualityLevels = 0; 

        if (!FAILED(pID3D->CheckDeviceMultiSampleType(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL , adapterFormat, !m_Fullscreen, 
				D3DMULTISAMPLE_2_SAMPLES, &qualityLevels))
			)
        {        
            // enable multi sampling
            m_PresentParams.SwapEffect         = D3DSWAPEFFECT_DISCARD; 
            m_PresentParams.MultiSampleType    = D3DMULTISAMPLE_2_SAMPLES; 
			m_PresentParams.MultiSampleQuality = qualityLevels-1;
        } 
        else 
        { 
            LOGGER.logWarn("Antialiasing disabled because hardware/driver lacks necessary caps.");
            m_Antialiasing = false;
        }
    }

	// check stencil buffer compatibility

	m_StencilBuffer = false;

	if (SUCCEEDED(pID3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			adapterFormat, D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_SURFACE, D3DFMT_D24S8)))
	// Try to 32-bit depth, with 8-bit stencil
	{
		// Woohoo!
        if (SUCCEEDED(pID3D->CheckDepthStencilMatch( 
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				adapterFormat, m_PresentParams.BackBufferFormat, 
				D3DFMT_D24S8)))
		{
			m_PresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
			m_StencilBuffer = true;
		}
        else 
			m_PresentParams.AutoDepthStencilFormat = D3DFMT_D24X8;
	}
	else
	{
		// Bugger, no 8-bit hardware stencil, just try 32-bit zbuffer 
		if (SUCCEEDED(pID3D->CheckDeviceFormat(
				D3DADAPTER_DEFAULT,	D3DDEVTYPE_HAL,  
				adapterFormat,  D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_SURFACE, D3DFMT_D32)))
			m_PresentParams.AutoDepthStencilFormat = D3DFMT_D32;
		else
			// Jeez, what a naff card. Fall back on 16-bit depth buffering
			m_PresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	}

	if (!m_StencilBuffer)
	{
		LOGGER.logWarn(
			" Device does not support stencilbuffer, disabling stencil buffer.");
	}	

	m_StencilBuffer &= m_Shadows;

    // create device

	HWND hwnd = reinterpret_cast<HWND>(out_video_data.Win32.HWnd);

    hr = pID3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PresentParams, &pID3DDevice);

    if (FAILED(hr))
    {
		LOGGER.logWarn(" Was not able to create hardware accelerated Direct3D device.");

		hr = pID3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
            D3DCREATE_MIXED_VERTEXPROCESSING, &m_PresentParams, &pID3DDevice);
        if (FAILED(hr))
        {
			hr = pID3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PresentParams, &pID3DDevice);
			if (FAILED(hr))
			{
				LOGGER.logErr(" Was not able to create Direct3D device.");
				return false;
			}
        }
    }

	LOGGER.log(" D3D device created");            

	// print device information
    D3DADAPTER_IDENTIFIER9 dai;
    if (!FAILED(pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
    {
        char tmp[512];

        s32 Product = HIWORD(dai.DriverVersion.HighPart);
        s32 Version = LOWORD(dai.DriverVersion.HighPart);
        s32 SubVersion = HIWORD(dai.DriverVersion.LowPart);
        s32 Build = LOWORD(dai.DriverVersion.LowPart);

        sprintf(tmp, " Video adapter model: %s %s", dai.Description, dai.Driver);
        LOGGER.log(tmp, io::ELL_INFORMATION);     

		sprintf(tmp, " D3D driver version: %d.%d.%d.%d", Product,Version, SubVersion, Build);
		LOGGER.log(tmp, io::ELL_INFORMATION);  		
    }

    // get caps
    pID3DDevice->GetDeviceCaps(&Caps);

    if (m_StencilBuffer && (
			!(Caps.StencilCaps & D3DSTENCILCAPS_DECR) ||
			!(Caps.StencilCaps & D3DSTENCILCAPS_INCR) ||
			!(Caps.StencilCaps & D3DSTENCILCAPS_KEEP)
			)
		)
    {
        LOGGER.log(" Device not able to use stencil buffer, disabling stencil buffer.", io::ELL_WARNING);
        m_StencilBuffer = false;
    }
	
	if( ( Caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED ) != 0 )
	{
		m_TwoSidedStencil = true;
	}
	else
	{
		m_TwoSidedStencil = false;
	}

	// initializes occlusion querry

	m_D3D9HardwareOcclusionQuery = new CD3D9HardwareOcclusionQuery(pID3DDevice);

	if (!m_D3D9HardwareOcclusionQuery->isOK())
	{
		SAFE_DROP(m_D3D9HardwareOcclusionQuery);
	}
	
	setGlobalAmbientColor(getGlobalAmbientColor());

	// D3D driver constants

	MaxAnisotropyLevel = Caps.MaxAnisotropy;		

	// MaxActiveLights of 0 or -1 are indicative of software processing.
	// Software processing having no limitations can support an infinite number of lights. 
	s32 maxlights = Caps.MaxActiveLights;
	CHECK_MIN_RANGE(maxlights, 8);
	m_MaxLights = maxlights;

	s32 max_tex_units = m_MaxTextureUnits = Caps.MaxSimultaneousTextures;
	CHECK_MAX_RANGE(m_MaxTextureUnits, MY_MATERIAL_MAX_LAYERS);

	m_MaxTextureSize = core::dimension2di(Caps.MaxTextureWidth, Caps.MaxTextureHeight);

    // set fog mode
    setFog(Fog);

    // set exposed data

    out_video_data.Win32.D3D9.D3D       = pID3D;
    out_video_data.Win32.D3D9.D3DDevice = pID3DDevice;
	out_video_data.DriverType = EDT_DIRECTX9;

    // Create Our Empty Fog Texture
    StencilFogTexture = createEmptyD3D9Texture(
        getStencilFogTextureSize(), getStencilFogTextureSize());  

	LOGGER.logInfo(" Direct3D driver features:");

	LOGGER.logInfo("  Multitexturing   : %s",
		queryFeature(EVDF_MULITEXTURE) ?
		"OK" : "None");
	LOGGER.logInfo("  Vertex buffer    : OK"); 
	LOGGER.logInfo("  Anisotropic filt.: %s (%d level)",
		queryFeature(EVDF_ANISOTROPIC_FILTER) ?
			"OK" : "None", MaxAnisotropyLevel);
	LOGGER.logInfo("  Swap control     : OK");
	if (queryFeature(EVDF_SHADER_LANGUAGE))
	{
		LOGGER.logInfo("  HLSL             : OK");
		LOGGER.logInfo("   Vertex shader v.%d.%d",
			0xff & Caps.VertexShaderVersion >> 8,
			0xff & Caps.VertexShaderVersion);
		LOGGER.logInfo("   Pixel shader v.%d.%d",
			0xff & Caps.PixelShaderVersion >> 8,
			0xff & Caps.PixelShaderVersion);
	}
	else
		LOGGER.logInfo("  HLSL             : None");

	LOGGER.logInfo("  Two side stencil : %s",
		m_TwoSidedStencil ? "OK" : "None");
	LOGGER.logInfo("  Occlusion Query  : %s",
		queryFeature(EVDF_OCCLUSION_QUERY) ? "OK" : "None");
	LOGGER.logInfo("  Render Target    : %s",
		queryFeature(EVDF_RENDER_TO_TARGET) ? "OK" : "None");
	LOGGER.logInfo("  Compressed tex.  : %s",
		queryFeature(EVDF_COMPRESSED_TEXTURES) ? "OK" : "None");

	//antialiasing ON! 
	if (m_Antialiasing)
		pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS , true); 

	 // log Video driver parameters

	s32 depthbit =24;
	if (m_PresentParams.AutoDepthStencilFormat == D3DFMT_D16) 
		depthbit = 16;
	else if (m_PresentParams.AutoDepthStencilFormat == D3DFMT_D32)
		depthbit = 32;

    LOGGER.logInfo(" Video driver parameters: ");
	LOGGER.logInfo("  color   : %d bit",
		(m_PresentParams.BackBufferFormat == D3DFMT_A8R8G8B8) ? 32 : 16);
    LOGGER.logInfo("  alpha   : %d bit",
		(m_PresentParams.BackBufferFormat == D3DFMT_A8R8G8B8) ? 8 : 1);
	LOGGER.logInfo("  depth(z): %d bit", depthbit);
    LOGGER.logInfo("  stencil : %d bit",
		(m_PresentParams.AutoDepthStencilFormat == D3DFMT_D24S8) ? 8 : 0);
    LOGGER.logInfo("  max lights    : %d", m_MaxLights);
    LOGGER.logInfo("  max tex. units: %d", max_tex_units);
    LOGGER.logInfo("  max tex. res. : %d x %d",
		m_MaxTextureSize.Width, m_MaxTextureSize.Height);

	// vertical syncronization
	if (m_Fullscreen && m_VerticalSync)
		LOGGER.logInfo(" Vertical Sync enabled");
	else if (m_Fullscreen && !m_VerticalSync)
		LOGGER.logInfo(" Vertical Sync disabled");
	else
		LOGGER.logInfo(" Can't on/off Vertical Sync ");

	// specular highlights
	pID3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	pID3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	
	// create event query
	if (FAILED(pID3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pEventQuery)))
	{
		LOGGER.logWarn("Can't allocate a Hardware event query. "
			"This video card doesn't supports it, sorry.");
	}

	// setting texture filter
	setTextureFilter(m_TextureFilter);

	CNullDriver::setColorMask(m_ColorMask);

	for (u32 i = 0; i < E_VERTEX_TYPE_COUNT; i++)
		pID3DDevice->CreateVertexDeclaration(
			D3D9VertexElements[i], &m_VertexDecls[i]);

	// so far so good.
	return CNullDriver::_initDriver(out_video_data);
}

//----------------------------------------------------------------------------

bool CD3D9Driver::_beginRendering()
{
    HRESULT hr;

    if (DeviceLost)
    {
        if(FAILED(hr = pID3DDevice->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
				return false;
            if (hr == D3DERR_DEVICENOTRESET)
                reset();
 			return false;
        }
    }

	if (!StencilPreInitialized && m_StencilBuffer)
	{
		// to pre-initialize stencil
		pID3DDevice->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
		pID3DDevice->Clear( 0L, NULL, D3DCLEAR_STENCIL , 0x00000002, 0L, 0 );			
		pID3DDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
		StencilPreInitialized = true;
	}
	setPolygonFillMode(m_PolygonFillMode);
  
	// begin scene
    hr = pID3DDevice->BeginScene();
    if (FAILED(hr))
    {
        LOGGER.log("DirectX9 begin scene failed.", io::ELL_WARNING);
		return false;
    }
	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Driver::_endRendering()
{
	if (DeviceLost)
		return false;

    HRESULT hr = pID3DDevice->EndScene();

    if (FAILED(hr))
    {
        LOGGER.log("DirectX9 end scene failed.", io::ELL_WARNING);
		return false;
    }
	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Driver::_swapBuffers()
{
	if (DeviceLost)
		return false;

	HRESULT hr = 0;

	if (pEventQuery)
	{
		// The DirectX API allows graphics drivers to buffer up to three frames 
		// in the command queue of the GPU. Such a large buffer enables CPU and GPU 
		// to work in parallel even as workload on CPU and GPU varies. 
		// If there was no buffer then the GPU would become idle as soon as the CPU 
		// reduced its graphics command output (for example, because it was solving physics equations) 
		// and conversely the CPU would become idle whenever it wanted to send another graphics command 
		// and the GPU was still busy rendering a previously submitted graphics command.

		// On the other hand, allowing the driver to buffer three frames worth of data also means 
		// that lag (the time between a user giving input and seeing its effect on-screen) increases 
		// by up to three frames.

		// Then just before calling Present(), insert the event into the command buffer: 

		pEventQuery->Issue(D3DISSUE_END);

		// If we wanted to limit the number of frames buffered to at most one, 
		// we need to check that the query has been processed at the end of the next frame. 
		// If it has not then we spin until it has been processed.

		bool data;

		while (pEventQuery->GetData(&data, sizeof(data), D3DGETDATA_FLUSH) == S_FALSE);
	}

	hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);	

    if (hr == D3DERR_DEVICELOST)
    {
        DeviceLost = true;
        LOGGER.log("DirectX9 device lost.", io::ELL_WARNING);
    }
    else if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
    {
        LOGGER.log("DirectX9 m_PresentParams failed.", io::ELL_WARNING);
        return false;
    }

	return true;
}

//----------------------------------------------------------------------------

//! resets the device
bool CD3D9Driver::reset()
{
    // reset
    LOGGER.log("Resetting D3D9 device.", io::ELL_WARNING);

	HRESULT hr = pID3DDevice->Reset(&m_PresentParams);

	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
		{
			DeviceLost = true;
			LOGGER.log("Resetting failed due to device lost.", io::ELL_WARNING);
		}
		else
			LOGGER.log("Resetting failed.", io::ELL_WARNING);

		return false;
	}

    DeviceLost = false;

	for (u32 i=0; i<MY_MATERIAL_MAX_LAYERS; ++i)
		SAFE_DROP(CurrentTexture[i]);

    setFog(Fog);

    return true;
}

//----------------------------------------------------------------------------

//! queries the features of the driver, returns true if feature is available
bool CD3D9Driver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
    switch (feature)
    {
	case EVDF_MULITEXTURE:
		return getMaximalTextureUnitsAmount()>1;
    case EVDF_BILINEAR_FILTER:
        return true;
    case EVDF_TRILINEAR_FILTER:
        return true;
    case EVDF_ANISOTROPIC_FILTER:
        return MaxAnisotropyLevel>0;
    case EVDF_RENDER_TO_TARGET:
        return Caps.NumSimultaneousRTs > 0;
    case EVDF_MIP_MAP:
        return (Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
    case EVDF_STENCIL_BUFFER:
        return m_StencilBuffer;
    case EVDF_SHADER_LANGUAGE:
        return Caps.VertexShaderVersion >= D3DVS_VERSION(1,1) &&
			Caps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_OCCLUSION_QUERY:
		return m_D3D9HardwareOcclusionQuery!=NULL;
	case EVDF_COMPRESSED_TEXTURES:
		return true;
    };

    return false;
}

//----------------------------------------------------------------------------

//! sets transformation
void CD3D9Driver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	CNullDriver::setTransform(state, mat);

	if (!m_UseFFP)
		return;

	switch(state)
	{
	case ETS_VIEW:
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&mat));
		break;
	case ETS_MODEL:
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&mat));
		break;
	case ETS_PROJ:
		pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)((void*)&mat));
		break;
	}
}

//----------------------------------------------------------------------------

bool CD3D9Driver::_bindGPUProgram(IGPUProgram *gpu_prog)
{
	if (m_UseShaders)
	{
		IDirect3DVertexShader9 *vsh = gpu_prog ?
			((CD3D9GPUProgram*)gpu_prog)->_getVertexShaderHandle() : NULL;
		IDirect3DPixelShader9 *psh = gpu_prog ?
			((CD3D9GPUProgram*)gpu_prog)->_getPixelShaderHandle() : NULL;

		if (FAILED(pID3DDevice->SetVertexShader(vsh)))
			LOGGER.logWarn("Could not set vertex shader.");
		if (FAILED(pID3DDevice->SetPixelShader(psh)))
			LOGGER.logWarn("Could not set pixel shader.");
	}
	return CNullDriver::_bindGPUProgram(gpu_prog);
}

//----------------------------------------------------------------------------

void CD3D9Driver::_setTexture(s32 stage, ITexture *texture)
{
	static u32 max_tex_units = getMaximalTextureUnitsAmount();

    if (stage<0 || stage>(s32)max_tex_units)
		return;

	CNullDriver::_setTexture(stage, texture);

	if (CurrentTexture[stage] == texture && 
		!(texture && texture->getType() == vid::ETT_ANIMATED))
        return;  

    SAFE_DROP(CurrentTexture[stage]);

    CurrentTexture[stage] = texture;    

    if (!texture)
	{
        pID3DDevice->SetTexture(stage, 0);
	}
    else
    {
		pID3DDevice->SetTexture( stage, (IDirect3DTexture9*)texture->getHardwareTexture() );
        texture->grab();
    }

	if (!m_ResetRenderStates && m_CurrentRenderPass.Layers[stage].getTexture() != texture)
		m_ResetRenderStates = true;
}

//----------------------------------------------------------------------------

ITexture* CD3D9Driver::_createDeviceDependentTexture(img::IImage* surface)
{
    return new CD3D9Texture(surface, TextureCreationFlags);
}

//----------------------------------------------------------------------------

ITexture* CD3D9Driver::_createDeviceDependentTexture(core::dimension2di &size, img::E_COLOR_FORMAT format)
{
    return new CD3D9Texture(size, format, TextureCreationFlags);
}

//----------------------------------------------------------------------------

//! Enables or disables a texture creation flag.
void CD3D9Driver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
    if (flag == ETCF_AUTOGEN_MIP_MAPS && !queryFeature(EVDF_MIP_MAP))
        enabled = false;

    CNullDriver::setTextureCreationFlag(flag, enabled);
}

//----------------------------------------------------------------------------

//! sets a render target
bool CD3D9Driver::setRenderTarget(
	ITexture* texture,		
	bool clearBackBuffer, bool clearZBuffer, 
	img::SColor color
	)
{
    // check for valid render target

    CD3D9Texture* tex = (CD3D9Texture*)texture;

    if (texture && !tex->isRenderTarget())
    {
        LOGGER.log("Tried to set a non render target texture as render target.",
			io::ELL_ERROR);
        return false;
    }

    if (texture && (tex->getSize().Width > m_ScreenSize.Width || 
        tex->getSize().Height > m_ScreenSize.Height ))
    {
        LOGGER.log("Tried to set a render target texture which is bigger than the screen.",
			io::ELL_ERROR);
        return false;
    }

    // check if we should set the previous RT back

    bool ret = true;

    if (tex == 0)
    {
        if (PrevRenderTarget)
        {
            if (FAILED(pID3DDevice->SetRenderTarget(0, PrevRenderTarget)))
            {
                LOGGER.log("Could not set back to previous render target.", io::ELL_ERROR);
                ret = false;
            }

            CurrentRendertargetSize = core::dimension2d<s32>(0,0);
            PrevRenderTarget->Release();
            PrevRenderTarget = 0;
        }
    }
    else
    {
        // we want to set a new target. so do this.

        // store previous target

        if (!PrevRenderTarget)
            if (FAILED(pID3DDevice->GetRenderTarget(0, &PrevRenderTarget)))
            {
                LOGGER.log("Could not get previous render target.", io::ELL_ERROR);
                return false;
            }

        // set new render target

        if (FAILED(pID3DDevice->SetRenderTarget(0, tex->getRenderTargetSurface())))
        {
            LOGGER.log("Could not set render target.", io::ELL_ERROR);
            return false;
        }

        CurrentRendertargetSize = tex->getSize();
    }

    if (clearBackBuffer || clearZBuffer)
    {
        DWORD flags = 0;

        if (clearBackBuffer)
            flags |= D3DCLEAR_TARGET;

        if (clearZBuffer)
            flags |= D3DCLEAR_ZBUFFER;

        pID3DDevice->Clear(0, NULL, flags, color.color, 1.0f, 0);
    }

    return ret;
}

//----------------------------------------------------------------------------

//! sets a viewport
void CD3D9Driver::setViewPort(const core::rect<s32>& area)
{
    core::rect<s32> vp = area;
    core::rect<s32> rendert(0,0, m_ScreenSize.Width, m_ScreenSize.Height);
    vp.clipAgainst(rendert);

    D3DVIEWPORT9 viewPort;
    viewPort.X = vp.UpperLeftCorner.X;
    viewPort.Y = vp.UpperLeftCorner.Y;
    viewPort.Width = vp.getWidth();
    viewPort.Height = vp.getHeight();
    viewPort.MinZ = 0.0f;
    viewPort.MaxZ = 1.0f;

    HRESULT hr = D3DERR_INVALIDCALL;
    if (vp.getHeight()>0 && vp.getWidth()>0)
        hr = pID3DDevice->SetViewport(&viewPort);

    if (FAILED(hr))
        LOGGER.logWarn("Failed setting the viewport.");

    ViewPort = vp;
}

//--------------------------------------------------------------------------

void CD3D9Driver::_setVertexType(E_VERTEX_TYPE newType)
{
    if (newType != m_CurrentVertexType)
    {
		HRESULT hr;
#if 0
		IDirect3DVertexDeclaration9 *d3dvdecl =  m_VertexDecls[newType]
		hr = pID3DDevice->SetVertexDeclaration(d3dvdecl);
#else
		DWORD d3dvtype = convertToD3D9VertexType(newType);
		hr = pID3DDevice->SetFVF(d3dvtype);
#endif
		if (FAILED(hr))
            LOGGER.logErr("Could not set Vertex Type %s",
				vid::getVertexTypeName(newType));
    }
	CNullDriver::_setVertexType(newType);
}

//--------------------------------------------------------------------------

//! Can be called by an IMaterialRenderer to make its work easier.
void CD3D9Driver::_setBasicRenderStates()
{
	u16 flags = m_CurrentRenderPass.getFlags();

	if (m_ResetRenderStates || m_LastRenderPass.getFlags() != flags)
	{
		// zwrite
        if (m_CurrentRenderPass.getFlag(EMF_ZWRITE_ENABLE))
            pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        else
            pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
 
		// back face culling and face culling order
        if (m_CurrentRenderPass.getFlag(EMF_BACK_FACE_CULLING))
		{
            if (m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW))
				pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			else
				pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
        else
            pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// blending
		if (!m_CurrentRenderPass.getFlag(EMF_BLENDING))
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
		else
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		// shademode
        if (m_CurrentRenderPass.getFlag(EMF_GOURAUD_SHADING))
            pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        else
            pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}
	
	// blending functions

	E_SRC_BLEND_FUNCTION srcBlend = m_CurrentRenderPass.getSrcBlendFunc();
	E_DST_BLEND_FUNCTION dstBlend = m_CurrentRenderPass.getDstBlendFunc();

	if (m_ResetRenderStates ||
		srcBlend != m_LastRenderPass.getSrcBlendFunc() ||
		dstBlend != m_LastRenderPass.getDstBlendFunc())
	{	
		pID3DDevice->SetRenderState(D3DRS_SRCBLEND,
			convertToD3D9SrcBlendFunction(srcBlend));
		pID3DDevice->SetRenderState(D3DRS_DESTBLEND,
			convertToD3D9DstBlendFunction(dstBlend));
	}

	E_BLEND_EQUATION blendEq = m_CurrentRenderPass.getBlendEquation();

	if (m_ResetRenderStates ||
		blendEq != m_LastRenderPass.getBlendEquation())
	{	
		pID3DDevice->SetRenderState(D3DRS_BLENDOP,
			convertToD3D9BlendEquation(blendEq));
	}

	// Textures filtering

	bool mipsf = !m_CurrentRenderPass.getFlag(EMF_MIP_MAP_OFF);

	for (s32 l = m_DirtyTexUnit; l >= 0; --l)
	{
		ITexture *tex = m_CurrentRenderPass.Layers[l].getTexture();
		if (!tex)
			continue;

		bool mips = tex->hasMipMaps() && mipsf;

		E_TEXTURE_FILTER tf = m_TextureFilter;
		if (tf!=ETF_NONE && m_CurrentRenderPass.Layers[l].getType() == vid::ETLT_LIGHT_MAP)
			tf = ETF_BILINEAR;

		if (tf==ETF_ANISOTROPIC)
		// Anisotropic filter
		{			
			pID3DDevice->SetSamplerState (l, D3DSAMP_MAGFILTER,  D3DTEXF_ANISOTROPIC);
			pID3DDevice->SetSamplerState (l, D3DSAMP_MINFILTER,  D3DTEXF_ANISOTROPIC);				
			pID3DDevice->SetSamplerState (l, D3DSAMP_MIPFILTER,  mips?D3DTEXF_LINEAR:D3DTEXF_NONE);
			pID3DDevice->SetSamplerState (l, D3DSAMP_MAXANISOTROPY, core::math::Min(8, MaxAnisotropyLevel));			
		}
		else 
		if (tf==ETF_TRILINEAR)
		// Trilinear filter
		{
			pID3DDevice->SetSamplerState (l, D3DSAMP_MAGFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetSamplerState (l, D3DSAMP_MINFILTER,  D3DTEXF_LINEAR);				
			pID3DDevice->SetSamplerState (l, D3DSAMP_MIPFILTER,  mips?D3DTEXF_LINEAR:D3DTEXF_NONE);
		}
		else 
		if (tf==ETF_BILINEAR)
		// Bilinear filter
		{
			pID3DDevice->SetSamplerState (l, D3DSAMP_MAGFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetSamplerState (l, D3DSAMP_MINFILTER,  D3DTEXF_LINEAR);				
			pID3DDevice->SetSamplerState (l, D3DSAMP_MIPFILTER,  mips?D3DTEXF_POINT:D3DTEXF_NONE);
		}
		else
		// No filter
		{
			pID3DDevice->SetSamplerState (l, D3DSAMP_MINFILTER,  D3DTEXF_POINT);			
			pID3DDevice->SetSamplerState (l, D3DSAMP_MAGFILTER,  D3DTEXF_POINT);
			pID3DDevice->SetSamplerState (l, D3DSAMP_MIPFILTER,  D3DTEXF_NONE);
		}
	}

	// alpha test

	E_ALPHA_TEST atest = m_CurrentRenderPass.getAlphaTest ();

	if (m_ResetRenderStates || m_LastRenderPass.getAlphaTest () != atest)
    {
		if (atest == vid::EAT_ALWAYS)
		{
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 	
		}
		else
		if (atest == vid::EAT_GE_80)
		{
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x80); 			
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);			
		}		
		else
		if (atest == vid::EAT_LT_80)
		{
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x80); 			
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);			
		}
		else
		if ( atest == vid::EAT_GT_0 )
		{
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x01); 			
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);	
		}
	}

	// depth buffer compare function

	E_COMPARE_TEST ztest = m_CurrentRenderPass.getDepthTest();

	if (m_ResetRenderStates || m_LastRenderPass.getDepthTest() != ztest)
    {
		if (ztest == ECT_ALWAYS)
			pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        else
		{
			DWORD dxfunc = convertToD3D9CompareFunction(ztest);
			pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_ZFUNC, dxfunc);
		}
    }
}

//----------------------------------------------------------------------------

void CD3D9Driver::_setFFPRenderStates()
{
	static const SRenderPass &m		= getRenderPass();
	static const SRenderPass &lastm	= getLastRenderPass();
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	bool reset = m_ResetRenderStates;

	for (u32 u = 0; u < max_units_cnt; u++)
	{
		if (!reset && lastm.Layers[u] == m.Layers[u])
			continue;

		vid::ITexture *texture = m.Layers[u].getType() != vid::ETLT_NORMAL_MAP ?
			m.Layers[u].getTexture() : NULL;

		_setTexture(u, texture);	

		if (texture)
		{
			// setting texture matrix
			
			static D3DXMATRIX d3dMat;

			if ( m.Layers[u].getTexCoordGen() == vid::ETCGT_PROJECTED_MAPPING )
			{
				// Derive camera space to projector space transform
				// To do this, we need to undo the camera view matrix, then 
				// apply the projector view & projection matrices

				core::matrix4 view_inv = getTransform(vid::ETS_VIEW);
				view_inv.makeInversed();
				core::matrix4 tex_matr = m.Layers[u].getTexMatrixCurrent() * view_inv;

				d3dMat = *((D3DMATRIX*)tex_matr.pointer());

				// We want texcoords (u, v, w, q) always get divided by q, but D3D
				// projected texcoords is divided by the last element (in the case of
				// 2D texcoord, is w). So we tweak the transform matrix, transform the
				// texcoords with w and q swapped: (u, v, q, w), and then D3D will
				// divide u, v by q. The w and q just ignored as it wasn't used by
				// rasterizer.

				core::math::Swap(d3dMat._13, d3dMat._14);
				core::math::Swap(d3dMat._23, d3dMat._24);
				core::math::Swap(d3dMat._33, d3dMat._34);
				core::math::Swap(d3dMat._43, d3dMat._44);
			}
			else
			{
				d3dMat = *((D3DMATRIX*)m.Layers[u].getTexMatrixCurrent().toD3DTextureMatrix());
			}

			pID3DDevice->SetTransform(
				(D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + u), &d3dMat);
			
			// setting texture addressing mode

			D3DTEXTUREADDRESS addr_mode = D3DTADDRESS_WRAP;

			switch ( m.Layers[u].getTexCoordAddr() )
			{
			case vid::ETCAM_MIRROR:
				addr_mode = D3DTADDRESS_MIRROR;					
				break;
			case vid::ETCAM_CLAMP:
				addr_mode = D3DTADDRESS_CLAMP;
				break;
			case vid::ETCAM_BORDER:
				addr_mode = D3DTADDRESS_BORDER;
				break;
			}

			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSU, addr_mode);
			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSV, addr_mode);
			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSW, addr_mode);			
			
			// setting texture mapping settings

			DWORD tc_transform, tc_index;

			switch ( m.Layers[u].getTexCoordGen() )
			{
			case vid::ETCGT_ENVIRONMENT_MAPPING:
				{
					tc_transform = D3DTTFF_COUNT2;
					tc_index     = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
				}
				break;
			case vid::ETCGT_PROJECTED_MAPPING:
				{
					tc_transform = D3DTTFF_PROJECTED | D3DTTFF_COUNT3;
					tc_index     = D3DTSS_TCI_CAMERASPACEPOSITION;
				}
				break;
			default:
				{		
					tc_transform   = D3DTTFF_COUNT2;
					tc_index       = ( m.Layers[u].getTexCoordChannel() == 1 ) ? 1 : 0;
				}
			}
	
			pID3DDevice->SetTextureStageState( u, D3DTSS_TEXTURETRANSFORMFLAGS, tc_transform );
			pID3DDevice->SetTextureStageState( u, D3DTSS_TEXCOORDINDEX,         tc_index     );

			// setting layers blending states

			switch ( m.Layers[u].getTexRGBScale () )
			{
			case 3:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
				break;
			case 2:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
				break;
			default:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE);	
			}

			pID3DDevice->SetTextureStageState( u, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
			pID3DDevice->SetTextureStageState( u, D3DTSS_COLORARG2, D3DTA_CURRENT ); 

			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ); 
			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );  
			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		}
	}

	bool alpha_color_gen = m_CurrentRenderPass.getColorGen() != vid::ECGT_VERTEX ||
		m_CurrentRenderPass.getAlphaGen() != vid::EAGT_VERTEX;
	bool lighting = m_CurrentRenderPass.isLighting() || alpha_color_gen;

	bool alpha_color_gen_last = m_LastRenderPass.getColorGen() != vid::ECGT_VERTEX ||
		m_LastRenderPass.getAlphaGen() != vid::EAGT_VERTEX;
	bool lighting_last = m_LastRenderPass.isLighting() || alpha_color_gen_last;

	if (m_ResetRenderStates ||
		m_CurrentRenderPass.getAmbientColor() != m_LastRenderPass.getAmbientColor() ||
		m_CurrentRenderPass.getDiffuseColorCurrent() != m_LastRenderPass.getDiffuseColorCurrent() ||
		m_CurrentRenderPass.getEmissiveColorCurrent() != m_LastRenderPass.getEmissiveColorCurrent() ||
		m_CurrentRenderPass.getSpecularColor() != m_LastRenderPass.getSpecularColor() ||
		m_CurrentRenderPass.getShininess() != m_LastRenderPass.getShininess() ||
		(lighting && lighting != lighting_last))
    {
        static D3DMATERIAL9 mat;
		img::SColor diff_col = m_CurrentRenderPass.getDiffuseColorCurrent();
		img::SColor amb_col = m_CurrentRenderPass.getAmbientColor();
		img::SColor spec_col = m_CurrentRenderPass.getSpecularColor();

		if (alpha_color_gen)
		{
			diff_col.set(diff_col.getAlpha(), 0, 0, 0);
			amb_col.set(0x00000000);
			spec_col.set(0x00000000);
		}

		mat.Diffuse  = colorToD3D(diff_col);
        mat.Ambient  = colorToD3D(amb_col);
		mat.Emissive = colorToD3D(m_CurrentRenderPass.getEmissiveColorCurrent());
        mat.Specular = colorToD3D(spec_col);
        mat.Power = m_CurrentRenderPass.getShininess();
        pID3DDevice->SetMaterial(&mat);
    }

	// lighting
	if (m_ResetRenderStates || lighting != lighting_last)
		pID3DDevice->SetRenderState(D3DRS_LIGHTING, lighting ? TRUE : FALSE);

	// fog
	bool fog = m_CurrentRenderPass.getFlag(EMF_FOG_ENABLE);
	bool fog_last = m_LastRenderPass.getFlag(EMF_FOG_ENABLE);
	if (m_ResetRenderStates || fog != fog_last)
		pID3DDevice->SetRenderState(D3DRS_FOGENABLE, fog);

	_setBasicRenderStates();
}

//----------------------------------------------------------------------------

void CD3D9Driver::_setShaderRenderStates()
{
	static const SRenderPass &m		= getRenderPass();
	static const SRenderPass &lastm	= getLastRenderPass();
	static u32 max_units_cnt = getMaximalTextureUnitsAmount();
	bool reset = m_ResetRenderStates;

	for (u32 u=0; u < max_units_cnt; u++)
	{
		if (!reset && lastm.Layers[u] == m.Layers[u])
			continue;

		vid::ITexture *texture = m.Layers[u].getTexture();

		_setTexture(u, texture);	

		if (texture)
		{
			// setting texture matrix
			
			static D3DXMATRIX d3dMat;

			if ( m.Layers[u].getTexCoordGen() == vid::ETCGT_PROJECTED_MAPPING )
			{
				// Derive camera space to projector space transform
				// To do this, we need to undo the camera view matrix, then 
				// apply the projector view & projection matrices

				core::matrix4 view_inv = getTransform(vid::ETS_VIEW);
				view_inv.makeInversed();

				static const core::matrix4 CLIPSPACE2DTOIMAGESPACE(
					0.5,  0,   0, 0, 
					0,   -0.5, 0, 0, 
					0,    0,   1, 0,
					0.5,  0.5, 0, 1
					);

				core::matrix4 tex_matr = 
					CLIPSPACE2DTOIMAGESPACE * m.Layers[u].getTexMatrix() * view_inv;

				d3dMat = *((D3DMATRIX*)tex_matr.pointer());

				// We want texcoords (u, v, w, q) always get divided by q, but D3D
				// projected texcoords is divided by the last element (in the case of
				// 2D texcoord, is w). So we tweak the transform matrix, transform the
				// texcoords with w and q swapped: (u, v, q, w), and then D3D will
				// divide u, v by q. The w and q just ignored as it wasn't used by
				// rasterizer.

				core::math::Swap(d3dMat._13, d3dMat._14);
				core::math::Swap(d3dMat._23, d3dMat._24);
				core::math::Swap(d3dMat._33, d3dMat._34);
				core::math::Swap(d3dMat._43, d3dMat._44);
			}
			else
			{
				d3dMat = *((D3DMATRIX*)m.Layers[u].getTexMatrix().toD3DTextureMatrix());
			}

			pID3DDevice->SetTransform(
				(D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + u), &d3dMat);
			
			// setting texture addressing mode

			D3DTEXTUREADDRESS addr_mode = D3DTADDRESS_WRAP;

			switch ( m.Layers[u].getTexCoordAddr() )
			{
			case vid::ETCAM_MIRROR:
				addr_mode = D3DTADDRESS_MIRROR;					
				break;
			case vid::ETCAM_CLAMP:
				addr_mode = D3DTADDRESS_CLAMP;
				break;
			case vid::ETCAM_BORDER:
				addr_mode = D3DTADDRESS_BORDER;
				break;
			}

			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSU, addr_mode);
			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSV, addr_mode);
			pID3DDevice->SetSamplerState(u, D3DSAMP_ADDRESSW, addr_mode);			
			
			// setting texture mapping settings

			DWORD tc_transform, tc_index;

			switch ( m.Layers[u].getTexCoordGen() )
			{
			case vid::ETCGT_ENVIRONMENT_MAPPING:
				{
					tc_transform = D3DTTFF_COUNT2;
					tc_index     = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
				}
				break;
			case vid::ETCGT_PROJECTED_MAPPING:
				{
					tc_transform = D3DTTFF_PROJECTED | D3DTTFF_COUNT3;
					tc_index     = D3DTSS_TCI_CAMERASPACEPOSITION;
				}
				break;
			default:
				{		
					tc_transform   = D3DTTFF_COUNT2;
					tc_index       = ( m.Layers[u].getTexCoordChannel() == 1 ) ? 1 : 0;
				}
			}
	
			pID3DDevice->SetTextureStageState( u, D3DTSS_TEXTURETRANSFORMFLAGS, tc_transform );
			pID3DDevice->SetTextureStageState( u, D3DTSS_TEXCOORDINDEX,         tc_index     );

			// setting layers blending states

			switch ( m.Layers[u].getTexRGBScale () )
			{
			case 3:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
				break;
			case 2:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
				break;
			default:
				pID3DDevice->SetTextureStageState(u, D3DTSS_COLOROP, D3DTOP_MODULATE);	
			}

			pID3DDevice->SetTextureStageState( u, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
			pID3DDevice->SetTextureStageState( u, D3DTSS_COLORARG2, D3DTA_CURRENT ); 

			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ); 
			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );  
			pID3DDevice->SetTextureStageState( u, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		}
	}
	_setBasicRenderStates();
}

//----------------------------------------------------------------------------

void CD3D9Driver::deleteAllDynamicLights()
{
    for (s32 i=0; i < m_LastEnabledLightsCount; ++i)
        pID3DDevice->LightEnable(i, false);
    CNullDriver::deleteAllDynamicLights();
}

//----------------------------------------------------------------------------

//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CD3D9Driver::setGlobalAmbientColor(const img::SColorf& color)
{
    if (!pID3DDevice)
        return;

	CNullDriver::setGlobalAmbientColor(color);

    D3DCOLOR col = color.toSColor().color;
    pID3DDevice->SetRenderState(D3DRS_AMBIENT, col);
}

//----------------------------------------------------------------------------

//! Create An Empty D3D9 Texture
IDirect3DTexture9 * CD3D9Driver::createEmptyD3D9Texture(s32 w, s32 h)
{	
    IDirect3DSurface9* pBackBuffer = 0; 

    if (FAILED(pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer)))
	{
        LOGGER.log(" Device not able to get back buffer.", io::ELL_WARNING); 

		return NULL;
	}

	IDirect3DTexture9 *tex;
    
    D3DSURFACE_DESC d3dsd;

    pBackBuffer->GetDesc(&d3dsd);

    pID3DDevice->CreateTexture(
        w, h,
        1, // number of mipmaplevels (0 = automatic all)
        D3DUSAGE_RENDERTARGET, 
        d3dsd.Format, 
        D3DPOOL_MANAGED, 
        &tex, NULL
        );

	SAFE_RELEASE(pBackBuffer); 
    
    return tex;
}

//----------------------------------------------------------------------------

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
s32 CD3D9Driver::getMaximalPrimitiveCount()
{
    return Caps.MaxPrimitiveCount;
}

//----------------------------------------------------------------------------

//! Sets the fog mode.
void CD3D9Driver::setFog(const SFog& fog)
{
    CNullDriver::setFog(fog);

    if (!pID3DDevice)
        return;

	// setting fog color
    pID3DDevice->SetRenderState(D3DRS_FOGCOLOR, fog.Color.color);   

	// setting fog quality and type
    pID3DDevice->SetRenderState(D3DRS_FOGTABLEMODE/*D3DRS_FOGVERTEXMODE*/, convertToD3D9FogMode(fog.Type)); 

	// setting fog parameters, depending on fog type
    if(fog.Type==EFT_LINEAR) 
    {
        pID3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fog.Start)); 
        pID3DDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD*)(&fog.End)); 
    }
    else
	{
        pID3DDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)(&fog.Density)); 
	}
}

//----------------------------------------------------------------------------

void CD3D9Driver::OnResize(const core::dimension2d<s32>& size)
{
	if (!pID3DDevice || m_ScreenSize == size)
		return;

	CNullDriver::OnResize(size);

	m_PresentParams.BackBufferWidth = size.Width;
	m_PresentParams.BackBufferHeight = size.Height;

	reset();
}

//---------------------------------------------------------------------------

//! set gamma correction value, directx only at the moment patch by reaper
void CD3D9Driver::setGammaRamp(f32 gamma, f32 contrast, f32 brightness)
{
   D3DGAMMARAMP ramp;
   f32 angle = contrast;
   f32 offset = (brightness - 1) * 256;

   for (s32 i = 0; i < 256; i++)
   {
      f32 k = i / 256.0f;
      k = powf(k, 1.f / gamma);
      k *= 256;

      f32 value = k * angle * 256 + offset * 256;
      if (value > 65535) value = 65535;
      if (value < 0) value = 0;

      ramp.red[i]   = (u16)value;
      ramp.green[i] = (u16)value;
      ramp.blue[i] = (u16)value;
   }

   pID3DDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &ramp);
}

//---------------------------------------------------------------------------

//! \brief Makes a screenshot and stores it into the given texture
//! \param texture, the texture we use for storing the screenshot
//! \return void
//! \note users must make sure the texture size is bigger or equal 
//! in dimensions as the current sceen dimensions
// added by zola, code by REAPER
void CD3D9Driver::makeScreenShot(ITexture* texture)
{
    /*
	// make screenshoot code sample
	IDirect3DSurface9 * pSurface = getRenderTargetSurface();;
	HRESULT hr  = Device->CreateOffscreenPlainSurface(getSize().Width, getSize().Height,
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, );
	if (SUCCEEDED(hr))
	{
		Device->GetFrontBufferData(0, pSurface);
		D3DXSaveSurfaceToFile(fname, D3DXIFF_BMP, pSurface, NULL, NULL);
	}
	else
		LOGGER.log("Could not make D3D screenshoot.", io::ELL_WARNING);
	*/
}

//----------------------------------------------------------------------------

//! Creates a render target texture.
ITexture* CD3D9Driver::createRenderTargetTexture(core::dimension2d<s32> size)
{
    return new CD3D9Texture(size);
}

//---------------------------------------------------------------------------

void CD3D9Driver::setColorMask(bool r, bool g, bool b, bool a)
{
	CNullDriver::setColorMask(r, g, b, a);
	pID3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
		(r ? D3DCOLORWRITEENABLE_RED : 0) |
		(g ? D3DCOLORWRITEENABLE_GREEN : 0) |
		(b ? D3DCOLORWRITEENABLE_BLUE : 0) |
		(a ? D3DCOLORWRITEENABLE_ALPHA : 0));
}

//----------------------------------------------------------------------------

void CD3D9Driver::clearZBuffer()
{
    HRESULT hr = pID3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, 0);

    if (FAILED(hr))
        LOGGER.log("CD3D9Driver clearZBuffer() failed.", io::ELL_ERROR);
}

//---------------------------------------------------------------------------

void CD3D9Driver::clearColorBuffer()
{
	// Emulate clear color buffer via redraw all screen rectangle.
	// This trick is needed only for DirectX, because
	// colorMask is not working on standard Clear method like bellow
	//
	// pID3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, BackColor.color, 1.0, 0);
	//

	m_TrianglesDrawn += 2;
	m_DIPsDrawn += 1;

    // store initial render states

	bool stencil = _isStencilEnabled();

	core::matrix4 matModel = getTransform(ETS_MODEL);
	core::matrix4 matView = getTransform(ETS_VIEW);
	core::matrix4 matProj = getTransform(ETS_PROJ);

	// Draw A Rectangle Covering The Entire Screen
	//  (-1, 1) 0---2 ( 1, 1)
	//          | / |
	//  (-1,-1) 1---3 ( 1,-1)

    S3DVertexSimpleColoured vtx[4] =
	{
		S3DVertexSimpleColoured(-1.0f, 1.0f, 0.0f, BackColor.color),
		S3DVertexSimpleColoured(-1.0f,-1.0f, 0.0f, BackColor.color),
		S3DVertexSimpleColoured( 1.0f, 1.0f, 0.0f, BackColor.color),
		S3DVertexSimpleColoured( 1.0f,-1.0f, 0.0f, BackColor.color),
	};

	static SRenderPass pass;
	pass.setLightingMode(ELM_NONE);
	pass.setFlag(EMF_GOURAUD_SHADING, false);
	pass.setFlag(EMF_ZWRITE_ENABLE, false);
	pass.setDepthTest(ECT_ALWAYS);
	pass.setAlphaTest(EAT_ALWAYS);
	pass.setFlag(EMF_BACK_FACE_CULLING, false);
	pass.setFlag(EMF_BLENDING, false);

	_disableStencil();

	core::matrix4 m;

	setTransform(ETS_MODEL, m);
	setTransform(ETS_VIEW, m);
	setTransform(ETS_PROJ, m);

	setRenderPass(pass);
	_setVertexType(EVT_SIMPLE_COLOURED);
	_setRenderStates();

	pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(*vtx));

	// restore modified states

	if (stencil)
		_enableStencil();

	setTransform(ETS_MODEL, matModel);
	setTransform(ETS_VIEW, matView);
	setTransform(ETS_PROJ, matProj);
}

//----------------------------------------------------------------------------

bool CD3D9Driver::setResourceContextCurrent()
{
	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Driver::setRenderContextCurrent()
{
	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Driver::setNullContextCurrent()
{
	return true;
}

//----------------------------------------------------------------------------

// returns the current size of the screen or rendertarget
core::dimension2d<s32> CD3D9Driver::getCurrentRenderTargetSize()
{
    if ( CurrentRendertargetSize.Width == 0 )
        return m_ScreenSize;
    else
        return CurrentRendertargetSize;
}

//---------------------------------------------------------------------------

//! return hardware occlusion object
IHardwareOcclusionQuery& CD3D9Driver::getHardwareOcclusionQuery()
{
	if (m_D3D9HardwareOcclusionQuery)
	{
		return *m_D3D9HardwareOcclusionQuery;
	}

	return m_HardwareOcclusionQueryStub;
}

//---------------------------------------------------------------------------

//! Setting polygon fill mode
void CD3D9Driver::setPolygonFillMode(E_POLYGON_FILL_MODE mode)
{
	CNullDriver::setPolygonFillMode(mode);

	// fillmode

	if (m_PolygonFillMode==EPFM_SOLID)
	{
		pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	else
	if (m_PolygonFillMode==EPFM_WIREFRAME)
	{
		pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	// EPFM_POINT
	{
		pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}
}

//----------------------------------------------------------------------------

void CD3D9Driver::resetTextureUnits()
{
	static core::matrix4 mat;
	static u32 max_units_cnt_1 = getMaximalTextureUnitsAmount() - 1;
	for (s32 unit = max_units_cnt_1; unit >= 0; unit--)
	{
		pID3DDevice->SetTransform(
			(D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+unit), (D3DMATRIX*)((void*)&mat));
		pID3DDevice->SetTextureStageState(unit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		pID3DDevice->SetTextureStageState(unit, D3DTSS_TEXCOORDINDEX, 0);
	}
	CNullDriver::resetTextureUnits();	
}

//----------------------------------------------------------------------------

//! Creating render buffer
IRenderBuffer * CD3D9Driver::createRenderBuffer(
	E_RENDER_BUFFER_TYPE rbt,
	E_VERTEX_TYPE vtype, u32 vert_size,
	E_INDEX_TYPE itype, u32 ind_size,
	E_DRAW_PRIMITIVE_TYPE dpt)
{
	switch (vtype)
	{
	case EVT_SIMPLE_COLOURED:
		return createRenderBufferTemplate <S3DVertexSimpleColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS:
		return createRenderBufferTemplate <S3DVertex1TCoords> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsColoured>(
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS:
		return createRenderBufferTemplate <S3DVertex2TCoords> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_COLOURED:
		return createRenderBufferTemplate <S3DVertex2TCoordsColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN:
		return createRenderBufferTemplate <S3DVertex1TCoordsTBN> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsTBNColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN:
		return createRenderBufferTemplate <S3DVertex2TCoordsTBN> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN_COLOURED:
		return createRenderBufferTemplate <S3DVertex2TCoordsTBNColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	case EVT_1TCOORDS_RHW_COLOURED:
		return createRenderBufferTemplate <S3DVertex1TCoordsRHWColoured> (
			rbt, vert_size, itype, ind_size, dpt);
	}
	// fail safe
	return createRenderBufferTemplate <S3DVertexSimple> (
		rbt, vert_size, itype, ind_size, dpt);
}

//----------------------------------------------------------------------------

//! Creating render buffer
IRenderBuffer * CD3D9Driver::createDynamicRenderBuffer(
	const void * varray, E_VERTEX_TYPE vtype, u32 vert_size,
	const void * iarray, E_INDEX_TYPE itype, u32 ind_size,
	E_DRAW_PRIMITIVE_TYPE dpt )
{
	switch (vtype)
	{
	case EVT_SIMPLE_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertexSimpleColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoords> (
			varray, vert_size, iarray, itype, ind_size, dpt );
	case EVT_1TCOORDS_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoords> (
			varray, vert_size, iarray, itype, ind_size, dpt );
	case EVT_2TCOORDS_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsTBN> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_TBN_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsTBNColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsTBN> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_2TCOORDS_TBN_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex2TCoordsTBNColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	case EVT_1TCOORDS_RHW_COLOURED:
		return createDynamicRenderBufferTemplate <S3DVertex1TCoordsRHWColoured> (
			varray, vert_size, iarray, itype, ind_size, dpt);
	}
	// fail safe
	return createDynamicRenderBufferTemplate <S3DVertexSimple> (
		varray, vert_size, iarray, itype, ind_size, dpt );
}

//----------------------------------------------------------------------------

template <class TVert>
inline IRenderBuffer * CD3D9Driver::createRenderBufferTemplate(
	E_RENDER_BUFFER_TYPE rbt,
	u32 vert_size, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt )
{
	if (rbt == ERBT_DYNAMIC)
		if (itype == EIT_32_BIT)
			return new CD3D9RenderArray <TVert, u32> (pID3DDevice, vert_size, ind_size, dpt);
		else
			return new CD3D9RenderArray <TVert, u16> (pID3DDevice, vert_size, ind_size, dpt);

	if (itype == EIT_32_BIT)
	 	return new CD3D9VertexBufferObject <TVert, u32> (pID3DDevice, vert_size, ind_size, dpt);

	return new CD3D9VertexBufferObject <TVert, u16> (pID3DDevice, vert_size, ind_size, dpt);
}

//----------------------------------------------------------------------------

template < class TVert >
inline IRenderBuffer * CD3D9Driver::createDynamicRenderBufferTemplate(
	const void * varray, u32 vert_size,
	const void * iarray, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt )
{
	if (itype == EIT_32_BIT)
		return new CD3D9RenderArray <TVert, u32> (
			pID3DDevice, (const TVert*)varray, vert_size, (const u32 *)iarray, ind_size, dpt );
	
	return new CD3D9RenderArray <TVert, u16> (
		pID3DDevice, (const TVert*)varray, vert_size, (const u16 *)iarray, ind_size, dpt);
}

//----------------------------------------------------------------------------

void CD3D9Driver::_renderStencilVolume(IRenderBuffer * rbuf, bool zfail)
{
	if (!m_StencilBuffer)  
        return;

	CNullDriver::_renderStencilVolume(rbuf, zfail);

	_setVertexType(rbuf->getVertices()->getType());
	_setRenderStates();

	if (zfail)
	// ZFAIL Method
	{   
		// First Pass:
		// If ztest fail increment stencil buffer value
		pID3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR );
		// Draw back-side of shadow volume in stencil only
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );

		((CNullRenderBuffer*)rbuf)->draw();     
	    
		// Second Pass:
		// If ztest fail decrement stencil buffer value
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL,  D3DSTENCILOP_DECR );
		// Draw front-side of shadow volume in stencil only
		pID3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );

		((CNullRenderBuffer*)rbuf)->draw();
	}
	else
	// ZPASS Method        
	{ 
		if (m_TwoSidedStencil)
		{
			// With 2-sided stencil, we can avoid rendering twice:

			// turn on two sided stencil
			pID3DDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );

			// setup stencil
			pID3DDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_INCRSAT );				
			pID3DDevice->SetRenderState( D3DRS_CCW_STENCILFUNC,  D3DCMP_ALWAYS );
			pID3DDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
			pID3DDevice->SetRenderState( D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP );
			pID3DDevice->SetRenderState( D3DRS_CCW_STENCILPASS,  D3DSTENCILOP_DECRSAT );

			// turn off culling
			pID3DDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );

			((CNullRenderBuffer*)rbuf)->draw();

			// turn off two sided stencil
			pID3DDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE );
		}
		else
		{
			// First Pass:
			// If ztest passes increment stencil buffer value
			pID3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCR );
			// Draw front-side of shadow volume in stencil only
			pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );

			((CNullRenderBuffer*)rbuf)->draw();   	

			// Second Pass:
			// Decrement stencil buffer value
			pID3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );
			// Draw back-side of shadow volume in stencil only
			pID3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );

			((CNullRenderBuffer*)rbuf)->draw();
		}
	}

	// restore modified states
	if (m_CurrentRenderPass.getFlag(EMF_FRONT_FACE_CCW))
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

//---------------------------------------------------------------------------

void CD3D9Driver::applyDynamicLights()
{
	CNullDriver::applyDynamicLights();

	if (!m_UseFFP)
		return;

	static u32 maxidx = getDynamicLightsMaximalAmount();
	u32 idx = 0;
	u32 enabledcnt = m_EnabledLights.size();
	for (idx = 0; idx < enabledcnt && idx < maxidx; idx++)
	{
		const SLight &dl = m_Lights[m_EnabledLights[idx]];
		{
			static D3DLIGHT9 light;

			if (dl.Type == ELT_POINT)
			{
				light.Type = D3DLIGHT_POINT;
				light.Position = *(D3DVECTOR*)((void*)(&dl.Position));
			}
			else
			if (dl.Type == ELT_DIRECTIONAL)
			{
				core::vector3df direction = dl.Position * (-1.0f);

				light.Type = D3DLIGHT_DIRECTIONAL;
				light.Direction = *(D3DVECTOR*)((void*)(&direction));
			}

			img::SColorf diffuse  = dl.DiffuseColor;
			img::SColorf specular = dl.SpecularColor;
			img::SColorf ambient  = dl.AmbientColor;

			f32 radius = dl.Radius;		
			
			light.Diffuse  = *(D3DCOLORVALUE*)((void*)(&diffuse ));
			light.Specular = *(D3DCOLORVALUE*)((void*)(&specular));
			light.Ambient  = *(D3DCOLORVALUE*)((void*)(&ambient ));

			light.Range = radius;

			light.Attenuation0 = 0.0f;
			light.Attenuation1 = 0.0f;
			light.Attenuation2 = 10.0f/(radius*radius);

			pID3DDevice->SetLight(idx, &light);
		}
		pID3DDevice->LightEnable(idx, true);
	}
	if (idx != m_LastEnabledLightsCount)
	{
		for (; idx < enabledcnt && idx < maxidx; idx++)
			pID3DDevice->LightEnable(idx, false);
		m_LastEnabledLightsCount = idx - 1;
	}
}

//----------------------------------------------------------------------------

void CD3D9Driver::_enableStencil()
{
	if (!m_StencilBuffer || _isStencilEnabled())
		return;
	pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	CNullDriver::_enableStencil();
}

//----------------------------------------------------------------------------

void CD3D9Driver::_clearStencil()
{
	if (!m_StencilBuffer)
		return;
	pID3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL , 0, 0.0f,
		MY_STENCIL_ZERO_VALUE);
}

//----------------------------------------------------------------------------

void CD3D9Driver::_setupStencil(
	E_STENCIL_OPERATION fail,  // to do if stencil test fails
	E_STENCIL_OPERATION zfail, // to do if stencil test passes and Z test fails
	E_STENCIL_OPERATION zpass, // to do if both stencil and Z tests pass	
	E_COMPARE_TEST  func,  // Stencil Test passes if ((ref & mask) func (stencil & mask)) is true
	s32 ref,        // Reference value used in stencil test
	u32 mask        // Mask value used in stencil test	
	)
{
	if (!m_StencilBuffer)
		return;

	// setup stencil compare fuction, 
	// reference value, and masks.
	// Stencil test passes if ((ref & mask) func (stencil & mask)) is true.

	pID3DDevice->SetRenderState(D3DRS_STENCILFUNC, convertToD3D9CompareFunction(func));

	pID3DDevice->SetRenderState(D3DRS_STENCILREF,       ref);
	pID3DDevice->SetRenderState(D3DRS_STENCILMASK,      mask);
	pID3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);

	// setup stencil operations

	pID3DDevice->SetRenderState(D3DRS_STENCILFAIL,  convertToD3D9StencilOp(fail)); 
	pID3DDevice->SetRenderState(D3DRS_STENCILZFAIL, convertToD3D9StencilOp(zfail)); 	
	pID3DDevice->SetRenderState(D3DRS_STENCILPASS,  convertToD3D9StencilOp(zpass)); 
}

//----------------------------------------------------------------------------

void CD3D9Driver::_disableStencil()
{
	if (!m_StencilBuffer || !_isStencilEnabled())
		return;
	pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	CNullDriver::_disableStencil();
}

//---------------------------------------------------------------------------

void CD3D9Driver::_enableScissor()
{
	pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
}

//---------------------------------------------------------------------------

void CD3D9Driver::_setupScissor(s32 left, s32 top, u32 width, u32 height)
{
	RECT rect = {left, top, left + width, top + height};
	pID3DDevice->SetScissorRect(&rect);
}

//---------------------------------------------------------------------------

void CD3D9Driver::_disableScissor()
{
	pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

//---------------------------------------------------------------------------

CNullGPUProgram* CD3D9Driver::_createGPUProgram(u32 uniforms, u32 lightcnt,
	E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
{
	if (m_UseShaders)
		return new CD3D9GPUProgram(uniforms, lightcnt,
			vertex_shader_ver, vertex_shader,
			pixel_shader_ver, pixel_shader);
	CNullGPUProgram *gpu_prog = gpu_prog = new CNullGPUProgram();
	gpu_prog->recreate(0, 0, EVSV_HLSL_1_1, NULL, EPSV_HLSL_1_1, NULL);
	return gpu_prog;
}

//----------------------------------------------------------------------------
#endif // _IRR_COMPILE_WITH_DIRECTX_9_
//----------------------------------------------------------------------------

//! creates a video driver
__MY_EXPORT__ IVideoDriver* createDirectX9Driver(
	SExposedVideoData &out_video_data,
	E_TEXTURE_FILTER textureFilter)
{
#ifdef _IRR_COMPILE_WITH_DIRECTX_9_
	CD3D9Driver* dx9 =  new CD3D9Driver(out_video_data.WindowSize);
    if (!dx9->_initDriver(out_video_data))
    {
        ((IVideoDriver*)dx9)->drop();
        dx9 = 0;
    }
	else
		dx9->setTextureFilter(textureFilter);
    return dx9;
#else
    return 0;
#endif // _IRR_COMPILE_WITH_DIRECTX_9_
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #if MY_PLATFORM == MY_PLATFORM_WIN32