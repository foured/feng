#pragma once

#include <glm/glm.hpp>

namespace feng {

	class aabb {
	public:
		aabb(glm::vec3 min = glm::vec3(0), glm::vec3 max = glm::vec3(0));
		
		glm::vec3 min;
		glm::vec3 max;

		glm::vec3 size() const;
		void set_numeric_limits();
		float volume() const;
		aabb scale(const glm::mat4& model);
		bool can_fit(const aabb& target) const;
		bool contains(const aabb& target) const;

	};

}