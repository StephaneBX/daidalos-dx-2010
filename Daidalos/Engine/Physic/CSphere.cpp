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
#include "../../Engine/Physic/CSphere.h"

/**
* Constructor
*/
CSphere::CSphere(){}

/**
* Constructor
* @param position The sphere origin
* @param offset The sphere offset
*/
CSphere::CSphere(const D3DXVECTOR3 position, const float offset) : CShape(position)
{
	this->m_offset = D3DXVECTOR3(offset, offset, offset);
}

/**
* Destructor
*/
CSphere::~CSphere(){}

/**
* Set the offset
* @param offset The new offset
*/
void CSphere::SetOffset(const float offset)
{
	this->m_offset = D3DXVECTOR3(offset, offset, offset);
}

/**
* Get the offset
* @param plane The plane equation of the surface to calculate the offset with. Unecessary for the sphere as the offset is constant everywhere.
* @return The offset
*/
const float CSphere::GetOffset(const CScenePlane * const plane = NULL) const
{
	return this->m_offset.x;
}
