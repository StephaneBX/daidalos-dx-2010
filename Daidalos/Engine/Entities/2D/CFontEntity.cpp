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
#include "../../../Engine/Entities/2D/CFontEntity.h"
#include "../../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../../Managers/COptionManager.h"

/**
* Constructor
* @param fontName The font file name
* @param text The text
* @param fontSize The font size
* @param alignment Set the alignment
* @param startX The start x coordinate
* @param startY The start y coordinate
* @param color The font color
*/
CFontEntity::CFontEntity(std::string fontName, std::string text, float fontSize, int alignment, int startX, int startY, D3DCOLOR color)
{
	this->m_optionM = COptionManager::GetInstance();

	this->m_fontName = fontName;
	this->m_text = text;
	this->m_alignment = alignment;
	this->m_originStartX = (float)startX;
	this->m_originStartY = (float)startY;;
	this->m_startX = this->m_originStartX;
	this->m_startY = this->m_originStartY;
	this->m_defaultColor = color;
	this->m_selectionColor = color;
	this->m_disabledColor = FONT_DEFAULT_DISABLE_COLOR;

	this->m_visible = true;
	this->m_enabled = true;
	this->m_pitch = this->m_optionM->GetFontTextureSize(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 16;
	this->m_currentTextSize = text.length();
	this->m_scale.x = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * FONT_CHARACTER_SCREENSIZE_WIDTH * fontSize) / this->m_pitch;
	this->m_scale.y = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * FONT_CHARACTER_SCREENSIZE_HEIGHT * fontSize) / this->m_pitch;
	Build();

	SetColor(this->m_defaultColor);
}

/**
* Constructor
* @param fontName The font file name
* @param text The text
* @param fontSize The font size
* @param alignment Set the alignment
* @param startX The start x coordinate
* @param startY The start y coordinate
* @param defaultColor The default color
* @param selectionColor The selection color
*/
CFontEntity::CFontEntity(std::string fontName, std::string text, float fontSize, int alignment, int startX, int startY, D3DCOLOR defaultColor, D3DCOLOR selectionColor)
{
	this->m_optionM = COptionManager::GetInstance();

	this->m_fontName = fontName;
	this->m_text = text;
	this->m_alignment = alignment;
	this->m_originStartX = (float)startX;
	this->m_originStartY = (float)startY;;
	this->m_startX = this->m_originStartX;
	this->m_startY = this->m_originStartY;
	this->m_defaultColor = defaultColor;
	this->m_selectionColor = selectionColor;
	this->m_disabledColor = FONT_DEFAULT_DISABLE_COLOR;

	this->m_visible = true;
	this->m_enabled = true;
	this->m_pitch = this->m_optionM->GetFontTextureSize(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 16;
	this->m_currentTextSize = text.length();
	this->m_scale.x = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * FONT_CHARACTER_SCREENSIZE_WIDTH * fontSize) / this->m_pitch;
	this->m_scale.y = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * FONT_CHARACTER_SCREENSIZE_HEIGHT * fontSize) / this->m_pitch;
	Build();

	SetColor(this->m_defaultColor);
}

/**
* Destructor
*/
CFontEntity::~CFontEntity()
{
	for(unsigned int i = 0 ; i < this->m_textSprite.size() ; i++)
		SAFE_DELETE(this->m_textSprite[i]);
}

/**
* Set the Y position and rebuild the font
*/
void CFontEntity::SetPositionY(const float positionY)
{
	this->m_originStartY = positionY;
	this->m_startY = positionY;
	Build();
}

/**
* Change the text
* @param text The new text
*/
void CFontEntity::SetText(std::string text)
{
	this->m_text = text;
	this->m_currentTextSize = text.length();
	Build();
	SetActive(true);
}

