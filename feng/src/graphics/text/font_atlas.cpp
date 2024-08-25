#include "font_atlas.h"

#include <glad/glad.h>

#include "../../logging/logging.h"

namespace feng {

	font_atlas::font_atlas(const char* font_path, FT_Library& lib, uint32_t size) {
		FT_Face face;
		if (FT_New_Face(lib, font_path, 0, &face)) {
			LOG_ERROR("Error to load font at: '" + std::string{ font_path } + "'.");
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, size);

		FT_GlyphSlot g = face->glyph;
		uint32_t w = 0, h = 0;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				LOG_ERROR("Error to load Glyph");
				continue;
			}
			w += g->bitmap.width;
			h = std::max(h, g->bitmap.rows);
		}

		atlas_width = w;
		atlas_height = h;

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int x = 0;
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				LOG_ERROR("Error to load Glyph");
				continue;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			characters[c].ax = g->advance.x >> 6;
			characters[c].ay = g->advance.y >> 6;
			characters[c].bw = g->bitmap.width;
			characters[c].bh = g->bitmap.rows;
			characters[c].bl = g->bitmap_left;
			characters[c].bt = g->bitmap_top;
			characters[c].tx = (float)x / w;

			x += g->bitmap.width;
		}

		FT_Done_Face(face);

		LOG_ACTION("Font atlas texture created for: '" + std::string{ font_path } + "'.");
	}

}