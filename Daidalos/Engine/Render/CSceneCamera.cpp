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
#include "../../Engine/Physic/CShape.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CSceneCamera.h"

/**
* Constructor
*/
CSceneCamera::CSceneCamera()
{
	this->m_lookOrigin = D3DXVECTOR3(0, 0, CAMERA_VIEW_POINT);
}

/**
* Destructor
*/
CSceneCamera::~CSceneCamera(){}

/**
* Update the position
* @param position The new position
*/
void CSceneCamera::UpdatePosition(const D3DXVECTOR3 position)
{
	this->m_position = position;
}

/**
* Update the forward view
* @param rotation The rotation
*/
void CSceneCamera::UpdateLook(const D3DXVECTOR3 * const rotation)
{
	D3DXMatrixRotationYawPitchRoll(&this->m_rotationMatrix, D3DXToRadian(rotation->y), D3DXToRadian(rotation->x), D3DXToRadian(rotation->z));
	D3DXVec3TransformCoord(&this->m_look, &this->m_lookOrigin, &this->m_rotationMatrix);
	this->m_look += this->m_position;
}

/**
* Set the view
* @param angle The half angle
* @param ratio The ratio
* @param nearDistance The near distance
* @param farDistance The far distance
*/
void CSceneCamera::SetView(const float angle, const float ratio, const float nearDistance, const float farDistance)
{
	this->m_frustum.SetFrustum(angle, ratio, nearDistance, farDistance);
}

/**
* Update the frustum
*/
void CSceneCamera::UpdateCamera()
{
	this->m_frustum.UpdateFrustum(&this->m_position, &this->m_look);
	this->m_forward = this->m_frustum.GetForward();
}

/**
* Perform a back-face culling test (test if the face is in front of the camera)
* @param face The face to test
*/
const bool CSceneCamera::BackfaceCull(CSceneFace * const face) const
{
	return this->m_frustum.BackfaceCull(face);
}

/**
* Perform a frustum culling test with the face to determin if it need to be discared for the rendering process
* The goal here is to verify the following restriction in order to be considered as visible:
* - The face is within the view range
* - The face is is the frustum represented by four planes
* @param shape The face shape that need to be tested
* @return True if the face need to be culled or false otherwise
*/
const bool CSceneCamera::FrustumCull(CShape * const shape)
{
	return this->m_frustum.FrustumCull(shape);
}

/**
* Get the fov
* @return The fov in degree
*/
const float CSceneCamera::GetFov() const
{
	return this->m_frustum.GetFov();
}

/**
* Get the position
* @return The position
*/
const D3DXVECTOR3 CSceneCamera::GetPosition() const
{
	return this->m_position;
}

/**
* Get the looking point
* @return The looking point
*/
const D3DXVECTOR3 CSceneCamera::GetLook() const
{
	return this->m_look;
}

/**
* Get the forward vector
* @return The forward vecctor
*/
const D3DXVECTOR3 CSceneCamera::GetForward() const
{
	return this->m_frustum.GetForward();
}

/**
* Get the frustum local points
* @return The frustum local points
*/
const D3DXVECTOR3 * CSceneCamera::GetFrustumLocalPoints() const
{
	return this->m_frustum.GetLocalSpacePoints();
}
