#include "../Misc/Global.h"
#include "../managers/CGamepadManager.h"
#include "../managers/COptionManager.h"

/**
* Constructor
*/
CGamepadManager::CGamepadManager(){}

/**
* Destructor
*/
CGamepadManager::~CGamepadManager()
{
	if(this->m_buttons != NULL)
	{
		for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
			SAFE_DELETE(this->m_buttons[i]);
		SAFE_DELETE_ARRAY(this->m_buttons);
	}

	Release();
}

/**
* Initialize the manager
*/
void CGamepadManager::Initialize()
{
	this->m_gamepadDetected = false;
	this->m_gamepad = NULL;
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();
	this->m_buttons = NULL;

	HRESULT result;
	result = this->m_deviceM->GetInput()->EnumDevices(DI8DEVCLASS_GAMECTRL, CreateDeviceCallback, NULL, DIEDFL_ATTACHEDONLY);

	if(this->m_gamepadDetected)
	{
		_VERIFY(this->m_gamepad->SetDataFormat(&c_dfDIJoystick), "Gamepad Data Format");

		this->m_buttons = new int*[ACTION_KEY_SIZE];
		for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		{
			this->m_buttons[i] = new int[GAMEPAD_KEY_INDEX_SIZE];
			this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] = ACTION_KEY_DISABLED;
		}

		// Reverse stick options
		this->m_reverseSticks[GAMEPAD_REVERSE_LX] = GetPrivateProfileInt("gamepad", "reverse_lx", 0, INI_FILENAME);
		this->m_reverseSticks[GAMEPAD_REVERSE_LY] = GetPrivateProfileInt("gamepad", "reverse_ly", 0, INI_FILENAME);
		this->m_reverseSticks[GAMEPAD_REVERSE_RX] = GetPrivateProfileInt("gamepad", "reverse_rx", 0, INI_FILENAME);
		this->m_reverseSticks[GAMEPAD_REVERSE_RY] = GetPrivateProfileInt("gamepad", "reverse_ry", 0, INI_FILENAME);

		// Sticks axes association
		this->m_axesAssociation[GAMEPAD_AXIS_LEFT_X] = GetPrivateProfileInt("gamepad", "lstick_x_assoc", 0, INI_FILENAME);
		this->m_axesAssociation[GAMEPAD_AXIS_LEFT_Y] = GetPrivateProfileInt("gamepad", "lstick_y_assoc", 1, INI_FILENAME);
		this->m_axesAssociation[GAMEPAD_AXIS_LEFT_Z] = GetPrivateProfileInt("gamepad", "lstick_z_assoc", 2, INI_FILENAME);
		this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_X] = GetPrivateProfileInt("gamepad", "rstick_x_assoc", 3, INI_FILENAME);
		this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_Y] = GetPrivateProfileInt("gamepad", "rstick_y_assoc", 4, INI_FILENAME);
		this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_Z] = GetPrivateProfileInt("gamepad", "rstick_z_assoc", 5, INI_FILENAME);

		// Get the keys and initialize them to the state UP
		for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		{
			this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] = GetPrivateProfileInt("gamepad", COptionManager::m_actionKeyName[i].c_str(), ACTION_KEY_DISABLED, INI_FILENAME);
			if(this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] < ACTION_KEY_DISABLED || this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] >= sizeof(this->m_state.rgbButtons))
				this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] = ACTION_KEY_DISABLED;
			this->m_buttons[i][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}

		// Axes association verification
		ApplyAxisModifications();

		// Get the default values of the axes
		HRESULT res = this->m_gamepad->GetDeviceState(sizeof(this->m_state),(LPVOID)&this->m_state);
		if(FAILED(res))
		{
			res = this->m_gamepad->Acquire();

			while(res == DIERR_INPUTLOST)    
				 res = this->m_gamepad->Acquire();

			if(SUCCEEDED(res))
				this->m_gamepad->GetDeviceState(sizeof(this->m_state), (LPVOID)&this->m_state);
		}

		InitializeAxesDefaultValue();
	}

	// Sensibility option
	this->m_gamepadSensibility = (float)GetPrivateProfileInt("gamepad", "sensibility", INI_DEFAULT_GAMEPAD_SENSIBILITY, INI_FILENAME);

	// Sensibility verification
	if(this->m_gamepadSensibility < 0 || this->m_gamepadSensibility > GAMEPAD_SENSIBILITY_MAX)
		this->m_gamepadSensibility = INI_DEFAULT_GAMEPAD_SENSIBILITY;
	this->m_gamepadSensibility /= 100.0f;
}

