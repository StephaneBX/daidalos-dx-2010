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

#include "../Game/CPlayer.h"
#include "../Game/CFlashlight.h"
#include "../Engine/Physic/CEllipse.h"
#include "../Engine/Physic/CMovement.h"
#include "../Engine/Entities/Sound/CEffectSoundEntity.h"
#include "../Managers/CDeviceManager.h"
#include "../Managers/CControlManager.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CModManager.h"
#include "../Managers/CCameraManager.h"

/**
* Constructor
* @param position The position
* @param rotation The rotation
* @param health The starting health
* @param armor The starting armor
*/
CPlayer::CPlayer(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation, const int health = PLAYER_RESPAWN_HEALTH, const int armor = PLAYER_RESPAWN_ARMOR)
{
	this->m_controlM = CControlManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_camera = CCameraManager::GetInstance()->GetCamera();

	this->m_hasFlashlight = false;
	this->m_shape = new CEllipse(position, D3DXVECTOR3(PLAYER_SHAPE_RADIUS, PLAYER_HEIGHT, PLAYER_SHAPE_RADIUS));
	this->m_movement = new CMovement(D3DXVECTOR3(PLAYER_MOVE_STAND_SPEED, 0, PLAYER_MOVE_STAND_SPEED));
	this->m_gravityMovement = new CMovement(D3DXVECTOR3(0, 0, 0));

	SetRespawnPoint(position, rotation);

	this->m_flashlight = new CFlashlight();
	this->m_flashlight->Switch();

	this->m_flashlightSound = CModManager::GetInstance()->LoadEffect("Player/flashlight.wav");
	this->m_jumpSound = CModManager::GetInstance()->LoadEffect("Player/jump.wav");
	this->m_hurtSound = CModManager::GetInstance()->LoadEffect("Player/hurt.wav");

	this->m_steps[0] = CModManager::GetInstance()->LoadEffect("Player/step_1.wav");
	this->m_steps[1] = CModManager::GetInstance()->LoadEffect("Player/step_2.wav");

	this->m_flashlightSound->SetRelativeToListener(true);
	this->m_jumpSound->SetRelativeToListener(true);
	this->m_hurtSound->SetRelativeToListener(true);
	this->m_steps[0]->SetRelativeToListener(true);
	this->m_steps[1]->SetRelativeToListener(true);

	Respawn(health, armor);
}

/**
* Destructor
*/
CPlayer::~CPlayer()
{
	SAFE_DELETE(this->m_shape);
	SAFE_DELETE(this->m_movement);
	SAFE_DELETE(this->m_gravityMovement);
	SAFE_DELETE(this->m_flashlight);
}

/**
* Give a armor bonus to the player
* @param armorBonus The armor bonus
*/
void CPlayer::GiveArmor(const int armorBonus)
{
	this->m_armor += armorBonus;
	if(this->m_armor > PLAYER_MAX_ARMOR)
		this->m_armor = PLAYER_MAX_ARMOR;
	this->m_hud.UpdateArmor(this->m_armor);
}

/**
* Give a flashlight to the player
*/
void CPlayer::GiveFlashlight()
{
	this->m_hasFlashlight = true;
	this->m_flashlight->Switch();
	this->m_hud.SetFlashLightEnabled(true);
}

/**
* Give a health bonus to the player
* @param healthBonus The health bonus
*/
void CPlayer::GiveHealth(const int healthBonus)
{
	this->m_health += healthBonus;
	if(this->m_health > PLAYER_MAX_HEALTH)
		this->m_health = PLAYER_MAX_HEALTH;
	this->m_hud.UpdateHealth(this->m_health);
}

/**
* Give a Magnetic Card to the player
* @param cardId The card id
*/
void CPlayer::GiveMagneticCard(const int cardId)
{
	this->m_inventory.GiveMagneticCard(cardId);
	this->m_hud.SetMagneticCardEnabled(cardId);
}

/**
* Render the Hud
*/
void CPlayer::RenderHud()
{
	this->m_hud.Render();
}

