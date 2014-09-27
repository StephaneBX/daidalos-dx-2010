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

#ifndef __CHUD_H
#define __CHUD_H

#define PLAYER_MAX_HEALTH 100
#define PLAYER_MAX_ARMOR 100

#define HUD_INFOSPRITE_RATIO 0.019f
#define HUD_MAGNETICCARD_SPRITE_RATIO 0.025f
#define HUD_INFOSPRITE_SIZE 32
#define HUD_FLASHLIGHT_SPRITE_SIZE 64
#define HUD_MAGNETICCARD_SPRITE_SIZE 64

#define HUD_HEALTH_SPRITE_POSITION_X 0.03f
#define HUD_HEALTH_SPRITE_POSITION_Y 0.87f
#define HUD_ARMOR_SPRITE_POSITION_X 0.03f
#define HUD_ARMOR_SPRITE_POSITION_Y 0.93f
#define HUD_AMMO_SPRITE_POSITION_X 0.87f
#define HUD_AMMO_SPRITE_POSITION_Y 0.93f

#define HUD_FLASHLIGHT_SPRITE_POSITION_Y 0.02f
#define HUD_MAGNETICCARD_SPRITE_POSITION_Y 0.02f
#define HUD_MAGNETICCARD_RED_SPRITE_POSITION_X 0.45f
#define HUD_MAGNETICCARD_BLUE_SPRITE_POSITION_X 0.48f
#define HUD_MAGNETICCARD_GREEN_SPRITE_POSITION_X 0.51f
#define HUD_MAGNETICCARD_YELLOW_SPRITE_POSITION_X 0.54f


#define HUD_HEALTH_FONT_POSITION_X 0.07f
#define HUD_HEALTH_FONT_POSITION_Y 0.8675f
#define HUD_ARMOR_FONT_POSITION_X 0.07f
#define HUD_ARMOR_FONT_POSITION_Y 0.9285f
#define HUD_AMMO_FONT_POSITION_X 0.92f
#define HUD_AMMO_FONT_POSITION_Y 0.9285f

#define HUD_RED_COLOR_MAX 25
#define HUD_WHITE_COLOR_MAX 75

#include <d3d9.h>
#include <d3dx9.h>
#include "../Misc/Global.h"

class CSpriteEntity;
class CFontEntity;
class COptionManager;

/**
* CHud class.
* Manages the player HUD.
*/
class CHud
{
private:
	std::ostringstream m_stream;
	std::string m_healthText;
	std::string m_armorText;
	std::string m_ammoText;

	CSpriteEntity * m_healthSprite;
	CSpriteEntity * m_armorSprite;
	CSpriteEntity * m_ammoSprite;
	CSpriteEntity * m_magneticCardSprite[OBJECT_MAGNETIC_CARD_SIZE];
	CSpriteEntity * m_flashlightSprite;
	CSpriteEntity * m_blood;

	CFontEntity * m_healthFont;
	CFontEntity * m_armorFont;
	CFontEntity * m_ammoFont;
	CFontEntity * m_magneticCardFont[OBJECT_MAGNETIC_CARD_SIZE];

	COptionManager * m_optionM;

public:
	CHud();
	~CHud();
	void UpdateHealth(const int health);
	void UpdateArmor(const int armor);
	void UpdateBloodAlpha(const float alpha);
	void SetMagneticCardEnabled(const int cardId);
	void SetFlashLightEnabled(const bool value);
	void Render();
};

#endif
