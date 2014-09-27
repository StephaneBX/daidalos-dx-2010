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

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <sstream>
#include "../Misc/Global.h"
#include "../Shaders/CShader.h"
#include "../Shaders/CDSShaderFillWorld.h"
#include "../Shaders/CDSShaderFillWorldNoShadows.h"
#include "../Shaders/CDSShaderFillLightning.h"
#include "../Shaders/CDSShaderFillLightningNoShadows.h"
#include "../Shaders/CDSShaderRenderWorld.h"
#include "../Shaders/CDSShaderFillShadowMap.h"
#include "../Shaders/CForwardShaderParticles.h"
#include "../Managers/CDeviceManager.h"
#include "../Managers/CLogManager.h"
#include "../Managers/CModManager.h"
#include "../Managers/CCameraManager.h"
#include "../Managers/CLightManager.h"
#include "../Managers/CCameraManager.h"
#include "../Managers/CParticleSystemManager.h"
#include "../Engine/Entities/3D/CObjectEntity.h"
#include "../Engine/Render/CEntityRender.h"
#include "../Engine/CEngine.h"

SEngineConfiguration * CEngine::m_configuration = new SEngineConfiguration;

/**
* Constructor
*/
CEngine::CEngine()
{
	this->m_staticWorldVertexBuffer = NULL;
	this->m_staticLightVertexBuffer = NULL;
	this->m_dynamicLightVertexBuffer = NULL;
	this->m_screenQuadBuffer = NULL;
	this->m_vDeclaration = NULL;
	this->m_vScreenQuadDeclaration = NULL;
	this->m_screenQuadInstance = NULL;
	this->m_normalRenderTarget = NULL;
	this->m_albedoRenderTarget = NULL;
	this->m_positionRenderTarget = NULL;
	this->m_lightningRenderTarget = NULL;
	this->m_shadowRenderTarget = NULL;
	this->m_normalTexture = NULL;
	this->m_albedoTexture = NULL;
	this->m_positionTexture = NULL;
	this->m_shadowTexture = NULL;
	this->m_shadowDepthStencil = NULL;
	this->m_vParticleInstance = NULL;

	for(int i = 0 ; i < LIGHT_SHADOW_MAXIMUM_SOURCES ; i++)
	{
		this->m_lightsShadowRenderTarget[i] = NULL;
		this->m_lightsShadowTexture[i] = NULL;
	}

	this->m_player = NULL;
	this->m_levelLoaded = false;
	
	Initialize();
}

/**
* Destructor
*/
CEngine::~CEngine()
{
	Destroy();
	SAFE_DELETE(this->m_configuration);
}

/**
* Initialize the engine
* Create all the buffers that don't need level information and the necessary shaders
*/
void CEngine::Initialize()
{
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_cameraM = CCameraManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();
	this->m_lightM = CLightManager::GetInstance();
	this->m_particleM = CParticleSystemManager::GetInstance();

	LoadEngineConfiguration();

	// Shaders instanciation
	if(this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
	{
			this->m_shaderFillShadowMap = this->m_modM->LoadShader(SHADER_TYPE_DS_FILL_SHADOW_MAP);
			this->m_shaderFillWorld = this->m_modM->LoadShader(SHADER_TYPE_DS_FILL_WORLD);
			this->m_shaderFillLightning = this->m_modM->LoadShader(SHADER_TYPE_DS_FILL_LIGHTNING);
			this->m_shaderRenderWorld = this->m_modM->LoadShader(SHADER_TYPE_DS_RENDER_WORLD);

			this->m_renderTechnic = ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_USE_SHADOWS;
	}
	else
	{
		this->m_shaderFillWorld = this->m_modM->LoadShader(SHADER_TYPE_DS_FILL_WORLD_NOSHADOWS);
		this->m_shaderFillLightning = this->m_modM->LoadShader(SHADER_TYPE_DS_FILL_LIGHTNING_NOSHADOWS);
		this->m_shaderRenderWorld = this->m_modM->LoadShader(SHADER_TYPE_DS_RENDER_WORLD);

		this->m_renderTechnic = ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_NO_SHADOWS;
	}

	// The Vertex Buffer declaration
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexDeclaration(g_VBDeclaration, &this->m_vDeclaration), "Main Vertex Declaration");
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexDeclaration(g_VBDeclarationScreen, &this->m_vScreenQuadDeclaration), "Screen Quad Declaration");
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexDeclaration(g_VBDeclarationLight, &this->m_vLightDeclaration), "Light Vertex Declaration");
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexDeclaration(g_VBDeclarationParticle, &this->m_vParticleDeclaration), "Particle Vertex Declaration");

	// The Render Targets creation
	_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->m_normalTexture, NULL), "Normal Render Target");
	_VERIFY(this->m_normalTexture->GetSurfaceLevel(0, &this->m_normalRenderTarget), "Normal Texture");

	_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->m_albedoTexture, NULL), "Albedo Render Target");
	_VERIFY(this->m_albedoTexture->GetSurfaceLevel(0, &this->m_albedoRenderTarget), "Albedo Texture");

	_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 1, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &this->m_positionTexture, NULL), "Depth Render Target");
	_VERIFY(this->m_positionTexture->GetSurfaceLevel(0, &this->m_positionRenderTarget), "Depth Texture");

	_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->m_lightningTexture, NULL), "Lightning Render Target");
	_VERIFY(this->m_lightningTexture->GetSurfaceLevel(0, &this->m_lightningRenderTarget), "Lightning Texture");

	// Static Vertex and Index Buffers
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(4 * sizeof(SVertexQuad), D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_screenQuadBuffer, NULL), "Screen Quad Vertex Buffer");
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(4 * sizeof(SVertexParticle), D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_vParticle, NULL), "Particle Vertex Buffer");
	_VERIFY(this->m_deviceM->GetDevice()->CreateIndexBuffer(6 * sizeof(int), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &this->m_screenQuadInstance, NULL), "Screen Quad Index Buffer");

	// Data need for the shadows
	if(this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
	{
		_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->m_shadowTexture, NULL), "Shadow Render Target");
		_VERIFY(this->m_shadowTexture->GetSurfaceLevel(0, &this->m_shadowRenderTarget), "Shadow Texture");

		_VERIFY(this->m_deviceM->GetDevice()->CreateDepthStencilSurface(LIGHT_SHADOWMAP_SIZE, LIGHT_SHADOWMAP_SIZE, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &this->m_shadowDepthStencil, NULL), "Shadow Depth Stencil");

		for(int i = 0 ; i < LIGHT_SHADOW_MAXIMUM_SOURCES ; i++)
		{
			_VERIFY(this->m_deviceM->GetDevice()->CreateTexture(LIGHT_SHADOWMAP_SIZE, LIGHT_SHADOWMAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &this->m_lightsShadowTexture[i], NULL), "Shadow Map Render Target");
			_VERIFY(this->m_lightsShadowTexture[i]->GetSurfaceLevel(0, &this->m_lightsShadowRenderTarget[i]), "Shadow Map Texture");
		}
	}
 
	// Screen Quad Index Buffer
	int g_indexVal[] = {0,1,2,2,3,0};
	void * pIndex = NULL;
	this->m_screenQuadInstance->Lock(0,6 * sizeof(int),(void**)&pIndex,0);
		memcpy(pIndex,g_indexVal,6 * sizeof(int));
	this->m_screenQuadInstance->Unlock();

	float particleTextureSize = 1.0f / (sqrt((float)this->m_configuration->m_spritesPerTexture));

	// Particle Vertex Buffer
	SVertexParticle g_vertexVal[] = 
	{
		{D3DXVECTOR3(-0.5f,-0.5f, 0.0f), D3DXVECTOR2(particleTextureSize, 0)},
		{D3DXVECTOR3(-0.5f, 0.5f ,0.0f), D3DXVECTOR2(0, 0)},
		{D3DXVECTOR3( 0.5f, 0.5f, 0.0f), D3DXVECTOR2(0, particleTextureSize)},
		{D3DXVECTOR3( 0.5f,-0.5f, 0.0f), D3DXVECTOR2(particleTextureSize, particleTextureSize)}
	};
	void * pVertex = NULL;
	this->m_vParticle->Lock(0, 4 * sizeof(SVertexParticle),(void**)&pVertex, 0);
		memcpy(pVertex, g_vertexVal,4 * sizeof(SVertexParticle));
	this->m_vParticle->Unlock();
}

