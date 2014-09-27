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

#include "../Misc/Global.h"
#include "../Managers/CModManager.h"

/**
* Constructor
*/
CModManager::CModManager(){}

/**
* Destructor
*/
CModManager::~CModManager(){}

/**
* Initialize the manager
*/
void CModManager::Initialize()
{
	this->m_langM = CLangManager::GetInstance();
	this->m_materialM = CMaterialManager::GetInstance();
	this->m_meshM = CMeshManager::GetInstance();
	this->m_shaderM = CShaderManager::GetInstance();
	this->m_textureM = CTextureManager::GetInstance();
	this->m_soundM = CSoundManager::GetInstance();
	this->m_particleM = CParticleSystemManager::GetInstance();

	ReloadModDatabase();
	SetDefaultMod();
}

/**
* Reload the database
*/
void CModManager::ReloadModDatabase()
{
	this->m_mods.clear();
	this->m_activeMod = -1;
	std::ostringstream stringstream;
	stringstream << DIRECTORY_MODS << "*.*";

	HANDLE hfind;
	WIN32_FIND_DATA wfd;
	hfind = FindFirstFile(stringstream.str().c_str(), &wfd);
	do
	{
		// If it's a directory, we increase numberOfMod value
		if(wfd.cFileName[0] != '.' && wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			this->m_mods.push_back(CMod(wfd.cFileName));
	}
	while(FindNextFile(hfind, &wfd));
	FindClose(hfind);
}

/**
* Set active the default mod
*/
void CModManager::SetDefaultMod()
{
	bool found = false;
	for(unsigned int i = 0 ; i < this->m_mods.size() && !found ; i++)
	{
		if(this->m_mods.at(i).GetDirectory() == MODMANAGER_DEFAULT_MOD)
		{
			SetActiveMod(i);
			std::ostringstream stream;
			stream << DIRECTORY_MODS << this->m_mods[i].GetDirectory() << "/";
			this->m_activeModDirectory = stream.str();
			found = true;
		}
	}
}

/**
* Set active a mod
* @param mod The mod id
*/
void CModManager::SetActiveMod(const int mod)
{
	this->m_activeMod = mod;
	this->m_langM->LoadLangFileList(GetActiveModDirectory());
	this->m_langM->ReloadLangDatabase(GetActiveModDirectory());
}

/**
* Pause all the effects
*/
void CModManager::PauseSoundEffects()
{
	this->m_soundM->PauseEffects();
}

/**
* Resume all the sounds
*/
void CModManager::ResumeSoundEffects()
{
	this->m_soundM->ResumeEffects();
}

/**
* Resume all the sounds
*/
void CModManager::StopSoundEffects()
{
	this->m_soundM->StopEffects();
}

/**
* Update the listener position
* @param x The listener x coordinate
* @param y The listener y coordinate
* @param z The listener z coordinate
*/
void CModManager::UpdateListenerPosition(const float x, const float y, const float z)
{
	this->m_soundM->UpdateListenerPosition(x, y, z);
}

/**
* Update the listener direction
* @param value The direction array
*/
void CModManager::UpdateListenerDirection(const float value[6])
{
	this->m_soundM->UpdateListenerDirection(value);
}

/**
* Get the number of mods
* @return The number of mods
*/
const int CModManager::GetNumberOfMod() const
{
	return this->m_mods.size();
}

/**
* Get the player starting health
* @return The player starting health
*/
const int CModManager::GetPlayerStartingHealth() const
{
	return this->m_mods[this->m_activeMod].GetPlayerStartingHealth();
}

/**
* Get the player starting armor
* @return The player starting armor
*/
const int CModManager::GetPlayerStartingArmor() const
{
	return this->m_mods[this->m_activeMod].GetPlayerStartingArmor();
}

/**
* Get a language text
* @param v The text id
* @return The text
*/
const std::string CModManager::GetLangText(const unsigned int v)
{
	return this->m_langM->GetText(v);
}

/**
* Get the active mod directory
* @return The active mod directory
*/
const std::string CModManager::GetActiveModDirectory() const
{
	return this->m_activeModDirectory;
}

/**
* Get a mod level name
* @param id The level id
* @return The choosen mod level name
*/
const std::string CModManager::GetModLevel(const int id)
{
	std::ostringstream stream;
	stream << GetActiveModDirectory() << DIRECTORY_LEVELS << this->m_mods[this->m_activeMod].GetLevel(id);
	return stream.str();
}

/**
* Load a material file
* @param fileName The material filename
* @return A pointer to the material
*/
CMaterial * const CModManager::LoadMaterial(const std::string fileName)
{
	return this->m_materialM->LoadMaterial(fileName, GetActiveModDirectory());
}

/**
* Load a mesh file
* @param fileName The mesh filename
* @return A pointer to the mesh
*/
CMesh * const CModManager::LoadMesh(const std::string fileName)
{
	return this->m_meshM->LoadMesh(fileName, GetActiveModDirectory());
}

/**
* Load a shader file
* @param shaderType The shader type
* @return A pointer to the shader
*/
CShader * const CModManager::LoadShader(const int shaderType)
{
	return this->m_shaderM->LoadShader(shaderType, GetActiveModDirectory());
}

/**
* Load a texture file
* @param filename The texture filename
* @param textureType The texture type
* @return A pointer to the texture
*/
LPDIRECT3DTEXTURE9 const CModManager::LoadTexture(const std::string filename, const int textureType)
{
	return this->m_textureM->LoadTexture(filename, textureType, GetActiveModDirectory());
}

/**
* Load an effect
* @param fileName The file name
* @return The sound effect
*/
CEffectSoundEntity * const CModManager::LoadEffect(const std::string fileName)
{
	return this->m_soundM->LoadEffect(fileName, GetActiveModDirectory());	
}

/**
* Load a music
* @param fileName The file name
* @return the music
*/
CMusicSoundEntity * const CModManager::LoadMusic(const std::string fileName)
{
	return this->m_soundM->LoadMusic(fileName, GetActiveModDirectory());
}

/**
* Load an emitter
* @param configurationName The configuration name
* @param position The initial position
*/
CEmitterEntity * const CModManager::LoadEmitter(const std::string configurationName)
{
	return this->m_particleM->LoadEmitter(configurationName, GetActiveModDirectory());
}
