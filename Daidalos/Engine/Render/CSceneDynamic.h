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

#ifndef __CSCENEDYNAMIC_H
#define __CSCENEDYNAMIC_H

#include "../../Misc/CArraySorter.h"
#include "../../Engine/Entities/3D/CObjectEntity.h"

class CLogManager;
class CModManager;
class COptionManager;
class CSceneCamera;
class CPlayer;

/**
* CSceneDynamic class.
* Manages the dynamic geometry of the world.
* This part is not in the bsp tree and need more work to be fully optimised.
* The dynamic objects are organized in a linked list.
*/
class CSceneDynamic
{
private:
	int m_facesCounter;
	CObjectEntity * m_dynamicObjectsBuffer;
	std::vector<SObjectCommand> m_commands;

	SVertexLightPass * m_lightsGeometry;
	CPlayer * m_player;

	CLogManager * m_logM;
	CModManager * m_modM;
	COptionManager * m_optionM;

public:
	CSceneDynamic();
	~CSceneDynamic();
	void LoadLevelFiles(const std::string levelName, const bool resetPlayer);
	void UnloadLevel();
	void Update(const float time, CSceneCamera * const camera);
	void LinkCamera(CSceneCamera * const camera);
	void LinkPlayer(CPlayer * const player);
	void NextAction();
	void OnLostDevice();
	void OnResetDevice();
	const int GetFacesCounter() const;
	SVertexLightPass * GetLightsGeometry() const;
	CObjectEntity * GetObjects() const;
	CPlayer * GetPlayer() const;
	std::vector<SObjectCommand> * GetObjectsCommandList();

private:
	void LoadDynamicEntities(const std::string fileName);
	void FillLightsGeometry();
	void ChangeGroupState(int groupId, int type, int state);
	void ChangeGroupTimer(int groupId, int type, float timer);
};

#endif