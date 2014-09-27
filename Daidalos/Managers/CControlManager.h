#ifndef __CCONTROLMANAGER_H
#define __CCONTROLMANAGER_H

#include "../managers/CSingletonManager.h"

class CKeyboardManager;
class CGamepadManager;
class CMouseManager;

/**
* CControlManager class.
* Manages the player inputs (gamepad, mouse and keyboard).
*/
class CControlManager : public CSingletonManager<CControlManager>
{
friend class CSingletonManager<CControlManager>;

private:
	CKeyboardManager * m_keyM;
	CGamepadManager * m_gamepadM;
	CMouseManager * m_mouseM;

public:
	~CControlManager();
	void Initialize();
	void Update();
	void SetKeyState(const int key, const int state);
	void SetKeyboardKey(const int key, const int assignedKey);
	void SetGamepadButton(const int button, const int assignedButton);
	void SetGamepadAxis(const int axis, const int assignedAxis);
	void SetGamepadSensibility(const float sensibility);
	void InverseGamepadAxis(const int axis, const int inverse);
	void SetMouseButton(const int button, const int assignedButton);
	void SetMouseSensibility(const float sensibility);
	void SetMouseSmoothing(const int smoothing);
	void ShowMouseCursor();
	void CenterMouseCursor();
	void FreezeKey(const int key);
	void Restore();
	void FreezeInputs();
	void SaveGamepadConfiguration();
	void SaveKeyboardConfiguration();
	void SaveMouseConfiguration();
	const bool IsGamepadDetected() const;
	const bool IsKeyDown(const int key);
	const bool IsMouseKeyDown(const int key);
	const bool IsGamepadAxisInversed(const int axis);
	const bool HasMouseMoved() const;
	const int GetKeyboardFirstKeyDown();
	const int GetGamepadFirstButtonDown();
	const int GetGamepadFirstAxisDown();
	const int GetMouseFirstButtonDown();
	const int GetKeyboardKey(const int key);
	const int GetGamepadButton(const int button);
	const int GetGamepadAxis(const int axis);
	const int GetMouseButton(const int button) const;
	const int GetMouseX() const;
	const int GetMouseY() const;
	const int GetMouseCursorX() const;
	const int GetMouseCursorY() const;
	const int GetMouseWheelMovement() const;
	const int GetMouseSmoothing() const;
	const float GetKeyRatio(const int key);
	const float GetGamepadSensibility() const;
	const float GetMouseSensibility() const;
	const std::string GetKeyboardKeyName(const int key);

private:
	CControlManager();
};

#endif
