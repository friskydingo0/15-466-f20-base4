#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
#include <hb-ft.h>

struct TextMode : Mode {
	TextMode();
	virtual ~TextMode();

	//functions called by main loop:
	// virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	// virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----
	// font stuff - based on harfbuzz tutorial & https://learnopengl.com/In-Practice/Text-Rendering
	struct Character{
		unsigned int textureID;
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
		// double x_offset, y_offset;
		// double x_advance, y_advance;
	};

	hb_buffer_t *buf;
	hb_font_t *font;
	FT_Face face;
	unsigned int glyph_count;

	// Map of glyph textures for each character
	// std::map<hb_codepoint_t, Character> char_map;
	std::map<GLchar, Character> Characters;

	void draw_text(std::string text, float x, float y, float scale, glm::vec3 color);
};