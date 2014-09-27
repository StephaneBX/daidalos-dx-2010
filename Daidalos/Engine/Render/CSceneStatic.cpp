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

#include "../../Managers/CLogManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CLightManager.h"
#include "../../Managers/COptionManager.h"
#include "../../Engine/Loaders/CMesh.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Engine/Render/CSceneBSP3D.h"
#include "../../Engine/Render/CSceneBSPTree.h"
#include "../../Engine/Render/CSceneNode.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Physic/CShape.h"
#include "../../Engine/Physic/CMovement.h"
#include "../../Engine/CTokenizer.h"
#include "../../Engine/Render/CSceneStatic.h"

/**
* Constructor
*/
CSceneStatic::CSceneStatic()
{
	this->m_logM = CLogManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_lightM = CLightManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();

	this->m_rootGraphic = NULL;
	this->m_rootPhysic = NULL;

	this->m_vertexOrder = NULL;
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
			this->m_visibleFaces[i] = NULL;

	this->m_visibleLightFaces = NULL;
	this->m_visibleTexturesDiffuse = NULL;
	this->m_visibleTexturesNormal = NULL;
	this->m_batchedVisibleFaces = NULL;
	this->m_batchedVisibleFacesNumber = NULL;
	this->m_batchedVisibleTexturesDiffuse = NULL;
	this->m_batchedVisibleTexturesNormal = NULL;

	this->m_finalGeometry = NULL;

	this->m_facesCounter = 0;
	this->m_nodesCounter = 0;
	this->m_visibleLightFacesCounter = 0;
}

/**
* Destructor
*/
CSceneStatic::~CSceneStatic()
{
	UnloadLevel();
}

/**
* Load the static files of a level
* @param levelName The level name
*/
void CSceneStatic::LoadLevelFiles(const std::string levelName)
{
	std::ostringstream stream;
	std::string tmpString;
	CSceneFace * envList = NULL;
	CSceneFace ** staticList = NULL;
	CSceneFace ** geometryList = NULL;
	int staticSize = 0;

	// Load environment geometry file
	stream << levelName << "/" << FILE_GEOMETRY;
	tmpString = stream.str();
	envList = LoadEnvironmentGeometry(tmpString);

	// Load static geometry file
	stream.str("");
	stream << levelName << "/" << FILE_STATIC;
	tmpString = stream.str();
	staticList = LoadStaticGeometry(tmpString, &staticSize);

	// Prepare the final list
	geometryList = new CSceneFace*[staticSize + 1];
	geometryList[0] = envList;
	for(int i = 1 ; i < staticSize + 1 ; i++)
		geometryList[i] = staticList[i - 1];
	delete[] staticList;

	// Prepare the visual bsp
	this->m_rootGraphic = new CSceneBSP3D();

	// Add the geometry
	for(int i = 0 ; i < staticSize + 1 ; i++)
	{
		CSceneFace * tmpGeometry = geometryList[i];
		while(tmpGeometry != NULL)
		{
			this->m_rootGraphic->AddFace(new CSceneFace(tmpGeometry));
			tmpGeometry = tmpGeometry->m_next;
		}
	}

	// Prepare the physic bsp and add the geoetry
	this->m_rootPhysic = new CSceneBSPTree(geometryList, staticSize + 1);
	delete[] geometryList;

	this->m_facesCounter = 0;
	this->m_nodesCounter = 0;
	FacesNodesCalculation(this->m_rootGraphic);

	// Load lights properties
	stream.str("");
	stream << levelName << "/" << FILE_LIGHTS;
	tmpString = stream.str();
	LoadLightsProperties(tmpString);

	// Unload previous allocations
	FreeBuffers();

	// Prepare the faces allocator
	this->m_vertexOrder = new int[this->m_facesCounter];
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		this->m_visibleFaces[i] = new SVertexElement[this->m_facesCounter];

	this->m_visibleLightFaces = new SVertexLightPass[this->m_facesCounter * VERTEX_PER_FACE];
	this->m_visibleTexturesDiffuse = new LPDIRECT3DTEXTURE9[this->m_facesCounter];
	this->m_visibleTexturesNormal = new LPDIRECT3DTEXTURE9[this->m_facesCounter];

	this->m_batchedVisibleFacesNumber = new int[this->m_facesCounter * VERTEX_PER_FACE];
	this->m_batchedVisibleTexturesDiffuse = new LPDIRECT3DTEXTURE9[this->m_facesCounter * VERTEX_PER_FACE];
	this->m_batchedVisibleTexturesNormal = new LPDIRECT3DTEXTURE9[this->m_facesCounter * VERTEX_PER_FACE];

	for(int i = 0 ; i < this->m_facesCounter * VERTEX_PER_FACE ; i++)
	{
		this->m_batchedVisibleTexturesDiffuse[i] = NULL;
		this->m_batchedVisibleTexturesNormal[i] = NULL;
	}

	this->m_batchedVisibleFaces = new SVertexElement*[this->m_facesCounter * VERTEX_PER_FACE];
	this->m_finalGeometry = new SVertexElement[this->m_facesCounter * VERTEX_PER_FACE];

	for(int i = 0 ; i < this->m_facesCounter * VERTEX_PER_FACE ; i++)
		this->m_batchedVisibleFaces[i] = NULL;


	// Find the best size for the vertex buffer that will hold the geometry.
	// Find the texture that uses the most triangles
	std::vector<LPDIRECT3DTEXTURE9> textureList;
	std::vector<int> sizeList;

	VertexBufferSizeCalculation(this->m_rootGraphic, &textureList, &sizeList);

	this->m_vertexBufferSize = 0;

	// Search the higher value
	for(unsigned int i = 0 ; i < sizeList.size() ; i++)
		if(sizeList.at(i) > this->m_vertexBufferSize)
			this->m_vertexBufferSize = sizeList.at(i);

	this->m_vertexBufferSize *= VERTEX_PER_FACE;

	// Log
	if(this->m_optionM->IsDebugModeEnabled(OPTIONMANAGER_CONFIGURATION_ACTIVE))
	{
		this->m_logM->GetStream() << "Faces: " << this->m_facesCounter << " - Nodes: " << this->m_nodesCounter << "\n";
		this->m_logM->Write();
	}
}

