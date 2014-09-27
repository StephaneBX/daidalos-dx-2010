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

#include "../../Engine/Render/CScenePlane.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CSceneBrush.h"
#include "../../Engine/Render/CSceneBSP3D.h"
#include "../../Engine/Physic/CShape.h"
#include "../../Engine/Physic/CImpact.h"
#include "../../Engine/Render/CSceneNode.h"

/**
* Constructor
* Create an empty leaf node
*/
CSceneNode::CSceneNode()
{
	this->m_type = BSP_EMPTY_LEAF;
	this->m_brushesSize = 0;
	this->m_plane = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_FRONT] = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_BACK] = NULL;
	this->m_brushesList = NULL;
}

/**
* Constructor
* Create a 'type' type node
* @param type 
*/
CSceneNode::CSceneNode(const int type)
{
	this->m_type = type;
	this->m_brushesSize = 0;
	this->m_plane = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_FRONT] = NULL;
	this->m_children[SCENE_FACE_ORIENTATION_BACK] = NULL;
	this->m_brushesList = NULL;
}

/**
* Destructor
*/
CSceneNode::~CSceneNode()
{
	SAFE_DELETE(this->m_plane);
	SAFE_DELETE(this->m_children[SCENE_FACE_ORIENTATION_FRONT]);
	SAFE_DELETE(this->m_children[SCENE_FACE_ORIENTATION_BACK]);
	SAFE_DELETE_ARRAY(this->m_brushesList);
}

/**
* Perform a collision detection between a shape and the node
* Save the collision result into the first impact
* @param shape The shape of the object that needs a collision test
* @param impact A pointer to the impact where the result will be save
* @param iBrush A pointer to the brush impact used to stock a temporary impact
*/
void CSceneNode::CollisionDetection(CShape * const shape, CImpact * const impact, CImpact * const iBrush)
{
	if(this->m_type != BSP_NODE)
		return;

	// Node
	float d1 = this->m_plane->PointDistance(impact->GetStartPoint());
	float d2 = this->m_plane->PointDistance(impact->GetEndPoint());

	// Offset calculation
	float offset1 = shape->GetOffset(this->m_plane);

	// The shape if in front of the plane
	if(d1 > offset1 && d2 > offset1)
	{
		this->m_children[SCENE_FACE_ORIENTATION_FRONT]->CollisionDetection(shape, impact, iBrush);
		return;
	}

	this->m_plane->Inverse();
	float offset2 = shape->GetOffset(this->m_plane);
	this->m_plane->Inverse();

	// The shape is behind the plane
	if(d1 < -offset2 && d2 < -offset2)
	{
		this->m_children[SCENE_FACE_ORIENTATION_BACK]->CollisionDetection(shape, impact, iBrush);
		return;
	}

	// The shape cross the plane, we test the brushes if there is any
	if(d1 > offset1 && d2 <= offset2)
	{
		for(int i = 0 ; i < this->m_brushesSize ; i++)
			if(this->m_brushesList[i]->m_collisionEnabled)
				this->m_brushesList[i]->m_bsp->CollisionDetection(shape, impact, iBrush);
	}

	// Visit in the right order
	if(d1 > d2)
	{
		this->m_children[SCENE_FACE_ORIENTATION_FRONT]->CollisionDetection(shape, impact, iBrush);
		this->m_children[SCENE_FACE_ORIENTATION_BACK]->CollisionDetection(shape, impact, iBrush);
	}
	else
	{
		this->m_children[SCENE_FACE_ORIENTATION_BACK]->CollisionDetection(shape, impact, iBrush);
		this->m_children[SCENE_FACE_ORIENTATION_FRONT]->CollisionDetection(shape, impact, iBrush);
	}

}

/**
* Classify a face regarding a plane
* @param face The face to classify
* @return The classify result, that can be SCENE_FACE_ORIENTATION_FRONT, SCENE_FACE_ORIENTATION_BACK, SCENE_FACE_ORIENTATION_COLPLANAR or SCENE_FACE_ORIENTATION_SPANNING
*/
const int CSceneNode::Classify(CSceneFace * const face) const
{
	float result;
	int frontCounter = 0;
	int backCounter = 0;
	int onCounter = 0;

	// Test all the points
	for(int i = 0 ; i < VERTEX_PER_FACE ; i++)
	{
		result = this->m_plane->PointDistance(&face->m_geometry[i].m_position);
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
