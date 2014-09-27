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

#include "../../../Engine/Entities/2D/CMenuEntity.h"
#include "../../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../../Engine/Entities/2D/CFontEntity.h"
#include "../../../Managers/COptionManager.h"
#include "../../../Managers/CControlManager.h"
#include "../../../Managers/CModManager.h"
#include "../../../Misc/Global.h"

/**
* Constructor
* @param actionSize The number of actions
* @param menuCode The code of the menu
*/
CMenuEntity::CMenuEntity(const int actionSize, const int menuCode)
{
	this->m_optionM = COptionManager::GetInstance();
	this->m_controlM = CControlManager::GetInstance();
	this->m_modM = CModManager::GetInstance();

	this->m_menuCode = menuCode;
	this->m_actionSize = actionSize;

	if(actionSize > 0)
	{
		this->m_actionList = new int[this->m_actionSize];
		this->m_textList = new CFontEntity*[this->m_actionSize];
	}
	else
	{
		this->m_actionList = NULL;
		this->m_textList = NULL;
	}

	this->m_movedTimer = 0;
	this->m_validationCode = this->m_menuCode;
	this->m_actionChoosen = 0;
	this->m_topFontId = 0;

	this->m_headerText = NULL;
	this->m_footerText = NULL;
	this->m_sprite = NULL;

	this->m_topArrow = new CSpriteEntity("top_arrow.png", D3DXVECTOR2(MENU_ARROW_POSITION_X * this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), MENU_ARROW_TOP_POSITION_Y * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)), D3DXVECTOR2(0, 0), D3DXVECTOR2((this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * MENU_ARROW_RATIO) / MENU_ARROW_SIZE, (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * MENU_ARROW_RATIO) / MENU_ARROW_SIZE) );
	this->m_bottomArrow = new CSpriteEntity("bottom_arrow.png", D3DXVECTOR2(MENU_ARROW_POSITION_X * this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), MENU_ARROW_BOTTOM_POSITION_Y * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)), D3DXVECTOR2(0, 0), D3DXVECTOR2((this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * MENU_ARROW_RATIO) / MENU_ARROW_SIZE, (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * MENU_ARROW_RATIO) / MENU_ARROW_SIZE) );
}

/**
* Destructor
*/
CMenuEntity::~CMenuEntity()
{
	SAFE_DELETE_ARRAY(this->m_actionList);
	SAFE_DELETE(this->m_sprite);

	SAFE_DELETE(this->m_topArrow);
	SAFE_DELETE(this->m_bottomArrow);

	SAFE_DELETE(this->m_headerText);
	SAFE_DELETE(this->m_footerText);
	for(int i = 0 ; i < this->m_actionSize ; i++)
		SAFE_DELETE(this->m_textList[i]);
	SAFE_DELETE_ARRAY(this->m_textList);
}

/**
* Get the menu code
* @return The menu code
*/
int CMenuEntity::GetMenuCode()
{
	return this->m_menuCode;
}

/**
* Get the selected action
* @return The selected action
*/
int CMenuEntity::GetSelectedMenu()
{
	return this->m_validationCode;
}

/**
* Reset the validation code
*/
void CMenuEntity::ResetState()
{
	this->m_validationCode = this->m_menuCode;
}

/**
* Fonts position recalculation
* The goal is to center the fonts that are supposed to be visible (only MENU_NUMBER_OF_VISIBLE_FONTS fonts are visible)
*/
void CMenuEntity::ProcessFontsLocation()
{
	float topY = 0;
	if(this->m_actionSize <= MENU_NUMBER_OF_VISIBLE_FONTS)
	{
		// All the fonts are visible
		topY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_CENTER_MODIFIER - (this->m_actionSize / 2.0f) * FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE);
	}
	else
	{
		// Draw only the visible fonts
		topY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_CENTER_MODIFIER - (MENU_NUMBER_OF_VISIBLE_FONTS / 2.0f) * FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE);
	}

	// Set to visible the MENU_NUMBER_OF_VISIBLE_FONTS visible texts
	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		this->m_textList[i]->SetPositionY(topY + i * FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		if(i >= MENU_NUMBER_OF_VISIBLE_FONTS)
			this->m_textList[i]->SetVisible(false);
	}
}

