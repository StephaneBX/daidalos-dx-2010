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

#include "../../../Misc/Global.h"
#include "../../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../../Managers/CModManager.h"
#include "../../../Managers/CDeviceManager.h"


LPD3DXSPRITE CSpriteEntity::m_sprite = NULL;

/**
* Constructor
* @param textureName The texture name
* @param position The sprite position
* @param center The sprite center
* @param scale The sprite scale
*/
CSpriteEntity::CSpriteEntity(std::string textureName, D3DXVECTOR2 position, D3DXVECTOR2 center, D3DXVECTOR2 scale)
{
	this->m_numRows = 1;
	this->m_numColumns = 1;
	this->m_center = center;
	this->m_scale = scale;
	this->m_rotation = 0;
	SetPosition(position);

	if(this->m_sprite == NULL)
		D3DXCreateSprite(CDeviceManager::GetInstance()->GetDevice(), &this->m_sprite);
	
	SetColor(D3DXCOLOR(1,1,1,1));

	this->m_texture = CModManager::GetInstance()->LoadTexture(textureName, TEXTURE_TYPE_SPRITE);

	this->m_animated = false;
	this->m_enabled = true;
}

/**
* Constructor for an animated sprite
* @param textureName The texture name
* @param position The sprite position
* @param center The sprite center
* @param scale The sprite scale
* @param rows The number of rows
* @param columns The number of columns
* @param width The width of one sprite
* @param height The height of one sprite
*/
CSpriteEntity::CSpriteEntity(std::string textureName, D3DXVECTOR2 position, D3DXVECTOR2 center, D3DXVECTOR2 scale, int rows, int columns, int width, int height)
{
	this->m_numRows = rows;
	this->m_numColumns = columns;
	this->m_width = width;
	this->m_height = height;
	this->m_center = center;
	this->m_scale = scale;
	this->m_rotation = 0;
	SetPosition(position);

	if(this->m_sprite == NULL)
		D3DXCreateSprite(CDeviceManager::GetInstance()->GetDevice(), &this->m_sprite);

	SetColor(D3DXCOLOR(1,1,1,1));

	this->m_texture = CModManager::GetInstance()->LoadTexture(textureName, TEXTURE_TYPE_SPRITE);

	this->m_animated = true;
	this->m_enabled = true;

	// Default to the top left corner
	Animate(0, 0);
}

/**
* Destructor
*/
CSpriteEntity::~CSpriteEntity()
{
	SAFE_RELEASE(this->m_sprite);
}

/**
* Animate the sprite
* @param row The choosen row
* @param column The choosen column
*/
void CSpriteEntity::Animate(const int row, const int column)
{
	this->m_rectangle.left = (column * this->m_width) % (this->m_numColumns * this->m_width);
	this->m_rectangle.right = ((column + 1) * this->m_width - 1) % (this->m_numColumns * this->m_width);
	this->m_rectangle.bottom = ((row + 1) * this->m_height - 1) % (this->m_numRows * this->m_height);
	this->m_rectangle.top = (row * this->m_height) % (this->m_numRows * this->m_height);
}

/**
* Set the position
* @param position The new position
*/
void CSpriteEntity::SetPosition(const D3DXVECTOR2 position)
{
	this->m_position = position;
	this->m_finalPosition = this->m_position - this->m_center;
}

/**
* Set the scale
* @param scale The new scale
*/
void CSpriteEntity::SetScale(const D3DXVECTOR2 scale)
{
	this->m_scale = scale;
}

/**
* Set the color
* @param color The new color
*/
void CSpriteEntity::SetColor(const D3DXCOLOR color)
{
	this->m_color = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
}

/**
* Enable / disable the sprite
* @param value The new state
*/
void CSpriteEntity::SetEnabled(const bool value)
{
	this->m_enabled = value;
}

/**
* Render the sprite
*/
void CSpriteEntity::Render()
{
	D3DXMatrixTransformation2D(&this->m_matrix, NULL, 0, &this->m_scale, &this->m_center, this->m_rotation, &this->m_finalPosition);
	this->m_sprite->SetTransform(&this->m_matrix);

	this->m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	if(this->m_animated)
		this->m_sprite->Draw(this->m_texture, &this->m_rectangle, NULL, NULL, this->m_color);
	else
		this->m_sprite->Draw(this->m_texture, NULL, NULL, NULL, this->m_color);
	this->m_sprite->End();
}

/**
* Warn the sprite that the device has been lost
*/
void CSpriteEntity::OnLostDevice()
{
	CSpriteEntity::m_sprite->OnLostDevice();
}

/**
* Warn the sprite that the device has been reset
*/
void CSpriteEntity::OnResetDevice()
{
	CSpriteEntity::m_sprite->OnResetDevice();
}

/**
* Check if the sprite is enabled
* @return The sprite state
*/
const bool CSpriteEntity::IsEnabled() const
{
	return this->m_enabled;
}

/**
* Get the sprite
* @return The sprite
*/
LPD3DXSPRITE const CSpriteEntity::GetSprite() const
{
	return this->m_sprite;
}
