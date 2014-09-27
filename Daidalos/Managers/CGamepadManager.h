#ifndef __CGAMEPADMANAGER_H
#define __CGAMEPADMANAGER_H

#define GAMEPAD_DEAD_ZONE 10000
#define GAMEPAD_CENTER 32767
#define GAMEPAD_SENSIBILITY_MAX 200
#define GAMEPAD_AXIS_NORMAL 1
#define GAMEPAD_AXIS_INVERSED -1

#include "../Managers/CDeviceManager.h"
#include "../Managers/CSingletonManager.h"

enum gamepadKeyIndex
{
	GAMEPAD_KEY_INDEX_ASSIGNED,
	GAMEPAD_KEY_INDEX_STATE,
	GAMEPAD_KEY_INDEX_SIZE
};

enum gamepadKeyState
{
	GAMEPAD_KEY_STATE_UP,
	GAMEPAD_KEY_STATE_DOWN,
	GAMEPAD_KEY_STATE_FROZEN
};

enum gamepadReverse
{
	GAMEPAD_REVERSE_LX,
	GAMEPAD_REVERSE_LY,
	GAMEPAD_REVERSE_RX,
	GAMEPAD_REVERSE_RY,
	GAMEPAD_REVERSE_SIZE
};

enum gamepadAxisReference
{
	GAMEPAD_AXIS_LEFT_X,
	GAMEPAD_AXIS_LEFT_Y,
	GAMEPAD_AXIS_LEFT_Z,
	GAMEPAD_AXIS_RIGHT_X,
	GAMEPAD_AXIS_RIGHT_Y,
	GAMEPAD_AXIS_RIGHT_Z,
	GAMEPAD_AXIS_SIZE
};

enum gamepadAxisOrientation
{
	GAMEPAD_AXIS_ORIENTATION_NEGATIVE,
	GAMEPAD_AXIS_ORIENTATION_NULL,
	GAMEPAD_AXIS_ORIENTATION_POSITIVE
};

class CLogManager;

/**
* CGamepadManager class.
* Manages the gamepad events.
*/
class CGamepadManager : public CSingletonManager<CGamepadManager>
{
friend class CSingletonManager<CGamepadManager>;

private:
	bool m_gamepadDetected;
	float m_gamepadSensibility;
	int m_reverseSticks[GAMEPAD_REVERSE_SIZE];
	int m_axesAssociation[GAMEPAD_AXIS_SIZE];
	long m_axesInitialValue[GAMEPAD_AXIS_SIZE];
	long * m_axesReference[GAMEPAD_AXIS_SIZE];
	long * m_axesInitialValueReference[GAMEPAD_AXIS_SIZE];
	int ** m_buttons;
	D3DXVECTOR2 m_lRatio;
	D3DXVECTOR2 m_rRatio;
	DIJOYSTATE m_state;
	LPDIRECTINPUTDEVICEA m_gamepad;

	CDeviceManager * m_deviceM;
	CLogManager * m_logM;

public:
	~CGamepadManager();
	void SetButtonState(const int button, const int state);
	void SetButton(const int button, const int assignedButton);
	void SetAxis(const int axis, const int assignedAxis);
	void InverseAxis(const int axis, const int inverse);
	void SetSensibility(const float sensibility);
	void SetGamepadDetected();
	void Initialize();
	void Update();
	void Restore();
	void FreezeButtons();
	void Release();
	void ApplyAxisModifications();
	void Save();
	const bool IsGamepadDetected() const;
	const bool IsButtonDown(const int button);
	const bool IsAxisInversed(const int axis);
	const int GetFirstButtonDown();
	const int GetFirstAxisDown();
	const int GetButton(const int button) const;
	const int GetAxis(const int axis) const;
	const float GetGamepadSensibility() const;
	const D3DXVECTOR2 GetLRatio() const;
	const D3DXVECTOR2 GetRRatio() const;
	LPDIRECTINPUTDEVICEA * GetGamepad();
	
private:
	CGamepadManager();
	void InitializeAxesDefaultValue();
	const int GetAxisOrientation(long axis, long defaultValue);
};

BOOL CALLBACK CreateDeviceCallback(LPCDIDEVICEINSTANCE instance, void * reference);

#endif
