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

#ifndef __CLIGHT_H
#define __CLIGHT_H

#define LIGHT_SCORE_INFINITY 100000.0f
#define LIGHT_SCORE_HIGH_PRIORITY -100000.0f
#define LIGHT_FLAG_SHADOW_CASTER 1

#include <d3d9.h>
#include <d3dx9.h>

class CShape;
class CSceneCamera;
class COptionManager;

/**
* CLight class.
* Manages the lights of the game. Only Spot light are currently available.
*/
class CLight
{
private:
	bool m_highPriority;
	bool m_enabled;
	bool m_shadowCaster;
	float m_score;
	float m_spotAngle;
	float m_spotLength;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_color;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projMatrix;
	D3DXMATRIX m_viewProjMatrix;
	CShape * m_boundingBox;
	
	COptionManager * m_optionM;


public:
	CLight();
	~CLight();
	void Update(CSceneCamera * const camera);
	void UpdatePosition(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction);
	void Initialize(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction, const D3DXVECTOR4 * const color, const float * angle, const float * length, bool * shadowCaster);
	void SetHighPriority(const bool value);
	void SetEnabled(const bool value);
	void SetPosition(const D3DXVECTOR3 * const position);
	void SetDirection(const D3DXVECTOR3 * const direction);
	void SetColor(const D3DXVECTOR4 * const color);
	bool * const IsHighPriority();
	bool * const IsEnabled();
	bool * const IsShadowCaster();
	bool const IsVisible();
	float * const GetSpotAngle();
	float * const GetSpotLength();
	float * const GetScore();
	const D3DXVECTOR3 * const GetPosition();
	D3DXVECTOR3 * const GetDirection();
	D3DXVECTOR4 * const GetColor();
	D3DXMATRIX * const GetViewProjMatrix();
	CLight & operator=(const CLight & light);

private:
	void UpdateView();
};

#endif
