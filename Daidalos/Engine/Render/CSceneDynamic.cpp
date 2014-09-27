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

#include <sstream>
#include "../../Misc/Global.h"
#include "../../Game/CPlayer.h"
#include "../../Game/Objects/Dynamic/CAlarmLight.h"
#include "../../Game/Objects/Dynamic/CHelix.h"
#include "../../Game/Objects/Dynamic/CBars.h"
#include "../../Game/Objects/Dynamic/CDwarf.h"
#include "../../Game/Objects/Doors/CPlainDoor1.h"
#include "../../Game/Objects/Doors/CPlainDoor3.h"
#include "../../Game/Objects/Doors/CPlainDoor5.h"
#include "../../Game/Objects/Doors/CPlainLockedDoor1.h"
#include "../../Game/Objects/Doors/CPlainLockedDoor3.h"
#include "../../Game/Objects/Doors/CPlainLockedDoor5.h"
#include "../../Game/Objects/Doors/CBarsLockedDoor.h"
#include "../../Game/Objects/Doors/CHiddenLockedDoor.h"
#include "../../Game/Objects/Doors/CCardReader.h"
#include "../../Game/Objects/Doors/CSwitch1.h"
#include "../../Game/Objects/Doors/CInvisibleSwitch.h"
#include "../../Game/Objects/Usable/CMagneticCard.h"
#include "../../Game/Objects/Usable/CShotgunAmmo.h"
#include "../../Game/Objects/Usable/CHandgunAmmo.h"
#include "../../Game/Objects/Usable/CArmorBonus.h"
#include "../../Game/Objects/Usable/CHealthBonus.h"
#include "../../Game/Objects/Usable/CFlashlightBonus.h"
#include "../../Managers/CLogManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/COptionManager.h"
#include "../../Engine/CTokenizer.h"
#include "../../Engine/Render/CEntityRender.h"
#include "../../Engine/Render/CSceneCamera.h"
#include "../../Engine/Render/CSceneDynamic.h"

/**
* Constructor
*/
CSceneDynamic::CSceneDynamic()
{
	this->m_facesCounter = 0;
	this->m_dynamicObjectsBuffer = NULL;
	this->m_lightsGeometry = NULL;
	this->m_player = NULL;

	this->m_logM = CLogManager::GetInstance();
	this->m_modM = CModManager::GetInstance();
	this->m_optionM = COptionManager::GetInstance();
}

/**
* Destructor
*/
CSceneDynamic::~CSceneDynamic()
{
	UnloadLevel();
	SAFE_DELETE(this->m_player);
}

/**
* Load the level files
* @param levelName The level files directory
* @param resetPlayer If the player state need to be reset
*/
void CSceneDynamic::LoadLevelFiles(const std::string levelName, const bool resetPlayer)
{
	std::ostringstream stream;
	std::string tmpString;

	if(resetPlayer)
		SAFE_DELETE(this->m_player);

	UnloadLevel();

	stream.str("");
	stream << levelName << "/" << FILE_DYNAMIC;
	tmpString = stream.str();
	LoadDynamicEntities(tmpString);

	// Faces Calculation
	this->m_facesCounter = 0;
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetRender() != NULL)
		{
			for(int i = 0 ; i < MAX_MESH_PER_OBJECT ; i++)
				this->m_facesCounter += tmpDObject->GetRender()->GetGeometrySize(i);
		}
		tmpDObject = tmpDObject->m_next;
	}

	// Buffers initialization
	if(this->m_facesCounter > 0)
		this->m_lightsGeometry = new SVertexLightPass[this->m_facesCounter];
}

/**
* Unload the levels
*/
void CSceneDynamic::UnloadLevel()
{
	// Unload Dynamic objects
	CObjectEntity * tmpDObject;
	while(this->m_dynamicObjectsBuffer != NULL)
	{
		tmpDObject = this->m_dynamicObjectsBuffer;
		this->m_dynamicObjectsBuffer = this->m_dynamicObjectsBuffer->m_next;
		delete tmpDObject;
	}

	SAFE_DELETE_ARRAY(this->m_lightsGeometry);
}

