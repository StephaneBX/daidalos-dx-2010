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

#ifndef __CLANGMANAGER_H
#define __CLANGMANAGER_H

#define LANGMANAGER_MAX_CHAR_PER_LINE 200

#include <vector>
#include <string>
#include "../Managers/CSingletonManager.h"

enum 
{
	LANGMANAGER_TEXT_MENU_PLAY,
	LANGMANAGER_TEXT_MENU_OPTIONS,
	LANGMANAGER_TEXT_MENU_OPTIONS_GAME,
	LANGMANAGER_TEXT_MENU_OPTIONS_GRAPHICS,
	LANGMANAGER_TEXT_MENU_OPTIONS_SOUNDS,
	LANGMANAGER_TEXT_MENU_OPTIONS_CONTROLS,
	LANGMANAGER_TEXT_MENU_QUIT,
	LANGMANAGER_TEXT_MENU_PREVIOUS,
	LANGMANAGER_TEXT_MENU_LOADING,
	LANGMANAGER_TEXT_MENU_RESUME,
	LANGMANAGER_TEXT_MENU_LOW,
	LANGMANAGER_TEXT_MENU_MEDIUM,
	LANGMANAGER_TEXT_MENU_HIGH,
	LANGMANAGER_TEXT_MENU_DISABLED,
	LANGMANAGER_TEXT_MENU_ENABLED,
	LANGMANAGER_TEXT_MENU_RESOLUTION,
	LANGMANAGER_TEXT_MENU_FULLSCREEN,
	LANGMANAGER_TEXT_MENU_TEXTUREQUALITY,
	LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING,
	LANGMANAGER_TEXT_MENU_SHADOWS,
	LANGMANAGER_TEXT_MENU_DEFERREDSHADING,
	LANGMANAGER_TEXT_MENU_VERTICALSYNCHRO,
	LANGMANAGER_TEXT_MENU_VIEWDISTANCE,
	LANGMANAGER_TEXT_MENU_LANGAGE,
	LANGMANAGER_TEXT_MENU_SHOWFPS,
	LANGMANAGER_TEXT_MENU_ENGLISH,
	LANGMANAGER_TEXT_MENU_FRENCH,
	LANGMANAGER_TEXT_MENU_MUSIC,
	LANGMANAGER_TEXT_MENU_EFFECTS,
	LANGMANAGER_TEXT_MENU_KEYBOARD,
	LANGMANAGER_TEXT_MENU_MOUSE,
	LANGMANAGER_TEXT_MENU_GAMEPAD,
	LANGMANAGER_TEXT_MENU_GENERALOPTIONS,
	LANGMANAGER_TEXT_MENU_KEYSASSIGNMENT,
	LANGMANAGER_TEXT_MENU_SENSIBILITY,
	LANGMANAGER_TEXT_MENU_SMOOTHING,
	LANGMANAGER_TEXT_MENU_INVERSELEFTXANALOGAXIS,
	LANGMANAGER_TEXT_MENU_INVERSELEFTYANALOGAXIS,
	LANGMANAGER_TEXT_MENU_INVERSERIGHTXANALOGAXIS,
	LANGMANAGER_TEXT_MENU_INVERSERIGHTYANALOGAXIS,
	LANGMANAGER_TEXT_MENU_FORWARD_BACKWARD,
	LANGMANAGER_TEXT_MENU_FORWARD,
	LANGMANAGER_TEXT_MENU_BACKWARD,
	LANGMANAGER_TEXT_MENU_STRAFE,
	LANGMANAGER_TEXT_MENU_STRAFE_LEFT,
	LANGMANAGER_TEXT_MENU_STRAFE_RIGHT,
	LANGMANAGER_TEXT_MENU_TURN,
	LANGMANAGER_TEXT_MENU_TURN_LEFT,
	LANGMANAGER_TEXT_MENU_TURN_RIGHT,
	LANGMANAGER_TEXT_MENU_LOOK,
	LANGMANAGER_TEXT_MENU_LOOK_UP,
	LANGMANAGER_TEXT_MENU_LOOK_DOWN,
	LANGMANAGER_TEXT_MENU_CENTER_VIEW,
	LANGMANAGER_TEXT_MENU_FLASHLIGHT,
	LANGMANAGER_TEXT_MENU_FIRE,
	LANGMANAGER_TEXT_MENU_JUMP,
	LANGMANAGER_TEXT_MENU_CROUCH,
	LANGMANAGER_TEXT_MENU_ACTION,
	LANGMANAGER_TEXT_MENU_MENU,
	LANGMANAGER_TEXT_MENU_WAITING_FOR_KEY,
	LANGMANAGER_TEXT_MENU_AXIS,
	LANGMANAGER_TEXT_MENU_BUTTON,
	LANGMANAGER_TEXT_MENU_NEED_REBOOT,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_GENERAL,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_GRAPHICS,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_SOUNDS,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_KEYBOARD,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_MOUSE,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_MOUSE_GENERAL,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_MOUSE_KEYS,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_GAMEPAD,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_GAMEPAD_GENERAL,
	LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_GAMEPAD_KEYS,
	LANGMANAGER_TEXT_SIZE
};

/**
* CLangManager Class.
* Manages langage files.
*/
class CLangManager : public CSingletonManager<CLangManager>
{
	friend class CSingletonManager<CLangManager>;

private:
	std::vector<std::string> m_langFilename;
	std::vector<std::string> m_langDatabase;

public:
	void LoadLangFileList(std::string modDirectory);
	void ReloadLangDatabase(std::string modDirectory);
	const std::string GetText(const unsigned int v);
	const int GetNumberOfLang() const;

private:
	CLangManager();
	~CLangManager();
};

#endif
