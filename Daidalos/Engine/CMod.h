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

#ifndef __CMOD_H
#define __CMOD_H

#define MOD_DEFAULT_PLAYER_STARTING_HEALTH 100
#define MOD_DEFAULT_PLAYER_STARTING_ARMOR 25

#include <string>
#include <vector>

enum modConfigurationState
{
	MOD_CONFIGURATION_STATE_NONE,
	MOD_CONFIGURATION_STATE_LEVELS_DECLARATIONS,
	MOD_CONFIGURATION_STATE_PLAYER_DECLARATIONS
};

/**
* CMod class.
* Manages a mod.
* It's composed of a list of levels and a starting Health and Armor.
* If the mod directory contains ressources (textures, sprites, models etc), they override the default ones.
*/
class CMod
{
private:
	std::string m_directory;
	std::vector<std::string> m_levels;
	int m_playerStartingHealth;
	int m_playerStartingArmor;

public:
	CMod(const std::string m_directory);
	~CMod();
	const int GetNumberOfLevels() const;
	const int GetPlayerStartingHealth() const;
	const int GetPlayerStartingArmor() const;
	const std::string GetDirectory() const;
	const std::string GetLevel(const int id);

private:
	void LoadModConfiguration();
};

#endif
