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

#ifndef __COBJECTENTITY_H
#define __COBJECTENTITY_H

#define OBJECT_GROUP_ID_NONE -1
#define OBJECT_MAGNETIC_CARD_NONE -1

#include <d3d9.h>
#include <d3dx9.h>

enum objectType
{
	OBJECT_TYPE_MISC,
	OBJECT_TYPE_DOOR,
	OBJECT_TYPE_LOCKED_DOOR,
	OBJECT_TYPE_SWITCH,
	OBJECT_TYPE_USABLE
};

enum objectState
{
	OBJECT_STATE_MISC_IDLE,
	OBJECT_STATE_DOOR_CLOSED,
	OBJECT_STATE_DOOR_OPENING,
	OBJECT_STATE_DOOR_OPENED,
	OBJECT_STATE_DOOR_CLOSING,
	OBJECT_STATE_LOCKED_DOOR_CLOSED,
	OBJECT_STATE_LOCKED_DOOR_OPENING,
	OBJECT_STATE_LOCKED_DOOR_OPENED,
	OBJECT_STATE_SWITCH_OFF,
	OBJECT_STATE_SWITCH_ON,
	OBJECT_STATE_USABLE_UNUSED,
	OBJECT_STATE_USABLE_USING
};

enum objectActivationType
{
	OBJECT_ACTIVATE_OFF,
	OBJECT_ACTIVATE_ON_TOUCH,
	OBJECT_ACTIVATE_ON_PUSH
};

enum objectCommandType
{
	OBJECT_COMMAND_NOTHING,
	OBJECT_COMMAND_ENABLE_BRUSH,
	OBJECT_COMMAND_DISABLE_BRUSH
};

struct SObjectCommand
{
	int m_type;
	int m_value;
};

struct SObjectAnimation
{
	float m_startFrame;
	float m_endFrame;
	float m_fps;
};

class CShape;
class CMovement;
class CEntityRender;
class CPlayer;
class CLightManager;
class CLogManager;
class CModManager;
class CLight;

/**
* CObjectEntity class.
* Generic class that manages the objects.
* An object can be composed of 3d models, sounds, lights.
*/
class CObjectEntity
{
protected:
	bool m_alive;
	int m_type;
	int m_state;
	int m_activationType;
	int m_groupId;
	float m_timer;
	SObjectCommand m_command;
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_rotation;
	D3DXMATRIX m_scalingMatrix;
	D3DXMATRIX m_rotationMatrix;
	D3DXMATRIX m_translationMatrix;

	CPlayer * m_player;
	CShape * m_visibleShape;
	CShape * m_activationShape;
	CEntityRender * m_render;
	CLight * m_light;

	CModManager * m_modM;
	CLogManager * m_logM;

public:
	CObjectEntity * m_next;

public:
	CObjectEntity(const D3DXVECTOR3 rotation);
	virtual ~CObjectEntity();
	void LinkPlayer(CPlayer * const player);
	void SetState(const int state);
	void SetGroup(const int groupId);
	void SetTimer(const float timer);
	virtual void Update(const float time);
	const bool IsAlive();
	const bool IsCollidedByPlayer();
	const int GetType() const;
	const int GetGroup() const;
	const int GetState() const;
	const SObjectCommand GetCommand() const;
	D3DXVECTOR3 * GetRotation();
	CShape * GetVisibleShape() const;
	CShape * GetActivationShape() const;
	CEntityRender * GetRender() const;

protected:
	virtual void FillGeometry(const int shadingType);
};


#endif