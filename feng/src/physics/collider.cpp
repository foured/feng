#include "collider.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <numeric>

#include "../logging/logging.h"
#include "../utilities/utilities.h"

namespace feng::phys {

	// COLLIDER_BASE--------------------------------------------------------------------------------------------------------

	bool collider_base::was_changed_after_update() {
		return _was_changed_after_update;
	}

	float collider_base::calculate_penetration(const glm::vec2& v1, const glm::vec2& v2) {
		float penetration = std::min(v1.y, v2.y) - std::max(v1.x, v2.x);
		return penetration;
	}

	bool collider_base::check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis, 
		collision_data* data) {
		float penetration = calculate_penetration(proj1, proj2);
		if (penetration <= 0.0f) {
			return false;
		}
		else if (penetration < data->penetration) {
			data->penetration = penetration;
			data->axis = axis;
		}
		return true;
	}

	bool collider_base::check_axis(collider_base* other, const glm::vec3& axis, collision_data* data) {
		glm::vec2 proj1 = project_onto(axis);
		glm::vec2 proj2 = other->project_onto(axis);

		return check_overlap(proj1, proj2, axis, data);
	}

	// SAT_COLLIDER-----------------------------------------------------------------------------------------------------

	mesh_collider_base::mesh_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals)
		: _points(points), _normals(normals) {
		calculate_center();
	}

	bool mesh_collider_base::collides(collider_base* other, collision_data* data) {
		bool res = other->collides_shape(this, data);
		//out->invert();
		return res;
	}

	bool mesh_collider_base::collides_shape(mesh_collider_base* other, collision_data* data) {
		for (const glm::vec3& axis : _normals) {
			if (!check_axis(other, axis, data)) {
				return false;
			}
		}
		for (const glm::vec3& axis : other->_normals) {
			if (!check_axis(other, axis, data)) {
				return false;
			}
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

		//glm::vec3 dir = other->_center - _center;
		//float d = glm::dot(dir, lcd.axis);
		//if (d > 0) {
		//	lcd.invert();
		//}
		//
		//other->lcd.penetration = lcd.penetration;
		//other->lcd.axis = -1.0f * lcd.axis;

		return true;
	}

	bool mesh_collider_base::collides_shape(sphere_collider_base* other, collision_data* data) {
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

		if (!check_axis(other, glm::normalize(sphere_axis), data)) {
			return false;
		}
		for (const glm::vec3& n : _normals) {
			if (!check_axis(other, n, data)) {
				return false;
			}
		}

		//glm::vec3 dir = sphere_center - _center;
		//float d = glm::dot(dir, lcd.axis);
		//if (d > 0) {
		//	lcd.invert();
		//}
		//
		//other->lcd.penetration = lcd.penetration;
		//other->lcd.axis = -1.0f * lcd.axis;

		return true;
	}

	collision_contact mesh_collider_base::calculate_collision_contact(const glm::vec3& axis) const {
		float min = FLT_MAX;

		float e = 0.00001;
		std::vector<glm::vec3> closest;

		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (utilities::compare_floats(d, min, e)) {
				closest.push_back(p);
			}
			else if (d < min) { 
				min = d; 
				closest.clear();
				closest.push_back(p);
			}
		}
		size_t csize = closest.size();
		FENG_ASSERT(csize > 0, "Number of contact points is 0.");
		collision_contact cc;
		if (csize == 1) {
			cc.data = closest[0];
			cc.type = collision_contact_type::point;
		}
		else if (csize == 2) {
			cc.data = edge{closest[0], closest[1]};
			cc.type = collision_contact_type::edge;
		}
		else {
			polygon p = polygon::make_convex_polygon(closest);
			//p.points = std::move(closest);
			//p.calculate_normal();
#ifdef FENG_DEBUG
			FENG_ASSERT(utilities::compare_normals_nd(axis, p.normal, e), "Noraml and axis are not the same");
#endif
			cc.data = p;
			cc.type = collision_contact_type::polygon;
		}
		return cc;
	}

	glm::vec3 mesh_collider_base::find_furthest_point(const glm::vec3& axis) const {
		float max = -FLT_MAX;
		glm::vec3 ret(0);
		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (d > max) {
				max = d;
				ret = p;
			}
		}
		return ret;
	}

	void mesh_collider_base::calculate_center() {
		_center = glm::vec3(0.0f);
		FENG_ASSERT(!_points.empty(), "Collider points are empty.");

		_center = std::reduce(_points.begin(), _points.end(), glm::vec3(0.0f));
		_center /= static_cast<float>(_points.size());
	}

	glm::vec2 mesh_collider_base::project_onto(const glm::vec3& axis) const {
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (d > max) max = d;
			if (d < min) min = d;
		}

		return { min, max };
	}

	void mesh_collider_base::add_data_offset(const glm::vec3& offset) {
		for (glm::vec3& p : _points) {
			p += offset;
		}
	}

	// SPHERE_COLLIDER--------------------------------------------------------------------------------------------------

	sphere_collider_base::sphere_collider_base(const glm::vec3& center, float radius) 
		: _radius(radius) {
		_center = center;
		FENG_ASSERT(radius >= 0, "sphere_collider doesn`t support negative radius");
	}

	bool sphere_collider_base::collides(collider_base* other, collision_data* data) {
		bool res = other->collides_shape(this, data);
		return res;
	}

	bool sphere_collider_base::collides_shape(sphere_collider_base* other, collision_data* data) {
		float fact_dist = glm::length2(_center - other->_center);
		float r_sum = _radius + other->_radius;
		if (fact_dist > r_sum * r_sum) {
			return false;
		}

		float dist = glm::sqrt(fact_dist);
		data->penetration = r_sum - dist;
		glm::vec3 dir = other->_center - _center;
		data->axis = dist > 0.0f ? dir / dist : dir / glm::vec3(0, 1, 0);
		return true;
	}

	bool sphere_collider_base::collides_shape(mesh_collider_base* other, collision_data* data) {
		bool res = other->collides_shape(this, data);
		return res;
	}

	float sphere_collider_base::get_radius() const {
		return _radius;
	}

	glm::vec3 sphere_collider_base::get_center() const {
		return _center;
	}

	collision_contact sphere_collider_base::calculate_collision_contact(const glm::vec3& axis) const {
		collision_contact cc;
		cc.data = _center + axis * _radius;
		cc.type = collision_contact_type::point;
		return cc;
	}

	glm::vec3 sphere_collider_base::find_furthest_point(const glm::vec3& axis) const {
		return _center + (axis * _radius);
	}

	glm::vec2 sphere_collider_base::project_onto(const glm::vec3& axis) const {
		glm::vec3 dir = axis * _radius;
		glm::vec3 p1 = _center + dir;
		glm::vec3 p2 = _center - dir;

		float min = glm::dot(p1, axis);
		float max = glm::dot(p2, axis);

		if (min > max) {
			std::swap(min, max);
		}

		return { min, max };
	}

	void sphere_collider_base::add_data_offset(const glm::vec3& offset) {
		_center += offset;
	}

}