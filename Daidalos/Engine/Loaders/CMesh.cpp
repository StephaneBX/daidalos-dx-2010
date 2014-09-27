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

#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"
#include "../../Engine/Loaders/CMesh.h"

/**
* Constructor
*/
CMesh::CMesh()
{
	this->m_geometryValid = true;
	this->m_geometrySize = 0;
	this->m_modifiedGeometry = NULL;
	this->m_modifiedLightGeometry = NULL;
	this->m_faces = NULL;
	this->m_defaultScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->m_optionM = COptionManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
}

/**
* Destructor
*/
CMesh::~CMesh()
{
	SAFE_DELETE_ARRAY(this->m_modifiedGeometry);
	SAFE_DELETE_ARRAY(this->m_modifiedLightGeometry);
	SAFE_DELETE_ARRAY(this->m_faces);
}

/**
* Animate the mesh
* @param frame The current frame
*/
void CMesh::Animate(const float frame){}

/**
* Animate the mesh with the default value
* @param frame The current frame
* @param shadingType The type of shading
*/
void CMesh::AnimateSourceCopy(const float frame, const int shadingType){}

/**
* Copy the source mesh coordinates into the faces.
* It's done in order to reset any scales/rotations/translations modifications done before.
*/
void CMesh::GeometrySourceCopy(const int shadingType){}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMesh::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals){}

/**
* Prepare the modified buffer by filling or calculating the information such as position, normal, binormal and tangent calculation.
*/
void CMesh::GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType){}

/**
* Override the default materials and use a custom one.
* @param filename The filename of a material file (.mtl)
*/
void CMesh::UseCustomMaterial(std::string filename){}

/**
* Check if the geometry is valid
* @return The geometry state
*/
const bool CMesh::IsGeometryValid() const
{
	return this->m_geometryValid;
}

/**
* Get the geometry size for each group
* @return The geometry size
*/
const int CMesh::GetGeometrySize() const
{
	return this->m_geometrySize;
}

/**
* Get the visible geometry size
* @return The visible geometry size
*/
const int CMesh::GetVisibleGeometrySize() const
{
	return this->m_visibleGeometrySize;
}

/**
* Get the animation fps
* @return The animation fps
*/
const float CMesh::GetAnimationFPS() const
{
	return this->m_animationFPS;
}

/**
* Get the faces
* @return the faces
*/
CSceneFace * CMesh::GetFaces() const
{
	return this->m_faces;
}

/**
* Get the modified geometry
* @return The modified geometry
*/
SVertexElement * CMesh::GetModifiedGeometry() const
{
	return this->m_modifiedGeometry;
}

/**
* Get the modified geometry for the lights
* @return The modified geometry
*/
SVertexLightPass * CMesh::GetModifiedLightGeometry() const
{
	return this->m_modifiedLightGeometry;
}

/**
* Load a Geometry file
* @param fileName The file name
*/
void CMesh::Load(const std::string fileName){}
