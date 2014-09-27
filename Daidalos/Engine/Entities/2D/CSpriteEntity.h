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

#ifndef __CSPRITEENTITY_H
#define __CSPRITEENTITY_H

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

class CTextureManager;

/**
* CSpriteEntity class.
* Manages the sprites.
* A sprite is composed of a texture and can be animated (all the animations muste be in this texture).
*/
class CSpriteEntity
{
private:
	int m_numRows;
	int m_numColumns;
	int m_width;
	int m_height;
	bool m_animated;
	bool m_enabled;
	float m_rotation;
	D3DXMATRIX m_matrix;
	D3DXVECTOR2 m_position;
	D3DXVECTOR2 m_center;
	D3DXVECTOR2 m_scale;
	D3DXVECTOR2 m_finalPosition;
	D3DCOLOR m_color;

	LPDIRECT3DTEXTURE9 m_texture;
	RECT m_rectangle;

	static LPD3DXSPRITE m_sprite;

public:
	CSpriteEntity(std::string textureName, D3DXVECTOR2 position, D3DXVECTOR2 center, D3DXVECTOR2 scale);
	CSpriteEntity(std::string textureName, D3DXVECTOR2 position, D3DXVECTOR2 center, D3DXVECTOR2 scale, int rows, int columns, int width, int height);
	~CSpriteEntity();
	void Animate(const int row, const int column);
	void SetPosition(const D3DXVECTOR2 position);
	void SetScale(const D3DXVECTOR2 scale);
	void SetColor(const D3DXCOLOR color);
	void SetEnabled(const bool value);
	void Render();
	static void OnLostDevice();
	static void OnResetDevice();
	const bool IsEnabled() const;
	LPD3DXSPRITE const GetSprite() const;
};

#endif
