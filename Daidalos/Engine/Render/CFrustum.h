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

#ifndef __CFRUSTUM_H
#define __CFRUSTUM_H

#define VIEW_FADE_OUT 35.0f

class CSceneFace;
class CShape;

enum frustumPoint
{
	FRUSTUM_POINT_NEAR_TOP_LEFT,
	FRUSTUM_POINT_NEAR_TOP_RIGHT,
	FRUSTUM_POINT_NEAR_BOTTOM_LEFT,
	FRUSTUM_POINT_NEAR_BOTTOM_RIGHT,
	FRUSTUM_POINT_FAR_TOP_LEFT,
	FRUSTUM_POINT_FAR_TOP_RIGHT,
	FRUSTUM_POINT_FAR_BOTTOM_LEFT,
	FRUSTUM_POINT_FAR_BOTTOM_RIGHT,
	FRUSTUM_POINT_SIZE
};

enum frustumPlane
{
	FRUSTUM_PLANE_LEFT,
	FRUSTUM_PLANE_RIGHT,
	FRUSTUM_PLANE_TOP,
	FRUSTUM_PLANE_BOTTOM,
	FRUSTUM_PLANE_NEAR,
	FRUSTUM_PLANE_FAR,
	FRUSTUM_PLANE_SIZE
};

#include <d3d9.h>
#include <d3dx9.h>
#include "../../Engine/Render/CSceneFace.h"

/**
* CFrustum class.
* Manages the a frustum cone composed of 6 planes.
*/
class CFrustum
{
public:
	float m_nearDistance;
	float m_nearWidth;
	float m_nearHeight;
	float m_farDistance;
	float m_farWidth;
	float m_farHeight;
	float m_angle;
	float m_ratio;
	float m_tang;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_forward;
	D3DXVECTOR3 m_worldSpacePoints[FRUSTUM_POINT_SIZE];
	D3DXVECTOR3 m_localSpacePoints[FRUSTUM_POINT_SIZE];

	CScenePlane m_frustum[FRUSTUM_PLANE_SIZE];

public:
	CFrustum();
	~CFrustum();
	void SetFrustum(float angle, float ratio, float nearDistance, float farDistance);
	void UpdateFrustum(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const look);
	const bool BackfaceCull(CSceneFace * const face) const;
	const bool FrustumCull(CShape * const shape);
	const float GetFov() const;
	const D3DXVECTOR3 GetForward() const;
	const D3DXVECTOR3 * GetLocalSpacePoints() const;
};

#endif
