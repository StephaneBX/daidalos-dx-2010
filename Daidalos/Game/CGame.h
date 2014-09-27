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

#ifndef __CGAME_H
#define __CGAME_H

#include <string>

class CMusicSoundEntity;
class CControlManager;
class COptionManager;
class CModManager;
class CFPSCounter;
class CEngine;
class CMenuEntity;
class CSceneCamera;

enum gameState
{
	GAME_STATE_MENU,
	GAME_STATE_LOADING,
	GAME_STATE_INGAME,
	GAME_STATE_EXIT
};

enum gameMenu
{
	GAME_MENU_INTRODUCTION,
	GAME_MENU_LOADING,
	GAME_MENU_INGAME,
	GAME_MENU_MAINMENU,
	GAME_MENU_OPTIONS,
	GAME_MENU_OPTIONS_GAME,
	GAME_MENU_OPTIONS_GRAPHICS,
	GAME_MENU_OPTIONS_SOUNDS,
	GAME_MENU_OPTIONS_CONTROLS,
	GAME_MENU_OPTIONS_CONTROLS_KEYBOARD,
	GAME_MENU_OPTIONS_CONTROLS_MOUSE,
	GAME_MENU_OPTIONS_CONTROLS_MOUSE_GENERAL,
	GAME_MENU_OPTIONS_CONTROLS_MOUSE_KEYS,
	GAME_MENU_OPTIONS_CONTROLS_GAMEPAD,
	GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_GENERAL,
	GAME_MENU_OPTIONS_CONTROLS_GAMEPAD_KEYS,
	GAME_MENU_SIZE
};

/**
* CGame class.
* Manages the game updating and drawing system.
*/
class CGame
{
private:
	bool m_resetPlayer;
	int m_gameState;
	std::string m_map;
	
	CMenuEntity * m_menuList[GAME_MENU_SIZE];
	CMenuEntity * m_menu;

	CMusicSoundEntity * m_musicMenu;
	CMusicSoundEntity * m_musicGame;

	CFPSCounter * m_fpsCounter;
	CEngine * m_engine;

	CControlManager * m_controlM;
	COptionManager * m_optionM;
	CModManager * m_modM;

public:
	CGame();
	~CGame();
	void Initialize();
	void Render();
	void LoadLevel(const std::string levelName);
	void UnloadLevel();
	void OnLostDevice();
	void OnResetDevice();
	const bool Update(const float time);
	const int GetGameState() const;
};

#endif
