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

	struct character_info {
		float ax;
		float ay;

		float bw;
		float bh;

		float bl;
		float bt;

		float tx;
	};

	struct text_vertex{
		float x;
		float y;
		float s;
		float t;
	};

	class text_renderer {
	public:
		void generate_atlas(const char* font_path, FT_Library& lib, uint32_t size);
		void render(shader& shader, const std::string& text, float x, float y, glm::vec3 color, float sx = 1, float sy = 1);

	private:
		character_info _characters[128];
		uint32_t _atlas_textur_id;
		vertexarray _vertex_array;
		arraybuffer _vertex_buffer;
		uint32_t _atlas_width, _atlas_height;

	};

}