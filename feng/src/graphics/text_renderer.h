#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/glm.hpp>
#include <map>
#include <string>

#include "vertexarray.h"
#include "gl_buffers/arraybuffer.hpp"
#include "shader.h"

namespace feng {

	struct character{
		uint32_t texture_id;
		glm::ivec2 size;
		glm::ivec2 bearing;
		uint32_t advance;
	};

	class text_renderer {
	public:
		void load_font(const char* font_path, FT_Library& lib, uint32_t size);
		void render(shader& shader, const std::string& text, float x, float y, float scale, glm::vec3 color);

	private:
		std::map<char, character> _characters;

		vertexarray _vertex_array;
		arraybuffer _vertex_buffer;

	};

}