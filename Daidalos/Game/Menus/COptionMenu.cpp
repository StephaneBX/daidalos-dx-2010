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

#include <sstream>
#include "../../Game/Menus/COptionMenu.h"
#include "../../Engine/Entities/2D/CFontEntity.h"
#include "../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"

/**
* Constructor
*/
COptionMenu::COptionMenu() : CMenuEntity(OPTION_MENU_OPTION_SIZE, ID_OPTIONMENU)
{
	std::ostringstream stream;
	stream.str("");
	stream << "Fonts/" << this->m_optionM->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/" << "default.tga";

	this->m_actionList[OPTION_MENU_OPTION_GAME] = ID_OPTIONMENU_GAME;
	this->m_actionList[OPTION_MENU_OPTION_GRAPHICS] = ID_OPTIONMENU_GRAPHICS;
	this->m_actionList[OPTION_MENU_OPTION_SOUNDS] = ID_OPTIONMENU_SOUNDS;
	this->m_actionList[OPTION_MENU_OPTION_CONTROLS] = ID_OPTIONMENU_CONTROLS;
	this->m_actionList[OPTION_MENU_OPTION_PREVIOUS] = ID_MAINMENU;

	int centerY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;

	this->m_textList[OPTION_MENU_OPTION_GAME] = new CFontEntity(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_OPTIONS_GAME), 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY - FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * 2), 0xFFFFFFFF, 0xFFFF0000);
	this->m_textList[OPTION_MENU_OPTION_GRAPHICS] = new CFontEntity(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_OPTIONS_GRAPHICS), 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY - FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)), 0xFFFFFFFF, 0xFFFF0000);
	this->m_textList[OPTION_MENU_OPTION_SOUNDS] = new CFontEntity(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_OPTIONS_SOUNDS), 1.0f, FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, centerY, 0xFFFFFFFF, 0xFFFF0000);
	this->m_textList[OPTION_MENU_OPTION_CONTROLS] = new CFontEntity(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_OPTIONS_CONTROLS), 1.0f, FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)), 0xFFFFFFFF, 0xFFFF0000);
	this->m_textList[OPTION_MENU_OPTION_PREVIOUS] = new CFontEntity(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_PREVIOUS), 1.0f, FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * 2), 0xFFFFFFFF, 0xFFFF0000);

	this->m_textList[0]->SetActive(true);

	SetHeader(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TITLE_OPTIONS));

	if(this->m_optionM->GetScreenFormat(OPTIONMANAGER_CONFIGURATION_ACTIVE) == SCREEN_FORMAT_STANDARD)
		this->m_sprite = new CSpriteEntity("menu_standard.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_STANDARD_RATIO));
	else
		this->m_sprite = new CSpriteEntity("menu_wide.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_WIDE));

	ProcessFontsLocation();
}

/**
* Destructor
*/
COptionMenu::~COptionMenu(){}
