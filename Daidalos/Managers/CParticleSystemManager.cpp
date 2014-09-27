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

#include <sstream>
#include "../Misc/Global.h"
#include "../Engine/CTokenizer.h"
#include "../Engine/Render/CSceneCamera.h"
#include "../Managers/CModManager.h"
#include "../Managers/CParticleSystemManager.h"

/**
* Constructor
*/
CParticleSystemManager::CParticleSystemManager(){}

/**
* Destructor
*/
CParticleSystemManager::~CParticleSystemManager()
{
	FreeDatabase();
}

/**
* Initialize the manager
*/
void CParticleSystemManager::Initialize()
{
	this->m_emitters.Allocate(PARTICLESYSTEM_MAXIMUM_EMITTERS);
	this->m_particleInstances = new SVertexParticleInstance[PARTICLESYSTEM_MAXIMUM_EMITTERS * EMITTER_MAXIMUM_PARTICLES];

	CModManager * modM = CModManager::GetInstance();
	this->m_textureDiffuse = modM->LoadTexture(PARTICLESYSTEM_TEXTURE_DIFFUSE, TEXTURE_TYPE_MODEL);
	this->m_textureNormal = modM->LoadTexture(PARTICLESYSTEM_TEXTURE_NORMAL, TEXTURE_TYPE_MODEL);
}

