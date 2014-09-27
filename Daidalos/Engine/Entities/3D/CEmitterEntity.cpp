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
#include "../../../Engine/Entities/3D/CParticleEntity.h"
#include "../../../Engine/Entities/3D/CEmitterEntity.h"

/**
* Constructor
*/
CEmitterEntity::CEmitterEntity()
{
	this->m_emitterConfiguration = NULL;
	this->m_particleConfiguration = NULL;
	this->m_particles.Allocate(EMITTER_MAXIMUM_PARTICLES);
	this->m_boundingBox = new CSphere();
}

CEmitterEntity::~CEmitterEntity(){}

/**
* Initialize the emitter
* @param emitterConfiguration The emitter configuration
* @param particleConfiguration The particle configuration
*/
void CEmitterEntity::Initialize(const SEmitterConfiguration * const emitterConfiguration, const SParticleConfiguration * const particleConfiguration)
{
	this->m_life = 0;
	this->m_particleCreationTimer = 0;
	this->m_emitterConfiguration = emitterConfiguration;
	this->m_particleConfiguration = particleConfiguration;
	this->m_particles.DeleteAll();
}

/**
* Set the emitter position
* @param position The emitter position
*/
void CEmitterEntity::SetPosition(const D3DXVECTOR3 position)
{
	this->m_boundingBox->SetPosition(position);
}

/**
* Update the emitter
* @param time Elapsed time
* @param cameraPosition The camera position
*/
void CEmitterEntity::Update(const float time, const D3DXVECTOR3 * const cameraPosition)
{
	float newBBoxRadius = 0;
	D3DXVECTOR3 tmpDistance;

	this->m_life += time / 1000.0f;
	if(this->m_life < this->m_emitterConfiguration->m_lifeMax || this->m_particles.GetNumUsedElements() > 0 || this->m_emitterConfiguration->m_lifeMax == -1)
	{
		// Update the particles
		for(int i = 0 ; i < this->m_particles.GetArraySize() ; i++)
		{
			if(this->m_particles.IsAlive(i))
			{
				this->m_particles.GetAt(i)->Update(time);
				if(!this->m_particles.GetAt(i)->IsAlive())
					this->m_particles.Delete(i);
				else
				{
					// Bounding box radius update
					tmpDistance = this->m_particles.GetAt(i)->GetPosition() - *this->m_boundingBox->GetPosition();
					newBBoxRadius = D3DXVec3Length(&tmpDistance);
					if(this->m_boundingBox->GetOffset(NULL) < newBBoxRadius)
						((CSphere*)this->m_boundingBox)->SetOffset(newBBoxRadius);
				}
			}
		}
	}

	// Particles creation
	if(this->m_life < this->m_emitterConfiguration->m_lifeMax || this->m_emitterConfiguration->m_lifeMax == -1)
	{
		// Count the number of particles that we can create with the timer
		this->m_particleCreationTimer += time;
		int particlesToCreate = (int)(this->m_particleCreationTimer / (1000.0f / (float)this->m_emitterConfiguration->m_emissionRate));

		// Substract the timer used for the particles creation
		this->m_particleCreationTimer -= particlesToCreate * (1000.0f / (float)this->m_emitterConfiguration->m_emissionRate);

		CParticleEntity * tmpParticle;

		// Create the particles
		for(int i = 0 ; i < particlesToCreate ; i++)
		{
			if(this->m_particles.GetNumUsedElements() < this->m_emitterConfiguration->m_maximumParticles)
			{
				tmpParticle = this->m_particles.New();
				if(tmpParticle != NULL)
				{
					tmpParticle->Initialize(this->m_particleConfiguration,  this->m_emitterConfiguration->m_variationFactor,  this->m_emitterConfiguration->m_gravity,  this->m_emitterConfiguration->m_initialVelocity, *this->m_boundingBox->GetPosition(), this->m_emitterConfiguration->m_textureId);
				}
			}
		}
	}
}

/**
* Return the bounding box
* @return The bounding boc
*/
CShape * const CEmitterEntity::GetShape()
{
	return this->m_boundingBox;
}

/**
* Get the choosen particle
* @param i The index
* @return The choosen particle
*/
CParticleEntity * const CEmitterEntity::GetParticle(const int i)
{
	return this->m_particles.GetAt(i);
}

/**
* Equal operator override
* @param emitter An emitter
*/
CEmitterEntity & CEmitterEntity::operator =(const CEmitterEntity & emitter)
{
	this->m_life = emitter.m_life;
	this->m_emitterConfiguration = emitter.m_emitterConfiguration;
	this->m_particleConfiguration = emitter.m_particleConfiguration;
	return *this;
}
