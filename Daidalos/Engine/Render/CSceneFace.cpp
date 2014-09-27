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

#include "../../Misc/Global.h"
#include "../../Engine/Physic/CSphere.h"
#include "../../Engine/Render/CSceneFace.h"

/**
* Constructor
*/
CSceneFace::CSceneFace()
{
	this->m_visible = true;
	this->m_id = FACE_DEFAULT_ID;
	this->m_useFaceNormal = true;

	this->m_boundingBox = new CSphere();

	// Null by default
	this->m_next = NULL;
}

/**
* Constructor
* @param face A face reference
*/
CSceneFace::CSceneFace(CSceneFace * const face)
{
	this->m_useFaceNormal = face->m_useFaceNormal;
	this->m_id = FACE_DEFAULT_ID;
	this->m_textureDiffuse = face->m_textureDiffuse;
	this->m_textureNormal = face->m_textureNormal;

	memcpy(this->m_geometry, face->m_geometry, sizeof(SVertexElement) * VERTEX_PER_FACE);

	this->m_plane = face->m_plane;
	this->m_boundingBox = new CSphere(*face->m_boundingBox->GetPosition(), face->m_boundingBox->GetOffset(NULL));
	this->m_separationPlane = face->m_separationPlane;
	this->m_visible = face->m_visible;
	this->m_forCulling = face->m_forCulling;
	this->m_next = NULL;
}

/**
* Constructor
* @param vertex The vertex
* @param color The color
* @param texcoord The texture coordinates
* @param textureDiffuse The diffuse texture pointer
* @param textureNormal The normal texture pointer
* @param visible If the face is visible
*/
CSceneFace::CSceneFace(const D3DXVECTOR3 vertex[VERTEX_PER_FACE], const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible)
{
	this->m_id = FACE_DEFAULT_ID;
	this->m_visible = *visible;
	this->m_useFaceNormal = true;

	this->m_boundingBox = new CSphere();

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		this->m_geometry[i].m_position = vertex[i];
		this->m_geometry[i].m_texcoord = texcoord[i];
		this->m_geometry[i].m_color = *color;
	}

	this->m_textureDiffuse = textureDiffuse;
	this->m_textureNormal = textureNormal;

	// False by default
	this->m_separationPlane = false;

	// Null by default
	this->m_next = NULL;

	BuildFace();
}

/**
* Constructor
* @param vertex The vertex
* @param normals The normal
* @param color The color
* @param texcoord The texture coordinates
* @param textureDiffuse The diffuse texture pointer
* @param textureNormal The normal texture pointer
* @param visible If the face is visible
*/
CSceneFace::CSceneFace(const D3DXVECTOR3 vertex[VERTEX_PER_FACE], const D3DXVECTOR3 normals[VERTEX_PER_FACE], const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible)
{
	this->m_id = FACE_DEFAULT_ID;
	this->m_visible = *visible;
	this->m_useFaceNormal = false;

	this->m_boundingBox = new CSphere();

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		this->m_geometry[i].m_position = vertex[i];
		this->m_geometry[i].m_normal = normals[i];
		this->m_geometry[i].m_texcoord = texcoord[i];
		this->m_geometry[i].m_color = *color;
	}

	this->m_textureDiffuse = textureDiffuse;
	this->m_textureNormal = textureNormal;

	// False by default
	this->m_separationPlane = false;

	// Null by default
	this->m_next = NULL;

	BuildFace();
}

/**
* Constructor
* @param color The color
* @param texcoord The texture coordinates
* @param textureDiffuse The diffuse texture pointer
* @param textureNormal The normal texture pointer
* @param visible If the face is visible
*/
CSceneFace::CSceneFace(const D3DXCOLOR * const color, const D3DXVECTOR2 texcoord[VERTEX_PER_FACE], const LPDIRECT3DTEXTURE9 textureDiffuse, const LPDIRECT3DTEXTURE9 textureNormal, const bool * const visible)
{
	this->m_id = FACE_DEFAULT_ID;
	this->m_visible = *visible;
	this->m_useFaceNormal = true;

	this->m_textureDiffuse = textureDiffuse;
	this->m_textureNormal = textureNormal;

	this->m_boundingBox = new CSphere();

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		this->m_geometry[i].m_texcoord = texcoord[i];
		this->m_geometry[i].m_color = *color;
	}

	// False by default
	this->m_separationPlane = false;

	// Null by default
	this->m_next = NULL;
}

/**
* Destructor
*/
CSceneFace::~CSceneFace()
{
	SAFE_DELETE(this->m_boundingBox);
}

/**
* Set the position
* @param vertex The new vertices
*/
void CSceneFace::SetPosition(const D3DXVECTOR3 vertex[VERTEX_PER_FACE])
{
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		this->m_geometry[i].m_position = vertex[i];
}

/**
* Set the normals
* @param normal The new normals
*/
void CSceneFace::SetNormal(const D3DXVECTOR3 * const normal)
{
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		this->m_geometry[i].m_normal = normal[i];
	this->m_useFaceNormal = false;
}

