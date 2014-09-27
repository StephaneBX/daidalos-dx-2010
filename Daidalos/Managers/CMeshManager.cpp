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
#include "../Engine/Loaders/CMesh.h"
#include "../Engine/Loaders/CMeshOBJ.h"
#include "../Engine/Loaders/CMeshMS3D.h"
#include "../Engine/Loaders/CMeshMD2.h"
#include "../Managers/CLogManager.h"
#include "../Managers/CMeshManager.h"

/**
* Constructor
*/
CMeshManager::CMeshManager(){}

/**
* Destructor
*/
CMeshManager::~CMeshManager()
{
	FreeDatabase();
}

/**
* Initialize the manager
*/
void CMeshManager::Initialize()
{
	this->m_logM = CLogManager::GetInstance();
}

/**
* Free the meshs database
*/
void CMeshManager::FreeDatabase()
{
	for(std::map<std::string, CMesh *>::iterator it = this->m_meshs.begin() ; it != this->m_meshs.end() ; it++)
		SAFE_DELETE(it->second);
	this->m_meshs.clear();
}

/**
* Load a mesh and put it in the database
* @param fileName the mesh file name
* @param modDirectory The mod directory
* @return A pointer to the mesh
*/
CMesh * CMeshManager::LoadMesh(const std::string fileName, std::string modDirectory)
{
	// Check if the current mod has a custom mesh to use
	std::string finalFileName = fileName;
	std::ifstream file;
	std::ostringstream stream;
	stream << modDirectory << fileName;
	if(FileExists(stream.str()))
		finalFileName = stream.str();

	if(!MeshExists(finalFileName))
	{
		if(fileName.rfind(".obj") != std::string::npos)
			this->m_meshs[finalFileName] = new CMeshOBJ(finalFileName);
		if(fileName.rfind(".ms3d") != std::string::npos)
			this->m_meshs[finalFileName] = new CMeshMS3D(finalFileName);
		if(fileName.rfind(".md2") != std::string::npos)
			this->m_meshs[finalFileName] = new CMeshMD2(finalFileName);

		if(!this->m_meshs[finalFileName]->IsGeometryValid())
		{
			this->m_logM->GetStream() << "Error: Unable to load the mesh: " << finalFileName << ".\n";
			this->m_logM->Write();
		}
	}
	return this->m_meshs[finalFileName];
}

/**
* Check if a given mesh already exists in the database
* @param fileName The mesh file name
* @return True if the mesh is found in the database, False otherwise
*/
bool CMeshManager::MeshExists(const std::string fileName)
{
	bool ret = false;
	for(std::map<std::string, CMesh *>::iterator it = this->m_meshs.begin() ; it != this->m_meshs.end() ; it++)
		if(it->first == fileName)
			ret = true;
	return ret;
}
