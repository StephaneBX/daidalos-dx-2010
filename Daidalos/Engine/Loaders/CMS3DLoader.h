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

#ifndef __CMS3DLOADER_H
#define __CMS3DLOADER_H

#define MS3D_ROOT_JOINT -1
#define MS3D_VERTEX_NO_BONES -1

#include <d3d9.h>
#include <d3dx9.h>

enum ms3dLoaderState
{
	MS3D_LOADER_STATE_OK,
	MS3D_LOADER_STATE_INVALID_FORMAT,
	MS3D_LOADER_STATE_FILE_NOT_FOUND
};

struct SMS3DHeader
{
	char m_id[10];
	int m_version;
};

struct SMS3DVertex
{
	unsigned char m_flag;
	D3DXVECTOR3 m_position;
	char m_boneId;
	unsigned char m_unused;

	char m_boneIds[3];
	unsigned char m_weights[3];
	unsigned int m_extra;
};

struct SMS3DFace
{
	unsigned short m_flags;
	unsigned short m_vertexIndices[3];
	D3DXVECTOR3 m_normals[3];
	float m_texCoords[2][3];
	unsigned char m_smoothing;
	unsigned char m_groupId;
};

struct SMS3DMesh
{
	unsigned char m_flags;
	char m_name[32];
	unsigned short m_numFaces;
	unsigned short * m_facesIndices;
	char m_materialId;
	std::string m_comment;
};

struct SM3DMaterial
{
	char m_name[32];
	float m_ambient[4];
	float m_diffuse[4];
	float m_specular[4];
	float m_emission[4];
	float m_shininess;
	float m_transparency;
	unsigned char m_mode;
	char m_texture[128];
	char m_alpha[128];
	std::string m_comment;
};

struct SM3DKeyFrame
{
	float m_time;
	D3DXVECTOR3 m_param;
};

struct SM3DJoint
{
	// Data from file
	unsigned char m_flags;
	char m_name[32];
	char m_parentName[32];
	D3DXVECTOR3 m_rotation;
	D3DXVECTOR3 m_position;
	unsigned short m_numRotationFrames;
	unsigned short m_numTranslationFrames;
	SM3DKeyFrame * m_rotationKeyFrames;
	SM3DKeyFrame * m_translationKeyFrames;

	// Extended data
	int m_parent;
	D3DXMATRIX m_matrixLocalSkeleton;
	D3DXMATRIX m_matrixGlobalSkeleton;
	D3DXMATRIX m_matrixLocal;
	D3DXMATRIX m_matrixGlobal;
	unsigned short m_currentRotationFrame;
	unsigned short m_currentTranslationFrame;
	std::string m_comment;
};

class CLogManager;

/**
* CObjLoader class.
* Manages the reading of Wavefront OBJ files.
*/
class CMS3DLoader
{
private:
	int m_fileState;
	int m_geometrySize;
	int m_totalFrames;
	float m_animationFPS;
	float m_currentTime;

	unsigned short m_numVertices;
	unsigned short m_numFaces;
	unsigned short m_numMeshes;
	unsigned short m_numMaterials;
	unsigned short m_numJoints;

	std::string m_comment;

	SMS3DVertex * m_vertices;
	SMS3DFace * m_faces;
	SMS3DMesh * m_meshes;
	SM3DMaterial * m_materials;
	SM3DJoint * m_joints;

	D3DXVECTOR3 * m_positionBuffer;
	D3DXVECTOR3 * m_normalBuffer;

	CLogManager * m_logM;

public:
	CMS3DLoader();
	~CMS3DLoader();
	void Load(const std::string filename);
	void Animate(float frame);
	void GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals);
	const int GetFileState() const;
	const int GetGeometrySize() const;
	const int GetNumVertices() const;
	const int GetNumFaces() const;
	const int GetNumMeshes() const;
	const float GetAnimationFPS() const;
	const SMS3DFace * GetFaces() const;
	const SMS3DMesh * GetMeshes() const;
	const SM3DMaterial * GetMaterials() const;
	const D3DXVECTOR3 * GetPositionBuffer() const;
	const D3DXVECTOR3 * GetNormalBuffer() const;

private:
	void InitializeAnimationData();
	void InitializeParents();
	void InitializeSkeletonsMatrixes();
	void BuildGlobalMatrix();
	void FinalVerticesCalculation();
	void FinalNormalsCalculation();
};

#endif