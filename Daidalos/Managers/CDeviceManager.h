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

#ifndef __CDEVICEMANAGER_H
#define __CDEVICEMANAGER_H

#define DIRECTINPUT_VERSION 0x0800
//#define PERFHUD_VERSION

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <dinput.h>
#include "../Managers/CSingletonManager.h"

class COptionManager;
class CLogManager;

/**
* CDeviceManager class.
* Manages the DirectX devices.
*/
class CDeviceManager : public CSingletonManager<CDeviceManager>
{
friend class CSingletonManager<CDeviceManager>;

private:
	HWND m_hwnd;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DDEVICE9 m_device;
	LPDIRECTINPUT m_input;
	LPDIRECT3DSURFACE9 m_zbuffer;
	LPDIRECT3DSURFACE9 m_mainRenderTarget;
	D3DPRESENT_PARAMETERS m_d3dpp;

	COptionManager * m_optM;
	CLogManager * m_logM;

public:
	~CDeviceManager();
	void Initialize(const HINSTANCE hInst, const HWND hWnd);
	void InitializeDevice();
	HRESULT Reset();
	LPDIRECT3DDEVICE9 GetDevice() const;
	LPDIRECT3DSURFACE9 GetMainRenderTarget() const;
	LPDIRECT3DSURFACE9 GetMainDepthStencilSurface() const;
	LPDIRECTINPUT GetInput() const;
	HWND GetHwnd() const;

private:
	CDeviceManager();
};

#endif
