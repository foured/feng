#pragma once

#include "gjk.h"
#include "collision_data.h"

namespace feng::phys::epa {

	// thx to: https://winter.dev/articles/epa-algorithm

	void epa(const gjk::simplex* simplex, const gjk::collider* col_1, const gjk::collider* col_2, collision_data* out);

	namespace impl {
		std::pair<std::vector<std::pair<glm::vec3, float>>, size_t> get_face_normals(
			const std::vector<glm::vec3>& polytope,
			const std::vector<size_t>& faces);

		void add_if_unique(
			std::vector<std::pair<size_t, size_t>>& edges,
			const std::vector<size_t>& faces,
			size_t a,
			size_t b);

		glm::vec3 support(const gjk::collider* col1, const gjk::collider* col2, const glm::vec3& axis,
			glm::vec3& col1_point, glm::vec3& col2_point);

	}

}