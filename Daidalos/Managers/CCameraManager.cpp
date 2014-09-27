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

#include <d3d9.h>
#include <d3dx9.h>
#include "../Managers/COptionManager.h"
#include "../Managers/CCameraManager.h"

/**
* Constructor
*/
CCameraManager::CCameraManager(){}

/**
* Destructor
*/
CCameraManager::~CCameraManager(){}

/**
* Refresh the martixes
*/
void CCameraManager::Update()
{
	D3DXMatrixLookAtLH(&this->m_viewMatrix, &this->m_camera.GetPosition(), &this->m_camera.GetLook(), &this->m_up);
	D3DXMatrixPerspectiveFovLH(&this->m_projMatrix, D3DXToRadian(CAMERAMANAGER_VERTICAL_FOV), this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / (float)this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 0.5f, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
	D3DXMatrixMultiply(&this->m_viewProjMatrix, &this->m_viewMatrix, &this->m_projMatrix);
}

/**
* Initialize the manager
*/
void CCameraManager::Initialize()
{
	this->m_optionM = COptionManager::GetInstance();
	this->m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	this->m_camera.SetView(this->m_optionM->GetFrustumAngle(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / (float)this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 0.5f, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
}

/**
* Get the View Matrix
* @return The view matrix
*/
D3DXMATRIX CCameraManager::GetViewMatrix() const
{
	return this->m_viewMatrix;
}

/**
* Get the Projection Matrix
* @return The projection matrix
*/
D3DXMATRIX CCameraManager::GetProjMatrix() const
{
	return this->m_projMatrix;
}

/**
* Get the View Proj Matrix
* @return the view prof matrix
*/
D3DXMATRIX CCameraManager::GetViewProjMatrix() const
{
	return this->m_viewProjMatrix;
}

/**
* Get the Eye position
* @return The eye position
*/
D3DXVECTOR3 CCameraManager::GetEye() const
{
	return this->m_eye;
}

CSceneCamera * CCameraManager::GetCamera()
{
	return &this->m_camera;
}
