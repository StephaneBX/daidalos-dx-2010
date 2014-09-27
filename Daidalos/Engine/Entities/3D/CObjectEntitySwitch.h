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

#ifndef __COBJECTENTITYSWITCH_H
#define __COBJECTENTITYSWITCH_H

#define OBJECT_SWITCH_ANIMATION_FPS 1

#include "../../../Engine/Entities/3D/CObjectEntity.h"

enum animationSwitch
{
	OBJECT_ANIMATION_SWITCH_ACTIVATION,
	OBJECT_ANIMATION_SWITCH_SIZE
};

class CEffectSoundEntity;

/**
* CObjectEntitySwitch class.
* Generic class that manages the switches
*/
class CObjectEntitySwitch : public CObjectEntity
{
protected:
	int m_cardColor;
	static SObjectAnimation m_animationList[OBJECT_ANIMATION_SWITCH_SIZE];
	CEffectSoundEntity * m_actionSoundEffect;

public:
	CObjectEntitySwitch(const D3DXVECTOR3 rotation, const int groupId, const int color);
	virtual ~CObjectEntitySwitch();
	void Update(const float time);
	const bool CanBeActivated() const;

private:
	void FillGeometry();
};


#endif