/**
* Refresh the gamepad
*/
void CGamepadManager::Update()
{
	HRESULT res = this->m_gamepad->GetDeviceState(sizeof(this->m_state),(LPVOID)&this->m_state);
	if(FAILED(res))
	{
		res = this->m_gamepad->Acquire();

		while(res == DIERR_INPUTLOST)    
			 res = this->m_gamepad->Acquire();

		if(SUCCEEDED(res))
			this->m_gamepad->GetDeviceState(sizeof(this->m_state), (LPVOID)&this->m_state);
	}

	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
		{
			if(KEYDOWN(this->m_state.rgbButtons, this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED]))
			{
				if(this->m_buttons[i][GAMEPAD_KEY_INDEX_STATE] == GAMEPAD_KEY_STATE_UP)
					this->m_buttons[i][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			}
			else
				this->m_buttons[i][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}
	}

	if(this->m_state.rgdwPOV[0] == 0 || this->m_state.rgdwPOV[0] == 4500 || this->m_state.rgdwPOV[0] == 31500)
		this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
	else
		this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;

	if(this->m_state.rgdwPOV[0] == 18000 || this->m_state.rgdwPOV[0] == 22500 || this->m_state.rgdwPOV[0] == 13500)
		this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
	else
		this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;

	if(this->m_state.rgdwPOV[0] == 27000 || this->m_state.rgdwPOV[0] == 22500 || this->m_state.rgdwPOV[0] == 31500)
		this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
	else
		this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;

	if(this->m_state.rgdwPOV[0] == 9000 || this->m_state.rgdwPOV[0] == 4500 || this->m_state.rgdwPOV[0] == 13500)
		this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
	else
		this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;


	// Left stick X update
	if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_LEFT_X], *this->m_axesInitialValueReference[GAMEPAD_AXIS_LEFT_X]) == GAMEPAD_AXIS_ORIENTATION_NEGATIVE)
	{
		// Left or right movement depending on the reverse option
		if(this->m_reverseSticks[GAMEPAD_REVERSE_LX] == 0)
		{
			this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}
		else
		{
			this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
		}

		this->m_lRatio.x = 1.0f - ((float)*this->m_axesReference[GAMEPAD_AXIS_LEFT_X] / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
		if(this->m_lRatio.x < 0)
			this->m_lRatio.x = 0;
	}
	else
	{
		if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_LEFT_X], *this->m_axesInitialValueReference[GAMEPAD_AXIS_LEFT_X]) == GAMEPAD_AXIS_ORIENTATION_POSITIVE)
		{
			// Left or right movement depending on the reverse option
			if(this->m_reverseSticks[GAMEPAD_REVERSE_LX] == 0)
			{
				this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
				this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			}
			else
			{
				this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
				this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			}

			this->m_lRatio.x = (((float)*this->m_axesReference[GAMEPAD_AXIS_LEFT_X] - GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE) / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
			if(this->m_lRatio.x > 1)
				this->m_lRatio.x = 1;
		}
		else
		{
			this->m_lRatio.x = 0;
		}
	}

	// Left stick Y update
	if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_LEFT_Y], *this->m_axesInitialValueReference[GAMEPAD_AXIS_LEFT_Y]) == GAMEPAD_AXIS_ORIENTATION_NEGATIVE)
	{
		// Up or down movement depending on the reverse option
		if(this->m_reverseSticks[GAMEPAD_REVERSE_LY] == 0)
		{
			this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
		}
		else
		{
			this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}

		this->m_lRatio.y = 1.0f - ((float)*this->m_axesReference[GAMEPAD_AXIS_LEFT_Y] / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
		if(this->m_lRatio.y < 0)
			this->m_lRatio.y = 0;
	}
	else
	{
		if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_LEFT_Y], *this->m_axesInitialValueReference[GAMEPAD_AXIS_LEFT_Y]) == GAMEPAD_AXIS_ORIENTATION_POSITIVE)
		{
			// Up or down movement depending on the reverse option
			if(this->m_reverseSticks[GAMEPAD_REVERSE_LY] == 0)
			{
				this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
				this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			}
			else
			{
				this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
				this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			}

			this->m_lRatio.y = (((float)*this->m_axesReference[GAMEPAD_AXIS_LEFT_Y] - GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE) / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
			if(this->m_lRatio.y > 1)
				this->m_lRatio.y = 1;
		}
		else
		{
			this->m_lRatio.y = 0;
		}
	}

	// Right stick X update
	if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_RIGHT_X], *this->m_axesInitialValueReference[GAMEPAD_AXIS_RIGHT_X]) == GAMEPAD_AXIS_ORIENTATION_NEGATIVE)
	{
		// Left or right movement depending on the reverse option
		if(this->m_reverseSticks[GAMEPAD_REVERSE_RX] == 0)
		{
			this->m_buttons[ACTION_KEY_TURN_LEFT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			this->m_buttons[ACTION_KEY_TURN_RIGHT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}
		else
		{
			this->m_buttons[ACTION_KEY_TURN_LEFT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			this->m_buttons[ACTION_KEY_TURN_RIGHT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
		}

		this->m_rRatio.x = 1.0f - ((float)*this->m_axesReference[GAMEPAD_AXIS_RIGHT_X] / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
		if(this->m_rRatio.x < 0)
			this->m_rRatio.x = 0;
	}
	else
	{
		if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_RIGHT_X], *this->m_axesInitialValueReference[GAMEPAD_AXIS_RIGHT_X]) == GAMEPAD_AXIS_ORIENTATION_POSITIVE)
		{
			// Left or right movement depending on the reverse option
			if(this->m_reverseSticks[GAMEPAD_REVERSE_RX] == 0)
			{
				this->m_buttons[ACTION_KEY_TURN_LEFT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
				this->m_buttons[ACTION_KEY_TURN_RIGHT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			}
			else
			{
				this->m_buttons[ACTION_KEY_TURN_LEFT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
				this->m_buttons[ACTION_KEY_TURN_RIGHT][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			}

			this->m_rRatio.x = (((float)*this->m_axesReference[GAMEPAD_AXIS_RIGHT_X] - GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE) / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
			if(this->m_rRatio.x > 1)
				this->m_rRatio.x = 1;
		}
		else
		{
			this->m_rRatio.x = 0;
		}
	}

	// Right stick Y update
	if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_RIGHT_Y], *this->m_axesInitialValueReference[GAMEPAD_AXIS_RIGHT_Y]) == GAMEPAD_AXIS_ORIENTATION_NEGATIVE)
	{
		// Up or down movement depending on the reverse option
		if(this->m_reverseSticks[GAMEPAD_REVERSE_RY] == 0)
		{
			this->m_buttons[ACTION_KEY_LOOK_DOWN][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			this->m_buttons[ACTION_KEY_LOOK_UP][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
		}
		else
		{
			this->m_buttons[ACTION_KEY_LOOK_DOWN][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			this->m_buttons[ACTION_KEY_LOOK_UP][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
		}

		this->m_rRatio.y = 1.0f - ((float)*this->m_axesReference[GAMEPAD_AXIS_RIGHT_Y] / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
		if(this->m_rRatio.y < 0)
			this->m_rRatio.y = 0;
	}
	else
	{
		if(GetAxisOrientation(*this->m_axesReference[GAMEPAD_AXIS_RIGHT_Y], *this->m_axesInitialValueReference[GAMEPAD_AXIS_RIGHT_Y]) == GAMEPAD_AXIS_ORIENTATION_POSITIVE)
		{
			//  Up or down movement depending on the reverse option
			if(this->m_reverseSticks[GAMEPAD_REVERSE_RY] == 0)
			{
				this->m_buttons[ACTION_KEY_LOOK_DOWN][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
				this->m_buttons[ACTION_KEY_LOOK_UP][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
			}
			else
			{
				this->m_buttons[ACTION_KEY_LOOK_DOWN][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_DOWN;
				this->m_buttons[ACTION_KEY_LOOK_UP][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
			}

			this->m_rRatio.y = (((float)*this->m_axesReference[GAMEPAD_AXIS_RIGHT_Y] - GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE) / (GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE));
			if(this->m_rRatio.y > 1)
				this->m_rRatio.y = 1;
		}
		else
		{
			this->m_rRatio.y = 0;
		}
	}

	this->m_buttons[ACTION_KEY_FORWARD][GAMEPAD_KEY_INDEX_STATE] = this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE];
	this->m_buttons[ACTION_KEY_BACKWARD][GAMEPAD_KEY_INDEX_STATE] = this->m_buttons[ACTION_KEY_DOWN_MENU][GAMEPAD_KEY_INDEX_STATE];
	this->m_buttons[ACTION_KEY_STRAFE_LEFT][GAMEPAD_KEY_INDEX_STATE] = this->m_buttons[ACTION_KEY_LEFT_MENU][GAMEPAD_KEY_INDEX_STATE];
	this->m_buttons[ACTION_KEY_STRAFE_RIGHT][GAMEPAD_KEY_INDEX_STATE] = this->m_buttons[ACTION_KEY_RIGHT_MENU][GAMEPAD_KEY_INDEX_STATE];
}

/**
* Change a button state
* @param button The button number
* @param state The new state
*/
void CGamepadManager::SetButtonState(const int button, const int state)
{
	this->m_buttons[button][GAMEPAD_KEY_INDEX_STATE] = state;
}

/**
* Set a button
* @param button The button id
* @param assignedButton The assigned button
*/
void CGamepadManager::SetButton(const int button, const int assignedButton)
{
	this->m_buttons[button][GAMEPAD_KEY_INDEX_ASSIGNED] = assignedButton;
}

/**
* Set an axis
* @param axis The axis id
* @param assignedAxis The assigned axis
*/
void CGamepadManager::SetAxis(const int axis, const int assignedAxis)
{
	this->m_axesAssociation[axis] = assignedAxis;
}

/**
* Inverse an axis
* @param axis The axis id
* @param inverse The inverse value (-1 or 1)
*/
void CGamepadManager::InverseAxis(const int axis, const int inverse)
{
	this->m_reverseSticks[axis] = inverse;
}

/**
* Set the sensibility
* sensibility The new sensibility
*/
void CGamepadManager::SetSensibility(const float sensibility)
{
	this->m_gamepadSensibility = sensibility;
}

/**
* Release the joypad
*/
void CGamepadManager::Release()
{
	SAFE_RELEASE(this->m_gamepad);
}

/**
* Restore the joypad
*/
void CGamepadManager::Restore()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
		this->m_buttons[ACTION_KEY_UP_MENU][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_UP;
}

/**
* Set the joypad detection to true
*/
void CGamepadManager::SetGamepadDetected()
{
	this->m_gamepadDetected = true;
}

/**
* Freeze the joypad
*/
void CGamepadManager::FreezeButtons()
{
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
			this->m_buttons[i][GAMEPAD_KEY_INDEX_STATE] = GAMEPAD_KEY_STATE_FROZEN;
	}
}

/**
* Apply the axis modifications
* The goal here is to link the axis to their associated action (move forward / backward etc)
*/
void CGamepadManager::ApplyAxisModifications()
{
	for(int i = 0 ; i < GAMEPAD_AXIS_SIZE ; i++)
	{
		switch(this->m_axesAssociation[i])
		{
		case GAMEPAD_AXIS_LEFT_X:
			this->m_axesReference[i] = &this->m_state.lX;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_X];
			break;

		case GAMEPAD_AXIS_LEFT_Y:
			this->m_axesReference[i] = &this->m_state.lY;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Y];
			break;

		case GAMEPAD_AXIS_LEFT_Z:
			this->m_axesReference[i] = &this->m_state.lZ;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Z];
			break;

		case GAMEPAD_AXIS_RIGHT_X:
			this->m_axesReference[i] = &this->m_state.lRx;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_X];
			break;

		case GAMEPAD_AXIS_RIGHT_Y:
			this->m_axesReference[i] = &this->m_state.lRy;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Y];
			break;

		case GAMEPAD_AXIS_RIGHT_Z:
			this->m_axesReference[i] = &this->m_state.lRz;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Z];
			break;

		default:
			this->m_axesReference[i] = &this->m_state.lX;
			this->m_axesInitialValueReference[i] = &this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_X];
			break;
		}
	}
}

/**
* Save the configuration
*/
void CGamepadManager::Save()
{
	std::ostringstream stream;
	std::string tmp;

	ApplyAxisModifications();

	// Sensibility
	stream.str("");
	stream << (this->m_gamepadSensibility * 100);
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "sensibility", tmp.c_str(), INI_FILENAME);

	// Inverse LX Axis
	stream.str("");
	stream << this->m_reverseSticks[GAMEPAD_REVERSE_LX];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "reverse_lx", tmp.c_str(), INI_FILENAME);

	// Inverse LY Axis
	stream.str("");
	stream << this->m_reverseSticks[GAMEPAD_REVERSE_LY];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "reverse_ly", tmp.c_str(), INI_FILENAME);

	// Inverse RX Axis
	stream.str("");
	stream << this->m_reverseSticks[GAMEPAD_REVERSE_RX];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "reverse_rx", tmp.c_str(), INI_FILENAME);

	// Inverse RY Axis
	stream.str("");
	stream << this->m_reverseSticks[GAMEPAD_REVERSE_RY];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "reverse_ry", tmp.c_str(), INI_FILENAME);

	// LX Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_LEFT_X];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "lstick_x_assoc", tmp.c_str(), INI_FILENAME);

	// LY Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_LEFT_Y];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "lstick_y_assoc", tmp.c_str(), INI_FILENAME);

	// LZ Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_LEFT_Z];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "lstick_z_assoc", tmp.c_str(), INI_FILENAME);

	// RX Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_X];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "rstick_x_assoc", tmp.c_str(), INI_FILENAME);

	// RY Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_Y];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "rstick_y_assoc", tmp.c_str(), INI_FILENAME);

	// RZ Axis associations
	stream.str("");
	stream << this->m_axesAssociation[GAMEPAD_AXIS_RIGHT_Z];
	tmp = stream.str();
	WritePrivateProfileString("gamepad", "rstick_z_assoc", tmp.c_str(), INI_FILENAME);

	// Buttons
	for(int i = 0 ; i < ACTION_KEY_SIZE ; i++)
	{
		if(this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED] != ACTION_KEY_DISABLED)
		{
			stream.str("");
			stream << this->m_buttons[i][GAMEPAD_KEY_INDEX_ASSIGNED];
			tmp = stream.str();
			WritePrivateProfileString("gamepad", COptionManager::m_actionKeyName[i].c_str(), tmp.c_str(), INI_FILENAME);
		}
		else
			WritePrivateProfileString("gamepad", COptionManager::m_actionKeyName[i].c_str(), NULL, INI_FILENAME);
	}
}

