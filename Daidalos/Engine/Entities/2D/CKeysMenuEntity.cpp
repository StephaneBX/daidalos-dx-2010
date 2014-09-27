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

#include "../../../Engine/Entities/2D/CKeysMenuEntity.h"
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
CKeysMenuEntity::CKeysMenuEntity(const int actionSize, const int menuCode) : CMenuEntity(actionSize, menuCode)
{
	this->m_optionsList = new int[this->m_actionSize];
	this->m_optionsTextList = new std::string[this->m_actionSize];
	this->m_keyAssignmentTimer = 0;

	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		this->m_optionsList[i] = 0;
		this->m_optionsTextList[i] = "";
	}

	this->m_mode = KEYS_MENU_ENTITY_MODE_NAVIGATION;
}

/**
* Destructor
*/
CKeysMenuEntity::~CKeysMenuEntity()
{
	SAFE_DELETE_ARRAY(this->m_optionsList);
	SAFE_DELETE_ARRAY(this->m_optionsTextList);
}

/**
* Refresh the menu
* @param time Elapsed time
*/
void CKeysMenuEntity::Update(const float time){}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CKeysMenuEntity::ValidateChanges(const float time){}
