#include "gjk.h"

#include "../../../logging/logging.h"

namespace feng::phys::gjk {

	// SIMPLEX -----------

	simplex::simplex() 
		: _size(0) { }

	simplex& simplex::operator=(std::initializer_list<glm::vec3> list) {
		_size = 0;
		for (const auto& point : list) {
			_points[_size++] = point;
		}
		return *this;
	}

	glm::vec3& simplex::operator[](size_t i) {
		return at(i);
	}

	glm::vec3& simplex::at(size_t i) {
#ifdef FENG_DEBUG
		FENG_ASSERT(i < _size, "Not enough points in simplex.");
#endif
		return _points[i];
	}

	void simplex::push_front(const glm::vec3& point) {
		_points = { point, _points[0], _points[1], _points[2] };
		_size = std::min(4, _size + 1);
	}

	uint8_t simplex::size() const {
		return _size;
	}

	// GJK ---------------

	bool gjk(const collider* col_1, const collider* col_2, simplex* out) {
		glm::vec3 support = impl::support(col_1, col_2, glm::vec3(1, 0, 0));

		out->push_front(support);

		glm::vec3 direction = -support;
		while (true) {
			support = impl::support(col_1, col_2, direction);

			if (glm::dot(support, direction) <= 0) {
				return false;
			}

			out->push_front(support);

			if (impl::next_simplex(out, direction)) {
				return true;
			}
		}
	}

	namespace impl {
		glm::vec3 support(const collider* col1, const collider* col2, const glm::vec3& axis) {
			return col1->find_furthest_point(axis) - col2->find_furthest_point(-axis);
		}

		bool next_simplex(simplex* points, glm::vec3& dir) {
			switch (points->size()) {
			case 2:
				return line(points, dir);
			case 3:
				return triangle(points, dir);
			case 4:
				return tetrahedron(points, dir);
			}

			LOG_WARNING("Never should be here (feng::physics::gjk::impl::next_simplex -> false)");
			return false;
		}

		bool sample_direction(const glm::vec3& dir, const glm::vec3& axis) {
			return glm::dot(dir, axis) > 1e-6f;
		}

		bool line(simplex* points, glm::vec3& dir) {
			glm::vec3 a = points->at(0);
			glm::vec3 b = points->at(1);

			glm::vec3 ab = b - a;
			glm::vec3 ao = -a;

			if (sample_direction(ab, ao)) {
				dir = glm::cross(glm::cross(ab, ao), ab);
			}
			else {
				*points = { a };
				dir = ao;
			}

			return false;
		}

		bool triangle(simplex* points, glm::vec3& dir) {
			glm::vec3 a = points->at(0);
			glm::vec3 b = points->at(1);
			glm::vec3 c = points->at(2);

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 ao = -a;

			glm::vec3 abc = glm::cross(ab, ac);

			if (sample_direction(glm::cross(abc, ac), ao)) {
				if (sample_direction(ac, ao)) {
					*points = { a, c };
					dir = glm::cross(glm::cross(ac, ao), ac);
				}
				else {
					*points = { a, b };
					return line(points, dir);
				}
			}
			else {
				if (sample_direction(glm::cross(ab, abc), ao)) {
					*points = { a , b };
					return line(points, dir);
				}
				else {
					if (sample_direction(abc, ao)) {
						dir = abc;
					}
					else {
						*points = { a, c, b };
						dir = -abc;
					}
				}
			}

			return false;
		}

		bool tetrahedron(simplex* points, glm::vec3& dir) {
			glm::vec3 a = points->at(0);
			glm::vec3 b = points->at(1);
			glm::vec3 c = points->at(2);
			glm::vec3 d = points->at(3);

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 ad = d - a;
			glm::vec3 ao = -a;

			glm::vec3 abc = glm::cross(ab, ac);
			glm::vec3 acd = glm::cross(ac, ad);
			glm::vec3 adb = glm::cross(ad, ab);

			if (sample_direction(abc, ao)) {
				*points = { a, b, c };
				return triangle(points, dir);
			}
			if (sample_direction(acd, ao)) {
				*points = { a, c, d };
				return triangle(points, dir);
			}
			if (sample_direction(adb, ao)) {
				*points = { a, d, b };
				return triangle(points, dir);
			}

			return true;
		}
	}


}