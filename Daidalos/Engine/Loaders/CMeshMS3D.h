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

#ifndef __CMESHMS3D_H
#define __CMESHMS3D_H

#include "../../Engine/Loaders/CMesh.h"
#include "../../Engine/Loaders/CMS3DLoader.h"

/**
* CMeshMS3D class.
* Manages the Milkshape 3D format.
*/
class CMeshMS3D : public CMesh
{
private:
	CMS3DLoader m_loader;

public:
	CMeshMS3D(const std::string fileName);
	~CMeshMS3D();
	void Animate(const float frame);
	void AnimateSourceCopy(const float frame, const int shadingType);
	void GeometrySourceCopy(const int shadingType);
	void GeometryTransform(const D3DXMATRIX * const matrix, const bool updateNormals);
	void GeometryFinalize(CSceneCamera * const camera, const bool visible, const int shadingType);

private:
	void Load(const std::string fileName);
};

#endif