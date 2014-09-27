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

#include "../../Engine/Physic/CImpact.h"

/**
* Constructor
*/
CImpact::CImpact()
{
	ClearImpact();
}

/**
* Destructor
*/
CImpact::~CImpact(){}

/**
* Initialize the impact
* @param startPoint The starting point of the element to test the impact with
* @param endPoint The ending point of the element to test the impact with
*/
void CImpact::Initialize(D3DXVECTOR3 * const startPoint, D3DXVECTOR3 * const endPoint)
{
	this->m_starPoint = *startPoint;
	this->m_endPoint = *endPoint;
	this->m_distance = this->m_endPoint - this->m_starPoint;
	ClearImpact();
}

/**
* Clear the impact
*/
void CImpact::ClearImpact()
{
	this->m_impactFraction = 1.0f;
	this->m_realFraction = 1.0f;
}

/**
* Set the impact
* @param planeNormal The normal of the plane of the impact
* @param impactFraction The fraction impact
* @param realFraction The real fraction
*/
void CImpact::SetImpact(D3DXVECTOR3 * const planeNormal, const float impactFraction, const float realFraction)
{
	this->m_planeNormal = *planeNormal;
	this->m_impactFraction = impactFraction;
	this->m_realFraction = realFraction;
}

/**
* Take the nearest impact between the current and the given
* @param planeNormal The normal of the plane of the impact
* @param impactFraction The fraction impact
* @param realFraction The real fraction
*/
void CImpact::SetNearerImpact(D3DXVECTOR3 * const planeNormal, const float impactFraction, const float realFraction)
{
	if(realFraction < this->m_realFraction)
		SetImpact(planeNormal, impactFraction, realFraction);
}

/**
* Test if an impact has occured
* @return The impact state
*/
const bool CImpact::IsImpact() const
{
	return this->m_realFraction < 1.0f;
}

/**
* Get the fraction impact
* @return The fraction impact
*/
const float CImpact::GetImpactFraction() const
{
	return this->m_impactFraction;
}

/**
* Get the real impact
* @return The real impact
*/
const float CImpact::GetRealFraction() const
{
	return this->m_realFraction;
}

/**
* Get the distance to perform
* @return The distance
*/
D3DXVECTOR3 * CImpact::GetDistance()
{
	return &this->m_distance;
}

/**
* Get the plane normal
* @return The plane normal
*/
D3DXVECTOR3 * CImpact::GetPlaneNormal()
{
	return &this->m_planeNormal;
}

/**
* Get the starting point
* @return The starting point
*/
D3DXVECTOR3 * CImpact::GetStartPoint()
{
	return &this->m_starPoint;
}

/**
* Get the ending point
* @return The ending point
*/
D3DXVECTOR3 * CImpact::GetEndPoint()
{
	return &this->m_endPoint;
}
