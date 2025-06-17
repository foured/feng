#pragma once

#include <glm/glm.hpp>

namespace feng {

	class aabb {
	public:
		aabb(glm::vec3 min = glm::vec3(0), glm::vec3 max = glm::vec3(0));
		
		glm::vec3 min;
		glm::vec3 max;

		void set_numeric_limits();

		// Must be fixed for objects with x, y or z = 0
		float volume() const;

		aabb scale(const glm::mat4& model);

	};

}