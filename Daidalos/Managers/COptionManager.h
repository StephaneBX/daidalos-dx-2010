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

#ifndef __COPTIONMANAGER_H
#define __COPTIONMANAGER_H

#define INI_FILENAME "./options.ini"
#define INI_DEFAULT_RESOLUTION 0
#define INI_DEFAULT_RESOLUTION_WIDTH 800
#define INI_DEFAULT_RESOLUTION_HEIGHT 600
#define INI_DEFAULT_ANISOTROPIC_FILTERING 0
#define INI_DEFAULT_SHADOW_SOURCES 0
#define INI_DEFAULT_EFFECTS_VOLUME 70
#define INI_DEFAULT_MUSIC_VOLUME 60
#define INI_DEFAULT_VIEW_DISTANCE 50
#define INI_DEFAULT_MOUSE_SMOOTHING 10
#define INI_DEFAULT_MOUSE_SENSIBILITY 100
#define INI_DEFAULT_GAMEPAD_SENSIBILITY 100
#define INI_DEFAULT_DEFERRED_RENDERING 0
#define INI_DEFAULT_TEXTURE_QUALITY 2
#define INI_DEFAULT_LANGAGE 0
#define INI_DEFAULT_FRUSTUM_ANGLE_STANDARD 27
#define INI_DEFAULT_FRUSTUM_ANGLE_WIDE 45
#define INI_DEFAULT_FONT_TEXTURE_SIZE_HIGH 2048
#define INI_DEFAULT_FONT_TEXTURE_SIZE_MEDIUM 1024
#define INI_DEFAULT_FONT_TEXTURE_SIZE_LOW 512
#define INI_DEFAULT_SHOW_FPS 0
#define RESOLUTION_TAB_SIZE 11
#define LIGHT_SHADOWMAP_SIZE 1024
#define LIGHT_SHADOW_MAXIMUM_SOURCES 4
#define SCREEN_FORMAT_STANDARD_RATIO 0.75f
#define SCREEN_FORMAT_WIDE_RATIO 0.625f
#define MUSIC_VOLUME_MAXIMUM 200
#define EFFECTS_VOLUME_MAXIMUM 200

#include <string>
#include "../Managers/CSingletonManager.h"

enum actionKeyList
{
	ACTION_KEY_UP_MENU,
	ACTION_KEY_DOWN_MENU,
	ACTION_KEY_LEFT_MENU,
	ACTION_KEY_RIGHT_MENU,
	ACTION_KEY_ESCAPE_MENU,
	ACTION_KEY_ENTER_MENU,
	ACTION_KEY_FORWARD,
	ACTION_KEY_BACKWARD,
	ACTION_KEY_STRAFE_LEFT,
	ACTION_KEY_STRAFE_RIGHT,
	ACTION_KEY_TURN_LEFT,
	ACTION_KEY_TURN_RIGHT,
	ACTION_KEY_LOOK_UP,
	ACTION_KEY_LOOK_DOWN,
	ACTION_KEY_CENTER_VIEW,
	ACTION_KEY_FLASHLIGHT,
	ACTION_KEY_FIRE,
	ACTION_KEY_JUMP,
	ACTION_KEY_CROUCH,
	ACTION_KEY_SIZE
};

enum optionConfigurationType
{
	OPTIONMANAGER_CONFIGURATION_ACTIVE,
	OPTIONMANAGER_CONFIGURATION_SAVED,
	OPTIONMANAGER_CONFIGURATION_SIZE
};

enum screenFormat
{
	SCREEN_FORMAT_STANDARD,
	SCREEN_FORMAT_WIDE
};

struct SConfigurationResolution
{
	int m_width;
	int m_height;
	int m_screenFormat;
	float m_screenRatio;
	bool m_available;
};

struct SConfigurationType
{
	SConfigurationResolution * m_resolution;
	int m_resolutionId;
	int m_fullscreen;
	int m_maxShadowSources;
	int m_anisotropicFiltering;
	int m_verticalSynchronization;
	int m_deferredRendering;
	int m_textureQuality;
	int m_fontTextureSize[3];
	int m_showFps;
	int m_debugMode;
	int m_langage;
	float m_musicVolume;
	float m_effectsVolume;
	float m_viewDistance;
	float m_frustumAngle;
	std::string m_texturePath[3];
};

/**
* COptionManager class.
* Manages the general options.
*/
class COptionManager : public CSingletonManager<COptionManager>
{
friend class CSingletonManager<COptionManager>;

private:
	SConfigurationType m_configurations[OPTIONMANAGER_CONFIGURATION_SIZE];

public:
	static SConfigurationResolution m_resolutionTab[RESOLUTION_TAB_SIZE];
	static std::string m_actionKeyName[ACTION_KEY_SIZE];

public:
	~COptionManager();
	void Save();
	void SetResolution(const int configurationType, const int resolutionId);
	void SetAnisotropicFiltering(const int configurationType, const int anisotropic);
	void SetMaxShadowSources(const int configurationType, const int maxShadowSources);
	void SetLangage(const int configurationType, const int langage);
	void SetViewDistance(const int configurationType, const int viewDistance);
	void SetFullScreen(const int configurationType, const int fullScreen);
	void SetVerticalSynchronization(const int configurationType, const int verticalSynchronisation);
	void SetDeferredRendering(const int configurationType, const int deferredShading);
	void SetFPSEnabled(const int configurationType, const int fpsEnabled);
	void SetMusicVolume(const int configurationType, const float musicVolume);
	void SetEffectsVolume(const int configurationType, const float effectsVolume);
	void SetTextureQuality(const int configurationType, const int textureQuality);
	const bool IsFullScreen(const int configurationType) const;
	const bool IsVerticalSynchronizationForced(const int configurationType) const;
	const bool IsDeferredRenderingEnabled(const int configurationType) const;
	const bool IsFPSEnabled(const int configurationType) const;
	const bool IsDebugModeEnabled(const int configurationType) const;
	const int GetResolutionId(const int configurationType) const;
	const int GetWidth(const int configurationType) const;
	const int GetHeight(const int configurationType) const;
	const int GetScreenFormat(const int configurationType) const;
	const int GetAnisotropicFiltering(const int configurationType) const;
	const int GetMaxShadowSources(const int configurationType) const;
	const int GetFontTextureSize(const int configurationType) const;
	const int GetLangage(const int configurationType) const;
	const int GetTextureQuality(const int configurationType) const;
	const float GetMusicVolume(const int configurationType) const;
	const float GetEffectsVolume(const int configurationType) const;
	const float GetScreenRatio(const int configurationType) const;
	const float GetViewDistance(const int configurationType) const;
	const float GetFrustumAngle(const int configurationType) const;
	const std::string GetTexturePath(const int configurationType) const;
	

private:
	COptionManager();
};

#endif
