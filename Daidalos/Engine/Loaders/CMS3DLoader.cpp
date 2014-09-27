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
#include "../../Misc/Global.h"
#include "../../Managers/CLogManager.h"
#include "../../Engine/Loaders/CMS3DLoader.h"

/**
* Constructor
*/
CMS3DLoader::CMS3DLoader()
{
	this->m_numVertices = 0;
	this->m_numFaces = 0;
	this->m_numMeshes = 0;
	this->m_numMaterials = 0;

	this->m_vertices = NULL;
	this->m_faces = NULL;
	this->m_meshes = NULL;
	this->m_materials = NULL;
	this->m_joints = NULL;

	this->m_logM = CLogManager::GetInstance();
}

/**
* Destructor
*/
CMS3DLoader::~CMS3DLoader()
{
	SAFE_DELETE_ARRAY(this->m_vertices);
	SAFE_DELETE_ARRAY(this->m_faces);
	SAFE_DELETE_ARRAY(this->m_materials);

	if(this->m_meshes != NULL)
	{
		for(int i = 0 ; i < this->m_numMeshes ; i++)
			SAFE_DELETE_ARRAY(this->m_meshes[i].m_facesIndices);
		SAFE_DELETE_ARRAY(this->m_meshes);
	}

	if(this->m_joints != NULL)
	{
		for(int i = 0 ; i < this->m_numJoints ; i++)
		{
			SAFE_DELETE_ARRAY(this->m_joints[i].m_rotationKeyFrames);
			SAFE_DELETE_ARRAY(this->m_joints[i].m_translationKeyFrames);
		}
		SAFE_DELETE_ARRAY(this->m_joints);
	}

	SAFE_DELETE_ARRAY(this->m_positionBuffer);
	SAFE_DELETE_ARRAY(this->m_normalBuffer);
}

