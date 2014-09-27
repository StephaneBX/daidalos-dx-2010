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
#include "../Engine/CMod.h"
#include "../Engine/CTokenizer.h"
#include "../Misc/Global.h"

/**
* Constructor
* @param directory The mod directory
*/
CMod::CMod(const std::string directory)
{
	this->m_directory = directory;
	this->m_playerStartingHealth = MOD_DEFAULT_PLAYER_STARTING_HEALTH;
	this->m_playerStartingArmor = MOD_DEFAULT_PLAYER_STARTING_ARMOR;
	LoadModConfiguration();
}

/**
* Destructor
*/
CMod::~CMod(){}

/**
* Load the configuration file
*/
void CMod::LoadModConfiguration()
{
	std::ostringstream stream;
	stream << DIRECTORY_MODS << this->m_directory << "/mod.conf";

	int configurationState = MOD_CONFIGURATION_STATE_NONE;

	CTokenizer tokenizer(stream.str());
	if(tokenizer.m_tokens.size() > 0)
	{
		// File not empty
		tokenizer.Tokenize(" ");
		for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ; )
		{
			switch(configurationState)
			{
			case MOD_CONFIGURATION_STATE_NONE:
				if(tokenizer.m_tokens.at(i) == "configuration_levels")
				{
					configurationState = MOD_CONFIGURATION_STATE_LEVELS_DECLARATIONS;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "configuration_player")
						configurationState = MOD_CONFIGURATION_STATE_PLAYER_DECLARATIONS;
				}
				i++;
				break;

			case MOD_CONFIGURATION_STATE_LEVELS_DECLARATIONS:
				if(tokenizer.m_tokens.at(i) == "levelName")
				{
					this->m_levels.push_back(tokenizer.m_tokens.at(i + 1));
					i += 2;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "end")
					{
						configurationState = MOD_CONFIGURATION_STATE_NONE;
						i++;
					}
					else
						i++;
				}
				break;

			case MOD_CONFIGURATION_STATE_PLAYER_DECLARATIONS:
				if(tokenizer.m_tokens.at(i) == "startHealth")
				{
					this->m_playerStartingHealth = atoi(tokenizer.m_tokens.at(i + 1).c_str());
					i += 2;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "startArmor")
					{
						this->m_playerStartingArmor = atoi(tokenizer.m_tokens.at(i + 1).c_str());
						i += 2;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "end")
						{
							configurationState = MOD_CONFIGURATION_STATE_NONE;
							i++;
						}
						else
							i++;
					}
				}
				break;
			}
		}
	}
}

/**
* Get the number of levels
* @return the number of levels
*/
const int CMod::GetNumberOfLevels() const
{
	return this->m_levels.size();
}

/**
* Get the player starting health
* @return The player starting health
*/
const int CMod::GetPlayerStartingHealth() const
{
	return this->m_playerStartingHealth;
}

/**
* Get the player starting armor
* @return The player starting armor
*/
const int CMod::GetPlayerStartingArmor() const
{
	return this->m_playerStartingArmor;
}

/**
* Get the mod directory
* @return The mod directory
*/
const std::string CMod::GetDirectory() const
{
	return this->m_directory;
}

/**
* Get the level filename of a corresponding id
* @param id The level id
* @return The level filename
*/
const std::string CMod::GetLevel(const int id)
{
	return this->m_levels.at(id);
}