/**
* Check the joypad
* @return If the joypad is detected
*/
const bool CGamepadManager::IsGamepadDetected() const
{
	return this->m_gamepadDetected;
}

/**
* Check if a button is down
* @param button The button key id
* @return The test result
*/
const bool CGamepadManager::IsButtonDown(const int button)
{
	return this->m_gamepadDetected && this->m_buttons[button][GAMEPAD_KEY_INDEX_STATE] == GAMEPAD_KEY_STATE_DOWN;
}

/**
* Check if a an axe is inversed
* @param axis The axe id
* @return The test result
*/
const bool CGamepadManager::IsAxisInversed(const int axis)
{
	return this->m_reverseSticks[axis] == 1;
}

/**
* Get the first pushed button
* @return The first pushed button
*/
const int CGamepadManager::GetFirstButtonDown()
{
	bool found = false;
	int ret = -1;
	for(int i = 0 ; i < 32 && !found; i++)
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
* Get the first pushed axis
* @return The first pushed axis
*/
const int CGamepadManager::GetFirstAxisDown()
{
	if(GetAxisOrientation(this->m_state.lX, this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_X]) != GAMEPAD_AXIS_ORIENTATION_NULL)
		return GAMEPAD_AXIS_LEFT_X;
	else
		if(GetAxisOrientation(this->m_state.lY, this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Y]) != GAMEPAD_AXIS_ORIENTATION_NULL)
			return GAMEPAD_AXIS_LEFT_Y;
		else
			if(GetAxisOrientation(this->m_state.lZ, this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Z]) != GAMEPAD_AXIS_ORIENTATION_NULL)
				return GAMEPAD_AXIS_LEFT_Z;
			else
				if(GetAxisOrientation(this->m_state.lRx, this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_X]) != GAMEPAD_AXIS_ORIENTATION_NULL)
					return GAMEPAD_AXIS_RIGHT_X;
				else
					if(GetAxisOrientation(this->m_state.lRy, this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Y]) != GAMEPAD_AXIS_ORIENTATION_NULL)
						return GAMEPAD_AXIS_RIGHT_Y;
					else
						if(GetAxisOrientation(this->m_state.lRz, this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Z]) != GAMEPAD_AXIS_ORIENTATION_NULL)
						return GAMEPAD_AXIS_RIGHT_Z;
	return -1;
}

