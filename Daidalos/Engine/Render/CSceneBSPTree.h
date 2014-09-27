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

#ifndef __CSCENEBSPTREE_H
#define __CSCENEBSPTREE_H

class CImpact;
class CSceneBrush;
class CSceneNode;

/**
* CSceneBSPTree class.
* Manages a bsp tree meant for physic calculation.
* It represent the world in an arborecent way and its general use is for collision detection.
* A big thanks to NICS for this tutorial (in french): http://nics.developpez.com/tutoriels/3D/physique/glissement-forme-contre-bsp/
*/
class CSceneBSPTree
{
private:
	int m_brushesSize;
	CSceneBrush ** m_brushesList;
	CSceneNode * m_root;
	
public:
	CSceneBSPTree(CSceneFace ** const meshs, const int meshsSize);
	~CSceneBSPTree();
	void CollisionDetection(CShape * const shape, CImpact * const impact, CImpact * const iBrush);
	void SetBrushCollision(const int brushesId, const bool value);

private:
	void BuildNodes(CSceneBSP3D * const bsp, CSceneNode * const node);
	void AddBrush(CSceneNode * const node, CSceneBrush * const brush, CSceneFace * const mesh);
};

#endif
