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

#include <d3d9.h>
#include <d3dx9.h>
#include "../Managers/CDeviceManager.h"
#include "../Managers/CLogManager.h"
#include "../Managers/CLightManager.h"

/**
* Constructor
*/
CLightManager::CLightManager(){}

/**
* Destructor
*/
CLightManager::~CLightManager(){}

/**
* Initialize the lights
*/
void CLightManager::Initialize()
{
	this->m_optionM = COptionManager::GetInstance();
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
	this->m_numberOfVisibleLights = 0;
	this->m_numberOfShadowCasters = 0;
	this->m_activeLight.Allocate(MAXIMUM_ACTIVE_LIGHTS);
}

/**
* Free the lights database
*/
void CLightManager::FreeDatabase()
{
	this->m_activeLight.DeleteAll();
}

/**
* Update the lights
* @param camera The camera position
*/
void CLightManager::Update(CSceneCamera * const camera)
{
	int counter = 0;
	this->m_numberOfVisibleLights = 0;

	// Update the lights and register the visible ones
	for(int i = 0 ; i < this->m_activeLight.GetArraySize() ; i++)
	{
		if(this->m_activeLight.IsAlive(i) && *this->m_activeLight.GetAt(i)->IsEnabled())
		{
			this->m_activeLight.GetAt(i)->Update(camera);
			if(this->m_activeLight.GetAt(i)->IsVisible())
			{
				this->m_lightSorterIndex[counter] = i;
				this->m_lightScore[counter] = *this->m_activeLight.GetAt(i)->GetScore();
				++counter;
			}
		}
	}

	// Light sort
	this->m_numberOfVisibleLights = counter;
	this->m_lightSorter.QuickSort(this->m_lightScore, this->m_lightSorterIndex, 0, this->m_numberOfVisibleLights - 1);

	// Copy visible lights information
	counter = 0;
	for(int i = this->m_numberOfVisibleLights - 1 ; i >= 0 ; i--)
	{
		this->m_visibleLightsPositionSpotAngle[counter].x = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetPosition()->x;
		this->m_visibleLightsPositionSpotAngle[counter].y = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetPosition()->y;
		this->m_visibleLightsPositionSpotAngle[counter].z = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetPosition()->z;
		this->m_visibleLightsPositionSpotAngle[counter].w = *this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetSpotAngle();

		this->m_visibleLightsDirectionSpotLength[counter].x = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetDirection()->x;
		this->m_visibleLightsDirectionSpotLength[counter].y = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetDirection()->y;
		this->m_visibleLightsDirectionSpotLength[counter].z = this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetDirection()->z;
		this->m_visibleLightsDirectionSpotLength[counter].w = *this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetSpotLength();

		this->m_visibleLightsColor[counter] = *this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetColor();
		this->m_visibleLightViewProjMatrix[counter] = *this->m_activeLight.GetAt(this->m_lightSorterIndex[i])->GetViewProjMatrix();

		this->m_visibleLightsIndex[counter] = this->m_lightSorterIndex[i];

		counter++;
	}

	// Choose the most suitable shadow caster lights
	int shadowCounter = 0;
	for(int i = 0 ; i < LIGHT_SHADOW_MAXIMUM_SOURCES ; i++)
	{
		this->m_choosenShadowLights[i] = -1;
		this->m_shadowCaster[i] = 0;
	}

	for(int i = 0 ; i < this->m_numberOfVisibleLights && shadowCounter < this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) ; i++)
	{
		if(*this->m_activeLight.GetAt(this->m_visibleLightsIndex[i])->IsShadowCaster())
		{
			this->m_choosenShadowLights[shadowCounter] = this->m_visibleLightsIndex[i];
			this->m_shadowCaster[shadowCounter] = 1;
			this->m_shadowLightViewProjMatrix[shadowCounter] = *this->m_activeLight.GetAt(this->m_visibleLightsIndex[i])->GetViewProjMatrix();
			++shadowCounter;
		}
	}
	this->m_numberOfShadowCasters = shadowCounter;
}

/**
* Get the number of visible lights
* @return The number of visible lights
*/
const int CLightManager::GetNumberOfVisibleLights() const
{
	return this->m_numberOfVisibleLights;
}

/**
* Get the number of shadow caster lights
* @return The number of shadow caster lights
*/
const int CLightManager::GetNumberOfShadowCasters() const
{
	return this->m_numberOfShadowCasters;
}

/**
* Get the indexes of the visible lights
* @return The indexes of the visible lights
*/
const int * CLightManager::GetVisibleLightsIndex() const
{
	return this->m_visibleLightsIndex;
}


/**
* Get the indexes of the choosen lights to cast shadow
* @return The indexes of the choosen lights to cast shadow
*/
const int * CLightManager::GetChoosenShadowLights() const
{
	return this->m_choosenShadowLights;
}

/**
* Get a buffer containing shadow casting capability
* @return A buffer containing shadow casting capability
*/
const int * CLightManager::GetShadowCasters() const
{
	return this->m_choosenShadowLights;
}

/**
* Add a light to the database
* @param position The position
* @param direction The direction
* @param color The color
* @param angle The half view angle of the spot
* @param length The length of the spot
* @param shadowCaster Cast shadow
* @return A pointer to the light
*/
CLight * CLightManager::AddLight(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction, const D3DXVECTOR4 * const color, const float * angle, const float * length, bool * shadowCaster)
{
	CLight * light = this->m_activeLight.New();
	if(light != NULL)
	{
		if(this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) == 0)
			*shadowCaster = false;
		light->Initialize(position, direction, color, angle, length, shadowCaster);
	}
	return light;
}

/**
* Get a buffer containing position and angle information used by the shaders
* @return A buffer containing position and angle information used by the shaders
*/
D3DXVECTOR4 * CLightManager::GetVisibleLightPositionSpotAngle()
{
	return this->m_visibleLightsPositionSpotAngle;
}

/**
* Get a buffer containing direction and length information used by the shaders
* @return A buffer containing direction and length information used by the shaders
*/
D3DXVECTOR4 * CLightManager::GetVisibleLightDirectionSpotLength()
{
	return this->m_visibleLightsDirectionSpotLength;
}

/**
* Get a buffer containing color information used by the shaders
* @return A buffer containing color information used by the shaders
*/
D3DXVECTOR4 * CLightManager::GetVisibleLightColor()
{
	return this->m_visibleLightsColor;
}

/**
* Get a buffer containing the viewProj matrix used by the shaders for the active lights
* @return A buffer containing the viewProj matrix used by the shaders for the active lights
*/
D3DXMATRIX * CLightManager::GetVisibleLightViewProjMatrix()
{
	return this->m_visibleLightViewProjMatrix;
}

/**
* Get a buffer containing the viewProj matrix used by the shaders for the shadow lights
* @return A buffer containing the viewProj matrix used by the shaders for the shadow lights
*/
D3DXMATRIX * CLightManager::GetShadowLightViewProjMatrix()
{
	return this->m_shadowLightViewProjMatrix;
}