/**
* Set the diffuse texture
* @param textureDiffuse The new diffuse texture
*/
void CSceneFace::SetTextureDiffuse(const LPDIRECT3DTEXTURE9 textureDiffuse)
{
	this->m_textureDiffuse = textureDiffuse;
}

/**
* Set the normal texture
* @param textureNormal The new normal texture
*/
void CSceneFace::SetTextureNormal(const LPDIRECT3DTEXTURE9 textureNormal)
{
	this->m_textureNormal = textureNormal;
}

/**
* Set the texture coordinate
* @param texCoord The new texture coordinate
*/
void CSceneFace::SetTexCoord(const D3DXVECTOR2 * const texCoord)
{
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		this->m_geometry[i].m_texcoord = texCoord[i];
}

/**
* Set the color
* @param color The new color
*/
void CSceneFace::SetColor(const D3DXCOLOR color)
{
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		this->m_geometry[i].m_color = color;
}

/**
* Change the face visibility state
* @param visible The new value
*/
void CSceneFace::SetVisible(const bool visible)
{
	this->m_visible = visible;
}

/**
* Build the face
* The goal here is to calculate the normal, binormal, tangent vector and the bounding box
*/
void CSceneFace::BuildFace()
{
	// Normal calculation
	D3DXVECTOR3 ab = this->m_geometry[1].m_position - this->m_geometry[0].m_position;
	D3DXVECTOR3 ac = this->m_geometry[2].m_position - this->m_geometry[0].m_position;

	D3DXVec3Cross(&this->m_plane.m_normalVector, &ab, &ac);
	D3DXVec3Normalize(&this->m_plane.m_normalVector, &this->m_plane.m_normalVector);

	// Use the normal plane as normals if the face uses flat shading
	if(this->m_useFaceNormal)
	{
		for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
			this->m_geometry[i].m_normal = this->m_plane.m_normalVector;
	}

	// Tangent and BiTangent calculation
	D3DXVECTOR2 abuv = this->m_geometry[1].m_texcoord - this->m_geometry[0].m_texcoord;
	D3DXVECTOR2 acuv = this->m_geometry[2].m_texcoord - this->m_geometry[0].m_texcoord;
	float fDenominator = abuv.x * acuv.y - acuv.x * abuv.y;

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		if(fDenominator >= -EPSILON && fDenominator <= EPSILON)
		{
			this->m_geometry[i].m_tangent = D3DXVECTOR3(1,0,0);
			D3DXVec3Cross(&this->m_geometry[i].m_bitangent, &this->m_geometry[i].m_tangent, &this->m_geometry[i].m_normal);
			D3DXVec3Normalize(&this->m_geometry[i].m_bitangent, &this->m_geometry[i].m_bitangent);
		}
		else
		{
			float fScale1 = 1.0f / fDenominator;
			this->m_geometry[i].m_tangent = (acuv.y * ab - abuv.y * ac) * fScale1;
			this->m_geometry[i].m_bitangent = (acuv.x * ab - abuv.x * ac) * fScale1;

			D3DXVec3Normalize(&this->m_geometry[i].m_tangent, &this->m_geometry[i].m_tangent);
			D3DXVec3Normalize(&this->m_geometry[i].m_bitangent, &this->m_geometry[i].m_bitangent);

			// Gram-Schmidt orthogonalization
			this->m_geometry[i].m_tangent -= this->m_geometry[i].m_normal * D3DXVec3Dot(&this->m_geometry[i].m_tangent, &this->m_geometry[i].m_normal);
			D3DXVec3Normalize(&this->m_geometry[i].m_tangent, &this->m_geometry[i].m_tangent);
			D3DXVec3Cross(&this->m_geometry[i].m_bitangent, &this->m_geometry[i].m_tangent, &this->m_geometry[i].m_normal);
			D3DXVec3Normalize(&this->m_geometry[i].m_bitangent, &this->m_geometry[i].m_bitangent);
		}
	}

	// Constant calculation
	this->m_plane.m_constant = -(this->m_geometry[1].m_position.x * this->m_plane.m_normalVector.x + this->m_geometry[1].m_position.y * this->m_plane.m_normalVector.y + this->m_geometry[1].m_position.z * this->m_plane.m_normalVector.z);

	// Culling
	this->m_forCulling = D3DXVec3Dot(&this->m_plane.m_normalVector, &this->m_geometry[0].m_position);

	// Bounding box calculation
	D3DXVECTOR3 gravityCenter = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 radiusCoord;
	float radius = 0;
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		gravityCenter.x += this->m_geometry[i].m_position.x;
		gravityCenter.y += this->m_geometry[i].m_position.y;
		gravityCenter.z += this->m_geometry[i].m_position.z;
	}
	gravityCenter /= VERTEX_PER_FACE;
	radiusCoord = gravityCenter - this->m_geometry[0].m_position;
	radius = D3DXVec3Length(&radiusCoord);

	this->m_boundingBox->SetPosition(&gravityCenter);
	((CSphere*)this->m_boundingBox)->SetOffset(radius);
}