/**
* Get a button associated to a button id
* @param button The button id
* @return The associated button
*/
const int CGamepadManager::GetButton(const int button) const
{
	return this->m_buttons[button][GAMEPAD_KEY_INDEX_ASSIGNED];
}

/**
* Get an axis associated to a axis id
* @param axis The axis id
* @return The associated axis
*/
const int CGamepadManager::GetAxis(const int axis) const
{
	return this->m_axesAssociation[axis];
}

/**
* Get the gamepad sensibility
* @return The gamepad sensibility
*/
const float CGamepadManager::GetGamepadSensibility() const
{
	return this->m_gamepadSensibility;
}

/**
* Get the left stick position ratio
* @return The left stick position ratio
*/
const D3DXVECTOR2 CGamepadManager::GetLRatio() const
{
	return this->m_lRatio;
}

/**
* Get the right stick position ratio
* @return The right stick position ratio
*/
const D3DXVECTOR2 CGamepadManager::GetRRatio() const
{
	return this->m_rRatio;
}

/**
* Get the joypad device
* @return The joypad device
*/
LPDIRECTINPUTDEVICEA * CGamepadManager::GetGamepad()
{
	return &this->m_gamepad;
}

/**
* Initialize the axes with their default value
*/
void CGamepadManager::InitializeAxesDefaultValue()
{
	this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_X] = this->m_state.lX;
	this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Y] = this->m_state.lY;
	this->m_axesInitialValue[GAMEPAD_AXIS_LEFT_Z] = this->m_state.lZ;
	this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_X] = this->m_state.lRx;
	this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Y] = this->m_state.lRy;
	this->m_axesInitialValue[GAMEPAD_AXIS_RIGHT_Z] = this->m_state.lRz;
}

