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

#include <stdlib.h>
#include "../../../Engine/Entities/Sound/CMusicSoundEntity.h"

/**
* Constructor
*/
CMusicSoundEntity::CMusicSoundEntity()
{
	this->m_cBuffer.Dat = NULL;
	this->m_playing = false;
}

/**
* Destructor
*/
CMusicSoundEntity::~CMusicSoundEntity()
{
	if(this->m_cBuffer.Dat != NULL)
		delete this->m_cBuffer.Dat;
}

/**
* Update the position and play the sound
*/
void CMusicSoundEntity::Play(const float x, const float y, const float z)
{
	SetRelativeToListener(false);
	UpdatePosition(x, y, z);
	alSourcePlay(this->m_source);
	this->m_playing = true;
}

/**
* Play the music
*/
void CMusicSoundEntity::Play()
{
	alSourcePlay(this->m_source);
	this->m_playing = true;
}

/**
* Stop the music if its playing or paused
*/
void CMusicSoundEntity::Stop(){}

/**
* Update the music
*/
void CMusicSoundEntity::Update(){}

/**
* Check if the music is playing
* @return True if the music is playing
*/
bool CMusicSoundEntity::IsPlaying()
{
	return this->m_playing;
}
