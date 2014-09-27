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

#ifndef __CMD2DLOADER_H
#define __CMD2DLOADER_H

#define MD2_ID (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION 8
#define MD2_SKIN_NAME_SIZE 68
#define MD2_FRAME_NAME_SIZE 16
#define MD2_NUMVERTEX_NORMALS 162

#include <d3d9.h>
#include <d3dx9.h>

enum md2LoaderState
{
	MD2_LOADER_STATE_OK,
	MD2_LOADER_STATE_INVALID_FORMAT,
	MD2_LOADER_STATE_FILE_NOT_FOUND
};

struct SMD2Header
{
	int m_id;
	int m_version;
	int m_skinWidth;
	int m_skinHeight;
	int m_frameSize;
	int m_numSkins;
	int m_numVertices;
	int m_numTextureCoordinates;
	int m_numTriangle;
	int m_numGLCommands;
	int m_numFrames;

	int m_offsetSkins;
	int m_offsetTextureCoordinates;
	int m_offsetTriangles;
	int m_offsetFrames;
	int m_offsetGLCommands;
	int m_offsetEnd;
};

struct SMD2Vertex
{
	unsigned char m_position[3];
	unsigned char m_normalIndice;
	D3DXVECTOR3 m_fullPosition;
	D3DXVECTOR3 m_averageNormal;
};

struct SMD2Triangle
{
	short m_vertices[3];
	short m_textureCoordinatesIndice[3];
	D3DXVECTOR3 m_faceNormal;
};

struct SMD2TextureCoordinates
{
	short m_u;
	short m_v;
};

struct SMD2Frame
{
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_translation;
	char m_name[MD2_FRAME_NAME_SIZE];
	SMD2Vertex * m_vertices;
};

struct SMD2Skin
{
	char m_name[MD2_SKIN_NAME_SIZE];
};

struct SMD2Face
{
	D3DXVECTOR3 m_position[3];
	D3DXVECTOR3 m_normal[3];
	D3DXVECTOR2 m_textureCoordinates[3];
};

class CLogManager;

/**
* CMD2Loader class.
* Manages the reading of MD2 files.
*/
class CMD2Loader
{
private:
	int m_fileState;
	int m_geometrySize;

	SMD2Header m_header;
	SMD2Skin * m_skins;
	SMD2TextureCoordinates * m_textureCoordinates;
	SMD2Triangle * m_triangles;
	SMD2Frame * m_frames;

	D3DXMATRIX m_rotation;
	D3DXMATRIX m_rotation2;

	D3DXVECTOR3 * m_positionBuffer;
	D3DXVECTOR3 * m_normalBuffer;
	D3DXVECTOR2 * m_textureBuffer;

	CLogManager * m_logM;

public:
	CMD2Loader();
	~CMD2Loader();
	void Load(const std::string filename);
	void Animate(const float frame);
	void GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals);
	const int GetFileState() const;
	const int GetGeometrySize() const;
	const int GetNumVertices() const;
	const int GetNumFaces() const;
	const SMD2Triangle * GetTriangles() const;
	const D3DXVECTOR3 * GetPositionBuffer() const;
	const D3DXVECTOR3 * GetNormalBuffer() const;
	const D3DXVECTOR2 * GetTextureBuffer() const;

private:
	void InitializeGeometry();
	void AverageNormalsCalculation(const int frameId);
	void FaceNormalsCalculation(const int frameId);
	D3DXVECTOR3 DoLinearInterpolation(D3DXVECTOR3 a, D3DXVECTOR3 b, float interpolation);
	D3DXVECTOR3 DoCosineInterpolation(D3DXVECTOR3 a, D3DXVECTOR3 b, float interpolation);
};

#endif