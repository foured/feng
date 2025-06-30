#include "collider.h"

#include "../utilities/utilities.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <numeric>

#define COLLISION_EPSILON 0.00001f

namespace feng {

	// TRIANGLE---------------------------------------------------------------------------------------------------------

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3) 
		: p1(_p1), p2(_p2), p3(_p3) {
		calculate_normal();
	}

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n) 
		: p1(_p1), p2(_p2), p3(_p3), normal(_n) {	}

	bool triangle::is_coplanar(const triangle& other) const {
		glm::vec3 n1 = normal;
		glm::vec3 n2 = other.normal;

		float dot = glm::dot(n1, n2);
		if (std::abs(std::abs(dot) - 1.0f) > 1e-6f)
			return false;

		float d = -glm::dot(n1, this->p1);
		float dist = glm::dot(n1, other.p1) + d;

		return std::abs(dist) < 1e-6f;
	}

	void triangle::calculate_normal() {
		normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
	}

	// POLYGON----------------------------------------------------------------------------------------------------------

	bool polygon::is_coplanar(const polygon& other) const {
		FENG_ASSERT(!points.empty() || !other.points.empty(), "Polygon is empty, can check if is coplanar");

		glm::vec3 n1 = normal;
		glm::vec3 n2 = other.normal;

		if (std::abs(std::abs(glm::dot(n1, n2)) - 1.0f) > 1e-6f) {
			return false;
		}

		float d = -glm::dot(n1, points[0]);

		for (const auto& p : other.points) {
			if (std::abs(glm::dot(n1, p) + d) > 1e-6f) {
				return false;
			}
		}

		return true;
	}

	bool polygon::is_coplanar(const glm::vec3& point) const {
		FENG_ASSERT(!points.empty(), "Polygon is empty, can check if is coplanar");

		float d1 = glm::dot(normal, points[0]);
		float d2 = glm::dot(normal, point);
		return glm::abs(d2 - d1) < COLLISION_EPSILON;
	}

	void polygon::calculate_normal() {
		FENG_ASSERT(points.size() >= 3, "Polygon doesn't have enough points.");

		glm::vec3 edge1 = points[1] - points[0];
		glm::vec3 edge2 = points[2] - points[0];

		normal = glm::normalize(glm::cross(edge1, edge2));
	}

	std::optional<edge> polygon::cyrus_beck_clip(const edge& seg, float epsilon) const {
		FENG_ASSERT(points.size() >= 3, "Not enough points for cyrus_beck_clip");

		//if (!is_coplanar(seg.p1) || !is_coplanar(seg.p2)) {
		//	LOG_INFO("Not coplanar");
		//	return std::nullopt;
		//}

		float t_enter = 0.0f;
		float t_exit = 1.0f;

		glm::vec3 d = seg.p2 - seg.p1;

		size_t n = points.size();
		for (size_t i = 0; i < n; ++i) {
			const glm::vec3& A = points[i];
			const glm::vec3& B = points[(i + 1) % n];
			glm::vec3 edge_vec = B - A;

			glm::vec3 edge_normal = glm::normalize(glm::cross(normal, edge_vec));

			float numerator = glm::dot(edge_normal, A - seg.p1);
			float denominator = glm::dot(edge_normal, d);

			if (std::abs(denominator) < epsilon) {
				if (numerator < 0) {
					LOG_INFO(1);
					return std::nullopt;
				}
				continue;
			}

			float t = numerator / denominator;

			if (denominator > 0) {
				if (t > t_enter) {
					t_enter = t;
				}
			}
			else {
				if (t < t_exit) {
					t_exit = t;
				}
			}

			if (t_enter > t_exit) {
				LOG_INFO(2);
				return std::nullopt;
			}
		}

		if (t_enter > 1.0f || t_exit < 0.0f) {
			LOG_INFO(3);
			return std::nullopt;
		}

		t_enter = glm::clamp(t_enter, 0.0f, 1.0f);
		t_exit = glm::clamp(t_exit, 0.0f, 1.0f);

		edge clipped_edge;
		clipped_edge.p1 = seg.p1 + t_enter * d;
		clipped_edge.p2 = seg.p1 + t_exit * d;

		return clipped_edge;
	}

	std::optional<polygon> polygon::sutherland_hodgman_clip(const polygon& clipper) {
		FENG_ASSERT(points.size() >= 3 && clipper.points.size() >= 3, "Not enough points for sutherland_hodgman_clip");

		if (!is_coplanar(clipper)) {
			return std::nullopt;
		}

		std::vector<glm::vec3> input = points;
		size_t clip_count = clipper.points.size();
		size_t input_count = 0;
		std::vector<glm::vec3> output;
		for (size_t i = 0; i < clip_count; ++i) {
			const glm::vec3& A = clipper.points[i];
			const glm::vec3& B = clipper.points[(i + 1) % clip_count];

			glm::vec3 edge_vec = B - A;
			glm::vec3 clip_normal = glm::normalize(glm::cross(clipper.normal, edge_vec));

			input_count = input.size();
			output.clear();
			output.reserve(input_count);
			for (size_t j = 0; j < input_count; ++j) {
				const glm::vec3& P = input[j];
				const glm::vec3& Q = input[(j + 1) % input_count];

				float dp1 = glm::dot(Q - A, clip_normal);
				float dp0 = glm::dot(P - A, clip_normal);

				bool P_inside = dp0 >= 0.0f;
				bool Q_inside = dp1 >= 0.0f;

				if (P_inside && Q_inside) {
					output.push_back(Q);
				}
				else if (P_inside && !Q_inside) {
					glm::vec3 dir = Q - P;
					float t = glm::dot(A - P, clip_normal) / glm::dot(dir, clip_normal);
					output.push_back(P + t * dir);
				}
				else if (!P_inside && Q_inside) {
					glm::vec3 dir = Q - P;
					float t = glm::dot(A - P, clip_normal) / glm::dot(dir, clip_normal);
					output.push_back(P + t * dir);
					output.push_back(Q);
				}
			}
			input = std::move(output);

			if (input.empty()) {
				return std::nullopt;
			}
		}

		polygon res;
		res.points = std::move(input);
		res.calculate_normal();
		return res;
	}

	// EDGE-------------------------------------------------------------------------------------------------------------

	edge edge::overlap(const edge& e1, const edge& e2, float epsilon) {
		FENG_ASSERT(are_on_same_line(e1, e2, epsilon), "Cannot compute overlap of edges that are not colinear.");

		glm::vec3 dir = e1.p2 - e1.p1;
		glm::vec3 unit_dir = glm::normalize(dir);

		auto project = [&](const glm::vec3& p) -> float {
			return glm::dot(p - e1.p1, unit_dir);
			};

		float a1 = 0.0f;                      
		float a2 = glm::length(dir);          
		float b1 = project(e2.p1);
		float b2 = project(e2.p2);

		if (a2 < a1) std::swap(a1, a2);
		if (b2 < b1) std::swap(b1, b2);

		float start = std::max(a1, b1);
		float end = std::min(a2, b2);

		FENG_ASSERT(end >= start - epsilon, "The edges do not overlap.");

		glm::vec3 p_start = e1.p1 + unit_dir * start;
		glm::vec3 p_end = e1.p1 + unit_dir * end;

		return edge{ p_start, p_end };
	}

	bool edge::are_on_same_line(const edge& e1, const edge& e2, float epsilon) {
		glm::vec3 d1 = e1.p2 - e1.p1;
		glm::vec3 d2 = e2.p2 - e2.p1;

		if (glm::length(glm::cross(d1, d2)) > epsilon)
			return false;

		glm::vec3 v = e2.p1 - e1.p1;
		if (glm::length(glm::cross(v, d1)) > epsilon)
			return false;

		return true;
	}

	// COLLISION_CONTACT------------------------------------------------------------------------------------------------

	collision_contact collision_contact::overlap(const collision_contact& c1, const collision_contact& c2) {
		// poins are points
		if (c1.type == collision_contact_type::point) {
			return c1;
		}

		if (c2.type == collision_contact_type::point) {
			return c2;
		}

		// edge - edge
		collision_contact cc;
		if (c1.type == collision_contact_type::edge && c2.type == collision_contact_type::edge) {
			LOG_WARNING("Untested part");
			// mb have to add penetration
			cc.type = collision_contact_type::edge;
			cc.data = edge::overlap(std::get<edge>(c1.data), std::get<edge>(c1.data));
			return cc;
		}

		// edge - polygon
		if (c1.type == collision_contact_type::edge && c2.type == collision_contact_type::polygon) {
			return overlap_edge_and_polygon(c1, c2);
		}

		if (c1.type == collision_contact_type::polygon && c2.type == collision_contact_type::edge) {
			return overlap_edge_and_polygon(c2, c1);
		}
		
		// both polygons
		std::optional<polygon> rp = c1.get<polygon>().sutherland_hodgman_clip(c2.get<polygon>());

		if (rp.has_value()) {
			cc.data = rp.value();
			cc.type = collision_contact_type::polygon;
		}
		else {
			THROW_ERROR("Polygons are not overlapping.");
		}

		return cc;
	}

	collision_contact collision_contact::overlap_edge_and_polygon(const collision_contact& ed, 
		const collision_contact& pl) {
		std::optional<edge> res = std::get<polygon>(pl.data).cyrus_beck_clip(std::get<edge>(ed.data));

		collision_contact cc;

		if (res.has_value()) {
			cc.data = res.value();
			cc.type = collision_contact_type::edge;
		}
		else {
			THROW_ERROR("Edge is not overlapping with polygon.");
		}

		return cc;
	}

	// COLLISION DATA---------------------------------------------------------------------------------------------------

	void collision_data::invert() {
		axis *= -1;
	}

	// COLLIDER_BASE--------------------------------------------------------------------------------------------------------

	bool collider_base::was_changed_after_update() {
		return _was_changed_after_update;
	}

	float collider_base::calculate_penetration(const glm::vec2& v1, const glm::vec2& v2) {
		float penetration = std::min(v1.y, v2.y) - std::max(v1.x, v2.x);
		return penetration;
	}

	bool collider_base::check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis) {
		float penetration = calculate_penetration(proj1, proj2);
		if (penetration <= 0.0f) {
			return false;
		}
		else if (penetration < lcd.penetration) {
			lcd.penetration = penetration;
			lcd.axis = axis;
		}
		return true;
	}

	bool collider_base::check_axis(collider_base* other, const glm::vec3& axis) {
		glm::vec2 proj1 = project_onto(axis);
		glm::vec2 proj2 = other->project_onto(axis);

		return check_overlap(proj1, proj2, axis);
	}

	// SAT_COLLIDER-----------------------------------------------------------------------------------------------------

	sat_collider_base::sat_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals)
		: _points(points), _normals(normals) {
		calculate_center();
	}

	bool sat_collider_base::collides(collider_base* other) {
		bool res = other->collides_shape(this);
		//out->invert();
		return res;
	}

	bool sat_collider_base::collides_shape(sat_collider_base* other) {
		for (const glm::vec3& axis : _normals) {
			if (!check_axis(other, axis)) {
				return false;
			}
		}
		for (const glm::vec3& axis : other->_normals) {
			if (!check_axis(other, axis)) {
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
		float d = glm::dot(dir, lcd.axis);
		if (d > 0) {
			lcd.invert();
		}
		collision_contact c1 = calculate_collision_contact(lcd.axis);
		collision_contact c2 = other->calculate_collision_contact(-1.0f * lcd.axis);
		lcd.contact = collision_contact::overlap(c1, c2);

		other->lcd.penetration = lcd.penetration;
		other->lcd.axis = -1.0f * lcd.axis;
		other->lcd.contact = lcd.contact;

		return true;
	}

	bool sat_collider_base::collides_shape(sphere_collider_base* other) {
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

		if (!check_axis(other, glm::normalize(sphere_axis))) {
			return false;
		}
		for (const glm::vec3& n : _normals) {
			if (!check_axis(other, n)) {
				return false;
			}
		}

		glm::vec3 dir = sphere_center - _center;
		float d = glm::dot(dir, lcd.axis);
		if (d > 0) {
			lcd.invert();
		}

		// copy lcd to other
		collision_contact c1 = calculate_collision_contact(lcd.axis);
		collision_contact c2 = other->calculate_collision_contact(-1.0f * lcd.axis);
		lcd.contact = collision_contact::overlap(c1, c2);

		other->lcd.penetration = lcd.penetration;
		other->lcd.axis = -1.0f * lcd.axis;
		other->lcd.contact = lcd.contact;

		return true;
	}

	collision_contact sat_collider_base::calculate_collision_contact(const glm::vec3& axis) const {
		float min = FLT_MAX;

		std::vector<glm::vec3> closest;

		for (const glm::vec3& p : _points) {
			float d = glm::dot(p, axis);
			if (d < min) { 
				min = d; 
				closest.clear();
				closest.push_back(p);
			}
			// fuck this floats
			else if (utilities::compare_floats(d, min, COLLISION_EPSILON)) {
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
			polygon p;
			p.points = std::move(closest);
			p.calculate_normal();
#ifdef FENG_DEBUG
			FENG_ASSERT(utilities::compare_normals_nd(axis, p.normal, COLLISION_EPSILON), "Noraml and axis are not the same");
#endif
			cc.data = p;
			cc.type = collision_contact_type::polygon;
		}
		return cc;
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

	void sat_collider_base::calculate_center() {
		_center = glm::vec3(0.0f);
		FENG_ASSERT(!_points.empty(), "Collider points are empty.");

		_center = std::reduce(_points.begin(), _points.end(), glm::vec3(0.0f));
		_center /= static_cast<float>(_points.size());
	}

	// SPHERE_COLLIDER--------------------------------------------------------------------------------------------------

	sphere_collider_base::sphere_collider_base(const glm::vec3& center, float radius) 
		: _center(center), _radius(radius) {
		FENG_ASSERT(radius >= 0, "sphere_collider doesn`t support negative radius");
	}

	bool sphere_collider_base::collides(collider_base* other) {
		bool res = other->collides_shape(this);
		//out->invert();
		return res;
	}

	bool sphere_collider_base::collides_shape(sphere_collider_base* other) {
		float fact_dist = glm::length2(_center - other->_center);
		float r_sum = _radius + other->_radius;
		if (fact_dist > r_sum * r_sum) {
			return false;
		}

		float dist = glm::sqrt(fact_dist);
		lcd.penetration = r_sum - dist;
		glm::vec3 dir = other->_center - _center;
		lcd.axis = dist > 0.0f ? dir / dist : dir / glm::vec3(0, 1, 0);
		return true;
	}

	bool sphere_collider_base::collides_shape(sat_collider_base* other) {
		bool res = other->collides_shape(this);
		//out->invert();
		return res;
	}

	float sphere_collider_base::get_radius() const {
		return _radius;
	}

	glm::vec3 sphere_collider_base::get_center() const {
		return _center;
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

	collision_contact sphere_collider_base::calculate_collision_contact(const glm::vec3& axis) const {
		collision_contact cc;
		cc.data = _center + lcd.axis * _radius;
		cc.type = collision_contact_type::point;
		return cc;
	}

}