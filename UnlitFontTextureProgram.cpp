#include "UnlitFontTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include "glm/gtc/matrix_transform.hpp"

// based on https://learnopengl.com/In-Practice/Text-Rendering and LitColorTextureProgram.cpp

Load< UnlitFontTextureProgram > unlit_font_texture_program(LoadTagEarly, []() -> UnlitFontTextureProgram const * {
	UnlitFontTextureProgram *ret = new UnlitFontTextureProgram();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);	// flat projection for text

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return ret;
});

UnlitFontTextureProgram::UnlitFontTextureProgram() {
	program = gl_compile_program(
		// vertex shader
		"#version 330 core\n"
		"layout (location = 0) in vec4 vertex;\n"
		"out vec2 TexCoords;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
		"    TexCoords = vertex.zw;\n"
		"}\n"
		,
		// fragment shader
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"void main()\n"
		"{\n"
    	"    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    	"    color = vec4(textColor, 1.0) * sampled;\n"
		"}\n"
	);

	Position_vec4 = glGetAttribLocation(program, "vertex");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	GLuint TEX_sampler2D = glGetUniformLocation(program, "text");

	glUseProgram(program);

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0);
}

UnlitFontTextureProgram::~UnlitFontTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}