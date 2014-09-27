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

#include "../../Misc/Global.h"
#include "../../Engine/Physic/CMovement.h"

/**
* Constructor
* @param movement The movement
*/
CMovement::CMovement(const D3DXVECTOR3 movement)
{
	this->m_restored = false;
	this->m_previousMovement = movement;
	this->m_movement = movement;
	this->m_distance = D3DXVECTOR3(0,0,0);
}

/**
* Destructor
*/
CMovement::~CMovement(){}

/**
* Set the movement
* @param movement The new movement
*/
void CMovement::SetMovement(const D3DXVECTOR3 movement)
{
	this->m_movement = movement;
}

/**
* Update the movement
* @param direction The new direction
* @param time Elapsed time
*/
void CMovement::Update(const D3DXVECTOR3 * const direction, const float time)
{
	this->m_restored = false;
	this->m_distance.x = this->m_movement.x * direction->x * time;
	this->m_distance.z = this->m_movement.z * direction->z * time;
	this->m_movement.y += GRAVITY * direction->y * time;
	this->m_distance.y = this->m_movement.y * direction->y * time;
}

/**
* Reset the gravity
*/
void CMovement::ResetGravity()
{
	this->m_previousMovement = this->m_movement;
	this->m_movement.y = 0;
	this->m_restored = true;
}

/**
* Return if the gravity has been restored
* @return The restored status
*/
const bool CMovement::IsRestored() const
{
	return this->m_restored;
}

/**
* Get the previous  movement
* @return The previous movement
*/
const D3DXVECTOR3 CMovement::GetPreviousMovement() const
{
	return this->m_previousMovement;
}

/**
* Get the distance to perform by the movement
* @return The distance
*/
D3DXVECTOR3 * CMovement::GetDistance()
{
	return &this->m_distance;
}