/**
* Change de active state
* @param value Active state value
*/
void CFontEntity::SetActive(const bool value)
{
	if(value)
	{
		// If true, use the selection color
		for(int i = 0 ; i < this->m_currentTextSize ; i++)
			this->m_textSprite.at(i)->SetColor(this->m_selectionColor);
	}
	else
	{
		if(this->m_enabled)
		{
			// If enabled, use the default color
			for(int i = 0 ; i < this->m_currentTextSize ; i++)
				this->m_textSprite.at(i)->SetColor(this->m_defaultColor);
		}
		else
		{
			// If disabled, use the last color
			for(int i = 0 ; i < this->m_currentTextSize ; i++)
				this->m_textSprite.at(i)->SetColor(this->m_disabledColor);
		}
	}
}

/**
* Enabled / Disable the font entity
* @param value True or false
*/
void CFontEntity::SetEnabled(const bool value)
{
	this->m_enabled = value;
}

/**
* Set visible / invisible the font entity
* @param value True for visible or false for invisible
*/
void CFontEntity::SetVisible(const bool value)
{
	this->m_visible = value;
}

/**
* Change de active color
* @param color The new color
*/
void CFontEntity::SetColor(const D3DCOLOR color)
{
	for(int i = 0 ; i < this->m_currentTextSize ; i++)
		this->m_textSprite.at(i)->SetColor(color);
}

/**
* Draw the font
*/
void CFontEntity::Render()
{
	if(this->m_visible)
	{
		for(int i = 0 ; i < this->m_currentTextSize ; i++)
			this->m_textSprite.at(i)->Render();
	}
}

/**
* Check if the font is selected
* @param x X coordinate of the cursor
* @param y Y coordinate of the cursor
* @return true if the font is selected
*/
const bool CFontEntity::IsSelected(const int x, const int y)
{
	return this->m_visible && x >= this->m_startX && x <= this->m_endX && y >= this->m_startY && y <= this->m_endY;
}

/**
* Check if the font is enabled
* @return True if the font is enabled, false otherwise
*/
const bool CFontEntity::IsEnabled() const
{
	return this->m_enabled;
}

/**
* Build the sprites to create the font
*/
void CFontEntity::Build()
{
	// Center, right or left
	if(this->m_alignment == FONT_ALIGN_CENTER)
		this->m_startX = this->m_originStartX - (this->m_currentTextSize * this->m_pitch * this->m_scale.x) / 2.0f;
	else
		if(this->m_alignment == FONT_ALIGN_RIGHT)
			this->m_startX = this->m_originStartX - this->m_currentTextSize * this->m_pitch * this->m_scale.x;

	this->m_endX = this->m_startX + (this->m_currentTextSize * this->m_pitch * this->m_scale.x);
	this->m_endY = this->m_originStartY + this->m_pitch * this->m_scale.y;

	// Create or update the sprites
	for(int i = 0 ; i < this->m_currentTextSize ; i++)
	{
		int texX = (int)((((unsigned char)this->m_text[i] - 32) % 16));
		int texY = (int)((unsigned char)(((unsigned char)this->m_text[i] - 32) / 16) + 1) - 1;
		if(texX < 0.0f || texX >= this->m_optionM->GetFontTextureSize(OPTIONMANAGER_CONFIGURATION_ACTIVE) || texY < 0.0f || texY >= this->m_optionM->GetFontTextureSize(OPTIONMANAGER_CONFIGURATION_ACTIVE))
		{
			texX = 0;
			texY = 0;
		}

		if((int)this->m_textSprite.size() < (i + 1))
			this->m_textSprite.push_back(new CSpriteEntity(this->m_fontName, D3DXVECTOR2(this->m_startX + (this->m_pitch * i * this->m_scale.x), this->m_startY), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_scale.x, this->m_scale.y), 16, 16, this->m_pitch, this->m_pitch));
		else
			this->m_textSprite.at(i)->SetPosition(D3DXVECTOR2(this->m_startX + (this->m_pitch * i * this->m_scale.x), this->m_startY));
		this->m_textSprite.at(i)->Animate(texY, texX);
	}
}

/**
* Get the y position
* @return The y position
*/
const float CFontEntity::GetPositionY() const
{
	return this->m_originStartY;
}
