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

#include "../../Misc/Global.h"
#include "../../Managers/CLogManager.h"
#include "../../Engine/CTokenizer.h"
#include "../../Engine/Loaders/CMaterial.h"

/**
* Constructor
*/
CMaterial::CMaterial()
{
	this->m_name = "";
	this->m_kDiffuse = D3DXVECTOR3(0,0,0);
	this->m_kAmbient = D3DXVECTOR3(0,0,0);
	this->m_kSpecular = D3DXVECTOR3(0,0,0);
	this->m_kEmission = D3DXVECTOR3(0,0,0);
	this->m_ns = 0;
}

/**
* Constructor
* @param filename The mtl filename
*/
CMaterial::CMaterial(std::string filename)
{
	this->m_name = "";
	this->m_kDiffuse = D3DXVECTOR3(0,0,0);
	this->m_kAmbient = D3DXVECTOR3(0,0,0);
	this->m_kSpecular = D3DXVECTOR3(0,0,0);
	this->m_kEmission = D3DXVECTOR3(0,0,0);
	this->m_ns = 0;

	Load(filename);
}

/**
* Load a material file
* @param filename The mtl filename
*/
void CMaterial::Load(std::string filename)
{
	D3DXVECTOR3 tmpVector;
	std::vector<std::string> filterList;
	filterList.push_back("newmtl ");
	filterList.push_back("Kd ");
	filterList.push_back("Ka ");
	filterList.push_back("Ke ");
	filterList.push_back("Ns ");

	CTokenizer tokenizer(filename, &filterList);
	tokenizer.Tokenize(" ");

	if(tokenizer.m_tokens.size() > 0)
	{
		for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ;)
		{
			if(tokenizer.m_tokens.at(i) == "newmtl")
			{
				this->m_name = tokenizer.m_tokens.at(i + 1);
				i += 2;
			}
			else
			{
				if(tokenizer.m_tokens.at(i) == "Kd")
				{
					// Diffuse modification
					this->m_kDiffuse.x = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
					this->m_kDiffuse.y = (float)atof(tokenizer.m_tokens.at(i + 2).c_str());
					this->m_kDiffuse.z = (float)atof(tokenizer.m_tokens.at(i + 3).c_str());
					i += 4;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "Ka")
					{
						// Ambient modification
						this->m_kAmbient.x = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
						this->m_kAmbient.y = (float)atof(tokenizer.m_tokens.at(i + 2).c_str());
						this->m_kAmbient.z = (float)atof(tokenizer.m_tokens.at(i + 3).c_str());
						i += 4;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "Ks")
						{
							// Specular modification
							this->m_kSpecular.x = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
							this->m_kSpecular.y = (float)atof(tokenizer.m_tokens.at(i + 2).c_str());
							this->m_kSpecular.z = (float)atof(tokenizer.m_tokens.at(i + 3).c_str());
							i += 4;
						}
						else
						{
							if(tokenizer.m_tokens.at(i) == "Ke")
							{
								// Emission modification
								this->m_kEmission.x = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
								this->m_kEmission.y = (float)atof(tokenizer.m_tokens.at(i + 2).c_str());
								this->m_kEmission.z = (float)atof(tokenizer.m_tokens.at(i + 3).c_str());
								i += 4;
							}
							else
							{
								if(tokenizer.m_tokens.at(i) == "Ns")
								{
									// Specular factor modification
									this->m_ns = atoi(tokenizer.m_tokens.at(i + 1).c_str());
									i += 2;
								}
								else
									i++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		CLogManager::GetInstance()->GetStream() << "Error: Unable to load the material: " << filename << ".\n";
		CLogManager::GetInstance()->Write();
	}
}

/**
* Get the specular coefficient
* @return the specular coefficient
*/
const int CMaterial::GetNs() const
{
	return this->m_ns;
}

/**
* Get the material name
* @return The material name
*/
const std::string CMaterial::GetName() const
{
	return this->m_name;
}

/**
* Get the diffuse factor
* @return the diffuse factor
*/
const D3DXVECTOR3 CMaterial::GetDiffuse() const
{
	return this->m_kDiffuse;
}

/**
* Get the ambient factor
* @return the ambient factor
*/
const D3DXVECTOR3 CMaterial::GetAmbient() const
{
	return this->m_kAmbient;
}

/**
* Get the specular factor
* @return the specular factor
*/
const D3DXVECTOR3 CMaterial::GetSpecular() const
{
	return this->m_kSpecular;
}

/**
* Get the emission factor
* @return the emission factor
*/
const D3DXVECTOR3 CMaterial::GetEmission() const
{
	return this->m_kEmission;
}
