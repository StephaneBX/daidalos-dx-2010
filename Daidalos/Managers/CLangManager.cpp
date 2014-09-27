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
#include <fstream>
#include "../Misc/Global.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CLangManager.h"

/**
* Constructor
*/
CLangManager::CLangManager(){}

/**
* Destructor
*/
CLangManager::~CLangManager()
{
	this->m_langDatabase.clear();
}

/**
* Read the lang directory and fill the filename list
* @param modDirectory The mod directory
*/
void CLangManager::LoadLangFileList(std::string modDirectory)
{
	std::ostringstream stringstream;
	stringstream << modDirectory << DIRECTORY_LANGAGES << "*.*";

	HANDLE hfind;
	WIN32_FIND_DATA wfd;
	hfind = FindFirstFile(stringstream.str().c_str(), &wfd);
	do
	{
		if (wfd.cFileName[0] != '.' && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			std::string tmpString = wfd.cFileName;
			size_t found = tmpString.rfind(".lang");
			if(found == (tmpString.length() - 5))
			{
				stringstream.str("");
				stringstream << modDirectory << DIRECTORY_LANGAGES << tmpString;
				tmpString = stringstream.str();
				this->m_langFilename.push_back(tmpString);
			}
		}
	}
	while(FindNextFile(hfind, &wfd));
	FindClose(hfind);

	// If no langage files is found, search in the default directory
	if(this->m_langFilename.size() == 0)
	{
		stringstream.str("");
		stringstream << DIRECTORY_LANGAGES << "*.*";

		HANDLE hfind;
		WIN32_FIND_DATA wfd;
		hfind = FindFirstFile(stringstream.str().c_str(), &wfd);
		do
		{
			if (wfd.cFileName[0] != '.' && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				std::string tmpString = wfd.cFileName;
				size_t found = tmpString.rfind(".lang");
				if(found == (tmpString.length() - 5))
				{
					stringstream.str("");
					stringstream << DIRECTORY_LANGAGES << tmpString;
					tmpString = stringstream.str();
					this->m_langFilename.push_back(tmpString);
				}
			}
		}
		while(FindNextFile(hfind, &wfd));
		FindClose(hfind);
	}
}

/**
* Reload language information into the Lang Database
*/
void CLangManager::ReloadLangDatabase(std::string modDirectory)
{
	this->m_langDatabase.clear();

	std::string choosenLangage;

	if((int)this->m_langFilename.size() > 0)
	{
		// Pick the correct lang file
		if(COptionManager::GetInstance()->GetLangage(OPTIONMANAGER_CONFIGURATION_ACTIVE) >= 0 && COptionManager::GetInstance()->GetLangage(OPTIONMANAGER_CONFIGURATION_ACTIVE) < (int)this->m_langFilename.size())
		{
			choosenLangage = this->m_langFilename[COptionManager::GetInstance()->GetLangage(OPTIONMANAGER_CONFIGURATION_ACTIVE)];
		}
		else
		{
			// Use the default langage
			choosenLangage = this->m_langFilename[0];
		}

		// Open the language file
		std::ifstream file;
		file.open(choosenLangage.c_str(), std::ios::in);

		char tmp[LANGMANAGER_MAX_CHAR_PER_LINE];

		// Read file
		if(file)
		{
			while(!file.eof())
			{
				file.getline(tmp, LANGMANAGER_MAX_CHAR_PER_LINE);
				this->m_langDatabase.push_back(tmp);
			}
			file.close();
		}
	}
}

/**
* Get the number of lang files
* @return The number of lang files
*/
const int CLangManager::GetNumberOfLang() const
{
	return (int)this->m_langFilename.size();
}

/**
* Get the text choosen
* @return Text
*/
const std::string CLangManager::GetText(const unsigned int v)
{
	std::string ret;
	if(v >= 0 && v < this->m_langDatabase.size())
		ret = this->m_langDatabase[v];
	else
		ret = " ";
	return ret;
}
