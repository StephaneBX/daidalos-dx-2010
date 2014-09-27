/*
==============================================================================================================================================================

Author: Stephane Baudoux
Attribution-Noncommercial-Share Alike 3.0 Unported

You are free:
to Share - to copy, distribute and transmit the work
to Remix - to adapt the work

Under the following conditions:
Attribution - You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
Noncommercial - You may not use this work for commercial purposes.
Share Alike - If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.

With the understanding that:
Waiver - Any of the above conditions can be waived if you get permission from the copyright holder.
Public Domain - Where the work or any of its elements is in the public domain under applicable law, that status is in no way affected by the license.
Other Rights - In no way are any of the following rights affected by the license:
Your fair dealing or fair use rights, or other applicable copyright exceptions and limitations;
The author's moral rights;
Rights other persons may have either in the work itself or in how the work is used, such as publicity or privacy rights.
Notice - For any reuse or distribution, you must make clear to others the license terms of this work. The best way to do this is with a link to this web page.

==============================================================================================================================================================
*/

#include "../Managers/CDeviceManager.h"
#include "../Misc/Global.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CLogManager.h"

/**
* Constructor
*/
CDeviceManager::CDeviceManager()
{
	this->m_zbuffer = NULL;
	this->m_input = NULL;
	this->m_d3d = NULL;
	this->m_device = NULL;
}

/**
* Destructor
*/
CDeviceManager::~CDeviceManager()
{
	SAFE_RELEASE(this->m_input);
	SAFE_RELEASE(this->m_zbuffer);
	SAFE_RELEASE(this->m_device);
	SAFE_RELEASE(this->m_d3d);
}

/**
* Initialize the manager
* @param hInst Instance
* @param hWnd Window
*/
void CDeviceManager::Initialize(const HINSTANCE hInst, const HWND hWnd)
{
	UINT AdapterToUse = D3DADAPTER_DEFAULT; 
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

	this->m_optM = COptionManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
	this->m_hwnd = hWnd;

	ZeroMemory(&this->m_d3dpp, sizeof(this->m_d3dpp));

	if(this->m_optM->IsFullScreen(OPTIONMANAGER_CONFIGURATION_ACTIVE))
		this->m_d3dpp.Windowed = FALSE;
	else
		this->m_d3dpp.Windowed = TRUE;

    this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    this->m_d3dpp.hDeviceWindow = this->m_hwnd;
    this->m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	this->m_d3dpp.BackBufferWidth = this->m_optM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE);
    this->m_d3dpp.BackBufferHeight = this->m_optM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE);
    this->m_d3dpp.EnableAutoDepthStencil = TRUE;	
    this->m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if(this->m_optM->IsVerticalSynchronizationForced(OPTIONMANAGER_CONFIGURATION_ACTIVE))
		this->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		this->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	#ifdef PERFHUD_VERSION
	for (UINT Adapter = 0 ; Adapter < this->m_d3d->GetAdapterCount() ; Adapter++)
	{
		D3DADAPTER_IDENTIFIER9  Identifier;
		HRESULT Res;
	 
		Res = this->m_d3d->GetAdapterIdentifier(Adapter,0,&Identifier);
		if(strstr(Identifier.Description,"PerfHUD") != 0)
		{ 
			AdapterToUse = Adapter;
			DeviceType = D3DDEVTYPE_REF;
			break;
		}
	}
	#endif
	
	this->m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	_VERIFY(this->m_d3d->CreateDevice(AdapterToUse, DeviceType, this->m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &this->m_d3dpp, &this->m_device), "Device Creation");

	if(this->m_device == NULL)
	{
		this->m_logM->GetStream() << "Error: Invalid resolution or format error, trying with the default configuration " << "(" << INI_DEFAULT_RESOLUTION_WIDTH << ", " << INI_DEFAULT_RESOLUTION_HEIGHT << ").\n";
		this->m_logM->Write();

		// Device initialization failed, using default value
		ZeroMemory(&this->m_d3dpp, sizeof(this->m_d3dpp));

		if(this->m_optM->IsFullScreen(OPTIONMANAGER_CONFIGURATION_ACTIVE))
			this->m_d3dpp.Windowed = FALSE;
		else
			this->m_d3dpp.Windowed = TRUE;

		this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		this->m_d3dpp.hDeviceWindow = this->m_hwnd;
		this->m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		this->m_d3dpp.BackBufferWidth = INI_DEFAULT_RESOLUTION_WIDTH;
		this->m_d3dpp.BackBufferHeight = INI_DEFAULT_RESOLUTION_HEIGHT;
		this->m_d3dpp.EnableAutoDepthStencil = TRUE;	
		this->m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		this->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		_VERIFY(this->m_d3d->CreateDevice(AdapterToUse, DeviceType, this->m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &this->m_d3dpp, &this->m_device), "Default Device Creation");

		this->m_optM->SetResolution(OPTIONMANAGER_CONFIGURATION_ACTIVE, INI_DEFAULT_RESOLUTION);
	}
	 
	InitializeDevice();

	//Init inputs
	if(this->m_input == NULL)
		_VERIFY(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->m_input, NULL), "Direct Input Creation");

	// Enumerate the full screen compatible resolutions
	int numberOfResolution = this->m_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	D3DDISPLAYMODE tempMod;
	for(int i = 0 ; i < numberOfResolution ; i++)
	{
		bool resolutionFound = false;
		this->m_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &tempMod);
		for(int j = 0 ; j < RESOLUTION_TAB_SIZE ; j++)
		{
			if(COptionManager::m_resolutionTab[j].m_width == tempMod.Width && COptionManager::m_resolutionTab[j].m_height == tempMod.Height)
			{
				COptionManager::m_resolutionTab[j].m_available = true;
				resolutionFound = true;
			}
		}
	}

	// If the game is not in fullscreen mode, enable intermediate resolutions
	if(!this->m_optM->IsFullScreen(OPTIONMANAGER_CONFIGURATION_ACTIVE))
	{
		int standardId = -1;
		int wideId = -1;
		bool wideDone = false;
		for(int i = RESOLUTION_TAB_SIZE - 1 ; i >= 0 ; i--)
		{
			// Find the last valid wide resolution
			if(wideId == -1)
			{
				if(COptionManager::m_resolutionTab[i].m_screenFormat == SCREEN_FORMAT_WIDE && COptionManager::m_resolutionTab[i].m_available == true)
					wideId = i;
			}
			else
			{
				// Enable wide resolutions that are smaller than the last valid wide resolution
				if(!wideDone)
				{
					if(COptionManager::m_resolutionTab[i].m_screenFormat == SCREEN_FORMAT_WIDE)
							COptionManager::m_resolutionTab[i].m_available = true;
				}
				else
				{
					// Find the last standard wide resolution
					if(standardId == -1)
					{
						if(COptionManager::m_resolutionTab[i].m_screenFormat == SCREEN_FORMAT_STANDARD && COptionManager::m_resolutionTab[i].m_available == true)
							standardId = i;
					}
					else
					{
						// Enable standard resolutions that are smaller than the last valid standard resolution
						if(COptionManager::m_resolutionTab[i].m_screenFormat == SCREEN_FORMAT_STANDARD)
							COptionManager::m_resolutionTab[i].m_available = true;
					}
				}
			}
		}
	}
}

