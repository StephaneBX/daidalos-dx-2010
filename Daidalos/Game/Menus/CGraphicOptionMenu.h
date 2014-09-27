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

#ifndef __CGRAPHICOPTIONMENU_H
#define __CGRAPHICOPTIONMENU_H

#define GRAPHIC_MENU_OPTION_RESOLUTION_SIZE RESOLUTION_TAB_SIZE
#define GRAPHIC_MENU_OPTION_FULLSCREEN_SIZE 2
#define GRAPHIC_MENU_OPTION_TEXTUREQUALITY_SIZE 3
#define GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE 5
#define GRAPHIC_MENU_OPTION_SHADOWS_SIZE 5
#define GRAPHIC_MENU_OPTION_DEFERREDSHADING_SIZE 2
#define GRAPHIC_MENU_OPTION_VERTICALSYNCHRO_SIZE 2
#define GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE 6
#define GRAPHIC_MENU_OPTION_VIEWDISTANCE_START 50
#define GRAPHIC_MENU_OPTION_VIEWDISTANCE_PITCH 10

#include "../../Engine/Entities/2D/CModificationMenuEntity.h"

enum graphicMenuOptions
{
	GRAPHIC_MENU_OPTION_RESOLUTION,
	GRAPHIC_MENU_OPTION_FULLSCREEN,
	GRAPHIC_MENU_OPTION_TEXTUREQUALITY,
	GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING,
	GRAPHIC_MENU_OPTION_SHADOWS,
	GRAPHIC_MENU_OPTION_DEFERREDRENDERING,
	GRAPHIC_MENU_OPTION_VERTICALSYNCHRO,
	GRAPHIC_MENU_OPTION_VIEWDISTANCE,
	GRAPHIC_MENU_OPTION_PREVIOUS,
	GRAPHIC_MENU_OPTION_SIZE
};

/**
* CGraphicOptionMenu class.
* Menu that contains a list of graphics options.
*/
class CGraphicOptionMenu : public CModificationMenuEntity
{
public:
	CGraphicOptionMenu();
	~CGraphicOptionMenu();

private:
	void ValidateChanges(const float time);
};

#endif
