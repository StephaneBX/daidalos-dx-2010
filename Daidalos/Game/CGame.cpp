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
#include "../Managers/COptionManager.h"
#include "../Managers/CModManager.h"
#include "../Managers/CControlManager.h"
#include "../Engine/CFPSCounter.h"
#include "../Engine/Entities/2D/CMenuEntity.h"
#include "../Engine/Entities/Sound/CMusicSoundEntity.h"
#include "../Engine/Render/CSceneCamera.h"
#include "../Engine/CEngine.h"
#include "../Game/Menus/CMainMenu.h"
#include "../Game/Menus/CGameMenu.h"
#include "../Game/Menus/COptionMenu.h"
#include "../Game/Menus/CIntroMenu.h"
#include "../Game/Menus/CLoadingMenu.h"
#include "../Game/Menus/CGameOptionMenu.h"
#include "../Game/Menus/CGraphicOptionMenu.h"
#include "../Game/Menus/CSoundOptionMenu.h"
#include "../Game/Menus/CControlOptionMenu.h"
#include "../Game/Menus/CKeyboardControlOptionMenu.h"
#include "../Game/Menus/CGamepadControlOptionMenu.h"
#include "../Game/Menus/CGamepadGeneralControlOptionMenu.h"
#include "../Game/Menus/CGamepadKeyControlOptionMenu.h"
#include "../Game/Menus/CMouseControlOptionMenu.h"
#include "../Game/Menus/CMouseGeneralControlOptionMenu.h"
#include "../Game/Menus/CMouseKeyControlOptionMenu.h"
#include "../Game/CGame.h"

/**
* Constructor
*/
CGame::CGame()
{
	Initialize();
}

/**
* Destructor
*/
CGame::~CGame()
{
	for(int i = 0 ; i < GAME_MENU_SIZE ; i++)
		SAFE_DELETE(this->m_menuList[i]);
	SAFE_DELETE(this->m_fpsCounter);
	SAFE_DELETE(this->m_engine);
}

/**
* Initialize the game
*/
void CGame::Initialize()
{
	this->m_controlM = CControlManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_fpsCounter = new CFPSCounter();
	this->m_engine = new CEngine();

	this->m_gameState = GAME_STATE_MENU;

	// Pre instanciate all the menus
	this->m_menuList[GAME_MENU_INTRODUCTION] = new CIntroMenu();
	this->m_menuList[GAME_MENU_LOADING] = new CLoadingMenu();
	this->m_menuList[GAME_MENU_INGAME] = new CGameMenu();
	this->m_menuList[GAME_MENU_MAINMENU] = new CMainMenu();
	this->m_menuList[GAME_MENU_OPTIONS] = new COptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_GAME] = new CGameOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_GRAPHICS] = new CGraphicOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_SOUNDS] = new CSoundOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_CONTROLS] = new CControlOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_KEYBOARD] = new CKeyboardControlOptionMenu();

	if(this->m_controlM->IsGamepadDetected())
	{
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD] = new CGamepadControlOptionMenu();
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_GENERAL] = new CGamepadGeneralControlOptionMenu();
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_KEYS] = new CGamepadKeyControlOptionMenu();
	}
	else
	{
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD] = NULL;
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_GENERAL] = NULL;
		this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_KEYS] = NULL;
	}

	this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE] = new CMouseControlOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE_GENERAL] = new CMouseGeneralControlOptionMenu();
	this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE_KEYS] = new CMouseKeyControlOptionMenu();

	this->m_menu = this->m_menuList[GAME_MENU_INTRODUCTION];

	this->m_resetPlayer = true;

	this->m_musicMenu = this->m_modM->LoadMusic("menus.wav");
	this->m_musicMenu->Play();

	this->m_musicGame = NULL;
}

/**
* Render the game
*/
void CGame::Render()
{
	switch(this->m_gameState)
	{
	case GAME_STATE_MENU:
		this->m_menu->Render();
		if(this->m_menu->GetMenuCode() != ID_INTRODUCTION)
			this->m_controlM->ShowMouseCursor();
		break;

	case GAME_STATE_INGAME:
		this->m_engine->Render();
		if(this->m_menu != NULL)
		{
			this->m_menu->Render();
			this->m_controlM->ShowMouseCursor();
		}
		if(this->m_optionM->IsFPSEnabled(OPTIONMANAGER_CONFIGURATION_ACTIVE))
			this->m_fpsCounter->Render();
		break;

	case GAME_STATE_LOADING:
		this->m_menu->Render();
		break;
	}
}

/**
* Load a level
* @param levelName The level name
*/
void CGame::LoadLevel(const std::string levelName)
{
	this->m_engine->LoadLevel(levelName, this->m_resetPlayer);
}

