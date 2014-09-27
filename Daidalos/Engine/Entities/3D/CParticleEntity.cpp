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

#include "../../../Engine/Entities/3D/CParticleEntity.h"

/**
* Constructor
*/
CParticleEntity::CParticleEntity()
{
	this->m_life = 0;
	this->m_lifeMax = 0;
	this->m_lifeNormalized = 1.0f;
	this->m_mass = 1;
	this->m_damping = 1;
	this->m_rotation = 0;
	this->m_initialRotationSpeed = 0;
	this->m_finalRotationSpeed = 0;
	this->m_size = 0;
	this->m_initialSize = 0;
	this->m_finalSize = 0;
	this->m_distanceToCamera = 0;
	this->m_position = D3DXVECTOR3(0,0,0);
	this->m_gravity = D3DXVECTOR3(0,0,0);
	this->m_velocity = D3DXVECTOR3(0,0,0);
	this->m_color = D3DXCOLOR(0,0,0,0);
	this->m_initialColor = D3DXCOLOR(0,0,0,0);
	this->m_finalColor = D3DXCOLOR(0,0,0,0);
}

/**
* Destructor
*/
CParticleEntity::~CParticleEntity(){}

/**
* Initialize the particle
* @param particleName The particle configuration name to use
* @param variationFactor The variation factor
*/
void CParticleEntity::Initialize(const SParticleConfiguration * const particleConfiguration, const int variationFactor, const D3DXVECTOR3 gravity, const D3DXVECTOR3 initialVelocity, const D3DXVECTOR3 emitterPosition, const int textureId)
{
	// If the particle configuration exists, we use it
	if(particleConfiguration != NULL)
	{
		float randFactor = 1.0f + ((((rand() % (variationFactor * 2)) - variationFactor) / 100.0f));

		this->m_life = 0;
		this->m_lifeMax = (float)(rand() % (int)particleConfiguration->m_maxLife);
		this->m_lifeNormalized = this->m_life / this->m_lifeMax;

		randFactor = 1.0f + ((((rand() % (variationFactor * 2)) - variationFactor) / 100.0f));
		this->m_initialRotationSpeed = particleConfiguration->m_initialRotationSpeed * randFactor;
		this->m_finalRotationSpeed = particleConfiguration->m_finalRotationSpeed * randFactor;

		randFactor = 1.0f + ((((rand() % (variationFactor * 2)) - variationFactor) / 100.0f));
		this->m_initialSize = particleConfiguration->m_initialSize * randFactor;
		this->m_finalSize = particleConfiguration->m_finalSize * randFactor;

		this->m_mass = particleConfiguration->m_mass;
		this->m_damping = particleConfiguration->m_damping;
		this->m_rotation = (float)(rand() % 360);
		this->m_size = this->m_initialSize;
		this->m_initialColor = particleConfiguration->m_initialColor;
		this->m_finalColor = particleConfiguration->m_finalColor;
		this->m_color = this->m_initialColor;
		this->m_gravity = gravity;
		this->m_velocity = initialVelocity;
		this->m_position = emitterPosition;

		// Initial velocity with variation
		randFactor = (((rand() % ((variationFactor * 2) + 1)) - variationFactor) / 100.0f);
		this->m_velocity.x = randFactor * initialVelocity.x;
		randFactor = (((rand() % ((variationFactor * 2) + 1)) - variationFactor) / 100.0f);
		this->m_velocity.z = randFactor * initialVelocity.z;

		// Fill the instance structure
		this->m_instanceData.position = this->m_position;
		this->m_instanceData.color = this->m_color;
		this->m_instanceData.data.x = this->m_rotation;
		this->m_instanceData.data.y = this->m_size;
		this->m_instanceData.data.z = this->m_lifeNormalized;
		this->m_instanceData.data.w = (float)textureId;
	}
	else
	{
		// If not the particle will be desallocated on the next frame
		this->m_life = 0;
		this->m_lifeMax = 0;
		this->m_lifeNormalized = 1.0f;
	}
}

