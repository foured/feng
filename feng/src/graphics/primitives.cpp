#include "primitives.h"

namespace feng {

	std::vector<mesh> primitives::generate_cube_mesh(std::vector<texture> textures) {
		return std::vector<mesh> { mesh(
			cube_vertices,
			cube_indices,
			textures) };
	}

	std::vector<mesh> primitives::generate_cube_mesh(glm::vec3 diffuse, glm::vec3 specular) {
		return std::vector<mesh> { mesh(
			cube_vertices, 
			cube_indices, 
			aiColor4D(diffuse.r, diffuse.g, diffuse.b, 1.0), 
			aiColor4D(specular.r, specular.g, specular.b, 1.0)) };
	}

	std::vector<mesh> primitives::generate_plane_mesh(std::vector<texture> textures) {
		return std::vector<mesh> { mesh(
			plane_vertices,
			plane_indices,
			textures) };
	}

	std::vector<mesh> primitives::generate_plane_mesh(glm::vec3 diffuse, glm::vec3 specular) {
		return std::vector<mesh>{ mesh(
			plane_vertices,
			plane_indices,
			aiColor4D(diffuse.r, diffuse.g, diffuse.b, 1.0),
			aiColor4D(specular.r, specular.g, specular.b, 1.0)) };
	}

}