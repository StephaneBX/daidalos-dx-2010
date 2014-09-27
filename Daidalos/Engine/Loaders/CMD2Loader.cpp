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
#include <sstream>
#include "../../Misc/Global.h"
#include "../../Managers/CLogManager.h"
#include "../../Engine/Loaders/CMD2Loader.h"


/**
* Constructor
*/
CMD2Loader::CMD2Loader()
{
	ZeroMemory(&this->m_header, sizeof(SMD2Header));

	this->m_skins = NULL;
	this->m_textureCoordinates = NULL;
	this->m_triangles = NULL;
	this->m_frames = NULL;

	this->m_positionBuffer = NULL;
	this->m_normalBuffer = NULL;
	this->m_textureBuffer = NULL;

	D3DXVECTOR3 rotation = D3DXVECTOR3(0, -90, 0);
	D3DXMatrixRotationYawPitchRoll(&this->m_rotation, D3DXToRadian(rotation.y), D3DXToRadian(rotation.x), D3DXToRadian(rotation.z));
}

/**
* Destructor
*/
CMD2Loader::~CMD2Loader()
{
	SAFE_DELETE(this->m_skins);
	SAFE_DELETE(this->m_textureCoordinates);
	SAFE_DELETE(this->m_triangles);

	SAFE_DELETE(this->m_positionBuffer);
	SAFE_DELETE(this->m_normalBuffer);
	SAFE_DELETE(this->m_textureBuffer);

	if(this->m_frames != NULL)
	{
		for(int i = 0 ; i < this->m_header.m_numFrames ; i++)
			SAFE_DELETE_ARRAY(this->m_frames[i].m_vertices);
		SAFE_DELETE(this->m_frames);
	}
}

/**
* Load a md2 file
* @param filename The md2 filename
*/
void CMD2Loader::Load(const std::string filename)
{
	FILE * file;

	fopen_s(&file, filename.c_str(), "rb");
	if(file)
	{
		fread(&this->m_header, sizeof(SMD2Header), 1, file);
		if(this->m_header.m_id == MD2_ID && this->m_header.m_version == MD2_VERSION)
		{
			// Geometry allocation
			this->m_skins = new SMD2Skin[this->m_header.m_numSkins];
			this->m_textureCoordinates = new SMD2TextureCoordinates[this->m_header.m_numTextureCoordinates];
			this->m_triangles = new SMD2Triangle[this->m_header.m_numTriangle];
			this->m_frames = new SMD2Frame[this->m_header.m_numFrames];

			this->m_positionBuffer = new D3DXVECTOR3[this->m_header.m_numVertices];
			this->m_normalBuffer = new D3DXVECTOR3[this->m_header.m_numVertices];
			this->m_textureBuffer = new D3DXVECTOR2[this->m_header.m_numTriangle * 3];

			// Read skins information
			fseek(file, this->m_header.m_offsetSkins, SEEK_SET);
			fread(this->m_skins, sizeof(SMD2Skin), this->m_header.m_numSkins, file);

			// Read texture coordinates
			fseek(file, this->m_header.m_offsetTextureCoordinates, SEEK_SET);
			fread(this->m_textureCoordinates, sizeof(SMD2TextureCoordinates), this->m_header.m_numTextureCoordinates, file);

			// Read triangles information
			fseek(file, this->m_header.m_offsetTriangles, SEEK_SET);
			for(int i = 0 ; i < this->m_header.m_numTriangle ; i++)
			{
				fread(this->m_triangles[i].m_vertices, sizeof(short), 3, file);
				fread(this->m_triangles[i].m_textureCoordinatesIndice, sizeof(short), 3, file);
				this->m_triangles[i].m_faceNormal = D3DXVECTOR3(0,0,0);
			}

			fseek(file, this->m_header.m_offsetFrames, SEEK_SET);

			for(int i = 0 ; i < this->m_header.m_numFrames ; i++)
			{
				// Vertex allocation
				this->m_frames[i].m_vertices = new SMD2Vertex[this->m_header.m_numVertices];

				// Read frame information
				fread(&this->m_frames[i].m_scale, sizeof(D3DXVECTOR3), 1, file);
				fread(&this->m_frames[i].m_translation, sizeof(D3DXVECTOR3), 1, file);
				fread(&this->m_frames[i].m_name, sizeof(char), MD2_FRAME_NAME_SIZE, file);

				for(int j = 0 ; j < this->m_header.m_numVertices ; j++)
				{
					// Read vertex information
					fread(&this->m_frames[i].m_vertices[j].m_position, sizeof(unsigned char), 3, file);
					fread(&this->m_frames[i].m_vertices[j].m_normalIndice, sizeof(unsigned char),1, file);
				}
			}
			fclose(file);

			this->m_fileState = MD2_LOADER_STATE_OK;
			this->m_geometrySize = this->m_header.m_numTriangle * 3;

			InitializeGeometry();
		}
		else
			this->m_fileState = MD2_LOADER_STATE_INVALID_FORMAT;
	}
	else
		this->m_fileState = MD2_LOADER_STATE_FILE_NOT_FOUND;
}

