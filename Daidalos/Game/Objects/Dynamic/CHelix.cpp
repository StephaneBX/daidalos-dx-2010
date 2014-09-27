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
#include <string>
#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Render/CSceneCamera.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Physic/CSphere.h"
#include "../../../Engine/Entities/Sound/CWaveMusicEntity.h"
#include "../../../Managers/CModManager.h"
#include "../../../Game/Objects/Dynamic/CHelix.h"

/**
* Constructor
* @param position The position
* @param rotation The rotation
*/
CHelix::CHelix(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation) : CObjectEntity(rotation)
{
	this->m_visibleShape = new CSphere(position, 5.0f);

	this->m_movingSoundEffect = this->m_modM->LoadEffect("Objects/Dynamic/helix.wav");
	this->m_movingSoundEffect->SetLoopable(true);
	this->m_movingSoundEffect->Play(this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, -this->m_visibleShape->GetPosition()->z);

	this->m_render = new CEntityRender();

	this->m_render->LoadMesh("helix_base.md2",0);
	this->m_render->LoadDiffuseTexture("cell_d.png", 0);
	this->m_render->LoadNormalTexture("cell_n.png", 0);

	this->m_render->LoadMesh("helix_main.md2",1);
	this->m_render->LoadDiffuseTexture("cell_d.png", 1);
	this->m_render->LoadNormalTexture("cell_n.png", 1);

	this->m_render->InitializeGeometry();
}

/**
* Destructor
*/
CHelix::~CHelix()
{
	this->m_movingSoundEffect->Stop();
}

/**
* Update the object
* @param time Elapsed time
*/
void CHelix::Update(const float time)
{
	this->m_rotation.y += HELIX_ROTATION_SPEED * time / 1000.0f;
	if(this->m_rotation.y >= 360)
		this->m_rotation.y -= 360;

	FillGeometry();	
}

/**
* Fill the geometry buffer
*/
void CHelix::FillGeometry()
{
	int index = 0;
	this->m_render->ResetRender();

	D3DXMatrixScaling(&this->m_scalingMatrix, this->m_scale.x, this->m_scale.y, this->m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&this->m_rotationMatrix, D3DXToRadian(this->m_rotation.y), D3DXToRadian(this->m_rotation.x), D3DXToRadian(this->m_rotation.z));
	D3DXMatrixTranslation(&this->m_translationMatrix, this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, this->m_visibleShape->GetPosition()->z);

	// Helix base
	this->m_render->GetMesh()[0]->GeometrySourceCopy(SHADING_TYPE_SMOOTH);
	this->m_render->GetMesh()[0]->GeometryTransform(&this->m_scalingMatrix, false);
	this->m_render->GetMesh()[0]->GeometryTransform(&this->m_translationMatrix, false);
	this->m_render->GetMesh()[0]->GeometryFinalize(this->m_render->GetCamera(), !this->m_render->GetCamera()->FrustumCull(this->m_visibleShape), SHADING_TYPE_SMOOTH);

	// Helix main
	this->m_render->GetMesh()[1]->GeometrySourceCopy(SHADING_TYPE_SMOOTH);
	this->m_render->GetMesh()[1]->GeometryTransform(&this->m_scalingMatrix, false);
	this->m_render->GetMesh()[1]->GeometryTransform(&this->m_rotationMatrix, true);
	this->m_render->GetMesh()[1]->GeometryTransform(&this->m_translationMatrix, false);
	this->m_render->GetMesh()[1]->GeometryFinalize(this->m_render->GetCamera(), !this->m_render->GetCamera()->FrustumCull(this->m_visibleShape), SHADING_TYPE_SMOOTH);

	this->m_render->FinalizeRender();
}
