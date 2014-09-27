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

#include <fstream>
#include "../Engine/CTokenizer.h"

/**
* Constructor
*/
CTokenizer::CTokenizer(){}

/**
* Constructor
* @param filename The file to tokenize
*/
CTokenizer::CTokenizer(const std::string filename)
{
	OpenFile(filename);
}

/**
* Constructor
* @param filename The file to tokenize
* @param filterList The filter list that contains all the accepted keywords
*/
CTokenizer::CTokenizer(const std::string filename, std::vector<std::string> * const filterList)
{
	bool valid = false;
	std::string tmpLine;
	this->m_tokens.clear();
	this->m_buffer.clear();

	std::fstream file(filename.c_str());
	if(file.is_open())
	{
		while(file.good())
		{
			std::getline(file, tmpLine);
			if(tmpLine.compare(0, 2, TOKENIZER_COMMENT) != 0)
			{
				for(unsigned int i = 0 ; i < filterList->size() && !valid ; i++)
				{
					if(tmpLine.substr(0, filterList->at(i).length()) == filterList->at(i))
						valid = true;
				}

				if(valid)
				{
					this->m_tokens.push_back(tmpLine);
					valid = false;
				}
			}
		}
		file.close();
	}
}

/**
* Destructor
*/
CTokenizer::~CTokenizer(){}

/**
* Open a file, read its content and put it in the token list
* @param filename The file to open
*/
void CTokenizer::OpenFile(const std::string filename)
{
	std::string tmpLine;
	this->m_tokens.clear();
	this->m_buffer.clear();

	std::fstream file(filename.c_str());
	while(file.good())
	{
		std::getline(file, tmpLine);
		this->m_tokens.push_back(tmpLine);
	}
	file.close();
}

/**
* Add a line to the line list
*/
void CTokenizer::AddLine(const std::string line)
{
	this->m_tokens.push_back(line);
}

/**
* Clear the tokens
*/
void CTokenizer::Clear()
{
	this->m_tokens.clear();
}

/**
* Tokenize the token list
* @param delimiter The delimiter that is used to split
* @param maxSplit The maximum split accepted for each split process
*/
bool CTokenizer::Tokenize(const std::string delimiter, const int maxSplit)
{
	bool valid = true;
	this->m_buffer.clear();
	int elementCount;

	for(unsigned int i = 0 ; i < this->m_tokens.size() && valid ; i++)
	{
		elementCount = SplitString(this->m_tokens.at(i), delimiter);
		if(maxSplit > 0 && elementCount != 1 && elementCount != maxSplit)
			valid = false;
	}

	this->m_tokens.clear();

	for(unsigned int i = 0 ; i < this->m_buffer.size() && valid ; i++)
		this->m_tokens.push_back(this->m_buffer.at(i));

	return valid ;
}

/**
* Count the number of 'token' tokens
* @param token The token to count
* @return The number of 'token' tokens found
*/
int CTokenizer::Count(const std::string token)
{
	int count = 0;
	
	for(unsigned int i = 0 ; i < this->m_tokens.size() ; i++)
	{
		if(this->m_tokens.at(i) == token)
			count++;
	}

	return count;
}

/**
* Split a token with a delimiter and push the result in the buffer
* @param token The token to split
* @param delimiter The delimiter to use
* @return The number of pushed tokens
*/
int CTokenizer::SplitString(const std::string token, const std::string delimiter)
{
	int elementCount = 1;
	unsigned int start = 0;
	unsigned int end = 0;
	unsigned int size = 0;

	while(start < token.length())
	{
		end = token.find(delimiter, start);
		if(end >= start && end < token.length())
		{
			size = end - start;
			if(size > 0)
				this->m_buffer.push_back(token.substr(start, size));
			else
				this->m_buffer.push_back("");

			start = end + 1;
			elementCount++;
		}
		else
		{
			this->m_buffer.push_back(token.substr(start, token.length() - start));
			start = token.length() + 1;
		}
	}
	return elementCount;
}
