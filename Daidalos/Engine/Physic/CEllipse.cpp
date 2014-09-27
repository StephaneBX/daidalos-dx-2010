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

#include "../../Engine/Render/CScenePlane.h"
#include "../../Engine/Physic/CEllipse.h"

/**
* Constructor
*/
CEllipse::CEllipse(){}

/**
* Constructor
* @param position The ellipse origin
* @param offset The ellipse offset
*/
CEllipse::CEllipse(const D3DXVECTOR3 position, const D3DXVECTOR3 offset) : CShape(position)
{
	this->m_offset = offset;
}

/**
* Destructor
*/
CEllipse::~CEllipse(){}

/**
* Set the offset
* @param offset The new offset
*/
void CEllipse::SetOffset(const D3DXVECTOR3 offset)
{
	this->m_offset = offset;
}

/**
* Get the offset
* @param plane The plane equation of the surface to calculate the offset with.
* @return The offset
*/
const float CEllipse::GetOffset(const CScenePlane * const plane) const
{
	if(plane == NULL)
		return this->m_offset.x;
	else
		return this->m_offset.x * plane->m_normalVector.x * plane->m_normalVector.x + this->m_offset.y * plane->m_normalVector.y * plane->m_normalVector.y + this->m_offset.z * plane->m_normalVector.z * plane->m_normalVector.z;
}