/**
* Load the obj file.
* @param filename The obj file name.
*/
void CMS3DLoader::Load(std::string filename)
{
	FILE * file;
	SMS3DHeader header;

	fopen_s(&file, filename.c_str(), "rb");
	if(file)
	{
		// Get the file size
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		fread(&header.m_id, sizeof(char), 10, file);
		fread(&header.m_version, sizeof(int), 1, file);

		if(strncmp(header.m_id, "MS3D000000", 10) == 0)
		{
			if(header.m_version == 4)
			{
				this->m_geometrySize = 0;

				// Load the vertices
				fread(&this->m_numVertices, sizeof(unsigned short), 1, file);
				this->m_vertices = new SMS3DVertex[this->m_numVertices];
				this->m_positionBuffer = new D3DXVECTOR3[this->m_numVertices];
				for(int i = 0 ; i < this->m_numVertices ; i++)
				{
					fread(&this->m_vertices[i].m_flag, sizeof(unsigned char), 1, file);
					fread(&this->m_vertices[i].m_position, sizeof(float), 3, file);
					fread(&this->m_vertices[i].m_boneId, sizeof(char), 1, file);
					fread(&this->m_vertices[i].m_unused, sizeof(unsigned char), 1, file);
				}

				// Load the faces
				fread(&this->m_numFaces, sizeof(unsigned short), 1, file);
				this->m_faces = new SMS3DFace[this->m_numFaces];
				this->m_normalBuffer = new D3DXVECTOR3[this->m_numFaces * 3];
				for(int i = 0 ; i < this->m_numFaces ; i++)
				{
					fread(&this->m_faces[i].m_flags, sizeof(unsigned short), 1, file);
					fread(&this->m_faces[i].m_vertexIndices, sizeof(unsigned short), 3, file);
					fread(&this->m_faces[i].m_normals, sizeof(float), 3 * 3, file);
					fread(&this->m_faces[i].m_texCoords[0], sizeof(float), 3, file);
					fread(&this->m_faces[i].m_texCoords[1], sizeof(float), 3, file);
					fread(&this->m_faces[i].m_smoothing, sizeof(unsigned char), 1, file);
					fread(&this->m_faces[i].m_groupId, sizeof(unsigned char), 1, file);
				}

				// Load the meshes
				fread(&this->m_numMeshes, sizeof(unsigned short), 1, file);
				this->m_meshes = new SMS3DMesh[this->m_numMeshes];
				for(int i = 0 ; i < this->m_numMeshes ; i++)
				{
					fread(&this->m_meshes[i].m_flags, sizeof(unsigned char), 1, file);
					fread(&this->m_meshes[i].m_name, sizeof(char), 32, file);
					fread(&this->m_meshes[i].m_numFaces, sizeof(unsigned short), 1, file);

					this->m_meshes[i].m_facesIndices = new unsigned short[this->m_meshes[i].m_numFaces];
					fread(this->m_meshes[i].m_facesIndices, sizeof(unsigned short), this->m_meshes[i].m_numFaces, file);
					fread(&this->m_meshes[i].m_materialId, sizeof(char), 1, file);

					this->m_geometrySize += this->m_meshes[i].m_numFaces * 3;
				}

				// Load the materials
				fread(&this->m_numMaterials, sizeof(unsigned short), 1, file);
				if(this->m_numMaterials > 0)
				{
					this->m_materials = new SM3DMaterial[this->m_numMaterials];
					for(int i = 0 ; i < this->m_numMaterials ; i++)
					{
						fread(&this->m_materials[i].m_name, sizeof(char), 32, file);
						fread(&this->m_materials[i].m_ambient, sizeof(float), 4, file);
						fread(&this->m_materials[i].m_diffuse, sizeof(float), 4, file);
						fread(&this->m_materials[i].m_specular, sizeof(float), 4, file);
						fread(&this->m_materials[i].m_emission, sizeof(float), 4, file);
						fread(&this->m_materials[i].m_shininess, sizeof(float), 1, file);
						fread(&this->m_materials[i].m_transparency, sizeof(float), 1, file);
						fread(&this->m_materials[i].m_mode, sizeof(unsigned char), 1, file);
						fread(&this->m_materials[i].m_texture, sizeof(char), 128, file);
						fread(&this->m_materials[i].m_alpha, sizeof(char), 128, file);
					}
				}

				// Animation information
				fread(&this->m_animationFPS, sizeof(float), 1, file);
				if(this->m_animationFPS < 1.0f)
					this->m_animationFPS = 1.0f;
				fread(&this->m_currentTime, sizeof(float), 1, file);
				fread(&this->m_totalFrames, sizeof(int), 1, file);

				// Load the joints
				fread(&this->m_numJoints, sizeof(unsigned short), 1, file);
				if(this->m_numJoints > 0)
				{
					this->m_joints = new SM3DJoint[this->m_numJoints];
					for(int i = 0 ; i < this->m_numJoints ; i++)
					{
						fread(&this->m_joints[i].m_flags, sizeof(unsigned char), 1, file);
						fread(&this->m_joints[i].m_name, sizeof(char), 32, file);
						fread(&this->m_joints[i].m_parentName, sizeof(char), 32, file);
						fread(&this->m_joints[i].m_rotation, sizeof(float), 3, file);
						fread(&this->m_joints[i].m_position, sizeof(float), 3, file);
						fread(&this->m_joints[i].m_numRotationFrames, sizeof(unsigned short), 1, file);
						fread(&this->m_joints[i].m_numTranslationFrames, sizeof(unsigned short), 1, file);

						// Rotation key frames
						this->m_joints[i].m_rotationKeyFrames = new SM3DKeyFrame[this->m_joints[i].m_numRotationFrames];
						for(int j = 0 ; j < this->m_joints[i].m_numRotationFrames ; j++)
						{
							fread(&this->m_joints[i].m_rotationKeyFrames[j].m_time, sizeof(float), 1, file);
							fread(&this->m_joints[i].m_rotationKeyFrames[j].m_param, sizeof(float), 3, file);

							// Time in second, we need to multiply by the animation fps to get the frame
							this->m_joints[i].m_rotationKeyFrames[j].m_time *= this->m_animationFPS;
						}

						// Translation key frames
						this->m_joints[i].m_translationKeyFrames = new SM3DKeyFrame[this->m_joints[i].m_numTranslationFrames];
						for(int j = 0 ; j < this->m_joints[i].m_numTranslationFrames ; j++)
						{
							fread(&this->m_joints[i].m_translationKeyFrames[j].m_time, sizeof(float), 1, file);
							fread(&this->m_joints[i].m_translationKeyFrames[j].m_param, sizeof(float), 3, file);

							// Time in second, we need to multiply by the animation fps to get the frame
							this->m_joints[i].m_translationKeyFrames[j].m_time *= this->m_animationFPS;
						}
					}
					InitializeAnimationData();
				}

				// Read comments
				char * tmpBuffer;
				long filePos = ftell(file);
				if(filePos < fileSize)
				{
					int subVersion = 0;
					fread(&subVersion, sizeof(int), 1, file);
					if(subVersion == 1)
					{
						int numComments = 0;
						unsigned int commentSize = 0;

						// Meshes
						fread(&numComments, sizeof(int), 1, file);
						for(int i = 0 ; i < numComments ; i++)
						{
							int index;
							fread(&index, sizeof(int), 1, file);
							fread(&commentSize, sizeof(unsigned int), 1, file);

							if(commentSize > 0)
							{
								tmpBuffer = new char[commentSize];
								fread(tmpBuffer, sizeof(char), commentSize, file);
								if (index >= 0 && index < (int)this->m_numMeshes)
									this->m_meshes[index].m_comment.append(tmpBuffer);
								delete[] tmpBuffer;
							}
						}

						// Materials
						fread(&numComments, sizeof(int), 1, file);
						for(int i = 0 ; i < numComments ; i++)
						{
							int index;
							fread(&index, sizeof(int), 1, file);
							fread(&commentSize, sizeof(unsigned int), 1, file);

							if(commentSize > 0)
							{
								tmpBuffer = new char[commentSize];
								fread(tmpBuffer, sizeof(char), commentSize, file);
								if (index >= 0 && index < (int)this->m_numMeshes)
									this->m_materials[index].m_comment.append(tmpBuffer);
								delete[] tmpBuffer;
							}
						}

						// Joints
						fread(&numComments, sizeof(int), 1, file);
						for(int i = 0 ; i < numComments ; i++)
						{
							int index;
							fread(&index, sizeof(int), 1, file);
							fread(&commentSize, sizeof(unsigned int), 1, file);

							if(commentSize > 0)
							{
								tmpBuffer = new char[commentSize];
								fread(tmpBuffer, sizeof(char), commentSize, file);
								if (index >= 0 && index < (int)this->m_numMeshes)
									this->m_joints[index].m_comment.append(tmpBuffer);
								delete[] tmpBuffer;
							}
						}

						// Model
						fread(&numComments, sizeof(int), 1, file);
						for(int i = 0 ; i < numComments ; i++)
						{
							fread(&commentSize, sizeof(unsigned int), 1, file);

							if(commentSize > 0)
							{
								tmpBuffer = new char[commentSize];
								fread(tmpBuffer, sizeof(char), commentSize, file);
								this->m_comment.append(tmpBuffer);
								delete[] tmpBuffer;
							}
						}
					}
				}
				
				// Read extra vertex data
				

				this->m_fileState = MS3D_LOADER_STATE_OK;
			}
			else
				this->m_fileState = MS3D_LOADER_STATE_INVALID_FORMAT;
		}
		else
			this->m_fileState = MS3D_LOADER_STATE_INVALID_FORMAT;

		fclose(file);
	}
	else
		this->m_fileState = MS3D_LOADER_STATE_FILE_NOT_FOUND;
}

