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

#ifndef __COBJLOADER_H
#define __COBJLOADER_H

#define OBJ_LOADER_ELEMENT_PER_LINE 4

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

enum objLoaderState
{
	OBJ_LOADER_STATE_OK,
	OBJ_LOADER_STATE_INVALID_FORMAT
};

class CMaterial;
class CLogManager;
class CModManager;

/**
* CObjLoader class.
* Manages the reading of Wavefront OBJ files.
*/
class CObjLoader
{
private:
	int m_numberGroup;
	int m_fileState;
	int * m_bufferSize;
	int m_geometrySize;
	bool m_hasNormals;
	D3DXVECTOR3 ** m_vertex;
	D3DXVECTOR3 ** m_normal;
	D3DXVECTOR2 ** m_texture;
	CMaterial ** m_material;
	std::vector<CMaterial *> m_materialList;

	CLogManager * m_logM;
	CModManager * m_modM;

public:
	CObjLoader();
	~CObjLoader();
	void Load(const std::string filename);
	const int GetFileState() const;
	const int GetNumberGroup() const;
	const int GetGeometrySize() const;
	const int * GetBufferSize() const;
	const bool hasNormals() const;
	CMaterial ** GetMaterial() const;
	D3DXVECTOR3 ** GetVertex() const;
	D3DXVECTOR3 ** GetNormal() const;
	D3DXVECTOR2 ** GetTexture() const;

private:
	void FreeData();
	void FillBuffers(std::vector<D3DXVECTOR3> * const tmpVertex, std::vector<D3DXVECTOR3> * const tmpNormal, std::vector<D3DXVECTOR2> * const tmpTexture, std::vector<int> * const vertexOrder, std::vector<int> * const normalOrder, std::vector<int> * const textureOrder,  const int groupCounter);
};

#endif