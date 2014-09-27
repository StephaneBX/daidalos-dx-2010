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

#include "../../../Engine/Entities/Sound/CEffectSoundEntity.h"
#include "../../../Engine/Entities/3D/CObjectEntityUsable.h"

/**
* Constructor
* @param rotation The object rotation
*/
CObjectEntityUsable::CObjectEntityUsable(const D3DXVECTOR3 rotation) : CObjectEntity(rotation)
{
	this->m_actionSoundEffect = NULL;
	this->m_alive = true;
	this->m_type = OBJECT_TYPE_USABLE;
	this->m_state = OBJECT_STATE_USABLE_UNUSED;
	this->m_activationType = OBJECT_ACTIVATE_ON_TOUCH;
	this->m_command.m_type = OBJECT_COMMAND_NOTHING;
	this->m_groupId = OBJECT_GROUP_ID_NONE;
	this->m_timer = 0;
}

/**
* Destructor
*/
CObjectEntityUsable::~CObjectEntityUsable(){}


/**
* Update the object. The function is different for each usable object
* @param time Elapsed time
*/
void CObjectEntityUsable::Update(const float time)
{
	this->m_rotation.y += (time * OBJECT_USABLE_ROTATION_SPEED) / 1000.0f;
	this->m_timer -= time;
	if(this->m_timer < 0)
		this->m_timer = 0;
}

/**
* Check if the entity can be used
* @return True if the entity can be used, false otherwise
*/
const bool CObjectEntityUsable::CanBeUsed()
{
	return (this->m_timer == 0);
}
