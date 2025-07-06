#include "shapes.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <numeric>

#include "../../logging/logging.h"
#include "../../utilities/utilities.h"

namespace feng::phys {

	// EDGE----

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

	// TRIANGLE ----

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3)
		: p1(_p1), p2(_p2), p3(_p3) {
		calculate_normal();
	}

	triangle::triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n)
		: p1(_p1), p2(_p2), p3(_p3), normal(_n) {
	}

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

	bool polygon::is_convex_and_correct_order() const {
		if (points.size() < 3) return false;

		glm::vec3 N = normal;

		bool is_positive = true;
		bool is_initialized = false;

		const size_t n = points.size();
		for (size_t i = 0; i < n; ++i) {
			const glm::vec3& A = points[i];
			const glm::vec3& B = points[(i + 1) % n];
			const glm::vec3& C = points[(i + 2) % n];

			glm::vec3 AB = B - A;
			glm::vec3 BC = C - B;
			glm::vec3 cross_prod = glm::cross(AB, BC);

			float dot_with_normal = glm::dot(N, cross_prod);

			if (!is_initialized) {
				is_positive = (dot_with_normal > 0);
				is_initialized = true;
			}
			else {
				if ((dot_with_normal > 0) != is_positive) {
					return false;
				}
			}
		}

		return true;
	}

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
		return glm::abs(d2 - d1) < 0.0001;
	}

	void polygon::calculate_normal() {
		FENG_ASSERT(points.size() >= 3, "Polygon doesn't have enough points.");

		glm::vec3 edge1 = points[1] - points[0];
		glm::vec3 edge2 = points[2] - points[0];

		normal = glm::normalize(glm::cross(edge1, edge2));
	}

	std::optional<edge> polygon::cyrus_beck_clip(const edge& seg, float epsilon) const {
		FENG_ASSERT(points.size() >= 3, "Not enough points for cyrus_beck_clip");
#ifdef  FENG_DEBUG
		FENG_ASSERT(is_convex_and_correct_order(), "Bad polygon");
#endif

		if (!is_coplanar(seg.p1) || !is_coplanar(seg.p2)) {
			return std::nullopt;
		}

		glm::vec3 d = seg.p2 - seg.p1;
		float t_enter = 0.0f;
		float t_exit = 1.0f;

		for (size_t i = 0; i < points.size(); ++i) {
			const glm::vec3& P0 = points[i];
			const glm::vec3& P1 = points[(i + 1) % points.size()];
			glm::vec3 edge_vec = P1 - P0;
			glm::vec3 edge_normal = glm::normalize(glm::cross(edge_vec, normal));

			glm::vec3 w = seg.p1 - P0;
			float denom = glm::dot(edge_normal, d);
			float numer = -glm::dot(edge_normal, w);

			if (std::abs(denom) < epsilon) {
				if (numer < 0) {
					return std::nullopt;
				}
				continue;
			}

			float t = numer / denom;
			if (denom > 0) {
				if (t < t_exit)
					t_exit = t;
			}
			else {
				if (t > t_enter)
					t_enter = t;
			}

			if (t_enter > t_exit) {
				return std::nullopt;
			}
		}

		glm::vec3 clipped_start = seg.p1 + d * t_enter;
		glm::vec3 clipped_end = seg.p1 + d * t_exit;
		return edge{ clipped_start, clipped_end };
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

	float polygon::orient(const glm::vec2& o, const glm::vec2& a, const glm::vec2& b) {
		return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
	}

	polygon polygon::make_convex_polygon(const std::vector<glm::vec3>& points) {
		size_t points_size = points.size();
		FENG_ASSERT(points_size >= 3, "Not enough points to make a polygon");

		glm::vec3 origin = points[0];
		glm::vec3 p1 = points[1];
		glm::vec3 p2 = points[2];

		glm::vec3 normal = glm::normalize(glm::cross(p1 - origin, p2 - origin));
		glm::vec3 x_axis = glm::normalize(p1 - origin);
		glm::vec3 y_axis = glm::normalize(glm::cross(normal, x_axis));

		struct point2d {
			glm::vec2 pos;
			glm::vec3 original;
		};

		std::vector<point2d> projected;
		projected.reserve(points_size);
		for (const glm::vec3& p : points) {
			glm::vec3 r_dir = p - origin;
			float r_x = glm::dot(x_axis, r_dir);
			float r_y = glm::dot(y_axis, r_dir);
			projected.emplace_back(glm::vec2(r_x, r_y), p);
		}

		std::swap(projected[0], *std::min_element(projected.begin(), projected.end(), [](const point2d& a, const point2d& b) {
			return (a.pos.y < b.pos.y) || (a.pos.y == b.pos.y && a.pos.x < b.pos.x);
			}));
		glm::vec2 pivot = projected[0].pos;

		std::sort(projected.begin(), projected.end(), [&pivot](const point2d& p1, const point2d& p2) {
			float ori = orient(pivot, p1.pos, p2.pos);
			if (ori == 0) {
				return glm::length2(p1.pos - pivot) < glm::length2(p2.pos - pivot);
			}
			return ori > 0;
			});

		std::vector<point2d> hull;
		size_t hs = 0;
		for (const auto& point : projected) {
			hs = hull.size();
			while (hs >= 2 && orient(hull[hs - 2].pos, hull[hs - 1].pos, point.pos) <= FLT_EPSILON) {
				hull.pop_back();
			}
			hull.push_back(point);
		}

		polygon res;
		res.normal = normal;
		for (const auto& point : hull) {
			res.points.push_back(point.original);
		}
		return res;
	}

}