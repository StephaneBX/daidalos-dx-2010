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

#ifndef __CMESHMD2_H
#define __CMESHMD2_H

#define MD2_ANIMATION_FPS 10.0f

#include "../../Engine/Loaders/CMesh.h"
#include "../../Engine/Loaders/CMD2Loader.h"

/**
* CMeshOBJ class.
* Manages the MD2 format.
*/
class CMeshMD2 : public CMesh
{
private:
	CMD2Loader m_loader;

public:
	CMeshMD2(const std::string fileName);
	~CMeshMD2();
	void Animate(const float frame);
	void AnimateSourceCopy(const float frame, const int shadingType);
	void GeometrySourceCopy(const int shadingType);
	void GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals);
	void GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType);
	void UseCustomMaterial(std::string filename);

private:
	void Load(const std::string fileName);
};

#endif