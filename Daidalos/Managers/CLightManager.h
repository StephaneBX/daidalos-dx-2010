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

#ifndef __CLIGHTMANAGER_H
#define __CLIGHTMANAGER_H

#include "../Misc/Global.h"
#include "../Misc/CArraySorter.h"
#include "../Engine/Render/CLight.h"
#include "../Misc/CMemoryAllocator.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CSingletonManager.h"

class CDeviceManager;
class CLogManager;

/**
* CLightManager class.
* Manages the lights.
* The goal is to:
* - Update the lights.
* - Sort them to determine the best suited lights for the shadows (their number is limited).
* - Fill the data needed by the shaders to perform the lights calculation.
*/
class CLightManager : public CSingletonManager<CLightManager>
{
friend class CSingletonManager<CLightManager>;

private:
	// Lights information
	int m_numberOfVisibleLights;
	int m_numberOfShadowCasters;
	CMemoryAllocator<CLight> m_activeLight;
	int m_choosenShadowLights[LIGHT_SHADOW_MAXIMUM_SOURCES];

	float m_lightScore[MAXIMUM_ACTIVE_LIGHTS];
	int m_lightSorterIndex[MAXIMUM_ACTIVE_LIGHTS];
	CArraySorter<float> m_lightSorter;

	// Data needed for the Shaders
	int m_visibleLightsIndex[MAXIMUM_ACTIVE_LIGHTS];
	D3DXVECTOR4 m_visibleLightsPositionSpotAngle[MAXIMUM_ACTIVE_LIGHTS];
	D3DXVECTOR4 m_visibleLightsDirectionSpotLength[MAXIMUM_ACTIVE_LIGHTS];
	D3DXVECTOR4 m_visibleLightsColor[MAXIMUM_ACTIVE_LIGHTS];
	D3DXMATRIX m_visibleLightViewProjMatrix[MAXIMUM_ACTIVE_LIGHTS];
	D3DXMATRIX m_shadowLightViewProjMatrix[LIGHT_SHADOW_MAXIMUM_SOURCES];
	int m_shadowCaster[LIGHT_SHADOW_MAXIMUM_SOURCES];

	COptionManager * m_optionM;
	CDeviceManager * m_deviceM;
	CLogManager * m_logM;

public:
	~CLightManager();
	void Initialize();
	void FreeDatabase();
	void Update(CSceneCamera * const camera);
	const int GetNumberOfVisibleLights() const;
	const int GetNumberOfShadowCasters() const;
	const int * GetVisibleLightsIndex() const;
	const int * GetChoosenShadowLights() const;
	const int * GetShadowCasters() const;
	CLight * AddLight(const D3DXVECTOR3 * const position, const D3DXVECTOR3 * const direction, const D3DXVECTOR4 * const color, const float * angle, const float * length, bool * shadowCaster);
	D3DXVECTOR4 * GetVisibleLightPositionSpotAngle();
	D3DXVECTOR4 * GetVisibleLightDirectionSpotLength();
	D3DXVECTOR4 * GetVisibleLightColor();
	D3DXMATRIX * GetVisibleLightViewProjMatrix();
	D3DXMATRIX * GetShadowLightViewProjMatrix();

private:
	CLightManager();
};

#endif
