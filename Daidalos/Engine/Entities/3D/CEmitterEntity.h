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

#ifndef __CEMITTERENTITY_H
#define __CEMITTERENTITY_H

#define EMITTER_MAXIMUM_PARTICLES 2000

#include <d3d9.h>
#include <d3dx9.h>
#include "../../../Misc/CMemoryAllocator.h"

struct SEmitterConfiguration
{
	int m_emissionRate;
	int m_variationFactor;
	int m_maximumParticles;
	int m_textureId;
	float m_lifeMax;
	std::string m_particleConfigurationName;
	D3DXVECTOR3 m_gravity;
	D3DXVECTOR3 m_initialVelocity;
};

class CShape;

/**
* CEmitterEntity class.
* Manages the particle emitter entities.
*/
class CEmitterEntity
{
private:
	float m_life;
	float m_particleCreationTimer;
	CMemoryAllocator<CParticleEntity> m_particles;
	CShape * m_boundingBox;
	const SEmitterConfiguration * m_emitterConfiguration;
	const SParticleConfiguration * m_particleConfiguration;

public:
	CEmitterEntity();
	~CEmitterEntity();
	void Initialize(const SEmitterConfiguration * const emitterConfiguration, const SParticleConfiguration * const particleConfiguration);
	void SetPosition(const D3DXVECTOR3 position);
	void Update(const float time, const D3DXVECTOR3 * const cameraPosition);
	CShape * const GetShape();
	CParticleEntity * const GetParticle(const int i);
	CEmitterEntity & CEmitterEntity::operator =(const CEmitterEntity & emitter);
};

#endif