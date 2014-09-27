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
#include "../../../Engine/Physic/CShape.h"
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Render/CSceneCamera.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Entities/Sound/CEffectSoundEntity.h"
#include "../../../Engine/Entities/3D/CObjectEntitySwitch.h"

// Animation list
SObjectAnimation CObjectEntitySwitch::m_animationList[OBJECT_ANIMATION_SWITCH_SIZE] = 
{
	{0, 1, OBJECT_SWITCH_ANIMATION_FPS}, // OBJECT_ANIMATION_SWITCH_ACTIVATION
};

/**
* Constructor
* @param rotation The object rotation
* @param groupId The object groupId
* @param color The object's target color
*/
CObjectEntitySwitch::CObjectEntitySwitch(const D3DXVECTOR3 rotation, const int groupId, const int color) : CObjectEntity(rotation)
{
	this->m_alive = true;
	this->m_type = OBJECT_TYPE_SWITCH;
	this->m_state = OBJECT_STATE_SWITCH_OFF;
	this->m_activationType = OBJECT_ACTIVATE_ON_TOUCH;
	this->m_command.m_type = OBJECT_COMMAND_NOTHING;
	this->m_groupId = groupId;
	this->m_cardColor = color;
	this->m_actionSoundEffect = NULL;
}

/**
* Destructor
*/
CObjectEntitySwitch::~CObjectEntitySwitch(){}

/**
* Test if the switch can be activated by the player.
* If color != OBJECT_MAGNETIC_CARD_NONE, the player must have the correct magnetic card
* @return If the switch can be activated
*/
const bool CObjectEntitySwitch::CanBeActivated() const
{
	bool ret = true;
	if(this->m_cardColor != OBJECT_MAGNETIC_CARD_NONE)
		ret = this->m_player->HasMagneticCard(this->m_cardColor);
	return ret;
}

/**
* Update the object
* @param time Elapsed time
*/
void CObjectEntitySwitch::Update(const float time)
{
	if(this->m_render != NULL)
	{
		SObjectAnimation * currentAnimation = &this->m_animationList[OBJECT_ANIMATION_SWITCH_ACTIVATION];
		switch(this->m_state)
		{
		case OBJECT_STATE_SWITCH_ON:
			if(this->m_render->GetFrame() < currentAnimation->m_startFrame)
			{
				this->m_render->SetFrame(currentAnimation->m_startFrame);
				this->m_render->SetAnimationFPS(currentAnimation->m_fps);
				if(this->m_actionSoundEffect != NULL)
					this->m_actionSoundEffect->Play(this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, -this->m_visibleShape->GetPosition()->z);
			}

			// Update the animation
			this->m_render->Update(time);

			// Test the end of the animation
			if(this->m_render->GetFrame() > currentAnimation->m_endFrame)
				this->m_render->SetFrame(currentAnimation->m_endFrame);
			break;
		}
		
		FillGeometry();
	}
}

/**
* Fill the geometry buffer
*/
void CObjectEntitySwitch::FillGeometry()
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
