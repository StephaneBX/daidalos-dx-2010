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

#ifndef __CMOUSEMANAGER_H
#define __CMOUSEMANAGER_H

#define MOUSE_CURSOR_FILE "cur.png"
#define MOUSE_SMOOTHING_MAX 50
#define MOUSE_SENSIBILITY_MAX 500
#define MOUSE_FILE_ENTRY_NAME "mouse"
#define MOUSE_FILE_ENTRY_SENSIBILITY "sensibility"
#define MOUSE_FILE_ENTRY_SMOOTHING "smoothing"

#include "../Managers/CDeviceManager.h"
#include "../Managers/CSingletonManager.h"

enum mouseButtonIndex
{
	MOUSE_BUTTON_INDEX_ASSIGNED,
	MOUSE_BUTTON_INDEX_STATE,
	MOUSE_BUTTON_INDEX_SIZE
};

enum mouseButtonState
{
	MOUSE_BUTTON_STATE_UP,
	MOUSE_BUTTON_STATE_DOWN,
	MOUSE_BUTTON_STATE_FROZEN
};

enum mouseMode
{
	MOUSE_MODE_MENU,
	MOUSE_MODE_INGAME
};

class CLogManager;
class COptionManager;
class CSpriteEntity;

/**
* CMouseManager class.
* Manages the mouse events.
*/
class CMouseManager : public CSingletonManager<CMouseManager>
{
friend class CSingletonManager<CMouseManager>;

private:
	bool m_hasMoved;
	int m_mouseSmoothing;
	int m_mouseDelay;
	int m_movementX;
	int m_movementY;
	int m_mouseMode;
	int m_pos[2];
	float m_mouseSensibility;
	int ** m_buttons;
	CSpriteEntity * m_cursor;
	DIMOUSESTATE m_state;
	LPDIRECTINPUTDEVICEA m_mouse;

	CDeviceManager * m_deviceM;
	COptionManager * m_optionM;
	CLogManager * m_logM;

public:
	~CMouseManager();
	void Initialize();
	void Update();
	void ShowCursor();
	void Restore();
	void Release();
	void FreezeButtons();
	void ReleaseResources();
	void RestoreResources();
	void SetSensibility(const float sensibility);
	void SetSmoothing(const int smoothing);
	void SetButton(const int button, const int assignedButton);
	void SetButtonState(const int button, const int state);
	void Save();
	void CenterCursor();
	const bool HasMoved() const;
	const bool IsButtonDown(int button);
	const int GetX() const;
	const int GetY() const;
	const int GetCursorX() const;
	const int GetCursorY() const;
	const int GetWheelMovement() const;
	const int GetMouseSmoothing() const;
	const int GetButton(const int button) const;
	const int GetFirstButtonDown();
	const float GetMouseSensibility() const;
	
private:
	CMouseManager();
};

#endif
