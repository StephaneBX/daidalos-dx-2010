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

#ifndef __CDSSHADERFILLWORLD_H
#define __CDSSHADERFILLWORLD_H

#include "../Shaders/CShader.h"

enum DSShaderFillWorldVertexHandles
{
	DSSHADERFILLWORLD_HANDLE_VERTEX_VIEWMATRIX,
	DSSHADERFILLWORLD_HANDLE_VERTEX_VIEWPROJMATRIX,
	DSSHADERFILLWORLD_HANDLE_VERTEX_LIGHTVIEWPROJ,
	DSSHADERFILLWORLD_HANDLE_VERTEX_SHADOWCASTER,
	DSSHADERFILLWORLD_HANDLE_VERTEX_SIZE
};

enum DSShaderFillWorldPixelHandles
{
	DSSHADERFILLWORLD_HANDLE_PIXEL_SHADOWCONFIGURATION,
	DSSHADERFILLWORLD_HANDLE_PIXEL_VIEWDISTANCE,
	DSSHADERFILLWORLD_HANDLE_PIXEL_SHADOWCASTER,
	DSSHADERFILLWORLD_HANDLE_PIXEL_SIZE
};

/**
* CDSShaderFillWorld class.
* Shader used to fill the world with the Deferred Shading technic.
*/
class CDSShaderFillWorld : public CShader
{
public:
	CDSShaderFillWorld(const std::string modDirectory);
	~CDSShaderFillWorld();
};

#endif
