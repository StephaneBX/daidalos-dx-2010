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

#include "../../../Engine/Loaders/CMesh.h"
#include "../../../Engine/Render/CSceneCamera.h"
#include "../../../Engine/Render/CEntityRender.h"
#include "../../../Engine/Physic/CSphere.h"
#include "../../../Managers/CLightManager.h"
#include "../../../Game/Objects/Dynamic/CDwarf.h"

/**
* Constructor
* @param position The position
* @param rotation The rotation
*/
CDwarf::CDwarf(const D3DXVECTOR3 position, const D3DXVECTOR3 rotation) : CObjectEntity(rotation)
{
	this->m_visibleShape = new CSphere(position, 3.0f);

	this->m_render = new CEntityRender();
	this->m_render->LoadMesh("zombie.ms3d",0);
	this->m_render->LoadDiffuseTexture("zombie_d.png", 0);
	this->m_render->LoadNormalTexture("zombie_n.png", 0);

	this->m_scale = D3DXVECTOR3(0.2f, 0.2f, 0.2f);

	this->m_render->InitializeGeometry();

	this->m_render->SetFrame(145);
	this->m_timer = 0;
}

/**
* Destructor
*/
CDwarf::~CDwarf(){}

/**
* Update the object
* @param time Elapsed time
*/
void CDwarf::Update(const float time)
{
	if(this->m_timer <= 0)
	{
		this->m_render->Update(time);
		if(this->m_render->GetFrame() > 200)
		{
			this->m_render->SetFrame(145);
			this->m_timer = 500;
		}
	}
	else
		this->m_timer -= time;

	FillGeometry(SHADING_TYPE_SMOOTH);
}

/**
* Fill the geometry buffer
*/
void CDwarf::FillGeometry(const int shadingType)
{
	this->m_render->ResetRender();

	D3DXMatrixScaling(&this->m_scalingMatrix, this->m_scale.x, this->m_scale.y, this->m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&this->m_rotationMatrix, D3DXToRadian(this->m_rotation.y), D3DXToRadian(this->m_rotation.x), D3DXToRadian(this->m_rotation.z));
	D3DXMatrixTranslation(&this->m_translationMatrix, this->m_visibleShape->GetPosition()->x, this->m_visibleShape->GetPosition()->y, this->m_visibleShape->GetPosition()->z);

	for(int k = 0 ; k < MAX_MESH_PER_OBJECT ; k++)
	{
		if(this->m_render->GetMesh()[k] != NULL)
		{
			this->m_render->GetMesh()[k]->AnimateSourceCopy(this->m_render->GetFrame(), shadingType);

			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_rotationMatrix, true);
			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_scalingMatrix, false);
			this->m_render->GetMesh()[k]->GeometryTransform(&this->m_translationMatrix, false);

			this->m_render->GetMesh()[k]->GeometryFinalize(this->m_render->GetCamera(), !this->m_render->GetCamera()->FrustumCull(this->m_visibleShape), shadingType);
		}
	}
	this->m_render->FinalizeRender();
}