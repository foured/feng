#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

#include "../vertexarray.h"
#include "../gl_buffers/arraybuffer.hpp"
#include "../shader.h"
#include "font_atlas.h"

namespace feng {

	class text_batcher;

	struct text_vertex{
		float x;
		float y;
		float z;
		float tcx;
		float tcy;
		float texture_index;
		glm::vec3 color;
	};

	class text_renderer {
	public:
		text_renderer(font_atlas& atlas);

		void render(text_batcher* batcher, const std::string& text, glm::vec3 pos, glm::vec3 color);

	private:
		vertexarray _vertex_array;
		arraybuffer _vertex_buffer;
		std::vector<text_vertex> _vertices;
		font_atlas& _atlas;

	};

}