/**
* Update the scene
* @param time Elapsed time
* @param camera The camera
*/
void CSceneDynamic::Update(const float time, CSceneCamera * const camera)
{
	// Update the light geometry for the shadows
	this->m_facesCounter = 0;

	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		// Update the object if it is alive
		if(tmpDObject->IsAlive())
		{
			tmpDObject->Update(time);
			this->m_commands.push_back(tmpDObject->GetCommand());
		
			if(tmpDObject->GetRender() != NULL && this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_ACTIVE) > 0)
				tmpDObject->GetRender()->FillLightsGeometry(this->m_lightsGeometry, &this->m_facesCounter);
		}
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Link the camera with the objects
* Used to keep track of the camera in order to perform visibility tests
* @param camera The camera
*/
void CSceneDynamic::LinkCamera(CSceneCamera * const camera)
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetRender() != NULL)
			tmpDObject->GetRender()->LinkCamera(camera);
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Link the player with the objects
* Used to keep track of the camera in order to perform updates
* @param player The player
*/
void CSceneDynamic::LinkPlayer(CPlayer * const player)
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		tmpDObject->LinkPlayer(player);
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Update the objects state depending on their type
*/
void CSceneDynamic::NextAction()
{
	int groupId;
	int state;
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;

	while(tmpDObject != NULL)
	{
		if(tmpDObject->IsCollidedByPlayer())
		{
			groupId = tmpDObject->GetGroup();
			state = tmpDObject->GetState();

			switch(tmpDObject->GetType())
			{
			case OBJECT_TYPE_DOOR:
				if(state == OBJECT_STATE_DOOR_CLOSED)
				{
					if(groupId != OBJECT_GROUP_ID_NONE)
						ChangeGroupState(groupId, OBJECT_TYPE_DOOR, OBJECT_STATE_DOOR_OPENING);
					else
						tmpDObject->SetState(OBJECT_STATE_DOOR_OPENING);
				}
				else
				{
					if(state == OBJECT_STATE_DOOR_OPENED)
					{
						if(groupId != OBJECT_GROUP_ID_NONE)
							ChangeGroupTimer(groupId, OBJECT_TYPE_DOOR, 0);
						else
							tmpDObject->SetTimer(0);
					}
				}
				break;
			
			case OBJECT_TYPE_SWITCH:
				if(state == OBJECT_STATE_SWITCH_OFF && ((CObjectEntitySwitch*)tmpDObject)->CanBeActivated())
				{
					if(groupId != OBJECT_GROUP_ID_NONE)
					{
						ChangeGroupState(groupId, OBJECT_TYPE_LOCKED_DOOR, OBJECT_STATE_LOCKED_DOOR_OPENING);
						tmpDObject->SetState(OBJECT_STATE_SWITCH_ON);
					}
				}
				break;

			case OBJECT_TYPE_USABLE:
				if(state == OBJECT_STATE_USABLE_UNUSED)
					tmpDObject->SetState(OBJECT_STATE_USABLE_USING);
				break;
			}
		}
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Action performed when a device lost event is detected
* Free any ressource that need to be before reseting the device
*/
void CSceneDynamic::OnLostDevice()
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;

	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetRender() != NULL)
			tmpDObject->GetRender()->DestroyGeometry();
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Action performed when a device lost reset is detected
* Allocate the necessary ressources
*/
void CSceneDynamic::OnResetDevice()
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;

	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetRender() != NULL)
			tmpDObject->GetRender()->InitializeGeometry();
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Get the number of faces
* @return The number of faces
*/
const int CSceneDynamic::GetFacesCounter() const
{
	return this->m_facesCounter;
}

/**
* Get the currently active objects
* @return A buffer containing all the active objects
*/
CObjectEntity * CSceneDynamic::GetObjects() const
{
	return this->m_dynamicObjectsBuffer;
}

/**
* Get the player
* @return The player
*/
CPlayer * CSceneDynamic::GetPlayer() const
{
	return this->m_player;
}

/**
* Get the objects commands list
* @return The objects commands list
*/
std::vector<SObjectCommand> *  CSceneDynamic::GetObjectsCommandList()
{
	return &this->m_commands;
}

/**
* Get the light geometry
* @return The light geometry
*/
SVertexLightPass * CSceneDynamic::GetLightsGeometry() const
{
	return this->m_lightsGeometry;
}

