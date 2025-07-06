#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "shader.h"

#include "gl_buffers/elementarraybuffer.hpp"
#include "gl_buffers/arraybuffer.hpp"
#include "vertexarray.h"

namespace feng {

	struct vertex2d{
		glm::vec3 position;
		glm::vec2 tex_coords;
	};

	class mesh2d {
	public:
		mesh2d(std::vector<vertex2d> vertices, std::vector<uint32_t> indices, texture texture);
		mesh2d(std::vector<vertex2d> vertices, std::vector<uint32_t> indices, glm::vec3 color);

		vertexarray vertex_array;

		void render(shader& shader_2d, uint32_t no_instances);

		std::vector<vertex2d> get_vertices();

	private:
		std::vector<vertex2d> _vertices;
		std::vector<uint32_t> _indices;

		texture _texture;
		glm::vec3 _color;

		bool _has_texture;

		arraybuffer _arraybuffer;
		elementarraybuffer _elementbuffer;

		void setup();
	};

}