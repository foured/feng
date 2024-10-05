#include "primitives.h"

namespace feng {

	void primitives::print_vertex_data(const model& model) {
		for (const mesh& m : model._meshes) {
			for (const vertex& v : m._vertices) {
				std::cout << "pos: " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
				std::cout << "norm: " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
				std::cout << "tc: " << v.tex_coords.x << " " << v.tex_coords.y << "\n";
				std::cout << "tng: " << v.tangent.x << " " << v.tangent.y << " " << v.tangent.z << "\n\n";
			}
			for (const auto& i : m._indices) {
				std::cout << i << ", ";
			}
			std::cout << '\n';
		}
	}

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

}