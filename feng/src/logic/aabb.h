#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace feng {

	class aabb {
	public:
		aabb(glm::vec3 min = glm::vec3(0), glm::vec3 max = glm::vec3(0));
		
		glm::vec3 min;
		glm::vec3 max;

		glm::vec3 size() const;
		glm::vec3 center() const;

		void set_numeric_limits();
		float volume() const;
		bool can_fit(const aabb& target) const;
		bool contains(const aabb& target) const;
		bool intersects(const aabb& target) const;

		aabb scale(const glm::mat4& model) const;
		aabb scale(const glm::vec3& v) const;
		aabb scale(float factor) const;
		aabb offset(const glm::vec3& p) const;
		aabb fit_rotation(const glm::mat3& rotation) const;

	};

}