/**
* Animate a ms3d file
* @param frame The frame to use
*/
void CMS3DLoader::Animate(float frame)
{
	if(frame < 0.0f)
	{
		for(int i = 0 ; i < this->m_numJoints ; i++)
		{
			this->m_joints[i].m_matrixLocal = this->m_joints[i].m_matrixLocalSkeleton;
			this->m_joints[i].m_matrixGlobal = this->m_joints[i].m_matrixGlobalSkeleton;
		}

		for(int i = 0 ; i < this->m_numVertices ; i++)
			this->m_positionBuffer[i] = this->m_vertices[i].m_position;

		for(int i = 0 ; i < this->m_numFaces ; i++)
		{
			for(int j = 0 ; j < 3 ; j++)
			{
				this->m_normalBuffer[i * 3 + j] = this->m_faces[i].m_normals[j];
			}
		}
	}
	else
	{
		SM3DJoint * currentJoint;
		float interpolationFactor;
		bool keyFrameFound;
		D3DXMATRIX animationMatrix, rotationMatrix, translationMatrix;
		D3DXQUATERNION rotationQuaternion1, rotationQuaternion2, rotationQuaternionResult;
		D3DXVECTOR3 interpolatedVector;

		// Update the joints animation matrixes
		for(int i = 0 ; i < this->m_numJoints ; i++)
		{
			keyFrameFound = false;
			currentJoint = &this->m_joints[i];
			currentJoint->m_currentTranslationFrame = currentJoint->m_numTranslationFrames;
			currentJoint->m_currentRotationFrame = currentJoint->m_numRotationFrames;

			D3DXMatrixIdentity(&animationMatrix);
			D3DXMatrixIdentity(&rotationMatrix);
			D3DXMatrixIdentity(&translationMatrix);
			D3DXMatrixIdentity(&currentJoint->m_matrixLocal);
			D3DXQuaternionIdentity(&rotationQuaternion1);
			D3DXQuaternionIdentity(&rotationQuaternion2);
			D3DXQuaternionIdentity(&rotationQuaternionResult);

			// We need to find the current keyframe

			// Translation
			for(int j = 0 ; j < currentJoint->m_numTranslationFrames - 1 && !keyFrameFound ; j++)
			{
				if(frame >= currentJoint->m_translationKeyFrames[j].m_time && frame < currentJoint->m_translationKeyFrames[j + 1].m_time)
				{
					currentJoint->m_currentTranslationFrame = j;
					keyFrameFound = true;
				}	
			}

			keyFrameFound = false;

			// Rotation
			for(int j = 0 ; j < currentJoint->m_numRotationFrames - 1 && !keyFrameFound ; j++)
			{
				if(frame >= currentJoint->m_rotationKeyFrames[j].m_time && frame < currentJoint->m_rotationKeyFrames[j + 1].m_time)
				{
					currentJoint->m_currentRotationFrame = j;
					keyFrameFound = true;
				}
			}

			// Translation calculation
			if(currentJoint->m_currentTranslationFrame == currentJoint->m_numTranslationFrames && frame < currentJoint->m_translationKeyFrames[0].m_time)
			{
				// The first keyframe, no interpolation needed
				interpolatedVector = currentJoint->m_translationKeyFrames[0].m_param;
			}
			else
			{
				if(currentJoint->m_currentTranslationFrame == currentJoint->m_numTranslationFrames && frame >= currentJoint->m_translationKeyFrames[currentJoint->m_numTranslationFrames - 1].m_time)
				{
					// The last keyframe, no interpolation needed
					interpolatedVector = currentJoint->m_translationKeyFrames[currentJoint->m_numTranslationFrames - 1].m_param;
				}
				else
				{
					// Interpolation needed as we are in the middle of the keyframes
					interpolationFactor = (frame - currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame].m_time) / (currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame + 1].m_time - currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame].m_time);
					interpolatedVector = currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame].m_param + (currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame + 1].m_param - currentJoint->m_translationKeyFrames[currentJoint->m_currentTranslationFrame].m_param) * interpolationFactor;
				}
			}
			D3DXMatrixTranslation(&translationMatrix, interpolatedVector.x, interpolatedVector.y, interpolatedVector.z);

			// Rotation calculation
			// We use quaternion to interpolate rotations, as it will produce better results
			if(currentJoint->m_currentRotationFrame == currentJoint->m_numRotationFrames && frame < currentJoint->m_rotationKeyFrames[0].m_time)
			{
				// The first keyframe, no interpolation needed
				interpolatedVector = currentJoint->m_rotationKeyFrames[0].m_param;
				D3DXMatrixRotationYawPitchRoll(&rotationMatrix, interpolatedVector.y, interpolatedVector.x, interpolatedVector.z);
			}
			else
			{
				if(currentJoint->m_currentRotationFrame == currentJoint->m_numRotationFrames && frame >= currentJoint->m_rotationKeyFrames[currentJoint->m_numRotationFrames - 1].m_time)
				{
					// The last keyframe, no interpolation needed
					interpolatedVector = currentJoint->m_rotationKeyFrames[currentJoint->m_numRotationFrames - 1].m_param;
					D3DXMatrixRotationYawPitchRoll(&rotationMatrix, interpolatedVector.y, interpolatedVector.x, interpolatedVector.z);
				}
				else
				{
					// Interpolation needed as we are in the middle of the keyframes
					interpolationFactor = (frame - currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame].m_time) / (currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame + 1].m_time - currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame].m_time);

					// Create a quaternion with the current keyframe values
					D3DXQuaternionRotationYawPitchRoll(&rotationQuaternion1, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame].m_param.y, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame].m_param.x, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame].m_param.z);
					
					// Create a quaternion with the next keyframe values
					D3DXQuaternionRotationYawPitchRoll(&rotationQuaternion2, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame + 1].m_param.y, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame + 1].m_param.x, currentJoint->m_rotationKeyFrames[currentJoint->m_currentRotationFrame + 1].m_param.z);

					// Interpolate between the two quaternions
					D3DXQuaternionSlerp(&rotationQuaternionResult, &rotationQuaternion1, &rotationQuaternion2, interpolationFactor);

					// Tranform the quaternion in a matrix
					D3DXMatrixRotationQuaternion(&rotationMatrix, &rotationQuaternionResult);
				}
			}
			
			// Get the final local matrix
			D3DXMatrixMultiply(&animationMatrix, &rotationMatrix, &translationMatrix);
			D3DXMatrixMultiply(&currentJoint->m_matrixLocal, &animationMatrix, &currentJoint->m_matrixLocalSkeleton);
		}
		BuildGlobalMatrix();
		FinalVerticesCalculation();
		FinalNormalsCalculation();
	}
}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMS3DLoader::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals)
{
	D3DXVec3TransformCoordArray(this->m_positionBuffer, sizeof(D3DXVECTOR3), this->m_positionBuffer, sizeof(D3DXVECTOR3), matrix, this->m_numVertices);

	if(updateNormals)
	{
		D3DXVECTOR3 tmp;
		D3DXMATRIX rotation;
		D3DXQUATERNION rotationQuaternion;

		D3DXMatrixDecompose(&tmp, &rotationQuaternion, &tmp, matrix);
		D3DXMatrixRotationQuaternion(&rotation, &rotationQuaternion);

		D3DXVec3TransformCoordArray(this->m_normalBuffer, sizeof(D3DXVECTOR3), this->m_normalBuffer, sizeof(D3DXVECTOR3), &rotation, this->m_numFaces * 3);
	}
}

