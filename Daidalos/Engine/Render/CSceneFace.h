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

#ifndef __CSCENEFACE_H
#define __CSCENEFACE_H

#define FACE_DEFAULT_ID -1

#include "../../Misc/Global.h"
#include "../../Engine/Render/CScenePlane.h"

class CShape;

/**
* CSceneFace class.
* Manages the faces on the world.
* This is the basic shape that modelize the world.
*/
class CSceneFace
{
public:
	int m_id;
	bool m_useFaceNormal;
	bool m_separationPlane;
	bool m_visible;
	bool m_drawn;
	float m_forCulling;
	LPDIRECT3DTEXTURE9 m_textureDiffuse;
	LPDIRECT3DTEXTURE9 m_textureNormal;
	SVertexElement m_geometry[VERTEX_PER_FACE];
	CScenePlane m_plane;
	CShape * m_boundingBox;

	CSceneFace * m_next;

public:
	CSceneFace();
	CSceneFace(CSceneFace * const face);
	CSceneFace(const D3DXVECTOR3 vertex[VERTEX_PER_FACE], const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible);
	CSceneFace(const D3DXVECTOR3 vertex[VERTEX_PER_FACE], const D3DXVECTOR3 normals[VERTEX_PER_FACE], const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible);
	CSceneFace(const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible);
	~CSceneFace();
	void SetPosition(const D3DXVECTOR3 vertex[VERTEX_PER_FACE]);
	void SetNormal(const D3DXVECTOR3 * const normal);
	void SetTexCoord(const D3DXVECTOR2 * const texCoord);
	void SetColor(const D3DXCOLOR color);
	void SetVisible(const bool visible);
	void SetTextureDiffuse(const LPDIRECT3DTEXTURE9 textureDiffuse);
	void SetTextureNormal(const LPDIRECT3DTEXTURE9 textureNormal);
	void BuildFace();
};

#endif