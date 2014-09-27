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
#include "../../Game/Menus/CGraphicOptionMenu.h"
#include "../../Engine/Entities/2D/CFontEntity.h"
#include "../../Engine/Entities/2D/CSpriteEntity.h"
#include "../../Managers/COptionManager.h"
#include "../../Managers/CModManager.h"
#include "../../Managers/CControlManager.h"
#include "../../Misc/Global.h"

/**
* Constructor
*/
CGraphicOptionMenu::CGraphicOptionMenu() : CModificationMenuEntity(GRAPHIC_MENU_OPTION_SIZE, ID_OPTIONMENU_GRAPHICS)
{
	std::ostringstream stream;

	// Action list initialization
	this->m_actionList[GRAPHIC_MENU_OPTION_RESOLUTION] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_FULLSCREEN] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_SHADOWS] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = ID_NONE;
	this->m_actionList[GRAPHIC_MENU_OPTION_PREVIOUS] = ID_OPTIONMENU;

	// Count the number of compatible resolutions
	int numberOfResolution = 0;
	for(int i = 0 ; i < RESOLUTION_TAB_SIZE ; i++)
		if(COptionManager::m_resolutionTab[i].m_available)
			numberOfResolution++;

	// Resolution - Options
	int counter = 0;
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_RESOLUTION] = numberOfResolution;
	this->m_optionsList[GRAPHIC_MENU_OPTION_RESOLUTION] = new int[numberOfResolution];
	for(int i = 0 ; i < RESOLUTION_TAB_SIZE ; i++)
	{
		if(COptionManager::m_resolutionTab[i].m_available)
		{
			this->m_optionsList[GRAPHIC_MENU_OPTION_RESOLUTION][counter] = i;
			counter++;
		}
	}

	// Resolution - Texts
	counter = 0;
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_RESOLUTION] = new std::string[numberOfResolution];
	for(int i = 0 ; i < RESOLUTION_TAB_SIZE ; i++)
	{
		if(COptionManager::m_resolutionTab[i].m_available)
		{
			stream.str("");
			stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_RESOLUTION) << " < " << COptionManager::m_resolutionTab[i].m_width << " * " << COptionManager::m_resolutionTab[i].m_height << " >";
			this->m_optionsTextList[GRAPHIC_MENU_OPTION_RESOLUTION][counter] = stream.str();
			counter++;
		}
	}

	// Resolution - Choosen option
	for(int i = 0 ; i < numberOfResolution ; i++)
		if(this->m_optionsList[GRAPHIC_MENU_OPTION_RESOLUTION][i] == this->m_optionM->GetResolutionId(OPTIONMANAGER_CONFIGURATION_SAVED))
			this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_RESOLUTION] = i;


	// Fullscreen - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_FULLSCREEN] = GRAPHIC_MENU_OPTION_FULLSCREEN_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_FULLSCREEN] = new int[GRAPHIC_MENU_OPTION_FULLSCREEN_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_FULLSCREEN_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_FULLSCREEN][i] = i;

	// Fullscreen - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_FULLSCREEN] = new std::string[GRAPHIC_MENU_OPTION_FULLSCREEN_SIZE];
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_FULLSCREEN) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DISABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_FULLSCREEN][0] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_FULLSCREEN) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ENABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_FULLSCREEN][1] = stream.str();

	// Fullscreen - Choosen option
	if(this->m_optionM->IsFullScreen(OPTIONMANAGER_CONFIGURATION_SAVED))
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_FULLSCREEN] = 1;
	else
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_FULLSCREEN] = 0;


	// Texture quality - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_TEXTUREQUALITY] = GRAPHIC_MENU_OPTION_TEXTUREQUALITY_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY] = new int[GRAPHIC_MENU_OPTION_TEXTUREQUALITY_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_TEXTUREQUALITY_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY][i] = i;

	// Texture quality - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY] = new std::string[GRAPHIC_MENU_OPTION_TEXTUREQUALITY_SIZE];
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TEXTUREQUALITY) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_LOW) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY][0] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TEXTUREQUALITY) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_MEDIUM) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY][1] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TEXTUREQUALITY) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_HIGH) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY][2] = stream.str();

	// Texture quality - Choosen option
	this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY] = this->m_optionM->GetTextureQuality(OPTIONMANAGER_CONFIGURATION_SAVED);


	// Anisotropic filtering - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING] = GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING] = new int[GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE];
	this->m_optionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][0] = 0;
	for(int i = 1 ; i < GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][i] = (int)pow(2.0f, i);

	// Anisotropic filtering - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING] = new std::string[GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE];
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DISABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][0] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING) << " < 2 >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][1] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING) << " < 4 >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][2] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING) << " < 8 >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][3] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ANISOTROPICFILTERING) << " < 16 >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][4] = stream.str();

	// Anisotropic filtering - Choosen option
	bool found = false;
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_ANISOTROPICFILTERING_SIZE && !found ; i++)
	{
		if(this->m_optionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][i] == this->m_optionM->GetAnisotropicFiltering(OPTIONMANAGER_CONFIGURATION_SAVED))
		{
			this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING] = i;
			found = true;
		}
	}


	// Shadows - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_SHADOWS] = GRAPHIC_MENU_OPTION_SHADOWS_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_SHADOWS] = new int[GRAPHIC_MENU_OPTION_SHADOWS_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_SHADOWS_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_SHADOWS][i] = i;

	// Shadows - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_SHADOWS] = new std::string[GRAPHIC_MENU_OPTION_SHADOWS_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_SHADOWS_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_SHADOWS) << " < " << i << " >";
		this->m_optionsTextList[GRAPHIC_MENU_OPTION_SHADOWS][i] = stream.str();
	}

	// Shadows - Choosen option
	this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_SHADOWS] = this->m_optionM->GetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_SAVED);


	// Deferred shading - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = GRAPHIC_MENU_OPTION_DEFERREDSHADING_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = new int[GRAPHIC_MENU_OPTION_DEFERREDSHADING_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_DEFERREDSHADING_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING][i] = i;

	// Deferred shading - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = new std::string[GRAPHIC_MENU_OPTION_DEFERREDSHADING_SIZE];
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DEFERREDSHADING) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DISABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING][0] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DEFERREDSHADING) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ENABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING][1] = stream.str();

	// Deferred shading - Choosen option
	if(this->m_optionM->IsDeferredRenderingEnabled(OPTIONMANAGER_CONFIGURATION_SAVED))
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = 1;
	else
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING] = 0;


	// Vertical synchro - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = GRAPHIC_MENU_OPTION_VERTICALSYNCHRO_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = new int[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_VERTICALSYNCHRO_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO][i] = i;

	// Vertical synchro - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = new std::string[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO_SIZE];
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_VERTICALSYNCHRO) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_DISABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO][0] = stream.str();
	stream.str("");
	stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_VERTICALSYNCHRO) << " < " << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_ENABLED) << " >";
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO][1] = stream.str();

	// Vertical synchro - Choosen option
	if(this->m_optionM->IsVerticalSynchronizationForced(OPTIONMANAGER_CONFIGURATION_SAVED))
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = 1;
	else
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO] = 0;


	// View distance - Options
	this->m_optionsListSize[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE;
	this->m_optionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = new int[GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE ; i++)
		this->m_optionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE][i] = i * GRAPHIC_MENU_OPTION_VIEWDISTANCE_PITCH + GRAPHIC_MENU_OPTION_VIEWDISTANCE_START;

	// View distance - Texts
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = new std::string[GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE];
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE ; i++)
	{
		stream.str("");
		stream << this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_VIEWDISTANCE) << " < " << (i * GRAPHIC_MENU_OPTION_VIEWDISTANCE_PITCH + GRAPHIC_MENU_OPTION_VIEWDISTANCE_START) << " >";
		this->m_optionsTextList[GRAPHIC_MENU_OPTION_VIEWDISTANCE][i] = stream.str();
	}

	// View distance - Choosen option
	found = false;
	for(int i = 0 ; i < GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE && !found ; i++)
	{
		if((int)this->m_optionM->GetViewDistance(OPTIONMANAGER_CONFIGURATION_SAVED) <= this->m_optionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE][i])
		{
			this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = i;
			found = true;
		}
	}
	if(!found)
		this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE] = GRAPHIC_MENU_OPTION_VIEWDISTANCE_SIZE - 1;

	// Previous - Text
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_PREVIOUS] = new std::string[1];
	this->m_optionsTextList[GRAPHIC_MENU_OPTION_PREVIOUS][0] = this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_PREVIOUS);

	// Previous - Choosen option
	this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_PREVIOUS] = 0;

	stream.str("");
	stream << "Fonts/" << this->m_optionM->GetTexturePath(OPTIONMANAGER_CONFIGURATION_ACTIVE) << "/" << "default.tga";

	int centerY = this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2;
	int decal = -this->m_actionSize / 2;

	for(int i = 0 ; i < this->m_actionSize ; i++)
	{
		this->m_textList[i] = new CFontEntity(stream.str(), this->m_optionsTextList[i][this->m_choosenOptionsList[i]], 1.0f,  FONT_ALIGN_CENTER, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / 2, (int)(centerY + FONT_CHARACTER_SPACE * this->m_optionM->GetHeight(OPTIONMANAGER_CONFIGURATION_ACTIVE) * decal), 0xFFFFFFFF, 0xFFFF0000);
		decal++;
	}
	this->m_textList[0]->SetActive(true);

	SetHeader(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_TITLE_OPTIONS_GRAPHICS));
	SetFooter(stream.str(), this->m_modM->GetLangText(LANGMANAGER_TEXT_MENU_NEED_REBOOT));

	if(this->m_optionM->GetScreenFormat(OPTIONMANAGER_CONFIGURATION_ACTIVE) == SCREEN_FORMAT_STANDARD)
		this->m_sprite = new CSpriteEntity("menu_standard.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_STANDARD_RATIO));
	else
		this->m_sprite = new CSpriteEntity("menu_wide.png", D3DXVECTOR2(0,0), D3DXVECTOR2(0,0), D3DXVECTOR2(this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE, this->m_optionM->GetWidth(OPTIONMANAGER_CONFIGURATION_ACTIVE) / MENU_TEXTURE_SIZE), 1, 1, (int)MENU_TEXTURE_SIZE, (int)(MENU_TEXTURE_SIZE * SCREEN_FORMAT_WIDE));

	ProcessFontsLocation();
}

