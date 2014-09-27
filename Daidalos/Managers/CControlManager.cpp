#include "../Misc/Global.h"
#include "../Managers/CControlManager.h"
#include "../Managers/CKeyboardManager.h"
#include "../Managers/CGamepadManager.h"
#include "../Managers/CMouseManager.h"
#include "../Managers/COptionManager.h"

/**
* Constructor
*/
CControlManager::CControlManager(){}

/**
* Destructor
*/
CControlManager::~CControlManager(){}

/**
* Initialize the manager
*/
void CControlManager::Initialize()
{
	this->m_keyM = CKeyboardManager::GetInstance();
	this->m_gamepadM = CGamepadManager::GetInstance();
	this->m_mouseM = CMouseManager::GetInstance();
}

/**
* Update the controls
*/
void CControlManager::Update()
{
	this->m_keyM->Update();
	if(this->m_gamepadM->IsGamepadDetected())
		this->m_gamepadM->Update();
	this->m_mouseM->Update();
}

/**
* Change the key state
* @param key The key number
* @param state The key state
*/
void CControlManager::SetKeyState(const int key, const int state)
{
	this->m_keyM->SetKeyState(key, state);
	if(this->m_gamepadM->IsGamepadDetected())
		this->m_gamepadM->SetButtonState(key, state);
}

/**
* Change the key
* @param key The key number
* @param assignedKey The new assigned key
*/
void CControlManager::SetKeyboardKey(const int key, const int assignedKey)
{
	this->m_keyM->SetKey(key, assignedKey);
}

/**
* Set a gamepad button
* @param button The button id
* @param assignedButton The assigned button
*/
void CControlManager::SetGamepadButton(const int button, const int assignedButton)
{
	this->m_gamepadM->SetButton(button, assignedButton);
}

/**
* Set a gamepad axis
* @param axis The axis id
* @param assignedAxis The assigned axis
*/
void CControlManager::SetGamepadAxis(const int axis, const int assignedAxis)
{
	this->m_gamepadM->SetAxis(axis, assignedAxis);
}

/**
* Set the gamepad sensibility
* @param sensibility The new sensibility
*/
void CControlManager::SetGamepadSensibility(const float sensibility)
{
	this->m_gamepadM->SetSensibility(sensibility);
}

/**
* Inverse a gamepad axis
* @param axis The axis id
* @param inverse The inverse value (-1 or 1)
*/
void CControlManager::InverseGamepadAxis(const int axis, const int inverse)
{
	this->m_gamepadM->InverseAxis(axis, inverse);
}

/**
* Set a mouse button
* @param button The button id
* @param assignedButton The assigned button
*/
void CControlManager::SetMouseButton(const int button, const int assignedButton)
{
	this->m_mouseM->SetButton(button, assignedButton);
}

/**
* Set the mouse sensibility
* @param sensibility The new sensibility
*/
void CControlManager::SetMouseSensibility(const float sensibility)
{
	this->m_mouseM->SetSensibility(sensibility);
}

/**
* Set the mouse smoothing
* @param smoothing The new smoothing
*/
void CControlManager::SetMouseSmoothing(const int smoothing)
{
	this->m_mouseM->SetSmoothing(smoothing);
}

/**
* Show the mouse cursor for the current frame
*/
void CControlManager::ShowMouseCursor()
{
	this->m_mouseM->ShowCursor();
}

/**
* Center the mouse cursor
*/
void CControlManager::CenterMouseCursor()
{
	this->m_mouseM->CenterCursor();
}

/**
* Freeze a key
* @param key The key number
*/
void CControlManager::FreezeKey(const int key)
{
	this->m_keyM->SetKeyState(key, KEYBOARD_KEY_STATE_FROZEN);
	if(this->m_gamepadM->IsGamepadDetected())
		this->m_gamepadM->SetButtonState(key, GAMEPAD_KEY_STATE_FROZEN);
	this->m_mouseM->SetButtonState(key, MOUSE_BUTTON_STATE_FROZEN);
}

/**
* Allow the use of the controls
*/
void CControlManager::Restore()
{
	this->m_keyM->Restore();
	if(this->m_gamepadM->IsGamepadDetected())
		this->m_gamepadM->Restore();
}

/**
* Freeze the controls
*/
void CControlManager::FreezeInputs()
{
	this->m_keyM->FreezeKeys();
	if(this->m_gamepadM->IsGamepadDetected())
		this->m_gamepadM->FreezeButtons();
	this->m_mouseM->FreezeButtons();
}

/**
* Save the gamepad configuration
*/
void CControlManager::SaveGamepadConfiguration()
{
	this->m_gamepadM->Save();
}

/**
* Save the keyboard configuration
*/
void CControlManager::SaveKeyboardConfiguration()
{
	this->m_keyM->Save();
}

/**
* Save the mouse configuration
*/
void CControlManager::SaveMouseConfiguration()
{
	this->m_mouseM->Save();
}

/**
* Check if a gamepad is plugged
* @return True if a gamepad is detected
*/
const bool CControlManager::IsGamepadDetected() const
{
	return this->m_gamepadM->IsGamepadDetected();
}

/**
* Check if a key (gamepad or keyboard) is down
* @param key The action key id
* @return The text result
*/
const bool CControlManager::IsKeyDown(const int key)
{
	return this->m_keyM->IsKeyDown(key) || this->m_gamepadM->IsButtonDown(key);
}

