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
#include <d3d9.h>
#include <d3dx9.h>
#include "../Managers/CKeyboardManager.h"
#include "../Managers/CLogManager.h"
#include "../Managers/COptionManager.h"
#include "../Misc/Global.h"


/**
* Constructor
*/
CKeyboardManager::CKeyboardManager(){}

/**
* Destructor
*/
CKeyboardManager::~CKeyboardManager()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		SAFE_DELETE(this->m_keys[i]);
	SAFE_DELETE_ARRAY(this->m_keys);

	Release();
}

/**
* Initialize the manager
*/
void CKeyboardManager::Initialize()
{
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();

	_VERIFY(this->m_deviceM->GetInput()->CreateDevice(GUID_SysKeyboard, &this->m_keyboard, NULL), "Keyboard Device Creation");
	_VERIFY(this->m_keyboard->SetDataFormat(&c_dfDIKeyboard), "Keyboard Data Format");
	_VERIFY(this->m_keyboard->SetCooperativeLevel(this->m_deviceM->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE), "Keyboard Cooperation Level");

	this->m_keyboard->Acquire();

	this->m_keys = new int*[ACTION_KEY_SIZE];	

	// Get the keys and initialize them to the state UP
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		this->m_keys[i] = new int[KEYBOARD_KEY_INDEX_SIZE];

		this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] = GetPrivateProfileInt("keyboard", COptionManager::m_actionKeyName[i].c_str(), ACTION_KEY_DISABLED, INI_FILENAME);
		if(this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] < ACTION_KEY_DISABLED || this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] >= sizeof(this->m_keysBuffer))
			this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] = ACTION_KEY_DISABLED;

		this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_UP;
	}

	//Initialization of keys that are linked to another
	this->m_keys[ACTION_KEY_UP_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_DOWN_MENU;
	this->m_keys[ACTION_KEY_DOWN_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_UP_MENU;
	this->m_keys[ACTION_KEY_LEFT_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_RIGHT_MENU;
	this->m_keys[ACTION_KEY_RIGHT_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_LEFT_MENU;
	this->m_keys[ACTION_KEY_ESCAPE_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_ENTER_MENU][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_FORWARD][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_BACKWARD;
	this->m_keys[ACTION_KEY_BACKWARD][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_FORWARD;
	this->m_keys[ACTION_KEY_STRAFE_LEFT][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_STRAFE_RIGHT;
	this->m_keys[ACTION_KEY_STRAFE_RIGHT][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_STRAFE_LEFT;
	this->m_keys[ACTION_KEY_TURN_LEFT][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_TURN_RIGHT;
	this->m_keys[ACTION_KEY_TURN_RIGHT][KEYBOARD_KEY_INDEX_LINKED_KEY] = ACTION_KEY_TURN_LEFT;
	this->m_keys[ACTION_KEY_LOOK_UP][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_LOOK_DOWN][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_FLASHLIGHT][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_JUMP][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
	this->m_keys[ACTION_KEY_CROUCH][KEYBOARD_KEY_INDEX_LINKED_KEY] = -1;
}

/**
* Change a key state
* @param key The key number
* @param state The new state
*/
void CKeyboardManager::SetKeyState(const int key, const int state)
{
	this->m_keys[key][KEYBOARD_KEY_INDEX_STATE] = state;
}

/**
* Change a key
* @param key The key number
* @param assignedKey The new assigned key
*/
void CKeyboardManager::SetKey(const int key, const int assignedKey)
{
	this->m_keys[key][KEYBOARD_KEY_INDEX_ASSIGNED] = assignedKey;
}

/**
* Save the configuration
*/
void CKeyboardManager::Save()
{
	std::ostringstream stream;
	std::string tmp;

	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
		{
			stream.str("");
			stream << this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED];
			tmp = stream.str();
			WritePrivateProfileString("keyboard", COptionManager::m_actionKeyName[i].c_str(), tmp.c_str(), INI_FILENAME);
		}
		else
			WritePrivateProfileString("keyboard", COptionManager::m_actionKeyName[i].c_str(), NULL, INI_FILENAME);
	}
}

/**
* Refresh the keyboard
*/
void CKeyboardManager::Update()
{
	HRESULT res = this->m_keyboard->GetDeviceState(sizeof(this->m_keysBuffer),(LPVOID)&this->m_keysBuffer);
	if(FAILED(res))
	{
		res = this->m_keyboard->Acquire();

		while(res == DIERR_INPUTLOST)    
			 res = this->m_keyboard->Acquire();

		if (SUCCEEDED(res))
			this->m_keyboard->GetDeviceState(sizeof(this->m_keysBuffer), (LPVOID)&this->m_keysBuffer);
	}

	//If the keyboard is acquired, we update the keys
	if(SUCCEEDED(res))
	{
		//Refresh the keys status
		for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		{
			if(this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
			{
				//Key down => we update the key variable if necessary
				if(KEYDOWN(this->m_keysBuffer, this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED]))
				{
					if(this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_UP)
					{
						this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_DOWN;
						if(this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY] > -1 && this->m_keys[this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY]][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_DOWN)
							this->m_keys[this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY]][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_DOWN_WAIT;
					}
				}
				else
				{
					//Key up => if KEYBOARD_KEY_STATE_DOWN, change to KEYBOARD_KEY_STATE_UP and change it's linked key to KEYBOARD_KEY_STATE_DOWN if necessary
					if(this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_DOWN)
					{
						this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_UP;
						if(this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY] > -1 && this->m_keys[this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY]][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_DOWN_WAIT)
							this->m_keys[this->m_keys[i][KEYBOARD_KEY_INDEX_LINKED_KEY]][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_DOWN;
					}
					else
					{
						//If a frozen key is not down, we set it to up
						if(!KEYDOWN(this->m_keysBuffer, this->m_keys[i][KEYBOARD_KEY_INDEX_ASSIGNED]) && this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_FROZEN)
							this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_UP;
						else
							//Key up => if KEYBOARD_KEY_STATE_DOWN_WAIT, change to KEYBOARD_KEY_STATE_UP
							if(this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_DOWN_WAIT)
								this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_UP;
					}
				}
			}
		}
	}
}

/**
* Restore the keyboard
*/
void CKeyboardManager::Restore()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_UP;
}

/**
* Freeze the keyboard
*/
void CKeyboardManager::FreezeKeys()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		this->m_keys[i][KEYBOARD_KEY_INDEX_STATE] = KEYBOARD_KEY_STATE_FROZEN;
}

/**
* Release the keyboard
*/
void CKeyboardManager::Release()
{
	if(this->m_keyboard != NULL) 
	{ 
		this->m_keyboard->Unacquire(); 
		SAFE_RELEASE(this->m_keyboard);
	}
}

/**
* Check if a key is down
* @param key The key id
* @return If the key is down
*/
const bool CKeyboardManager::IsKeyDown(const int key) const
{
	return (this->m_keys[key][KEYBOARD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED && this->m_keys[key][KEYBOARD_KEY_INDEX_STATE] == KEYBOARD_KEY_STATE_DOWN);
}

/**
* Get the first pushed key
* @return The first pushed key
*/
const int CKeyboardManager::GetFirstKeyDown()
{
	bool found = false;
	int ret = -1;
	for(int i = 0 ; i < 256 && !found; i++)
	{
		if(KEYDOWN(this->m_keysBuffer, i))
		{
			ret = i;
			found = true;
		}
	}
	return ret;
}

/**
* Get a key associated to a key id
* @param key The key id
* @return The associated key
*/
const int CKeyboardManager::GetKey(const int key)
{
	return this->m_keys[key][KEYBOARD_KEY_INDEX_ASSIGNED];
}

/**
* Get a key name
* @param key The key id
* @return The key name
*/
const std::string CKeyboardManager::GetKeyName(const int key)
{
	DWORD keyId = key;
	DIPROPSTRING keyname;

	keyname.diph.dwSize = sizeof(DIPROPSTRING); 
	keyname.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	keyname.diph.dwObj = keyId;
	keyname.diph.dwHow = DIPH_BYOFFSET; 
	this->m_keyboard->GetProperty(DIPROP_KEYNAME, &keyname.diph);

	std::wstring wtest = keyname.wsz;
	std::string tmpString;
	tmpString.assign(wtest.begin(), wtest.end());

	return tmpString;
}
