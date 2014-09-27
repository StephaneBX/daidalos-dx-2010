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
#include "../../Engine/Physic/CSphere.h"
#include "../../Managers/COptionManager.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Engine/Render/CLight.h"

/**
* Constructor
*/
CLight::CLight()
{
	this->m_highPriority = false;
	this->m_enabled = false;
	this->m_shadowCaster = true;
	this->m_spotAngle = 0;
	this->m_spotLength = 0;
	this->m_boundingBox = new CSphere();
	this->m_optionM = COptionManager::GetInstance();
}

/**
* Destructor
*/
CLight::~CLight()
{
	SAFE_DELETE(this->m_boundingBox);
}

/**
* Update the light with camera information
* @param camera The camera 
*/
void CLight::Update(CSceneCamera * const camera)
{
	D3DXVECTOR3 lightCameraVector = *this->m_boundingBox->GetPosition() - camera->m_position;
	D3DXVec3Normalize(&lightCameraVector, &lightCameraVector);
	D3DXVECTOR3 length = *this->m_boundingBox->GetPosition() - camera->m_position;

	if(this->m_highPriority)
	{
		// High priority lights have the smallest score. The only light that uses this property is the player light. Use this property only when necessary.
		this->m_score = LIGHT_SCORE_HIGH_PRIORITY;
	}
	else
	{
		if(!camera->FrustumCull(this->m_boundingBox))
		{
			// The light is visible in the camera frustum
			float angle = (180.0f * acos(D3DXVec3Dot(&camera->m_forward, &lightCameraVector)) / PI);
			this->m_score = D3DXVec3Length(&length) - this->m_spotLength + D3DXVec3Length(&length) * (angle / (camera->GetFov() + this->m_spotAngle));
		}
		else
		{
			// The light is out of the camera frustum
			this->m_score = LIGHT_SCORE_INFINITY;
		}
	}
}

/**
* Update the light position
* @param position The new position
* @param direction The new direction
*/
void CLight::UpdatePosition(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction)
{
	this->m_boundingBox->SetPosition(*position);
	this->m_direction = *direction;

	UpdateView();
}

/**
* Initialize the light
* @param position The position
* @param direction The direction
* @param color The color
* @param angle The angle
* @param length The length
* @param shadowCaster Cast shadow
*/
void CLight::Initialize(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction, const D3DXVECTOR4 * const color, const float * angle, const float * length, bool * shadowCaster)
{
	this->m_boundingBox->SetPosition(*position);
	this->m_direction = *direction;
	this->m_color = *color;
	this->m_spotAngle = *angle;
	this->m_spotLength = *length;
	this->m_shadowCaster = *shadowCaster;

	UpdateView();

	this->m_enabled = true;
}

/**
* Set the High Priority member
* @param value New high priority value
*/
void CLight::SetHighPriority(const bool value)
{
	this->m_highPriority = value;
}

/**
* Enable / Disable the light
* @param value The new value
*/
void CLight::SetEnabled(const bool value)
{
	this->m_enabled = value;
}

/**
* Set the position
* @param position The new position
*/
void CLight::SetPosition(const D3DXVECTOR3 * const position)
{
	this->m_boundingBox->SetPosition(*position);
	UpdateView();
}

/**
* Set the direction
* @param direction The new direction
*/
void CLight::SetDirection(const D3DXVECTOR3 * const direction)
{
	this->m_direction = *direction;
	UpdateView();
}

/**
* Set the color
* @param color The color
*/
void CLight::SetColor(const D3DXVECTOR4 * const color)
{
	this->m_color = *color;
}

/**
* Return if the light is a high priority type
* @return If the light is a high priority type
*/
bool * const CLight::IsHighPriority()
{
	return &this->m_highPriority;
}

/**
* Return if the light can cast shadows
* @return If the light can cast shadows
*/
bool * const CLight::IsShadowCaster()
{
	return &this->m_shadowCaster;
}

/**
* Return if the light is enabled
* @return If the light is enabled
*/
bool * const CLight::IsEnabled()
{
	return &this->m_enabled;
}

/**
* Return if the light is visible
* @return If the light is visible
*/
bool const CLight::IsVisible()
{
	return this->m_score != LIGHT_SCORE_INFINITY;
}

/**
* Get the spot angle
* @return The spot angle
*/
float * const CLight::GetSpotAngle()
{
	return &this->m_spotAngle;
}

/**
* Get the spot length
* @return The spot length
*/
float * const CLight::GetSpotLength()
{
	return &this->m_spotLength;
}

/**
* Get the score result
* The more close the light is to the camera, the better is the score
* It's used to decide which light to activate
*/
float * const CLight::GetScore()
{
	return &this->m_score;
}

/**
* Get the position
* @return The position
*/
const D3DXVECTOR3 * const CLight::GetPosition()
{
	return this->m_boundingBox->GetPosition();
}

/**
* Get the direction
* @return The direction
*/
D3DXVECTOR3 * const CLight::GetDirection()
{
	return &this->m_direction;
}

/**
* Get the color
* @return The color
*/
D3DXVECTOR4 * const CLight::GetColor()
{
	return &this->m_color;
}

/**
* Get the view matrix
* @return The view matrix
*/
D3DXMATRIX * const CLight::GetViewProjMatrix()
{
	return &this->m_viewProjMatrix;
}

/**
* = Operator override
*/
CLight & CLight::operator =(const CLight & light)
{
	this->m_highPriority = light.m_highPriority;
	this->m_enabled = light.m_enabled;
	this->m_shadowCaster = light.m_shadowCaster;
	this->m_color = light.m_color;
	this->m_direction = light.m_direction;
	this->m_boundingBox->SetPosition(*light.m_boundingBox->GetPosition());
	((CSphere*)this->m_boundingBox)->SetOffset(light.m_boundingBox->GetOffset(NULL));
	this->m_spotAngle = light.m_spotAngle;
	this->m_spotLength = light.m_spotLength;
	this->m_viewMatrix = light.m_viewMatrix;
	this->m_projMatrix = light.m_projMatrix;
	this->m_viewProjMatrix = light.m_viewProjMatrix;
	this->m_score = light.m_score;
	return *this;
}

/**
* Update the view matrixes
* Create the viewMatrix, projMatrix and viewProjMatrix depending on the properties defined before.
*/
void CLight::UpdateView()
{
	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 at = *this->m_boundingBox->GetPosition() + this->m_direction;
	D3DXMatrixLookAtLH(&this->m_viewMatrix, this->m_boundingBox->GetPosition(), &at, &up);

	D3DXMatrixPerspectiveFovLH(&this->m_projMatrix, D3DXToRadian(this->m_spotAngle * 2.0f), 1, 0.5f, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
	D3DXMatrixMultiply(&this->m_viewProjMatrix, &this->m_viewMatrix, &this->m_projMatrix);

	D3DXVECTOR3 at2 = *this->m_boundingBox->GetPosition() + (this->m_direction * this->m_spotLength);
	at2 = *this->m_boundingBox->GetPosition() - at2;
	((CSphere*)this->m_boundingBox)->SetOffset(D3DXVec3Length(&at2));
}
