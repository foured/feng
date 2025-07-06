#include "collider.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <numeric>

#include "../../logging/logging.h"
#include "../../utilities/utilities.h"

namespace feng::phys {

	// COLLIDER_BASE--------------------------------------------------------------------------------------------------------

	bool collider_base::was_changed_after_update() {
		return _was_changed_after_update;
	}

	// SAT_COLLIDER-----------------------------------------------------------------------------------------------------

	mesh_collider_base::mesh_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals) {
		_points = points;
		_normals = normals;
		calculate_center();
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

	void mesh_collider_base::add_data_offset(const glm::vec3& offset) {
		for (glm::vec3& p : _points) {
			p += offset;
		}
	}

	// SPHERE_COLLIDER--------------------------------------------------------------------------------------------------

	sphere_collider_base::sphere_collider_base(const glm::vec3& center, float radius)  {
		_center = center;
		_radius = radius;
		FENG_ASSERT(radius >= 0, "sphere_collider doesn`t support negative radius");
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

	void sphere_collider_base::add_data_offset(const glm::vec3& offset) {
		_center += offset;
	}

}