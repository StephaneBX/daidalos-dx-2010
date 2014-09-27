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

#ifndef __CMODMANAGER_H
#define __CMODMANAGER_H

#define MODMANAGER_MAX_CHAR_PER_LINE 200
#define MODMANAGER_DEFAULT_MOD "Default"

#include <vector>
#include <string>
#include "../Engine/CMod.h"
#include "../Managers/CLangManager.h"
#include "../Managers/CMaterialManager.h"
#include "../Managers/CMeshManager.h"
#include "../Managers/CShaderManager.h"
#include "../Managers/CTextureManager.h"
#include "../Managers/CSoundManager.h"
#include "../Managers/CParticleSystemManager.h"
#include "../Managers/CSingletonManager.h"

class CEngineConfiguration;

/**
* CModManager Class.
* Manages the mods.
* This manager is also used for calling any ressource (language file, material, mesh, shader, texture), we don't call the corresponding 
* managers directly because the Mod Manager redirects the ressource path to the local mod directory to override existing ressource.
*/
class CModManager : public CSingletonManager<CModManager>
{
	friend class CSingletonManager<CModManager>;

private:
	int m_activeMod;
	std::string m_activeModDirectory;
	std::vector<CMod> m_mods;
	CEngineConfiguration * m_activeEngineConfiguration;

	CLangManager * m_langM;
	CMaterialManager * m_materialM;
	CMeshManager * m_meshM;
	CShaderManager * m_shaderM;
	CTextureManager * m_textureM;
	CSoundManager * m_soundM;
	CParticleSystemManager * m_particleM;

public:
	void Initialize();
	void ReloadModDatabase();
	void SetDefaultMod();
	void SetActiveMod(const int mod);
	void PauseSoundEffects();
	void ResumeSoundEffects();
	void StopSoundEffects();
	void UpdateListenerPosition(const float x, const float y, const float z);
	void UpdateListenerDirection(const float value[6]);
	const int GetNumberOfMod() const;
	const int GetPlayerStartingHealth() const;
	const int GetPlayerStartingArmor() const;
	const std::string GetActiveModDirectory() const;
	const std::string GetLangText(const unsigned int v);
	const std::string GetModLevel(const int id);
	CMaterial * const LoadMaterial(const std::string fileName);
	CMesh * const LoadMesh(const std::string fileName);
	CShader * const LoadShader(const int shaderType);
	LPDIRECT3DTEXTURE9 const LoadTexture(const std::string filename, const int textureType);
	CEffectSoundEntity * const LoadEffect(const std::string fileName);
	CMusicSoundEntity * const LoadMusic(const std::string fileName);
	CEmitterEntity * const LoadEmitter(const std::string configurationName);
	CEngineConfiguration * const GetEngineConfiguration() const;

private:
	CModManager();
	~CModManager();
	void LoadEngineConfiguration();
};

#endif
