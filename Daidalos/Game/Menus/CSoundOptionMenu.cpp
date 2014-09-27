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
#include "../../Game/Menus/CSoundOptionMenu.h"
#include "../../Engine/Entities/2D/CFontEntity.h"
#include "../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CControlManager.h"
#include "../../Misc/Global.h"

/**
* Constructor
*/
CSoundOptionMenu::CSoundOptionMenu() : CModificationMenuEntity(SOUND_MENU_OPTION_SIZE, ID_OPTIONMENU_GAME)
{
	std::ostringstream stream;
	bool found;

	// Action list initialization
	this->m_actionList[SOUND_MENU_OPTION_MUSIC] = ID_NONE;
	this->m_actionList[SOUND_MENU_OPTION_EFFECTS] = ID_NONE;
	this->m_actionList[SOUND_MENU_OPTION_PREVIOUS] = ID_OPTIONMENU;

	// Music - Options
	this->m_optionsListSize[SOUND_MENU_OPTION_MUSIC] = SOUND_MENU_OPTION_MUSIC_SIZE;
	this->m_optionsList[SOUND_MENU_OPTION_MUSIC] = new int[SOUND_MENU_OPTION_MUSIC_SIZE];
	for(int i = 0 ; i < SOUND_MENU_OPTION_MUSIC_SIZE ; i++)
		this->m_optionsList[SOUND_MENU_OPTION_MUSIC][i] = i * SOUND_MENU_OPTION_MUSIC_PITCH;

	// Music - Texts
	this->m_optionsTextList[SOUND_MENU_OPTION_MUSIC] = new std::string[SOUND_MENU_OPTION_MUSIC_SIZE];
	for(int i = 0 ; i < SOUND_MENU_OPTION_MUSIC_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_MUSIC) << " < " << (i * SOUND_MENU_OPTION_MUSIC_PITCH) << "% >";
		this->m_optionsTextList[SOUND_MENU_OPTION_MUSIC][i] = stream.str();
	}

	// Music - Choosen option
	found = false;
	for(int i = 0 ; i < SOUND_MENU_OPTION_MUSIC_SIZE && !found ; i++)
	{
		if(this->m_optionsList[SOUND_MENU_OPTION_MUSIC][i] >= this->m_optionM->GetMusicVolume(OPTIONMANAGER_CONFIGURATION_SAVED) * 1000.0f)
		{
			this->m_choosenOptionsList[SOUND_MENU_OPTION_MUSIC] = i;
			found = true;
		}
	}
	if(!found)
		this->m_choosenOptionsList[SOUND_MENU_OPTION_MUSIC] = SOUND_MENU_OPTION_MUSIC_SIZE - 1;

	// Effects - Options
	this->m_optionsListSize[SOUND_MENU_OPTION_EFFECTS] = SOUND_MENU_OPTION_EFFECTS_SIZE;
	this->m_optionsList[SOUND_MENU_OPTION_EFFECTS] = new int[SOUND_MENU_OPTION_EFFECTS_SIZE];
	for(int i = 0 ; i < SOUND_MENU_OPTION_EFFECTS_SIZE ; i++)
		this->m_optionsList[SOUND_MENU_OPTION_EFFECTS][i] = i * SOUND_MENU_OPTION_MUSIC_PITCH;

	// Effects - Texts
	this->m_optionsTextList[SOUND_MENU_OPTION_EFFECTS] = new std::string[SOUND_MENU_OPTION_EFFECTS_SIZE];
	for(int i = 0 ; i < SOUND_MENU_OPTION_EFFECTS_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_EFFECTS) << " < " << (i * SOUND_MENU_OPTION_EFFECTS_PITCH) << "% >";
		this->m_optionsTextList[SOUND_MENU_OPTION_EFFECTS][i] = stream.str();
	}

	// Effects - Choosen option
	found = false;
	for(int i = 0 ; i < SOUND_MENU_OPTION_EFFECTS_SIZE && !found ; i++)
	{
		if(this->m_optionsList[SOUND_MENU_OPTION_EFFECTS][i] >= this->m_optionM->GetEffectsVolume(OPTIONMANAGER_CONFIGURATION_SAVED) * 1000.0f)
		{
			this->m_choosenOptionsList[SOUND_MENU_OPTION_EFFECTS] = i;
			found = true;
		}
	}
	if(!found)
		this->m_choosenOptionsList[SOUND_MENU_OPTION_EFFECTS] = SOUND_MENU_OPTION_EFFECTS_SIZE - 1;


	// Previous - Text
	this->m_optionsTextList[SOUND_MENU_OPTION_PREVIOUS] = new std::string[1];
	this->m_optionsTextList[SOUND_MENU_OPTION_PREVIOUS][0] = this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_PREVIOUS);

	// Previous - Choosen option
	this->m_choosenOptionsList[SOUND_MENU_OPTION_PREVIOUS] = 0;

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

	SetHeader(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_SOUNDS));

	if(this->m_optionM->GetScreenFormat(OPTIONMANAGER_CONFIGURATION_ACTIVE) == SCREEN_FORMAT_STANDARD)
		this->m_sprite = new CSpriteEntity("menu_standard.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_STANDARD_RATIO));
	else
		this->m_sprite = new CSpriteEntity("menu_wide.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_WIDE));

	ProcessFontsLocation();
}

/**
* Destructor
*/
CSoundOptionMenu::~CSoundOptionMenu(){}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CSoundOptionMenu::ValidateChanges(const float time)
{
	// Validation
	if(this->m_actionSize > 0)
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || (this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_actionList[this->m_actionChoosen] != ID_NONE)
		{
			this->m_validationCode = this->m_actionList[this->m_actionChoosen];
			
			// Save the options
			this->m_optionM->SetMusicVolume(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[SOUND_MENU_OPTION_MUSIC][this->m_choosenOptionsList[SOUND_MENU_OPTION_MUSIC]] / 1000.0f);
			this->m_optionM->SetEffectsVolume(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[SOUND_MENU_OPTION_EFFECTS][this->m_choosenOptionsList[SOUND_MENU_OPTION_EFFECTS]] / 1000.0f);
			this->m_optionM->Save();
		}
	}

	// Wait to avoid too much fps
	PerformWait(time);
}
