#include "text_renderer.h"

#include <glad/glad.h>
#include <string>

#include "../logging/logging.h"

namespace feng {

	void text_renderer::load_font(const char* font_path, FT_Library& lib, uint32_t size) {
		FT_Face face;
		if (FT_New_Face(lib, font_path, 0, &face)) {
			LOG_ERROR("Error to load font at: '" + std::string{ font_path } + "'.");
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, size);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				LOG_ERROR("Error to load Glyph");
			}
		
			uint32_t texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0, 
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
			character c_char = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			_characters.insert({c, c_char});
		}
		FT_Done_Face(face);

		_vertex_array.generate();
		_vertex_array.bind();
		_vertex_buffer.generate();
		_vertex_buffer.bind();

		_vertex_buffer.buffer_data<float>(sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		_vertex_array.set_attrib_pointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		_vertex_buffer.unbind();
		vertexarray::unbind();

		LOG_ACTION("Loaded font at: '" + std::string{font_path} + "'.");
	}

	void text_renderer::render(shader& shader, const std::string& text, float x, float y, float scale, glm::vec3 color) {
		shader.activate();
		shader.set_3float("text_color", color);
		glActiveTexture(GL_TEXTURE0);
		_vertex_array.bind();

		for (size_t i = 0, len = text.size(); i < len; i++) {
			character c = _characters[text[i]];

			float xpos = x + c.bearing.x * scale;
			float ypos = y - (c.size.y - c.bearing.y) * scale;

			float w = c.size.x * scale;
			float h = c.size.y * scale;

			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			glBindTexture(GL_TEXTURE_2D, c.texture_id);
			_vertex_buffer.bind();
			_vertex_buffer.buffer_sub_data(0, sizeof(vertices), vertices);
			_vertex_buffer.unbind();

			_vertex_array.draw_arrays(GL_TRIANGLES, 6);

			x += (c.advance >> 6) * scale;
		}
		vertexarray::unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}