/**
* Get the file state
* @return The file state
*/
const int CMS3DLoader::GetFileState() const
{
	return this->m_fileState;
}

/**
* Get the geometry size
* @return The geometry size
*/
const int CMS3DLoader::GetGeometrySize() const
{
	return this->m_geometrySize;
}

/**
* Get the number of vertices
* @return The number of vertices
*/
const int CMS3DLoader::GetNumVertices() const
{
	return this->m_numVertices;
}

/**
* Get the number of faces
* @return The number of faces
*/
const int CMS3DLoader::GetNumFaces() const
{
	return this->m_numFaces;
}

/**
* Get the number of meshes
* @return The number of meshes
*/
const int CMS3DLoader::GetNumMeshes() const
{
	return this->m_numMeshes;
}

/**
* Get the animation fps
* @return The animation fps
*/
const float CMS3DLoader::GetAnimationFPS() const
{
	return this->m_animationFPS;
}

/**
* Get the triangles
* @return The triangles
*/
const SMS3DFace * CMS3DLoader::GetFaces() const
{
	return this->m_faces;
}

/**
* Get the meshes
* @return The meshes
*/
const SMS3DMesh * CMS3DLoader::GetMeshes() const
{
	return this->m_meshes;
}

/**
* Get the materials
* @return The materials
*/
const SM3DMaterial * CMS3DLoader::GetMaterials() const
{
	return this->m_materials;
}