/**
* Respawn the player to the origin position
*/
void CPlayer::Respawn(const int health = PLAYER_RESPAWN_HEALTH, const int armor = PLAYER_RESPAWN_ARMOR)
{
	this->m_hasMoved = false;
	this->m_alive = true;
	this->m_verticalMoveModification = -PI;
	this->m_movementState = PLAYER_STATE_STAND;
	this->m_camera->m_position = this->m_originPosition;
	this->m_rotation = this->m_originRotation;
	this->m_health = health;
	this->m_armor = armor;
	this->m_shape->SetPosition(&this->m_camera->m_position);
	this->m_shape->SetOffset(D3DXVECTOR3(PLAYER_SHAPE_RADIUS, PLAYER_HEIGHT, PLAYER_SHAPE_RADIUS));
	this->m_movement->SetMovement(D3DXVECTOR3(PLAYER_MOVE_STAND_SPEED, 0, PLAYER_MOVE_STAND_SPEED));
	this->m_gravityMovement->SetMovement(D3DXVECTOR3(0, 0, 0));
	this->m_previousPosition = this->m_originPosition;
	this->m_deathTimer = 0;
	this->m_damageTimer= 0;

	this->m_hud.UpdateHealth(this->m_health);
	this->m_hud.UpdateArmor(this->m_armor);
	this->m_hud.UpdateBloodAlpha(0);
}

/**
* Change the respawn point
*/
void CPlayer::SetRespawnPoint(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation)
{
	this->m_originPosition = position;
	this->m_originPosition.y += PLAYER_HEIGHT + 0.1f;
	this->m_originRotation = rotation;
}

/**
* Give damage
* @param damage The damage value
*/
void CPlayer::TakeDamage(const int damage)
{
	this->m_health -= damage;
	this->m_hurtSound->Play();
	if(this->m_health <= 0)
	{
		this->m_health = 0;
		this->m_deathTimer = 0;
		this->m_alive = false;
		this->m_shape->SetOffset(D3DXVECTOR3(PLAYER_SHAPE_RADIUS, PLAYER_HEIGHT_DEAD, PLAYER_SHAPE_RADIUS));
	}
	else
		this->m_damageTimer = PLAYER_DAMAGE_IMMUNITY_DELAY;
	
	this->m_hud.UpdateHealth(this->m_health);
}

