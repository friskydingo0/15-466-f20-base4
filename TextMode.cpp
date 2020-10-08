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

	// loading in from a json file
	try {
		std::ifstream in("story.json");
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
	// code for splitting a string based on a delimiter came from here: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139
	std::string delimiter = "\n";
	size_t pos = 0;
	std::string token;
	while ((pos = json_string.find(delimiter)) != std::string::npos) {
		token = json_string.substr(0, pos);
		json_string_splits.emplace_back(token);
		json_string.erase(0, pos + delimiter.length());
	}
	// end of code from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139
	std::cout << std::endl << json_string << std::endl;
	for (int i = 1; i <= std::stoi((std::string)json[next_text]["choices"][0], &sz); i++) {
		json_choices.emplace_back((std::string)json[next_text]["choices"][i]);
		std::cout << i % 10 << ": " << json_choices[i - 1] << std::endl;
		json_results.emplace_back((std::string)json[next_text]["results"][i]);
	}
}

TextMode::~TextMode() {
	hb_buffer_destroy(buf);
	hb_font_destroy(font);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

bool TextMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {

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
				json_string_splits.clear();
				// code for splitting a string based on a delimiter came from here: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139
				std::string delimiter = "\n";
				size_t pos = 0;
				std::string token;
				while ((pos = json_string.find(delimiter)) != std::string::npos) {
					token = json_string.substr(0, pos);
					json_string_splits.emplace_back(token);
					json_string.erase(0, pos + delimiter.length());
				}
				// end of code from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139
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

		// updating json_string_splits for the in-game draw_text function
		json_string_splits.clear();
		// code for splitting a string based on a delimiter came from here: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139
		std::string delimiter = "\n";
		size_t pos = 0;
		std::string token;
		while ((pos = json_string.find(delimiter)) != std::string::npos) {
			token = json_string.substr(0, pos);
			json_string_splits.emplace_back(token);
			json_string.erase(0, pos + delimiter.length());
		}
		// end of code from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/14266139#14266139

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

void TextMode::update(float elapsed) { }

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
	if (json_string_splits.size() == 0)
		draw_text(json_string, 25.0f, drawable_size.y * 0.9f, 1.0f, glm::vec3(1.0, 1.0f, 1.0f));
	else {
		for (int i = 0; i < json_string_splits.size(); i++)
			draw_text(json_string_splits[i], 25.0f, drawable_size.y * 0.9f - (32.0f * i), 1.0f, glm::vec3(1.0, 1.0f, 1.0f));
		if (json_string != "")
			draw_text(json_string, 25.0f, drawable_size.y * 0.9f - (32.0f * json_string_splits.size()), 1.0f, glm::vec3(1.0, 1.0f, 1.0f));
	}

	for (int i = 0; i < json_choices.size(); i++) {
		int choice_number = i % 10 + 1;
		std::string choice = std::to_string(choice_number) + ": " + json_choices[i];
		draw_text(choice, 25.0f, drawable_size.y * 0.9f - (32.0f * (i + json_string_splits.size() + 2)), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	
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

		// std::cout << text[i] << " | " << x_offset << " " << x_advance << " " << y_offset << " " << slot->bitmap_top << std::endl;

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
		float ypos = y - (slot->bitmap.rows - slot->bitmap_top) * scale; // Assuming horizontal layout only :(

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