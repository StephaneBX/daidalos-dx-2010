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

#include "../../Engine/Render/CScenePlane.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Managers/COptionManager.h"
#include "../../Engine/Loaders/CMaterial.h"
#include "../../Engine/Loaders/CObjLoader.h"
#include "../../Engine/Loaders/CMeshOBJ.h"

/**
* Constructor
* @param fileName The mesh to load
*/
CMeshOBJ::CMeshOBJ(const std::string fileName)
{
	Load(fileName);
}

/**
* Destructor
*/
CMeshOBJ::~CMeshOBJ()
{
	SAFE_DELETE_ARRAY(this->m_sourceGeometry);
	SAFE_DELETE_ARRAY(this->m_sourceNormals);
	SAFE_DELETE_ARRAY(this->m_transformedGeometry);
	SAFE_DELETE_ARRAY(this->m_transformedNormals);
}

/**
* Animate the mesh with the default value
* @param frame The current frame
* @param shadingType The type of shading
*/
void CMeshOBJ::AnimateSourceCopy(const float frame, const int shadingType)
{
	if(this->m_geometryValid)
		GeometrySourceCopy(shadingType);
}

/**
* Reset the mesh position.
* It's done in order to reset any scales/rotations/translations modifications done before.
*/
void CMeshOBJ::GeometrySourceCopy(const int shadingType)
{
	if(this->m_geometryValid)
	{
		memcpy(this->m_transformedGeometry, this->m_sourceGeometry, sizeof(D3DXVECTOR3) * this->m_geometrySize);
		if(this->m_sourceNormals != NULL)
			memcpy(this->m_transformedNormals, this->m_sourceNormals, sizeof(D3DXVECTOR3) * this->m_geometrySize);
	}
}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMeshOBJ::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals)
{
	if(this->m_geometryValid)
	{
		D3DXVec3TransformCoordArray(this->m_transformedGeometry, sizeof(D3DXVECTOR3), this->m_transformedGeometry, sizeof(D3DXVECTOR3), matrix, this->m_geometrySize);

		if(updateNormals && this->m_sourceNormals != NULL)
		{
			D3DXVECTOR3 tmp;
			D3DXMATRIX rotation;
			D3DXQUATERNION rotationQuaternion;

			D3DXMatrixDecompose(&tmp, &rotationQuaternion, &tmp, matrix);
			D3DXMatrixRotationQuaternion(&rotation, &rotationQuaternion);

			D3DXVec3TransformCoordArray(this->m_transformedNormals, sizeof(D3DXVECTOR3), this->m_transformedNormals, sizeof(D3DXVECTOR3), &rotation, this->m_geometrySize);
		}
	}
}

