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
#include "../../Game/Menus/CMouseGeneralControlOptionMenu.h"
#include "../../Engine/Entities/2D/CFontEntity.h"
#include "../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CControlManager.h"
#include "../../Managers/CGamepadManager.h"
#include "../../Misc/Global.h"

/**
* Constructor
*/
CMouseGeneralControlOptionMenu::CMouseGeneralControlOptionMenu() : CModificationMenuEntity(MOUSE_GENERAL_CONTROL_MENU_OPTION_SIZE, ID_OPTIONMENU_CONTROLS_MOUSE_GENERAL)
{
	std::ostringstream stream;

	// Action list initialization
	this->m_actionList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY] = ID_NONE;
	this->m_actionList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING] = ID_NONE;
	this->m_actionList[MOUSE_GENERAL_CONTROL_MENU_OPTION_PREVIOUS] = ID_OPTIONMENU_CONTROLS_MOUSE;

	// Sensibility - Options
	this->m_optionsListSize[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY] = MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE;
	this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY] = new int[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE];
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE ; i++)
		this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY][i] = MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_START + i * MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_PITCH;

	// Sensibility - Texts
	this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY] = new std::string[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE];
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_SENSIBILITY) << " < " << (MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_START + i * MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_PITCH) << "% >";
		this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY][i] = stream.str();
	}

	// Sensibility - Choosen option
	bool found = false;
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE && !found ; i++)
	{
		if(this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY][i] >= (this->m_controlM->GetMouseSensibility() * 100))
		{
			this->m_choosenOptionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY] = i;
			found = true;
		}
	}


	// Smoothing - Options
	this->m_optionsListSize[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING] = MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE;
	this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING] = new int[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE];
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE ; i++)
		this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING][i] = MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_START + i * MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_PITCH;

	// Smoothing - Texts
	this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING] = new std::string[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE];
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_SMOOTHING) << " < " << (MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_START + i * MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_PITCH) << " >";
		this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING][i] = stream.str();
	}

	// Smoothing - Choosen option
	found = false;
	for(int i = 0 ; i < MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE && !found ; i++)
	{
		if(this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING][i] >= this->m_controlM->GetMouseSmoothing())
		{
			this->m_choosenOptionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING] = i;
			found = true;
		}
	}


	// Previous - Text
	this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_PREVIOUS] = new std::string[1];
	this->m_optionsTextList[MOUSE_GENERAL_CONTROL_MENU_OPTION_PREVIOUS][0] = this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_PREVIOUS);

	// Previous - Choosen option
	this->m_choosenOptionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_PREVIOUS] = 0;

	stream.str("");
	stream << "Fonts/" << this->m_optionM->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/" << "default.tga";

	int centerY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	int decal = -this->m_actionSize / 2;

	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		this->m_textList[i] = new CFontEntity(stream.str(), this->m_optionsTextList[i][this->m_choosenOptionsList[i]], 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * decal), 0xFFFFFFFF, 0xFFFF0000);
		decal++;
	}
	this->m_textList[0]->SetActive(true);

	SetHeader(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_CONTROLS_MOUSE_GENERAL));

	if(this->m_optionM->GetScreenFormat(OPTIONMANAGER_CONFIGURATION_ACTIVE) == SCREEN_FORMAT_STANDARD)
		this->m_sprite = new CSpriteEntity("menu_standard.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_STANDARD_RATIO));
	else
		this->m_sprite = new CSpriteEntity("menu_wide.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_WIDE));

	ProcessFontsLocation();
}

/**
* Destructor
*/
CMouseGeneralControlOptionMenu::~CMouseGeneralControlOptionMenu(){}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CMouseGeneralControlOptionMenu::ValidateChanges(const float time)
{
	// Validation
	if(this->m_actionSize > 0)
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || (this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_actionList[this->m_actionChoosen] != ID_NONE)
		{
			this->m_validationCode = this->m_actionList[this->m_actionChoosen];
			
			// Save the options
			this->m_controlM->SetMouseSensibility(this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY][this->m_choosenOptionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY]] / 100.0f);
			this->m_controlM->SetMouseSmoothing(this->m_optionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING][this->m_choosenOptionsList[MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING]]);
			this->m_controlM->SaveMouseConfiguration();
		}
	}

	
	// Wait to avoid too much fps
	PerformWait(time);
}
