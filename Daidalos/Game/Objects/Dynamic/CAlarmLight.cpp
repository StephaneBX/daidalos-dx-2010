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

#include "../../../Engine/Physic/CSphere.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Managers/CLightManager.h"
#include "../../../Game/Objects/Dynamic/CAlarmLight.h"

/**
* Constructor
* @param position The position
* @param rotation The rotation
*/
CAlarmLight::CAlarmLight(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation) : CObjectEntity(rotation)
{
	this->m_visibleShape = new CSphere(position, 0);
	const D3DXVECTOR3 direction = D3DXVECTOR3(1,-1,0);
	const D3DXVECTOR4 color = D3DXVECTOR4(0.5f,0,0,1);
	const float angle = 50;
	const float length = 50;
	bool shadowCaster = true;
	this->m_light = CLightManager::GetInstance()->AddLight(this->m_visibleShape->GetPosition(), &direction, &color, &angle, &length, &shadowCaster);
}

/**
* Destructor
*/
CAlarmLight::~CAlarmLight(){}

/**
* Update the object
* @param time Elapsed time
*/
void CAlarmLight::Update(const float time)
{
	D3DXVECTOR3 direction = D3DXVECTOR3(0,0,0);
	this->m_rotation.y += ALARM_LIGHT_ROTATION_SPEED * time / 1000.0f;
	direction.x = sin(this->m_rotation.y * PI_OVER_180);
	direction.z = cos(this->m_rotation.y * PI_OVER_180);
	D3DXVec3Normalize(&direction, &direction);
	this->m_light->SetDirection(&direction);
}