/**
* Render the scene
* @param camera A pointer to the camera
* @param renderType The render type
*/
void CSceneStatic::Render(CSceneCamera * const camera, const int renderType)
{
	this->m_visibleFacesCounter = 0;

	if(renderType == SCENE_RENDER_GEOMETRY_LIGHTS)
		this->m_visibleLightFacesCounter = 0;

	this->m_rootGraphic->ResetDraw();
	PrepareRendering(this->m_rootGraphic, camera, renderType);
	this->m_visibleFacesCounter *= VERTEX_PER_FACE;
	BatchGeometry();
}

/**
* Unload the level
*/
void CSceneStatic::UnloadLevel()
{
	FreeBuffers();

	SAFE_DELETE(this->m_rootGraphic);
	SAFE_DELETE(this->m_rootPhysic);

	this->m_facesCounter = 0;
	this->m_nodesCounter = 0;
	this->m_visibleLightFacesCounter = 0;
}

/**
* Change a brush collision state
* @param brushesGroupId The brush id
* @param value The collision state
*/
void CSceneStatic::ChangeBrushesCollision(const int brushesGroupId, const bool value)
{
	this->m_rootPhysic->SetBrushCollision(brushesGroupId, value);
}

/**
* Slide an object to the bsp tree if a collision is detected.
* Move the object to its final position otherwise
* @param shape The object shape
* @param movement The object movement
* @param resetGravity If the collision must reset the gravity
* @return The collision test result
*/
const bool CSceneStatic::Slide(CShape * const shape, CMovement * const movement, const bool resetGravity)
{
	D3DXVECTOR3 startPosition, endPosition, distance, edge, velocity, distanceLast;
	bool isCollision = false;
	int i, j, k;
	int collisionNormalIndex = 0;
	float fraction = 1;
	float collisionAngle;
	float dot;

	// Starting values
	startPosition = *shape->GetPosition();
	velocity = *movement->GetDistance();
	distanceLast = velocity;

	// Main loop
	for(i = 0 ; i < MAX_IMPACT ; i++)
	{
		// End position calculation
		endPosition = startPosition + distanceLast;

		// Impact objects calculation
		this->m_impact.Initialize(&startPosition, &endPosition);
		this->m_iBrush.Initialize(&startPosition, &endPosition);

		// Collision test
		this->m_rootPhysic->CollisionDetection(shape, &this->m_impact, &this->m_iBrush);

		// Collision detected
		if(this->m_impact.IsImpact())
		{
			isCollision = true;
			if(resetGravity)
			{
				D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
				collisionAngle = (180.0f * acos(D3DXVec3Dot(&up, this->m_impact.GetPlaneNormal())) / PI);

				// Reset gravity when a vertical collision occurs with a surface less or equal that MAX_STOPPABLE_VERTICAL_ANGLE degrees
				if(collisionAngle <= MAX_STOPPABLE_VERTICAL_ANGLE)
					movement->ResetGravity();
			}
		}
		else
		{
			// If not, we give its final position to the object (no collision detected)
			shape->SetPosition(&endPosition);
			return isCollision;
		}

		// Fraction update
		fraction -= fraction * this->m_impact.GetImpactFraction();

		// Distance calculation
		distance = *this->m_impact.GetDistance() * this->m_impact.GetImpactFraction();

		if(D3DXVec3Length(&distance) > MIN_MOVE)
			collisionNormalIndex = 0;

		// Put the plane normal
		this->m_collisionNormal[collisionNormalIndex] = *this->m_impact.GetPlaneNormal();
		collisionNormalIndex++;

		startPosition += distance;

		// Count the valid normals
		for(j = 0 ; j < collisionNormalIndex ; j++)
		{
			dot = D3DXVec3Dot(&velocity, &this->m_collisionNormal[j]);
			distanceLast = velocity - this->m_collisionNormal[j] * dot;
			for(k = 0 ; k < collisionNormalIndex ; k++)
			{
				if(j != k)
				{
					if(D3DXVec3Dot(&distanceLast, &this->m_collisionNormal[k]) < 0.0f)
						break;
				}
			}

			if(k == collisionNormalIndex)
				break;
		}

		// Perform sliding position calculation
		if(i == collisionNormalIndex)
		{
			if(collisionNormalIndex == 2)
			{
				D3DXVec3Cross(&edge, &this->m_collisionNormal[0], &this->m_collisionNormal[1]);
				D3DXVec3Normalize(&edge, &edge);
				dot = D3DXVec3Dot(&edge, &velocity);
				distanceLast = edge * dot;
			}
			else
			{
				shape->SetPosition(&startPosition);
				return isCollision;
			}
		}

		distanceLast *= fraction;

		// Don't move if the sliding movement is toward the velocity or too small
		if(D3DXVec3Dot(&velocity, &distanceLast) < 0 || D3DXVec3Length(&distanceLast) < MIN_MOVE)
		{
			D3DXVECTOR3 distance = startPosition - *shape->GetPosition();
			if(D3DXVec3Length(&distance) >= MIN_MOVE)
				shape->SetPosition(&startPosition);
			return isCollision;
		}
	}

	return isCollision;
}

