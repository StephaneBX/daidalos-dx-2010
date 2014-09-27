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
#include "../../Engine/Render/CScenePlane.h"

/**
* Constructor
*/
CScenePlane::CScenePlane(){}

/**
* Constructor
* @param plane A plane reference
*/
CScenePlane::CScenePlane(CScenePlane * const plane)
{
	this->m_normalVector = plane->m_normalVector;
	this->m_constant = plane->m_constant;
}

/**
* Constructor
* @param vertex A vertex list that is used to create the plane
*/
CScenePlane::CScenePlane(const D3DXVECTOR3 vertex[VERTEX_PER_FACE])
{
	BuildPlane(vertex);
}

/**
* Destructor
*/
CScenePlane::~CScenePlane(){}

/**
* = Operator overrides
*/
CScenePlane & CScenePlane::operator =(const CScenePlane & plane)
{
	this->m_normalVector = plane.m_normalVector;
	this->m_constant = plane.m_constant;
	return *this;
}

/**
* Inverse the plane equation
*/
void CScenePlane::Inverse()
{
	this->m_normalVector *= -1;
}

/**
* Perform a distance calculation between the plane and a point
* @param point The point
* @return A positive value if the point is in front of the plane, a negative value if the point is behind the plane or a nul value if the point is on the plane
*/
const float CScenePlane::PointDistance(const D3DXVECTOR3 * const point)
{
	return this->m_constant + D3DXVec3Dot(&this->m_normalVector, point);
}

/**
* Classify a point
* @param point The point to test
* @return The classify result, that can be SCENE_VERTEX_ORIENTATION_ON_PLANE, SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE or SCENE_VERTEX_ORIENTATION_NEGATIVE_SIDE
*/
const int CScenePlane::ClassifyPoint(const D3DXVECTOR3 * const point)
{
	float result = PointDistance(point);
	int orientation = SCENE_VERTEX_ORIENTATION_ON_PLANE;
	if(result > EPSILON)
			orientation = SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE;
		else
			if(result < -EPSILON)
				orientation = SCENE_VERTEX_ORIENTATION_NEGATIVE_SIDE;

	return orientation;
}

/**
* Intersect a segment with the plane a find the intersection point
* The function suppose that the segment IS NOT PARALLEL to the plane
* @param p1 First poit of the segment
* @param p2 Second point of the segment
* @param k The intersection factor
* @return The intersection point
*/
D3DXVECTOR3 CScenePlane::IntersectSegment(const D3DXVECTOR3 * const p1, const D3DXVECTOR3 * const p2, float * const k) const
{
	D3DXVECTOR3 u, result;
	u = *p1 - *p2;

	*k = (this->m_normalVector.x * p1->x + this->m_normalVector.y * p1->y + this->m_normalVector.z * p1->z + this->m_constant) / (this->m_normalVector.x * u.x + this->m_normalVector.y * u.y + this->m_normalVector.z * u.z);

	result.x = p1->x - *k * u.x;
	result.y = p1->y - *k * u.y;
	result.z = p1->z - *k * u.z;

	return result;
}

/**
* Build the plane
* @param vertex A vector that contains 3 points
*/
void CScenePlane::BuildPlane(const D3DXVECTOR3 vertex[VERTEX_PER_FACE])
{
	// Normal calculation
	D3DXVECTOR3 ab = vertex[1] - vertex[0];
	D3DXVECTOR3 ac = vertex[2] - vertex[0];
	D3DXVec3Cross(&this->m_normalVector, &ab, &ac);
	D3DXVec3Normalize(&this->m_normalVector, &this->m_normalVector);

	// Constant calculation
	this->m_constant = -(vertex[1].x * this->m_normalVector.x + vertex[1].y * this->m_normalVector.y + vertex[1].z * this->m_normalVector.z);
}
