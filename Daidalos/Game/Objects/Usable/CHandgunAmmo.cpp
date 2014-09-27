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

#include "../../../Engine/Physic/CSphere.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Managers/CLightManager.h"
#include "../../../Managers/CModManager.h"
#include "../../../Game/Objects/Usable/CHandgunAmmo.h"

/**
* Constructor
* @param position The position
*/
CHandgunAmmo::CHandgunAmmo(const D3DXVECTOR3 position) : CObjectEntityUsable(D3DXVECTOR3(0,0,0))
{
	this->m_visibleShape = new CSphere(position, 1.0f);
	this->m_activationShape = new CSphere(position, 1.5f);

	this->m_actionSoundEffect = this->m_modM->LoadEffect("Objects/Usable/weapon_bonus.wav");

	this->m_render = new CEntityRender();
	this->m_render->LoadMesh("Usable/handgun_ammo.md2",0);
	this->m_render->LoadDiffuseTexture("handgun_ammo_d.png", 0);
	this->m_render->LoadNormalTexture("handgun_ammo_n.png", 0);

	this->m_render->InitializeGeometry();
}

/**
* Destructor
*/
CHandgunAmmo::~CHandgunAmmo(){}

/**
* Update the object
* @param time Elapsed time
*/
void CHandgunAmmo::Update(const float time)
{
	if(this->m_state == OBJECT_STATE_USABLE_USING)
	{
		this->m_actionSoundEffect->Play(this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, -this->m_visibleShape->GetPosition()->z);
		this->m_alive = false;
	}

	CObjectEntityUsable::Update(time);

	FillGeometry(SHADING_TYPE_SMOOTH);
}
