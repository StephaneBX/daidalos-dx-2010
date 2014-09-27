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
#include "../../Engine/Render/CScenePlane.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Physic/CShape.h"
#include "../../Engine/Physic/CImpact.h"
#include "../../Engine/Render/CSceneBSP3D.h"

/**
* Constructor
* Create an empty 3D bsp tree
*/
CSceneBSP3D::CSceneBSP3D()
{
	this->m_type = BSP_EMPTY_LEAF;
	this->m_facesSize = 0;
	this->m_plane = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_FRONT] = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_BACK] = NULL;
	this->m_facesList = NULL;
}

/**
* Constructor
* Create an empty 3D bsp tree with the default node defined as a 'type' type
* @param type The defaut node type
*/
CSceneBSP3D::CSceneBSP3D(const int type)
{
	this->m_type = type;
	this->m_facesSize = 0;
	this->m_plane = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_FRONT] = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_BACK] = NULL;
	this->m_facesList = NULL;
}

/**
* Constructor
* Create a bsp tree and fill the geometry in it
* @param faceList A list of CSceneFace elements used to create the bsp tree
*/
CSceneBSP3D::CSceneBSP3D(CSceneFace * const faceList)
{
	int facesSize = 0;
	this->m_type = BSP_EMPTY_LEAF;
	this->m_facesSize = 0;
	this->m_plane = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_FRONT] = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_BACK] = NULL;
	this->m_facesList = NULL;

	// Faces calculation
	CSceneFace * tmp = faceList;
	while(tmp != NULL)
	{
		facesSize++;
		tmp = tmp->m_next;
	}

	// Put the faces in a tab
	CSceneFace ** tab = new CSceneFace*[facesSize];
	int i = 0;
	tmp = faceList;
	while(tmp != NULL)
	{
		tab[i] = new CSceneFace(tmp);
		i++;
		tmp = tmp->m_next;
	}

	// Remove the list and add the faces to the tree
	for(i = 0 ; i < facesSize ; i++)
		AddFace(tab[i]);

	// delete the unused tab
	delete[] tab;
}

/**
* Destructor
*/
CSceneBSP3D::~CSceneBSP3D()
{
	SAFE_DELETE(this->m_plane);
	SAFE_DELETE(this->m_children[SCENE_FACE_ORIENTATION_FRONT]);
	SAFE_DELETE(this->m_children[SCENE_FACE_ORIENTATION_BACK]);

	if(this->m_facesList != NULL)
	{
		CSceneFace * tmp = this->m_facesList;
		CSceneFace * next = NULL;
		while(tmp != NULL)
		{
			next = tmp->m_next;
			delete tmp;
			tmp = next;
		}
	}
}

