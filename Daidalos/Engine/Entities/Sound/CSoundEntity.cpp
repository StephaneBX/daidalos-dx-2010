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

#include "../../../Engine/Entities/Sound/CSoundEntity.h"

/**
* Constructor
*/
CSoundEntity::CSoundEntity()
{
	this->m_source = 0;
}

/**
* Destructor
*/
CSoundEntity::~CSoundEntity()
{
	alDeleteSources(1, &this->m_source);
}

/**
* Set the volume
* @param volume The new volume
*/
void CSoundEntity::SetVolume(const float volume)
{
	alSourcef(this->m_source, AL_GAIN, volume);
}

/**
* Change the sound policy
* If the sound is relative to the listener, its values are relative
* If the sound is not relative to the listener, its values are absolute
* @param value The value
*/
void CSoundEntity::SetRelativeToListener(const bool value)
{
	if(value)
		alSourcei(this->m_source, AL_SOURCE_RELATIVE, AL_TRUE);
	else
		alSourcei(this->m_source, AL_SOURCE_RELATIVE, AL_FALSE);
}

/**
* Change the sound policy
* If the sound if loopable, it will be played indefinitly
* @param value The new value
*/
void CSoundEntity::SetLoopable(const bool value)
{
	if(value)
		alSourcei(this->m_source, AL_LOOPING, AL_TRUE);
	else
		alSourcei(this->m_source, AL_LOOPING, AL_FALSE);
}

/**
* Change the maximum audible distance
* @param distance The new distance
*/
void CSoundEntity::SetMaxAudibleDistance(const int distance)
{
	alSourcei(this->m_source, AL_MAX_DISTANCE, distance);
}

/**
* Change the roll off factor
* @param facor The new roll off factor
*/
void CSoundEntity::SetRollOffFactor(const float facor)
{
	alSourcef(this->m_source, AL_ROLLOFF_FACTOR, facor);
}

/**
* Update the position and play the sound
*/
void CSoundEntity::Play(const float x, const float y, const float z)
{
	UpdatePosition(x, y, z);
	alSourcePlay(this->m_source);
}

/**
* Play the sound
*/
void CSoundEntity::Play()
{
	alSourcePlay(this->m_source);
}

/**
* Pause the sound
*/
void CSoundEntity::Pause()
{
	ALint status;
	alGetSourcei(this->m_source, AL_SOURCE_STATE, &status);
	if(status == AL_PLAYING)
		alSourcePause(this->m_source);
}

/**
* Resume the sound
*/
void CSoundEntity::Resume()
{
	ALint status;
	alGetSourcei(this->m_source, AL_SOURCE_STATE, &status);
	if(status == AL_PAUSED)
		alSourcePlay(this->m_source);
}

/**
* Stop the sound
*/
void CSoundEntity::Stop()
{
	ALint status;
	alGetSourcei(this->m_source, AL_SOURCE_STATE, &status);
	if(status == AL_PAUSED || status == AL_PLAYING)
		alSourceStop(this->m_source);
}

/**
* Check if the effect is playing
* @return True if the effect is playing
*/
bool CSoundEntity::IsPlaying()
{
	return GetState() == AL_PLAYING;
}

/**
* Check if the effect is paused
* @return True if the effect is paused
*/
bool CSoundEntity::IsPaused()
{
	return GetState() == AL_PAUSED;
}

/**
* Check if the effect is stopped
* @return True if the effect is stopped
*/
bool CSoundEntity::IsStopped()
{
	return (!IsPlaying() && !IsPaused());
}

/**
* Update the position
* @param x The listener x direction
* @param y The listener y direction
* @param z The listener z direction
*/
void CSoundEntity::UpdatePosition(const float x, const float y, const float z)
{
	alSource3f(this->m_source, AL_POSITION, x, y, z);
}

/**
* Get the state
* @return The state
*/
ALint CSoundEntity::GetState()
{
	ALint status;
	alGetSourcei(this->m_source, AL_SOURCE_STATE, &status);
	return status;
}
