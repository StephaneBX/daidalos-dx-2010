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

#include "../../../Engine/Entities/2D/CModificationMenuEntity.h"
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
CModificationMenuEntity::CModificationMenuEntity(const int actionSize, const int menuCode) : CMenuEntity(actionSize, menuCode)
{
	this->m_choosenOptionsList = new int[this->m_actionSize];
	this->m_optionsListSize = new int[this->m_actionSize];
	this->m_optionsList = new int*[this->m_actionSize];
	this->m_optionsTextList = new std::string*[this->m_actionSize];

	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		this->m_choosenOptionsList[i] = 0;
		this->m_optionsListSize[i] = 0;
		this->m_optionsList[i] = NULL;
		this->m_optionsTextList[i] = NULL;
	}
}

/**
* Destructor
*/
CModificationMenuEntity::~CModificationMenuEntity()
{
	SAFE_DELETE_ARRAY(this->m_choosenOptionsList);
	SAFE_DELETE_ARRAY(this->m_optionsListSize);

	for(int i = 0 ; i < this->m_actionSize ; i++)
		SAFE_DELETE_ARRAY(this->m_optionsList[i]);
	SAFE_DELETE_ARRAY(this->m_optionsList);

	for(int i = 0 ; i < this->m_actionSize ; i++)
		SAFE_DELETE_ARRAY(this->m_optionsTextList[i]);
	SAFE_DELETE_ARRAY(this->m_optionsTextList);
}

/**
* Refresh the menu
* @param time Elapsed time
*/
void CModificationMenuEntity::Update(const float time)
{
	bool optionModified = false;
	CMenuEntity::Update(time);

	if((this->m_controlM->IsKeyDown(ACTION_KEY_RIGHT_MENU) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY())) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_movedTimer >= MENU_ENTITY_DELAY2 && this->m_actionList[this->m_actionChoosen] == ID_NONE)
	{
		optionModified = true;
		this->m_movedTimer = 0;
		this->m_choosenOptionsList[this->m_actionChoosen]++;
		if(this->m_choosenOptionsList[this->m_actionChoosen] >= this->m_optionsListSize[this->m_actionChoosen])
			this->m_choosenOptionsList[this->m_actionChoosen] = 0;
	}
	else
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_LEFT_MENU) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_ESCAPE_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY())) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_movedTimer >= MENU_ENTITY_DELAY2 && this->m_actionList[this->m_actionChoosen] == ID_NONE)
		{
			optionModified = true;
			this->m_movedTimer = 0;
			this->m_choosenOptionsList[this->m_actionChoosen]--;
			if(this->m_choosenOptionsList[this->m_actionChoosen] < 0)
				this->m_choosenOptionsList[this->m_actionChoosen] = this->m_optionsListSize[this->m_actionChoosen] - 1;
		}
	}

	// Update the modified option text if necessary
	if(optionModified)
		this->m_textList[this->m_actionChoosen]->SetText(this->m_optionsTextList[this->m_actionChoosen][this->m_choosenOptionsList[this->m_actionChoosen]]);
}

/**
* Draw the menu
*/
void CModificationMenuEntity::Render()
{
	CMenuEntity::Render();
}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CModificationMenuEntity::ValidateChanges(const float time)
{
	// Wait to avoid too much fps
	PerformWait(time);
}