/**
* Add a face to the bsp
* @param face The face to add
*/
void CSceneBSP3D::AddFace(CSceneFace * const face)
{
	// A leaf
	if(this->m_type != BSP_NODE)
	{
		// Change to node type
		this->m_type = BSP_NODE;

		// Creation of the node plane
		this->m_plane = new CScenePlane(face->m_plane);

		// Put the face in the empty list
		this->m_facesList = face;

		// Increment the size
		this->m_facesSize = 1;

		// Create new leafs
		this->m_children[SCENE_FACE_ORIENTATION_FRONT] = new CSceneBSP3D(BSP_EMPTY_LEAF);
		this->m_children[SCENE_FACE_ORIENTATION_BACK] = new CSceneBSP3D(BSP_SOLID_LEAF);
	}
	else
	{
		// A node type

		// Classify the face in order to know what to do
		switch(Classify(face, this->m_plane))
		{
		case SCENE_FACE_ORIENTATION_COLPLANAR:
			// We need to test if the face is in the same orientation as the plane
			if(D3DXVec3Dot(&face->m_plane.m_normalVector, &this->m_plane->m_normalVector) >= 0)
			{
				// It's a match, we add the face to the node
				face->m_next = this->m_facesList;
				this->m_facesList = face;
				this->m_facesSize++;
			}
			else
			{
				// We need to visit the back child
				if(this->m_children[SCENE_FACE_ORIENTATION_FRONT] == NULL)
					this->m_children[SCENE_FACE_ORIENTATION_FRONT] = new CSceneBSP3D(BSP_SOLID_LEAF);
				this->m_children[SCENE_FACE_ORIENTATION_FRONT]->AddFace(face);
			}
			break;

		case SCENE_FACE_ORIENTATION_FRONT:
			// We need to visit the front child
			if(this->m_children[SCENE_FACE_ORIENTATION_FRONT] == NULL)
				this->m_children[SCENE_FACE_ORIENTATION_FRONT] = new CSceneBSP3D(BSP_EMPTY_LEAF);
			this->m_children[SCENE_FACE_ORIENTATION_FRONT]->AddFace(face);
			break;

		case SCENE_FACE_ORIENTATION_BACK:
			// We need to visit the back child
			if(this->m_children[SCENE_FACE_ORIENTATION_BACK] == NULL)
				this->m_children[SCENE_FACE_ORIENTATION_BACK] = new CSceneBSP3D(BSP_SOLID_LEAF);
			this->m_children[SCENE_FACE_ORIENTATION_BACK]->AddFace(face);
			break;

		case SCENE_FACE_ORIENTATION_SPANNING:
		default:
			// Need to split the face
			CSceneFace * frontFaces = NULL;
			CSceneFace * backFaces = NULL;
			int frontNumber = 0;
			int backNumber = 0;

			// Split the face when necessary
			SplitFace(face, this->m_plane, &frontFaces, &frontNumber, &backFaces, &backNumber);

			if(this->m_children[SCENE_FACE_ORIENTATION_FRONT] == NULL)
					this->m_children[SCENE_FACE_ORIENTATION_FRONT] = new CSceneBSP3D(BSP_EMPTY_LEAF);

			// One face in the front
			if(frontNumber == 1)
			{
				this->m_children[SCENE_FACE_ORIENTATION_FRONT]->AddFace(frontFaces);
			}
			else
			{
				// Two faces in the front, we need to unlink them
				if(frontNumber == 2)
				{
					CSceneFace * front1 = frontFaces;
					CSceneFace * front2 = frontFaces->m_next;
					front1->m_next = NULL;
					this->m_children[SCENE_FACE_ORIENTATION_FRONT]->AddFace(front1);
					this->m_children[SCENE_FACE_ORIENTATION_FRONT]->AddFace(front2);
				}
			}

			// One face in the back
			if(backNumber == 1)
			{
				this->m_children[SCENE_FACE_ORIENTATION_BACK]->AddFace(backFaces);
			}
			else
			{
				// Two faces in the back, we need to unlink them
				if(backNumber == 2)
				{
					CSceneFace * back1 = backFaces;
					CSceneFace * back2 = backFaces->m_next;
					back1->m_next = NULL;
					this->m_children[SCENE_FACE_ORIENTATION_BACK]->AddFace(back1);
					this->m_children[SCENE_FACE_ORIENTATION_BACK]->AddFace(back2);
				}
			}

			break;
		}
	}
}

