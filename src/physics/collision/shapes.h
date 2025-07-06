#pragma once

#include <vector>
#include <optional>
#include <glm/glm.hpp>

namespace feng::phys {
	struct edge {
		glm::vec3 p1, p2;

		static edge overlap(const edge& e1, const edge& e2, float epsilon = FLT_EPSILON);
		static bool are_on_same_line(const edge& e1, const edge& e2, float epsilon = FLT_EPSILON);

	};

	struct triangle {

		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);
		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n);

		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;

		glm::vec3 normal;

		bool is_coplanar(const triangle& other) const;
		void calculate_normal();

	};

	struct polygon {
		std::vector<glm::vec3> points;
		glm::vec3 normal;

		bool is_convex_and_correct_order() const;
		bool is_coplanar(const polygon& other) const;
		bool is_coplanar(const glm::vec3& point) const;
		void calculate_normal();

		std::optional<edge> cyrus_beck_clip(const edge& seg, float epsilon = FLT_EPSILON) const;
		std::optional<polygon> sutherland_hodgman_clip(const polygon& clipper);

		static float orient(const glm::vec2& o, const glm::vec2& a, const glm::vec2& b);
		static polygon make_convex_polygon(const std::vector<glm::vec3>& points);

	};

}