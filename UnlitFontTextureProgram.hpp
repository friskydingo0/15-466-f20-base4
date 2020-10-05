#pragma once

#include "GL.hpp"
#include "Load.hpp"

// based on LitColorTextureProgram.hpp

struct UnlitFontTextureProgram {
	UnlitFontTextureProgram();
	~UnlitFontTextureProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	GLuint textColor_vec3 = -1U;
};

extern Load< UnlitFontTextureProgram > unlit_font_texture_program;