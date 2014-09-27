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
#include "../../Engine/Loaders/CMeshMS3D.h"

/**
* Constructor
* @param fileName The mesh to load
*/
CMeshMS3D::CMeshMS3D(const std::string fileName)
{
	Load(fileName);
	if(this->m_geometryValid)
		this->m_loader.Animate(-1);
}

/**
* Destructor
*/
CMeshMS3D::~CMeshMS3D(){}

/**
* Animate the mesh
* @param frame The current frame
*/
void CMeshMS3D::Animate(const float frame)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(frame);
}

/**
* Animate the mesh and copy the result in the geometry
* @param frame The current frame
* @param shadingType The type of shading
*/
void CMeshMS3D::AnimateSourceCopy(const float frame, const int shadingType)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(frame);
}

/**
* Reset the mesh position.
* It's done in order to reset any scales/rotations/translations modifications done before.
*/
void CMeshMS3D::GeometrySourceCopy(const int shadingType)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(-1);
}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMeshMS3D::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals)
{
	if(this->m_geometryValid)
		this->m_loader.GeometryTransform(matrix, updateNormals);
}

/**
* Prepare the modified buffer by filling or calculating the information such as position, normal, binormal and tangent calculation.
*/
void CMeshMS3D::GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType)
{
	this->m_visibleGeometrySize = 0;
	int lightIndex = 0;
	int index = 0;
	D3DXVECTOR3 position[VERTEX_PER_FACE];
	D3DXVECTOR3 normal[VERTEX_PER_FACE];

	if(this->m_geometryValid)
	{
		const D3DXVECTOR3 * positionBuffer = this->m_loader.GetPositionBuffer();
		const D3DXVECTOR3 * normalBuffer = this->m_loader.GetNormalBuffer();

		for(int i = 0 ; i < this->m_loader.GetNumFaces() ; i++)
		{
			const SMS3DFace * currentFace = &this->m_loader.GetFaces()[i];

			// Vertices position
			position[0] = positionBuffer[currentFace->m_vertexIndices[0]];
			position[1] = positionBuffer[currentFace->m_vertexIndices[1]];
			position[2] = positionBuffer[currentFace->m_vertexIndices[2]];
			this->m_faces[index].SetPosition(position);

			// Normals
			if(shadingType == SHADING_TYPE_SMOOTH)
			{
				normal[0] = normalBuffer[i * 3];
				normal[1] = normalBuffer[i * 3 + 1];
				normal[2] = normalBuffer[i * 3 + 2];
				this->m_faces[index].SetNormal(normal);
			}

			this->m_faces[i].BuildFace();
			index++;
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
* Load a Milkshape 3D file
* @param fileName The file name
*/
void CMeshMS3D::Load(const std::string fileName)
{
	D3DXVECTOR3 tmpColor;
	D3DXVECTOR2 tmpTexCoord[VERTEX_PER_FACE];
	float tmpNs;
	this->m_loader.Load(fileName);
	if(this->m_loader.GetFileState() == MS3D_LOADER_STATE_OK)
	{
		this->m_geometrySize = this->m_loader.GetGeometrySize();
		this->m_animationFPS = this->m_loader.GetAnimationFPS();
		this->m_modifiedGeometry = new SVertexElement[this->m_geometrySize];
		this->m_modifiedLightGeometry = new SVertexLightPass[this->m_geometrySize];
		this->m_faces = new CSceneFace[this->m_geometrySize / VERTEX_PER_FACE];

		for(int i = 0 ; i < this->m_loader.GetNumMeshes() ; i++)
		{
			const SMS3DMesh * currentMesh = &this->m_loader.GetMeshes()[i];

			// Materials information
			if(this->m_loader.GetMeshes()[i].m_materialId > -1)
			{
				tmpColor = this->m_loader.GetMaterials()[this->m_loader.GetMeshes()[i].m_materialId].m_emission;
				tmpNs = this->m_loader.GetMaterials()[this->m_loader.GetMeshes()[i].m_materialId].m_shininess / MESH_DEFAULT_NS_MODIFIER;
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

			// Fill the source and modified buffers
			for(int j = 0 ; j < this->m_loader.GetMeshes()[i].m_numFaces ; j++)
			{
				const SMS3DFace * currentFace = &this->m_loader.GetFaces()[currentMesh->m_facesIndices[j]];
				CSceneFace * currentFace2 = &this->m_faces[currentMesh->m_facesIndices[j]];

				// Texture coordinates and Materials
				for(int k = 0 ; k < VERTEX_PER_FACE ; k++)
				{
					tmpTexCoord[k].x = currentFace->m_texCoords[0][k];
					tmpTexCoord[k].y = currentFace->m_texCoords[1][k];
				}

				currentFace2->SetTexCoord(tmpTexCoord);
				currentFace2->SetColor(D3DXCOLOR(tmpColor.x, tmpColor.y, tmpColor.z, tmpNs));
			}
		}

		this->m_geometryValid = true;
	}
	else
		this->m_geometryValid = false;
}