/**
* Load the engine configuration file associated to the current choosen mod
*/
void CEngine::LoadEngineConfiguration()
{
	char tmp[ENGINE_CONFIGURATION_CHAR_SIZE];
	std::ostringstream stream;
	std::istringstream ss;
	stream << this->m_modM->GetActiveModDirectory() << DIRECTORY_ENGINE_CONFIGURATION << ENGINE_CONFIGURATION_FILE;
	if(!FileExists(stream.str()))
	{
		stream.str("");
		stream << DIRECTORY_ENGINE_CONFIGURATION << ENGINE_CONFIGURATION_FILE;
	}

	memset(this->m_configuration, 0, sizeof(SEngineConfiguration));

	// Shadows
	GetPrivateProfileString("engine_shadows", "shadow_map_size", ENGINE_CONFIGURATION_DEFAULT_SHADOW_MAP_SIZE, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.str(tmp);
	ss >> this->m_configuration->m_shadows.x;

	GetPrivateProfileString("engine_shadows", "shadow_map_epsilon", ENGINE_CONFIGURATION_DEFAULT_SHADOW_EPSILON, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_shadows.y;

	// Lightning
	GetPrivateProfileString("engine_lightning", "light_angle_fade_out", ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_ANGLE_FADE_OUT, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_lightning.x;

	GetPrivateProfileString("engine_lightning", "light_distance_fade_out", ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_DISTANCE_FADE_OUT, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_lightning.y;

	GetPrivateProfileString("engine_lightning", "light_fog_fade_out", ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_FOG_FADE_OUT, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_lightning.z;

	GetPrivateProfileString("engine_lightning", "light_ns_modifier", ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_NS_MODIFIER, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_lightning.w;

	// Particles
	GetPrivateProfileString("engine_particles", "maximum_emitters", ENGINE_CONFIGURATION_DEFAULT_PARTICLE_MAXIMUM_EMITTERS, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_maximumEmitters;

	GetPrivateProfileString("engine_particles", "maximum_particles_per_emitter", ENGINE_CONFIGURATION_DEFAULT_PARTICLE_MAXIMUM_PARTICLE_PER_EMITTER, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_maximumParticlesPerEmitter;

	GetPrivateProfileString("engine_particles", "sprites_per_texture", ENGINE_CONFIGURATION_DEFAULT_PARTICLE_SPRITES_PER_TEXTURE, tmp, ENGINE_CONFIGURATION_CHAR_SIZE, stream.str().c_str());
	ss.clear();
	ss.str(tmp);
	ss >> this->m_configuration->m_spritesPerTexture;
}

/**
* Load a level
* @param levelName The level name, wich is also the directoty name of the level
* @param resetPlayer True if the player's state need to be reset
*/
void CEngine::LoadLevel(const std::string levelName, const bool resetPlayer)
{
	this->m_bspWorld.LoadLevelFiles(levelName);
	this->m_dynamicWorld.LoadLevelFiles(levelName, resetPlayer);
	this->m_player = this->m_dynamicWorld.GetPlayer();
	this->m_staticWorldVertexBufferSize = this->m_bspWorld.GetVertexBufferSize();
	this->m_staticLightVertexBufferSize = this->m_bspWorld.GetFacesCounter() * VERTEX_PER_FACE;
	this->m_dynamicLightVertexBufferSize = this->m_dynamicWorld.GetFacesCounter();
	this->m_dynamicWorld.LinkCamera(this->m_cameraM->GetCamera());
	this->m_dynamicWorld.LinkPlayer(this->m_player);
	InitializeBuffers();
	this->m_levelLoaded = true;
}

/**
* Unload the current level
*/
void CEngine::UnloadLevel()
{
	this->m_bspWorld.UnloadLevel();
	this->m_dynamicWorld.UnloadLevel();
	this->m_lightM->FreeDatabase();
	this->m_particleM->FreeDatabase();
	this->m_levelLoaded = false;
}

/**
* Update the engine
* @param time Elapsed time
*/
void CEngine::Update(const float time)
{
	this->m_player->Update(time);
	this->m_bspWorld.Slide(this->m_player->GetShape(), this->m_player->GetGravityMovement(), true);
	this->m_bspWorld.Slide(this->m_player->GetShape(), this->m_player->GetMovement(), false);
	this->m_player->UpdateCamera();
	this->m_dynamicWorld.NextAction();
	this->m_dynamicWorld.Update(time, this->m_cameraM->GetCamera());
	this->m_lightM->Update(this->m_cameraM->GetCamera());
	this->m_particleM->Update(time, this->m_cameraM->GetCamera());

	// Commit commands changes from dynamicWorld to bspWorld
	std::vector<SObjectCommand> * commands = this->m_dynamicWorld.GetObjectsCommandList();
	for(unsigned int i = 0 ; i < commands->size() ; i++)
	{
		switch(commands->at(i).m_type)
		{
		// Enable brushes with group id = m_value
		case OBJECT_COMMAND_ENABLE_BRUSH:
			this->m_bspWorld.ChangeBrushesCollision(commands->at(i).m_value, true);
			break;

		// Disable brushes with group id = m_value
		case OBJECT_COMMAND_DISABLE_BRUSH:
			this->m_bspWorld.ChangeBrushesCollision(commands->at(i).m_value, false);
			break;
		}
	}
	// Erase the commands as they are no longer needed for this frame
	commands->clear();
}

/**
* Render the engine
* Call the right technic depending on the graphic options
*/
void CEngine::Render()
{
	switch(this->m_renderTechnic)
	{
	case ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_USE_SHADOWS:
		RenderDeferredShading();
		break;

	case ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_NO_SHADOWS:
		RenderDeferredShadingNoShadows();
		break;

	/*case ENGINE_RENDER_TECHNIC_FORWARD_USE_SHADOWS:
		RenderForward();
		break;

	case ENGINE_RENDER_TECHNIC_FORWARD_NO_SHADOWS:
		RenderForwardNoShadows();
		break;*/
	};
}

/**
* Destroy the engine states
*/
void CEngine::Destroy()
{
	SAFE_RELEASE(this->m_staticWorldVertexBuffer);
	SAFE_RELEASE(this->m_staticLightVertexBuffer);
	SAFE_RELEASE(this->m_dynamicLightVertexBuffer);
	SAFE_RELEASE(this->m_vParticleInstance);
	SAFE_RELEASE(this->m_screenQuadBuffer);
	SAFE_RELEASE(this->m_vParticle);
	SAFE_RELEASE(this->m_vDeclaration);
	SAFE_RELEASE(this->m_vScreenQuadDeclaration);
	SAFE_RELEASE(this->m_vLightDeclaration);
	SAFE_RELEASE(this->m_vParticleDeclaration);
	SAFE_RELEASE(this->m_screenQuadInstance);
	SAFE_RELEASE(this->m_normalRenderTarget);
	SAFE_RELEASE(this->m_albedoRenderTarget);
	SAFE_RELEASE(this->m_positionRenderTarget);
	SAFE_RELEASE(this->m_lightningRenderTarget);
	SAFE_RELEASE(this->m_shadowRenderTarget);
	SAFE_RELEASE(this->m_shadowDepthStencil);
	SAFE_RELEASE(this->m_normalTexture);
	SAFE_RELEASE(this->m_albedoTexture);
	SAFE_RELEASE(this->m_positionTexture);
	SAFE_RELEASE(this->m_lightningTexture);
	SAFE_RELEASE(this->m_shadowTexture);

	for(int i = 0 ; i < LIGHT_SHADOW_MAXIMUM_SOURCES ; i++)
	{
		SAFE_RELEASE(this->m_lightsShadowRenderTarget[i]);
		SAFE_RELEASE(this->m_lightsShadowTexture[i]);
	}
}

/**
* Action performed when a device lost event is detected
* Free any ressource that need to be before reseting the device
*/
void CEngine::OnLostDevice()
{
	Destroy();
	if(this->m_levelLoaded)
		this->m_dynamicWorld.OnLostDevice();
}

/**
* Action performed when a device lost reset is detected
* Allocate the necessary ressources
*/
void CEngine::OnResetDevice()
{
	Initialize();
	if(this->m_levelLoaded)
	{
		this->m_dynamicWorld.OnResetDevice();
		InitializeBuffers();
	}
}

/**
* Initialize the buffers that needs level information
*/
void CEngine::InitializeBuffers()
{
	SAFE_RELEASE(this->m_staticWorldVertexBuffer);
	SAFE_RELEASE(this->m_staticLightVertexBuffer);
	SAFE_RELEASE(this->m_dynamicLightVertexBuffer);
	SAFE_RELEASE(this->m_vParticleInstance);

	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(this->m_staticWorldVertexBufferSize * sizeof(SVertexElement), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_staticWorldVertexBuffer, NULL), "Static World Vertex Buffer");
	_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(PARTICLESYSTEM_MAXIMUM_EMITTERS * EMITTER_MAXIMUM_PARTICLES * sizeof(SVertexParticleInstance), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_vParticleInstance, NULL), "Particle Instance Vertex Buffer");

	// Vertex buffers for shadows generation
	if(this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
	{
		_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(this->m_staticLightVertexBufferSize * sizeof(SVertexLightPass), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_staticLightVertexBuffer, NULL), "Static Light Vertex Buffer");

		// Dynamic Light Vertex Buffer if dynamic objects exists
		if(this->m_dynamicLightVertexBufferSize > 0)
			_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(this->m_dynamicLightVertexBufferSize * sizeof(SVertexLightPass), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_dynamicLightVertexBuffer, NULL), "Dynamic Light Vertex Buffer");
		
		SVertexLightPass * pLightVertices;

		// Precompute the static part of the light buffer
		this->m_bspWorld.Render(NULL, SCENE_RENDER_GEOMETRY_LIGHTS);
		this->m_staticLightVertexBuffer->Lock(0, 0, (void**)&pLightVertices, D3DLOCK_DISCARD);
			memcpy(pLightVertices, this->m_bspWorld.GetVisibleLightFaces(), this->m_bspWorld.GetVisibleLightFacesCounter() * sizeof(SVertexLightPass));
		this->m_staticLightVertexBuffer->Unlock();
	}
}

/**
* Initialize the sampler stats for the shadow map pass
*/
void CEngine::InitializeShadowSamplerStates()
{
	for(int i = 0 ; i < 3 ; i++)
	{
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 0);
	}
}

/**
* Initialize the sampler stats for the rendering pass
*/
void CEngine::InitializeRenderSamplerStates()
{
	// Position, Shadow Map states
	for(int i = 2 ; i < LIGHT_SHADOW_MAXIMUM_SOURCES + 4 ; i++)
	{
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 0);
	}

	// Albedo, Normal
	for(int i = 0 ; i < 2 ; i++)
	{
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, this->m_optionM->GetAnisotropicFiltering(OPTIONMANAGER_CONFIGURATION_ACTIVE));
	}
}

/**
* Fill the shadow map
*/
void CEngine::FillShadowMap()
{
	SVertexLightPass * pLightVertices;

	this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vLightDeclaration);
	this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFillShadowMap->GetVertexShader());
	this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFillShadowMap->GetPixelShader());

	this->m_deviceM->GetDevice()->SetTexture(0, NULL);
	this->m_deviceM->GetDevice()->SetTexture(1, NULL);
	this->m_deviceM->GetDevice()->SetTexture(2, NULL);
	this->m_deviceM->GetDevice()->SetTexture(3, NULL);
	this->m_deviceM->GetDevice()->SetTexture(4, NULL);
	this->m_deviceM->GetDevice()->SetDepthStencilSurface(this->m_shadowDepthStencil);

	for(int i = 0 ; i < this->m_lightM->GetNumberOfShadowCasters() ; i++)
	{
		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_lightsShadowRenderTarget[i]);
		this->m_deviceM->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);

		D3DXMATRIX viewProjection = this->m_lightM->GetShadowLightViewProjMatrix()[i];
		this->m_shaderFillShadowMap->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFillShadowMap->GetVertexHandle(DSSHADERFILLSHADOWMAP_HANDLE_VERTEX_VIEWPROJ), &viewProjection);		

		// Static part
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_staticLightVertexBuffer, 0, sizeof(SVertexLightPass));
		this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_bspWorld.GetVisibleLightFacesCounter() / 3);

		// Dynamic part
		if(this->m_dynamicWorld.GetFacesCounter() > 0)
		{
			this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_dynamicLightVertexBuffer, 0, sizeof(SVertexLightPass));

			this->m_dynamicLightVertexBuffer->Lock(0, 0, (void**)&pLightVertices, D3DLOCK_DISCARD);
				memcpy(pLightVertices, this->m_dynamicWorld.GetLightsGeometry(), this->m_dynamicWorld.GetFacesCounter() * sizeof(SVertexLightPass));
			this->m_dynamicLightVertexBuffer->Unlock();

			this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_dynamicWorld.GetFacesCounter() / 3);
		}
	}
}

