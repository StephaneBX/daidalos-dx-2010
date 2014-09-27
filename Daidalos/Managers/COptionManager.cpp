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

#include <windows.h>
#include <sstream>
#include "../Managers/COptionManager.h"

// Resolutions tab
SConfigurationResolution COptionManager::m_resolutionTab[RESOLUTION_TAB_SIZE] = 
{
	{800, 600, SCREEN_FORMAT_STANDARD, SCREEN_FORMAT_STANDARD_RATIO, false},
	{1024, 768, SCREEN_FORMAT_STANDARD, SCREEN_FORMAT_STANDARD_RATIO, false},
	{1280, 960, SCREEN_FORMAT_STANDARD, SCREEN_FORMAT_STANDARD_RATIO, false},
	{1600, 1200, SCREEN_FORMAT_STANDARD, SCREEN_FORMAT_STANDARD_RATIO, false},
	{1900, 1600, SCREEN_FORMAT_STANDARD, SCREEN_FORMAT_STANDARD_RATIO, false},
	{1024, 640, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false},
	{1280, 800, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false},
	{1440, 900, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false},
	{1680, 1050, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false},
	{1920, 1080, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false},
	{1920, 1200, SCREEN_FORMAT_WIDE, SCREEN_FORMAT_WIDE_RATIO, false}
};

// Action key names
std::string COptionManager::m_actionKeyName[ACTION_KEY_SIZE] = 
{
	"up_menu",
	"down_menu",
	"left_menu",
	"right_menu",
	"escape_menu",
	"enter_menu",
	"forward",
	"backward",
	"strafe_left",
	"strafe_right",
	"turn_left",
	"turn_right",
	"look_up",
	"look_down",
	"center_view",
	"flashlight",
	"fire",
	"jump",
	"crouch"
};

