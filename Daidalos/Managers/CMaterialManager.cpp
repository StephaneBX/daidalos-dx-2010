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

#include "../Managers/CMaterialManager.h"
#include "../Misc/Global.h"
#include "../Engine/Loaders/CMaterial.h"
#include "../Managers/CLogManager.h"

/**
* Constructor
*/
CMaterialManager::CMaterialManager(){}

/**
* Destructor
*/
CMaterialManager::~CMaterialManager()
{
	FreeDatabase();
}

/**
* Initialize the manager
*/
void CMaterialManager::Initialize()
{
	this->m_logM = CLogManager::GetInstance();
}

/**
* Free the meshs database
*/
void CMaterialManager::FreeDatabase()
{
	for(std::map<std::string, CMaterial *>::iterator it = this->m_materials.begin() ; it != this->m_materials.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_materials.clear();
}

/**
* Load a material and put it in the database
* @param fileName The material file name
* @param modDirectory The mod root directory
* @return A pointer to the material
*/
CMaterial * CMaterialManager::LoadMaterial(const std::string fileName, std::string modDirectory)
{
	// Check if the current mod has a custom material to use
	CMaterial * ret = NULL;
	std::string modFileName;
	std::string defaultFileName;

	std::ostringstream stream;
	stream << modDirectory << DIRECTORY_MATERIALS << fileName;
	modFileName = stream.str();

	stream.str("");
	stream << DIRECTORY_MATERIALS << fileName;
	defaultFileName = stream.str();

	// If the mod material exists in the database, we use it
	if(!MaterialExists(modFileName))
	{
		// If the default material exists in the database, we use it
		if(!MaterialExists(defaultFileName))
		{
			// If not, we need to know which one to use
			if(FileExists(modFileName))
			{
				// A custom material file exists, we use it
				this->m_materials[modFileName] = new CMaterial(modFileName);
				ret = this->m_materials[modFileName];
			}
			else
			{
				// If none exist, we use the default material file
				this->m_materials[defaultFileName] = new CMaterial(defaultFileName);
				ret = this->m_materials[defaultFileName];
			}
		}
		else
			ret = this->m_materials[defaultFileName];
	}
	else
		ret = this->m_materials[modFileName];

	return ret;
}

/**
* Check if a given material already exists in the database
* @param fileName The material file name
* @return True if the material is found in the database, False otherwise
*/
bool CMaterialManager::MaterialExists(const std::string fileName)
{
	bool ret = false;
	for(std::map<std::string, CMaterial *>::iterator it = this->m_materials.begin() ; it != this->m_materials.end() ; it++)
		if(it->first == fileName)
			ret = true;
	return ret;
}