/**
* Render with the forward technic
*/
/*void CEngine::RenderForward()
{
	this->m_bspWorld.Render(this->m_cameraM->GetCamera(), SCENE_RENDER_GEOMETRY_FACES);
	int elementToDraw = this->m_bspWorld.GetVisibleFacesCounter();

	if(elementToDraw > 0)
	{
		SVertexElement * pVertices;
		D3DXMATRIX viewProjection;
		int elementIndex = 0;
		int callIndex = 0;

		InitializeShadowSamplerStates();

		// Shadow Prepass : Fill the shadow map for the lights
		FillShadowMap();

		InitializeRenderSamplerStates();

		this->m_deviceM->GetDevice()->SetDepthStencilSurface(this->m_deviceM->GetMainDepthStencilSurface());
		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_deviceM->GetMainRenderTarget());

		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vDeclaration);
		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFirstRender->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFirstRender->GetPixelShader());
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_staticWorldVertexBuffer, 0, sizeof(SVertexElement));

		for(int i = 2 ; i < this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) + 2 ; i++)
		{
			if(this->m_lightM->GetNumberOfVisibleLights() >= i - 1)
				this->m_deviceM->GetDevice()->SetTexture(i, this->m_lightsShadowTexture[i - 2]);
		}

		viewProjection = this->m_cameraM->GetViewProjMatrix();

		// Basic information
		this->m_shaderFirstRender->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(FORWARDSHADER_HANDLE_VERTEX_VIEWPROJ), &viewProjection);

		// Needed for the fog
		D3DXVECTOR4 cameraPositionDistance = D3DXVECTOR4(this->m_cameraM->GetCamera()->m_position.x, this->m_cameraM->GetCamera()->m_position.y, this->m_cameraM->GetCamera()->m_position.z, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		this->m_shaderFirstRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE), &cameraPositionDistance);

		// Lights information
		this->m_shaderFirstRender->GetConstantTableVS()->SetMatrixArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(FORWARDSHADER_HANDLE_VERTEX_LIGHTVIEWPROJ), this->m_lightM->GetVisibleLightViewProjMatrix(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTableVS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(FORWARDSHADER_HANDLE_VERTEX_LIGHTSHADOWCASTERINDEXES), this->m_lightM->GetChoosenShadowLights(), LIGHT_SHADOW_MAXIMUM_SOURCES);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTPOSITIONANGLE), this->m_lightM->GetVisibleLightPositionSpotAngle(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH), this->m_lightM->GetVisibleLightDirectionSpotLength(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTCOLOR), this->m_lightM->GetVisibleLightColor(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTFLAGS), this->m_lightM->GetVisibleLightsFlags(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTSHADOWCASTERINDEXES), this->m_lightM->GetChoosenShadowLights(), LIGHT_SHADOW_MAXIMUM_SOURCES);

		this->m_shaderFirstRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_SHADOWCONFIGURATION), &this->m_configuration->m_shadows);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADER_HANDLE_PIXEL_LIGHTNINGCONFIGURATION), &this->m_configuration->m_lightning);
		
		// Draw the static part
		while(elementIndex < elementToDraw)
		{
			this->m_deviceM->GetDevice()->SetTexture(0, &this->m_bspWorld.GetBatchedVisibleTexturesDiffuse()[elementIndex][0]);
			this->m_deviceM->GetDevice()->SetTexture(1, &this->m_bspWorld.GetBatchedVisibleTexturesNormal()[elementIndex][0]);

			this->m_staticWorldVertexBuffer->Lock(0,this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement), (void**)&pVertices, D3DLOCK_DISCARD);
				memcpy(pVertices, &this->m_bspWorld.GetFinalGeometry()[elementIndex], this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement));
			this->m_staticWorldVertexBuffer->Unlock();

			this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] / 3);

			elementIndex += this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex];
			callIndex++;
		}

		// Draw the Dynamic part

		CObjectEntity * tmpObj = this->m_dynamicWorld.GetObjects();
		while(tmpObj != NULL)
		{
			if(tmpObj->IsAlive() && tmpObj->GetRender() != NULL)
			{
				for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
				{
					if(tmpObj->GetRender()->GetVisibleGeometrySize(i) > 0)
					{
						this->m_deviceM->GetDevice()->SetStreamSource(0, tmpObj->GetRender()->GetVertexBuffer(i), 0, sizeof(SVertexElement));
						this->m_deviceM->GetDevice()->SetTexture(0, &tmpObj->GetRender()->GetTextureDiffuse(i)[0]);
						this->m_deviceM->GetDevice()->SetTexture(1, &tmpObj->GetRender()->GetTextureNormal(i)[0]);
						this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, tmpObj->GetRender()->GetVisibleGeometrySize(i) / 3);
					}
				}
			}
			tmpObj = tmpObj->m_next;
		}

		this->m_deviceM->GetDevice()->SetVertexShader(NULL);
		this->m_deviceM->GetDevice()->SetPixelShader(NULL);
	}

	// Particles part
	this->m_particleM->FillParticleInstance(this->m_cameraM->GetCamera());
	if(this->m_particleM->GetParticleBufferUsedSize() > 0)
	{
		D3DXMATRIX matTranspose, matBillboard, viewProjection;

		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);

		this->m_deviceM->GetDevice()->SetTexture(0, this->m_particleM->GetTextureDiffuse());
		this->m_deviceM->GetDevice()->SetTexture(1, this->m_particleM->GetTextureNormal());

		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderParticles->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderParticles->GetPixelShader());
		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vParticleDeclaration);
		this->m_deviceM->GetDevice()->SetIndices(this->m_screenQuadInstance);
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_vParticle, 0, sizeof(SVertexParticle));
		this->m_deviceM->GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | this->m_particleM->GetParticleBufferUsedSize() * 4);
		this->m_deviceM->GetDevice()->SetStreamSource(1, this->m_vParticleInstance, 0, sizeof(SVertexParticleInstance));
		this->m_deviceM->GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 4);

		viewProjection = this->m_cameraM->GetViewProjMatrix();

		// Billboard calculation
		D3DXMatrixTranspose(&matTranspose, &this->m_cameraM->GetViewMatrix());
		D3DXMatrixIdentity(&matBillboard);

		matBillboard._11 = matTranspose._11;
		matBillboard._12 = matTranspose._12;
		matBillboard._13 = matTranspose._13;
		matBillboard._21 = matTranspose._21;
		matBillboard._22 = matTranspose._22;
		matBillboard._23 = matTranspose._23;
		matBillboard._31 = matTranspose._31;
		matBillboard._32 = matTranspose._32;
		matBillboard._33 = matTranspose._33;

		this->m_shaderParticles->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetVertexHandle(FORWARDSHADERPARTICLES_HANDLE_VERTEX_BILLBOARD), &matBillboard);
		this->m_shaderParticles->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetVertexHandle(FORWARDSHADERPARTICLES_HANDLE_VERTEX_VIEWPROJ), &viewProjection);
		this->m_shaderParticles->GetConstantTableVS()->SetMatrixArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetVertexHandle(FORWARDSHADERPARTICLES_HANDLE_VERTEX_LIGHTVIEWPROJ), this->m_lightM->GetVisibleLightViewProjMatrix(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderParticles->GetConstantTableVS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetVertexHandle(FORWARDSHADERPARTICLES_HANDLE_VERTEX_LIGHTSHADOWCASTERINDEXES), this->m_lightM->GetChoosenShadowLights(), LIGHT_SHADOW_MAXIMUM_SOURCES);

		// Needed for the fog
		D3DXVECTOR4 cameraPositionDistance = D3DXVECTOR4(this->m_cameraM->GetCamera()->m_position.x, this->m_cameraM->GetCamera()->m_position.y, this->m_cameraM->GetCamera()->m_position.z, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		this->m_shaderParticles->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE), &cameraPositionDistance);

		this->m_shaderParticles->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTPOSITIONANGLE), this->m_lightM->GetVisibleLightPositionSpotAngle(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderParticles->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH), this->m_lightM->GetVisibleLightDirectionSpotLength(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderParticles->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTCOLOR), this->m_lightM->GetVisibleLightColor(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderParticles->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTFLAGS), this->m_lightM->GetVisibleLightsFlags(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderParticles->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTSHADOWCASTERINDEXES), this->m_lightM->GetChoosenShadowLights(), LIGHT_SHADOW_MAXIMUM_SOURCES);

		this->m_shaderParticles->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_SHADOWCONFIGURATION), &this->m_configuration->m_shadows);
		this->m_shaderParticles->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderParticles->GetPixelHandle(FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTNINGCONFIGURATION), &this->m_configuration->m_lightning);

		SVertexParticleInstance * pVertices;
		this->m_vParticleInstance->Lock(0, this->m_particleM->GetParticleBufferUsedSize() * sizeof(SVertexParticleInstance),(void**)&pVertices, NULL);
			memcpy(pVertices, this->m_particleM->GetParticleInstances(), this->m_particleM->GetParticleBufferUsedSize() * sizeof(SVertexParticleInstance));
		this->m_vParticleInstance->Unlock();

		this->m_deviceM->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		this->m_deviceM->GetDevice()->SetStreamSourceFreq( 0, 1 );
		this->m_deviceM->GetDevice()->SetStreamSourceFreq( 1, 1 );
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}

	this->m_player->RenderHud();
}*/

