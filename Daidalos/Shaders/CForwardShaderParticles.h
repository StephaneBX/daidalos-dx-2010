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

#ifndef __CFORWARDSHADERPARTICLES_H
#define __CFORWARDSHADERPARTICLES_H

#include "../Shaders/CShader.h"

enum ForwardShaderParticlesVertexHandles
{
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_SPRITESPERLINE,
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_BILLBOARD,
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_VIEWPROJ,
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_LIGHTVIEWPROJ,
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_LIGHTSHADOWCASTERINDEXES,
	FORWARDSHADERPARTICLES_HANDLE_VERTEX_SIZE
};

enum ForwardShaderParticlesPixelHandles
{
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_SHADOWCONFIGURATION,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTNINGCONFIGURATION,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_CAMERAPOSITIONDISTANCE,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTPOSITIONANGLE,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTDIRECTIONLENGTH,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTCOLOR,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTFLAGS,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_LIGHTSHADOWCASTERINDEXES,
	FORWARDSHADERPARTICLES_HANDLE_PIXEL_SIZE
};

/**
* CForwardShaderParticles class.
* Shader used to render the particles with forward rendering technic.
*/
class CForwardShaderParticles : public CShader
{
public:
	CForwardShaderParticles(const std::string modDirectory);
	~CForwardShaderParticles();
};

#endif