/**
* Get the position buffer
* @return the position buffer
*/
const D3DXVECTOR3 * CMS3DLoader::GetPositionBuffer() const
{
	return this->m_positionBuffer;
}

/**
* Get the normal buffer
* @return the normal buffer
*/
const D3DXVECTOR3 * CMS3DLoader::GetNormalBuffer() const
{
	return this->m_normalBuffer;
}

/**
* Initialize the animation data
* - Find the parent of each bone
* - Initialize the skeleton matrixes
*/
void CMS3DLoader::InitializeAnimationData()
{
	InitializeParents();
	InitializeSkeletonsMatrixes();
}

/**
* Initialize the parents
* Find the parent for each bone
*/
void CMS3DLoader::InitializeParents()
{
	bool parentFound;
	for(int i = 0 ; i < this->m_numJoints ; i++)
	{
		parentFound = false;
		for(int j = 0 ; j < this->m_numJoints && !parentFound ; j++)
		{
			if(strncmp(this->m_joints[i].m_parentName, this->m_joints[j].m_name, 32) == 0)
			{
				this->m_joints[i].m_parent = j;
				parentFound = true;
			}
		}

		// The joint is not the root
		if(!parentFound)
			this->m_joints[i].m_parent = MS3D_ROOT_JOINT;
	}
}