/**
* Get an axis orientation (GAMEPAD_AXIS_ORIENTATION_NEGATIVE, GAMEPAD_AXIS_ORIENTATION_NULL or GAMEPAD_AXIS_ORIENTATION_POSITIVE)
* @param axis The axis value
* @param defaultValue The axis default value
*/
const int CGamepadManager::GetAxisOrientation(long axis, long defaultValue)
{
	if(defaultValue - GAMEPAD_DEAD_ZONE < 0)
	{
		if(axis + GAMEPAD_DEAD_ZONE >= GAMEPAD_CENTER)
			return GAMEPAD_AXIS_ORIENTATION_POSITIVE;
		else
			return GAMEPAD_AXIS_ORIENTATION_NULL;
	}
	else
	{
		if(axis < GAMEPAD_CENTER - GAMEPAD_DEAD_ZONE)
			return GAMEPAD_AXIS_ORIENTATION_NEGATIVE;
		else
			if(axis > GAMEPAD_CENTER + GAMEPAD_DEAD_ZONE)
				return GAMEPAD_AXIS_ORIENTATION_POSITIVE;
			else
				return GAMEPAD_AXIS_ORIENTATION_NULL;
	}
}

/**
* Create the joypad device
* @return The enumeration state
*/
BOOL CALLBACK CreateDeviceCallback(LPCDIDEVICEINSTANCE instance, void * reference)
{
	CGamepadManager * gamepadM = CGamepadManager::GetInstance();
	CDeviceManager * deviceM = CDeviceManager::GetInstance();

	HRESULT result = deviceM->GetInput()->CreateDevice( instance->guidInstance, gamepadM->GetGamepad(), NULL);
	
	if(SUCCEEDED(result))
	{
		gamepadM->SetGamepadDetected();
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}
