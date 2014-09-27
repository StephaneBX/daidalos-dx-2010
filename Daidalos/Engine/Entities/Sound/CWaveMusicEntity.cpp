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

#include "../../../Misc/Global.h"
#include "../../../Engine/Entities/Sound/CWaveMusicEntity.h"

/**
* Constructor
* @param fileName The wave file name
*/
CWaveMusicEntity::CWaveMusicEntity(std::string fileName)
{
	this->m_waveLoader = new CWaves();

	alDeleteSources(1, &this->m_source);
	alDeleteBuffers(MUSICSOUNDENTITY_BUFFER_SIZE, this->m_buffers);
	if(this->m_cBuffer.Dat != NULL)
		delete this->m_cBuffer.Dat;

	this->m_waveLoader->OpenWaveFile(fileName.c_str(), &this->m_wID);
	GetWaveInfo(this->m_waveLoader, this->m_wID, &this->m_cBuffer.Fmt, &this->m_cBuffer.Siz, &this->m_cBuffer.Frq);
	this->m_waveLoader->GetWaveFormatExHeader(this->m_wID, &this->m_wFtex);
	this->m_cBuffer.Siz = this->m_wFtex.nAvgBytesPerSec / 4;
	this->m_cBuffer.Siz -= this->m_cBuffer.Siz % this->m_wFtex.nBlockAlign;
	this->m_cBuffer.Dat = new ALchar[this->m_cBuffer.Siz];
	alGenBuffers(MUSICSOUNDENTITY_BUFFER_SIZE, this->m_buffers);
	alGenSources(1, &this->m_source);
	this->m_waveLoader->SetWaveDataOffset(this->m_wID, 0);

	for(int i = 0 ; i < MUSICSOUNDENTITY_BUFFER_SIZE ; i++)
		this->m_read = PushBuffer(this->m_buffers[i]);

	SetRelativeToListener(true);
}

/**
* Destructor
*/
CWaveMusicEntity::~CWaveMusicEntity()
{
	SAFE_DELETE(this->m_waveLoader);
}

/**
* Stop the music
*/
void CWaveMusicEntity::Stop()
{
	if(this->m_playing)
	{
		int queued;
		alSourceStop(this->m_source);
		this->m_playing = false;
		this->m_waveLoader->SetWaveDataOffset(this->m_wID, 0);
		alGetSourcei(this->m_source, AL_BUFFERS_QUEUED, &queued);
	    
		while(queued--)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(this->m_source, 1, &buffer);
		}
		for(int i = 0 ; i < MUSICSOUNDENTITY_BUFFER_SIZE ; i++)
			this->m_read = PushBuffer(this->m_buffers[i]);
	}
}

/**
* Update the music
*/
void CWaveMusicEntity::Update()
{
	ALint iBuff = 0;
	ALuint buffId;

	alGetSourcei(this->m_source, AL_BUFFERS_PROCESSED, &iBuff);			
	for(int i = 0 ; i < iBuff ; i++) 
	{
		alSourceUnqueueBuffers(this->m_source, 1, &buffId);
		this->m_read = PushBuffer(buffId);
	}
	if(this->m_read == 0)
		this->m_waveLoader->SetWaveDataOffset(this->m_wID, 0);
}

/**
* Push the buffer in the current data
* @param buff The next buffer to read
*/
ALuint CWaveMusicEntity::PushBuffer(ALuint buff)
{
	unsigned long read = 0;
	WAVERESULT h;
	h = this->m_waveLoader->ReadWaveData(this->m_wID, this->m_cBuffer.Dat, this->m_cBuffer.Siz, &read);
	if(SUCCEEDED(h))
	{
		alBufferData(buff, this->m_cBuffer.Fmt, this->m_cBuffer.Dat, read, this->m_cBuffer.Frq);			
		alSourceQueueBuffers(this->m_source, 1, &buff);
	}
	return read;
}
