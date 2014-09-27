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
#include "../../Game/Menus/CMouseKeyControlOptionMenu.h"
#include "../../Engine/Entities/2D/CFontEntity.h"
#include "../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CControlManager.h"
#include "../../Managers/CMouseManager.h"
#include "../../Misc/Global.h"

/**
* Constructor
*/
CMouseKeyControlOptionMenu::CMouseKeyControlOptionMenu() : CKeysMenuEntity(MOUSE_KEY_CONTROL_MENU_OPTION_SIZE, ID_OPTIONMENU_CONTROLS_MOUSE_KEYS)
{
	std::ostringstream stream;

	// Action list initialization
	this->m_actionList[MOUSE_KEY_CONTROL_MENU_OPTION_CENTER_VIEW] = ID_NONE;
	this->m_actionList[MOUSE_KEY_CONTROL_MENU_OPTION_FLASHLIGHT] = ID_NONE;
	this->m_actionList[MOUSE_KEY_CONTROL_MENU_OPTION_JUMP] = ID_NONE;
	this->m_actionList[MOUSE_KEY_CONTROL_MENU_OPTION_PREVIOUS] = ID_OPTIONMENU_CONTROLS_MOUSE;


	// Center view - Option
	this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_CENTER_VIEW] = this->m_controlM->GetMouseButton(ACTION_KEY_CENTER_VIEW);

	// Center view - Text
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_CENTER_VIEW);
	this->m_optionsTextList[MOUSE_KEY_CONTROL_MENU_OPTION_CENTER_VIEW] = stream.str();


	// Flashlight - Option
	this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_FLASHLIGHT] = this->m_controlM->GetMouseButton(ACTION_KEY_FLASHLIGHT);

	// Flashlight - Text
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_FLASHLIGHT);
	this->m_optionsTextList[MOUSE_KEY_CONTROL_MENU_OPTION_FLASHLIGHT] = stream.str();


	// Jump - Option
	this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_JUMP] = this->m_controlM->GetMouseButton(ACTION_KEY_JUMP);

	// Jump - Text
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_JUMP);
	this->m_optionsTextList[MOUSE_KEY_CONTROL_MENU_OPTION_JUMP] = stream.str();


	// Previous - Text
	this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_PREVIOUS] = 0;
	this->m_optionsTextList[MOUSE_KEY_CONTROL_MENU_OPTION_PREVIOUS] = this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_PREVIOUS);

	stream.str("");
	stream << "Fonts/" << this->m_optionM->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/" << "default.tga";
	std::string font = stream.str();

	int centerY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	int decal = -this->m_actionSize / 2;

	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		stream.str("");
		if(this->m_optionsList[i] == ACTION_KEY_DISABLED)
			stream << this->m_optionsTextList[i] << " : ";
		else
			stream << this->m_optionsTextList[i] << " : " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_BUTTON) << " " << this->m_optionsList[i];

		if(this->m_actionList[i] == ID_NONE)
			this->m_textList[i] = new CFontEntity(font, stream.str(), 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * decal), 0xFFFFFFFF, 0xFFFF0000);
		else
			this->m_textList[i] = new CFontEntity(font, this->m_optionsTextList[i], 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * decal), 0xFFFFFFFF, 0xFFFF0000);

		decal++;
	}
	this->m_textList[0]->SetActive(true);

	SetHeader(font, this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_MOUSE_KEYS));

	if(this->m_optionM->GetScreenFormat(OPTIONMANAGER_CONFIGURATION_ACTIVE) == SCREEN_FORMAT_STANDARD)
		this->m_sprite = new CSpriteEntity("menu_standard.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_STANDARD_RATIO));
	else
		this->m_sprite = new CSpriteEntity("menu_wide.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_WIDE));

	ProcessFontsLocation();
}

/**
* Destructor
*/
CMouseKeyControlOptionMenu::~CMouseKeyControlOptionMenu(){}

/**
* Refresh the menu
* @param time Elapsed time
*/
void CMouseKeyControlOptionMenu::Update(const float time)
{
	std::ostringstream stream;

	if(this->m_mode == KEYS_MENU_ENTITY_MODE_NAVIGATION)
	{
		CMenuEntity::Update(time);
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY())) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_movedTimer >= MENU_ENTITY_DELAY2 && this->m_actionList[this->m_actionChoosen] == ID_NONE)
		{
			stream.str("");
			stream << this->m_optionsTextList[this->m_actionChoosen] << " : " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_WAITING_FOR_KEY);
			this->m_textList[this->m_actionChoosen]->SetText(stream.str());
			this->m_controlM->FreezeInputs();
			this->m_mode = KEYS_MENU_ENTITY_MODE_ASSIGNMENT;
		}
	}
	else
	{
		this->m_keyAssignmentTimer += time;
		if(this->m_keyAssignmentTimer >= KEY_MENU_ENTITY_KEY_ASSIGNMENT_DELAY)
		{
			int newKey = this->m_controlM->GetMouseFirstButtonDown();
			int escapeKey = this->m_controlM->GetKeyboardFirstKeyDown();

			if(escapeKey == DIK_ESCAPE)
			{
				stream.str("");
				stream << this->m_optionsTextList[this->m_actionChoosen] << " : ";

				this->m_optionsList[this->m_actionChoosen] = ACTION_KEY_DISABLED;
				this->m_textList[this->m_actionChoosen]->SetText(stream.str());
				this->m_controlM->FreezeInputs();
				this->m_mode = KEYS_MENU_ENTITY_MODE_NAVIGATION;
				this->m_keyAssignmentTimer = 0;
			}
			else
			{
				if(newKey != -1)
				{
					this->m_optionsList[this->m_actionChoosen] = newKey;

					stream.str("");
					stream << this->m_optionsTextList[this->m_actionChoosen] << " : " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_BUTTON) << " " << newKey;
					this->m_textList[this->m_actionChoosen]->SetText(stream.str());
					this->m_controlM->FreezeInputs();
					this->m_mode = KEYS_MENU_ENTITY_MODE_NAVIGATION;
					this->m_keyAssignmentTimer = 0;
				}
			}
		}
	}
}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CMouseKeyControlOptionMenu::ValidateChanges(const float time)
{
	// Validation
	if(this->m_actionSize > 0)
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || (this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_actionList[this->m_actionChoosen] != ID_NONE)
		{
			this->m_validationCode = this->m_actionList[this->m_actionChoosen];
			this->m_mode = KEYS_MENU_ENTITY_MODE_NAVIGATION;

			// Save the options
			this->m_controlM->SetMouseButton(ACTION_KEY_CENTER_VIEW, this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_CENTER_VIEW]);
			this->m_controlM->SetMouseButton(ACTION_KEY_FLASHLIGHT, this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_FLASHLIGHT]);
			this->m_controlM->SetMouseButton(ACTION_KEY_JUMP, this->m_optionsList[MOUSE_KEY_CONTROL_MENU_OPTION_JUMP]);
			this->m_controlM->SaveMouseConfiguration();
		}
	}

	// Wait to avoid too much fps
	PerformWait(time);
}