/**
* Free the databases
*/
void CParticleSystemManager::FreeDatabase()
{
	for(std::map<std::string, SParticleConfiguration *>::iterator it = this->m_particleConfigurations.begin() ; it != this->m_particleConfigurations.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_particleConfigurations.clear();

	for(std::map<std::string, SEmitterConfiguration *>::iterator it = this->m_emitterConfigurations.begin() ; it != this->m_emitterConfigurations.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_emitterConfigurations.clear();

	SAFE_DELETE_ARRAY(this->m_particleInstances);
}

/**
* @param time Elapsed time
* @param cameraPosition The camera position
*/
void CParticleSystemManager::Update(const float time, CSceneCamera * const camera)
{
	for(int i = 0 ; i < this->m_emitters.GetArraySize() ; i++)
		if(this->m_emitters.IsAlive(i))
			this->m_emitters.GetAt(i)->Update(time, &camera->GetPosition());
}

/**
* Fill the particle buffer with the visible emitters' particles
*/
void CParticleSystemManager::FillParticleInstance(CSceneCamera * const camera)
{
	// Fill the temporary particle buffer and index in order to sort the particles
	this->m_particlesBufferUsedSize = 0;
	D3DXVECTOR3 cameraPosition = camera->GetPosition();
	for(int i = 0 ; i < this->m_emitters.GetArraySize() ; i++)
	{
		if(this->m_emitters.IsAlive(i) && !camera->FrustumCull(this->m_emitters.GetAt(i)->GetShape()))
		{
			for(int j = 0 ; j < EMITTER_MAXIMUM_PARTICLES ; j++)
			{
				if(this->m_emitters.GetAt(i)->GetParticle(j)->IsAlive())
				{
					this->m_emitters.GetAt(i)->GetParticle(j)->CameraDistanceCalculation(&cameraPosition);
					this->m_particlesBuffer[this->m_particlesBufferUsedSize] = this->m_emitters.GetAt(i)->GetParticle(j);
					this->m_particleDistance[this->m_particlesBufferUsedSize] = this->m_emitters.GetAt(i)->GetParticle(j)->GetDistanceToCamera();
					++this->m_particlesBufferUsedSize;
				}
			}
		}
	}

	// Sort the particles
	for(int i = 0 ; i < this->m_particlesBufferUsedSize ; i++)
		this->m_particleIndex[i] = i;
	this->m_distanceSorter.QuickSort(this->m_particleDistance, this->m_particleIndex, 0, this->m_particlesBufferUsedSize - 1);

	// Fill the final buffer
	for(int i = 0 ; i < this->m_particlesBufferUsedSize ; i++)
		memcpy(&this->m_particleInstances[i], this->m_particlesBuffer[this->m_particleIndex[i]]->GetParticleInstance(), sizeof(SVertexParticleInstance));
}

/**
* Get the used size in the particle buffer
* @return The used size in the particle buffer
*/
int CParticleSystemManager::GetParticleBufferUsedSize() const
{
	return this->m_particlesBufferUsedSize;
}

/**
* Load an emitter
* @param configurationName The emitter configuration name
* @param position The emitter initial position
* @param modDirectory The mod directory
*/
CEmitterEntity * const CParticleSystemManager::LoadEmitter(const std::string configurationName, const std::string modDirectory)
{
	CEmitterEntity * tmpEmitter = NULL;

	const SEmitterConfiguration * const tmpEmitterConfiguration = LoadEmitterConfiguration(configurationName, modDirectory);
	if(tmpEmitterConfiguration != NULL)
	{
		const SParticleConfiguration * const tmpParticleConfiguration = LoadParticleConfiguration(tmpEmitterConfiguration->m_particleConfigurationName, modDirectory);
		if(tmpParticleConfiguration != NULL)
		{
			tmpEmitter = this->m_emitters.New();
			tmpEmitter->Initialize(tmpEmitterConfiguration, tmpParticleConfiguration);
		}
	}
	return tmpEmitter;
}

/**
* Get the particle instances
* @return The particle instances
*/
const SVertexParticleInstance * const CParticleSystemManager::GetParticleInstances() const
{
	return this->m_particleInstances;
}

/**
* Return the particle system diffuse texture
* @return The particle system diffuse texture
*/
const LPDIRECT3DTEXTURE9 CParticleSystemManager::GetTextureDiffuse() const
{
	return this->m_textureDiffuse;
}

/**
* Return the particle system normal texture
* @return The particle system normal texture
*/
const LPDIRECT3DTEXTURE9 CParticleSystemManager::GetTextureNormal() const
{
	return this->m_textureNormal;
}

/**
* Read a particle configuration in the particles file and store it in the database
* @param filename The particles file to use
* @param configurationName The particle configuration name
*/
void CParticleSystemManager::ReadAndStoreParticleConfiguration(const std::string filename, const std::string configurationName)
{
	CTokenizer tokenizer;
	std::istringstream ss;
	char tmp[PARTCILESYSTEM_CHARSIZE_INI];
	SParticleConfiguration * particleConfiguration = new SParticleConfiguration;

	GetPrivateProfileString(configurationName.c_str(), "minLife", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_minLife;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "maxLife", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_maxLife;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "mass", "1", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_mass;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "damping", "1", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_damping;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "initialRotationSpeed", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_initialRotationSpeed;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "finalRotationSpeed", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_finalRotationSpeed;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "initialSize", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_initialSize;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "finalSize", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> particleConfiguration->m_finalSize;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "initialColor", "0,0,0,0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	tokenizer.AddLine(std::string(tmp));
	GetPrivateProfileString(configurationName.c_str(), "finalColor", "0,0,0,0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	tokenizer.AddLine(std::string(tmp));

	if(tokenizer.Tokenize(",", 4) && tokenizer.m_tokens.size() == 8)
	{
		particleConfiguration->m_initialColor = D3DXCOLOR(0,0,0,1);
		particleConfiguration->m_finalColor = D3DXCOLOR(0,0,0,1);

		ss.str(tokenizer.m_tokens.at(0));
		ss >> particleConfiguration->m_initialColor.r;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(1));
		ss >> particleConfiguration->m_initialColor.g;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(2));
		ss >> particleConfiguration->m_initialColor.b;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(3));
		ss >> particleConfiguration->m_initialColor.a;
		ss.clear();

		ss.str(tokenizer.m_tokens.at(4));
		ss >> particleConfiguration->m_finalColor.r;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(5));
		ss >> particleConfiguration->m_finalColor.g;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(6));
		ss >> particleConfiguration->m_finalColor.b;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(7));
		ss >> particleConfiguration->m_finalColor.a;
		ss.clear();
	}

	// Data verification
	if(particleConfiguration->m_minLife < 0)
		particleConfiguration->m_minLife = 0;
	if(particleConfiguration->m_maxLife < 0)
		particleConfiguration->m_maxLife = 0;
	if(particleConfiguration->m_mass < 1)
		particleConfiguration->m_mass = 1;
	if(particleConfiguration->m_damping > 1)
		particleConfiguration->m_damping = 1;
	if(particleConfiguration->m_damping < 0)
		particleConfiguration->m_damping = 0;
	if(particleConfiguration->m_initialSize < 0)
		particleConfiguration->m_initialSize = 1;
	if(particleConfiguration->m_finalSize < 0)
		particleConfiguration->m_finalSize = 1;

	// Add the particle configuration to the database
	this->m_particleConfigurations[configurationName] = particleConfiguration;
}