/**
* Render with the forward technic
*/
/*void CEngine::RenderForwardNoShadows()
{
	this->m_bspWorld.Render(this->m_cameraM->GetCamera(), SCENE_RENDER_GEOMETRY_FACES);
	int elementToDraw = this->m_bspWorld.GetVisibleFacesCounter();

	if(elementToDraw > 0)
	{
		SVertexElement * pVertices;
		D3DXMATRIX viewProjection;
		int elementIndex = 0;
		int callIndex = 0;

		InitializeRenderSamplerStates();

		this->m_deviceM->GetDevice()->SetDepthStencilSurface(this->m_deviceM->GetMainDepthStencilSurface());
		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_deviceM->GetMainRenderTarget());

		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vDeclaration);
		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFirstRender->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFirstRender->GetPixelShader());
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_staticWorldVertexBuffer, 0, sizeof(SVertexElement));

		viewProjection = this->m_cameraM->GetViewProjMatrix();

		// Basic information
		this->m_shaderFirstRender->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(FORWARDSHADERNOSHADOWS_HANDLE_VERTEX_VIEWPROJ), &viewProjection);

		// Needed for the fog
		D3DXVECTOR4 cameraPositionDistance = D3DXVECTOR4(this->m_cameraM->GetCamera()->m_position.x, this->m_cameraM->GetCamera()->m_position.y, this->m_cameraM->GetCamera()->m_position.z, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		this->m_shaderFirstRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE), &cameraPositionDistance);

		// Lights information (Blinn-Phong rendering)
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_LIGHTPOSITIONANGLE), this->m_lightM->GetVisibleLightPositionSpotAngle(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH), this->m_lightM->GetVisibleLightDirectionSpotLength(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_LIGHTCOLOR), this->m_lightM->GetVisibleLightColor(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_LIGHTFLAGS), this->m_lightM->GetVisibleLightsFlags(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderFirstRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(FORWARDSHADERNOSHADOWS_HANDLE_PIXEL_LIGHTNINGCONFIGURATION), &this->m_configuration->m_lightning);
		
		// Draw the static part
		while(elementIndex < elementToDraw)
		{
			this->m_deviceM->GetDevice()->SetTexture(0, &this->m_bspWorld.GetBatchedVisibleTexturesDiffuse()[elementIndex][0]);
			this->m_deviceM->GetDevice()->SetTexture(1, &this->m_bspWorld.GetBatchedVisibleTexturesNormal()[elementIndex][0]);

			this->m_staticWorldVertexBuffer->Lock(0,this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement), (void**)&pVertices, D3DLOCK_DISCARD);
				memcpy(pVertices, &this->m_bspWorld.GetFinalGeometry()[elementIndex], this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement));
			this->m_staticWorldVertexBuffer->Unlock();

			this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] / 3);

			elementIndex += this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex];
			callIndex++;
		}

		// Draw the Dynamic part

		CObjectEntity * tmpObj = this->m_dynamicWorld.GetObjects();
		while(tmpObj != NULL)
		{
			if(tmpObj->IsAlive() && tmpObj->GetRender() != NULL)
			{
				for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
				{
					if(tmpObj->GetRender()->GetVisibleGeometrySize(i) > 0)
					{
						this->m_deviceM->GetDevice()->SetStreamSource(0, tmpObj->GetRender()->GetVertexBuffer(i), 0, sizeof(SVertexElement));
						this->m_deviceM->GetDevice()->SetTexture(0, &tmpObj->GetRender()->GetTextureDiffuse(i)[0]);
						this->m_deviceM->GetDevice()->SetTexture(1, &tmpObj->GetRender()->GetTextureNormal(i)[0]);
						this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, tmpObj->GetRender()->GetVisibleGeometrySize(i) / 3);
					}
				}
			}
			tmpObj = tmpObj->m_next;
		}

		this->m_deviceM->GetDevice()->SetVertexShader(NULL);
		this->m_deviceM->GetDevice()->SetPixelShader(NULL);
	}

	// Particles part
	this->m_particleM->FillParticleInstance(this->m_cameraM->GetCamera());
	if(this->m_particleM->GetParticleBufferUsedSize() > 0)
	{

	}

	this->m_player->RenderHud();
}*/