/**
* Animate a md2 file
* @param frame The frame to use
*/
void CMD2Loader::Animate(const float frame)
{
	int frameA = (int)floor(frame);
	int frameB = (int)floor(frame + 1);

	// Linear
	float interpolation = frame - frameA;

	// Cosine
	float interpolation2 = (1 - cos(interpolation * PI)) / 2.0f;

	// Use the first frame if the choosen frame is less than 0
	if(frameA < 0)
	{
		frameA = 0;
		frameB = 0;
	}

	// Use the last frame if the choosen frame is bigger than this->m_header.m_numFrames - 1
	if(frameB > this->m_header.m_numFrames - 1)
	{
		frameA = this->m_header.m_numFrames - 1;
		frameB = this->m_header.m_numFrames - 1;
	}

	SMD2Frame * currentFrameA = &this->m_frames[frameA];
	SMD2Frame * currentFrameB = &this->m_frames[frameB];
	SMD2Vertex * currentVertexA;
	SMD2Vertex * currentVertexB;
	D3DXVECTOR3 currentNormalA, currentNormalB, currentPositionA, currentPositionB;

	for(int i = 0 ; i < this->m_header.m_numVertices ; i++)
	{
		currentVertexA = &currentFrameA->m_vertices[i];
		currentVertexB = &currentFrameB->m_vertices[i];

		currentNormalA.x = currentVertexA->m_averageNormal.x;
		currentNormalA.z = currentVertexA->m_averageNormal.y;
		currentNormalA.y = currentVertexA->m_averageNormal.z;

		currentNormalB.x = currentVertexB->m_averageNormal.x;
		currentNormalB.z = currentVertexB->m_averageNormal.y;
		currentNormalB.y = currentVertexB->m_averageNormal.z;

		this->m_normalBuffer[i] = DoCosineInterpolation(currentNormalA, currentNormalB, interpolation2);

		// Swap the y and z axis and inverse the x and z axis values, the md2 file uses a different space coordinates
		currentPositionA.x = currentVertexA->m_fullPosition.x;
		currentPositionA.z = currentVertexA->m_fullPosition.y;
		currentPositionA.y = currentVertexA->m_fullPosition.z;

		currentPositionB.x = currentVertexB->m_fullPosition.x;
		currentPositionB.z = currentVertexB->m_fullPosition.y;
		currentPositionB.y = currentVertexB->m_fullPosition.z;

		this->m_positionBuffer[i] = DoCosineInterpolation(currentPositionA, currentPositionB, interpolation2);
	}

	// Do a 90 degree rotation to match with the engine space coordinates
	D3DXVec3TransformCoordArray(this->m_positionBuffer, sizeof(D3DXVECTOR3), this->m_positionBuffer, sizeof(D3DXVECTOR3), &this->m_rotation, this->m_header.m_numVertices);
	D3DXVec3TransformCoordArray(this->m_normalBuffer, sizeof(D3DXVECTOR3), this->m_normalBuffer, sizeof(D3DXVECTOR3), &this->m_rotation, this->m_header.m_numVertices);
}

