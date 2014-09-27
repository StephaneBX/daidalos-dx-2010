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
#include "../../Engine/Render/CFrustum.h"
#include "../../Engine/Physic/CShape.h"
#include "../../Engine/Render/CSceneFace.h"

/**
* Constructor
*/
CFrustum::CFrustum(){}

/**
* Destructor
*/
CFrustum::~CFrustum(){}

/**
* Set the frustum
* @param angle The half angle in degree
* @param ratio The screen ratio
* @param nearDistance The near distance
* @param farDistance The far distance
*/
void CFrustum::SetFrustum(float angle, float ratio, float nearDistance, float farDistance)
{
	this->m_angle = angle;
	this->m_ratio = ratio;
	this->m_nearDistance = nearDistance;
	this->m_farDistance = farDistance;

	this->m_tang = D3DXToRadian(this->m_angle * 0.5f);
	this->m_nearHeight = this->m_nearDistance * this->m_tang;
	this->m_nearWidth = this->m_nearHeight * this->m_ratio;
	this->m_farHeight = this->m_farDistance * this->m_tang;
	this->m_farWidth = this->m_farHeight * this->m_ratio;
}

/**
* Update the frustum
* @param position The new frustum's owner position
* @param look The new frustum's owner looking coordinate
*/
void CFrustum::UpdateFrustum(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const look)
{
	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 nearCenter;
	D3DXVECTOR3 farCenter;
	D3DXVECTOR3 x, y, z;

	this->m_position = *position;

	// Z axis camera
	z = *look - this->m_position;
	D3DXVec3Normalize(&z, &z);

	// X axis camera
	D3DXVec3Cross(&x, &up, &z);
	D3DXVec3Normalize(&x, &x);

	// Y axis camera
	D3DXVec3Cross(&y, &z, &x);
	D3DXVec3Normalize(&y, &y);

	// World space calculation

	// Find the center of the planes
	nearCenter = this->m_position + z * this->m_nearDistance;
	farCenter = this->m_position + z * this->m_farDistance;

	// Find the forward vector
	D3DXVECTOR3 dist = nearCenter - this->m_position;
	D3DXVec3Normalize(&this->m_forward, &dist);

	// Find the for points of the near plane
	this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_LEFT] = nearCenter + y * this->m_nearHeight - x * this->m_nearWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_RIGHT] = nearCenter + y * this->m_nearHeight + x * this->m_nearWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_LEFT] = nearCenter - y * this->m_nearHeight - x * this->m_nearWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_RIGHT] = nearCenter - y * this->m_nearHeight + x * this->m_nearWidth;

	// Find the for points of the far plane
	this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_LEFT] = farCenter + y * this->m_farHeight - x * this->m_farWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_RIGHT] = farCenter + y * this->m_farHeight + x * this->m_farWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_LEFT] = farCenter - y * this->m_farHeight - x * this->m_farWidth;
	this->m_worldSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_RIGHT] = farCenter - y * this->m_farHeight + x * this->m_farWidth;


	// Local space calculation

	// Find the center of the planes
	nearCenter = z * this->m_nearDistance;
	farCenter = z * this->m_farDistance;

	// Find the for points of the near plane
	this->m_localSpacePoints[FRUSTUM_POINT_NEAR_TOP_LEFT] = nearCenter + y * this->m_nearHeight - x * this->m_nearWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_NEAR_TOP_RIGHT] = nearCenter + y * this->m_nearHeight + x * this->m_nearWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_LEFT] = nearCenter - y * this->m_nearHeight - x * this->m_nearWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_RIGHT] = nearCenter - y * this->m_nearHeight + x * this->m_nearWidth;

	// Find the for points of the far plane
	this->m_localSpacePoints[FRUSTUM_POINT_FAR_TOP_LEFT] = farCenter + y * this->m_farHeight - x * this->m_farWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_FAR_TOP_RIGHT] = farCenter + y * this->m_farHeight + x * this->m_farWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_LEFT] = farCenter - y * this->m_farHeight - x * this->m_farWidth;
	this->m_localSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_RIGHT] = farCenter - y * this->m_farHeight + x * this->m_farWidth;


	// Table creation for the frustum planes
	D3DXVECTOR3 leftFrustrum[VERTEX_PER_FACE]   = {this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_LEFT]};
	D3DXVECTOR3 rightFrustrum[VERTEX_PER_FACE]  = {this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_RIGHT]};
	D3DXVECTOR3 topFrustrum[VERTEX_PER_FACE]    = {this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_RIGHT]};
	D3DXVECTOR3 bottomFrustrum[VERTEX_PER_FACE] = {this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_LEFT]};
	D3DXVECTOR3 nearFrustrum[VERTEX_PER_FACE]   = {this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_TOP_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_NEAR_BOTTOM_LEFT]};
	D3DXVECTOR3 farFrustrum[VERTEX_PER_FACE]    = {this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_LEFT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_TOP_RIGHT], this->m_worldSpacePoints[FRUSTUM_POINT_FAR_BOTTOM_RIGHT]};

	// Update the frustum planes
	this->m_frustum[FRUSTUM_PLANE_LEFT].BuildPlane(leftFrustrum);
	this->m_frustum[FRUSTUM_PLANE_RIGHT].BuildPlane(rightFrustrum);
	this->m_frustum[FRUSTUM_PLANE_TOP].BuildPlane(topFrustrum);
	this->m_frustum[FRUSTUM_PLANE_BOTTOM].BuildPlane(bottomFrustrum);
	this->m_frustum[FRUSTUM_PLANE_NEAR].BuildPlane(nearFrustrum);
	this->m_frustum[FRUSTUM_PLANE_FAR].BuildPlane(farFrustrum);
}


