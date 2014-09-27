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
#include "../../Managers/CDeviceManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CLogManager.h"
#include "../../Engine/Loaders/CMesh.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CEntityRender.h"

/**
* Constructor
*/
CEntityRender::CEntityRender()
{
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
	{
		this->m_meshGeometrySize[i] = 0;
		this->m_textureDiffuse[i] = NULL;
		this->m_textureNormal[i] = NULL;
		this->m_mesh[i] = NULL;
		this->m_vertexBuffer[i] = NULL;
	}

	this->m_animationSpeed = 1.0f;
	this->m_frame = -1.0f;

	this->m_modM = CModManager::GetInstance();
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
}

/**
* Destructor
*/
CEntityRender::~CEntityRender()
{
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
		SAFE_RELEASE(this->m_vertexBuffer[i]);
}

/**
* Update the render
* @param time Elapsed time
*/
void CEntityRender::Update(const float time)
{
	this->m_frame += (this->m_animationSpeed * time * this->m_animationFPS) / 1000.00f;
}

/**
* Load a mesh in a slot
* @param meshName The mesh file name
* @param slot The slot to load the mesh into
*/
void CEntityRender::LoadMesh(const std::string meshName, const int slot)
{
	std::string tmpString;
	std::ostringstream stream;
	stream.str("");
	stream << DIRECTORY_DYNAMIC_MESH << meshName;
	tmpString = stream.str();

	this->m_mesh[slot] = this->m_modM->LoadMesh(tmpString);
	this->m_meshGeometrySize[slot] = 0;

	// Count the number of faces
	for(int i = 0 ; i < this->m_mesh[slot]->GetGeometrySize() ; i += VERTEX_PER_FACE)
		this->m_meshGeometrySize[slot] += VERTEX_PER_FACE;

	// Initial position
	this->m_mesh[slot]->Animate(-1.0f);

	this->m_animationFPS = this->m_mesh[slot]->GetAnimationFPS();
}

/**
* Load a diffuse texture
* @param fileName The texture filename
* @param slot The mesh slot
*/
void CEntityRender::LoadDiffuseTexture(const std::string fileName, const int slot)
{
	if(slot >= 0 && slot < MAX_MESH_PER_OBJECT)
		this->m_textureDiffuse[slot] = this->m_modM->LoadTexture(fileName, TEXTURE_TYPE_MODEL);
}

/**
* Load a normal texture
* @param fileName The texture filename
* @param slot The mesh slot
*/
void CEntityRender::LoadNormalTexture(const std::string fileName, const int slot)
{
	if(slot >= 0 && slot < MAX_MESH_PER_OBJECT)
		this->m_textureNormal[slot] = this->m_modM->LoadTexture(fileName, TEXTURE_TYPE_MODEL);
}

/**
* Load a material
* @param fileName The material filename
* @param slot The mesh slot
*/
void CEntityRender::LoadMaterial(const std::string fileName, const int slot)
{
	if(slot >= 0 && slot < MAX_MESH_PER_OBJECT && this->m_mesh[slot] != NULL)
		this->m_mesh[slot]->UseCustomMaterial(fileName);
}

/**
* Set the current frame
* @param frame The new frame
*/
void CEntityRender::SetFrame(const float frame)
{
	this->m_frame = frame;
}

/**
* Set the current animation speed
* @param speed The new animation speed
*/
void CEntityRender::SetAnimationSpeed(const float speed)
{
	this->m_animationSpeed = speed;
}

/**
* Set the current animation fps
* @param fps The new animation fps
*/
void CEntityRender::SetAnimationFPS(const float fps)
{
	this->m_animationFPS = fps;
}

/**
* Destroy any allocated geometry
*/
void CEntityRender::DestroyGeometry()
{
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
		SAFE_RELEASE(this->m_vertexBuffer[i]);
}

/**
* Initialize the geometry buffer
*/
void CEntityRender::InitializeGeometry()
{
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
		if(this->m_meshGeometrySize[i] > 0)
			_VERIFY(this->m_deviceM->GetDevice()->CreateVertexBuffer(this->m_meshGeometrySize[i] * sizeof(SVertexElement), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &this->m_vertexBuffer[i], NULL), "Vertex Buffer Creation");
}

