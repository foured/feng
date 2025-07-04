#include "epa.h"

#include "../logging/logging.h"

#define EPA_MAX_SEARCH_DEPTH 100

namespace feng::phys::epa {

	void epa(const gjk::simplex* simplex, const gjk::collider* col_1, const gjk::collider* col_2, collision_data* out) {
		std::vector<glm::vec3> polytope(simplex->begin(), simplex->end());
		std::vector<size_t> faces = {
			0, 1, 2,
			0, 3, 1,
			0, 2, 3,
			1, 3, 2
		};

		auto [normals, min_face] = impl::get_face_normals(polytope, faces);

		glm::vec3 min_normal;
		float min_dist = FLT_MAX;
		bool keep_expanding = true;

		glm::vec3 c1_point(0);
		glm::vec3 c2_point(0);

		size_t search_depth = 0;
		while (search_depth < EPA_MAX_SEARCH_DEPTH && keep_expanding) {
			search_depth++;
			min_normal = normals[min_face].first;
			min_dist = normals[min_face].second;

			//glm::vec3 support = impl::support(col_1, col_2, min_normal, c1_point, c2_point);
			glm::vec3 support = gjk::impl::support(col_1, col_2, min_normal);
			float d = glm::dot(min_normal, support);

			if (std::abs(d - min_dist) > 0.001f) {
				//min_dist = FLT_MAX;

				std::vector<std::pair<size_t, size_t>> unique_edges;

				for (size_t i = 0; i < normals.size(); i++) {
					if (glm::dot(normals[i].first, support) > 0) {
						size_t f = i * 3;

						impl::add_if_unique(unique_edges, faces, f,     f + 1);
						impl::add_if_unique(unique_edges, faces, f + 1, f + 2);
						impl::add_if_unique(unique_edges, faces, f + 2, f);

						faces[f + 2] = faces.back();
						faces.pop_back();
						faces[f + 1] = faces.back();
						faces.pop_back();
						faces[f    ] = faces.back();
						faces.pop_back();

						normals[i] = normals.back();
						normals.pop_back();
						i--;
					}
				}

				std::vector<size_t> new_faces;
				for (auto [ei1, ei2] : unique_edges) {
					new_faces.push_back(ei1);
					new_faces.push_back(ei2);
					new_faces.push_back(polytope.size());
				}

				polytope.push_back(support);

				auto [new_normals, new_min_face] = impl::get_face_normals(polytope, new_faces);

				float old_min_dist = FLT_MAX;
				for (size_t i = 0; i < normals.size(); i++) {
					if (normals[i].second < old_min_dist) {
						old_min_dist = normals[i].second;
						min_face = i;
					}
				}

				if (new_normals[new_min_face].second < old_min_dist) {
					min_face = new_min_face + normals.size();
				}

				faces.insert(faces.end(), new_faces.begin(), new_faces.end());
				normals.insert(normals.end(), new_normals.begin(), new_normals.end());
			}
			else {
				keep_expanding = false;
			}
		}

		FENG_ASSERT(search_depth < EPA_MAX_SEARCH_DEPTH, "EPA_MAX_SEARCH_DEPTH was reached.");

		out->axis = min_normal;
		// without + 0.0001f collision detection passes every time
		out->penetration = min_dist + 0.0001f;
	}

	namespace impl {
		std::pair<std::vector<std::pair<glm::vec3, float>>, size_t> get_face_normals(
			const std::vector<glm::vec3>& polytope,
			const std::vector<size_t>& faces) {
			std::vector<std::pair<glm::vec3, float>> normals;
			size_t minTriangle = 0;
			float  minDistance = FLT_MAX;

			for (size_t i = 0; i < faces.size(); i += 3) {
				glm::vec3 a = polytope[faces[i]];
				glm::vec3 b = polytope[faces[i + 1]];
				glm::vec3 c = polytope[faces[i + 2]];

				glm::vec3 normal = glm::normalize(cross(b - a, c - a));
				float distance = glm::dot(normal, a);

				if (distance < 0) {
					normal *= -1;
					distance *= -1;
				}

				normals.emplace_back(normal, distance);

				if (distance < minDistance) {
					minTriangle = i / 3;
					minDistance = distance;
				}
			}
			return { normals, minTriangle };
		}

		void add_if_unique(
			std::vector<std::pair<size_t, size_t>>& edges,
			const std::vector<size_t>& faces,
			size_t a,
			size_t b) {
			auto reverse = std::find(                       
				edges.begin(),                              
				edges.end(),                                
				std::make_pair(faces[b], faces[a]) 
			);

			if (reverse != edges.end()) {
				edges.erase(reverse);
			}

			else {
				edges.emplace_back(faces[a], faces[b]);
			}
		}

		glm::vec3 support(const gjk::collider* col1, const gjk::collider* col2, const glm::vec3& axis,
			glm::vec3& col1_point, glm::vec3& col2_point) {
			col1_point = col1->find_furthest_point(axis);
			col2_point = col1->find_furthest_point(-axis);
			return col1_point - col2_point;
		}
	}

}