/**
* Get the number of faces
* @return The number of faces
*/
const int CSceneStatic::GetFacesCounter() const
{
	return this->m_facesCounter;
}

/**
* Get the vertex buffer size
* @return The vertex buffer size
*/
const int CSceneStatic::GetVertexBufferSize() const
{
	return this->m_vertexBufferSize;
}

/**
* Get the number of visible faces
* @return The number of visible faces
*/
const int CSceneStatic::GetVisibleFacesCounter() const
{
	return this->m_visibleFacesCounter;
}

/**
* Get the number of visible faces for the lights
* @return The number of visible faces for the lights
*/
const int CSceneStatic::GetVisibleLightFacesCounter() const
{
	return this->m_visibleLightFacesCounter;
}

/**
* Get a table corresponding to the number of faces after batching
* @return A table corresponding to the number of faces after batching
*/
const int * CSceneStatic::GetBatchedVisibleFacesNumber() const
{
	return this->m_batchedVisibleFacesNumber;
}

/**
* Get the visible geometry for the lights
* @return The visible geometry for the lights
*/
const SVertexLightPass * CSceneStatic::GetVisibleLightFaces() const
{
	return this->m_visibleLightFaces;
}

/**
* Get the final geometry
* @return The final geometry
*/
const SVertexElement * CSceneStatic::GetFinalGeometry() const
{
	return this->m_finalGeometry;
}

/**
* Get the visible diffuse texture
* @return The visible diffuse texture
*/
const LPDIRECT3DTEXTURE9 * CSceneStatic::GetVisibleTexturesDiffuse() const
{
	return this->m_visibleTexturesDiffuse;
}

/**
* Get the visible normal texture
* @return The visible normal texture
*/
const LPDIRECT3DTEXTURE9 * CSceneStatic::GetVisibleTexturesNormal() const
{
	return this->m_visibleTexturesNormal;
}

/**
* Get the visible diffuse texture after batching
* @return The visible diffuse texture after batching
*/
const LPDIRECT3DTEXTURE9 * CSceneStatic::GetBatchedVisibleTexturesDiffuse() const
{
	return this->m_batchedVisibleTexturesDiffuse;
}

/**
* Get the visible normal texture after batching
* @return The visible normal texture after batching
*/
const LPDIRECT3DTEXTURE9 * CSceneStatic::GetBatchedVisibleTexturesNormal() const
{
	return this->m_batchedVisibleTexturesNormal;
}

