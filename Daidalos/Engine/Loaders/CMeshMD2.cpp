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
#include "../../Managers/CModManager.h"
#include "../../Engine/Loaders/CMaterial.h"
#include "../../Engine/Loaders/CMeshMD2.h"

/**
* Constructor
* @param fileName The mesh to load
*/
CMeshMD2::CMeshMD2(const std::string fileName)
{
	Load(fileName);
	if(this->m_geometryValid)
		this->m_loader.Animate(-1);
}

/**
* Destructor
*/
CMeshMD2::~CMeshMD2(){}

/**
* Animate the mesh
* @param frame The current frame
*/
void CMeshMD2::Animate(const float frame)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(frame);
}

/**
* Animate the mesh and copy the result in the geometry
* @param frame The current frame
* @param shadingType The type of shading
*/
void CMeshMD2::AnimateSourceCopy(const float frame, const int shadingType)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(frame);
}

/**
* Reset the mesh position.
* It's done in order to reset any scales/rotations/translations modifications done before.
*/
void CMeshMD2::GeometrySourceCopy(const int shadingType)
{
	if(this->m_geometryValid)
		this->m_loader.Animate(-1);
}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMeshMD2::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals)
{
	if(this->m_geometryValid)
		this->m_loader.GeometryTransform(matrix, updateNormals);
}

/**
* Prepare the modified buffer by filling or calculating the information such as position, normal, binormal and tangent calculation.
*/
void CMeshMD2::GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType)
{
	this->m_visibleGeometrySize = 0;
	int lightIndex = 0;

	if(this->m_geometryValid)
	{
		const D3DXVECTOR3 * positionBuffer = this->m_loader.GetPositionBuffer();
		const D3DXVECTOR3 * normalBuffer = this->m_loader.GetNormalBuffer();
		const SMD2Triangle * triangleBuffer = this->m_loader.GetTriangles();

		D3DXVECTOR3 tmpPosition[3];
		D3DXVECTOR3 tmpNormal[3];

		for(int i = 0 ; i < this->m_loader.GetNumFaces() ; i++)
		{
			tmpPosition[0] = positionBuffer[triangleBuffer[i].m_vertices[0]];
			tmpPosition[1] = positionBuffer[triangleBuffer[i].m_vertices[1]];
			tmpPosition[2] = positionBuffer[triangleBuffer[i].m_vertices[2]];
			this->m_faces[i].SetPosition(tmpPosition);

			if(shadingType == SHADING_TYPE_SMOOTH)
			{
				tmpNormal[0] = normalBuffer[triangleBuffer[i].m_vertices[0]];
				tmpNormal[1] = normalBuffer[triangleBuffer[i].m_vertices[1]];
				tmpNormal[2] = normalBuffer[triangleBuffer[i].m_vertices[2]];
				this->m_faces[i].SetNormal(tmpNormal);
			}

			this->m_faces[i].BuildFace();
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
* Override the default materials and use a custom one.
* @param filename The filename of a material file (.mtl)
*/
void CMeshMD2::UseCustomMaterial(std::string filename)
{
	if(this->m_faces != NULL && this->m_geometryValid)
	{
		CMaterial * material = this->m_modM->LoadMaterial(filename);

		D3DXVECTOR3 tmpColor = material->GetEmission();
		float tmpNs = material->GetNs() / MESH_DEFAULT_NS_MODIFIER;

		for(int i = 0 ; i < this->m_loader.GetNumFaces() ; i++)
			this->m_faces[i].SetColor(D3DXCOLOR(tmpColor.x, tmpColor.y, tmpColor.z, tmpNs));
	}
}

/**
* Load a Md2 3D file
* @param fileName The file name
*/
void CMeshMD2::Load(const std::string fileName)
{
	D3DXVECTOR3 tmpColor = D3DXVECTOR3(0,0,0);
	D3DXVECTOR2 tmpTexCoord[3];
	float tmpNs = MESH_DEFAULT_NS / MESH_DEFAULT_NS_MODIFIER;

	CSceneFace * currentFace = NULL;

	this->m_loader.Load(fileName);
	if(this->m_loader.GetFileState() == MD2_LOADER_STATE_OK)
	{
		this->m_geometrySize = this->m_loader.GetGeometrySize();
		this->m_animationFPS = MD2_ANIMATION_FPS;
		this->m_modifiedGeometry = new SVertexElement[this->m_geometrySize];
		this->m_modifiedLightGeometry = new SVertexLightPass[this->m_geometrySize];
		this->m_faces = new CSceneFace[this->m_loader.GetNumFaces()];

		const D3DXVECTOR2 * textureBuffer = this->m_loader.GetTextureBuffer();
		const SMD2Triangle * triangleBuffer = this->m_loader.GetTriangles();

		for(int i = 0 ; i < this->m_loader.GetNumFaces() ; i++)
		{
			currentFace = &this->m_faces[i];

			tmpTexCoord[0] = textureBuffer[i * 3];
			tmpTexCoord[1] = textureBuffer[i * 3 + 1];
			tmpTexCoord[2] = textureBuffer[i * 3 + 2];


			// Texture coordinates and Materials
			currentFace->SetTexCoord(tmpTexCoord);
			currentFace->SetColor(D3DXCOLOR(tmpColor.x, tmpColor.y, tmpColor.z, tmpNs));
		}

		this->m_geometryValid = true;
	}
	else
		this->m_geometryValid = false;
}
