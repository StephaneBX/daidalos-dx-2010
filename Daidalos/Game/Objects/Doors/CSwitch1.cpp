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

#include "../../../Game/CPlayer.h"
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Render/CSceneCamera.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Physic/CSphere.h"
#include "../../../Managers/CLightManager.h"
#include "../../../Managers/CModManager.h"
#include "../../../Game/Objects/Doors/CSwitch1.h"

/**
* Constructor
* @param position The position
* @param rotation The rotation
* @param groupId The associated group
*/
CSwitch1::CSwitch1(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation, const int groupId) : CObjectEntitySwitch(rotation, groupId, OBJECT_MAGNETIC_CARD_NONE)
{
	this->m_visibleShape = new CSphere(position, 1.5f);
	this->m_activationShape = new CSphere(position, 3.0f);

	this->m_actionSoundEffect = this->m_modM->LoadEffect("Objects/Doors/switch1.wav");

	this->m_render = new CEntityRender();
	this->m_render->LoadMesh("Doors/switch1_core.md2",0);
	this->m_render->LoadDiffuseTexture("switch1_core_d.png", 0);
	this->m_render->LoadNormalTexture("switch1_core_n.png", 0);
	this->m_render->LoadMesh("Doors/switch1_button.md2",1);
	this->m_render->LoadDiffuseTexture("switch1_button_d.png", 1);
	this->m_render->LoadNormalTexture("switch1_button_n.png", 1);

	this->m_render->InitializeGeometry();
}

/**
* Destructor
*/
CSwitch1::~CSwitch1(){}