/**
* Perform a collision detection between a shape and the bsp tree
* Save the collision result into the first impact
* @param shape The shape of the object that needs a collision test
* @param impact A pointer to the impact where the result will be save
* @param iBrush A pointer to the brush impact used to stock a temporary impact
*/
void CSceneBSP3D::CollisionDetection(CShape * const shape, CImpact * const impact, CImpact * const iBrush)
{
	float impactFraction, realFraction, diff;
	D3DXVECTOR3 * normalTemp;

	// Empty leaf
	if(this->m_type == BSP_EMPTY_LEAF)
	{
		iBrush->ClearImpact();
		return;
	}

	// Solid leaf
	if(this->m_type == BSP_SOLID_LEAF)
	{
		impact->SetNearerImpact(iBrush->GetPlaneNormal(), iBrush->GetImpactFraction(), iBrush->GetRealFraction());
		iBrush->ClearImpact();
		return;
	}

	// Node
	float d1 = this->m_plane->PointDistance(impact->GetStartPoint());
	float d2 = this->m_plane->PointDistance(impact->GetEndPoint());

	// Offset calculation
	float offset1 = shape->GetOffset(this->m_plane);

	// The object is in front
	if(d1 > offset1 && d2 > offset1)
	{
		this->m_children[SCENE_FACE_ORIENTATION_FRONT]->CollisionDetection(shape, impact, iBrush);
		return;
	}

	// Reverse the plane equation
	this->m_plane->Inverse();
	float offset2 = shape->GetOffset(this->m_plane);
	this->m_plane->Inverse();

	// The object is in the back
	if(d1 < -offset2 && d2 < -offset2)
	{
		this->m_children[SCENE_FACE_ORIENTATION_BACK]->CollisionDetection(shape, impact, iBrush);
		return;
	}

	// The object cross the plane
	if(d1 > offset1 && d2 <= offset1)
	{
		d1 -= offset1;
		d2 -= offset1;
		diff = 1.0f / (d1 - d2);
		realFraction = d1 * diff;

		if(realFraction >= impact->GetRealFraction())
			return;
		
		if(realFraction < iBrush->GetRealFraction()) 
		{
			// Ignore the collision if the vertical movement difference is smaller that MIN_HEIGHT and the movement is vertically positive
			//if(iBrush->GetDistance()->y * realFraction >= MIN_DISTANCE && iBrush->GetDistance()->y * realFraction <= MIN_HEIGHT)
				//return;

				normalTemp = &this->m_plane->m_normalVector;
				impactFraction = (d1 - MIN_DISTANCE) * diff;
				iBrush->SetImpact(normalTemp, impactFraction, realFraction);
		}
		else 
		{
			normalTemp = iBrush->GetPlaneNormal();
			impactFraction = iBrush->GetImpactFraction();
			realFraction = iBrush->GetRealFraction();
		}
	}
	else
	{
		normalTemp = iBrush->GetPlaneNormal();
		realFraction = iBrush->GetRealFraction();
		impactFraction = iBrush->GetImpactFraction();
	}

	this->m_children[SCENE_FACE_ORIENTATION_BACK]->CollisionDetection(shape, impact, iBrush);
	iBrush->SetImpact(normalTemp, impactFraction, realFraction);
	this->m_children[SCENE_FACE_ORIENTATION_FRONT]->CollisionDetection(shape, impact, iBrush);
}

