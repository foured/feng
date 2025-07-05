#include "sat.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>

namespace feng::phys::sat {

	// MESH_COLLIDER ----

	bool mesh_collider::collides(const collider* other, collision_data* data) const {
		return other->collides_shape(this, data);
	}

	bool mesh_collider::collides_shape(const mesh_collider* other, collision_data* data) const {
		return sat::solve(this, other, data);
	}

	bool mesh_collider::collides_shape(const sphere_collider* other, collision_data* data) const {
		return sat::solve(this, other, data);
	}

	glm::vec2 mesh_collider::project_onto(const glm::vec3& axis) const {
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (d > max) max = d;
			if (d < min) min = d;
		}

		return { min, max };
	}

	bool mesh_collider::chech_axes(const collider* other, collision_data* data) const {
		for (const glm::vec3& n : _normals) {
			if (!sat::check_axis(this, other, n, data)) {
				return false;
			}
		}
		return true;
	}

	// SPHERE_COLLIDER ----

	bool sphere_collider::collides(const collider* other, collision_data* data) const {
		return other->collides(this, data);
	}

	bool sphere_collider::collides_shape(const mesh_collider* other, collision_data* data) const {
		return sat::solve(other, this, data);
	}

	bool sphere_collider::collides_shape(const sphere_collider* other, collision_data* data) const {
		return sat::solve(this, other, data);
	}

	glm::vec2 sphere_collider::project_onto(const glm::vec3& axis) const {
		glm::vec3 p1 = _center - (axis * _radius);
		glm::vec3 p2 = _center +(axis * _radius);
		
		float d1 = glm::dot(p1, axis);
		float d2 = glm::dot(p2, axis);

		if (d1 > d2) {
			std::swap(d1, d2);
		}

		return { d1, d2 };
	}

	// SAT ----

	bool sat::check_axis(const collider* col_1, const collider* col_2, const glm::vec3& axis, collision_data* data) {
		glm::vec2 p1 = col_1->project_onto(axis);
		glm::vec2 p2 = col_2->project_onto(axis);

		float penetration = std::min(p1.y, p2.y) - std::max(p1.x, p2.x);
		// can add skin width
		if(penetration <= 0.0f){
			return false;
		}
		else if (penetration < data->penetration) {
			data->penetration = penetration;
			data->axis = axis;
		}
		return true;
	}

	bool sat::solve(collider* col_1, collider* col_2, collision_data* data) {
		return col_1->collides(col_2, data);
	}

	bool sat::solve(const mesh_collider* col_1, const mesh_collider* col_2, collision_data* data) {
		if (!col_1->chech_axes(col_2, data)) {
			return false;
		}

		if (!col_2->chech_axes(col_1, data)) {
			return false;
		}

		// Edge case
		//size_t s1 = _points.size();
		//size_t s2 = other->_points.size();
		//glm::vec3 edge1, edge2, edge_axis;
		//for (size_t i = 0; i < s1; i++) {
		//	edge1 = _points[(i + 1) % s1] - _points[i];
		//	for (size_t j = 0; j < s2; j++) {
		//		edge2 = other->_points[(j + 1) % s2] - other->_points[j];
		//		edge_axis = glm::cross(edge1, edge2);
		//		if (glm::length2(edge_axis) > 1e-6f) {
		//			if (!check_axis(other, glm::normalize(edge_axis))) {
		//				return false;
		//			}
		//		}
		//	}
		//}

		return true;
	}

	bool sat::solve(const mesh_collider* mesh, const sphere_collider* sphere, collision_data* data) {
		glm::vec3 sphere_axis(0.0f);
		glm::vec3 sphere_center = sphere->_center;
		float min_dist_sqrt = FLT_MAX;
		for (const glm::vec3& p : mesh->_points) {
			float d_sqrt = glm::length2(sphere_center - p);
			if (d_sqrt < min_dist_sqrt) {
				min_dist_sqrt = d_sqrt;
				sphere_axis = p - sphere_center;
			}
		}

		if (!check_axis(mesh, sphere, sphere_axis, data)) {
			return false;
		}

		if (!mesh->chech_axes(sphere, data)) {
			return false;
		}

		return true;
	}

	bool sat::solve(const sphere_collider* col_1, const sphere_collider* col_2, collision_data* data) {
		glm::vec3 dir = col_2->_center - col_1->_center;
		float r1 = col_1->_radius;
		float r2 = col_2->_radius;

		float fact_dist_sqrt = glm::length2(dir);
		float r_sum = r1 + r2;
		if (fact_dist_sqrt > r_sum * r_sum) {
			return false;
		}
		float fact_dist = glm::sqrt(fact_dist_sqrt);
		data->penetration = r_sum - fact_dist;
		data->axis = fact_dist > 0.0f ? dir / fact_dist : glm::vec3(0, 1, 0);
		return true;
	}

}