/**
* Load the dynamic information of a level
* @param fileName The file name
*/
void CSceneDynamic::LoadDynamicEntities(const std::string fileName)
{
	std::string entity;
	D3DXVECTOR3 position, rotation;
	float scale;
	int groupId, color;

	CTokenizer tokenizer(fileName);
	if(tokenizer.m_tokens.size() > 0)
	{
		// Light properties found
		tokenizer.Tokenize(" ");
		for(unsigned int i = 0 ; i < tokenizer.m_tokens.size() ; )
		{
			if(tokenizer.m_tokens.at(i) == "object")
			{
				// New object, initialization of the variables and get the object name
				position = D3DXVECTOR3(0,0,0);
				rotation = D3DXVECTOR3(0,-1,0);
				scale = 1.0f;
				groupId = OBJECT_GROUP_ID_NONE;
				color = OBJECT_MAGNETIC_CARD_NONE;
				entity = tokenizer.m_tokens.at(i + 1);
				i += 2;
			}
			else
			{
				if(tokenizer.m_tokens.at(i) == "position")
				{
					// Read position
					position = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
					i += 4;
				}
				else
				{
					if(tokenizer.m_tokens.at(i) == "rotation")
					{
						// Read rotation
						rotation = D3DXVECTOR3((float)atof(tokenizer.m_tokens.at(i + 1).c_str()), (float)atof(tokenizer.m_tokens.at(i + 2).c_str()), (float)atof(tokenizer.m_tokens.at(i + 3).c_str()));
						i += 4;
					}
					else
					{
						if(tokenizer.m_tokens.at(i) == "scale")
						{
							// Read scale
							scale = (float)atof(tokenizer.m_tokens.at(i + 1).c_str());
							i += 2;
						}
						else
						{
							// Group Id for doors
							if(tokenizer.m_tokens.at(i) == "group")
							{
								groupId = atoi(tokenizer.m_tokens.at(i + 1).c_str());
								i += 2;
							}
							else
							{
								// Color for locked doors, card readers and magnetic cards
								if(tokenizer.m_tokens.at(i) == "color")
								{
									if(tokenizer.m_tokens.at(i + 1) == "red")
									{
										color = OBJECT_MAGNETIC_CARD_RED;
									}
									else
									{
										if(tokenizer.m_tokens.at(i + 1) == "blue")
										{
											color = OBJECT_MAGNETIC_CARD_BLUE;
										}
										else
										{
											if(tokenizer.m_tokens.at(i + 1) == "green")
											{
												color = OBJECT_MAGNETIC_CARD_GREEN;
											}
											else
											{
												if(tokenizer.m_tokens.at(i + 1) == "yellow")
												{
													color = OBJECT_MAGNETIC_CARD_YELLOW;
												}
												else
												{
													color = OBJECT_MAGNETIC_CARD_NONE;
												}
											}
										}
									}
									i += 2;
								}
								else
								{
									if(tokenizer.m_tokens.at(i) == "end")
									{
										// End of the object propertie, add the object if the entity is known
										if(entity == "alarm_light")
										{
											CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
											this->m_dynamicObjectsBuffer = new CAlarmLight(position, rotation);
											this->m_dynamicObjectsBuffer->m_next = tmpObj;
										}
										else
										{
											if(entity == "shotgun_ammo")
											{
												CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
												this->m_dynamicObjectsBuffer = new CShotgunAmmo(position);
												this->m_dynamicObjectsBuffer->m_next = tmpObj;
											}
											else
											{
												if(entity == "helix")
												{
													CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
													this->m_dynamicObjectsBuffer = new CHelix(position, rotation);
													this->m_dynamicObjectsBuffer->m_next = tmpObj;
												}
												else
												{
													if(entity == "bars_locked_door")
													{
														CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
														this->m_dynamicObjectsBuffer = new CBarsLockedDoor(position, rotation, groupId);
														this->m_dynamicObjectsBuffer->m_next = tmpObj;
													}
													else
													{
														if(entity == "dwarf")
														{
															CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
															this->m_dynamicObjectsBuffer = new CDwarf(position, rotation);
															this->m_dynamicObjectsBuffer->m_next = tmpObj;
														}
														else
														{
															if(entity == "plain_door1")
															{
																CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																this->m_dynamicObjectsBuffer = new CPlainDoor1(position, rotation, groupId);
																this->m_dynamicObjectsBuffer->m_next = tmpObj;
															}
															else
															{
																if(entity == "plain_door3")
																{
																	CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																	this->m_dynamicObjectsBuffer = new CPlainDoor3(position, rotation, groupId);
																	this->m_dynamicObjectsBuffer->m_next = tmpObj;
																}
																else
																{
																	if(entity == "plain_door5")
																	{
																		CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																		this->m_dynamicObjectsBuffer = new CPlainDoor5(position, rotation, groupId);
																		this->m_dynamicObjectsBuffer->m_next = tmpObj;
																	}
																	else
																	{
																		if(entity == "plain_locked_door1")
																		{
																			CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																			this->m_dynamicObjectsBuffer = new CPlainLockedDoor1(position, rotation, groupId, color);
																			this->m_dynamicObjectsBuffer->m_next = tmpObj;
																		}
																		else
																		{
																			if(entity == "plain_locked_door3")
																			{
																				CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																				this->m_dynamicObjectsBuffer = new CPlainLockedDoor3(position, rotation, groupId, color);
																				this->m_dynamicObjectsBuffer->m_next = tmpObj;
																			}
																			else
																			{
																				if(entity == "plain_locked_door5")
																				{
																					CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																					this->m_dynamicObjectsBuffer = new CPlainLockedDoor5(position, rotation, groupId, color);
																					this->m_dynamicObjectsBuffer->m_next = tmpObj;
																				}
																				else
																				{
																					if(entity == "card_reader")
																					{
																						CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																						this->m_dynamicObjectsBuffer = new CCardReader(position, rotation, groupId, color);
																						this->m_dynamicObjectsBuffer->m_next = tmpObj;
																					}
																					else
																					{
																						if(entity == "magnetic_card")
																						{
																							CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																							this->m_dynamicObjectsBuffer = new CMagneticCard(position, color);
																							this->m_dynamicObjectsBuffer->m_next = tmpObj;
																						}
																						else
																						{
																							if(entity == "handgun_ammo")
																							{
																								CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																								this->m_dynamicObjectsBuffer = new CHandgunAmmo(position);
																								this->m_dynamicObjectsBuffer->m_next = tmpObj;
																							}
																							else
																							{
																								if(entity == "armor_bonus")
																								{
																									CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																									this->m_dynamicObjectsBuffer = new CArmorBonus(position);
																									this->m_dynamicObjectsBuffer->m_next = tmpObj;
																								}
																								else
																								{
																									if(entity == "health_bonus")
																									{
																										CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																										this->m_dynamicObjectsBuffer = new CHealthBonus(position);
																										this->m_dynamicObjectsBuffer->m_next = tmpObj;
																									}
																									else
																									{
																										if(entity == "flashlight")
																										{
																											CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																											this->m_dynamicObjectsBuffer = new CFlashlightBonus(position);
																											this->m_dynamicObjectsBuffer->m_next = tmpObj;
																										}
																										else
																										{
																											if(entity == "switch1")
																											{
																												CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																												this->m_dynamicObjectsBuffer = new CSwitch1(position, rotation, groupId);
																												this->m_dynamicObjectsBuffer->m_next = tmpObj;
																											}
																											else
																											{
																												if(entity == "invisible_switch")
																												{
																													CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																													this->m_dynamicObjectsBuffer = new CInvisibleSwitch(position, rotation, groupId, 10);
																													this->m_dynamicObjectsBuffer->m_next = tmpObj;
																												}
																												else
																												{
																													if(entity == "hidden_locked_door")
																													{
																														CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																														this->m_dynamicObjectsBuffer = new CHiddenLockedDoor(position, rotation, groupId);
																														this->m_dynamicObjectsBuffer->m_next = tmpObj;
																													}
																													else
																													{
																														if(entity == "bars")
																														{
																															CObjectEntity * tmpObj = this->m_dynamicObjectsBuffer;
																															this->m_dynamicObjectsBuffer = new CBars(position, rotation);
																															this->m_dynamicObjectsBuffer->m_next = tmpObj;
																														}
																														else
																														{
																															if(entity == "player")
																															{
																																if(this->m_player == NULL)
																																	this->m_player = new CPlayer(position, rotation, this->m_modM->GetPlayerStartingHealth(), this->m_modM->GetPlayerStartingArmor());
																																else
																																	this->m_player->SetRespawnPoint(position, rotation);
																															}
																														}
																													}
																												}
																											}
																										}
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
										i++;
									}
									else
										i++;
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
* Change the state of an object group
* @param groupId The group id
* @param type The type of object to change
* @param state The new state
*/
void CSceneDynamic::ChangeGroupState(int groupId, int type, int state)
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetType() == type && tmpDObject->GetGroup() == groupId)
			tmpDObject->SetState(state);
		tmpDObject = tmpDObject->m_next;
	}
}

/**
* Change the timer of an object group
* @param groupId The group id
* @param type The type of object to change
* @param timer The new timer
*/
void CSceneDynamic::ChangeGroupTimer(int groupId, int type, float timer)
{
	CObjectEntity * tmpDObject = this->m_dynamicObjectsBuffer;
	while(tmpDObject != NULL)
	{
		if(tmpDObject->GetType() == type && tmpDObject->GetGroup() == groupId)
			tmpDObject->SetTimer(timer);
		tmpDObject = tmpDObject->m_next;
	}
}
