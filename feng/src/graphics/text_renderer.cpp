#include "text_renderer.h"

#include <glad/glad.h>
#include <string>

#include "../logging/logging.h"

namespace feng {

	void text_renderer::generate_atlas(const char* font_path, FT_Library& lib, uint32_t size) {
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
			}
			w += g->bitmap.width;
			h = std::max(h, g->bitmap.rows);
		}

		_atlas_width = w;
		_atlas_height = h;

		glGenTextures(1, &_atlas_textur_id);
		glBindTexture(GL_TEXTURE_2D, _atlas_textur_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int x = 0;
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				LOG_ERROR("Error to load Glyph");
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			_characters[c].ax = g->advance.x >> 6;
			_characters[c].ay = g->advance.y >> 6;
			_characters[c].bw = g->bitmap.width;
			_characters[c].bh = g->bitmap.rows;
			_characters[c].bl = g->bitmap_left;
			_characters[c].bt = g->bitmap_top;
			_characters[c].tx = (float)x / w;

			x += g->bitmap.width;
		}

		FT_Done_Face(face);

		_vertex_array.generate();
		_vertex_array.bind();
		_vertex_buffer.generate();
		_vertex_buffer.bind();
		
		_vertex_buffer.buffer_data<float>(sizeof(text_vertex) * 6 * 20, NULL, GL_DYNAMIC_DRAW);
		_vertex_array.set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		
		_vertex_buffer.unbind();
		vertexarray::unbind();

		LOG_ACTION("Font atlas texture created for: '" + std::string{ font_path } + "'.");
	}

	void text_renderer::render(shader& shader, const std::string& text, float x, float y, glm::vec3 color, float sx, float sy) {
		shader.set_3float("text_color", color);
		std::vector<text_vertex> vertices(text.size() * 6);

		int n = 0;
		for (const char c : text) {
			character_info cci = _characters[c];

			float x2 = x + cci.bl * sx;
			float y2 = -y - cci.bt * sy;
			float w = cci.bw * sx;
			float h = cci.bh * sy;

			x += cci.ax * sx;
			y += cci.ay * sy;

			//if (!w || !h)
			//	continue;
			
			float tx_left = cci.tx;
			float tx_right = cci.tx + cci.bw / _atlas_width;
			float ty_top = 0.0f;
			float ty_bottom = cci.bh / _atlas_height;

			vertices[n++] = { x2,     -y2,     tx_left,  ty_top };
			vertices[n++] = { x2,     -y2 - h, tx_left,  ty_bottom };
			vertices[n++] = { x2 + w, -y2,     tx_right, ty_top };
			vertices[n++] = { x2 + w, -y2,     tx_right, ty_top };
			vertices[n++] = { x2,     -y2 - h, tx_left,  ty_bottom };
			vertices[n++] = { x2 + w, -y2 - h, tx_right, ty_bottom };
		}

		glBindTexture(GL_TEXTURE_2D, _atlas_textur_id);
		_vertex_buffer.bind();
		_vertex_buffer.buffer_sub_data(0, sizeof(text_vertex) * vertices.size(), vertices.data());
		_vertex_array.bind();
		_vertex_array.draw_arrays(GL_TRIANGLES, n);

		vertexarray::unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}