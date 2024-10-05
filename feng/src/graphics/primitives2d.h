#pragma once

#include "model2d.h"
#include "mesh2d.h"

namespace feng {

	const std::vector<vertex2d> square_vertices {
		vertex2d { glm::vec3(-0.5,  0.5, 0), glm::vec2(0, 1) },
		vertex2d { glm::vec3( 0.5,  0.5, 0), glm::vec2(1, 1) },
		vertex2d { glm::vec3( 0.5, -0.5, 0), glm::vec2(1, 0) },
		vertex2d { glm::vec3(-0.5, -0.5, 0), glm::vec2(0, 0) }
	};

	const std::vector<uint32_t> square_indices{
		2, 1, 0,
		0, 3, 2
	};

	struct primitives2d {
		static std::vector<mesh2d> generate_square_mesh(texture& tex);
		static std::vector<mesh2d> generate_square_mesh(glm::vec3 color);

	};
}