/**
* Set the brushes draw status to false
*/
void CSceneBSP3D::ResetDraw()
{
	if(this->m_facesList != NULL)
	{
		CSceneFace * tmp = this->m_facesList;
		while(tmp != NULL)
		{
			tmp->m_drawn = false;
			tmp = tmp->m_next;
		}
	}

	if(this->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
		this->m_children[SCENE_FACE_ORIENTATION_FRONT]->ResetDraw();

	if(this->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
		this->m_children[SCENE_FACE_ORIENTATION_BACK]->ResetDraw();
}

/**
* Get the faces size
* @return The faces size
*/
const int CSceneBSP3D::GetFacesSize() const
{
	return this->m_facesSize;
}

/**
* Split a face
* This function can return one face (no split performed), two or three faces (split performed)
* @param face The face that will be split
* @param plane The plane used as a split reference
* @param frontFaces A linked list that contains faces in front of the plane
* @param numFrontFaces The number of front faces returned
* @param backFaces A linked list that contains faces on the back of the plane
* @param numBackFaces The number of back faces returned
*/
void CSceneBSP3D::SplitFace(CSceneFace * const face, CScenePlane * const plane, CSceneFace ** frontFaces, int * const numFrontFaces, CSceneFace ** backFaces, int * const numBackFaces)
{
	D3DXVECTOR3 pivot, positive1, positive2, negative1, negative2, intersection1, intersection2;
	D3DXVECTOR3 coordinates[VERTEX_PER_FACE];
	D3DXVECTOR3 normals[VERTEX_PER_FACE];
	D3DXVECTOR2 texcoord[VERTEX_PER_FACE];
	D3DXCOLOR splitColor = face->m_geometry[0].m_color;
	float result, kFactor1, kFactor2;
	int orientation[VERTEX_PER_FACE];
	int orientationCounter = 0;
	bool split;

	// Test the orientation of the face
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		result = plane->PointDistance(&face->m_geometry[i].m_position);
		if(result > EPSILON)
			orientationCounter++;
		else
			if(result < -EPSILON)
				orientationCounter--;
		orientation[i] = plane->ClassifyPoint(&face->m_geometry[i].m_position);
	}

	// Modification depending on the orientation
	switch(orientationCounter)
	{
	// All the points on the positive side, no need the split
	case 3:
		*frontFaces = face;
		*numFrontFaces = 1;
		*numBackFaces = 0;
		break;

	// All the points on the negative side, no need the split
	case -3:
		*backFaces = face;
		*numFrontFaces = 0;
		*numBackFaces = 1;
		break;

	// The points are on the positive side and on the plane, no need the split
	case 2:
		*frontFaces = face;
		*numFrontFaces = 1;
		*numBackFaces = 0;
		break;

	// The points are on the negative side and on the plane, no need the split
	case -2:
		*backFaces = face;
		*numFrontFaces = 0;
		*numBackFaces = 1;
		break;

	// One point is on the plane, the other two are on opposing side, need to split
	case 0:
		// Fill the variables depending on the orientation of the vertices
		if(orientation[0] == SCENE_VERTEX_ORIENTATION_ON_PLANE)
		{
			pivot = face->m_geometry[0].m_position;
			if(orientation[1] == SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE)
			{
				positive1 = face->m_geometry[1].m_position;
				negative1 = face->m_geometry[2].m_position;

				// Find the intersection point
				intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

				// Fill the left and right faces
				coordinates[0] = positive1;
				coordinates[1] = intersection1;
				coordinates[2] = pivot;
				texcoord[0] = face->m_geometry[1].m_texcoord;
				texcoord[1] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
				texcoord[2] = face->m_geometry[0].m_texcoord;
				normals[0] = face->m_geometry[1].m_normal;
				normals[1] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
				normals[2] = face->m_geometry[0].m_normal;
				*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
				*numFrontFaces = 1;

				coordinates[0] = negative1;
				coordinates[1] = pivot;
				coordinates[2] = intersection1;
				texcoord[0] = face->m_geometry[2].m_texcoord;
				texcoord[1] = face->m_geometry[0].m_texcoord;
				texcoord[2] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
				normals[0] = face->m_geometry[2].m_normal;
				normals[1] = face->m_geometry[0].m_normal;
				normals[2] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
				*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
				*numBackFaces = 1;
			}
			else
			{
				positive1 = face->m_geometry[2].m_position;
				negative1 = face->m_geometry[1].m_position;

				// Find the intersection point
				intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

				// Fill the left and right faces
				coordinates[0] = positive1;
				coordinates[1] = pivot;
				coordinates[2] = intersection1;
				texcoord[0] = face->m_geometry[2].m_texcoord;
				texcoord[1] = face->m_geometry[0].m_texcoord;
				texcoord[2] = face->m_geometry[2].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
				normals[0] = face->m_geometry[2].m_normal;
				normals[1] = face->m_geometry[0].m_normal;
				normals[2] = face->m_geometry[2].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
				*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
				*numFrontFaces = 1;

				coordinates[0] = negative1;
				coordinates[1] = intersection1;
				coordinates[2] = pivot;
				texcoord[0] = face->m_geometry[1].m_texcoord;
				texcoord[1] = face->m_geometry[2].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
				texcoord[2] = face->m_geometry[0].m_texcoord;
				normals[0] = face->m_geometry[1].m_normal;
				normals[1] = face->m_geometry[2].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
				normals[2] = face->m_geometry[0].m_normal;
				*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
				*numBackFaces = 1;
			}
		}
		else
		{
			if(orientation[1] == SCENE_VERTEX_ORIENTATION_ON_PLANE)
			{
				pivot = face->m_geometry[1].m_position;
				if(orientation[0] == SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE)
				{
					positive1 = face->m_geometry[0].m_position;
					negative1 = face->m_geometry[2].m_position;

					// Find the intersection point
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

					// Fill the left and right faces
					coordinates[0] = positive1;
					coordinates[1] = pivot;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[0].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord;
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[0].m_normal;
					normals[1] = face->m_geometry[1].m_normal;
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numFrontFaces = 1;

					coordinates[0] = negative1;
					coordinates[1] = intersection1;
					coordinates[2] = pivot;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[1].m_texcoord;
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[1].m_normal;
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numBackFaces = 1;
				}
				else
				{
					positive1 = face->m_geometry[2].m_position;
					negative1 = face->m_geometry[0].m_position;

					// Find the intersection point
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

					// Fill the left and right faces
					coordinates[0] = positive1;
					coordinates[1] = intersection1;
					coordinates[2] = pivot;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[2].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
					texcoord[2] = face->m_geometry[1].m_texcoord;
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[2].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
					normals[2] = face->m_geometry[1].m_normal;
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numFrontFaces = 1;

					coordinates[0] = negative1;
					coordinates[1] = pivot;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[0].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord;
					texcoord[2] = face->m_geometry[2].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
					normals[0] = face->m_geometry[0].m_normal;
					normals[1] = face->m_geometry[1].m_normal;
					normals[2] = face->m_geometry[2].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numBackFaces = 1;
				}
			}
			else
			{
				pivot = face->m_geometry[2].m_position;
				if(orientation[0] == SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE)
				{
					positive1 = face->m_geometry[0].m_position;
					negative1 = face->m_geometry[1].m_position;

					// Find the intersection point
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

					// Fill the left and right faces
					coordinates[0] = positive1;
					coordinates[1] = intersection1;
					coordinates[2] = pivot;
					texcoord[0] = face->m_geometry[0].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[2].m_texcoord;
					normals[0] = face->m_geometry[0].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[2].m_normal;
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numFrontFaces = 1;

					coordinates[0] = negative1;
					coordinates[1] = pivot;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[2].m_texcoord;
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[2].m_normal;
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numBackFaces = 1;
				}
				else
				{
					positive1 = face->m_geometry[1].m_position;
					negative1 = face->m_geometry[0].m_position;

					// Find the intersection point
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);

					// Fill the left and right faces
					coordinates[0] = positive1;
					coordinates[1] = pivot;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[2].m_texcoord;
					texcoord[2] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[2].m_normal;
					normals[2] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numFrontFaces = 1;

					coordinates[0] = negative1;
					coordinates[1] = intersection1;
					coordinates[2] = pivot;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
					texcoord[2] = face->m_geometry[2].m_texcoord;
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
					normals[2] = face->m_geometry[2].m_normal;
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);
					*numBackFaces = 1;
				}
			}
		}
		break;

	// Two points are on the positive side, one point is on the negative side, need to split
	// Or two points are on the plane and one point is on the positive side, no need to split
	case 1:
		split = true;
		// Test the presence of a point on the plane
		for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
			if(orientation[i] == SCENE_VERTEX_ORIENTATION_ON_PLANE)
				split = false;

		if(split == true)
		{
			// First case

			// The first vertex is the negative one
			if(orientation[0] == SCENE_VERTEX_ORIENTATION_NEGATIVE_SIDE)
			{
				negative1 = face->m_geometry[0].m_position;
				positive1 = face->m_geometry[1].m_position;
				positive2 = face->m_geometry[2].m_position;

				// Find the intersections points
				intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);
				intersection2 = plane->IntersectSegment(&positive2, &negative1, &kFactor2);

				// Fill the left faces
				coordinates[0] = positive1;
				coordinates[1] = intersection2;
				coordinates[2] = intersection1;
				texcoord[0] = face->m_geometry[1].m_texcoord;
				texcoord[1] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				texcoord[2] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
				normals[0] = face->m_geometry[1].m_normal;
				normals[1] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				normals[2] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
				*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				coordinates[0] = positive1;
				coordinates[1] = positive2;
				coordinates[2] = intersection2;
				texcoord[0] = face->m_geometry[1].m_texcoord;
				texcoord[1] = face->m_geometry[2].m_texcoord;
				texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				normals[0] = face->m_geometry[1].m_normal;
				normals[1] = face->m_geometry[2].m_normal;
				normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				(*frontFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				*numFrontFaces = 2;

				// Fill the right face
				coordinates[0] = negative1;
				coordinates[1] = intersection1;
				coordinates[2] = intersection2;
				texcoord[0] = face->m_geometry[0].m_texcoord;
				texcoord[1] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
				texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				normals[0] = face->m_geometry[0].m_normal;
				normals[1] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
				normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				*numBackFaces = 1;
			}
			else
			{
				// The second vertex is the negative one
				if(orientation[1] == SCENE_VERTEX_ORIENTATION_NEGATIVE_SIDE)
				{
					negative1 = face->m_geometry[1].m_position;
					positive1 = face->m_geometry[0].m_position;
					positive2 = face->m_geometry[2].m_position;

					// Find the intersections points
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);
					intersection2 = plane->IntersectSegment(&positive2, &negative1, &kFactor2);

					// Fill the left faces
					coordinates[0] = positive1;
					coordinates[1] = intersection1;
					coordinates[2] = intersection2;
					texcoord[0] = face->m_geometry[0].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
					normals[0] = face->m_geometry[0].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					coordinates[0] = positive2;
					coordinates[1] = positive1;
					coordinates[2] = intersection2;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord;
					texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal;
					normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
					(*frontFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numFrontFaces = 2;

					// Fill the right face
					coordinates[0] = negative1;
					coordinates[1] = intersection2;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numBackFaces = 1;
				}
				else
				{
					// The third vertex is the negative one
					negative1 = face->m_geometry[2].m_position;
					positive1 = face->m_geometry[0].m_position;
					positive2 = face->m_geometry[1].m_position;

					// Find the intersections points
					intersection1 = plane->IntersectSegment(&positive1, &negative1, &kFactor1);
					intersection2 = plane->IntersectSegment(&positive2, &negative1, &kFactor2);

					// Fill the left faces
					coordinates[0] = positive1;
					coordinates[1] = intersection2;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[0].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord + kFactor2 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[0].m_normal;
					normals[1] = face->m_geometry[1].m_normal + kFactor2 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					coordinates[0] = positive2;
					coordinates[1] = intersection2;
					coordinates[2] = positive1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord + kFactor2 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord;
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[1].m_normal + kFactor2 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
					normals[2] = face->m_geometry[0].m_normal;
					(*frontFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numFrontFaces = 2;

					// Fill the right face
					coordinates[0] = negative1;
					coordinates[1] = intersection1;
					coordinates[2] = intersection2;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[1].m_texcoord + kFactor2 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[1].m_normal + kFactor2 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numBackFaces = 1;
				}
			}
		}
		else
		{
			// Second case
			*frontFaces = face;
			*numFrontFaces = 1;
			*numBackFaces = 0;
		}
		break;

	// One point is on the positive side, two points are on the negative side, need to split
	// Or two points are on the plane and one point is on the negative side, no need to split
	case -1:
		split = true;
		// Test the presence of a point on the plane
		for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
			if(orientation[i] == SCENE_VERTEX_ORIENTATION_ON_PLANE)
				split = false;

		if(split == true)
		{
			// First case

			// The first vertex is the positive one
			if(orientation[0] == SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE)
			{
				positive1 = face->m_geometry[0].m_position;
				negative1 = face->m_geometry[1].m_position;
				negative2 = face->m_geometry[2].m_position;

				// Find the intersections points
				intersection1 = plane->IntersectSegment(&negative1, &positive1, &kFactor1);
				intersection2 = plane->IntersectSegment(&negative2, &positive1, &kFactor2);

				// Fill the left face
				coordinates[0] = positive1;
				coordinates[1] = intersection1;
				coordinates[2] = intersection2;
				texcoord[0] = face->m_geometry[0].m_texcoord;
				texcoord[1] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
				texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				normals[0] = face->m_geometry[0].m_normal;
				normals[1] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
				normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				*numFrontFaces = 1;

				// Fill the right faces
				coordinates[0] = negative1;
				coordinates[1] = intersection2;
				coordinates[2] = intersection1;
				texcoord[0] = face->m_geometry[1].m_texcoord;
				texcoord[1] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				texcoord[2] = face->m_geometry[1].m_texcoord + kFactor1 * (face->m_geometry[0].m_texcoord - face->m_geometry[1].m_texcoord);
				normals[0] = face->m_geometry[1].m_normal;
				normals[1] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				normals[2] = face->m_geometry[1].m_normal + kFactor1 * (face->m_geometry[0].m_normal - face->m_geometry[1].m_normal);
				*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				coordinates[0] = negative2;
				coordinates[1] = intersection2;
				coordinates[2] = negative1;
				texcoord[0] = face->m_geometry[2].m_texcoord;
				texcoord[1] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[0].m_texcoord - face->m_geometry[2].m_texcoord);
				texcoord[2] = face->m_geometry[1].m_texcoord;
				normals[0] = face->m_geometry[2].m_normal;
				normals[1] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[0].m_normal - face->m_geometry[2].m_normal);
				normals[2] = face->m_geometry[1].m_normal;
				(*backFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

				*numBackFaces = 2;
			}
			else
			{
				// The second vertex is the positive one
				if(orientation[1] == SCENE_VERTEX_ORIENTATION_POSITIVE_SIDE)
				{
					positive1 = face->m_geometry[1].m_position;
					negative1 = face->m_geometry[0].m_position;
					negative2 = face->m_geometry[2].m_position;

					// Find the intersections points
					intersection1 = plane->IntersectSegment(&negative1, &positive1, &kFactor1);
					intersection2 = plane->IntersectSegment(&negative2, &positive1, &kFactor2);

					// Fill the left face
					coordinates[0] = positive1;
					coordinates[1] = intersection2;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numFrontFaces = 1;

					// Fill the right faces
					coordinates[0] = negative2;
					coordinates[1] = intersection1;
					coordinates[2] = intersection2;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[2].m_texcoord + kFactor2 * (face->m_geometry[1].m_texcoord - face->m_geometry[2].m_texcoord);
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[2].m_normal + kFactor2 * (face->m_geometry[1].m_normal - face->m_geometry[2].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					coordinates[0] = negative2;
					coordinates[1] = negative1;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord;
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[1].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal;
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[1].m_normal - face->m_geometry[0].m_normal);
					(*backFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numBackFaces = 2;
				}
				else
				{
					// The third vertex is the positive one
					positive1 = face->m_geometry[2].m_position;
					negative1 = face->m_geometry[0].m_position;
					negative2 = face->m_geometry[1].m_position;

					// Find the intersections points
					intersection1 = plane->IntersectSegment(&negative1, &positive1, &kFactor1);
					intersection2 = plane->IntersectSegment(&negative2, &positive1, &kFactor2);

					// Fill the left face
					coordinates[0] = positive1;
					coordinates[1] = intersection1;
					coordinates[2] = intersection2;
					texcoord[0] = face->m_geometry[2].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[1].m_texcoord + kFactor2 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
					normals[0] = face->m_geometry[2].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[1].m_normal + kFactor2 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
					*frontFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numFrontFaces = 1;

					// Fill the right faces
					coordinates[0] = negative2;
					coordinates[1] = intersection2;
					coordinates[2] = intersection1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[1].m_texcoord + kFactor2 * (face->m_geometry[2].m_texcoord - face->m_geometry[1].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[1].m_normal + kFactor2 * (face->m_geometry[2].m_normal - face->m_geometry[1].m_normal);
					normals[2] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					*backFaces = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					coordinates[0] = negative2;
					coordinates[1] = intersection1;
					coordinates[2] = negative1;
					texcoord[0] = face->m_geometry[1].m_texcoord;
					texcoord[1] = face->m_geometry[0].m_texcoord + kFactor1 * (face->m_geometry[2].m_texcoord - face->m_geometry[0].m_texcoord);
					texcoord[2] = face->m_geometry[0].m_texcoord;
					normals[0] = face->m_geometry[1].m_normal;
					normals[1] = face->m_geometry[0].m_normal + kFactor1 * (face->m_geometry[2].m_normal - face->m_geometry[0].m_normal);
					normals[2] = face->m_geometry[0].m_normal;
					(*backFaces)->m_next = new CSceneFace(coordinates, normals, &splitColor, texcoord, face->m_textureDiffuse, face->m_textureNormal, &face->m_visible);

					*numBackFaces = 2;
				}
			}
		}
		else
		{
			// Second case
			*backFaces = face;
			*numFrontFaces = 0;
			*numBackFaces = 1;
		}
		break;
	}
	if(*numFrontFaces + *numBackFaces > 1)
		delete face;
}

/**
* Classify a face regarding a plane
* @param face The face to classify
* @param plane The plane used as reference
* @return The classify result, that can be SCENE_FACE_ORIENTATION_FRONT, SCENE_FACE_ORIENTATION_BACK, SCENE_FACE_ORIENTATION_COLPLANAR or SCENE_FACE_ORIENTATION_SPANNING
*/
const int CSceneBSP3D::Classify(CSceneFace * const face, CScenePlane * const plane)
{
	float result;
	int frontCounter = 0;
	int backCounter = 0;
	int onCounter = 0;

	// Test all the points
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		result = plane->PointDistance(&face->m_geometry[i].m_position);
		if(result > EPSILON)
		{
			frontCounter++;
		}
		else
		{
			if(result < -EPSILON)
			{
				backCounter++;
			}
			else
			{
				frontCounter++;
				backCounter++;
				onCounter++;
			}
		}
	}

	// Return the appropriate result
	if(onCounter == VERTEX_PER_FACE)
	{
		return SCENE_FACE_ORIENTATION_COLPLANAR;
	}
	else
	{
		if(frontCounter == VERTEX_PER_FACE)
		{
			return SCENE_FACE_ORIENTATION_FRONT;
		}
		else
		{
			if(backCounter == VERTEX_PER_FACE)
				return SCENE_FACE_ORIENTATION_BACK;
			else
				return SCENE_FACE_ORIENTATION_SPANNING;
		}
	}
}