/**
* Perform a tranformation
* @param matrix The matrix to use
* @param updateNormals Information to know if the normals need to be transformed too
*/
void CMD2Loader::GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals)
{
	D3DXVec3TransformCoordArray(this->m_positionBuffer, sizeof(D3DXVECTOR3), this->m_positionBuffer, sizeof(D3DXVECTOR3), matrix, this->m_header.m_numVertices);

	if(updateNormals)
	{
		D3DXVECTOR3 tmp;
		D3DXMATRIX rotation;
		D3DXQUATERNION rotationQuaternion;

		D3DXMatrixDecompose(&tmp, &rotationQuaternion, &tmp, matrix);
		D3DXMatrixRotationQuaternion(&rotation, &rotationQuaternion);

		D3DXVec3TransformCoordArray(this->m_normalBuffer, sizeof(D3DXVECTOR3), this->m_normalBuffer, sizeof(D3DXVECTOR3), &rotation, this->m_header.m_numVertices);
	}
}

/**
* Get the file state
* @return The file state
*/
const int CMD2Loader::GetFileState() const
{
	return this->m_fileState;
}

/**
* Get the geometry size
* @return The geometry size
*/
const int CMD2Loader::GetGeometrySize() const
{
	return this->m_geometrySize;
}

/**
* Get the number of vertices
* @return The number of vertices
*/
const int CMD2Loader::GetNumVertices() const
{
	return this->m_header.m_numVertices;
}

/**
* Get the number of faces
* @return The number of faces
*/
const int CMD2Loader::GetNumFaces() const
{
	return this->m_header.m_numTriangle;
}

/**
* Get the triangles
* @return The triangles
*/
const SMD2Triangle * CMD2Loader::GetTriangles() const
{
	return this->m_triangles;
}

/**
* Get the position buffer
* @return The position buffer
*/
const D3DXVECTOR3 * CMD2Loader::GetPositionBuffer() const
{
	return this->m_positionBuffer;
}

/**
* Get the normal buffer
* @return The normal buffer
*/
const D3DXVECTOR3 * CMD2Loader::GetNormalBuffer() const
{
	return this->m_normalBuffer;
}

/**
* Get the texture coordinate buffer
* @return The texture coordinate buffer
*/
const D3DXVECTOR2 * CMD2Loader::GetTextureBuffer() const
{
	return this->m_textureBuffer;
}

/**
* Initialize the geometry
* - Get the texture coordinates
* - Compute the average normals
*/
void CMD2Loader::InitializeGeometry()
{
	// Copy texture coordinates
	for(int i = 0 ; i < this->m_header.m_numTriangle ; i++)
	{
		for(int j = 0 ; j < 3 ; j++)
		{
			this->m_textureBuffer[i * 3 + j].x = (float)(this->m_textureCoordinates[this->m_triangles[i].m_textureCoordinatesIndice[j]].m_u) / ((float)this->m_header.m_skinWidth);
			this->m_textureBuffer[i * 3 + j].y = (float)(this->m_textureCoordinates[this->m_triangles[i].m_textureCoordinatesIndice[j]].m_v) / ((float)this->m_header.m_skinHeight);
		}
	}

	// Get the real vertex positions
	for(int i = 0 ; i < this->m_header.m_numFrames ; i++)
	{
		for(int j = 0 ; j < this->m_header.m_numVertices ; j++)
		{
			this->m_frames[i].m_vertices[j].m_fullPosition.x = this->m_frames[i].m_vertices[j].m_position[0] * this->m_frames[i].m_scale[0] + this->m_frames[i].m_translation[0];
			this->m_frames[i].m_vertices[j].m_fullPosition.y = this->m_frames[i].m_vertices[j].m_position[1] * this->m_frames[i].m_scale[1] + this->m_frames[i].m_translation[1];
			this->m_frames[i].m_vertices[j].m_fullPosition.z = this->m_frames[i].m_vertices[j].m_position[2] * this->m_frames[i].m_scale[2] + this->m_frames[i].m_translation[2];
		}
	}

	// Average Normals calculation
	for(int i = 0 ; i < this->m_header.m_numFrames ; i++)
		AverageNormalsCalculation(i);
}

