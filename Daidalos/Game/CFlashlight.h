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

#ifndef __CFLASHLIGHT_H
#define __CFLASHLIGHT_H

#define FLASHLIGHT_POSITION -1.2f
#define FLASHLIGHT_DEFAULT_ANGLE 20
#define FLASHLIGHT_DEFAULT_LENGTH 50
#define FLASHLIGHT_DEFAULT_COLOR D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1)
#define FLASHLIGHT_VIEW_POINT 3000.0f

#include <d3d9.h>
#include <d3dx9.h>

class CLight;

/**
* CFlashlight class.
* Manages the player flashlight.
*/
class CFlashlight
{
private:
	float m_angle;
	float m_length;
	bool m_shadowCaster;
	D3DXMATRIX m_rotationMatrix;
	D3DXVECTOR3 m_lookOrigin;
	D3DXVECTOR3 m_look;

	D3DXVECTOR4 m_color;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_direction;
	CLight * m_flashlight;

public:
	CFlashlight();
	~CFlashlight();
	void Update(D3DXVECTOR3 * position, D3DXVECTOR3 * rotation);
	void SetEnabled(const bool value);
	void Switch();
};

#endif