/**
* Find the best faces to be used as a splitter
* @param faceList The list of faces to test
* @return The face choosen to be a splitter
*/
CSceneFace * CSceneBSP3D::FindBestSplitter(CSceneFace * const faceList)
{
	CSceneFace * runningFace1;
	CSceneFace * runningFace2;
	CSceneFace * bestChoice = NULL;
	float bspScore = DEFAULT_BSP_SCORE;
	float score;
	int faceOrientation;

	// No splitter if the list is empty
	if(faceList == NULL)
		return NULL;

	// First path
	for(runningFace1 = faceList ; runningFace1 != NULL ; runningFace1 = runningFace1->m_next)
	{
		int frontCounter = 0;
		int backCounter = 0;
		int coplanarCounter = 0;
		int splitCounter = 0;

		if(!runningFace1->m_separationPlane)
		{
			// Second path
			for(runningFace2 = faceList ; runningFace2 != NULL ; runningFace2 = runningFace2->m_next)
			{
				// If the faces are different and the second face not already used
				if(!runningFace2->m_separationPlane && runningFace1 != runningFace2)
				{
					// Perform an orientation test
					faceOrientation = Classify(runningFace2, &runningFace1->m_plane);
					switch(faceOrientation)
					{
					case SCENE_FACE_ORIENTATION_FRONT:
						frontCounter++;
						break;

					case SCENE_FACE_ORIENTATION_BACK:
						backCounter++;
						break;
						
					case SCENE_FACE_ORIENTATION_COLPLANAR:
						coplanarCounter++;
						break;

					default:
						splitCounter++;
						break;
					}
				}
			}
			score = (float)(2 * splitCounter + abs(frontCounter - backCounter) + coplanarCounter);

			// Update the best face if necessary
			if(score > 0 && score < bspScore)
			{
				bspScore = score;
				bestChoice = runningFace1;
			}
		}
	}

	return bestChoice;
}
