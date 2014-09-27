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

#include "../Game/CFlashlight.h"
#include "../Misc/Global.h"
#include "../Managers/CLightManager.h"

/**
* Constructor
*/
CFlashlight::CFlashlight()
{
	this->m_angle = FLASHLIGHT_DEFAULT_ANGLE;
	this->m_length = FLASHLIGHT_DEFAULT_LENGTH;
	this->m_color = FLASHLIGHT_DEFAULT_COLOR;
	this->m_shadowCaster = true;

	this->m_lookOrigin = D3DXVECTOR3(0, 0, FLASHLIGHT_VIEW_POINT);

	this->m_flashlight = CLightManager::GetInstance()->AddLight(&this->m_position, &this->m_direction, &this->m_color, &this->m_angle, &this->m_length, &this->m_shadowCaster);
	this->m_flashlight->SetHighPriority(true);
}

/**
* Destructor
*/
CFlashlight::~CFlashlight(){}

/**
* Update the flashlight
* @param position The new position
* @param rotation The new rotation
*/
void CFlashlight::Update(D3DXVECTOR3 * position, D3DXVECTOR3 * rotation)
{
	this->m_position = *position;
	this->m_position.y += FLASHLIGHT_POSITION;

	D3DXMatrixRotationYawPitchRoll(&this->m_rotationMatrix, D3DXToRadian(rotation->y), D3DXToRadian(rotation->x), D3DXToRadian(rotation->z));
	D3DXVec3TransformCoord(&this->m_look, &this->m_lookOrigin, &this->m_rotationMatrix);
	this->m_look += this->m_position;

	this->m_direction = this->m_look - this->m_position;
	D3DXVec3Normalize(&this->m_direction, &this->m_direction);

	this->m_flashlight->UpdatePosition(&this->m_position, &this->m_direction);
}

/**
* Change the flashlight state
* @param value The flashlight state
*/
void CFlashlight::SetEnabled(const bool value)
{
	this->m_flashlight->SetEnabled(value);
}

/**
* Switch the flashlight state
*/
void CFlashlight::Switch()
{
	this->m_flashlight->SetEnabled(!*this->m_flashlight->IsEnabled());
}
