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

#ifndef __CMENUENTITY_H
#define __CMENUENTITY_H

#define MENU_ENTITY_DELAY 200
#define MENU_ENTITY_DELAY2 200
#define MENU_TEXTURE_SIZE 1024.0f
#define MENU_DELAY_TIMER 32
#define MENU_NUMBER_OF_VISIBLE_FONTS 9
#define MENU_CENTER_MODIFIER 1.75f
#define MENU_ARROW_RATIO 0.02f
#define MENU_ARROW_SIZE 32
#define MENU_ARROW_POSITION_X 0.9f
#define MENU_ARROW_TOP_POSITION_Y 0.2f
#define MENU_ARROW_BOTTOM_POSITION_Y 0.9f

#include <string>

enum menuId
{
	ID_NONE,
	ID_MAINMENU,
	ID_OPTIONMENU,
	ID_OPTIONMENU_GAME,
	ID_OPTIONMENU_GRAPHICS,
	ID_OPTIONMENU_CONTROLS,
	ID_OPTIONMENU_SOUNDS,
	ID_OPTIONMENU_CONTROLS_KEYBOARD,
	ID_OPTIONMENU_CONTROLS_MOUSE,
	ID_OPTIONMENU_CONTROLS_GAMEPAD,
	ID_OPTIONMENU_CONTROLS_MOUSE_GENERAL,
	ID_OPTIONMENU_CONTROLS_MOUSE_KEYS,
	ID_OPTIONMENU_CONTROLS_GAMEPAD_GENERAL,
	ID_OPTIONMENU_CONTROLS_GAMEPAD_KEYS,
	ID_INGAMEMENU,
	ID_EXITGAME,
	ID_PLAYGAME,
	ID_PAUSEMENU,
	ID_RESUMEGAME,
	ID_RETRYGAME,
	ID_GAMEOVERMENU,
	ID_INTRODUCTION,
	ID_LOADING
};

enum menuShift
{
	MENU_ENTITY_SHIFT_BOTTOM,
	MENU_ENTITY_SHIFT_TOP
};

class CSpriteEntity;
class CFontEntity;
class COptionManager;
class CControlManager;
class CModManager;

/**
* CMenuEntity class.
* Generic CMenuEntity class that manages every menu.
*/
class CMenuEntity
{
protected:
	int m_actionSize;
	int m_menuCode;
	int m_actionChoosen;
	int m_validationCode;
	int m_topFontId;
	float m_movedTimer;
	int * m_actionList;
	CFontEntity * m_headerText;
	CFontEntity * m_footerText;
	CFontEntity ** m_textList;
	CSpriteEntity * m_sprite;
	CSpriteEntity * m_topArrow;
	CSpriteEntity * m_bottomArrow;
	
	COptionManager * m_optionM;
	CControlManager * m_controlM;
	CModManager * m_modM;

public:
	CMenuEntity(const int actionSize, const int codeMenu);
	virtual ~CMenuEntity();
	void ResetState();
	void ProcessFontsLocation();
	virtual void Update(const float time);
	virtual void Render();
	virtual void ValidateChanges(const float time);
	int GetMenuCode();
	int GetSelectedMenu();

protected:
	void PerformWait(const float time);
	void ShiftText(const int way);
	void SetHeader(std::string font, std::string text);
	void SetFooter(std::string font, std::string text);
};

#endif