/**
* Read an emitter configuration in the emitters file and store it in the database
* @param filename The emitters file to use
* @param configurationName The emitter configuration name
*/
void CParticleSystemManager::ReadAndStoreEmitterConfiguration(const std::string filename, const std::string configurationName)
{
	CTokenizer tokenizer;
	std::istringstream ss;
	char tmp[PARTCILESYSTEM_CHARSIZE_INI];
	SEmitterConfiguration * emitterConfiguration = new SEmitterConfiguration;

	GetPrivateProfileString(configurationName.c_str(), "particleConfiguration", "", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_particleConfigurationName;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "life", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_lifeMax;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "emissionRate", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_emissionRate;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "variationFactor", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_variationFactor;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "maximumParticles", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_maximumParticles;
	ss.clear();

	GetPrivateProfileString(configurationName.c_str(), "textureId", "0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	ss.str(tmp);
	ss >> emitterConfiguration->m_textureId;
	ss.clear();


	GetPrivateProfileString(configurationName.c_str(), "gravity", "0,0,0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	tokenizer.AddLine(std::string(tmp));
	GetPrivateProfileString(configurationName.c_str(), "initialVelocity", "0,0,0", tmp, PARTCILESYSTEM_CHARSIZE_INI, filename.c_str());
	tokenizer.AddLine(std::string(tmp));

	if(tokenizer.Tokenize(",", 3) && tokenizer.m_tokens.size() == 6)
	{
		emitterConfiguration->m_initialVelocity = D3DXVECTOR3(0,0,0);
		emitterConfiguration->m_gravity = D3DXVECTOR3(0,0,0);

		ss.str(tokenizer.m_tokens.at(0));
		ss >> emitterConfiguration->m_gravity.x;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(1));
		ss >> emitterConfiguration->m_gravity.y;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(2));
		ss >> emitterConfiguration->m_gravity.z;
		ss.clear();

		ss.str(tokenizer.m_tokens.at(3));
		ss >> emitterConfiguration->m_initialVelocity.x;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(4));
		ss >> emitterConfiguration->m_initialVelocity.y;
		ss.clear();
		ss.str(tokenizer.m_tokens.at(5));
		ss >> emitterConfiguration->m_initialVelocity.z;
		ss.clear();
	}

	// Data verification
	if(emitterConfiguration->m_lifeMax < -1)
		emitterConfiguration->m_lifeMax = -1;
	if(emitterConfiguration->m_emissionRate < 0)
		emitterConfiguration->m_emissionRate = 0;
	if(emitterConfiguration->m_variationFactor < 0)
		emitterConfiguration->m_variationFactor = 0;
	if(emitterConfiguration->m_variationFactor > 100)
		emitterConfiguration->m_variationFactor = 100;
	if(emitterConfiguration->m_maximumParticles < 0)
		emitterConfiguration->m_maximumParticles = 0;

	// Add the emitter configuration to the database
	this->m_emitterConfigurations[configurationName] = emitterConfiguration;
}

/**
* Check if the particle configuration already exists in the database
* @param configurationName The particle configuration name
* @return True if the configuration exists, false otherwise
*/
const bool CParticleSystemManager::ParticleConfigurationExists(const std::string configurationName)
{
	bool ret = false;
	for(std::map<std::string, SParticleConfiguration *>::iterator it = this->m_particleConfigurations.begin() ; it != this->m_particleConfigurations.end() ; it++)
		if(it->first == configurationName)
			ret = true;
	return ret;
}

/**
* Check if the emitter configuration already exists in the database
* @param configurationName The emitter configuration name
* @return True if the configuration exists, false otherwise
*/
const bool CParticleSystemManager::EmitterConfigurationExists(const std::string configurationName)
{
	bool ret = false;
	for(std::map<std::string, SEmitterConfiguration *>::iterator it = this->m_emitterConfigurations.begin() ; it != this->m_emitterConfigurations.end() ; it++)
		if(it->first == configurationName)
			ret = true;
	return ret;
}

/**
* Load a particle configuration
* @param configurationName The particle configuration name
* @param modDirectory The mod directory
*/
const SParticleConfiguration * const CParticleSystemManager::LoadParticleConfiguration(const std::string configurationName, const std::string modDirectory)
{
	if(!ParticleConfigurationExists(configurationName))
	{
		std::stringstream stream;
		stream << DIRECTORY_ENGINE_CONFIGURATION << modDirectory << PARTCILESYSTEM_PARTICLES_INI_FILE;
		if(!FileExists(stream.str()))
		{
			stream.str("");
			stream << DIRECTORY_ENGINE_CONFIGURATION << PARTCILESYSTEM_PARTICLES_INI_FILE;
		}

		// Load the particle configurations
		ReadAndStoreParticleConfiguration(stream.str(), configurationName);
	}

	return this->m_particleConfigurations[configurationName];
}

/**
* Load an emitter configuration
* @param configurationName The emitter configuration name
* @param modDirectory The mod directory
*/
const SEmitterConfiguration * const CParticleSystemManager::LoadEmitterConfiguration(const std::string configurationName, const std::string modDirectory)
{
	if(!EmitterConfigurationExists(configurationName))
	{
		std::stringstream stream;
		stream << DIRECTORY_ENGINE_CONFIGURATION << modDirectory << PARTCILESYSTEM_EMITTERS_INI_FILE;
		if(!FileExists(stream.str()))
		{
			stream.str("");
			stream << DIRECTORY_ENGINE_CONFIGURATION << PARTCILESYSTEM_EMITTERS_INI_FILE;
		}

		// Load the emitter configurations
		ReadAndStoreEmitterConfiguration(stream.str(), configurationName);
	}

	return this->m_emitterConfigurations[configurationName];
}