/**
* Load the static lights file of the level
* @param filename The static lights file name
*/
void CSceneStatic::LoadLightsProperties(const std::string filename)
{
	D3DXVECTOR3 position, direction;
	D3DXVECTOR4 color;
	float angle, length;
	bool shadowCaster;

	CTokenizer tokenizer(filename);
	if(tokenizer.m_tokens.size() > 0)
	{
		// Light properties found
		tokenizer.Tokenize(" ");
		for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ; )
		{
			if(tokenizer.m_tokens.at(i) == "light")
			{
				// New light, initialization of the variables
				position = D3DXVECTOR3(0,0,0);
				direction = D3DXVECTOR3(0,-1,0);
				color = D3DXVECTOR4(1,1,1,1);
				angle = 0;
				length = 0;
				shadowCaster = false;
				i++;
			}
			else
			{
				if(tokenizer.m_tokens.at(i) == "position")
				{
					// Read position
					position = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
					i += 4;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "direction")
					{
						// Read direction
						direction = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
						D3DXVec3Normalize(&direction, &direction);
						i += 4;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "color")
						{
							// Read color
							color = D3DXVECTOR4((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()),1);
							i += 5;
						}
						else
						{
							if(tokenizer.m_tokens.at(i) == "angle")
							{
								// Read angle
								angle = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
								i += 2;
							}
							else
							{
								if(tokenizer.m_tokens.at(i) == "length")
								{
									// Read length
									length = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
									i += 2;
								}
								else
								{
									if(tokenizer.m_tokens.at(i) == "shadowCaster")
									{
										// Shadow caster light type
										shadowCaster = true;
										i++;
									}
									else
									{
										if(tokenizer.m_tokens.at(i) == "end")
										{
											// End of the light propertie, add the light if the variables are correct
											if( angle > 0 && length > 0)
											{
												D3DXVec3Normalize(&direction, &direction);
												this->m_lightM->AddLight(&position, &direction, &color, &angle, &length, &shadowCaster);
											}
											i++;
										}
										else
											i++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
* Calculate the number of faces per node and the number of nodes
* @param bsp A pointer to the current node
*/
void CSceneStatic::FacesNodesCalculation(CSceneBSP3D * const bsp)
{
	if(bsp == NULL)
		return;

	this->m_nodesCounter++;

	if(bsp->m_type != BSP_NODE)
		return;

	this->m_facesCounter += bsp->m_facesSize;

	FacesNodesCalculation(bsp->m_children[SCENE_FACE_ORIENTATION_FRONT]);
	FacesNodesCalculation(bsp->m_children[SCENE_FACE_ORIENTATION_BACK]);
}

/**
* Count the number of vertex per texture
* @param node The current node
* @param textureList The texture list
* @param sizeList The size list
*/
void CSceneStatic::VertexBufferSizeCalculation(CSceneBSP3D * const node, std::vector<LPDIRECT3DTEXTURE9> * const textureList, std::vector<int> * const sizeList)
{
	if(node == NULL || node->m_type != BSP_NODE)
		return;

	// A node that contains faces
	if(node->m_facesSize > 0)
	{
		CSceneFace * currentFace = node->m_facesList;
		while(currentFace != NULL)
		{
			if(currentFace->m_textureDiffuse != NULL)
			{
				bool textureFound = false;

				// Test if the texture is already in the texture list
				for(unsigned int i = 0 ; i < textureList->size() && !textureFound ; i++)
				{
					// Increment the value if the texture already exists
					if(textureList->at(i) == currentFace->m_textureDiffuse)
					{
						sizeList->at(i)++;
						textureFound = true;
					}
				}

				// Push the texture if it doesn't exist
				if(!textureFound)
				{
					textureList->push_back(currentFace->m_textureDiffuse);
					sizeList->push_back(1);
				}
			}

			currentFace = currentFace->m_next;
		}
	}

	if(node->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
		VertexBufferSizeCalculation(node->m_children[SCENE_FACE_ORIENTATION_FRONT], textureList, sizeList);
	if(node->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
		VertexBufferSizeCalculation(node->m_children[SCENE_FACE_ORIENTATION_BACK], textureList, sizeList);
}

/**
* Prepare the rendering
* Visit the bsp tree in search for potiential visible faces to prepare
* @param node The node t prepare
* @param camera The game camera
*/
void CSceneStatic::PrepareRendering(CSceneBSP3D * const node, CSceneCamera * const camera, const int renderType)
{
	if(node == NULL || node->m_type != BSP_NODE)
		return;

	if(node->m_facesSize > 0)
	{
		if(renderType == SCENE_RENDER_GEOMETRY_FACES)
			PrepareFace(node, camera);
		else
			PrepareLightsBuffer(node);
	}

	// Prepare the correct node depending on the camera position
	if (camera != NULL)
	{
		if(node->m_plane->PointDistance(&camera->m_position) > EPSILON)
		{
			if(node->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
				PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_BACK], camera, renderType);
			if(node->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
				PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_FRONT], camera, renderType);
		}
		else
		{
			if(node->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
				PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_FRONT], camera, renderType);
			if(node->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
				PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_BACK], camera, renderType);
		}
	}
	else
	{
		PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_FRONT], camera, renderType);
		PrepareRendering(node->m_children[SCENE_FACE_ORIENTATION_BACK], camera, renderType);
	}
}

/**
* Prepare a list of faces for rendering
* Test each face in order to determine if it's visible. Discard or copy its property depending on the result
* @param node The node containing the faces
* @param camera The game camera
*/
void CSceneStatic::PrepareFace(CSceneBSP3D * const node, CSceneCamera * const camera)
{
	CSceneFace * currentFace = node->m_facesList;

	// Fill the visible faces buffer with the faces in the list
	while(currentFace != NULL)
	{
		if(currentFace->m_visible && !currentFace->m_drawn && !camera->BackfaceCull(currentFace) && !camera->FrustumCull(currentFace->m_boundingBox))
		{
			for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
				memcpy(&this->m_visibleFaces[i][this->m_visibleFacesCounter], &currentFace->m_geometry[i], sizeof(SVertexElement));

			this->m_visibleTexturesDiffuse[this->m_visibleFacesCounter] = currentFace->m_textureDiffuse;
			this->m_visibleTexturesNormal[this->m_visibleFacesCounter] = currentFace->m_textureNormal;
			this->m_visibleFacesCounter++;
			currentFace->m_drawn = true;
		}

		currentFace = currentFace->m_next;
	}
}

/**
* Prepare a list of faces for shadows rendering
* @param node The node containing the faces
*/
void CSceneStatic::PrepareLightsBuffer(CSceneBSP3D * const node)
{
	CSceneFace * currentFace = node->m_facesList;

	// Fill the faces buffer with the faces in the list
	while(currentFace != NULL)
	{
		// Temp light buffer modification for shadow mapping
		if(currentFace->m_visible && this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
		{
			for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
			{
 				this->m_visibleLightFaces[this->m_visibleLightFacesCounter].m_position = currentFace->m_geometry[i].m_position;
				this->m_visibleLightFacesCounter++;
			}
		}

		currentFace = currentFace->m_next;
	}
}

/**
* Test a collision between an object and the bsp tree
* Suppose that the m_impact and m_iBrush elements have been correctly initialized before calling this function
* @param shape The object shape
*/
void CSceneStatic::GetCollision(CSceneNode * const node, CShape * const shape)
{
	float impactFraction, realFraction;
	D3DXVECTOR3 * normalTemp;

	if(node == NULL)
		return;

	// Empty leaf
	if(node->m_type == BSP_EMPTY_LEAF)
	{
		this->m_iBrush.ClearImpact();
		return;
	}

	// Solid leaf
	if(node->m_type == BSP_SOLID_LEAF)
	{
		this->m_impact.SetNearerImpact(this->m_iBrush.GetPlaneNormal(), this->m_iBrush.GetImpactFraction(), this->m_iBrush.GetRealFraction());
		this->m_iBrush.ClearImpact();
		return;
	}

	// Node
	float d1 = node->m_plane->PointDistance(this->m_impact.GetStartPoint());
	float d2 = node->m_plane->PointDistance(this->m_impact.GetEndPoint());

	// Offset calculation
	float offset = shape->GetOffset(node->m_plane);

	// The object is in front
	if(d1 > offset && d2 > offset)
	{
		GetCollision(node->m_children[SCENE_FACE_ORIENTATION_FRONT], shape);
		return;
	}

	node->m_plane->Inverse();
	float offset2 = shape->GetOffset(node->m_plane);
	node->m_plane->Inverse();

	// The object is in the back
	if(d1 < -offset2 && d2 < -offset2)
	{
		GetCollision(node->m_children[SCENE_FACE_ORIENTATION_BACK], shape);
		return;
	}

	float d1r = d1 - offset;
	float d2r = d2 - offset;
	realFraction = d1r / (d1r - d2r);
	
	if(d1 > d2 && realFraction < this->m_iBrush.GetRealFraction()) 
	{
		float d1i = d1 - MIN_DISTANCE - offset;
		float d2i = d2 - MIN_DISTANCE - offset;
		impactFraction = d1i / (d1i - d2i);
		
		this->m_iBrush.SetImpact(&node->m_plane->m_normalVector, impactFraction, realFraction);
        
		normalTemp = this->m_iBrush.GetPlaneNormal();
	}
	else 
	{
		normalTemp = this->m_iBrush.GetPlaneNormal();
		impactFraction = this->m_iBrush.GetImpactFraction();
		realFraction = this->m_iBrush.GetRealFraction();
	}

	GetCollision(node->m_children[SCENE_FACE_ORIENTATION_BACK], shape);

	this->m_iBrush.SetImpact(normalTemp, impactFraction, realFraction);

	GetCollision(node->m_children[SCENE_FACE_ORIENTATION_FRONT], shape);
}

/**
* Batch the geometry
* The goal here is to sort the visible geometry by the diffuse texture in order to minimize the draw calls
*/
void CSceneStatic::BatchGeometry()
{
	if(this->m_visibleFacesCounter > 0)
	{
		int batchCounter = 0;
		int bachIndex = 0;
		LPDIRECT3DTEXTURE9 tmp2;

		// Order array initialization
		for(int i = 0 ; i < this->m_visibleFacesCounter / VERTEX_PER_FACE ; i++)
			this->m_vertexOrder[i] = i;

		// Sort the data depending on the texture id
		this->m_textureSorter.QuickSort(this->m_visibleTexturesDiffuse, this->m_vertexOrder, 0, (this->m_visibleFacesCounter / VERTEX_PER_FACE) - 1);

		// Sort the other data depending on the order returned by the Quick Sort
		for(int i = 0 ; i < this->m_visibleFacesCounter ; i += VERTEX_PER_FACE)
		{
			for(int j = 0 ; j < VERTEX_PER_FACE ; j++)
			{
				this->m_batchedVisibleFaces[i + j] = &this->m_visibleFaces[j][this->m_vertexOrder[i / VERTEX_PER_FACE]];
				this->m_batchedVisibleTexturesDiffuse[i + j] = this->m_visibleTexturesDiffuse[i / VERTEX_PER_FACE];
				this->m_batchedVisibleTexturesNormal[i + j] = this->m_visibleTexturesNormal[this->m_vertexOrder[i / VERTEX_PER_FACE]];
			}
		}

		// Count the number of vertex for each texture
		tmp2 = this->m_batchedVisibleTexturesDiffuse[0];
		batchCounter = 1;
		for(int i = 1 ; i < this->m_visibleFacesCounter ; i++)
		{
			if(this->m_batchedVisibleTexturesDiffuse[i] == tmp2)
			{
				batchCounter++;
			}
			else
			{
				tmp2 = this->m_batchedVisibleTexturesDiffuse[i];
				this->m_batchedVisibleFacesNumber[bachIndex] = batchCounter;
				batchCounter = 1;
				bachIndex++;
			}
		}
		this->m_batchedVisibleFacesNumber[bachIndex] = batchCounter;

		// Copy the final geometry
		for(int i = 0 ; i < this->m_visibleFacesCounter ; i ++)
			memcpy(&this->m_finalGeometry[i], this->m_batchedVisibleFaces[i], sizeof(SVertexElement));
	}
}

/**
* Free the memory used by the buffers
*/
void CSceneStatic::FreeBuffers()
{
	SAFE_DELETE_ARRAY(this->m_vertexOrder);
	SAFE_DELETE_ARRAY(this->m_visibleLightFaces);
	SAFE_DELETE_ARRAY(this->m_visibleTexturesDiffuse);
	SAFE_DELETE_ARRAY(this->m_visibleTexturesNormal);
	SAFE_DELETE_ARRAY(this->m_batchedVisibleFacesNumber);
	SAFE_DELETE_ARRAY(this->m_batchedVisibleFaces);
	SAFE_DELETE_ARRAY(this->m_batchedVisibleTexturesDiffuse);
	SAFE_DELETE_ARRAY(this->m_batchedVisibleTexturesNormal);
	SAFE_DELETE_ARRAY(this->m_finalGeometry);

	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
		SAFE_DELETE_ARRAY(this->m_visibleFaces[i])
}

/**
* Log the graph in the log file
* @param node The current node
*/
void CSceneStatic::LogGraph(CSceneNode * const node)
{
	if(node == NULL)
		return;

	if(node->m_type == BSP_NODE)
		this->m_logM->GetStream() << "Node " << "\n";

	if(node->m_type == BSP_EMPTY_LEAF)
		this->m_logM->GetStream() << "Empty Leaf " << "\n";

	if(node->m_type == BSP_SOLID_LEAF)
		this->m_logM->GetStream() << "Solid Leaf " << "\n";

	this->m_logM->Write();
	
	if(node->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
	{
		this->m_logM->GetStream() << SCENE_FACE_ORIENTATION_FRONT << "\n";
		this->m_logM->Write();
		LogGraph(node->m_children[SCENE_FACE_ORIENTATION_FRONT]);
		this->m_logM->GetStream() << "Retour" << "\n";
		this->m_logM->Write();
	}

	if(node->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
	{
		this->m_logM->GetStream() << SCENE_FACE_ORIENTATION_BACK << "\n";
		this->m_logM->Write();
		LogGraph(node->m_children[SCENE_FACE_ORIENTATION_BACK]);
		this->m_logM->GetStream() << "Retour" << "\n";
		this->m_logM->Write();
	}
}

/**
* Load the environment geometry file and return a linked chain containing the faces
* It contains a convex space representing the general geometry of the level (walls, ceiling and ground)
* @param filename The environment geometry file name
* @return A linked chain compsed of CSceneFace elements
*/
CSceneFace * CSceneStatic::LoadEnvironmentGeometry(const std::string filename)
{
	std::ostringstream stream;
	std::string tmpString;
	bool visible = true;
	D3DXCOLOR color = 0xFF111111;
	D3DXVECTOR3 scale = D3DXVECTOR3(1,1,1);
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 vertex[VERTEX_PER_FACE];
	D3DXVECTOR2 texcoord[VERTEX_PER_FACE];
	D3DXMATRIX scaleMatrix, translationMatrix, rotationMatrix;
	LPDIRECT3DTEXTURE9 textureDiffuse;
	LPDIRECT3DTEXTURE9 textureNormal;
	CMesh * staticMesh = NULL;
	CSceneFace * pFace = NULL;
	CSceneFace * listFace = NULL;

	CTokenizer tokenizer(filename);
	if(tokenizer.m_tokens.size() > 0)
	{
		// File not empty
		tokenizer.Tokenize(" ");
		for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ; )
		{
			if(tokenizer.m_tokens.at(i) == "object")
			{
				// Start a new mesh
				stream.str("");
				stream << DIRECTORY_STATIC_MESH << tokenizer.m_tokens.at(i + 1);
				tmpString = stream.str();
				staticMesh = this->m_modM->LoadMesh(tmpString);

				// Default values
				scale = D3DXVECTOR3(1,1,1);
				rotation = D3DXVECTOR3(0,0,0);
				visible = true;

				i += 2;
			}
			else
			{
				if(tokenizer.m_tokens.at(i) == "textureD")
				{
					// Load the diffuse texture
					textureDiffuse = this->m_modM->LoadTexture(tokenizer.m_tokens.at(i + 1), TEXTURE_TYPE_MODEL);
					i += 2;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "textureN")
					{
						// Load the normal texture
						textureNormal = this->m_modM->LoadTexture(tokenizer.m_tokens.at(i + 1), TEXTURE_TYPE_MODEL);
						i += 2;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "position")
						{
							// Read the position values
							position = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
							i += 4;
						}
						else
						{
							if(tokenizer.m_tokens.at(i) == "rotation")
							{
								// Read the rotation values
								rotation = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
								i += 4;
							}
							else
							{
								if(tokenizer.m_tokens.at(i) == "scale")
								{
									// Read the scale values
									scale = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
									i += 4;
								}
								else
								{
									if(tokenizer.m_tokens.at(i) == "invisible")
									{
										// An invisible geometry
										visible = false;
										i++;
									}
									else
									{
										if(tokenizer.m_tokens.at(i) == "end")
										{
											// Continue only if the mesh is valid
											if(staticMesh->IsGeometryValid())
											{
												// If it's an invisible geometry, ignore the texture declarations
												if(!visible)
												{
													textureDiffuse = NULL;
													textureNormal = NULL;
												}

												// Add the mesh to the scene graph
												staticMesh->GeometrySourceCopy(SHADING_TYPE_FLAT);
												D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
												D3DXMatrixRotationYawPitchRoll(&rotationMatrix, D3DXToRadian(rotation.y), D3DXToRadian(rotation.x), D3DXToRadian(rotation.z));
												D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
												staticMesh->GeometryTransform(&scaleMatrix, false);
												staticMesh->GeometryTransform(&rotationMatrix, true);
												staticMesh->GeometryTransform(&translationMatrix, false);
												staticMesh->GeometryFinalize(NULL, true, SHADING_TYPE_FLAT);

												for(int j = 0 ; j < staticMesh->GetGeometrySize() / VERTEX_PER_FACE ; j++)
												{
													// Face creation
													pFace = new CSceneFace(&staticMesh->GetFaces()[j]);
													pFace->SetVisible(visible);
													pFace->SetTextureDiffuse(textureDiffuse);
													pFace->SetTextureNormal(textureNormal);

													// Add to the face list
													pFace->m_next = listFace;
													listFace = pFace;
												}
											}
											i++;
										}
										else
											i++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	// Return the list of the created faces
	return listFace;
}

/**
* Load the static geometry and return multiple linked chain of faces
* Each linked chain represent a 3D model and MUST be a convex shape
* @param filename The static geometry file name
* @param returnedSize The number of linked chain returned
*/
CSceneFace ** CSceneStatic::LoadStaticGeometry(const std::string filename, int * const returnedSize)
{
	*returnedSize = 0;
	int meshsCounter = 0;
	int meshId;
	int shadingType = SHADING_TYPE_FLAT;
	bool visible = true;
	std::ostringstream stream;
	std::string tmpString;
	D3DXCOLOR color = 0xFF111111;
	D3DXVECTOR3 scale = D3DXVECTOR3(1,1,1);
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 vertex[VERTEX_PER_FACE];
	D3DXVECTOR2 texcoord[VERTEX_PER_FACE];
	D3DXVECTOR3 normal[VERTEX_PER_FACE];
	D3DXMATRIX scaleMatrix, translationMatrix, rotationMatrix;
	LPDIRECT3DTEXTURE9 textureDiffuse;
	LPDIRECT3DTEXTURE9 textureNormal;
	CMesh * staticMesh = NULL;
	CSceneFace * pFace = NULL;
	CSceneFace * facesList = NULL;
	CSceneFace ** meshsList = NULL;

	CTokenizer tokenizer(filename);
	if(tokenizer.m_tokens.size() > 0)
	{
		// File not empty
		tokenizer.Tokenize(" ");
		*returnedSize = tokenizer.Count("object");
		if(*returnedSize > 0)
		{
			meshsList = new CSceneFace *[*returnedSize];
			for(int i = 0 ; i < *returnedSize ; i++)
				meshsList[i] = NULL;

			for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ; )
			{
				if(tokenizer.m_tokens.at(i) == "object")
				{
					// Start a new mesh
					stream.str("");
					stream << DIRECTORY_STATIC_MESH << tokenizer.m_tokens.at(i + 1);
					tmpString = stream.str();
					staticMesh = this->m_modM->LoadMesh(tmpString);
					
					// Default values
					scale = D3DXVECTOR3(1,1,1);
					rotation = D3DXVECTOR3(0,0,0);
					meshId = FACE_DEFAULT_ID;
					shadingType = SHADING_TYPE_FLAT;
					visible = true;

					i += 2;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "textureD")
					{
						// Load the diffuse texture
						textureDiffuse = this->m_modM->LoadTexture(tokenizer.m_tokens.at(i + 1), TEXTURE_TYPE_MODEL);
						i += 2;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "textureN")
						{
							// Load the normal texture
							textureNormal = this->m_modM->LoadTexture(tokenizer.m_tokens.at(i + 1), TEXTURE_TYPE_MODEL);
							i += 2;
						}
						else
						{
							if(tokenizer.m_tokens.at(i) == "position")
							{
								// Read the position values
								position = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
								i += 4;
							}
							else
							{
								if(tokenizer.m_tokens.at(i) == "rotation")
								{
									// Read the rotation values
									rotation = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
									i += 4;
								}
								else
								{
									if(tokenizer.m_tokens.at(i) == "scale")
									{
										// Read the scale values
										scale = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
										i += 4;
									}
									else
									{
										if(tokenizer.m_tokens.at(i) == "invisible")
										{
											// An invisible geometry
											visible = false;
											i++;
										}
										else
										{
											if(tokenizer.m_tokens.at(i) == "group")
											{
												meshId = atoi(tokenizer.m_tokens.at(i + 1).c_str());
												i += 2;
											}
											else
											{
												if(tokenizer.m_tokens.at(i) == "shading")
												{
													if(tokenizer.m_tokens.at(i + 1) == "smooth")
														shadingType = SHADING_TYPE_SMOOTH;
													i += 2;
												}
												else
												{
													if(tokenizer.m_tokens.at(i) == "end")
													{
														// Continue only if the mesh is valid
														if(staticMesh->IsGeometryValid())
														{
															// If it's an invisible geometry, ignore the texture declarations
															if(!visible)
															{
																textureDiffuse = NULL;
																textureNormal = NULL;
															}

															// Add the mesh to the scene graph
															staticMesh->GeometrySourceCopy(SHADING_TYPE_FLAT);
															D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
															D3DXMatrixRotationYawPitchRoll(&rotationMatrix, D3DXToRadian(rotation.y), D3DXToRadian(rotation.x), D3DXToRadian(rotation.z));
															D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
															staticMesh->GeometryTransform(&scaleMatrix, false);
															staticMesh->GeometryTransform(&rotationMatrix, true);
															staticMesh->GeometryTransform(&translationMatrix, false);
															staticMesh->GeometryFinalize(NULL, true, shadingType);

															for(int j = 0 ; j < staticMesh->GetGeometrySize() / VERTEX_PER_FACE ; j++)
															{
																// Face creation
																pFace = new CSceneFace(&staticMesh->GetFaces()[j]);
																pFace->SetVisible(visible);
																pFace->SetTextureDiffuse(textureDiffuse);
																pFace->SetTextureNormal(textureNormal);

																// Add to the face list
																pFace->m_next = facesList;
																facesList = pFace;
															}
															meshsList[meshsCounter] = facesList;
															facesList->m_id = meshId;
															meshsCounter++;
															facesList = NULL;
														}
														i++;
													}
													else
														i++;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	// Return the list of the created faces
	return meshsList;
}