/**
* Refresh the menu
* @param time Elapsed time
*/
void CMenuEntity::Update(const float time)
{
	this->m_movedTimer += time;
	bool mouseOnText = false;

	// Change the active text by the mouse
	if(this->m_controlM->HasMouseMoved())
	{
		for(int i = 0 ; i < this->m_actionSize && !mouseOnText ; i++)
		{
			if(this->m_textList[i]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))
			{
				mouseOnText = true;
				this->m_actionChoosen = i;
			}
		}
	}

	int mouseWheel = this->m_controlM->GetMouseWheelMovement();
	if(mouseWheel > 0 && this->m_topFontId > 0)
	{
		ShiftText(MENU_ENTITY_SHIFT_BOTTOM);
	}
	else
	{
		if(mouseWheel < 0 && this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS < this->m_actionSize)
			ShiftText(MENU_ENTITY_SHIFT_TOP);
	}

	// If the mouse hasn't changed a text, we test the other controls
	if(!mouseOnText)
	{
		// Move up the choosen text
		if(this->m_controlM->IsKeyDown(ACTION_KEY_UP_MENU) && this->m_movedTimer >= MENU_ENTITY_DELAY)
		{
			this->m_actionChoosen--;
			if(this->m_actionChoosen < 0)
				this->m_actionChoosen = 0;

			// If the user select a text on the non-visible upper part
			if(this->m_actionChoosen < this->m_topFontId)
			{
				// Shift the texts to one element to the bottom and update the visible texts
				ShiftText(MENU_ENTITY_SHIFT_BOTTOM);
			}

			this->m_movedTimer = 0;
		}
		else
		{
			// Move down the choosen text
			if(this->m_controlM->IsKeyDown(ACTION_KEY_DOWN_MENU) && this->m_movedTimer >= MENU_ENTITY_DELAY)
			{
				this->m_actionChoosen++;
				if(this->m_actionChoosen >= this->m_actionSize)
					this->m_actionChoosen = this->m_actionSize - 1;

				// If the user select a text on the non-visible bottom part
				if((this->m_actionChoosen - this->m_topFontId) >= MENU_NUMBER_OF_VISIBLE_FONTS)
				{
					// Shift the texts to one element to the top and update the visible texts
					ShiftText(MENU_ENTITY_SHIFT_TOP);
				}

				this->m_movedTimer = 0;
			}
		}
	}

	// Update the active text
	for(int i = 0 ; i < this->m_actionSize ; i++)
		this->m_textList[i]->SetActive(false);
	this->m_textList[this->m_actionChoosen]->SetActive(true);
}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CMenuEntity::ValidateChanges(const float time)
{
	// Validation
	if(this->m_actionSize > 0)
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || (this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))) && this->m_textList[this->m_actionChoosen]->IsEnabled())
			this->m_validationCode = this->m_actionList[this->m_actionChoosen];
	}

	// Wait to avoid too much fps
	PerformWait(time);
}

/**
* Draw the menu
*/
void CMenuEntity::Render()
{
	this->m_sprite->Render();
	for(int i = 0 ; i < this->m_actionSize ; i++)
		this->m_textList[i]->Render();

	// Render only the visible part
	if(this->m_actionSize >= MENU_NUMBER_OF_VISIBLE_FONTS)
	{
		if(this->m_topFontId > 0)
			this->m_topArrow->Render();

		if(this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS < this->m_actionSize)
			this->m_bottomArrow->Render();
	}

	if(this->m_headerText != NULL)
		this->m_headerText->Render();

	if(this->m_footerText != NULL)
		this->m_footerText->Render();
}

/**
* Perform a sleep to prevent having too high fps in the menus
* @param time Elapsed time
*/
void CMenuEntity::PerformWait(const float time)
{
	int sleep = (int)(MENU_DELAY_TIMER - time);
	if(sleep > 0)
		Sleep(sleep);
}

/**
* Shift the text vertically
* @param way The way of the shift, MENU_ENTITY_SHIFT_BOTTOM or MENU_ENTITY_SHIFT_TOP
*/
void CMenuEntity::ShiftText(const int way)
{
	if(way == MENU_ENTITY_SHIFT_BOTTOM)
	{
		this->m_topFontId--;
		for(int i = 0 ; i < this->m_actionSize ; i++)
		{
			this->m_textList[i]->SetPositionY(this->m_textList[i]->GetPositionY() + (FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)));
			if(i >= this->m_topFontId && i < this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS)
				this->m_textList[i]->SetVisible(true);
			else
				this->m_textList[i]->SetVisible(false);
		}

		if(this->m_actionChoosen >= this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS)
			this->m_actionChoosen = this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS - 1;
	}
	else
	{
		if(way == MENU_ENTITY_SHIFT_TOP)
		{
			this->m_topFontId++;
			for(int i = 0 ; i < this->m_actionSize ; i++)
			{
				this->m_textList[i]->SetPositionY(this->m_textList[i]->GetPositionY() - (FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)));
				if(i >= this->m_topFontId && i < this->m_topFontId + MENU_NUMBER_OF_VISIBLE_FONTS)
					this->m_textList[i]->SetVisible(true);
				else
					this->m_textList[i]->SetVisible(false);
			}

			if(this->m_actionChoosen < this->m_topFontId)
				this->m_actionChoosen = this->m_topFontId;
		}
	}
}

/**
* Set the header text
* @param font The font texture name
* @param text The text to display
*/
void CMenuEntity::SetHeader(std::string font, std::string text)
{
	SAFE_DELETE(this->m_headerText);
	this->m_headerText = new CFontEntity(font, text, 0.9f,  FONT_ALIGN_CENTER, (int)(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2.0f), (int) (this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * 0.2f), 0xFF8888AA, 0xFFFF0000);
}

/**
* Set the footer text
* @param font The font texture name
* @param text The text to display
*/
void CMenuEntity::SetFooter(std::string font, std::string text)
{
	SAFE_DELETE(this->m_footerText);
	this->m_footerText = new CFontEntity(font, this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_NEED_REBOOT), 0.6f,  FONT_ALIGN_LEFT, (int)(FONT_CHARACTER_SCREENSIZE_WIDTH * this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE)), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) - (int)(FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)), 0xFFFFFFFF, 0xFFFF0000);
}
