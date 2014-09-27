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

#ifndef __CENGINE_H
#define __CENGINE_H

#define ENGINE_CONFIGURATION_FILE "engine.ini"
#define ENGINE_CONFIGURATION_CHAR_SIZE 50

#define ENGINE_CONFIGURATION_DEFAULT_SHADOW_MAP_SIZE "1024"
#define ENGINE_CONFIGURATION_DEFAULT_SHADOW_EPSILON "0.0005f"
#define ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_ANGLE_FADE_OUT "0.7f"
#define ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_DISTANCE_FADE_OUT "0.5f"
#define ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_FOG_FADE_OUT "30.0f"
#define ENGINE_CONFIGURATION_DEFAULT_LIGHTNING_NS_MODIFIER "128"
#define ENGINE_CONFIGURATION_DEFAULT_PARTICLE_MAXIMUM_EMITTERS "40"
#define ENGINE_CONFIGURATION_DEFAULT_PARTICLE_MAXIMUM_PARTICLE_PER_EMITTER "200"
#define ENGINE_CONFIGURATION_DEFAULT_PARTICLE_SPRITES_PER_TEXTURE "16"

#include "../Engine/Render/CSceneStatic.h"
#include "../Engine/Render/CSceneDynamic.h"
#include "../Game/CPlayer.h"
#include "../Misc/CArraySorter.h"
#include "../Managers/COptionManager.h"

enum
{
	ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_USE_SHADOWS,
	ENGINE_RENDER_TECHNIC_DEFERRED_SHADING_NO_SHADOWS,
	ENGINE_RENDER_TECHNIC_FORWARD_USE_SHADOWS,
	ENGINE_RENDER_TECHNIC_FORWARD_NO_SHADOWS
};

/**
* Common vertex declaration used by the geometry (except the particles)
*/
const D3DVERTEXELEMENT9 g_VBDeclaration[] =
{
	{0,  0,  D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
	{0,  12, D3DDECLTYPE_FLOAT2,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
	{0,  20, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0,  32, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	1},
	{0,  44, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	2},
	{0,  56, D3DDECLTYPE_FLOAT4,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	0},
	D3DDECL_END()
};

/**
* Vertex declaration used to render the screen quad with the deferred rendering
*/
const D3DVERTEXELEMENT9 g_VBDeclarationScreen[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
	{0, 12, D3DDECLTYPE_FLOAT2,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
	{0, 20, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	D3DDECL_END()
};

/**
* Vertex declaration used to render the light depth
*/
const D3DVERTEXELEMENT9 g_VBDeclarationLight[] =
{
	{0,  0,  D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
	D3DDECL_END()
};

/**
* Common vertex declaration used by the particles
*/
const D3DVERTEXELEMENT9 g_VBDeclarationParticle[] =
{
	{0,  0,	D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
	{0, 12, D3DDECLTYPE_FLOAT2,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
	{1,  0,	D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	1},
	{1, 12, D3DDECLTYPE_FLOAT4,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,		0},
	{1, 28, D3DDECLTYPE_FLOAT4,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	2},
	D3DDECL_END()
};

/**
* Structure used to describe the engine configuration
*/
struct SEngineConfiguration
{
	// Particles
	int m_maximumEmitters;
	int m_maximumParticlesPerEmitter;
	int m_spritesPerTexture;

	// Shaders
	D3DXVECTOR4 m_shadows; // shadow map size, shadow epsilon
	D3DXVECTOR4 m_lightning; // angle fade out, distance fade out, fog fade out, ns modifier
};

/**
* Structure used for the screen quad
*/
struct SVertexQuad
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texcoord;
	D3DXVECTOR3 viewDistance;
};

/**
* Structure used for the particle 
*/
struct SVertexParticle
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texcoord;
};

class CShader;
class CDeviceManager;
class CLogManager;
class CModManager;
class CCameraManager;
class CLightManager;
class CParticleSystemManager;
class CCameraManager;

/**
* CEngine class.
* Manages the game rendering system.
* The goal is to:
* - Prepare the differents buffers and structures used for the rendering part.
* - Get the rendering information from the static and dynamic parts of the world.
*/
class CEngine
{
private:
	bool m_levelLoaded;
	int m_renderTechnic;
	int m_staticWorldVertexBufferSize;
	int m_staticLightVertexBufferSize;
	int m_dynamicLightVertexBufferSize;

	CPlayer * m_player;

	CSceneStatic m_bspWorld;
	CSceneDynamic m_dynamicWorld;
	std::ostringstream m_faceTextStream;

	// Buffers
	LPDIRECT3DVERTEXBUFFER9 m_staticWorldVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_staticLightVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_dynamicLightVertexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_screenQuadBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_vParticle;
	LPDIRECT3DVERTEXBUFFER9 m_vParticleInstance;
	LPDIRECT3DVERTEXDECLARATION9 m_vDeclaration;
	LPDIRECT3DVERTEXDECLARATION9 m_vScreenQuadDeclaration;
	LPDIRECT3DVERTEXDECLARATION9 m_vLightDeclaration;
	LPDIRECT3DVERTEXDECLARATION9 m_vParticleDeclaration;
	LPDIRECT3DINDEXBUFFER9 m_screenQuadInstance;

	// Rendering surfaces
	LPDIRECT3DSURFACE9 m_normalRenderTarget;
	LPDIRECT3DSURFACE9 m_albedoRenderTarget;
	LPDIRECT3DSURFACE9 m_positionRenderTarget;
	LPDIRECT3DSURFACE9 m_lightningRenderTarget;
	LPDIRECT3DSURFACE9 m_shadowRenderTarget;
	LPDIRECT3DSURFACE9 m_shadowDepthStencil;
	LPDIRECT3DTEXTURE9 m_normalTexture;
	LPDIRECT3DTEXTURE9 m_albedoTexture;
	LPDIRECT3DTEXTURE9 m_positionTexture;
	LPDIRECT3DTEXTURE9 m_lightningTexture;
	LPDIRECT3DTEXTURE9 m_shadowTexture;

	LPDIRECT3DSURFACE9 m_lightsShadowRenderTarget[LIGHT_SHADOW_MAXIMUM_SOURCES];
	LPDIRECT3DTEXTURE9 m_lightsShadowTexture[LIGHT_SHADOW_MAXIMUM_SOURCES];

	CShader * m_shaderFillShadowMap;
	CShader * m_shaderFillWorld;
	CShader * m_shaderFillLightning;
	CShader * m_shaderRenderWorld;
	CShader * m_shaderParticles;

	CDeviceManager * m_deviceM;
	CLogManager * m_logM;
	CModManager * m_modM;
	CCameraManager * m_cameraM;
	COptionManager * m_optionM;
	CLightManager * m_lightM;
	CParticleSystemManager * m_particleM;

public:
	static SEngineConfiguration * m_configuration;

public:
	CEngine();
	~CEngine();
	void Initialize();
	void LoadEngineConfiguration();
	void ResetRenderTargets();
	void LoadLevel(const std::string levelName, const bool resetPlayer);
	void UnloadLevel();
	void Update(const float time);
	void Render();
	void Destroy();
	void OnLostDevice();
	void OnResetDevice();

private:
	void InitializeBuffers();
	void InitializeShadowSamplerStates();
	void InitializeRenderSamplerStates();
	void FillShadowMap();
	//void RenderForward();
	//void RenderForwardNoShadows();
	void RenderDeferredShading();
	void RenderDeferredShadingNoShadows();
};

#endif
