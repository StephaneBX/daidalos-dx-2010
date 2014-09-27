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

#ifndef __CSCENESTATIC_H
#define __CSCENESTATIC_H

#define MAX_IMPACT 5
#define MIN_MOVE 0.02f
#define MAX_STOPPABLE_VERTICAL_ANGLE 35.0f

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "../../Misc/Global.h"
#include "../../Misc/CArraySorter.h"
#include "../../Engine/Physic/CImpact.h"
#include "../../Engine/Render/CScenePlane.h"

enum loadLevelState
{
	SCENE_LOAD_LEVEL_STATE_OK,
	SCENE_LOAD_LEVEL_STATE_INVALID
};

enum staticRenderType
{
	SCENE_RENDER_GEOMETRY_FACES,
	SCENE_RENDER_GEOMETRY_LIGHTS
};

class CSceneCamera;
class CSceneBSP3D;
class CSceneBSPTree;
class CShape;
class CMovement;
class CSceneNode;
class CSceneFace;
class CLogManager;
class CModManager;
class CLightManager;
class COptionManager;

/**
* CSceneStatic class.
* Manages the static part of the world.
* The goal here is to:
* - Maintain a bsp tree used for rendering the static part of the world.
* - Sort the visible geometry in order to use it efficiently.
* - Give to the engine all the necessary information to render the static part of the world.
*/
class CSceneStatic
{
public:
	int m_facesCounter;
	int m_nodesCounter;
	int m_visibleFacesCounter;
	int m_visibleLightFacesCounter;
	int m_vertexBufferSize;
	D3DXVECTOR3 m_collisionNormal[MAX_IMPACT];
	CImpact m_impact;
	CImpact m_iBrush;
	CArraySorter<LPDIRECT3DTEXTURE9> m_textureSorter;

	int * m_vertexOrder;
	CSceneBSP3D * m_rootGraphic;
	CSceneBSPTree * m_rootPhysic;

	SVertexElement * m_visibleFaces[VERTEX_PER_FACE];
	SVertexLightPass * m_visibleLightFaces;
	LPDIRECT3DTEXTURE9 * m_visibleTexturesDiffuse;
	LPDIRECT3DTEXTURE9 * m_visibleTexturesNormal;

	int * m_batchedVisibleFacesNumber;
	SVertexElement ** m_batchedVisibleFaces;
	LPDIRECT3DTEXTURE9 * m_batchedVisibleTexturesDiffuse;
	LPDIRECT3DTEXTURE9 * m_batchedVisibleTexturesNormal;

	SVertexElement * m_finalGeometry;

	CLogManager * m_logM;
	CModManager * m_modM;
	CLightManager * m_lightM;
	COptionManager * m_optionM;

public:
	CSceneStatic();
	~CSceneStatic();
	void LoadLevelFiles(const std::string levelName);
	void Render(CSceneCamera * const camera, const int renderType);
	void UnloadLevel();
	void ChangeBrushesCollision(const int brushesGroupId, const bool value);
	const bool Slide(CShape * const shape, CMovement * const movement, const bool resetGravity);
	const int GetFacesCounter() const;
	const int GetVertexBufferSize() const;
	const int GetVisibleFacesCounter() const;
	const int GetVisibleLightFacesCounter() const;
	const int * GetBatchedVisibleFacesNumber() const;
	const SVertexLightPass * GetVisibleLightFaces() const;
	const SVertexElement * GetFinalGeometry() const;
	const LPDIRECT3DTEXTURE9 * GetVisibleTexturesDiffuse() const;
	const LPDIRECT3DTEXTURE9 * GetVisibleTexturesNormal() const;
	const LPDIRECT3DTEXTURE9 * GetBatchedVisibleTexturesDiffuse() const;
	const LPDIRECT3DTEXTURE9 * GetBatchedVisibleTexturesNormal() const;

private:
	void LoadLightsProperties(const std::string filename);
	void FacesNodesCalculation(CSceneBSP3D * const bsp);
	void VertexBufferSizeCalculation(CSceneBSP3D * const node, std::vector<LPDIRECT3DTEXTURE9> * const textureList, std::vector<int> * const sizeList);
	void PrepareRendering(CSceneBSP3D * const node, CSceneCamera * const camera, const int renderType);
	void PrepareFace(CSceneBSP3D * const node, CSceneCamera * const camera);
	void PrepareLightsBuffer(CSceneBSP3D * const node);
	void GetCollision(CSceneNode * const node, CShape * const shape);
	void BatchGeometry();
	void FreeBuffers();
	void LogGraph(CSceneNode * const node);
	CSceneFace * LoadEnvironmentGeometry(const std::string filename);
	CSceneFace ** LoadStaticGeometry(const std::string filename, int * const returnedSize);
};

#endif