/**
* Update the particle
* @param time Elapsed time
* @param cameraPosition The camera position
*/
void CParticleEntity::Update(const float time)
{
	float tmpTime = time / 1000.0f;
	this->m_life += time;
	this->m_lifeNormalized = this->m_life / this->m_lifeMax;

	if(this->m_life < this->m_lifeMax)
	{
		this->m_position += this->m_velocity * tmpTime;

		this->m_velocity += this->m_gravity * this->m_mass * tmpTime;
		this->m_velocity.x *= (1 - (1 - this->m_damping) * tmpTime);
		this->m_velocity.z *= (1 - (1 - this->m_damping) * tmpTime);

		this->m_rotation += (this->m_initialRotationSpeed + (this->m_finalRotationSpeed - this->m_initialRotationSpeed) * this->m_lifeNormalized) * tmpTime;
		if(this->m_rotation >= 360)
			this->m_rotation -= 360;
		if(this->m_rotation < 0)
			this->m_rotation += 360;

		this->m_color = this->m_initialColor + (this->m_finalColor - this->m_initialColor) * this->m_lifeNormalized * tmpTime;
		this->m_size = this->m_initialSize + (this->m_finalSize - this->m_initialSize) * this->m_lifeNormalized * tmpTime;

		// Fill the instance structure
		this->m_instanceData.position = this->m_position;
		this->m_instanceData.color = this->m_color;
		this->m_instanceData.data.x = this->m_rotation;
		this->m_instanceData.data.y = this->m_size;
		this->m_instanceData.data.z = this->m_lifeNormalized;
	}
}

/**
* Perform the distance to camera calculation
*/
void CParticleEntity::CameraDistanceCalculation(const D3DXVECTOR3 * const cameraPosition)
{
	D3DXVECTOR3 cameraParticle = this->m_position - *cameraPosition;
	this->m_distanceToCamera = D3DXVec3Length(&cameraParticle);
}

/**
* Check if the particle is alive
* @return True if the particle is alive, false otherwise
*/
const bool CParticleEntity::IsAlive() const
{
	return this->m_lifeNormalized < 1.0f;
}

/**
* Get the normalized life
* @return The normalized life
*/
const float CParticleEntity::GetLifeNormalized() const
{
	return this->m_lifeNormalized;
}

/**
* Get the rotation
* @return The rotation
*/
const float CParticleEntity::GetRotation() const
{
	return this->m_rotation;
}

/**
* Get the size
* @return The size
*/
const float CParticleEntity::GetSize() const
{
	return this->m_size;
}

/**
* Get the distance to the camera
* @return The distance to the camera
*/
const float CParticleEntity::GetDistanceToCamera() const
{
	return this->m_distanceToCamera;
}

/**
* Get the position
* @return The position
*/
const D3DXVECTOR3 CParticleEntity::GetPosition() const
{
	return this->m_position;
}

/**
* Get the color
* @return The color
*/
const D3DXCOLOR CParticleEntity::GetColor() const
{
	return this->m_color;
}

/**
* Get the particle instance
* @return The particle instance
*/
const SVertexParticleInstance * const CParticleEntity::GetParticleInstance() const
{
	return &this->m_instanceData;
}

/**
* Equal operator override
* @param particle A particle
*/
CParticleEntity & CParticleEntity::operator =(const CParticleEntity & particle)
{
	this->m_life = particle.m_life;
	this->m_lifeMax = particle.m_lifeMax;
	this->m_lifeNormalized = particle.m_lifeNormalized;
	this->m_mass = particle.m_mass;
	this->m_rotation = particle.m_rotation;
	this->m_initialRotationSpeed = particle.m_initialRotationSpeed;
	this->m_finalRotationSpeed = particle.m_finalRotationSpeed;
	this->m_size = particle.m_size;
	this->m_initialSize = particle.m_initialSize;
	this->m_finalSize = particle.m_finalSize;
	this->m_distanceToCamera = particle.m_distanceToCamera;
	this->m_position = particle.m_position;
	this->m_gravity = particle.m_gravity;
	this->m_velocity = particle.m_gravity;
	this->m_color = particle.m_color;
	this->m_initialColor = particle.m_initialColor;
	this->m_finalColor = particle.m_finalColor;
	return *this;
}
