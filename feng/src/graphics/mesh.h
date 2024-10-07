#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "shader.h"

#include "gl_buffers/elementarraybuffer.hpp"
#include "gl_buffers/arraybuffer.hpp"
#include "vertexarray.h"

namespace feng {

	struct vertex{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
		glm::vec3 tangent;
	};

	class mesh {
	public:
		mesh(std::vector<vertex> vertices, std::vector<uint32_t> indices, std::vector<texture> textures);
		mesh(std::vector<vertex> vertices, std::vector<uint32_t> indices, aiColor4D diffuse, aiColor4D specular);

		void render(shader& shader, uint32_t no_instances);
		void setup();

		vertexarray vertex_array;

	private:
		friend class model;
		friend struct mesh_batch;

		std::vector<texture> _textures;
		std::vector<uint32_t> _indices;
		std::vector<vertex> _vertices;

		aiColor4D _diffuse, _specular;

		arraybuffer _arraybuffer;
		elementarraybuffer _elementbuffer;
		bool _has_textures;

	};

}