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

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <fstream>
#include "../Managers/CLogManager.h"

#define _VERIFY(x, text){if(x != S_OK){CLogManager::GetInstance()->GetStream() << "DX error : " << text << " (" << __FILE__ << ", " << __FUNCTION__ << "() in line " << __LINE__ << " code " << x << ")\n"; CLogManager::GetInstance()->Write();}}

#define SAFE_RELEASE(x){ if(x != NULL){ x->Release(); x = NULL; }}
#define SAFE_DELETE(x){ if(x != NULL){ delete x; x = NULL; }}
#define SAFE_DELETE_ARRAY(x){ if(x != NULL){ delete[] x; x = NULL; }}

#define MAXIMUM_ACTIVE_LIGHTS 50

#define DIRECTORY_STATIC_MESH "Ressources/Static/"
#define DIRECTORY_DYNAMIC_MESH "Ressources/Dynamic/"
#define DIRECTORY_TEXTURES "Ressources/Textures/"
#define DIRECTORY_SPRITES "Ressources/Sprites/"
#define DIRECTORY_MATERIALS "Ressources/Materials/"
#define DIRECTORY_SHADERS "Ressources/Shaders/"
#define DIRECTORY_SOUND_EFFECT "Ressources/Sounds/Effects/"
#define DIRECTORY_SOUND_MUSIC "Ressources/Sounds/Musics/"
#define DIRECTORY_LANGAGES "Ressources/Languages/Game/"
#define DIRECTORY_ENGINE_CONFIGURATION "Ressources/Engine/"
#define DIRECTORY_MODS "Mods/"
#define DIRECTORY_LEVELS "Levels/"
#define FILE_GEOMETRY "geometry.dd"
#define FILE_LIGHTS "lights.dd"
#define FILE_STATIC "static.dd"
#define FILE_DYNAMIC "dynamic.dd"

#define OBJECT_FILE_MAXIMUM_CHAR 50
#define OBJECT_MAGNETIC_CARD_NONE -1

#define EPSILON 0.001f
#define INFINITY 999999999.0f
#define GRAVITY -30.0f
#define PI 3.14159265f
#define PI_OVER_180 0.0174532925f
#define DEFAULT_BSP_SCORE 10000000.0f

#define KEYDOWN( buffer, key ) ( buffer[key] & 0x80 )
#define ACTION_KEY_DISABLED -1

enum nodeType
{
	BSP_NODE,
	BSP_EMPTY_LEAF,
	BSP_SOLID_LEAF
};

enum faceOrientation
{
	SCENE_FACE_ORIENTATION_FRONT,
	SCENE_FACE_ORIENTATION_BACK,
	SCENE_FACE_ORIENTATION_COLPLANAR,
	SCENE_FACE_ORIENTATION_SPANNING
};

enum objectMagneticCardType
{
	OBJECT_MAGNETIC_CARD_RED,
	OBJECT_MAGNETIC_CARD_BLUE,
	OBJECT_MAGNETIC_CARD_GREEN,
	OBJECT_MAGNETIC_CARD_YELLOW,
	OBJECT_MAGNETIC_CARD_SIZE
};

/**
* Structure used to store the faces
*/
struct SVertexElement
{
	D3DXVECTOR3 m_position;
	D3DXVECTOR2 m_texcoord;
	D3DXVECTOR3 m_normal;
	D3DXVECTOR3 m_tangent;
	D3DXVECTOR3 m_bitangent;
	D3DXCOLOR m_color;
};

/**
* Structure used to store the faces for the light pass
*/
struct SVertexLightPass
{
	D3DXVECTOR3 m_position;
};

inline bool Segment2SegmentIntersection(D3DXVECTOR3 * a, D3DXVECTOR3 * b, D3DXVECTOR3 * c, D3DXVECTOR3 * d)
{
	D3DXVECTOR3 ab = *b - *a;
	D3DXVECTOR3 cd = *d - *c;
	float k, j;
	float denom = ab.x * cd.y - ab.y * cd.x;

	// Parallel segment
	if(denom < EPSILON)
		return false;

	k = -(-ab.x * a->y + ab.x * c->y + ab.y * a->x - ab.y * c->x) / denom;
	j = -( a->x * cd.y - c->x * cd.y - cd.x * a->y + cd.x * c->y) / denom;

	return (k >= 0 && k <= 1 && j >= 0 && j <= 1);
}

inline bool TriangleInCircle2D(D3DXVECTOR3 * p1, D3DXVECTOR3 * p2, D3DXVECTOR3 * p3, D3DXVECTOR3 * c, float radius)
{
	// Test if the circle is in collision with one of the segments
	D3DXVECTOR3 d1, d2, d3;
	float t1, t2, t3;
	d1 = *p2 - *p1;
	d2 = *p3 - *p1;
	d3 = *p3 - *p2;
	t1 = -1 * ((p1->x - c->x) * d1.x + (p1->z - c->z) * d1.z) / ((d1.x * d1.x) + (d1.z * d1.z));
	t2 = -1 * ((p1->x - c->x) * d2.x + (p1->z - c->z) * d2.z) / ((d2.x * d2.x) + (d2.z * d2.z));
	t3 = -1 * ((p2->x - c->x) * d3.x + (p2->z - c->z) * d3.z) / ((d3.x * d3.x) + (d3.z * d3.z));

	if(t1 < 0.0)
		t1 = 0.0f;
	else 
		if(t1 > 1.0)
			t1 = 1.0f;

	if(t2 < 0.0)
		t2 = 0.0f;
	else 
		if(t2 > 1.0)
			t2 = 1.0f;

	if(t3 < 0.0)
		t3 = 0.0f;
	else 
		if(t3 > 1.0)
			t3 = 1.0f;

	d1 = (*p1 + t1 * d1) - *c;
	d2 = (*p1 + t2 * d2) - *c;
	d3 = (*p2 + t3 * d3) - *c;
	
	return (D3DXVec3Length(&d1) <= radius || D3DXVec3Length(&d2) <= radius || D3DXVec3Length(&d3) <= radius);
}

inline D3DXVECTOR3 ClosestPointOnLine(D3DXVECTOR3 * a, D3DXVECTOR3 * b, D3DXVECTOR3 * point, bool * valid)
{
	float d, t;
	D3DXVECTOR3 ab = *b - *a;
	D3DXVECTOR3 c = *point - *a;
	D3DXVECTOR3 dir;
	D3DXVec3Normalize(&dir, &ab);
	d = D3DXVec3Length(&ab);
	t = D3DXVec3Dot(&dir,&c);

	// Check to see if the point is on the line
	// if not then return the endpoint
	if(t < 0 || t > d)
		*valid = false;
	else
		*valid = true;

	dir = dir * t;

	return *a + dir;
}

inline bool FileExists(std::string filename)
{
	bool ret = false;
	std::ifstream file;
	file.open(filename.c_str(), std::ios::in);
	if(file)
	{
		ret = true;
		file.close();
	}
	return ret;
}

#endif
