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

#include "../../../Engine/Entities/2D/CIntroMenuEntity.h"
#include "../../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../../Engine/Entities/2D/CFontEntity.h"
#include "../../../Managers/COptionManager.h"
#include "../../../Managers/CControlManager.h"

/**
* Constructor
* @param previousMenu The previous menu code
*/
CIntroMenuEntity::CIntroMenuEntity(int previousMenu) : CMenuEntity(0, ID_INTRODUCTION)
{
	this->m_timer = 0;
}

/**
* Destructor
*/
CIntroMenuEntity::~CIntroMenuEntity(){}

/**
* Refresh the menu
* @param time Elapsed time
*/
void CIntroMenuEntity::Update(float time)
{
	this->m_timer += time;

	float gammaValue;

	// Change the gamma
	if(this->m_timer <= INTROMENU_FADE_TIME)
	{
		gammaValue = (float)this->m_timer / INTROMENU_FADE_TIME;
	}
	else
	{
		if(this->m_timer  >= INTROMENU_TIME - INTROMENU_FADE_TIME)
			gammaValue = 1.0f - ((float)(this->m_timer - INTROMENU_TIME + INTROMENU_FADE_TIME) / INTROMENU_FADE_TIME);
		else
			gammaValue = 1.0f;
	}

	this->m_sprite->SetColor(D3DXCOLOR(gammaValue, gammaValue, gammaValue, 1));

	if(this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) || this->m_controlM->IsKeyDown(ACTION_KEY_ESCAPE_MENU) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_ESCAPE_MENU) || this->m_timer >= INTROMENU_TIME)
		this->m_validationCode = ID_MAINMENU;

	// Wait to avoid too much fps
	PerformWait(time);
}
