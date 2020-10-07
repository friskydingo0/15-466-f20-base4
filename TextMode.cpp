#include "TextMode.hpp"

#include "UnlitFontTextureProgram.hpp"

#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

// font stuff - based on harfbuzz tutorial & https://learnopengl.com/In-Practice/Text-Rendering

TextMode::TextMode(){

	// Font stuff init
	buf = hb_buffer_create();
	
	auto error = FT_Init_FreeType( &library );
	if ( error )
	{ std::cout << "ERROR::FREETYTPE:library" << std::endl; }

	error = FT_New_Face( library, data_path(font_name).c_str(), 0, &face ); //"C:\\Windows\\Fonts\\Arial.ttf"
	if ( error )
	{ std::cout << "ERROR::FREETYTPE:face" << std::endl; }

	error = FT_Set_Char_Size( face, 0, 32*64, 0, 0 );
	if ( error )
	{ std::cout << "ERROR::FREETYTPE:charsize" << std::endl; }

	font = hb_ft_font_create(face, nullptr);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	// End font stuff init
}

TextMode::~TextMode() {
	hb_buffer_destroy(buf);
	hb_font_destroy(font);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void TextMode::draw(glm::uvec2 const &drawable_size) {
	
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(unlit_font_texture_program->program);
	glm::mat4 projection = glm::ortho(0.0f, float(drawable_size.x), 0.0f, float(drawable_size.y));
	glUniformMatrix4fv(glGetUniformLocation(unlit_font_texture_program->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	// Do all text rendering calls here
	draw_text("Font name : " + font_name, 25.0f, drawable_size.y * 0.9f, 1.5f, glm::vec3(0.5, 0.8f, 0.2f));
	
	draw_text("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLM􀃺OPQRSTUVWXYZ", 0.0f, drawable_size.y*0.5f, 1.0f, glm::vec3(1.0f,1.0f,0.0f));
	draw_text("1234567890.:,; ' \" (!?) +-*/=", 0.0f, drawable_size.y * 0.5f - 32.0f, 1.0f, glm::vec3(0.0f,1.0f,1.0f));
	
	glUseProgram(0);
}

void TextMode::draw_text(std::string text, float x, float y, float scale, glm::vec3 color)
{
	
	glUseProgram(unlit_font_texture_program->program);

	// ---------- Work in progress ------------
	glUniform3f(glGetUniformLocation(unlit_font_texture_program->program, "textColor"), color.x, color.y, color.z);
	
    glActiveTexture(GL_TEXTURE0);
	
    glBindVertexArray(unlit_font_texture_program->VAO);
	
	hb_buffer_clear_contents(buf);
	hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));
	hb_shape(font, buf, NULL, 0);

	FT_GlyphSlot slot = face->glyph;
	unsigned int length = static_cast<unsigned int>(text.size());
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &length);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &length);
	
	for (size_t i = 0; i < text.length(); i++)
	{
		auto glyphid = glyph_info[i].codepoint;
		auto x_offset = glyph_pos[i].x_offset / 64.0f;
		auto y_offset = glyph_pos[i].y_offset / 64.0f;
		auto x_advance = glyph_pos[i].x_advance / 64.0f;
		auto y_advance = glyph_pos[i].y_advance / 64.0f;

		// std::cout << x_offset << " " << x_advance << " " << y_offset << " " << y_advance << std::endl;

		if ( FT_Load_Glyph( face, glyphid, FT_LOAD_RENDER ) )
			std::cout << "ERROR: Couldn't load glyph/bitmap" << std::endl;

		assert(slot != nullptr && "Glyph is not loaded");

		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			slot->bitmap.width,
			slot->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Add the rendering code here
		float xpos = x + x_offset * scale;
        float ypos = y - y_offset * scale;//(ch.Size.y - ch.Bearing.y) * scale;

        float w = slot->bitmap.width * scale;
        float h = slot->bitmap.rows * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, unlit_font_texture_program->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

		x += x_advance * scale;
		y += y_advance * scale;
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}