/**
* Constructor
*/
COptionManager::COptionManager()
{
	// General part
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_resolutionId = GetPrivateProfileInt("general", "resolution", INI_DEFAULT_RESOLUTION, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_anisotropicFiltering = GetPrivateProfileInt("general", "anisotropic_filtering", INI_DEFAULT_ANISOTROPIC_FILTERING, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fullscreen = GetPrivateProfileInt("general", "fullscreen", 0, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_maxShadowSources = GetPrivateProfileInt("general", "shadow_sources", INI_DEFAULT_SHADOW_SOURCES, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_viewDistance = (float)GetPrivateProfileInt("general", "view_distance", INI_DEFAULT_VIEW_DISTANCE, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_verticalSynchronization = GetPrivateProfileInt("general", "vertical_synchro", 0, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_deferredRendering = GetPrivateProfileInt("general", "deferred_rendering", INI_DEFAULT_DEFERRED_RENDERING, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality = GetPrivateProfileInt("general", "texture_quality", INI_DEFAULT_TEXTURE_QUALITY, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_langage = GetPrivateProfileInt("general", "langage", INI_DEFAULT_LANGAGE, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume = (float)GetPrivateProfileInt("general", "music_volume", INI_DEFAULT_MUSIC_VOLUME, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume = (float)GetPrivateProfileInt("general", "effects_volume", INI_DEFAULT_EFFECTS_VOLUME, INI_FILENAME);
	SetResolution(OPTIONMANAGER_CONFIGURATION_ACTIVE, this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_resolutionId);

	// Value verification
	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_anisotropicFiltering < 0 || this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_anisotropicFiltering > 16 || this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_anisotropicFiltering %2 != 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_anisotropicFiltering = 0;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_maxShadowSources < 0 || this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_maxShadowSources > LIGHT_SHADOW_MAXIMUM_SOURCES)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_maxShadowSources = 0;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_deferredRendering != 0 && this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_deferredRendering != 1)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_deferredRendering = 0;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality < 0 || this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality > 2)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality = INI_DEFAULT_TEXTURE_QUALITY;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume < 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume = 0;
	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume > MUSIC_VOLUME_MAXIMUM)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume = MUSIC_VOLUME_MAXIMUM;
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_musicVolume /= 1000.0f;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume < 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume = 0;
	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume > EFFECTS_VOLUME_MAXIMUM)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume = EFFECTS_VOLUME_MAXIMUM;
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_effectsVolume /= 1000.0f;

	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_texturePath[0] = "Low";
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_texturePath[1] = "Medium";
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_texturePath[2] = "High";

	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[0] = GetPrivateProfileInt("engine", "low_font_texture_size", INI_DEFAULT_FONT_TEXTURE_SIZE_LOW, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[1] = GetPrivateProfileInt("engine", "medium_font_texture_size", INI_DEFAULT_FONT_TEXTURE_SIZE_MEDIUM, INI_FILENAME);
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[2] = GetPrivateProfileInt("engine", "high_font_texture_size", INI_DEFAULT_FONT_TEXTURE_SIZE_HIGH, INI_FILENAME);

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[0] < 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[0] = INI_DEFAULT_FONT_TEXTURE_SIZE_LOW;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[1] < 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[1] = INI_DEFAULT_FONT_TEXTURE_SIZE_MEDIUM;

	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[2] < 0)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_fontTextureSize[2] = INI_DEFAULT_FONT_TEXTURE_SIZE_HIGH;

	// Misc part
	this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_showFps = GetPrivateProfileInt("misc", "show_fps", INI_DEFAULT_SHOW_FPS, INI_FILENAME);
	if(this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_showFps < 0 || this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_showFps > 1)
		this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_showFps = INI_DEFAULT_SHOW_FPS;

	this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED] = this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE];
}

/**
* Destructor
*/
COptionManager::~COptionManager(){}

/**
* Save the configuration
*/
void COptionManager::Save()
{
	std::ostringstream stream;
	std::string tmp;

	// Resolution
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_resolutionId;
	tmp = stream.str();
	WritePrivateProfileString("general", "resolution", tmp.c_str(), INI_FILENAME);

	// Texture quality
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_textureQuality;
	tmp = stream.str();
	WritePrivateProfileString("general", "texture_quality", tmp.c_str(), INI_FILENAME);

	// Anisotropic filtering
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_anisotropicFiltering;
	tmp = stream.str();
	WritePrivateProfileString("general", "anisotropic_filtering", tmp.c_str(), INI_FILENAME);

	// Shadow sources
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_maxShadowSources;
	tmp = stream.str();
	WritePrivateProfileString("general", "shadow_sources", tmp.c_str(), INI_FILENAME);

	// Deferred rendering
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_deferredRendering;
	tmp = stream.str();
	WritePrivateProfileString("general", "deferred_rendering", tmp.c_str(), INI_FILENAME);

	// Full screen
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_fullscreen;
	tmp = stream.str();
	WritePrivateProfileString("general", "fullscreen", tmp.c_str(), INI_FILENAME);

	// View distance
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_viewDistance;
	tmp = stream.str();
	WritePrivateProfileString("general", "view_distance", tmp.c_str(), INI_FILENAME);

	// Vertical synchronisation
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_verticalSynchronization;
	tmp = stream.str();
	WritePrivateProfileString("general", "vertical_synchro", tmp.c_str(), INI_FILENAME);

	// Langage
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_langage;
	tmp = stream.str();
	WritePrivateProfileString("general", "langage", tmp.c_str(), INI_FILENAME);

	// Music volume
	stream.str("");
	stream << (this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_musicVolume * 1000.0f);
	tmp = stream.str();
	WritePrivateProfileString("general", "music_volume", tmp.c_str(), INI_FILENAME);

	// Effects volume
	stream.str("");
	stream << (this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_effectsVolume * 1000.0f);
	tmp = stream.str();
	WritePrivateProfileString("general", "effects_volume", tmp.c_str(), INI_FILENAME);

	// Show FPS
	stream.str("");
	stream << this->m_configurations[OPTIONMANAGER_CONFIGURATION_SAVED].m_showFps;
	tmp = stream.str();
	WritePrivateProfileString("misc", "show_fps", tmp.c_str(), INI_FILENAME);
}

/**
* Set the resolution
* @param configurationType The configuration type
* @param resolutionId The resolution id
*/
void COptionManager::SetResolution(const int configurationType, const int resolutionId)
{
	if(resolutionId >= 0 && resolutionId < RESOLUTION_TAB_SIZE)
	{
		this->m_configurations[configurationType].m_resolution = &this->m_resolutionTab[resolutionId];
		this->m_configurations[configurationType].m_resolutionId = resolutionId;
	}
	else
	{
		this->m_configurations[configurationType].m_resolution = &this->m_resolutionTab[0];
		this->m_configurations[configurationType].m_resolutionId = 0;
	}

	// Engine part
	// Modify the frustum depending on the screen ratio. 4/3 is 1.33, 16/10 is 1.66 and 16/9 is 1.77
	// Set the correct screen format
	if(this->m_configurations[configurationType].m_resolution->m_screenFormat == SCREEN_FORMAT_STANDARD)
	{
		this->m_configurations[configurationType].m_frustumAngle = (float)GetPrivateProfileInt("engine", "fov_wide", INI_DEFAULT_FRUSTUM_ANGLE_WIDE, INI_FILENAME);
		if(this->m_configurations[configurationType].m_frustumAngle < 0 || this->m_configurations[configurationType].m_frustumAngle > 90)
			this->m_configurations[configurationType].m_frustumAngle = INI_DEFAULT_FRUSTUM_ANGLE_WIDE;
	}
	else
	{
		this->m_configurations[configurationType].m_frustumAngle = (float)GetPrivateProfileInt("engine", "fov_standard", INI_DEFAULT_FRUSTUM_ANGLE_STANDARD, INI_FILENAME);
		if(this->m_configurations[configurationType].m_frustumAngle < 0 || this->m_configurations[configurationType].m_frustumAngle > 90)
			this->m_configurations[configurationType].m_frustumAngle = INI_DEFAULT_FRUSTUM_ANGLE_STANDARD;
	}
}

/**
* Set the anisotropic filtering
* @param configurationType The configuration type
* @param anisotropic The anisotropic filtering
*/
void COptionManager::SetAnisotropicFiltering(const int configurationType, const int anisotropic)
{
	this->m_configurations[configurationType].m_anisotropicFiltering = anisotropic;
}

/**
* Set the maximum number of shadow sources
* @param configurationType The configuration type
* @param maxShadowSources The maximum number of shadow sources
*/
void COptionManager::SetMaxShadowSources(const int configurationType, const int maxShadowSources)
{
	this->m_configurations[configurationType].m_maxShadowSources = maxShadowSources;
}

/**
* Set the language
* @param configurationType The configuration type
* @param langage The language
*/
void COptionManager::SetLangage(const int configurationType, const int langage)
{
	this->m_configurations[configurationType].m_langage = langage;
}

/**
* Set the view distance
* @param configurationType The configuration type
* @param viewDistance The view distance
*/
void COptionManager::SetViewDistance(const int configurationType, const int viewDistance)
{
	this->m_configurations[configurationType].m_viewDistance = (float)viewDistance;
}

/**
* Set the fullscreen
* @param configurationType The configuration type
* @param fullScreen The fullscreen
*/
void COptionManager::SetFullScreen(const int configurationType, const int fullScreen)
{
	this->m_configurations[configurationType].m_fullscreen = fullScreen;
}

/**
* Set the vertical synchronisation
* @param configurationType The configuration type
* @param verticalSynchronisation The vertical synchronisation
*/
void COptionManager::SetVerticalSynchronization(const int configurationType, const int verticalSynchronisation)
{
	this->m_configurations[configurationType].m_verticalSynchronization = verticalSynchronisation;
}

/**
* Set the display of fps
* @param configurationType The configuration type
* @param fpsEnabled The display of fps
*/
void COptionManager::SetFPSEnabled(const int configurationType, const int fpsEnabled)
{
	this->m_configurations[configurationType].m_showFps = fpsEnabled;
}

/**
* Set the deferred shading
* @param configurationType The configuration type
* @param deferredShading The deferred shading
*/
void COptionManager::SetDeferredRendering(const int configurationType, const int deferredShading)
{
	this->m_configurations[configurationType].m_deferredRendering = deferredShading;
}

/**
* Set the music volume
* @param configurationType The configuration type
* @param musicVolume The music volume
*/
void COptionManager::SetMusicVolume(const int configurationType, const float musicVolume)
{
	this->m_configurations[configurationType].m_musicVolume = musicVolume;
}

/**
* Set the effects volume
* @param configurationType The configuration type
* @param effectsVolume The effects volume
*/
void COptionManager::SetEffectsVolume(const int configurationType, const float effectsVolume)
{
	this->m_configurations[configurationType].m_effectsVolume = effectsVolume;
}

/**
* Set the texture quality
* @param configurationType The configuration type
* @param textureQuality The texture quality
*/
void COptionManager::SetTextureQuality(const int configurationType, const int textureQuality)
{
	this->m_configurations[configurationType].m_textureQuality = textureQuality;
}

/**
* Get the fullscreen state
* @param configurationType The configuration type
* @return If the fullscreen is activated
*/
const bool COptionManager::IsFullScreen(const int configurationType) const
{
	return this->m_configurations[configurationType].m_fullscreen == 1;
}

/**
* Check if the vertical sync is enabled
* @param configurationType The configuration type
* @return The result
*/
const bool COptionManager::IsVerticalSynchronizationForced(const int configurationType) const
{
	return this->m_configurations[configurationType].m_verticalSynchronization == 1;
}

/**
* Check if the deferred shading is enabled
* @param configurationType The configuration type
* @return The result
*/
const bool COptionManager::IsDeferredRenderingEnabled(const int configurationType) const
{
	return this->m_configurations[configurationType].m_deferredRendering == 1;
}

/**
* Check if the fps is enabled
* @param configurationType The configuration type
* @return The result
*/
const bool COptionManager::IsFPSEnabled(const int configurationType) const
{
	return this->m_configurations[configurationType].m_showFps == 1;
}

/**
* Check if the debug mode is enabled
* @param configurationType The configuration type
* @return The result
*/
const bool COptionManager::IsDebugModeEnabled(const int configurationType) const
{
	return this->m_configurations[configurationType].m_debugMode == 1;
}

/**
* Get the resolution id
* @param configurationType The configuration type
* @return The resolution id
*/
const int COptionManager::GetResolutionId(const int configurationType) const
{
	return this->m_configurations[configurationType].m_resolutionId;
}

/**
* Get the y screen resolution
* @param configurationType The configuration type
* @return The y screen resolution
*/
const int COptionManager::GetHeight(const int configurationType) const
{
	return this->m_configurations[configurationType].m_resolution->m_height;
}

/**
* Get the x screen resolution
* @param configurationType The configuration type
* @return The x screen resolution
*/
const int COptionManager::GetWidth(const int configurationType) const
{
	return this->m_configurations[configurationType].m_resolution->m_width;
}

/**
* Get the screen format
* @param configurationType The configuration type
* @return The screen format
*/
const int COptionManager::GetScreenFormat(const int configurationType) const
{
	return this->m_configurations[configurationType].m_resolution->m_screenFormat;
}

/**
* Get the anisotropic filtering value
* @param configurationType The configuration type
* @return The anisotropic filtering value
*/
const int COptionManager::GetAnisotropicFiltering(const int configurationType) const
{
	return this->m_configurations[configurationType].m_anisotropicFiltering;
}

/**
* Get the max simultaneous shadow sources
* @param configurationType The configuration type
* @return The maximum number of simultaneous shadow sources
*/
const int COptionManager::GetMaxShadowSources(const int configurationType) const
{
	return this->m_configurations[configurationType].m_maxShadowSources;
}

/**
* Get the font texture size
* @param configurationType The configuration type
* @return The font texture size
*/
const int COptionManager::GetFontTextureSize(const int configurationType) const
{
	return this->m_configurations[configurationType].m_fontTextureSize[this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality];
}

/**
* Get choosen langage number
* @param configurationType The configuration type
* @return The choosen langage number
*/
const int COptionManager::GetLangage(const int configurationType) const
{
	return this->m_configurations[configurationType].m_langage;
}

/**
* Get the texture quality
* @param configurationType The configuration type
* @return The texture quality
*/
const int COptionManager::GetTextureQuality(const int configurationType) const
{
	return this->m_configurations[configurationType].m_textureQuality;
}

/**
* Get the music volume
* @param configurationType The configuration type
* @return The music volume
*/
const float COptionManager::GetMusicVolume(const int configurationType) const
{
	return this->m_configurations[configurationType].m_musicVolume;
}

/**
* Get the effects volume
* @param configurationType The configuration type
* @return The effects volume
*/
const float COptionManager::GetEffectsVolume(const int configurationType) const
{
	return this->m_configurations[configurationType].m_effectsVolume;
}

/**
* Get the screen ratio
* @param configurationType The configuration type
* @return The screen ratio
*/
const float COptionManager::GetScreenRatio(const int configurationType) const
{
	return this->m_configurations[configurationType].m_resolution->m_screenRatio;
}

/**
* Get the view distance
* @param configurationType The configuration type
* @return The view distance
*/
const float COptionManager::GetViewDistance(const int configurationType) const
{
	return this->m_configurations[configurationType].m_viewDistance;
}

/**
* Get the frustum angle
* @param configurationType The configuration type
* @return The frustum angle
*/
const float COptionManager::GetFrustumAngle(const int configurationType) const
{
	return this->m_configurations[configurationType].m_frustumAngle;
}

/**
* Get the texture path depending on the quality
* @param configurationType The configuration type
* @return The current texture path
*/
const std::string COptionManager::GetTexturePath(const int configurationType) const
{
	return this->m_configurations[configurationType].m_texturePath[this->m_configurations[OPTIONMANAGER_CONFIGURATION_ACTIVE].m_textureQuality];
}
