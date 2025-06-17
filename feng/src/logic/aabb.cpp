#include "aabb.h"

namespace feng {

	aabb::aabb(glm::vec3 min, glm::vec3 max)
		: min(min), max(max) { }

	void aabb::set_numeric_limits() {
		min = glm::vec3(std::numeric_limits<float>().max());
		max = glm::vec3(std::numeric_limits<float>().min());
	}
	
	float aabb::volume() const{
		glm::vec3 size = glm::abs(max - min);
		float v = 1.0f;
		if (size.x != 0)
			v *= size.x;
		if (size.y != 0)
			v *= size.y;
		if (size.z != 0)
			v *= size.z;
		return v;
	}

	aabb aabb::scale(const glm::mat4& model) {
		glm::vec3 nmax = glm::vec3(model * glm::vec4(max, 1.0f));
		glm::vec3 nmin = glm::vec3(model * glm::vec4(min, 1.0f));
		return aabb(nmax, nmin);
	}


}