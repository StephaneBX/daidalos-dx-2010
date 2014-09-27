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

#include <d3d9.h>
#include <d3dx9.h>
#include <sstream>
#include <string>
#include "../Misc/Global.h"
#include "../managers/CDeviceManager.h"
#include "../managers/CLogManager.h"
#include "../shaders/CShader.h"

/**
* Constructor
* @param name The shader name
* @param modDirectory The mod directory
*/
CShader::CShader(const std::string name, const std::string modDirectory)
{
	std::ostringstream stream;
	std::string tmpString;
	this->m_deviceM = CDeviceManager::GetInstance();
	this->m_logM = CLogManager::GetInstance();

	HRESULT hr;
    LPD3DXBUFFER pCode;
    DWORD dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL1;
	LPD3DXBUFFER pBufferErrors = NULL;

	// Load the vertex shader
	stream << modDirectory << DIRECTORY_SHADERS << name << ".vsh";
	tmpString = stream.str();

	if(!FileExists(tmpString))
	{
		stream.str("");
		stream << DIRECTORY_SHADERS << name << ".vsh";
		tmpString = stream.str();
	}

	hr = D3DXCompileShaderFromFile(tmpString.c_str(), NULL, NULL, "main", "vs_3_0", dwShaderFlags, &pCode, &pBufferErrors, &this->m_constantTableVS);
    if(FAILED(hr))
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		this->m_logM->GetStream() << "Vertex Shader Compilation error: " << (const char*)pCompilErrors << "\n";
		this->m_logM->Write();
	}
	this->m_deviceM->GetDevice()->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),&this->m_vertexShader);
    pCode->Release();

	// Load the pixel shader
	stream.str("");
	stream << modDirectory << DIRECTORY_SHADERS << name << ".psh";
	tmpString = stream.str();

	if(!FileExists(tmpString))
	{
		stream.str("");
		stream << DIRECTORY_SHADERS << name << ".psh";
		tmpString = stream.str();
	}

	hr = D3DXCompileShaderFromFile(tmpString.c_str(), NULL, NULL, "main", "ps_3_0", dwShaderFlags, &pCode, &pBufferErrors, &this->m_constantTablePS );
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		this->m_logM->GetStream() << "Pixel Shader Compilation error: " << (const char*)pCompilErrors << "\n";
		this->m_logM->Write();
	}
    this->m_deviceM->GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &this->m_pixelShader);
	pCode->Release();
}

/**
* Destructor
*/
CShader::~CShader()
{
	SAFE_RELEASE(this->m_vertexShader);
	SAFE_RELEASE(this->m_constantTableVS);
	SAFE_RELEASE(this->m_pixelShader);
	SAFE_RELEASE(this->m_constantTablePS);

	SAFE_DELETE_ARRAY(this->m_pixelHandles);
	SAFE_DELETE_ARRAY(this->m_vertexHandles);
}

/**
* Get the Vertex Shader
* @return The Vertex Shader
*/
LPDIRECT3DVERTEXSHADER9 CShader::GetVertexShader() const
{
	return this->m_vertexShader;
}

/**
* Get the Pixel Shader
* @return The Pixel Shader
*/
LPDIRECT3DPIXELSHADER9 CShader::GetPixelShader() const
{
	return this->m_pixelShader;
}

/**
* Get the Vertex Shader Constant Table
* @return The Vertex Shader Constant Table
*/
LPD3DXCONSTANTTABLE CShader::GetConstantTableVS() const
{
	return this->m_constantTableVS;
}

/**
* Get the Pixel Shader Constant Table
* @return The Pixel Shader Constant Table
*/
LPD3DXCONSTANTTABLE CShader::GetConstantTablePS() const
{
	return this->m_constantTablePS;
}

/**
* Get a Vertex Shader handle
* @param i The Vertex Shader handle number
* @return The Vertex Shader handle
*/
D3DXHANDLE CShader::GetVertexHandle(const int i) const
{
	return this->m_vertexHandles[i];
}

/**
* Get a Pixel Shader handle
* @param i The Pixel Shader handle number
* @return The Pixel Shader handle
*/
D3DXHANDLE CShader::GetPixelHandle(const int i) const
{
	return this->m_pixelHandles[i];
}
