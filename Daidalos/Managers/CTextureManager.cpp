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

#include <string>
#include <sstream>
#include "../Misc/Global.h"
#include "../Managers/CDeviceManager.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CLogManager.h"
#include "../Managers/CTextureManager.h"

/**
* Constructor
*/
CTextureManager::CTextureManager(){}

/**
* Destructor
*/
CTextureManager::~CTextureManager()
{
	FreeDatabase();
}

/**
* Initialize the manager
*/
void CTextureManager::Initialize()
{
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
	this->m_stream.str("");
	this->m_stream << DIRECTORY_TEXTURES << COptionManager::GetInstance()->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/";
	this->m_texturePath = this->m_stream.str();
}

/**
* Free the manager database
*/
void CTextureManager::FreeDatabase()
{
	for(std::map<std::string, LPDIRECT3DTEXTURE9>::iterator it = this->m_textureDatabase.begin() ; it != this->m_textureDatabase.end() ; it++)
		SAFE_RELEASE(it->second);
	this->m_textureDatabase.clear();
}

/**
* Check if the texture already exists in the database
* @param filename The texture filename
* @return The test result
*/
const bool CTextureManager::TextureExists(const std::string filename)
{
	bool ret = false;
	for(std::map<std::string, LPDIRECT3DTEXTURE9>::iterator it = this->m_textureDatabase.begin() ; it != this->m_textureDatabase.end() ; it++)
		if(it->first == filename)
			ret = true;
	return ret;
}

/**
* Load a texture and return a pointer of it. If the texture already exists in the database, simply return the pointer.
* @param filename The texture filename
* @param textureType The texture type
* @param modDirectory The mod directory
* @return A pointer of the texture
*/
LPDIRECT3DTEXTURE9 CTextureManager::LoadTexture(const std::string filename, const int textureType, std::string modDirectory)
{
	std::string absolutePath;
	std::ifstream file;
	this->m_stream.str("");

	switch(textureType)
	{
	case TEXTURE_TYPE_MODEL:

		this->m_stream << modDirectory << this->m_texturePath << filename;
		if(!FileExists(this->m_stream.str()))
		{
			this->m_stream.str("");
			this->m_stream << this->m_texturePath << filename;
		}
		absolutePath = this->m_stream.str();
		break;

	case TEXTURE_TYPE_SPRITE:

		this->m_stream << modDirectory << DIRECTORY_SPRITES << filename;
		if(!FileExists(this->m_stream.str()))
		{
			this->m_stream.str("");
			this->m_stream << DIRECTORY_SPRITES << filename;
		}
		absolutePath = this->m_stream.str();
		break;
	}

	if(!TextureExists(absolutePath))
	{
		this->m_textureDatabase[absolutePath] = NULL;
		_VERIFY(D3DXCreateTextureFromFileEx(this->m_deviceM->GetDevice(), absolutePath.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &this->m_textureDatabase[absolutePath]), "Texture creation");	

		if(this->m_textureDatabase[absolutePath] == NULL)
		{
			this->m_logM->GetStream() << "Error: Unable to load the texture: " << absolutePath << ".\n";
			this->m_logM->Write();
		}
	}
	return this->m_textureDatabase[absolutePath];
}
