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

#ifndef __CSHADER_H
#define __CSHADER_H

/**
* The available shader type
*/
enum shaderType
{
	SHADER_TYPE_DS_FILL_SHADOW_MAP,
	SHADER_TYPE_DS_FILL_WORLD,
	SHADER_TYPE_DS_FILL_WORLD_NOSHADOWS,
	SHADER_TYPE_DS_FILL_LIGHTNING,
	SHADER_TYPE_DS_FILL_LIGHTNING_NOSHADOWS,
	SHADER_TYPE_DS_RENDER_WORLD,
	SHADER_TYPE_FORWARD_PARTICLES
};

class CDeviceManager;
class CLogManager;

/**
* CShader class.
* Generic shader class.
*/
class CShader
{
protected:
	D3DXHANDLE * m_vertexHandles;
	D3DXHANDLE * m_pixelHandles;
	LPDIRECT3DVERTEXSHADER9 m_vertexShader;
	LPD3DXCONSTANTTABLE  m_constantTableVS;
	LPDIRECT3DPIXELSHADER9 m_pixelShader;
	LPD3DXCONSTANTTABLE m_constantTablePS;

	CDeviceManager * m_deviceM;
	CLogManager * m_logM;

public:
	CShader(const std::string name, const std::string modDirectory);
	~CShader();
	LPDIRECT3DVERTEXSHADER9 GetVertexShader() const;
	LPDIRECT3DPIXELSHADER9 GetPixelShader() const;
	LPD3DXCONSTANTTABLE GetConstantTableVS() const;
	LPD3DXCONSTANTTABLE GetConstantTablePS() const;
	D3DXHANDLE GetVertexHandle(const int i) const;
	D3DXHANDLE GetPixelHandle(const int i) const;
};

#endif
