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

#include "../Misc/Global.h"
#include "../Engine/Entities/Sound/CWaveEffectEntity.h"
#include "../Engine/Entities/Sound/CWaveMusicEntity.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CLogManager.h"
#include "../Managers/CSoundManager.h"

/**
* Constructor
*/
CSoundManager::CSoundManager(){}

/**
* Destructor
*/
CSoundManager::~CSoundManager()
{
	FreeDatabase();
	alcDestroyContext(this->m_context);
	alcCloseDevice(this->m_soundDevice);
}

/**
* Initialize the manager
*/
void CSoundManager::Initialize()
{
	this->m_logM = CLogManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();

	this->m_soundDevice = alcOpenDevice(NULL);
	this->m_context = NULL;
	if(this->m_soundDevice) 
	{
		this->m_context = alcCreateContext(this->m_soundDevice, NULL);
		if(this->m_context != NULL)
		{
			alcMakeContextCurrent(this->m_context);
			this->m_soundEnabled = true;
		}
		else
		{
			this->m_logM->GetStream() << "Sound error: Unable to create the sound context.\n";
			this->m_logM->Write();
			this->m_soundEnabled = false;
		}
	}
	else
		this->m_soundEnabled = false;
}

/**
* Update the listener position
* @param x The listener x coordinate
* @param y The listener y coordinate
* @param z The listener z coordinate
*/
void CSoundManager::UpdateListenerPosition(const float x, const float y, const float z)
{
	alListener3f(AL_POSITION, x, y, z);
}

/**
* Update the listener direction
* @param value The direction array
*/
void CSoundManager::UpdateListenerDirection(const float value[6])
{
	alListenerfv(AL_ORIENTATION, value);
}

/**
* Pause all the effects
*/
void CSoundManager::PauseEffects()
{
	for(std::map<std::string, CEffectSoundEntity *>::iterator it = this->m_effects.begin() ; it != this->m_effects.end() ; it++)
		if(it->second->IsPlaying())
			it->second->Pause();
}

/**
* Resume all the sounds
*/
void CSoundManager::ResumeEffects()
{
	for(std::map<std::string, CEffectSoundEntity *>::iterator it = this->m_effects.begin() ; it != this->m_effects.end() ; it++)
		if(it->second->IsPaused())
			it->second->Resume();
}

/**
* Resume all the sounds
*/
void CSoundManager::StopEffects()
{
	for(std::map<std::string, CEffectSoundEntity *>::iterator it = this->m_effects.begin() ; it != this->m_effects.end() ; it++)
		if(it->second->IsPlaying())
			it->second->Stop();
}

/**
* Load a music
* @param fileName The file name
* @param modDirectory The mod directory
* @return The music
*/
CMusicSoundEntity * CSoundManager::LoadMusic(const std::string fileName, std::string modDirectory)
{
	std::string finalFileName;
	std::ifstream file;
	std::ostringstream stream;

	stream << DIRECTORY_SOUND_MUSIC << fileName;
	finalFileName = stream.str();

	stream << modDirectory << DIRECTORY_SOUND_MUSIC << fileName;
	if(FileExists(stream.str()))
		finalFileName = stream.str();

	if(!MusicExists(finalFileName))
	{
		if(finalFileName.rfind(".wav") != std::string::npos)
		{
			this->m_musics[finalFileName] = new CWaveMusicEntity(finalFileName);
			this->m_musics[finalFileName]->SetVolume(this->m_optionM->GetMusicVolume(OPTIONMANAGER_CONFIGURATION_ACTIVE));
			this->m_musics[finalFileName]->SetRollOffFactor(SOUND_ROLL_OFF_FACTOR);
		}
	}
	return this->m_musics[finalFileName];
}

/**
* Load an effect
* @param fileName The file name
* @param modDirectory The mod directory
* @return The sound effect
*/
CEffectSoundEntity * CSoundManager::LoadEffect(const std::string fileName, std::string modDirectory)
{
	std::string finalFileName;
	std::ifstream file;
	std::ostringstream stream;

	stream << DIRECTORY_SOUND_EFFECT << fileName;
	finalFileName = stream.str();

	stream.str("");
	stream << modDirectory << DIRECTORY_SOUND_EFFECT << fileName;
	if(FileExists(stream.str()))
		finalFileName = stream.str();

	if(!EffectExists(finalFileName))
	{
		if(finalFileName.rfind(".wav") != std::string::npos)
		{
			this->m_effects[finalFileName] = new CWaveEffectEntity(finalFileName);
			this->m_effects[finalFileName]->SetVolume(this->m_optionM->GetEffectsVolume(OPTIONMANAGER_CONFIGURATION_ACTIVE));
			this->m_effects[finalFileName]->SetRollOffFactor(SOUND_ROLL_OFF_FACTOR);
		}
	}
	return this->m_effects[finalFileName];
}

/**
* Free the sound database
*/
void CSoundManager::FreeDatabase()
{
	for(std::map<std::string, CEffectSoundEntity *>::iterator it = this->m_effects.begin() ; it != this->m_effects.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_effects.clear();
	
	for(std::map<std::string, CMusicSoundEntity *>::iterator it = this->m_musics.begin() ; it != this->m_musics.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_musics.clear();
}

/**
* Check if an effect already exists in the database
* @param fileName The effect file name
* @return True if the effect is found in the database, False otherwise
*/
bool CSoundManager::EffectExists(const std::string fileName)
{
	bool ret = false;
	for(std::map<std::string, CEffectSoundEntity *>::iterator it = this->m_effects.begin() ; it != this->m_effects.end() ; it++)
		if(it->first == fileName)
			ret = true;
	return ret;
}

/**
* Check if a music already exists in the database
* @param fileName The music file name
* @return True if the music is found in the database, False otherwise
*/
bool CSoundManager::MusicExists(const std::string fileName)
{
	bool ret = false;
	for(std::map<std::string, CMusicSoundEntity *>::iterator it = this->m_musics.begin() ; it != this->m_musics.end() ; it++)
		if(it->first == fileName)
			ret = true;
	return ret;
}
