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

	//----- Font -----
	// font stuff - based on harfbuzz tutorial & https://learnopengl.com/In-Practice/Text-Rendering

	hb_buffer_t *buf;
	hb_font_t *font;

	FT_Library  library;
	FT_Face face;
	unsigned int glyph_count;

	const std::string font_name = "SansitaSwashed.ttf";

	void draw_text(std::string text, float x, float y, float scale, glm::vec3 color);
};