/**
* Initialize the device states and get the DSS
*/
void CDeviceManager::InitializeDevice()
{
	_VERIFY(this->m_device->CreateDepthStencilSurface(this->m_d3dpp.BackBufferWidth, this->m_d3dpp.BackBufferHeight, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &this->m_zbuffer, NULL), "Default Main Depth Stencil Creation");

	// Get the current surface
	this->m_device->GetRenderTarget(0, &this->m_mainRenderTarget);

    this->m_device->SetRenderState(D3DRS_LIGHTING, false);
    this->m_device->SetRenderState(D3DRS_ZENABLE, true);
    this->m_device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));

    this->m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    this->m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    this->m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    this->m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	this->m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
	this->m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);

	this->m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	this->m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	this->m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	this->m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	this->m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	this->m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
}

/**
* Reset the device
*/
HRESULT CDeviceManager::Reset()
{
	SAFE_RELEASE(this->m_zbuffer);
	SAFE_RELEASE(this->m_mainRenderTarget);
	return this->m_device->Reset(&this->m_d3dpp);
}

/**
* Get the device
* @return The device
*/
LPDIRECT3DDEVICE9 CDeviceManager::GetDevice() const
{
	return this->m_device;
}

/**
* Get the main Render Target
* @return The main Render Target
*/
LPDIRECT3DSURFACE9 CDeviceManager::GetMainRenderTarget() const
{
	return this->m_mainRenderTarget;
}

/**
* Get the Z Buffer
* @return The Z Buffer
*/
LPDIRECT3DSURFACE9 CDeviceManager::GetMainDepthStencilSurface() const
{
	return this->m_zbuffer;
}

/**
* Get the input device
* @return The input device
*/
LPDIRECTINPUT CDeviceManager::GetInput() const
{
	return this->m_input;
}

/**
* Get the window handle
* @return The window handle
*/
HWND CDeviceManager::GetHwnd() const
{
	return this->m_hwnd;
}
