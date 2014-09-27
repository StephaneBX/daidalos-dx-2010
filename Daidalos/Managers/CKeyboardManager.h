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

#ifndef __CKEYBOARDMANAGER_H
#define __CKEYBOARDMANAGER_H

#include "../Managers/CDeviceManager.h"
#include "../Managers/CSingletonManager.h"

enum keyboardKeyIndex
{
	KEYBOARD_KEY_INDEX_ASSIGNED,
	KEYBOARD_KEY_INDEX_STATE,
	KEYBOARD_KEY_INDEX_LINKED_KEY,
	KEYBOARD_KEY_INDEX_SIZE
};

enum keyboardKeyState
{
	KEYBOARD_KEY_STATE_UP,
	KEYBOARD_KEY_STATE_DOWN,
	KEYBOARD_KEY_STATE_FROZEN,
	KEYBOARD_KEY_STATE_DOWN_WAIT
};

class CLogManager;

/**
* CKeyboardManager class.
* Manages the keyboard events.
*/
class CKeyboardManager : public CSingletonManager<CKeyboardManager>
{
friend class CSingletonManager<CKeyboardManager>;

private:
	char m_keysBuffer[256];
	int ** m_keys;
	LPDIRECTINPUTDEVICEA m_keyboard;

	CDeviceManager * m_deviceM;
	CLogManager * m_logM;

public:
	~CKeyboardManager();
	void Initialize();
	void SetKeyState(const int key, const int state);
	void SetKey(const int key, const int assignedKey);
	void Save();
	void Update();
	void Restore();
	void FreezeKeys();
	void Release();
	const bool IsKeyDown(const int key) const;
	const int GetFirstKeyDown();
	const int GetKey(const int key);
	const std::string GetKeyName(const int key);
	
private:
	CKeyboardManager();
};

#endif
