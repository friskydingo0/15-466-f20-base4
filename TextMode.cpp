#include "TextMode.hpp"

#include "UnlitFontTextureProgram.hpp"

#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

TextMode::TextMode(){

	buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, "Sampler", -1, 0, -1);
	
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	FT_Library  library;
	
	auto error = FT_Init_FreeType( &library );
	if ( error )
	{ std::cout << error << std::endl; }

	error = FT_New_Face( library, data_path("American Desktop.ttf").c_str(), 0, &face );
	if ( error )
	{ std::cout << error << std::endl; }

	error = FT_Set_Char_Size( face, 0, 16*64, 0, 0 );
	if ( error )
	{ std::cout << error << std::endl; }

	font = hb_ft_font_create(face, nullptr);

	hb_shape(font, buf, NULL, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	FT_GlyphSlot slot = face->glyph;
	
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
	double cursor_x = 0.0, cursor_y = 0.0;

	for (unsigned int i = 0; i < glyph_count; ++i) {
		auto glyphid = glyph_info[i].codepoint;
		auto x_offset = glyph_pos[i].x_offset / 64.0;
		auto y_offset = glyph_pos[i].y_offset / 64.0;
		auto x_advance = glyph_pos[i].x_advance / 64.0;
		auto y_advance = glyph_pos[i].y_advance / 64.0;

		if ( FT_Load_Glyph( face, glyphid, FT_LOAD_RENDER ) )
			std::cout << "ERROR: Couldn't load glyph/bitmap" << std::endl;

		assert(face->glyph != nullptr && "Glyph is not loaded");

		// generate textures for each glyph
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character font_char = {textureId, x_offset, y_offset, x_advance, y_advance};

		char_map.emplace(glyphid, font_char);
		
		std::cout<< glyphid << "|" << cursor_x + x_offset << ", " << cursor_y + y_offset << std::endl;

		cursor_x += x_advance;
		cursor_y += y_advance;
	}
	
	

	hb_buffer_destroy(buf);
	hb_font_destroy(font);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void TextMode::draw(glm::uvec2 const &drawable_size) {
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
	}

	// Call draw_text()
}

void TextMode::draw_text(std::string text, float x, float y, float scale, glm::vec3 color)
{
	glUseProgram(unlit_font_texture_program->program);
	
	
	// ---------- Work in progress ------------
	/*glUniform3f(glGetUniformLocation(unlit_font_texture_program->program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);*/

	glUseProgram(0);
}