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

#include "../../../Engine/Entities/3D/CObjectEntityDoor.h"
#include "../../../Engine/Physic/CShape.h"
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Render/CSceneCamera.h"
#include "../../../Engine/Render/CEntityRender.h"

// Animation list
SObjectAnimation CObjectEntityDoor::m_animationList[OBJECT_ANIMATION_DOOR_SIZE] = 
{
	{0, 1, OBJECT_DOOR_ANIMATION_FPS}, // OBJECT_ANIMATION_DOOR_OPENING
	{1, 2, OBJECT_DOOR_ANIMATION_FPS}  // OBJECT_ANIMATION_DOOR_CLOSING
};

/**
* Constructor
* @param rotation The object rotation
* @param groupId The object groupId
*/
CObjectEntityDoor::CObjectEntityDoor(const D3DXVECTOR3 rotation, const int groupId) : CObjectEntity(rotation)
{
	this->m_alive = true;
	this->m_type = OBJECT_TYPE_DOOR;
	this->m_state = OBJECT_STATE_DOOR_CLOSED;
	this->m_activationType = OBJECT_ACTIVATE_ON_TOUCH;
	this->m_command.m_type = OBJECT_COMMAND_NOTHING;
	this->m_groupId = groupId;
}

/**
* Destructor
*/
CObjectEntityDoor::~CObjectEntityDoor(){}


/**
* Update the object
* @param time Elapsed time
*/
void CObjectEntityDoor::Update(const float time)
{
	SObjectAnimation * currentAnimation;
	switch(this->m_state)
	{
	case OBJECT_STATE_DOOR_OPENING:
		currentAnimation = &this->m_animationList[OBJECT_ANIMATION_DOOR_OPENING];

		// The animation has just begun, we initialize the m_frame value and the fps
		if(this->m_render->GetFrame() < currentAnimation->m_startFrame || this->m_render->GetFrame() > currentAnimation->m_endFrame)
		{
			this->m_render->SetFrame(currentAnimation->m_startFrame);
			this->m_render->SetAnimationFPS(currentAnimation->m_fps);
		}

		// Update the animation
		this->m_render->Update(time);

		// Test the end of the animation
		if(this->m_render->GetFrame() >= currentAnimation->m_endFrame)
		{
			this->m_render->SetFrame(currentAnimation->m_endFrame);
			this->m_state = OBJECT_STATE_DOOR_OPENED;
			this->m_timer = 0;

			// Add the command to disable the associated brush for collision
			this->m_command.m_type = OBJECT_COMMAND_DISABLE_BRUSH;
			this->m_command.m_value = this->m_groupId;
		}

		break;

	case OBJECT_STATE_DOOR_CLOSING:
		currentAnimation = &this->m_animationList[OBJECT_ANIMATION_DOOR_CLOSING];

		// Update the animation
		this->m_render->Update(time);

		// Test the end of the animation
		if(this->m_render->GetFrame() >= currentAnimation->m_endFrame)
		{
			this->m_render->SetFrame(currentAnimation->m_endFrame);
			this->m_state = OBJECT_STATE_DOOR_CLOSED;
			this->m_timer = 0;
		}

		break;

	case OBJECT_STATE_DOOR_OPENED:
		this->m_timer += time;
		if(this->m_timer >= OBJECT_DOOR_TIME_OPEN)
		{
			currentAnimation = &this->m_animationList[OBJECT_ANIMATION_DOOR_CLOSING];

			this->m_state = OBJECT_STATE_DOOR_CLOSING;

			this->m_render->SetFrame(currentAnimation->m_startFrame);
			this->m_render->SetAnimationFPS(currentAnimation->m_fps);

			// Add the command to enable the associated brush for collision
			this->m_command.m_type = OBJECT_COMMAND_ENABLE_BRUSH;
			this->m_command.m_value = this->m_groupId;
		}
		break;
	}
	
	FillGeometry();
}

/**
* Fill the geometry buffer
*/
void CObjectEntityDoor::FillGeometry()
{
	int index = 0;
	this->m_render->ResetRender();

	D3DXMatrixScaling(&this->m_scalingMatrix, this->m_scale.x, this->m_scale.y, this->m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&this->m_rotationMatrix, D3DXToRadian(this->m_rotation.y), D3DXToRadian(this->m_rotation.x), D3DXToRadian(this->m_rotation.z));
	D3DXMatrixTranslation(&this->m_translationMatrix, this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, this->m_visibleShape->GetPosition()->z);

	for(int k = 0 ; k < MAX_MESH_PER_OBJECT ; k++)
	{
		if(this->m_render->GetMesh()[k] != NULL)
		{
			this->m_render->GetMesh()[k]->AnimateSourceCopy(this->m_render->GetFrame(), SHADING_TYPE_FLAT);

			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_rotationMatrix, true);
			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_scalingMatrix, false);
			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_translationMatrix, false);

			this->m_render->GetMesh()[k]->GeometryFinalize(this->m_render->GetCamera(), !this->m_render->GetCamera()->FrustumCull(this->m_visibleShape), SHADING_TYPE_FLAT);
		}
	}
	this->m_render->FinalizeRender();
}