/**
* Render with the Deferred Shading technic
*/
void CEngine::RenderDeferredShading()
{
	this->m_bspWorld.Render(this->m_cameraM->GetCamera(), SCENE_RENDER_GEOMETRY_FACES);
	int elementToDraw = this->m_bspWorld.GetVisibleFacesCounter();

	if(elementToDraw > 0)
	{
		SVertexElement * pVertices;
		D3DXMATRIX viewMatrix;
		D3DXMATRIX viewProjectionMatrix;
		int elementIndex = 0;
		int callIndex = 0;

		InitializeShadowSamplerStates();

		// Deferred Shading Prepass : Fill the shadow map for the lights
		FillShadowMap();

		// Deferred Shading first pass : Fill the Render Targets
		InitializeRenderSamplerStates();

		this->m_deviceM->GetDevice()->SetDepthStencilSurface(this->m_deviceM->GetMainDepthStencilSurface());
		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_albedoRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(1, this->m_normalRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(2, this->m_positionRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(3, this->m_shadowRenderTarget);

		for(int i = 2 ; i < this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) + 2 ; i++)
		{
			if(this->m_lightM->GetNumberOfVisibleLights() >= i - 1)
				this->m_deviceM->GetDevice()->SetTexture(i, this->m_lightsShadowTexture[i - 2]);
		}

		this->m_deviceM->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vDeclaration);
		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFillWorld->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFillWorld->GetPixelShader());
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_staticWorldVertexBuffer, 0, sizeof(SVertexElement));

		viewMatrix = this->m_cameraM->GetViewMatrix();
		viewProjectionMatrix = this->m_cameraM->GetViewProjMatrix();

		// Vertex Shader
		this->m_shaderFillWorld->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetVertexHandle(DSSHADERFILLWORLD_HANDLE_VERTEX_VIEWMATRIX), &viewMatrix);
		this->m_shaderFillWorld->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetVertexHandle(DSSHADERFILLWORLD_HANDLE_VERTEX_VIEWPROJMATRIX), &viewProjectionMatrix);
		this->m_shaderFillWorld->GetConstantTableVS()->SetMatrixArray(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetVertexHandle(DSSHADERFILLWORLD_HANDLE_VERTEX_LIGHTVIEWPROJ), this->m_lightM->GetShadowLightViewProjMatrix(), LIGHT_SHADOW_MAXIMUM_SOURCES);
		this->m_shaderFillWorld->GetConstantTableVS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetVertexHandle(DSSHADERFILLWORLD_HANDLE_VERTEX_SHADOWCASTER), this->m_lightM->GetShadowCasters(), LIGHT_SHADOW_MAXIMUM_SOURCES);

		// Pixel shader
		this->m_shaderFillWorld->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetPixelHandle(DSSHADERFILLWORLD_HANDLE_PIXEL_SHADOWCONFIGURATION), &this->m_configuration->m_shadows);
		this->m_shaderFillWorld->GetConstantTablePS()->SetFloat(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetPixelHandle(DSSHADERFILLWORLD_HANDLE_PIXEL_VIEWDISTANCE), this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		this->m_shaderFillWorld->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderFillWorld->GetPixelHandle(DSSHADERFILLWORLD_HANDLE_PIXEL_SHADOWCASTER), this->m_lightM->GetShadowCasters(), LIGHT_SHADOW_MAXIMUM_SOURCES);
		
		// Draw the static part
		while(elementIndex < elementToDraw)
		{
			this->m_deviceM->GetDevice()->SetTexture(0, &this->m_bspWorld.GetBatchedVisibleTexturesDiffuse()[elementIndex][0]);
			this->m_deviceM->GetDevice()->SetTexture(1, &this->m_bspWorld.GetBatchedVisibleTexturesNormal()[elementIndex][0]);

			this->m_staticWorldVertexBuffer->Lock(0,this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement), (void**)&pVertices, D3DLOCK_DISCARD);
				memcpy(pVertices, &this->m_bspWorld.GetFinalGeometry()[elementIndex], this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement));
			this->m_staticWorldVertexBuffer->Unlock();

			this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] / 3);

			elementIndex += this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex];
			callIndex++;
		}

		// Draw the Dynamic part

		CObjectEntity * tmpObj = this->m_dynamicWorld.GetObjects();
		while(tmpObj != NULL)
		{
			if(tmpObj->IsAlive() && tmpObj->GetRender() != NULL)
			{
				for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
				{
					if(tmpObj->GetRender()->GetVisibleGeometrySize(i) > 0)
					{
						this->m_deviceM->GetDevice()->SetStreamSource(0, tmpObj->GetRender()->GetVertexBuffer(i), 0, sizeof(SVertexElement));
						this->m_deviceM->GetDevice()->SetTexture(0, &tmpObj->GetRender()->GetTextureDiffuse(i)[0]);
						this->m_deviceM->GetDevice()->SetTexture(1, &tmpObj->GetRender()->GetTextureNormal(i)[0]);
						this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, tmpObj->GetRender()->GetVisibleGeometrySize(i) / 3);
					}
				}
			}
			tmpObj = tmpObj->m_next;
		}

		// Fill the Screen Quad
		SVertexQuad verts[4] =
		{
			{ D3DXVECTOR3(  1, -1, 0 ), D3DXVECTOR2(1,1), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_BOTTOM_RIGHT] },
			{ D3DXVECTOR3( -1, -1, 0 ), D3DXVECTOR2(0,1), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_BOTTOM_LEFT] },
			{ D3DXVECTOR3( -1,  1, 0 ), D3DXVECTOR2(0,0), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_TOP_LEFT] },
			{ D3DXVECTOR3(  1,  1, 0 ), D3DXVECTOR2(1,0), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_TOP_RIGHT] }
		};
		void * pVertex = NULL;
		this->m_screenQuadBuffer->Lock(0,4 * sizeof(SVertexQuad),(void**)&pVertex,0);
			memcpy(pVertex,verts,4 * sizeof(SVertexQuad));
		this->m_screenQuadBuffer->Unlock();


		// Lightning passes

		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_MAX);

		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_lightningRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(1, NULL);
		this->m_deviceM->GetDevice()->SetRenderTarget(2, NULL);
		this->m_deviceM->GetDevice()->SetRenderTarget(3, NULL);
		this->m_deviceM->GetDevice()->SetRenderTarget(4, NULL);
		this->m_deviceM->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		if(this->m_lightM->GetNumberOfVisibleLights() > 0)
		{
			int shadowCount = 0;
			int noShadow = -1;

			this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFillLightning->GetVertexShader());
			this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFillLightning->GetPixelShader());
			this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vScreenQuadDeclaration);
			this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_screenQuadBuffer, 0, sizeof(SVertexQuad));
			this->m_deviceM->GetDevice()->SetIndices(this->m_screenQuadInstance);
			this->m_deviceM->GetDevice()->SetTexture(0, this->m_normalTexture);
			this->m_deviceM->GetDevice()->SetTexture(1, this->m_positionTexture);
			this->m_deviceM->GetDevice()->SetTexture(2, this->m_shadowTexture);

			// Disable any filetring to avoid line glitches
			/*this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 0);*/

			// Global variables
			D3DXVECTOR4 cameraPositionDistance = D3DXVECTOR4(this->m_cameraM->GetCamera()->m_position.x, this->m_cameraM->GetCamera()->m_position.y, this->m_cameraM->GetCamera()->m_position.z, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
			this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE), &cameraPositionDistance);

			this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_SHADOWCONFIGURATION), &this->m_configuration->m_shadows);
			this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_LIGHTNINGCONFIGURATION), &this->m_configuration->m_lightning);

			for(int i = 0 ; i < this->m_lightM->GetNumberOfVisibleLights() ; i++)
			{
				this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_LIGHTPOSITIONANGLE), &this->m_lightM->GetVisibleLightPositionSpotAngle()[i]);
				this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH), &this->m_lightM->GetVisibleLightDirectionSpotLength()[i]);
				this->m_shaderFillLightning->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_LIGHTCOLOR), &this->m_lightM->GetVisibleLightColor()[i]);

				if(shadowCount < this->m_lightM->GetNumberOfShadowCasters() && this->m_lightM->GetChoosenShadowLights()[shadowCount] == this->m_lightM->GetVisibleLightsIndex()[i])
				{
					this->m_shaderFillLightning->GetConstantTablePS()->SetInt(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_SHADOWCASTER), shadowCount);
					shadowCount++;
				}
				else
					this->m_shaderFillLightning->GetConstantTablePS()->SetInt(this->m_deviceM->GetDevice(), this->m_shaderFillLightning->GetPixelHandle(DSSHADERFILLLIGHTNING_HANDLE_PIXEL_SHADOWCASTER), noShadow);

				this->m_deviceM->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
			}
		}


		// Final pass

		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
		this->m_deviceM->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_deviceM->GetMainRenderTarget());

		// Disable any filetring to avoid line glitches
		/*for(int i = 0 ; i < 2 ; i++)
		{
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			this->m_deviceM->GetDevice()->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 0);
		}*/

		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderRenderWorld->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderRenderWorld->GetPixelShader());
		this->m_deviceM->GetDevice()->SetTexture(0, this->m_albedoTexture);
		this->m_deviceM->GetDevice()->SetTexture(1, this->m_lightningTexture);

		this->m_deviceM->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		this->m_deviceM->GetDevice()->SetVertexShader(NULL);
		this->m_deviceM->GetDevice()->SetPixelShader(NULL);
	}

	this->m_player->RenderHud();

	/*this->m_faceTextStream.str("");
	this->m_faceTextStream << "Tri Count: " << (this->m_bspWorld.GetVisibleFacesCounter() / 3) << " / " << this->m_bspWorld.GetFacesCounter() << "\n";
	this->m_fontM->DrawText("FACECOUNT_FONT", this->m_faceTextStream.str(), FONTMANAGER_ALIGN_LEFT, 20, 20, 200, 200, 0xFFFFFFFF);*/
}