/**
* Initlaize the skeleton matrixes
*/
void CMS3DLoader::InitializeSkeletonsMatrixes()
{
	// Local Skeleton Matrix initialization
	D3DXMATRIX rotationMatrix, translationMatrix;
	SM3DJoint * currentJoint;
	SM3DJoint * parentJoint;

	// Initialize the local skeleton matrixes that contains translation and rotation information
	for(int i = 0 ; i < this->m_numJoints ; i++)
	{
		currentJoint = &this->m_joints[i];

		D3DXMatrixIdentity(&rotationMatrix);
		D3DXMatrixIdentity(&translationMatrix);

		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, currentJoint->m_rotation.y, currentJoint->m_rotation.x, currentJoint->m_rotation.z);
		D3DXMatrixTranslation(&translationMatrix, currentJoint->m_position.x, currentJoint->m_position.y, currentJoint->m_position.z);
		D3DXMatrixMultiply(&currentJoint->m_matrixLocalSkeleton, &rotationMatrix, &translationMatrix);
	}

	// Global Skeleton Matrix initialization
	// Multiply the local skeleton matrix with the global skeleton matrix of the parent
	for(int i = 0 ; i < this->m_numJoints ; i++)
	{
		currentJoint = &this->m_joints[i];

		if(currentJoint->m_parent == MS3D_ROOT_JOINT)
		{
			currentJoint->m_matrixGlobalSkeleton = currentJoint->m_matrixLocalSkeleton;
		}
		else
		{
			parentJoint = &this->m_joints[currentJoint->m_parent];
			D3DXMatrixMultiply(&currentJoint->m_matrixGlobalSkeleton, &currentJoint->m_matrixLocalSkeleton, &parentJoint->m_matrixGlobalSkeleton);
		}
	}
}