/**
* Update the player
* @param time Elapsed time
*/
void CPlayer::Update(const float time)
{
	this->m_hasMoved = false;
	D3DXVECTOR3 direction = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 direction2 = D3DXVECTOR3(0,1,0);
	float timeSecond = time / 1000.0f;

	// Update when the player is dead
	if(!this->m_alive)
	{
		this->m_deathTimer += time;
		if(this->m_deathTimer > PLAYER_DEATH_TIME)
			this->m_deathTimer = PLAYER_DEATH_TIME;

		this->m_hud.UpdateBloodAlpha(this->m_deathTimer / (PLAYER_DEATH_TIME * 2));

		// Respawn the player if the fire key is pressed and the timer is over
		if((this->m_controlM->IsKeyDown(ACTION_KEY_FIRE) || this->m_controlM->IsMouseKeyDown(ACTION_KEY_FIRE)) && this->m_deathTimer == PLAYER_DEATH_TIME)
		{
			Respawn();
			this->m_controlM->FreezeKey(ACTION_KEY_FIRE);
		}
	}

	if(this->m_alive)
	{
		// Damage hud alpha update
		if(this->m_damageTimer > 0)
		{
			this->m_damageTimer -= time;
			if(this->m_damageTimer < 0)
				this->m_damageTimer = 0;
			if(this->m_damageTimer >= PLAYER_DAMAGE_IMMUNITY_DELAY / 2)
				this->m_hud.UpdateBloodAlpha((1 - ((this->m_damageTimer - (PLAYER_DAMAGE_IMMUNITY_DELAY / 2)) / (PLAYER_DAMAGE_IMMUNITY_DELAY / 2))) / 2.0f);
			else
				this->m_hud.UpdateBloodAlpha((this->m_damageTimer / (PLAYER_DAMAGE_IMMUNITY_DELAY / 2)) / 2.0f);
		}

		// Update the movement state
		if(this->m_movementState == PLAYER_STATE_STAND)
		{
			// Stand mode and push jump key -> Jump mode
			if(this->m_controlM->IsKeyDown(ACTION_KEY_JUMP))
			{
				this->m_gravityMovement->SetMovement(D3DXVECTOR3(0, PLAYER_JUMP_VERTICAL_SPEED, 0));
				this->m_movement->SetMovement(D3DXVECTOR3(PLAYER_JUMP_HORIZONTAL_SPEED, 0, PLAYER_JUMP_HORIZONTAL_SPEED));
				this->m_movementState = PLAYER_STATE_JUMP;
				this->m_controlM->FreezeKey(ACTION_KEY_JUMP);
				this->m_jumpSound->Play();
			}
			/*else
			{
				// Stand mode and push crouch key -> Crouch mode
				if(this->m_controlM->IsKeyDown(ACTION_KEY_CROUCH))
				{
					this->m_shape->SetOffset(D3DXVECTOR3(0.7f, PLAYER_HEIGHT_CROUCH, 0.7f));
					this->m_movement->SetMovement(D3DXVECTOR3(PLAYER_MOVE_CROUCH_SPEED, 0, PLAYER_MOVE_CROUCH_SPEED));
					this->m_movementState = PLAYER_STATE_CROUCH;
					this->m_controlM->FreezeKey(ACTION_KEY_CROUCH);
				}
			}*/
		}
		else
		{
			// Jump mode and touched the ground -> Stand mode
			if(this->m_movementState == PLAYER_STATE_JUMP && this->m_gravityMovement->IsRestored())
			{
				this->m_movementState = PLAYER_STATE_STAND;
				this->m_movement->SetMovement(D3DXVECTOR3(PLAYER_MOVE_STAND_SPEED, 0, PLAYER_MOVE_STAND_SPEED));
			}
			else
			{
				// Crouch mode and push crouch key -> Stand mode
				if(this->m_movementState == PLAYER_STATE_CROUCH && this->m_controlM->IsKeyDown(ACTION_KEY_CROUCH))
				{
					this->m_shape->SetOffset(D3DXVECTOR3(PLAYER_SHAPE_RADIUS, PLAYER_HEIGHT, PLAYER_SHAPE_RADIUS));
					this->m_movement->SetMovement(D3DXVECTOR3(PLAYER_MOVE_STAND_SPEED, 0, PLAYER_MOVE_STAND_SPEED));
					this->m_gravityMovement->SetMovement(D3DXVECTOR3(0, PLAYER_CROUCH_TO_STAND_IMPULSE, 0));
					this->m_movementState = PLAYER_STATE_STAND;
					this->m_controlM->FreezeKey(ACTION_KEY_CROUCH);
				}
			}
		}
	}

	// Rotation update
	if(this->m_controlM->IsKeyDown(ACTION_KEY_TURN_LEFT))
	{
		this->m_rotation.y -= PLAYER_TURN_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_TURN_LEFT);
	}

	if(this->m_controlM->IsKeyDown(ACTION_KEY_TURN_RIGHT))
	{
		this->m_rotation.y += PLAYER_TURN_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_TURN_RIGHT);
	}

	if(this->m_alive)
	{
		// Movement update
		if(this->m_controlM->IsKeyDown(ACTION_KEY_FORWARD))
		{
			direction.x += sin(this->m_rotation.y * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_FORWARD);
			direction.z += cos(this->m_rotation.y * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_FORWARD);
			if(this->m_controlM->GetKeyRatio(ACTION_KEY_FORWARD) > 0)
				this->m_hasMoved = true;
		}

		if(this->m_controlM->IsKeyDown(ACTION_KEY_BACKWARD))
		{
			direction.x += -sin(this->m_rotation.y * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_BACKWARD);
			direction.z += -cos(this->m_rotation.y * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_BACKWARD);
			if(this->m_controlM->GetKeyRatio(ACTION_KEY_BACKWARD) > 0)
				this->m_hasMoved = true;
		}

		if(this->m_controlM->IsKeyDown(ACTION_KEY_STRAFE_LEFT))
		{
			direction.x += -sin((this->m_rotation.y + 90.0f) * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_LEFT);
			direction.z += -cos((this->m_rotation.y + 90.0f) * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_LEFT);
			if(this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_LEFT) > 0)
				this->m_hasMoved = true;
		}

		if(this->m_controlM->IsKeyDown(ACTION_KEY_STRAFE_RIGHT))
		{
			direction.x += -sin((this->m_rotation.y - 90.0f) * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_RIGHT);
			direction.z += -cos((this->m_rotation.y - 90.0f) * PI_OVER_180) * this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_RIGHT);
			if(this->m_controlM->GetKeyRatio(ACTION_KEY_STRAFE_RIGHT) > 0)
				this->m_hasMoved = true;
		}

		if(this->m_controlM->IsKeyDown(ACTION_KEY_FLASHLIGHT) && this->m_hasFlashlight)
		{
			this->m_flashlight->Switch();
			this->m_flashlightSound->Play();
			this->m_controlM->FreezeKey(ACTION_KEY_FLASHLIGHT);
		}
	}

	// Update the movement. We normalize the direction to uniformize the speed movement if it exceed a length of 1
	if(D3DXVec3Length(&direction) > 1.0f)
		D3DXVec3Normalize(&direction, &direction);

	this->m_movement->Update(&direction, timeSecond);
	this->m_gravityMovement->Update(&direction2, timeSecond);

	// Impulse modifier -> Need to be deleted and implemented for the weapons instead
	if(this->m_hasMoved && this->m_movementState == PLAYER_STATE_STAND)
	{
		this->m_verticalMoveModification += PI * PLAYER_VERTICAL_MODIFIER_SPEED * timeSecond * 1.5f * D3DXVec3Length(&direction);
		if(this->m_verticalMoveModification > PI)
		{
			this->m_verticalMoveModification -= PI * 2.0f;
			this->m_steps[rand() % 2]->Play();
		}
	}

	// View update
	if(this->m_controlM->GetMouseX() != 0 || this->m_controlM->GetMouseY() != 0)
	{
		this->m_rotation.y += this->m_controlM->GetMouseX() * PLAYER_MOUSE_ROTATION_SPEED * timeSecond * this->m_controlM->GetMouseSensibility();
		this->m_rotation.x += this->m_controlM->GetMouseY() * PLAYER_MOUSE_ROTATION_SPEED * timeSecond * this->m_controlM->GetMouseSensibility();
		this->m_rotation.z += this->m_controlM->GetMouseY() * PLAYER_MOUSE_ROTATION_SPEED * timeSecond * this->m_controlM->GetMouseSensibility();
	}
	else
	{
		if(this->m_controlM->IsKeyDown(ACTION_KEY_LOOK_UP))
		{
			this->m_rotation.x -= PLAYER_GAMEPAD_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_LOOK_UP) * this->m_controlM->GetGamepadSensibility();
			this->m_rotation.z -= PLAYER_GAMEPAD_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_LOOK_UP) * this->m_controlM->GetGamepadSensibility();
		}
		else
		{
			if(this->m_controlM->IsKeyDown(ACTION_KEY_LOOK_DOWN))
			{
				this->m_rotation.x += PLAYER_GAMEPAD_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_LOOK_DOWN) * this->m_controlM->GetGamepadSensibility();
				this->m_rotation.z += PLAYER_GAMEPAD_ROTATION_SPEED * timeSecond * this->m_controlM->GetKeyRatio(ACTION_KEY_LOOK_DOWN) * this->m_controlM->GetGamepadSensibility();
			}
		}
	}

	if(this->m_controlM->IsKeyDown(ACTION_KEY_CENTER_VIEW))
	{
		this->m_rotation.x = 0;
		this->m_rotation.z = 0;
		this->m_controlM->FreezeKey(ACTION_KEY_CENTER_VIEW);
	}

	// Perform valid range verification
	if(this->m_rotation.x >= PLAYER_MAX_VERTICAL_ANGLE)
		this->m_rotation.x = PLAYER_MAX_VERTICAL_ANGLE;

	if(this->m_rotation.x < -PLAYER_MAX_VERTICAL_ANGLE)
		this->m_rotation.x = -PLAYER_MAX_VERTICAL_ANGLE;

	if(this->m_rotation.y >= 360.0f)
		this->m_rotation.y -= 360.0f;

	if(this->m_rotation.y < 0)
		this->m_rotation.y += 360.0f;

	if(this->m_rotation.z >= PLAYER_MAX_VERTICAL_ANGLE)
		this->m_rotation.z = PLAYER_MAX_VERTICAL_ANGLE;

	if(this->m_rotation.z < -PLAYER_MAX_VERTICAL_ANGLE)
		this->m_rotation.z = -PLAYER_MAX_VERTICAL_ANGLE;

	this->m_camera->UpdateLook(&this->m_rotation);

}