/**
* Prepare the modified buffer by filling or calculating the information such as position, normal, binormal and tangent calculation.
*/
void CMeshOBJ::GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType)
{
	this->m_visibleGeometrySize = 0;
	int lightIndex = 0;
	D3DXVECTOR3 position[VERTEX_PER_FACE];
	D3DXVECTOR3 normal[VERTEX_PER_FACE];

	if(this->m_geometryValid)
	{
		for(int i = 0 ; i < this->m_geometrySize ; i += VERTEX_PER_FACE)
		{
			position[0] = this->m_transformedGeometry[i];
			position[1] = this->m_transformedGeometry[i + 1];
			position[2] = this->m_transformedGeometry[i + 2];

			this->m_faces[i / VERTEX_PER_FACE].SetPosition(position);

			if(shadingType == SHADING_TYPE_SMOOTH && this->m_sourceNormals != NULL)
			{
				normal[0] = this->m_transformedNormals[i];
				normal[1] = this->m_transformedNormals[i + 1];
				normal[2] = this->m_transformedNormals[i + 2];

				this->m_faces[i / VERTEX_PER_FACE].SetNormal(normal);
			}
			this->m_faces[i / VERTEX_PER_FACE].BuildFace();
		}

		// Geometry
		if(visible)
		{
			for(int i = 0 ; i < this->m_geometrySize / VERTEX_PER_FACE ; i++)
			{
				if((camera != NULL && !camera->FrustumCull(this->m_faces[i].m_boundingBox) && !camera->BackfaceCull(&this->m_faces[i])) || camera == NULL)
				{
					// Copy the faces information
					memcpy(&this->m_modifiedGeometry[this->m_visibleGeometrySize], this->m_faces[i].m_geometry, sizeof(SVertexElement) * VERTEX_PER_FACE);
					this->m_visibleGeometrySize += VERTEX_PER_FACE;
				}
			}
		}

		// Light Geometry
		if(this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
		{
			for(int i = 0 ; i < this->m_geometrySize / VERTEX_PER_FACE ; i++)
			{
				for(int j = 0 ; j < VERTEX_PER_FACE ; j++)
				{
					this->m_modifiedLightGeometry[lightIndex].m_position = this->m_faces[lightIndex / VERTEX_PER_FACE].m_geometry[j].m_position;
					lightIndex++;
				}
			}
		}
	}
}

/**
* Load an obj Wavefront file
* @param fileName The file name
*/
void CMeshOBJ::Load(const std::string fileName)
{
	D3DXVECTOR3 tmpColor;
	D3DXVECTOR2 tmpTexCoord[VERTEX_PER_FACE];
	float tmpNs;
	CObjLoader loader;
	loader.Load(fileName);
	if(loader.GetNumberGroup() > 0)
	{
		this->m_geometrySize = loader.GetGeometrySize();
		this->m_sourceGeometry = new D3DXVECTOR3[this->m_geometrySize];
		this->m_transformedGeometry = new D3DXVECTOR3[this->m_geometrySize];
		this->m_modifiedGeometry = new SVertexElement[this->m_geometrySize];
		this->m_modifiedLightGeometry = new SVertexLightPass[this->m_geometrySize];
		this->m_faces = new CSceneFace[this->m_geometrySize / VERTEX_PER_FACE];
		int index = 0;

		if(loader.hasNormals())
		{
			this->m_sourceNormals = new D3DXVECTOR3[this->m_geometrySize];
			this->m_transformedNormals = new D3DXVECTOR3[this->m_geometrySize];
		}
		else
		{
			this->m_sourceNormals = NULL;
			this->m_transformedNormals = NULL;
		}

		// Fill the source and modified buffers
		for(int i = 0 ; i < loader.GetNumberGroup() ; i++)
		{
			if(loader.GetBufferSize()[i] > 0)
			{
				for(int j = 0 ; j < loader.GetBufferSize()[i] ; j++)
				{
					this->m_sourceGeometry[index] = loader.GetVertex()[i][j];

					if(loader.hasNormals())
						this->m_sourceNormals[index] = loader.GetNormal()[i][j];
					
					if(loader.GetMaterial()[i] != NULL)
					{
						tmpColor = loader.GetMaterial()[i]->GetEmission();
						tmpNs = loader.GetMaterial()[i]->GetNs() / MESH_DEFAULT_NS_MODIFIER;
					}
					else
					{
						tmpColor = D3DXVECTOR3(0,0,0);
						tmpNs = MESH_DEFAULT_NS / MESH_DEFAULT_NS_MODIFIER;
					}

					if(tmpColor.x > 1)
						tmpColor.x = 1;
					if(tmpColor.y > 1)
						tmpColor.y = 1;
					if(tmpColor.z > 1)
						tmpColor.z = 1;

					if(tmpNs < 0)
						tmpNs = 0;
					if(tmpNs > 1)
						tmpNs = 1;

					this->m_modifiedGeometry[index].m_color = D3DXCOLOR(tmpColor.x, tmpColor.y, tmpColor.z, tmpNs);

					if(loader.GetTexture()[i] != NULL)
						this->m_modifiedGeometry[index].m_texcoord = loader.GetTexture()[i][j];
					else
						this->m_modifiedGeometry[index].m_texcoord = D3DXVECTOR2(0,0);

					index++;
				}
			}
		}

		// Fill the faces
		for(int i = 0 ; i < this->m_geometrySize ; i += VERTEX_PER_FACE)
		{
			tmpTexCoord[0] = this->m_modifiedGeometry[i].m_texcoord;
			tmpTexCoord[1] = this->m_modifiedGeometry[i + 1].m_texcoord;
			tmpTexCoord[2] = this->m_modifiedGeometry[i + 2].m_texcoord;
			this->m_faces[i / VERTEX_PER_FACE].SetTexCoord(tmpTexCoord);
			this->m_faces[i / VERTEX_PER_FACE].SetColor(this->m_modifiedGeometry[i].m_color);
		}
	}
	else
		this->m_geometryValid = false;
}
