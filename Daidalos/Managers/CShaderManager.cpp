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

#include <map>
#include "../Misc/Global.h"
#include "../Shaders/CShader.h"
#include "../Shaders/CDSShaderFillShadowMap.h"
#include "../Shaders/CDSShaderFillWorld.h"
#include "../Shaders/CDSShaderFillWorldNoShadows.h"
#include "../Shaders/CDSShaderFillLightning.h"
#include "../Shaders/CDSShaderFillLightningNoShadows.h"
#include "../Shaders/CDSShaderRenderWorld.h"
#include "../Shaders/CForwardShaderParticles.h"
#include "../Managers/CShaderManager.h"

/**
* Constructor
*/
CShaderManager::CShaderManager(){}

/**
* Destructor
*/
CShaderManager::~CShaderManager()
{
	FreeDatabase();
}

/**
* Load, compile and return a shader. If the shader already exists in the shader database, simply return the shader.
* @param shaderType The shader type
* @param modDirectory The mod directory
* @return A pointer to the shader
*/
CShader * CShaderManager::LoadShader(const int shaderType, const std::string modDirectory)
{
	if(!ShaderExists(shaderType))
	{
		switch(shaderType)
		{
		case SHADER_TYPE_DS_FILL_SHADOW_MAP:
			this->m_shaderDatabase[shaderType] = new CDSShaderFillShadowMap(modDirectory);
			break;

		case SHADER_TYPE_DS_FILL_WORLD:
			this->m_shaderDatabase[shaderType] = new CDSShaderFillWorld(modDirectory);
			break;

		case SHADER_TYPE_DS_FILL_WORLD_NOSHADOWS:
			this->m_shaderDatabase[shaderType] = new CDSShaderFillWorldNoShadows(modDirectory);
			break;

		case SHADER_TYPE_DS_FILL_LIGHTNING:
			this->m_shaderDatabase[shaderType] = new CDSShaderFillLightning(modDirectory);
			break;

		case SHADER_TYPE_DS_FILL_LIGHTNING_NOSHADOWS:
			this->m_shaderDatabase[shaderType] = new CDSShaderFillLightningNoShadows(modDirectory);
			break;

		case SHADER_TYPE_DS_RENDER_WORLD:
			this->m_shaderDatabase[shaderType] = new CDSShaderRenderWorld(modDirectory);
			break;

		case SHADER_TYPE_FORWARD_PARTICLES:
			this->m_shaderDatabase[shaderType] = new CForwardShaderParticles(modDirectory);
			break;
		}
	}
	return this->m_shaderDatabase[shaderType];
}

/**
* Verify if a shader exists in the shader database
* @param shaderType The shader type
* @return The test result
*/
bool CShaderManager::ShaderExists(const int shaderType)
{
	bool ret = false;
	for(std::map<int, CShader *>::iterator it = this->m_shaderDatabase.begin() ; it != this->m_shaderDatabase.end() ; it++)
		if(it->first == shaderType)
			ret = true;
	return ret;
}

/**
* Free the manager database
*/
void CShaderManager::FreeDatabase()
{
	for(std::map<int, CShader *>::iterator it = this->m_shaderDatabase.begin() ; it != this->m_shaderDatabase.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_shaderDatabase.clear();
}