/**
* Perform the average normals calculation
* @param frameId The frame id to calculate
*/
void CMD2Loader::AverageNormalsCalculation(const int frameId)
{
	D3DXVECTOR3 tmpNormal;
	float facesCount;

	// Stock faces normals in the faces structure
	FaceNormalsCalculation(frameId);

	// Perform normal calculation for each vertex
	for(int i = 0 ; i < this->m_header.m_numVertices ; i++)
	{
		tmpNormal = D3DXVECTOR3(0,0,0);
		facesCount = 0;

		for(int j = 0 ; j < this->m_header.m_numTriangle ; j++)
		{
			// This triangle uses this vertex, so we add the face normal to the list
			if((int)this->m_triangles[j].m_vertices[0] == i || (int)this->m_triangles[j].m_vertices[1] == i || (int)this->m_triangles[j].m_vertices[2] == i)
			{
				tmpNormal += this->m_triangles[j].m_faceNormal;
				facesCount++;
			}
		}

		// Divide the normal by the number of founded triangles and normalize the result
		if(facesCount > 0)
			tmpNormal /= facesCount;
		D3DXVec3Normalize(&tmpNormal, &tmpNormal);

		// We use this normal
		this->m_frames[frameId].m_vertices[i].m_averageNormal = tmpNormal;
	}
}

/**
* Perform the face normals calculation
* @param frameId The frame id to calculate
*/
void CMD2Loader::FaceNormalsCalculation(const int frameId)
{
	// Perform face normal calculation for each triangle of the frame
	for(int i = 0 ; i < this->m_header.m_numTriangle ; i++)
	{
		// Normal calculation
		D3DXVECTOR3 ab = this->m_frames[frameId].m_vertices[this->m_triangles[i].m_vertices[0]].m_fullPosition - this->m_frames[frameId].m_vertices[this->m_triangles[i].m_vertices[2]].m_fullPosition;
		D3DXVECTOR3 ac = this->m_frames[frameId].m_vertices[this->m_triangles[i].m_vertices[2]].m_fullPosition - this->m_frames[frameId].m_vertices[this->m_triangles[i].m_vertices[1]].m_fullPosition;
		D3DXVec3Cross(&this->m_triangles[i].m_faceNormal, &ab, &ac);
	}
}

/**
* Do a linear interpolation of two vectors
* @param a The first vector
* @param b The second vector
* @param interpolation The interpolation value
*/
D3DXVECTOR3 CMD2Loader::DoLinearInterpolation(D3DXVECTOR3 a, D3DXVECTOR3 b, float interpolation)
{
	return a + interpolation * (b - a);
}

/**
* Do a cosine interpolation of two vectors
* @param a The first vector
* @param b The second vector
* @param interpolation The interpolation value
*/
D3DXVECTOR3 CMD2Loader::DoCosineInterpolation(D3DXVECTOR3 a, D3DXVECTOR3 b, float interpolation)
{
	D3DXVECTOR3 ret;

	ret.x = a.x * (1 - interpolation) + b.x * interpolation;
	ret.y = a.y * (1 - interpolation) + b.y * interpolation;
	ret.z = a.z * (1 - interpolation) + b.z * interpolation;

	return ret;
}
