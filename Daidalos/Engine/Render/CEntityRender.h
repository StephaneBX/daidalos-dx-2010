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

#ifndef __CENTITYRENDER_H
#define __CENTITYRENDER_H

#define MAX_MESH_PER_OBJECT 2

#include <d3d9.h>
#include <d3dx9.h>
#include "../../Misc/Global.h"

class CSceneCamera;
class CMesh;
class CModManager;
class CDeviceManager;
class CLogManager;

/**
* CEntityRender class.
* Generic render class that render the entities.
* A render can be composed of more that one 3d model.
*/
class CEntityRender
{
private:
	int m_meshGeometrySize[MAX_MESH_PER_OBJECT];
	int m_meshVisibleGeometrySize[MAX_MESH_PER_OBJECT];
	float m_frame;
	float m_animationSpeed;
	float m_animationFPS;
	D3DXVECTOR3 m_scale;
	CSceneCamera * m_camera;

	LPDIRECT3DTEXTURE9 m_textureDiffuse[MAX_MESH_PER_OBJECT];
	LPDIRECT3DTEXTURE9 m_textureNormal[MAX_MESH_PER_OBJECT];
	CMesh * m_mesh[MAX_MESH_PER_OBJECT];

	LPDIRECT3DVERTEXBUFFER9 m_vertexBuffer[MAX_MESH_PER_OBJECT];

	CModManager * m_modM;
	CDeviceManager * m_deviceM;
	CLogManager * m_logM;

public:
	CEntityRender();
	~CEntityRender();
	void Update(const float time);
	void LoadMesh(const std::string meshName, const int slot);
	void LoadDiffuseTexture(const std::string fileName, const int slot);
	void LoadNormalTexture(const std::string fileName, const int slot);
	void LoadMaterial(const std::string fileName, const int slot);
	void SetFrame(const float frame);
	void SetAnimationSpeed(const float speed);
	void SetAnimationFPS(const float fps);
	void DestroyGeometry();
	void InitializeGeometry();
	void FinalizeRender();
	void ResetRender();
	void FillLightsGeometry(SVertexLightPass * const geometry, int * const start);
	void LinkCamera(CSceneCamera * const camera);
	const int GetGeometrySize(const int mesh) const;
	const int GetVisibleGeometrySize(const int mesh) const;
	const float GetFrame() const;
	const float GetAnimationFPS() const;
	const LPDIRECT3DTEXTURE9 GetTextureDiffuse(const int mesh) const;
	const LPDIRECT3DTEXTURE9 GetTextureNormal(const int mesh) const;
	const LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer(const int mesh) const;
	CSceneCamera * GetCamera() const;
	CMesh ** GetMesh();
};


#endif