/**
* Fill the vertex buffers with the visible geometry
*/
void CEntityRender::FinalizeRender()
{
	SVertexElement * pVertices;
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
	{
		if(this->m_mesh[i] != NULL && this->m_mesh[i]->IsGeometryValid())
		{
			this->m_meshVisibleGeometrySize[i] = this->m_mesh[i]->GetVisibleGeometrySize();
			if(this->m_meshVisibleGeometrySize[i] > 0)
			{
				this->m_vertexBuffer[i]->Lock(0,this->m_meshVisibleGeometrySize[i] * sizeof(SVertexElement), (void**)&pVertices, D3DLOCK_DISCARD);
					memcpy(pVertices, this->m_mesh[i]->GetModifiedGeometry(), this->m_meshVisibleGeometrySize[i] * sizeof(SVertexElement));
				this->m_vertexBuffer[i]->Unlock();
			}
		}
	}
}

/**
* Reset the render counters
*/
void CEntityRender::ResetRender()
{
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
		this->m_meshVisibleGeometrySize[i] = 0;
}

/**
* Fill the lights geometry
* @param geometry A pointer to the geometry
* @param start The beginning of the geometry buffer to fill
*/
void CEntityRender::FillLightsGeometry(SVertexLightPass * const geometry, int * const start)
{
	int index = *start;
	for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
	{
		if(this->m_mesh[i] != NULL && this->m_mesh[i]->IsGeometryValid())
		{
			for(int j = 0 ; j < this->m_mesh[i]->GetGeometrySize() ; j++)
			{
				geometry[index].m_position = this->m_mesh[i]->GetModifiedLightGeometry()[j].m_position;
				index++;
			}
		}
	}
	*start = index;
}

/**
* Link the camera to the render
* @param camera The camera
*/
void CEntityRender::LinkCamera(CSceneCamera * const camera)
{
	this->m_camera = camera;
}

/**
* Get the geometry size of a mesh
* @param mesh A mesh id
* @return The geometry size of the choosen mesh
*/
const int CEntityRender::GetGeometrySize(const int mesh) const
{
	return this->m_meshGeometrySize[mesh];
}

/**
* Get the size of the visible of a mesh
* @param mesh A mesh id
* @return The size of the visible geometry of the choosen mesh
*/
const int CEntityRender::GetVisibleGeometrySize(const int mesh) const
{
	return this->m_meshVisibleGeometrySize[mesh];
}

/**
* Get the current frame
* @return The current frame or -1 for a non-animated entity
*/
const float CEntityRender::GetFrame() const
{
	return this->m_frame;
}

/**
* Get the current animation FPS
* @return The current animation FPS
*/
const float CEntityRender::GetAnimationFPS() const
{
	return this->m_animationFPS;
}

/**
* Get the diffuse texture corresponding to the choosen mesh
* @param mesh The mesh number
* @return The associated texture
*/
const LPDIRECT3DTEXTURE9 CEntityRender::GetTextureDiffuse(const int mesh) const
{
	return this->m_textureDiffuse[mesh];
}

/**
* Get the normal texture corresponding to the choosen mesh
* @param mesh The mesh number
* @return The associated texture
*/
const LPDIRECT3DTEXTURE9 CEntityRender::GetTextureNormal(const int mesh) const
{
	return this->m_textureNormal[mesh];
}

/**
* Get the vertex buffer corresponding to the choosen mesh
* @param mesh The mesh number
* @return The associated vertex buffer
*/
const LPDIRECT3DVERTEXBUFFER9 CEntityRender::GetVertexBuffer(const int mesh) const
{
	return this->m_vertexBuffer[mesh];
}

/**
* Get the linked camera
* @return The linked camera
*/
CSceneCamera * CEntityRender::GetCamera() const
{
	return this->m_camera;
}

/**
* Get the mesh
* @return The mesh
*/
CMesh ** CEntityRender::GetMesh()
{
	return this->m_mesh;
}