/**
* Check if a mouse key is down
* @param key The action key id
* @return The text result
*/
const bool CControlManager::IsMouseKeyDown(const int key)
{
	return this->m_mouseM->IsButtonDown(key);
}

/**
* Check if a gamepad axis is inversed
* @param axis The axis id
* @return True if the axis is inversed
*/
const bool CControlManager::IsGamepadAxisInversed(const int axis)
{
	return this->m_gamepadM->IsAxisInversed(axis);
}

/**
* Check if the mouse has moved during this frame
* @return True if the mouse has moved
*/
const bool CControlManager::HasMouseMoved() const
{
	return this->m_mouseM->HasMoved();
}

/**
* Get the first pushed keyboard key
* @return The id of the first pushed keyboard key
*/
const int CControlManager::GetKeyboardFirstKeyDown()
{
	return this->m_keyM->GetFirstKeyDown();
}

/**
* Get the first pushed gamepad button
* @return The first pushed gamepad button
*/
const int CControlManager::GetGamepadFirstButtonDown()
{
	return this->m_gamepadM->GetFirstButtonDown();
}

/**
* Get the first pushed gamepad axis
* @return The first pushed gamepad axis
*/
const int CControlManager::GetGamepadFirstAxisDown()
{
	return this->m_gamepadM->GetFirstAxisDown();
}

/**
* Get the first pushed mouse button
* @return The first pushed mouse button
*/
const int CControlManager::GetMouseFirstButtonDown()
{
	return this->m_mouseM->GetFirstButtonDown();
}

/**
* Get the key to a corresponding keyboard key id
* @param key The key id
* @return The key to the associated key id;
*/
const int CControlManager::GetKeyboardKey(const int key)
{
	return this->m_keyM->GetKey(key);
}

/**
* Get the button to a corresponding gamepad button id
* @param button The key id
* @return The button to the associated button id;
*/
const int CControlManager::GetGamepadButton(const int button)
{
	return this->m_gamepadM->GetButton(button);
}

/**
* Get the axis to a corresponding gamepad axis id
* @param axis The axis id
* @return The axis to the associated axis id;
*/
const int CControlManager::GetGamepadAxis(const int axis)
{
	return this->m_gamepadM->GetAxis(axis);
}

/**
* Get the button to a corresponding mouse button id
* @param button The key id
* @return The button to the associated button id;
*/
const int CControlManager::GetMouseButton(const int button) const
{
	return this->m_mouseM->GetButton(button);
}

/**
* Get the mouse x movement during the current frame
* @return The mouse x movement
*/
const int CControlManager::GetMouseX() const
{
	return this->m_mouseM->GetX();
}

/**
* Get the mouse y movement during the current frame
* @return The mouse y movement
*/
const int CControlManager::GetMouseY() const
{
	return this->m_mouseM->GetY();
}

/**
* Get the mouse x screen coordinate
* @return The mouse x screen coordinate
*/
const int CControlManager::GetMouseCursorX() const
{
	return this->m_mouseM->GetCursorX();
}

/**
* Get the mouse y screen coordinate
* @return The mouse y screen coordinate
*/
const int CControlManager::GetMouseCursorY() const
{
	return this->m_mouseM->GetCursorY();
}

/**
* Get the mouse wheel movement
* @return The mouse wheel movement
*/
const int CControlManager::GetMouseWheelMovement() const
{
	return this->m_mouseM->GetWheelMovement();
}

/**
* Get the mouse smoothing
* @return The mouse smoothing
*/
const int CControlManager::GetMouseSmoothing() const
{
	return this->m_mouseM->GetMouseSmoothing();
}

/**
* Get the key ratio
* A keyboard key ratio is always 1, because its not analogic
* @param key The key id
* @return The key ratio
*/
const float CControlManager::GetKeyRatio(const int key)
{
	float ret = 0.0f;
	if(this->m_keyM->IsKeyDown(key))
	{
		ret = 1.0f;
	}
	else
	{
		if(this->m_gamepadM->IsButtonDown(key))
		{
			if(key == ACTION_KEY_STRAFE_LEFT || key == ACTION_KEY_STRAFE_RIGHT)
			{
				ret = this->m_gamepadM->GetLRatio().x;
			}
			else
			{
				if(key == ACTION_KEY_FORWARD || key == ACTION_KEY_BACKWARD)
				{
					ret = this->m_gamepadM->GetLRatio().y;
				}
				else
				{
					if(key == ACTION_KEY_TURN_LEFT || key == ACTION_KEY_TURN_RIGHT)
					{
						ret = this->m_gamepadM->GetRRatio().x;
					}
					else
					{
						if(key == ACTION_KEY_LOOK_UP || key == ACTION_KEY_LOOK_DOWN)
						{
							ret = this->m_gamepadM->GetRRatio().y;
						}
					}
				}
			}
		}
	}
	return ret;
}

/**
* Get the gamepad sensibility
* @return The gamepad sensibility
*/
const float CControlManager::GetGamepadSensibility() const
{
	return this->m_gamepadM->GetGamepadSensibility();
}

/**
* Get the mouse sensibility
* @return The mouse sensibility
*/
const float CControlManager::GetMouseSensibility() const
{
	return this->m_mouseM->GetMouseSensibility();
}

/**
* Get the keyboard key name
* @param key The key id
* @return The keyboard key name
*/
const std::string CControlManager::GetKeyboardKeyName(const int key)
{
	return this->m_keyM->GetKeyName(key);
}