/**
* Unload a level
*/
void CGame::UnloadLevel()
{
	this->m_engine->UnloadLevel();
}

/**
* Action performed when a device lost event is detected
* Free any ressource that need to be before reseting the device
*/
void CGame::OnLostDevice()
{
	this->m_engine->OnLostDevice();
}

/**
* Action performed when a device lost reset is detected
* Allocate the necessary ressources
*/
void CGame::OnResetDevice()
{
	this->m_engine->OnResetDevice();
}

/**
* Update the game
* @return True if the player has requested to exit the game
*/
const bool CGame::Update(const float time)
{
	bool ret = false;
	int internalState;

	this->m_controlM->Update();

	if(this->m_musicMenu->IsPlaying())
		this->m_musicMenu->Update();

	if(this->m_musicGame != NULL && this->m_musicGame->IsPlaying())
		this->m_musicGame->Update();

	if(this->m_optionM->IsFPSEnabled(OPTIONMANAGER_CONFIGURATION_ACTIVE))
		this->m_fpsCounter->Update(time);

	switch(this->m_gameState)
	{
	case GAME_STATE_MENU:
		this->m_menu->Update(time);
		this->m_menu->ValidateChanges(time);
		internalState = this->m_menu->GetSelectedMenu();
		if(internalState != this->m_menu->GetMenuCode())
		{
			switch(internalState)
			{
			case ID_EXITGAME:
				this->m_gameState = GAME_STATE_EXIT;
				ret = true;
				break;

			case ID_PLAYGAME:
				this->m_gameState = GAME_STATE_LOADING;
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_LOADING];
				this->m_map = this->m_modM->GetModLevel(0);
				break;

			case ID_OPTIONMENU:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_GAME:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_GAME];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_GRAPHICS:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_GRAPHICS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_SOUNDS:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_SOUNDS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_KEYBOARD:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_KEYBOARD];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_GAMEPAD:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_GAMEPAD_GENERAL:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_GENERAL];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_GAMEPAD_KEYS:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_KEYS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_MOUSE:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_MOUSE_GENERAL:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE_GENERAL];
				this->m_controlM->FreezeInputs();
				break;

			case ID_OPTIONMENU_CONTROLS_MOUSE_KEYS:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_OPTIONS_CONTROLS_MOUSE_KEYS];
				this->m_controlM->FreezeInputs();
				break;

			case ID_MAINMENU:
				this->m_menu->ResetState();
				this->m_menu = this->m_menuList[GAME_MENU_MAINMENU];
				this->m_controlM->FreezeInputs();
				break;
			}
		}
		break;

	case GAME_STATE_INGAME:
		if(this->m_menu != NULL)
		{
			this->m_menu->Update(time);
			this->m_menu->ValidateChanges(time);
			internalState = this->m_menu->GetSelectedMenu();
			if(internalState != this->m_menu->GetMenuCode())
			{
				switch(internalState)
				{
				case ID_RESUMEGAME:
					this->m_menu->ResetState();
					this->m_menu = NULL;
					this->m_controlM->FreezeInputs();
					break;

				case ID_MAINMENU:
					this->m_musicGame->Stop();
					this->m_gameState = GAME_STATE_MENU;
					this->m_menu->ResetState();
					this->m_menu = this->m_menuList[GAME_MENU_MAINMENU];
					this->m_controlM->FreezeInputs();
					UnloadLevel();
					this->m_musicMenu->Play();
					break;
				}
			}
		}
		else
		{
			if(this->m_controlM->IsKeyDown(ACTION_KEY_ESCAPE_MENU))
			{
				this->m_menu = this->m_menuList[GAME_MENU_INGAME];
				this->m_controlM->FreezeInputs();
				this->m_controlM->CenterMouseCursor();
			}
			else
			{
				this->m_engine->Update(time);
			}
		}
		break;

	case GAME_STATE_LOADING:
		this->m_musicMenu->Stop();
		LoadLevel(this->m_map);
		this->m_menu = NULL;
		this->m_gameState = GAME_STATE_INGAME;
		this->m_controlM->FreezeInputs();
		this->m_musicGame = this->m_modM->LoadMusic("level_1.wav");
		this->m_musicGame->Play();

		// Temporary
		/*CEmitterEntity * emitter = this->m_modM->LoadEmitter("smoke leak");
		emitter->SetPosition(D3DXVECTOR3(0, 2, 10));*/
		break;
	}

	return ret;
}

/**
* Get the game state
* @return The game state
*/
const int CGame::GetGameState() const
{
	return this->m_gameState;
}
