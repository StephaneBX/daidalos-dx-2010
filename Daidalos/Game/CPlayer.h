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

#ifndef __CPLAYER_H
#define __CPLAYER_H

#define PLAYER_HEIGHT 2.0f
#define PLAYER_HEIGHT_CROUCH 1.3f
#define PLAYER_HEIGHT_DEAD 0.5f
#define PLAYER_SHAPE_RADIUS 0.7f
#define PLAYER_MOVE_STAND_SPEED 12.0f
#define PLAYER_MOVE_CROUCH_SPEED 6.0f
#define PLAYER_JUMP_VERTICAL_SPEED 13.0f
#define PLAYER_JUMP_HORIZONTAL_SPEED 12.0f
#define PLAYER_CROUCH_TO_STAND_IMPULSE 5.0f
#define PLAYER_TURN_ROTATION_SPEED 50.0f
#define PLAYER_MOUSE_ROTATION_SPEED 20.0f
#define PLAYER_GAMEPAD_ROTATION_SPEED 60.0f
#define PLAYER_VERTICAL_MODIFIER_SPEED 4.0f
#define PLAYER_MAX_VERTICAL_ANGLE 85.0f
#define PLAYER_DEATH_TIME 4000
#define PLAYER_RESPAWN_HEALTH 100
#define PLAYER_RESPAWN_ARMOR 0
#define PLAYER_HEIGHT_DAMAGE_RATIO 10
#define PLAYER_HEIGHT_DAMAGE_MINIMUM_GRAVITY 25
#define PLAYER_HEIGHT_DAMAGE_MINIMUM_HEIGHT 16
#define PLAYER_DAMAGE_IMMUNITY_DELAY 600
#define PLAYER_STEP_SOUNDS 2

#include <d3d9.h>
#include <d3dx9.h>
#include "../Game/CInventory.h"
#include "../Game/CHud.h"
#include "../Engine/Render/CSceneCamera.h"

enum playerState
{
	PLAYER_STATE_STAND,
	PLAYER_STATE_JUMP,
	PLAYER_STATE_CROUCH
};

class CShape;
class CMovement;
class CFlashlight;
class CCamera;
class CEffectSoundEntity;
class CControlManager;
class COptionManager;
class CModManager;

/**
* CPlayer class.
* Manages the player.
*/
class CPlayer
{
private:
	bool m_alive;
	bool m_hasMoved;
	bool m_hasFlashlight;
	int m_movementState;
	int m_health;
	int m_armor;
	int m_stepOrder;
	float m_verticalMoveModification;
	float m_deathTimer;
	float m_damageTimer;

	CInventory m_inventory;
	CHud m_hud;
	CSceneCamera * m_camera;
	CFlashlight * m_flashlight;

	D3DXVECTOR3 m_rotation;
	D3DXVECTOR3 m_originPosition;
	D3DXVECTOR3 m_originRotation;
	D3DXVECTOR3 m_previousPosition;

	CShape * m_shape;
	CMovement * m_movement;
	CMovement * m_gravityMovement;

	CEffectSoundEntity * m_flashlightSound;
	CEffectSoundEntity * m_jumpSound;
	CEffectSoundEntity * m_hurtSound;
	CEffectSoundEntity * m_steps[PLAYER_STEP_SOUNDS];

	CControlManager * m_controlM;
	COptionManager * m_optionM;
	CModManager * m_modM;

public:
	CPlayer(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation, const int health, const int armor);
	~CPlayer();
	void GiveArmor(const int armorBonus);
	void GiveFlashlight();
	void GiveHealth(const int healthBonus);
	void GiveMagneticCard(const int cardId);
	void RenderHud();
	void Respawn(const int health, const int armor);
	void SetRespawnPoint(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation);
	void TakeDamage(const int healthBonus);
	void Update(const float time);
	void UpdateCamera();
	const bool HasFlashlight() const;
	const bool HasMagneticCard(const int cardId) const;
	const bool IsAlive() const;
	const int GetArmor() const;
	const int GetHealth() const;
	CMovement * GetGravityMovement() const;
	CMovement * GetMovement() const;
	CShape * GetShape() const;
};

#endif
