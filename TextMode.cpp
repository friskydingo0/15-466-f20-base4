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

	// loading in from a json file
	try {
		std::ifstream in("test.json");
		in >> json;
	}
	catch (int e) {
		std::cout << "A json exception occurred. Your json file is most likely formatted incorrectly. Exception #: " << e << std::endl;
	}

	// for converting from a string to an int: http://www.cplusplus.com/reference/string/stoi/
	std::string::size_type sz;   // alias of size_t

	// assigning the start of the json file
	next_text = "start";
	json_string = json[next_text]["string"][0];
	std::cout << std::endl << json_string << std::endl;
	for (int i = 1; i <= std::stoi((std::string)json[next_text]["choices"][0], &sz); i++) {
		json_choices.emplace_back((std::string)json[next_text]["choices"][i]);
		std::cout << i % 10 << ": " << json_choices[i - 1] << std::endl;
		json_results.emplace_back((std::string)json[next_text]["results"][i]);
	}
}

TextMode::~TextMode() {}

bool TextMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_1 && !one.pressed) {
			update_json_strings(0);
			one.downs += 1;
			one.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_2 && !two.pressed) {
			update_json_strings(1);
			two.downs += 1;
			two.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_3 && !three.pressed) {
			update_json_strings(2);
			three.downs += 1;
			three.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_4 && !four.pressed) {
			update_json_strings(3);
			four.downs += 1;
			four.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_5 && !five.pressed) {
			update_json_strings(4);
			five.downs += 1;
			five.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_6 && !six.pressed) {
			update_json_strings(5);
			six.downs += 1;
			six.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_7 && !seven.pressed) {
			update_json_strings(6);
			seven.downs += 1;
			seven.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_8 && !eight.pressed) {
			update_json_strings(7);
			eight.downs += 1;
			eight.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_9 && !nine.pressed) {
			update_json_strings(8);
			nine.downs += 1;
			nine.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_0 && !zero.pressed) {
			update_json_strings(9);
			zero.downs += 1;
			zero.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RETURN && !enter.pressed) {
			if (json_results.size() == 0) {
				// for converting from a string to an int: http://www.cplusplus.com/reference/string/stoi/
				std::string::size_type sz;   // alias of size_t
				next_text = "start";
				json_string = json[next_text]["string"][0];
				std::cout << std::endl << json_string << std::endl;
				for (int i = 1; i <= std::stoi((std::string)json[next_text]["choices"][0], &sz); i++) {
					json_choices.emplace_back((std::string)json[next_text]["choices"][i]);
					std::cout << i % 10 << ": " << json_choices[i - 1] << std::endl;
					json_results.emplace_back((std::string)json[next_text]["results"][i]);
				}
			}
			enter.downs += 1;
			enter.pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_1) {
			one.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_2) {
			two.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_3) {
			three.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_4) {
			four.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_5) {
			five.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_6) {
			six.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_7) {
			seven.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_8) {
			eight.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_9) {
			nine.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_0) {
			zero.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RETURN) {
			enter.pressed = false;
			return true;
		}
	}

	return false;
}

void TextMode::update_json_strings(int choice) {
	if (choice < json_results.size()) {
		// for converting from a string to an int: http://www.cplusplus.com/reference/string/stoi/
		std::string::size_type sz;   // alias of size_t

		// for actually displaying the next text line (currently just in the console)
		next_text = json_results[choice];

		// updating and displaying json_string (in the console)
		json_string = json[next_text]["string"][0];
		std::cout << std::endl << json_string << std::endl;

		// updating and displaying json_choices (in the console); updating json_results
		json_choices.clear();
		json_results.clear();
		for (int i = 1; i <= std::stoi((std::string)json[next_text]["choices"][0], &sz); i++) {
			json_choices.emplace_back((std::string)json[next_text]["choices"][i]);
			std::cout << i % 10 << ": " << json_choices[i - 1] << std::endl;
			json_results.emplace_back((std::string)json[next_text]["results"][i]);
		}
	}
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

	GL_ERRORS();
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