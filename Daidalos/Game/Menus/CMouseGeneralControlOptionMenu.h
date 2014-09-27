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

#ifndef __CMOUSEGENERALCONTROLOPTIONMENU_H
#define __CMOUSEGENERALCONTROLOPTIONMENU_H

#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_SIZE 20
#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_PITCH 10
#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY_START 10
#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_SIZE 20
#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_PITCH 1
#define MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING_START 1

#include "../../Engine/Entities/2D/CModificationMenuEntity.h"

enum mouseGeneralControlMenuOptions
{
	MOUSE_GENERAL_CONTROL_MENU_OPTION_SENSIBILITY,
	MOUSE_GENERAL_CONTROL_MENU_OPTION_SMOOTHING,
	MOUSE_GENERAL_CONTROL_MENU_OPTION_PREVIOUS,
	MOUSE_GENERAL_CONTROL_MENU_OPTION_SIZE
};

/**
* CMouseGeneralControlOptionMenu class.
* Menu that contains a list of general options for the mouse.
*/
class CMouseGeneralControlOptionMenu : public CModificationMenuEntity
{
public:
	CMouseGeneralControlOptionMenu();
	~CMouseGeneralControlOptionMenu();

private:
	void ValidateChanges(const float time);
};

#endif
