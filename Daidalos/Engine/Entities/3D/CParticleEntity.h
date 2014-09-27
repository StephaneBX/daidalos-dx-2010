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

#ifndef __CPARTICLEENTITY_H
#define __CPARTICLEENTITY_H

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

struct SParticleConfiguration
{
	float m_minLife;
	float m_maxLife;
	float m_mass;
	float m_damping;
	float m_initialRotationSpeed;
	float m_finalRotationSpeed;
	float m_initialSize;
	float m_finalSize;
	D3DXCOLOR m_initialColor;
	D3DXCOLOR m_finalColor;
};

/**
* Structure used for the particle instances
*/
struct SVertexParticleInstance
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR4 data; // rotation, size, life, textureId
};

/**
* CParticleEntity class.
* Manages the particles entities.
*/
class CParticleEntity
{
private:
	float m_life;
	float m_lifeMax;
	float m_lifeNormalized;
	float m_mass;
	float m_damping;
	float m_rotation;
	float m_initialRotationSpeed;
	float m_finalRotationSpeed;
	float m_size;
	float m_initialSize;
	float m_finalSize;
	float m_distanceToCamera;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_gravity;
	D3DXVECTOR3 m_velocity;
	D3DXCOLOR m_color;
	D3DXCOLOR m_initialColor;
	D3DXCOLOR m_finalColor;
	SVertexParticleInstance m_instanceData;

public:
	CParticleEntity();
	~CParticleEntity();
	void Initialize(const SParticleConfiguration * const m_particleConfiguration, const int variationFactor, const D3DXVECTOR3 gravity, const D3DXVECTOR3 initialVelocity, const D3DXVECTOR3 emitterPosition, const int textureId);
	void Update(const float time);
	void CameraDistanceCalculation(const D3DXVECTOR3 * const cameraPosition);
	const bool IsAlive() const;
	const float GetLifeNormalized() const;
	const float GetRotation() const;
	const float GetSize() const;
	const float GetDistanceToCamera() const;
	const D3DXVECTOR3 GetPosition() const;
	const D3DXCOLOR GetColor() const;
	const SVertexParticleInstance * const GetParticleInstance() const;
	CParticleEntity & CParticleEntity::operator =(const CParticleEntity & particle);
};

#endif