/**
* Render with the Deferred Shading technic but without shadows
*/
void CEngine::RenderDeferredShadingNoShadows()
{
	/*this->m_bspWorld.Render(this->m_cameraM->GetCamera(), SCENE_RENDER_GEOMETRY_FACES);
	int elementToDraw = this->m_bspWorld.GetVisibleFacesCounter();

	if(elementToDraw > 0)
	{
		SVertexElement * pVertices;
		D3DXMATRIX viewMatrix;
		D3DXMATRIX viewProjectionMatrix;
		int elementIndex = 0;
		int callIndex = 0;

		// Deferred Shading first pass : Fill the Render Targets
		InitializeRenderSamplerStates();

		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_albedoRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(1, this->m_normalRenderTarget);
		this->m_deviceM->GetDevice()->SetRenderTarget(2, this->m_positionRenderTarget);

		this->m_deviceM->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, RGB(0, 0, 0), 1.0f, 0);

		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vDeclaration);
		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderFirstRender->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderFirstRender->GetPixelShader());
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_staticWorldVertexBuffer, 0, sizeof(SVertexElement));

		viewMatrix = this->m_cameraM->GetViewMatrix();
		viewProjectionMatrix = this->m_cameraM->GetViewProjMatrix();

		// Basic information
		this->m_shaderFirstRender->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(DSSHADERFILLWORLDNOSHADOWS_HANDLE_VERTEX_VIEWMATRIX), &viewMatrix);
		this->m_shaderFirstRender->GetConstantTableVS()->SetMatrix(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetVertexHandle(DSSHADERFILLWORLDNOSHADOWS_HANDLE_VERTEX_VIEWPROJMATRIX), &viewProjectionMatrix);
		this->m_shaderFirstRender->GetConstantTablePS()->SetFloat(this->m_deviceM->GetDevice(), this->m_shaderFirstRender->GetPixelHandle(DSSHADERFILLWORLDNOSHADOWS_HANDLE_PIXEL_VIEWDISTANCE), this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		
		// Draw the static part
		while(elementIndex < elementToDraw)
		{
			this->m_deviceM->GetDevice()->SetTexture(0, &this->m_bspWorld.GetBatchedVisibleTexturesDiffuse()[elementIndex][0]);
			this->m_deviceM->GetDevice()->SetTexture(1, &this->m_bspWorld.GetBatchedVisibleTexturesNormal()[elementIndex][0]);

			this->m_staticWorldVertexBuffer->Lock(0,this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement), (void**)&pVertices, D3DLOCK_DISCARD);
				memcpy(pVertices, &this->m_bspWorld.GetFinalGeometry()[elementIndex], this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] * sizeof(SVertexElement));
			this->m_staticWorldVertexBuffer->Unlock();

			this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex] / 3);

			elementIndex += this->m_bspWorld.GetBatchedVisibleFacesNumber()[callIndex];
			callIndex++;
		}

		// Draw the Dynamic part
		CObjectEntity * tmpObj = this->m_dynamicWorld.GetObjects();
		while(tmpObj != NULL)
		{
			if(tmpObj->IsAlive() && tmpObj->GetRender() != NULL)
			{
				for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
				{
					if(tmpObj->GetRender()->GetVisibleGeometrySize(i) > 0)
					{
						this->m_deviceM->GetDevice()->SetStreamSource(0, tmpObj->GetRender()->GetVertexBuffer(i), 0, sizeof(SVertexElement));
						this->m_deviceM->GetDevice()->SetTexture(0, &tmpObj->GetRender()->GetTextureDiffuse(i)[0]);
						this->m_deviceM->GetDevice()->SetTexture(1, &tmpObj->GetRender()->GetTextureNormal(i)[0]);
						this->m_deviceM->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, tmpObj->GetRender()->GetVisibleGeometrySize(i) / 3);
					}
				}
			}
			tmpObj = tmpObj->m_next;
		}


		// Deferred Shading second pass : Render the scene

		// Fill the Screen Quad
		SVertexQuad verts[4] =
		{
			{ D3DXVECTOR3(  1, -1, 0 ), D3DXVECTOR2(1,1), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_BOTTOM_RIGHT] },
			{ D3DXVECTOR3( -1, -1, 0 ), D3DXVECTOR2(0,1), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_BOTTOM_LEFT] },
			{ D3DXVECTOR3( -1,  1, 0 ), D3DXVECTOR2(0,0), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_TOP_LEFT] },
			{ D3DXVECTOR3(  1,  1, 0 ), D3DXVECTOR2(1,0), this->m_cameraM->GetCamera()->GetFrustumLocalPoints()[FRUSTUM_POINT_FAR_TOP_RIGHT] }
		};
		void * pVertex = NULL;
		this->m_screenQuadBuffer->Lock(0,4 * sizeof(SVertexQuad),(void**)&pVertex,0);
			memcpy(pVertex,verts,4 * sizeof(SVertexQuad));
		this->m_screenQuadBuffer->Unlock();

		this->m_deviceM->GetDevice()->SetRenderTarget(0, this->m_deviceM->GetMainRenderTarget());
		this->m_deviceM->GetDevice()->SetRenderTarget(1, NULL);
		this->m_deviceM->GetDevice()->SetRenderTarget(2, NULL);

		// Disable any filetring to avoid line glitches
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		this->m_deviceM->GetDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 0);

		this->m_deviceM->GetDevice()->SetVertexShader(this->m_shaderSecondRender->GetVertexShader());
		this->m_deviceM->GetDevice()->SetPixelShader(this->m_shaderSecondRender->GetPixelShader());
		this->m_deviceM->GetDevice()->SetVertexDeclaration(this->m_vScreenQuadDeclaration);
		this->m_deviceM->GetDevice()->SetStreamSource(0, this->m_screenQuadBuffer, 0, sizeof(SVertexQuad));
		this->m_deviceM->GetDevice()->SetIndices(this->m_screenQuadInstance);
		this->m_deviceM->GetDevice()->SetTexture(0, this->m_albedoTexture);
		this->m_deviceM->GetDevice()->SetTexture(1, this->m_normalTexture);
		this->m_deviceM->GetDevice()->SetTexture(2, this->m_positionTexture);

		// Needed for the fog
		D3DXVECTOR4 cameraPositionDistance = D3DXVECTOR4(this->m_cameraM->GetCamera()->m_position.x, this->m_cameraM->GetCamera()->m_position.y, this->m_cameraM->GetCamera()->m_position.z, this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_ACTIVE));
		this->m_shaderSecondRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE), &cameraPositionDistance);

		// Lights information (Blinn-Phong rendering)
		this->m_shaderSecondRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_LIGHTPOSITIONANGLE), this->m_lightM->GetVisibleLightPositionSpotAngle(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderSecondRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH), this->m_lightM->GetVisibleLightDirectionSpotLength(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderSecondRender->GetConstantTablePS()->SetVectorArray(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_LIGHTCOLOR), this->m_lightM->GetVisibleLightColor(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderSecondRender->GetConstantTablePS()->SetIntArray(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_LIGHTFLAGS), this->m_lightM->GetVisibleLightsFlags(), MAXIMUM_VISIBLE_LIGHTS);
		this->m_shaderSecondRender->GetConstantTablePS()->SetVector(this->m_deviceM->GetDevice(), this->m_shaderSecondRender->GetPixelHandle(DSSHADERRENDERWORLDNOSHADOWS_HANDLE_PIXEL_LIGHTNINGCONFIGURATION), &this->m_configuration->m_lightning);

		this->m_deviceM->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		this->m_deviceM->GetDevice()->SetVertexShader(NULL);
		this->m_deviceM->GetDevice()->SetPixelShader(NULL);
	}

	this->m_player->RenderHud();*/

	/*this->m_faceTextStream.str("");
	this->m_faceTextStream << "Tri Count: " << (this->m_bspWorld.GetVisibleFacesCounter() / 3) << " / " << this->m_bspWorld.GetFacesCounter() << "\n";
	this->m_fontM->DrawText("FACECOUNT_FONT", this->m_faceTextStream.str(), FONTMANAGER_ALIGN_LEFT, 20, 20, 200, 200, 0xFFFFFFFF);*/
}