/**
* Destructor
*/
CGraphicOptionMenu::~CGraphicOptionMenu(){}

/**
* Validate the changes that happened during the frame
* @param time Elapsed time
*/
void CGraphicOptionMenu::ValidateChanges(const float time)
{
	// Validation
	if(this->m_actionSize > 0)
	{
		if((this->m_controlM->IsKeyDown(ACTION_KEY_ENTER_MENU) || (this->m_controlM->IsMouseKeyDown(ACTION_KEY_ENTER_MENU) && this->m_textList[this->m_actionChoosen]->IsSelected(this->m_controlM->GetMouseCursorX(), this->m_controlM->GetMouseCursorY()))) && this->m_textList[this->m_actionChoosen]->IsEnabled() && this->m_actionList[this->m_actionChoosen] != ID_NONE)
		{
			this->m_validationCode = this->m_actionList[this->m_actionChoosen];
			
			// Save the options
			this->m_optionM->SetResolution(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_RESOLUTION][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_RESOLUTION]]);
			this->m_optionM->SetFullScreen(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_FULLSCREEN][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_FULLSCREEN]]);
			this->m_optionM->SetTextureQuality(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_TEXTUREQUALITY]]);
			this->m_optionM->SetAnisotropicFiltering(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_ANISOTROPICFILETRING]]);
			this->m_optionM->SetMaxShadowSources(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_SHADOWS][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_SHADOWS]]);
			this->m_optionM->SetDeferredRendering(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_DEFERREDRENDERING]]);
			this->m_optionM->SetVerticalSynchronization(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VERTICALSYNCHRO]]);
			this->m_optionM->SetViewDistance(OPTIONMANAGER_CONFIGURATION_SAVED, this->m_optionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE][this->m_choosenOptionsList[GRAPHIC_MENU_OPTION_VIEWDISTANCE]]);
			this->m_optionM->Save();
		}
	}

	// Wait to avoid too much fps
	PerformWait(time);
}
