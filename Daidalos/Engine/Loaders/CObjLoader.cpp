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
#include "../../Engine/Loaders/CObjLoader.h"
#include "../../Misc/Global.h"
#include "../../Managers/CLogManager.h"
#include "../../Managers/CModManager.h"
#include "../../Engine/Loaders/CMaterial.h"
#include "../../Engine/CTokenizer.h"

/**
* Constructor
*/
CObjLoader::CObjLoader()
{
	this->m_fileState = OBJ_LOADER_STATE_OK;
	this->m_vertex = NULL;
	this->m_normal = NULL;
	this->m_texture = NULL;
	this->m_bufferSize = NULL;
	this->m_material = NULL;
	this->m_hasNormals = false;
	this->m_logM = CLogManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
}

/**
* Destructor
*/
CObjLoader::~CObjLoader()
{
	FreeData();
}

/**
* Load the obj file.
* @param filename The obj file name.
*/
void CObjLoader::Load(const std::string filename)
{
	D3DXVECTOR3 tmpVec3;
	D3DXVECTOR2 tmpVec2;
	std::ostringstream stream;
	std::vector<D3DXVECTOR3> tmpVertex;
	std::vector<D3DXVECTOR3> tmpNormal;
	std::vector<D3DXVECTOR2> tmpTexture;
	std::vector<int> vertexOrder;
	std::vector<int> normalOrder;
	std::vector<int> textureOrder;
	std::vector<std::string> materialOrder;
	std::string tmpLine;
	std::string tmpString;
	int groupCounter = -1;

	FreeData();

	std::vector<std::string> filterList;
	filterList.push_back("vt ");
	filterList.push_back("vn ");
	filterList.push_back("v ");
	filterList.push_back("f ");
	filterList.push_back("o ");
	filterList.push_back("g ");
	filterList.push_back("mtllib ");
	filterList.push_back("usemtl ");

	CTokenizer token(filename, &filterList);
	token.Tokenize(" ");
	token.Tokenize("/",3);

	this->m_numberGroup = token.Count("g");

	// Create the buffers
	this->m_vertex = new D3DXVECTOR3*[this->m_numberGroup];
	this->m_normal = new D3DXVECTOR3*[this->m_numberGroup];
	this->m_texture = new D3DXVECTOR2*[this->m_numberGroup];
	this->m_bufferSize = new int[this->m_numberGroup];
	this->m_material = new CMaterial*[this->m_numberGroup];

	// Buffers initialization
	for(int i = 0 ; i < this->m_numberGroup ; i++)
	{
		this->m_vertex[i] = NULL;
		this->m_normal[i] = NULL;
		this->m_texture[i] = NULL;
		this->m_material[i] = NULL;
		this->m_bufferSize[i] = 0;
	}

	// Read the Tokens
	for(unsigned int i = 0 ; i < token.m_tokens.size() && this->m_fileState == OBJ_LOADER_STATE_OK ; )
	{
		if(token.m_tokens.at(i) == "v")
		{
			// Vertex declaration
			tmpVec3.x = (float)atof(token.m_tokens.at(i + 1).c_str());
			tmpVec3.y = (float)atof(token.m_tokens.at(i + 2).c_str());
			tmpVec3.z = (float)atof(token.m_tokens.at(i + 3).c_str());
			tmpVertex.push_back(tmpVec3);
			i += OBJ_LOADER_ELEMENT_PER_LINE;
		}
		else
		{
			if(token.m_tokens.at(i) == "vt")
			{
				// Texture declaration
				tmpVec2.x = (float)atof(token.m_tokens.at(i + 1).c_str());
				tmpVec2.y = 1.0f - (float)atof(token.m_tokens.at(i + 2).c_str());
				tmpTexture.push_back(tmpVec2);
				i += OBJ_LOADER_ELEMENT_PER_LINE - 1;
			}
			else
			{
				if(token.m_tokens.at(i) == "vn")
				{
					// Normal declaration
					tmpVec3.x = (float)atof(token.m_tokens.at(i + 1).c_str());
					tmpVec3.y = (float)atof(token.m_tokens.at(i + 2).c_str());
					tmpVec3.z = (float)atof(token.m_tokens.at(i + 3).c_str());
					tmpNormal.push_back(tmpVec3);
					i += OBJ_LOADER_ELEMENT_PER_LINE;
				}
				else
				{
					if(token.m_tokens.at(i) == "f")
					{
						// Face declaration
						// Count the number of vertex for this face

						int vertexNumber = 0;
						int vertexCountIndex = 1;

						while((i + vertexCountIndex) < (int)token.m_tokens.size() && token.m_tokens.at(i + vertexCountIndex) != "f" && token.m_tokens.at(i + vertexCountIndex) != "g")
						{
							vertexNumber++;
							vertexCountIndex += 3;
						}

						// Accept only triangles and quads
						if(vertexNumber == 3 ||  vertexNumber == 4)
						{
							for(int k = 0 ; k < 3 && this->m_fileState == OBJ_LOADER_STATE_OK ; k++)
							{
								if(token.m_tokens.at(i + k * 3 + 1) != "")
									vertexOrder.push_back(atoi(token.m_tokens.at(i + k * 3 + 1).c_str()));

								if(token.m_tokens.at(i + k * 3 + 2) != "")
									textureOrder.push_back(atoi(token.m_tokens.at(i + k * 3 + 2).c_str()));

								if(token.m_tokens.at(i + k * 3 + 3) != "")
									normalOrder.push_back(atoi(token.m_tokens.at(i + k * 3 + 3).c_str()));
							}

							// Quad detected, need to add a second triangle
							if(vertexNumber == 4)
							{
								for(int k = 2 ; k < 5 && this->m_fileState == OBJ_LOADER_STATE_OK ; k++)
								{
									if(token.m_tokens.at(i + (k % 4) * 3 + 1) != "")
										vertexOrder.push_back(atoi(token.m_tokens.at(i + (k % 4) * 3 + 1).c_str()));

									if(token.m_tokens.at(i + (k % 4) * 3 + 2) != "")
										textureOrder.push_back(atoi(token.m_tokens.at(i + (k % 4) * 3 + 2).c_str()));

									if(token.m_tokens.at(i + (k % 4) * 3 + 3) != "")
										normalOrder.push_back(atoi(token.m_tokens.at(i + (k % 4) * 3 + 3).c_str()));
								}
								i += 13;
							}
							else
								i += 10;
						}
					}
					else
					{
						if(token.m_tokens.at(i) == "o")
						{
							// New object detected, we add the previus and clear all the temporary buffers
							if(groupCounter >= 0)
								FillBuffers(&tmpVertex, &tmpNormal, &tmpTexture, &vertexOrder, &normalOrder, &textureOrder, groupCounter);

							i += 2;
						}
						else
						{
							if(token.m_tokens.at(i) == "g")
							{
								// Clear the face buffer
								if(groupCounter >= 0)
									FillBuffers(&tmpVertex, &tmpNormal, &tmpTexture, &vertexOrder, &normalOrder, &textureOrder, groupCounter);
								vertexOrder.clear();
								normalOrder.clear();
								textureOrder.clear();
								groupCounter++;
								
								i += 2;
							}
							else
							{
								if(token.m_tokens.at(i) == "usemtl")
								{
									// Memorize the material to use
									materialOrder.push_back(token.m_tokens.at(i + 1));
									i += 2;
								}
								else
								{
									if(token.m_tokens.at(i) == "mtllib")
									{
										// Load the material file
										this->m_materialList.push_back(this->m_modM->LoadMaterial(token.m_tokens.at(i + 1)));

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
	}
	FillBuffers(&tmpVertex, &tmpNormal, &tmpTexture, &vertexOrder, &normalOrder, &textureOrder, groupCounter);

	// Material assignment
	for(unsigned int i = 0 ; i < materialOrder.size() ; i++)
	{
		this->m_material[i] = NULL;
		for(unsigned int j = 0 ; j < this->m_materialList.size() ; j++)
		{
			if(this->m_materialList.at(j)->GetName() == materialOrder.at(i))
				this->m_material[i] = this->m_materialList.at(j);
		}
	}

	// Count the geometry size
	this->m_geometrySize = 0;
	for(int i = 0 ; i < this->m_numberGroup ; i++)
		this->m_geometrySize += this->m_bufferSize[i];
}

/**
* Get the file state.
* It's used to know if the loading process was a success or if something wrong happened.
* @return The file state
*/
const int CObjLoader::GetFileState() const
{
	return this->m_fileState;
}

/**
* Get the number of group of the mesh.
* @return The number of group.
*/
const int CObjLoader::GetNumberGroup() const
{
	return this->m_numberGroup;
}

/**
* Get the geometry size.
* @return The geometry size.
*/
const int CObjLoader::GetGeometrySize() const
{
	return this->m_geometrySize;
}

/**
* Get a buffer containing the size of each group.
* @return The size of each group.
*/
const int * CObjLoader::GetBufferSize() const
{
	return this->m_bufferSize;
}

/**
* Check if the model has normals information
* @return True if the model has normals information, false otherwise
*/
const bool CObjLoader::hasNormals() const
{
	return this->m_hasNormals;
}

/**
* Get the material buffer.
* @return The material buffer.
*/
CMaterial ** CObjLoader::GetMaterial() const
{
	return this->m_material;
}

/**
* Get the vertices buffer.
* @return Teh vertices.
*/
D3DXVECTOR3 ** CObjLoader::GetVertex() const
{
	return this->m_vertex;
}

/**
* Get the normal buffer
* @return the normal buffer.
*/
D3DXVECTOR3 ** CObjLoader::GetNormal() const
{
	return this->m_normal;
}

/**
* Get the texture buffer
* @return the texture buffer.
*/
D3DXVECTOR2 ** CObjLoader::GetTexture() const
{
	return this->m_texture;
}

/**
* Unload used data
*/
void CObjLoader::FreeData()
{
	if(this->m_vertex != NULL)
	{
		for(int i = 0 ; i < this->m_numberGroup ; i++)
			SAFE_DELETE_ARRAY(this->m_vertex[i]);
		SAFE_DELETE_ARRAY(this->m_vertex);
	}

	if(this->m_normal != NULL)
	{
		for(int i = 0 ; i < this->m_numberGroup ; i++)
			SAFE_DELETE_ARRAY(this->m_normal[i]);
		SAFE_DELETE_ARRAY(this->m_normal);
	}

	if(this->m_texture != NULL)
	{
		for(int i = 0 ; i < this->m_numberGroup ; i++)
			SAFE_DELETE_ARRAY(this->m_texture[i]);
		SAFE_DELETE_ARRAY(this->m_texture);
	}
	
	SAFE_DELETE_ARRAY(this->m_bufferSize);

	this->m_materialList.clear();

	SAFE_DELETE_ARRAY(this->m_material);

	this->m_numberGroup = 0;
}

/**
* Fill the final buffers with the corresponding information.
* When the list of vertices, normals, textures coordinates and the faces order are read from the file, this fonction create the associated data.
* @param tmpVertex A vector containing the vertices
* @param tmpNormal A vector containing the normals
* @param tmpTexture A vector containing the textures
* @param vertexOrder A vector containing the order of the vertices to use
* @param normalOrder A vector containing the order of the normal to use
* @param textureOrder A vector containing the order of the texture to use
* @param groupCounter The group number for this data
*/
void CObjLoader::FillBuffers(std::vector<D3DXVECTOR3> * const tmpVertex, std::vector<D3DXVECTOR3> * const tmpNormal, std::vector<D3DXVECTOR2> * const tmpTexture, std::vector<int> * const vertexOrder, std::vector<int> * const normalOrder, std::vector<int> * const textureOrder, const int groupCounter)
{
	// The data is read, time to fill the right buffers
	if(vertexOrder->size() > 3 && this->m_fileState == OBJ_LOADER_STATE_OK)
	{
		this->m_bufferSize[groupCounter] = vertexOrder->size();
		this->m_vertex[groupCounter] = new D3DXVECTOR3[this->m_bufferSize[groupCounter]];

		// Fill the vertex buffer
		for(int i = 0 ; i < this->m_bufferSize[groupCounter] && this->m_fileState == OBJ_LOADER_STATE_OK ; i++)
		{
			if(vertexOrder->at(i) >= 1 && vertexOrder->at(i) <= (int)tmpVertex->size())
				this->m_vertex[groupCounter][i] = tmpVertex->at(vertexOrder->at(i) - 1);
			else
				this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
		}

		// If normal information exists, fill the normal buffer
		if(normalOrder->size() > 0 && this->m_fileState == OBJ_LOADER_STATE_OK)
		{
			if(normalOrder->size() == this->m_bufferSize[groupCounter])
			{
				// Verify the validity of the normal numbers
				this->m_hasNormals = true;

				for(int i = 0 ; i < this->m_bufferSize[groupCounter] && this->m_hasNormals ; i++)
				{
					if(normalOrder->at(i) < 1 || normalOrder->at(i) > (int)tmpNormal->size())
						this->m_hasNormals = false;
				}

				if(this->m_hasNormals)
				{
					this->m_normal[groupCounter] = new D3DXVECTOR3[this->m_bufferSize[groupCounter]];
					for(int i = 0 ; i < this->m_bufferSize[groupCounter] && this->m_fileState == OBJ_LOADER_STATE_OK ; i++)
					{
						if(normalOrder->at(i) >= 1 && normalOrder->at(i) <= (int)tmpNormal->size())
							this->m_normal[groupCounter][i] = tmpNormal->at(normalOrder->at(i) - 1);
						else
							if(normalOrder->at(i) != 0)
								this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
					}
				}
			}
			else
			{
				this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
				this->m_logM->GetStream() << "CObjLoader error: The number of normals is incorrect\n";
				this->m_logM->Write();
			}
		}

		// If texture information exists, fill the texture coordinate buffer
		if(textureOrder->size() > 0 && this->m_fileState == OBJ_LOADER_STATE_OK)
		{
			if(textureOrder->size() == this->m_bufferSize[groupCounter])
			{
				this->m_texture[groupCounter] = new D3DXVECTOR2[this->m_bufferSize[groupCounter]];
				for(int i = 0 ; i < this->m_bufferSize[groupCounter] && this->m_fileState == OBJ_LOADER_STATE_OK ; i++)
				{
					if(textureOrder->at(i) >= 1 && textureOrder->at(i) <= (int)tmpTexture->size())
						this->m_texture[groupCounter][i] = tmpTexture->at(textureOrder->at(i) - 1);
					else
						this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
				}
			}
			else
			{
				this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
				this->m_logM->GetStream() << "CObjLoader error: The number of texture coordinate is incorrect\n";
				this->m_logM->Write();
			}
		}
	}
	else
	{
		this->m_fileState = OBJ_LOADER_STATE_INVALID_FORMAT;
		this->m_numberGroup = 0;
	}
}
