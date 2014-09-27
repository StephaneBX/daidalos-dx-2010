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

#include "../../Engine/Physic/CImpact.h"
#include "../../Engine/Render/CSceneBrush.h"
#include "../../Engine/Render/CSceneNode.h"
#include "../../Engine/Render/CSceneBSP3D.h"
#include "../../Engine/Render/CSceneFace.h"
#include "../../Engine/Render/CSceneBSPTree.h"

/**
* Constructor
* @param meshs A list of faces ordianized by meshs
* @param meshsSize The number of meshs
*/
CSceneBSPTree::CSceneBSPTree(CSceneFace ** const meshs, const int meshsSize)
{
	CSceneFace * tmp = NULL;

	// Brush list creation
	CSceneBrush ** tmpBrushesList = new CSceneBrush*[meshsSize];
	for(int i = 0 ; i < meshsSize ; i++)
		tmpBrushesList[i] = new CSceneBrush(meshs[i]);

	// Bsp initialization
	CSceneBSP3D * bsp3D = new CSceneBSP3D();
	this->m_root = new CSceneNode();

	// We add the faces to the CSceneBSP3D
	for(int i = 0 ; i < meshsSize ; i++)
	{
		tmp = meshs[i];
		while(tmp != NULL)
		{
			bsp3D->AddFace(new CSceneFace(tmp));
			tmp = tmp->m_next;
		}
	}

	// Create the empty collision bsp
	BuildNodes(bsp3D, this->m_root);

	// Add the brushes to the nodes
	for(int i = 0 ; i < meshsSize ; i++)
		AddBrush(this->m_root, tmpBrushesList[i], meshs[i]);

	// Copy the brushes list
	this->m_brushesList = tmpBrushesList;
	this->m_brushesSize = meshsSize;

	// Delete the meshs
	for(int i = 0 ; i < meshsSize ; i++)
	{
		tmp = meshs[i];
		CSceneFace * tmp2 = NULL;
		while(tmp != NULL)
		{
			tmp2 = tmp->m_next;
			delete tmp;
			tmp = tmp2;
		}
	}

	// We don't need it anymore
	delete bsp3D;
}

/**
* Destructor
*/
CSceneBSPTree::~CSceneBSPTree()
{
	if(this->m_brushesList != NULL)
	{
		for(int i = 0 ; i < this->m_brushesSize ; i++)
			SAFE_DELETE(this->m_brushesList[i]);
		SAFE_DELETE_ARRAY(this->m_brushesList);
	}

	SAFE_DELETE(this->m_root);
}

/**
* Perform a collision detection between a shape and the bsp tree
* Save the collision result into the first impact
* @param shape The shape of the object that needs a collision test
* @param impact A pointer to the impact where the result will be save
* @param iBrush A pointer to the brush impact used to stock a temporary impact
*/
void CSceneBSPTree::CollisionDetection(CShape * const shape, CImpact * const impact, CImpact * const iBrush)
{
	for(int i = 0 ; i < this->m_brushesSize ; i++)
		this->m_brushesList[i]->m_collisionTested = false;

	this->m_root->CollisionDetection(shape, impact, iBrush);
}

/**
* Change the collision status of a set of brushes
*/
void CSceneBSPTree::SetBrushCollision(const int brushesGroupId, const bool value)
{
	for(int i = 0 ; i < m_brushesSize ; i++)
	{
		if(this->m_brushesList[i]->m_groupId == brushesGroupId)
			this->m_brushesList[i]->m_collisionEnabled = value;
	}
}

/**
* Build the nodes by copying the architecture of a CSceneBSP3D bsp tree
* @param bsp The CSceneBSP3D bsp tree to be used as reference
* @param node The current node
*/
void CSceneBSPTree::BuildNodes(CSceneBSP3D * const bsp, CSceneNode * const node)
{
	// Create the back node if it not already existed
	if(bsp->m_children[SCENE_FACE_ORIENTATION_BACK] != NULL)
	{
		if(node->m_children[SCENE_FACE_ORIENTATION_BACK] == NULL)
			node->m_children[SCENE_FACE_ORIENTATION_BACK] = new CSceneNode(BSP_SOLID_LEAF);
		BuildNodes(bsp->m_children[SCENE_FACE_ORIENTATION_BACK], node->m_children[SCENE_FACE_ORIENTATION_BACK]);
	}

	// Copy the node information
	if(bsp->m_type == BSP_NODE)
	{
		node->m_type = bsp->m_type;
		node->m_plane = new CScenePlane(bsp->m_plane);
	}

	// Create the front node if it not already existed
	if(bsp->m_children[SCENE_FACE_ORIENTATION_FRONT] != NULL)
	{
		if(node->m_children[SCENE_FACE_ORIENTATION_FRONT] == NULL)
			node->m_children[SCENE_FACE_ORIENTATION_FRONT] = new CSceneNode(BSP_EMPTY_LEAF);
		BuildNodes(bsp->m_children[SCENE_FACE_ORIENTATION_FRONT], node->m_children[SCENE_FACE_ORIENTATION_FRONT]);
	}
}

/**
* Add a brush to the bsp tree
* Visit the nodes of the bsp and add the brush in the list of any nodes where the plane equation matches one of the faces equation of the brush
* @param node The current node
* @param brush The brush to add
* @param mesh The mesh associated to the brush
*/
void CSceneBSPTree::AddBrush(CSceneNode * const node, CSceneBrush * const brush, CSceneFace * const mesh)
{
	// There is brushes only on nodes
	if(node->m_type != BSP_NODE)
		return;

	CSceneFace * tmp = mesh;
	bool front = false;
	bool back = false;
	bool coplanar = false;
	int faceOrientation;

	// Search for faces orientation information
	while(tmp != NULL)
	{
		faceOrientation = node->Classify(tmp);
		if(faceOrientation == SCENE_FACE_ORIENTATION_FRONT || faceOrientation == SCENE_FACE_ORIENTATION_SPANNING)
			front = true;
		if(faceOrientation == SCENE_FACE_ORIENTATION_BACK || faceOrientation == SCENE_FACE_ORIENTATION_SPANNING)
			back = true;
		if(faceOrientation == SCENE_FACE_ORIENTATION_COLPLANAR)
			coplanar = true;
		tmp = tmp->m_next;
	}

	// Brushes are put only if at least a face is coplanar to the plane
	if(coplanar)
	{
		bool exists = false;
		for(int i = 0 ; i < node->m_brushesSize && !exists ; i++)
			if(node->m_brushesList[i] == brush)
				exists = true;

		// We add the brushes only if it not already exist in the node
		if(!exists)
		{
			CSceneBrush ** tmpBrushes = new CSceneBrush*[node->m_brushesSize + 1];
			for(int i = 0 ; i < node->m_brushesSize ; i++)
				tmpBrushes[i] = node->m_brushesList[i];
			tmpBrushes[node->m_brushesSize] = brush;
			node->m_brushesSize++;
			delete[] node->m_brushesList;
			node->m_brushesList = tmpBrushes; 
		}
	}

	// Visit the childs if necessary
	if(front)
		AddBrush(node->m_children[SCENE_FACE_ORIENTATION_FRONT], brush, mesh);
	if(back)
		AddBrush(node->m_children[SCENE_FACE_ORIENTATION_BACK], brush, mesh);
}
