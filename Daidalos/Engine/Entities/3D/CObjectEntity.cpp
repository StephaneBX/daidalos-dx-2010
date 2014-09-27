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

#include "../../../Game/CPlayer.h"
#include "../../../Managers/CLightManager.h"
#include "../../../Managers/CLogManager.h"
#include "../../../Managers/CModManager.h"
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Physic/CShape.h"
#include "../../../Engine/Physic/CMovement.h"
#include "../../../Engine/Render/CSceneFace.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Entities/3D/CObjectEntity.h"

/**
* Constructor
* @param rotation The object rotation
*/
CObjectEntity::CObjectEntity(const D3DXVECTOR3 rotation)
{
	// Default to misc type (no interaction)
	this->m_alive = true;
	this->m_type = OBJECT_TYPE_MISC;
	this->m_state = OBJECT_STATE_MISC_IDLE;
	this->m_activationType = OBJECT_ACTIVATE_OFF;
	this->m_command.m_type = OBJECT_COMMAND_NOTHING;
	this->m_groupId = OBJECT_GROUP_ID_NONE;

	this->m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->m_rotation = rotation;

	this->m_visibleShape = NULL;
	this->m_activationShape = NULL;
	this->m_render = NULL;
	this->m_light = NULL;
	this->m_logM = CLogManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_next = NULL;
}

/**
* Destructor
*/
CObjectEntity::~CObjectEntity()
{
	SAFE_DELETE(this->m_visibleShape);
	SAFE_DELETE(this->m_activationShape);
	SAFE_DELETE(this->m_render);
}

/**
* Attach a player pointer to the object
* @param player A pointer to the player
*/
void CObjectEntity::LinkPlayer(CPlayer * const player)
{
	this->m_player = player;
}

/**
* Change the object state
* @param state The new state
*/
void CObjectEntity::SetState(const int state)
{
	this->m_state = state;
}

/**
* Change the object group
* @param groupId The group 
*/
void CObjectEntity::SetGroup(const int groupId)
{
	this->m_groupId = groupId;
}

/**
* Change the object timer
* @param timer The timer 
*/
void CObjectEntity::SetTimer(const float timer)
{
	this->m_timer = timer;
}

/**
* Update the object
* @param time Elapsed time
*/
void CObjectEntity::Update(const float time)
{
	FillGeometry(SHADING_TYPE_SMOOTH);
}

/**
* Test if the object is alive
* @return True if the object is alive, false otherwise
*/
const bool CObjectEntity::IsAlive()
{
	return this->m_alive;
}

/**
* Test if the player is colliding the object
* @return The collision test
*/
const bool CObjectEntity::IsCollidedByPlayer()
{
	bool ret = false;
	if(this->m_activationShape != NULL)
	{
		D3DXVECTOR3 pos = *this->m_activationShape->GetPosition() - *this->m_player->GetShape()->GetPosition();
		pos.y = 0;
		ret = (D3DXVec3Length(&pos) <= (this->m_activationShape->GetOffset(NULL) + this->m_player->GetShape()->GetOffset(NULL)));
	}
	return ret;
}

/**
* Get the object type
* @return The type
*/
const int CObjectEntity::GetType() const
{
	return this->m_type;
}

/**
* Get the object group
* @return The group
*/
const int CObjectEntity::GetGroup() const
{
	return this->m_groupId;
}

/**
* Get the object state
* @return The state
*/
const int CObjectEntity::GetState() const
{
	return this->m_state;
}

/**
* Get the command
* @return the command or {OBJECT_COMMAND_NOTHING,0} if no command is needed
*/
const SObjectCommand CObjectEntity::GetCommand() const
{
	return this->m_command;
}

/**
* Get the object rotation
* @return The object rotation
*/
D3DXVECTOR3 * CObjectEntity::GetRotation()
{
	return &this->m_rotation;
}

/**
* Get the shape that define the visibility of the object
* @return The object visible shape
*/
CShape * CObjectEntity::GetVisibleShape() const
{
	return this->m_visibleShape;
}

/**
* Get the shape that define the activation radius of the object
* @return The object activation shape
*/
CShape * CObjectEntity::GetActivationShape() const
{
	return this->m_activationShape;
}

/**
* Get the Render if on is attached to the object
* @return The render
*/
CEntityRender * CObjectEntity::GetRender() const
{
	return this->m_render;
}

/**
* Fill the geometry buffer
*/
void CObjectEntity::FillGeometry(const int shadingType)
{
	if(this->m_render != NULL)
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
				this->m_render->GetMesh()[k]->GeometrySourceCopy(shadingType);

				this->m_render->GetMesh()[k]->GeometryTransform(&this->m_rotationMatrix, true);
				this->m_render->GetMesh()[k]->GeometryTransform(&this->m_scalingMatrix, false);
				this->m_render->GetMesh()[k]->GeometryTransform(&this->m_translationMatrix, false);

				this->m_render->GetMesh()[k]->GeometryFinalize(this->m_render->GetCamera(), !this->m_render->GetCamera()->FrustumCull(this->m_visibleShape), shadingType);
			}
		}
		this->m_render->FinalizeRender();
	}
}
