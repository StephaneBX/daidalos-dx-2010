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
#include "../Engine/Entities/2D/CSpriteEntity.h"
#include "../Managers/CLogManager.h"
#include "../Managers/COptionManager.h"
#include "../Managers/CMouseManager.h"

/**
* Constructor
*/
CMouseManager::CMouseManager(){}

/**
* Destructor
*/
CMouseManager::~CMouseManager()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		SAFE_DELETE_ARRAY(this->m_buttons[i]);
	SAFE_DELETE_ARRAY(this->m_buttons);
	SAFE_DELETE(this->m_cursor);
	Release();
}

/**
* Initialize the manager
*/
void CMouseManager::Initialize()
{
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();

	_VERIFY(this->m_deviceM->GetInput()->CreateDevice(GUID_SysMouse, &this->m_mouse, NULL), "Mouse Device Creation");
	_VERIFY(this->m_mouse->SetDataFormat(&c_dfDIMouse), "Mouse Data Format");
	_VERIFY(this->m_mouse->SetCooperativeLevel(this->m_deviceM->GetHwnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE ), "Mouse Cooperation Level");

	this->m_mouse->Acquire();

	this->m_hasMoved = false;
	this->m_mouseDelay = 0;
	this->m_movementX = 0;
	this->m_movementY = 0;

	// Mouse options
	this->m_mouseSmoothing = GetPrivateProfileInt(MOUSE_FILE_ENTRY_NAME, MOUSE_FILE_ENTRY_SMOOTHING, INI_DEFAULT_MOUSE_SMOOTHING, INI_FILENAME);
	this->m_mouseSensibility = (float)GetPrivateProfileInt(MOUSE_FILE_ENTRY_NAME, MOUSE_FILE_ENTRY_SENSIBILITY, INI_DEFAULT_MOUSE_SENSIBILITY, INI_FILENAME);

	// Mouse options verification
	if(this->m_mouseSmoothing < 0 || this->m_mouseSmoothing > MOUSE_SMOOTHING_MAX)
		this->m_mouseSmoothing = INI_DEFAULT_MOUSE_SMOOTHING;

	if(this->m_mouseSensibility < 0 || this->m_mouseSensibility > MOUSE_SENSIBILITY_MAX)
		this->m_mouseSensibility = INI_DEFAULT_MOUSE_SENSIBILITY;
	this->m_mouseSensibility /= 100.0f;

	this->m_buttons = new int*[ACTION_KEY_SIZE];
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		this->m_buttons[i] = new int[MOUSE_BUTTON_INDEX_SIZE];

		this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] = GetPrivateProfileInt(MOUSE_FILE_ENTRY_NAME, COptionManager::m_actionKeyName[i].c_str(), ACTION_KEY_DISABLED, INI_FILENAME);;
		if(this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] < ACTION_KEY_DISABLED || this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] >= sizeof(this->m_state.rgbButtons))
			this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] = ACTION_KEY_DISABLED;

		this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] = MOUSE_BUTTON_STATE_UP;
	}

	this->m_cursor = new CSpriteEntity(MOUSE_CURSOR_FILE, D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2.0f, this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2.0f), D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1,1));
	this->m_pos[0] = this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	this->m_pos[1] = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;

	this->m_mouseMode = MOUSE_MODE_MENU;
}

/**
* Refresh the mouse and the cursor
*/
void CMouseManager::Update()
{
	HRESULT res;
	this->m_hasMoved = false;
	res = this->m_mouse->GetDeviceState(sizeof(this->m_state), (LPVOID)&this->m_state);
	if(FAILED(res))
	{
		res = this->m_mouse->Acquire();

		while(res == DIERR_INPUTLOST)    
			 res = this->m_mouse->Acquire();

		if (SUCCEEDED(res))
		{
			SetCursorPos(0,0);
			this->m_mouse->GetDeviceState(sizeof(this->m_state), (LPVOID)&this->m_state);
		}
	}
	else
		SetCursorPos(0,0);


	if(this->m_state.lX == 0 && this->m_state.lY == 0)
	{
		this->m_mouseDelay--;
		if(this->m_mouseDelay > 0)
		{
			this->m_hasMoved = true;
		}
		else
		{
			this->m_hasMoved = false;
			this->m_mouseDelay = 0;
			this->m_movementX = 0;
			this->m_movementY = 0;
		}
	}
	else
	{
		this->m_hasMoved = true;
		this->m_mouseDelay = this->m_mouseSmoothing;
		this->m_movementX = (int)this->m_state.lX;
		this->m_movementY = (int)this->m_state.lY;

		// Cursor update
		this->m_pos[0] += (int)this->m_state.lX;
		this->m_pos[1] += (int)this->m_state.lY;

		if(this->m_pos[0] < 0)
			this->m_pos[0] = 0;
		if(this->m_pos[1] < 0)
			this->m_pos[1] = 0;

		if(this->m_pos[0] > this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE))
			this->m_pos[0] = this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE);
		if(this->m_pos[1] > this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE))
			this->m_pos[1] = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE);

		this->m_cursor->SetPosition(D3DXVECTOR2((float)this->m_pos[0], (float)this->m_pos[1]));
	}

	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(KEYDOWN(this->m_state.rgbButtons, this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED]))
		{
			if(this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] == MOUSE_BUTTON_STATE_UP)
				this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] = MOUSE_BUTTON_STATE_DOWN;
		}
		else
		{
			if(this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] == MOUSE_BUTTON_STATE_DOWN || this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] == MOUSE_BUTTON_STATE_FROZEN)
				this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] = MOUSE_BUTTON_STATE_UP;
		}
	}
}

/**
* Draw the cursor
*/
void CMouseManager::ShowCursor()
{
	this->m_cursor->Render();
}

