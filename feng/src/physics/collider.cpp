#include "collider.h"

#include "../logging/logging.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <numeric>

namespace feng {

	// TRIANGLE---------------------------------------------------------------------------------------------------------

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3) 
		: p1(_p1), p2(_p2), p3(_p3) {
		calculate_normal();
	}

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n) 
		: p1(_p1), p2(_p2), p3(_p3), normal(_n) {	}

	bool triangle::is_coplanar(const triangle* other) const {
		glm::vec3 n1 = normal;
		glm::vec3 n2 = other->normal;

		float dot = glm::dot(n1, n2);
		if (std::abs(std::abs(dot) - 1.0f) > 1e-6f)
			return false;

		float d = -glm::dot(n1, this->p1);
		float dist = glm::dot(n1, other->p1) + d;

		return std::abs(dist) < 1e-6f;
	}

	void triangle::calculate_normal() {
		normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
	}

	// POLYGON----------------------------------------------------------------------------------------------------------

	bool polygon::is_coplanar(const polygon* other) const {
		FENG_ASSERT(!points.empty() || !other->points.empty(), "Polygon is empty, can check if is coplanar");

		glm::vec3 n1 = normal;
		glm::vec3 n2 = other->normal;

		if (std::abs(std::abs(glm::dot(n1, n2)) - 1.0f) > 1e-6f) {
			return false;
		}

		float d = -glm::dot(n1, points[0]);

		for (const auto& p : other->points) {
			if (std::abs(glm::dot(n1, p) + d) > 1e-6f) {
				return false;
			}
		}

		return true;
	}

	bool polygon::is_coplanar(const glm::vec3& point) const {
		FENG_ASSERT(!points.empty(), "Polygon is empty, can check if is coplanar");

		float d = glm::dot(normal, points[0]);
		return glm::abs(glm::dot(normal, point) - d) < 1e-6f;
	}

	void polygon::calculate_normal() {
		FENG_ASSERT(points.size() >= 3, "Polygon doesn't have enough points.");

		glm::vec3 edge1 = points[1] - points[0];
		glm::vec3 edge2 = points[2] - points[0];

		normal = glm::normalize(glm::cross(edge1, edge2));
	}

	// COLLISION DATA---------------------------------------------------------------------------------------------------

	void collision_data::invert() {
		axis *= -1;
	}

	// COLLIDER_BASE--------------------------------------------------------------------------------------------------------

	bool collider_base::was_changed_after_update() {
		return _was_changed_after_update;
	}

	// SAT_COLLIDER-----------------------------------------------------------------------------------------------------

	sat_collider_base::sat_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals)
		: _points(points), _normals(normals) {
		calculate_center();
	}

	bool sat_collider_base::collides(collider_base* other, collision_data* out) {
		bool res = other->collides_shape(this, out);
		out->invert();
		return res;
	}

	bool sat_collider_base::collides_shape(sat_collider_base* other, collision_data* out) {
		for (const glm::vec3& axis : _normals) {
			if (!check_axis_sat(other, axis, out)) {
				return false;
			}
		}
		for (const glm::vec3& axis : other->_normals) {
			if (!check_axis_sat(other, axis, out)) {
				return false;
			}
		}

		// Edge case
		//size_t s1 = _points.size() - 1;
		//size_t s2 = other->_points.size() - 1;
		//glm::vec3 edge1, edge2, edge_axis;
		//for (size_t i = 0; i < s1; i++) {
		//	edge1 = _points[i + 1] - _points[i];
		//	for (size_t j = 0; j < s2; j++) {
		//		edge2 = other->_points[j + 1] - other->_points[j];
		//		edge_axis = glm::cross(edge1, edge2);
		//		if (glm::length2(edge_axis) > 1e-6f) {
		//			if (!check_axis(other, glm::normalize(edge_axis), out)) {
		//				return false;
		//			}
		//		}
		//	}
		//	edge2 = other->_points[s2] - other->_points[0];
		//	edge_axis = glm::cross(edge1, edge2);
		//	if (glm::length2(edge_axis) > 1e-6f) {
		//		if (!check_axis(other, glm::normalize(edge_axis), out)) {
		//			return false;
		//		}
		//	}
		//}

		glm::vec3 dir = other->_center - _center;
		float d = glm::dot(dir, out->axis);
		if (d > 0) {
			out->invert();
		}

		return true;
	}

	bool sat_collider_base::collides_shape(sphere_collider_base* other, collision_data* out) {
		glm::vec3 sphere_axis;
		glm::vec3 sphere_center = other->get_center();
		float min_dist_sqrt = FLT_MAX;
		for (const glm::vec3& point : _points) {
			float d_sqrt = glm::length2(sphere_center - point);
			if (d_sqrt < min_dist_sqrt) {
				min_dist_sqrt = d_sqrt;
				sphere_axis = point - sphere_center;
			}
		}

		if (!check_axis_sphere(other, glm::normalize(sphere_axis), out)) {
			return false;
		}
		for (const glm::vec3& n : _normals) {
			if (!check_axis_sphere(other, n, out)) {
				return false;
			}
		}

		glm::vec3 dir = sphere_center - _center;
		float d = glm::dot(dir, out->axis);
		if (d < 0) {
			out->invert();
		}

		return true;
	}

	// PRIVATE----------------------------------------------------------------------------------------------------------

	bool sat_collider_base::check_axis_sat(sat_collider_base* other, const glm::vec3& axis, collision_data* out) {
		glm::vec2 proj1 = project_onto(axis);
		glm::vec2 proj2 = other->project_onto(axis);

		return check_overlap(proj1, proj2, axis, out);
	}

	bool sat_collider_base::check_axis_sphere(sphere_collider_base* other, const glm::vec3& axis, collision_data* out) {
		float r = other->get_radius();
		glm::vec3 c = other->get_center();
		glm::vec3 dir = axis * r;
		glm::vec3 p1 = c + dir;
		glm::vec3 p2 = c - dir;

		float min = glm::dot(p1, axis);
		float max = glm::dot(p2, axis);

		if (min > max) {
			std::swap(min, max);
		}

		glm::vec2 proj1 { min, max };
		glm::vec2 proj2 = project_onto(axis);

		return check_overlap(proj1, proj2, axis, out);
	}

	bool sat_collider_base::check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis, 
		collision_data* out) {
		float penetration = calculate_penetration(proj1, proj2);
		if (penetration <= 0.0f) {
			return false;
		}
		else if (penetration < out->penetration) {
			out->penetration = penetration;
			out->axis = axis;
		}
		return true;
	}

	glm::vec2 sat_collider_base::project_onto(const glm::vec3& axis) const {
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (d > max) max = d;
			if (d < min) min = d;
		}

		return { min, max };
	}

	float sat_collider_base::calculate_penetration(const glm::vec2& v1, const glm::vec2& v2) const {
		float penetration = std::min(v1.y, v2.y) - std::max(v1.x, v2.x);
		return penetration;
	}

	void sat_collider_base::calculate_center() {
		_center = glm::vec3(0.0f);
		FENG_ASSERT(!_points.empty(), "Collider points are empty.");

		_center = std::reduce(_points.begin(), _points.end(), glm::vec3(0.0f));
		_center /= static_cast<float>(_points.size());
	}

	// SPHERE_COLLIDER--------------------------------------------------------------------------------------------------

	// PUBLIC-----------------------------------------------------------------------------------------------------------

	sphere_collider_base::sphere_collider_base(const glm::vec3& center, float radius) 
		: _center(center), _radius(radius) {
		FENG_ASSERT(radius >= 0, "sphere_collider doesn`t support negative radius");
	}

	bool sphere_collider_base::collides(collider_base* other, collision_data* out) {
		bool res = other->collides_shape(this, out);
		out->invert();
		return res;
	}

	bool sphere_collider_base::collides_shape(sphere_collider_base* other, collision_data* out) {
		float fact_dist = glm::length2(_center - other->_center);
		float r_sum = _radius + other->_radius;
		if (fact_dist > r_sum * r_sum) {
			return false;
		}

		float dist = glm::sqrt(fact_dist);
		out->penetration = r_sum - dist;
		glm::vec3 dir = other->_center - _center;
		out->axis = dist > 0.0f ? dir / dist : dir / glm::vec3(0, 1, 0);
		return true;
	}

	bool sphere_collider_base::collides_shape(sat_collider_base* other, collision_data* out) {
		bool res = other->collides_shape(this, out);
		out->invert();
		return res;
	}

	float sphere_collider_base::get_radius() const {
		return _radius;
	}

	glm::vec3 sphere_collider_base::get_center() const {
		return _center;
	}

}