/**
* Build the global matrix
*/
void CMS3DLoader::BuildGlobalMatrix()
{
	SM3DJoint * currentJoint;
	SM3DJoint * parentJoint;

	// Build the hierarchy for the global matrix
	for(int i = 0 ; i < this->m_numJoints ; i++)
	{
		currentJoint = &this->m_joints[i];

		if(currentJoint->m_parent == MS3D_ROOT_JOINT)
		{
			currentJoint->m_matrixGlobal = currentJoint->m_matrixLocal;
		}
		else
		{
			parentJoint = &this->m_joints[currentJoint->m_parent];
			D3DXMatrixMultiply(&currentJoint->m_matrixGlobal, &currentJoint->m_matrixLocal, &parentJoint->m_matrixGlobal);
		}
	}
}

/**
* Perform calculation to have te final vertices
*/
void CMS3DLoader::FinalVerticesCalculation()
{
	int currentBone;
	D3DXVECTOR3 mulResult;
	D3DXMATRIX inverse, rotation;
	

	// Apply modifications to vertices
	for(int i = 0 ; i < this->m_numVertices ; i++)
	{
		// Only vertices that are attached to a bone will be updated
		currentBone = (int)this->m_vertices[i].m_boneId;
		if(currentBone != MS3D_VERTEX_NO_BONES)
		{
			D3DXMatrixInverse(&inverse, NULL, &this->m_joints[currentBone].m_matrixGlobalSkeleton);
			D3DXVec3TransformCoord(&mulResult, &this->m_vertices[i].m_position, &inverse);
			D3DXVec3TransformCoord(&this->m_positionBuffer[i], &mulResult, &this->m_joints[currentBone].m_matrixGlobal);
		}
		else
			this->m_positionBuffer[i] = this->m_vertices[i].m_position;
	}
}

/**
* Perform calculation to have te final normals
*/
void CMS3DLoader::FinalNormalsCalculation()
{
	int currentBone;
	D3DXVECTOR3 mulResult, tmp;
	D3DXMATRIX inverse, rotation;
	D3DXQUATERNION rotationQuaternion;
	SMS3DFace * currentFace;

	// Copy local static normals
	for(int i = 0 ; i < this->m_numFaces ; i++)
	{
		currentFace = &this->m_faces[i];
		for(int j = 0 ; j < 3 ; j++)
		{
			this->m_normalBuffer[i * 3 + j] = currentFace->m_normals[j];
		}
	}

	// Apply modifications to normals
	for(int i = 0 ; i < this->m_numFaces ; i++)
	{
		currentFace = &this->m_faces[i];
		for(int j = 0 ; j < 3 ; j++)
		{
			currentBone = this->m_vertices[currentFace->m_vertexIndices[j]].m_boneId;

			if(currentBone != MS3D_VERTEX_NO_BONES)
			{
				D3DXMatrixDecompose(&tmp, &rotationQuaternion, &tmp, &this->m_joints[currentBone].m_matrixGlobalSkeleton);
				D3DXMatrixRotationQuaternion(&rotation, &rotationQuaternion);
				D3DXMatrixInverse(&inverse, NULL, &rotation);
				D3DXVec3TransformCoord(&mulResult, &this->m_normalBuffer[i * 3 + j], &inverse);

				D3DXMatrixDecompose(&tmp, &rotationQuaternion, &tmp, &this->m_joints[currentBone].m_matrixGlobal);
				D3DXMatrixRotationQuaternion(&rotation, &rotationQuaternion);
				D3DXVec3TransformCoord(&this->m_normalBuffer[i * 3 + j], &mulResult, &rotation);
			}
		}
	}
}
