#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

namespace feng::phys::gjk {

	// thx to: https://winter.dev/articles/gjk-algorithm

	struct collider {
		virtual glm::vec3 find_furthest_point(const glm::vec3& axis) const = 0;
	};

	class simplex {
	public:
		simplex();

		simplex& operator=(std::initializer_list<glm::vec3> list);
		glm::vec3& operator[](size_t i);

		glm::vec3& at(size_t i);

		void push_front(const glm::vec3& point);
		uint8_t size() const;

		auto begin() const {
			return _points.begin();
		}

		auto end() const {
			return _points.end() - (4 - _size);
		}

	private:
		std::array<glm::vec3, 4> _points;
		uint8_t _size;

	};

	bool gjk(const collider* col_1, const collider* col_2, simplex* out);

	namespace impl {
		glm::vec3 support(const collider* col1, const collider* col2, const glm::vec3& axis);
		bool next_simplex(simplex* points, glm::vec3& dir);
		bool sample_direction(const glm::vec3& dir, const glm::vec3& axis);
		bool line(simplex* points, glm::vec3& dir);
		bool triangle(simplex* points, glm::vec3& dir);
		bool tetrahedron(simplex* points, glm::vec3& dir);
	}

}