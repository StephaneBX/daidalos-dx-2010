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

#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>

#define PROGRAM_NAME "Daidalos Alpha 2"
#define MAXIMUM_FRAME_TIME 50

#include "Misc/CTimer.h"
#include "Managers/COptionManager.h"
#include "Managers/CDeviceManager.h"
#include "Managers/CCameraManager.h"
#include "Managers/CLogManager.h"
#include "Managers/CKeyboardManager.h"
#include "Managers/CMouseManager.h"
#include "Managers/CGamepadManager.h"
#include "Managers/CControlManager.h"
#include "Managers/CShaderManager.h"
#include "Managers/CTextureManager.h"
#include "Managers/CMeshManager.h"
#include "Managers/CMaterialManager.h"
#include "Managers/CLightManager.h"
#include "Managers/CMeshManager.h"
#include "Managers/CLangManager.h"
#include "Managers/CSoundManager.h"
#include "Managers/CParticleSystemManager.h"
#include "Managers/CModManager.h"
#include "Engine/Entities/2D/CSpriteEntity.h"
#include "Game/CGame.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"dxerr.lib")
#pragma comment(lib,"OpenAL32.lib")

COptionManager * optM;
CLogManager * logM;
CDeviceManager * deviceM;
CCameraManager * cameraM;
CKeyboardManager * keyM;
CMouseManager * mouseM;
CGamepadManager * gamepadM;
CControlManager * controlM;
CShaderManager * shaderM;
CTextureManager * textureM;
CLightManager * lightM;
CMeshManager * meshM;
CMaterialManager * materialM;
CLangManager * langM;
CSoundManager * soundM;
CParticleSystemManager * particleM;
CModManager * modM;
CGame * game;

bool deviceLost = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
bool CheckDevice();
void Render();

bool CheckDevice(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT deviceState = deviceM->GetDevice()->TestCooperativeLevel();

    switch (deviceState)
    {
        case D3DERR_DEVICELOST :
			if(!deviceLost)
			{
				CSpriteEntity::OnLostDevice();
				game->OnLostDevice();
			}
			deviceLost = true;
            Sleep(500);
            return false;
    
        case D3DERR_DEVICENOTRESET :

            if(FAILED(deviceM->Reset()))
			{
				Sleep(500);
				return false;
			}
			
			deviceM->InitializeDevice();
			CSpriteEntity::OnResetDevice();
			game->OnResetDevice();

			deviceLost = false;

            return true;
    }
	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#ifdef _WIN32
	// Debuging code
	int BreakAlloc = -1;
	_CrtSetBreakAlloc(BreakAlloc);
	// Debuging code
#endif

    HWND		hWnd;
    WNDCLASSEX	wc;
	MSG msg;

	srand((unsigned int)time(NULL));
	
	// Get the instance of the classes
	logM = CLogManager::GetInstance();
	optM = COptionManager::GetInstance();
	deviceM = CDeviceManager::GetInstance();
	cameraM = CCameraManager::GetInstance();
	keyM = CKeyboardManager::GetInstance();
	mouseM = CMouseManager::GetInstance();
	gamepadM = CGamepadManager::GetInstance();
	controlM = CControlManager::GetInstance();
	shaderM = CShaderManager::GetInstance();
	textureM = CTextureManager::GetInstance();
	meshM = CMeshManager::GetInstance();
	materialM = CMaterialManager::GetInstance();
	lightM = CLightManager::GetInstance();
	langM = CLangManager::GetInstance();
	soundM = CSoundManager::GetInstance();
	modM = CModManager::GetInstance();
	particleM = CParticleSystemManager::GetInstance();

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize			= sizeof(WNDCLASSEX);
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WindowProc;
    wc.hInstance		= hInstance;
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH) COLOR_WINDOW;
    wc.lpszClassName	= PROGRAM_NAME;
	wc.hIcon = LoadIcon(hInstance, "logo.ico");
	wc.hIconSm = LoadIcon(hInstance, "logo.ico");

    RegisterClassEx(&wc);

	if(optM->IsFullScreen(OPTIONMANAGER_CONFIGURATION_ACTIVE))
		hWnd = CreateWindowEx(NULL, PROGRAM_NAME, PROGRAM_NAME, WS_EX_TOPMOST | WS_POPUP, 0,  0, optM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), optM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), NULL, NULL, hInstance, NULL);	
	else
		hWnd = CreateWindowEx(NULL, PROGRAM_NAME, PROGRAM_NAME, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, 100, 100,optM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), optM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), NULL, NULL, hInstance, NULL);	

	if(hWnd != NULL)
	{
		ShowWindow(hWnd, nCmdShow);

		// Initialize the classes that need it
		deviceM->Initialize(hInstance, hWnd);
		textureM->Initialize();
		meshM->Initialize();
		materialM->Initialize();
		soundM->Initialize();
		modM->Initialize();
		cameraM->Initialize();
		keyM->Initialize();
		mouseM->Initialize();
		gamepadM->Initialize();
		controlM->Initialize();
		lightM->Initialize();
		particleM->Initialize();


		CHighResolutionTimer hrt;
		game = new CGame();
		float time;
		double previousTime = hrt.Elapsed();

		bool exit = false;

		while(!exit)
		{
			time = (float)((hrt.Elapsed() - previousTime) * 1000);
			previousTime = hrt.Elapsed();

			if(time > MAXIMUM_FRAME_TIME)
				time = MAXIMUM_FRAME_TIME;

			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(CheckDevice(hInstance, hWnd))
			{
				exit = game->Update(time);
				if(game->GetGameState() == GAME_STATE_INGAME)
					cameraM->Update();
				Render();
			}
		}

		delete game;
		CTextureManager::Kill();
		CLangManager::Kill();
		CShaderManager::Kill();
		CLightManager::Kill();
		CMeshManager::Kill();
		CMaterialManager::Kill();
		CSoundManager::Kill();
		CModManager::Kill();
		CControlManager::Kill();
		CGamepadManager::Kill();
		CMouseManager::Kill();
		CKeyboardManager::Kill();
		CCameraManager::Kill();
		COptionManager::Kill();
		CDeviceManager::Kill();
		CLogManager::Kill();
		
	}
	else
	{
		logM->GetStream() << "Error: Unable to create the window.\n";
		logM->Write();
	}

#ifdef _WIN32
	// Debuging code
	_CrtDumpMemoryLeaks();
	// Debuging code
#endif
	
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		break;

		case WM_SETCURSOR:
			SetCursor(NULL);
			return TRUE;
			break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}

/**
 * Draw the frame
 */
void Render()
{
	deviceM->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	deviceM->GetDevice()->BeginScene();

	game->Render();

	deviceM->GetDevice()->EndScene(); 
	deviceM->GetDevice()->Present(NULL, NULL, NULL, NULL);
}