/**
* Perform a back-face culling test (test if the face is in front of the camera)
* @param face The face to test
*/
const bool CFrustum::BackfaceCull(CSceneFace * const face) const
{
	D3DXVECTOR3 dummy;
	int frontCounter = 0;
	int backCounter = 0;
	float result = face->m_forCulling - D3DXVec3Dot(&face->m_plane.m_normalVector, &this->m_position);

	if(result > EPSILON)
		return true;

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		dummy = face->m_geometry[i].m_position - this->m_position;
		result = D3DXVec3Dot(&this->m_forward, &dummy);

		if(result > EPSILON)
			frontCounter++;
		else
		{
			if(result < -EPSILON)
				backCounter++;
			else
				return false;
		}
	}

	if(frontCounter == VERTEX_PER_FACE)
		return false;
	else
		if(backCounter == VERTEX_PER_FACE)
			return true;

	return false;
}

/**
* Perform a frustum culling test with the face to determin if it need to be discared for the rendering process
* The goal here is to verify the following restriction in order to be considered as visible:
* - The face is within the view range
* - The face is is the frustum represented by four planes
* @param shape The face shape that need to be tested
* @return True if the face need to be culled or false otherwise
*/
const bool CFrustum::FrustumCull(CShape * const shape)
{
	bool result = false;

	for(int i = 0 ; i < FRUSTUM_PLANE_SIZE && !result ; i++)
	{
		float dist = this->m_frustum[i].PointDistance(shape->GetPosition());
		if(dist < -shape->GetOffset(NULL))
			result = true;
	}

	return result;
}

/**
* Get the fov
* @return The fov in degree
*/
const float CFrustum::GetFov() const
{
	return this->m_angle;
}

/**
* Get the forward vector
* @return The foward vector
*/
const D3DXVECTOR3 CFrustum::GetForward() const
{
	return this->m_forward;
}

/**
* Get the local space points of the frustum (used by the deferred shading to build the position informations)
* @return The local points
*/
const D3DXVECTOR3 * CFrustum::GetLocalSpacePoints() const
{
	return this->m_localSpacePoints;
}
