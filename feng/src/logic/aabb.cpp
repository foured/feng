#include "aabb.h"

namespace feng {

	aabb::aabb(glm::vec3 min, glm::vec3 max)
		: min(min), max(max) { }

	void aabb::set_numeric_limits() {
		min = glm::vec3(std::numeric_limits<float>().max());
		max = glm::vec3(std::numeric_limits<float>().min());
	}

}