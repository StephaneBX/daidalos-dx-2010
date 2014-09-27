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

#include <sstream>
#include "../Game/CHud.h"
#include "../Engine/Entities/2D/CSpriteEntity.h"
#include "../Engine/Entities/2D/CFontEntity.h"
#include "../Managers/COptionManager.h"

/**
* Constructor
*/
CHud::CHud()
{
	this->m_optionM = COptionManager::GetInstance();

	std::ostringstream stream;
	stream.str("");
	stream << "Fonts/" << this->m_optionM->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/" << "default.tga";

	this->m_healthText = "100%";
	this->m_armorText = "100%";
	this->m_ammoText = "0/0";

	float spriteRatio = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_INFOSPRITE_RATIO) / HUD_INFOSPRITE_SIZE;
	float flashlightRatio = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_INFOSPRITE_RATIO) / HUD_FLASHLIGHT_SPRITE_SIZE;
	float magneticCardRatio = (this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_SPRITE_RATIO) / HUD_MAGNETICCARD_SPRITE_SIZE;

	this->m_healthSprite = new CSpriteEntity("health.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_HEALTH_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_HEALTH_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(spriteRatio, spriteRatio));
	this->m_armorSprite = new CSpriteEntity("armor.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_ARMOR_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_ARMOR_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(spriteRatio, spriteRatio));
	this->m_ammoSprite = new CSpriteEntity("ammo.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_AMMO_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_AMMO_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(spriteRatio, spriteRatio));
	this->m_flashlightSprite = new CSpriteEntity("flashlight.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) - (HUD_FLASHLIGHT_SPRITE_SIZE * flashlightRatio * 2), this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_FLASHLIGHT_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(flashlightRatio, flashlightRatio));
	this->m_flashlightSprite->SetEnabled(false);

	this->m_blood = new CSpriteEntity("blood.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2((float)this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE), (float)this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE)));
	this->m_blood->SetColor(0x00FFFFFF);

	this->m_healthFont = new CFontEntity(stream.str(), this->m_healthText, 1.0f, FONT_ALIGN_LEFT, (int)(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_HEALTH_FONT_POSITION_X), (int)(this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_HEALTH_FONT_POSITION_Y), 0xFF00FF00);
	this->m_armorFont = new CFontEntity(stream.str(), this->m_armorText, 1.0f, FONT_ALIGN_LEFT, (int)(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_ARMOR_FONT_POSITION_X), (int)(this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_ARMOR_FONT_POSITION_Y), 0xFF00FF00);
	this->m_ammoFont = new CFontEntity(stream.str(), this->m_ammoText, 1.0f, FONT_ALIGN_LEFT, (int)(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_AMMO_FONT_POSITION_X), (int)(this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_AMMO_FONT_POSITION_Y), 0xFF00FF00);

	this->m_magneticCardSprite[OBJECT_MAGNETIC_CARD_RED] = new CSpriteEntity("magnetic_card_red.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_RED_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(magneticCardRatio, magneticCardRatio));
	this->m_magneticCardSprite[OBJECT_MAGNETIC_CARD_BLUE] = new CSpriteEntity("magnetic_card_blue.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_BLUE_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(magneticCardRatio, magneticCardRatio));
	this->m_magneticCardSprite[OBJECT_MAGNETIC_CARD_GREEN] = new CSpriteEntity("magnetic_card_green.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_GREEN_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(magneticCardRatio, magneticCardRatio));
	this->m_magneticCardSprite[OBJECT_MAGNETIC_CARD_YELLOW] = new CSpriteEntity("magnetic_card_yellow.png", D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_YELLOW_SPRITE_POSITION_X, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * HUD_MAGNETICCARD_SPRITE_POSITION_Y), D3DXVECTOR2(0,0), D3DXVECTOR2(magneticCardRatio, magneticCardRatio));

	for(int i = 0 ; i < OBJECT_MAGNETIC_CARD_SIZE ; i++)
		this->m_magneticCardSprite[i]->SetEnabled(false);
}

/**
* Destructor
*/
CHud::~CHud()
{
	SAFE_DELETE(this->m_healthSprite);
	SAFE_DELETE(this->m_armorSprite);
	SAFE_DELETE(this->m_ammoSprite);
	SAFE_DELETE(this->m_blood);
	SAFE_DELETE(this->m_flashlightSprite);

	for(int i = 0 ; i < OBJECT_MAGNETIC_CARD_SIZE ; i++)
		SAFE_DELETE(this->m_magneticCardSprite[i]);

	SAFE_DELETE(this->m_healthFont);
	SAFE_DELETE(this->m_armorFont);
	SAFE_DELETE(this->m_ammoFont);
}

/**
* Update the health value
* @param health The health value
*/
void CHud::UpdateHealth(const int health)
{
	this->m_stream.str("");
	int size = (int) (1 + log10((float)health));
	if(size < 1)
		size = 1;

	for(int i = 0 ; i < (int) (1 + log10((float)PLAYER_MAX_HEALTH)) - size ; i++)
		this->m_stream << " ";
	this->m_stream << health << "%";

	this->m_healthText = this->m_stream.str();
	this->m_healthFont->SetText(this->m_healthText);

	if(health < HUD_RED_COLOR_MAX)
	{
		this->m_healthFont->SetColor(0xFFFF0000);
	}
	else
	{
		if(health < HUD_WHITE_COLOR_MAX)
			this->m_healthFont->SetColor(0xFFFFFFFF);
		else
			this->m_healthFont->SetColor(0xFF00FF00);
	}
}

/**
* Update the armor value
* @param armor The armor value
*/
void CHud::UpdateArmor(const int armor)
{
	this->m_stream.str("");
	int size = (int) (1 + log10((float)armor));
	if(size < 1)
		size = 1;

	for(int i = 0 ; i < (int) (1 + log10((float)PLAYER_MAX_ARMOR)) - size ; i++)
		this->m_stream << " ";
	this->m_stream << armor << "%";

	this->m_armorText = this->m_stream.str();
	this->m_armorFont->SetText(this->m_armorText);

	if(armor < HUD_RED_COLOR_MAX)
	{
		this->m_armorFont->SetColor(0xFFFF0000);
	}
	else
	{
		if(armor < HUD_WHITE_COLOR_MAX)
			this->m_armorFont->SetColor(0xFFFFFFFF);
		else
			this->m_armorFont->SetColor(0xFF00FF00);
	}
}

/**
* Update the blood alpha
* @param alpha The blood alpha
*/
void CHud::UpdateBloodAlpha(const float alpha)
{
	this->m_blood->SetColor(D3DXCOLOR(1, 1 ,1, alpha));
}

/**
* Change the magnetic card state
* @param cardId The magnetic card id
*/
void CHud::SetMagneticCardEnabled(const int cardId)
{
	this->m_magneticCardSprite[cardId]->SetEnabled(true);
}

/**
* Change the flashlight state
* @param value The flashlight value
*/
void CHud::SetFlashLightEnabled(const bool value)
{
	return this->m_flashlightSprite->SetEnabled(value);
}

/**
* Render the HUD
*/
void CHud::Render()
{
	this->m_healthSprite->Render();
	this->m_armorSprite->Render();
	this->m_ammoSprite->Render();

	for(int i = 0 ; i < OBJECT_MAGNETIC_CARD_SIZE ; i++)
		if(this->m_magneticCardSprite[i]->IsEnabled())
			this->m_magneticCardSprite[i]->Render();

	if(this->m_flashlightSprite->IsEnabled())
		this->m_flashlightSprite->Render();

	this->m_healthFont->Render();
	this->m_armorFont->Render();
	this->m_ammoFont->Render();

	this->m_blood->Render();
}