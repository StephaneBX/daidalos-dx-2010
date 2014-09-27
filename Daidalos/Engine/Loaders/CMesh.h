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

#ifndef __CMESH_H
#define __CMESH_H

#define MESH_DEFAULT_NS_MODIFIER 128.0f
#define MESH_DEFAULT_NS 30.0f

#include <d3d9.h>
#include <d3dx9.h>
#include "../../Misc/Global.h"

enum shadingType
{
	SHADING_TYPE_FLAT,
	SHADING_TYPE_SMOOTH
};

class CSceneFace;
class CSceneCamera;
class COptionManager;
class CModManager;

/**
* CMesh class.
* General class that read the mesh files in order to create the geometry.
* It's possible to render static or animated meshes.
*/
class CMesh
{
protected:
	bool m_geometryValid;
	int m_geometrySize;
	int m_visibleGeometrySize;
	float m_animationFPS;
	D3DXVECTOR3 m_defaultScale;
	SVertexElement * m_modifiedGeometry;
	SVertexLightPass * m_modifiedLightGeometry;

	CSceneFace * m_faces;
	COptionManager * m_optionM;
	CModManager * m_modM;

public:
	CMesh();
	virtual ~CMesh();
	virtual void Animate(const float frame);
	virtual void AnimateSourceCopy(const float frame, const int shadingType);
	virtual void GeometrySourceCopy(const int shadingType);
	virtual void GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals);
	virtual void GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType);
	virtual void UseCustomMaterial(std::string filename);
	const bool IsGeometryValid() const;
	const int GetGeometrySize() const;
	const int GetVisibleGeometrySize() const;
	const float GetAnimationFPS() const;
	CSceneFace * GetFaces() const;
	SVertexElement * GetModifiedGeometry() const;
	SVertexLightPass * GetModifiedLightGeometry() const;

protected:
	virtual void Load(const std::string fileName);
};

#endif