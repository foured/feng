#include "primitives2d.h"

#include "../logger/logger.h"

namespace feng {


	std::vector<mesh2d> primitives2d::generate_square_mesh(texture& tex) {
		return std::vector<mesh2d> { mesh2d(square_vertices, square_indices, tex) };
	}

	std::vector<mesh2d> primitives2d::generate_square_mesh(glm::vec3 color) {
		return std::vector<mesh2d> { mesh2d(square_vertices, square_indices, color) };
	}
}