/**
* Restore the mouse device
*/
void CMouseManager::Restore()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] = MOUSE_BUTTON_STATE_UP;
}

/**
* Release the mouse device
*/
void CMouseManager::Release()
{
	if(this->m_mouse != NULL) 
	{ 
		this->m_mouse->Unacquire(); 
		SAFE_RELEASE(this->m_mouse); 
	}
}

/**
* Freeze the mouse buttons
*/
void CMouseManager::FreezeButtons()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		if(this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
			this->m_buttons[i][MOUSE_BUTTON_INDEX_STATE] = MOUSE_BUTTON_STATE_FROZEN;
}

/**
* Release the ressources
*/
void CMouseManager::ReleaseResources()
{
	this->m_cursor->GetSprite()->OnLostDevice();
}

/**
* Restore the ressources
*/
void CMouseManager::RestoreResources()
{
	this->m_cursor->GetSprite()->OnResetDevice();
	this->m_pos[0] = this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	this->m_pos[1] = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
}

/**
* Set the sensibility
* @param sensibility The new sensibility
*/
void CMouseManager::SetSensibility(const float sensibility)
{
	this->m_mouseSensibility = sensibility;
}

/**
* Set the smoothing
* @param smoothing The new smoothing
*/
void CMouseManager::SetSmoothing(const int smoothing)
{
	this->m_mouseSmoothing = smoothing;
}

/**
* Set a button
* @param button The button id
* @param assignedButton The button
*/
void CMouseManager::SetButton(const int button, const int assignedButton)
{
	this->m_buttons[button][MOUSE_BUTTON_INDEX_ASSIGNED] = assignedButton;
}

/**
* Set a button state
* @param button The button id
* @param state The button state
*/
void CMouseManager::SetButtonState(const int button, const int state)
{
	this->m_buttons[button][MOUSE_BUTTON_INDEX_STATE] = state;
}

/**
* Save the configuration
*/
void CMouseManager::Save()
{
	std::ostringstream stream;
	std::string tmp;

	// Sensibility
	stream << this->m_mouseSensibility * 100;
	tmp = stream.str();
	WritePrivateProfileString(MOUSE_FILE_ENTRY_NAME, MOUSE_FILE_ENTRY_SENSIBILITY, tmp.c_str(), INI_FILENAME);

	// Smoothing
	stream.str("");
	stream << this->m_mouseSmoothing;
	tmp = stream.str();
	WritePrivateProfileString(MOUSE_FILE_ENTRY_NAME, MOUSE_FILE_ENTRY_SMOOTHING, tmp.c_str(), INI_FILENAME);

	// Buttons
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
		{
			stream.str("");
			stream << this->m_buttons[i][MOUSE_BUTTON_INDEX_ASSIGNED];
			tmp = stream.str();
			WritePrivateProfileString(MOUSE_FILE_ENTRY_NAME, COptionManager::m_actionKeyName[i].c_str(), tmp.c_str(), INI_FILENAME);
		}
		else
			WritePrivateProfileString(MOUSE_FILE_ENTRY_NAME, COptionManager::m_actionKeyName[i].c_str(), NULL, INI_FILENAME);
	}
}

/**
* Center the mouse cursor
*/
void CMouseManager::CenterCursor()
{
	this->m_pos[0] = this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	this->m_pos[1] = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	this->m_cursor->SetPosition(D3DXVECTOR2((float)this->m_pos[0], (float)this->m_pos[1]));
}

/**
* Test if the mouse has moved since its previous known coordinate
* @return The test result
*/
const bool CMouseManager::HasMoved() const
{
	return this->m_hasMoved;
}

/**
* Check if a button is pressed
* @param button The button to test
* @return True if the button is pressed, false otherwise
*/
const bool CMouseManager::IsButtonDown(int button)
{
	return this->m_buttons[button][MOUSE_BUTTON_INDEX_STATE] == MOUSE_BUTTON_STATE_DOWN;
}

/**
* Get the x coordinate movement
* @return The x coordinate movement
*/
const int CMouseManager::GetX() const
{
	return this->m_movementX;
}

/**
* Get the y coordinate movement
* @return The y coordinate movement
*/
const int CMouseManager::GetY() const
{
	return this->m_movementY;
}

/**
* Get the x coordinate of the cursor
* @return The x coordinate movement
*/
const int CMouseManager::GetCursorX() const
{
	return this->m_pos[0];
}

/**
* Get the y coordinate of the cursor
* @return The y coordinate movement
*/
const int CMouseManager::GetCursorY() const
{
	return this->m_pos[1];
}

/**
* Get the wheel movement
* @return The wheel coordinate movement
*/
const int CMouseManager::GetWheelMovement() const
{
	return (int)this->m_state.lZ;
}

/**
* Get the smoothing
* @return The smoothing
*/
const int CMouseManager::GetMouseSmoothing() const
{
	return this->m_mouseSmoothing;
}

/**
* Get a button
* @param button The button id
* @return The button
*/
const int CMouseManager::GetButton(const int button) const
{
	return this->m_buttons[button][MOUSE_BUTTON_INDEX_ASSIGNED];
}

/**
* Get the first pushed button
* @return The first pushed button
*/
const int CMouseManager::GetFirstButtonDown()
{
	bool found = false;
	int ret = ACTION_KEY_DISABLED;
	for(int i = 0 ; i < 4 ; i++)
	{
		if(KEYDOWN(this->m_state.rgbButtons, i))
		{
			ret = i;
			found = true;
		}
	}
	return ret;
}

/**
* Get the mouse sensibility
* @return The mouse sensibility
*/
const float CMouseManager::GetMouseSensibility() const
{
	return this->m_mouseSensibility;
}