/**
* Update the camera
*/
void CPlayer::UpdateCamera()
{
	D3DXVECTOR3 tmpPos = *this->m_shape->GetPosition();
	tmpPos.y += POSITION_Y_FACTOR - sin(this->m_verticalMoveModification) * 0.1f;;
	this->m_camera->UpdatePosition(tmpPos);

	// Test if the gravity has been restored (wich mean that the player has been stoped by a ceiling or a ground)
	if(this->m_gravityMovement->IsRestored())
	{
		float fallDistance = abs(this->m_previousPosition.y - this->m_shape->GetPosition()->y);
		float gravityFall = -this->m_gravityMovement->GetPreviousMovement().y;
		if(gravityFall > PLAYER_HEIGHT_DAMAGE_MINIMUM_GRAVITY && fallDistance > PLAYER_HEIGHT_DAMAGE_MINIMUM_HEIGHT)
			TakeDamage((int)(PLAYER_HEIGHT_DAMAGE_RATIO * (gravityFall - PLAYER_HEIGHT_DAMAGE_MINIMUM_GRAVITY)));
	}

	this->m_flashlight->Update(&this->m_camera->m_position, &this->m_rotation);

	// Camera frustum update
	this->m_camera->UpdateCamera();

	// Update the listener position
	this->m_modM->UpdateListenerPosition(this->m_shape->GetPosition()->x, this->m_shape->GetPosition()->y, -this->m_shape->GetPosition()->z);

	// Update the listener direction
	float listenerDirection[6];
	listenerDirection[0] = this->m_camera->GetForward().x;
	listenerDirection[1] = this->m_camera->GetForward().y;
	listenerDirection[2] = -this->m_camera->GetForward().z;
	listenerDirection[3] = 0;
	listenerDirection[4] = 1;
	listenerDirection[5] = 0;
	this->m_modM->UpdateListenerDirection(listenerDirection);
}

/**
* Test if the player has a flashlight
* @return True if the player possess a flashlight, false otherwise
*/
const bool CPlayer::HasFlashlight() const
{
	return this->m_hasFlashlight;
}

/**
* Test if the player has a magnetic card
* @param cardId The magnetic card id
* @return True if the player possess the magnetic card, false otherwise
*/
const bool CPlayer::HasMagneticCard(const int cardId) const
{
	return this->m_inventory.HasMagneticCard(cardId);
}

/**
* Get the player armor
* @return The player armor
*/
const int CPlayer::GetArmor() const
{
	return this->m_armor;
}

/**
* Get the player health
* @return The player health
*/
const int CPlayer::GetHealth() const
{
	return this->m_health;
}

/**
* Get the gravity movement of the player
* @return The player gravity movement
*/
CMovement * CPlayer::GetGravityMovement() const
{
	return this->m_gravityMovement;
}

/**
* Get the movement of the player
* @return The player movement
*/
CMovement * CPlayer::GetMovement() const
{
	return this->m_movement;
}

/**
* Get the shape of the player
* @return The player shape
*/
CShape * CPlayer::GetShape() const
{
	return this->m_shape;
}
