#include "aabb.h"

namespace feng {

	aabb::aabb(glm::vec3 min, glm::vec3 max)
		: min(min), max(max) { }

	glm::vec3 aabb::size() const {
		return max - min;
	}

	glm::vec3 aabb::center() const {
		return (min + max) * 0.5f;
	}

	void aabb::set_numeric_limits() {
		min = glm::vec3(std::numeric_limits<float>().max());
		max = glm::vec3(std::numeric_limits<float>().min());
	}
	
	float aabb::volume() const{
		glm::vec3 size = max - min;
		return size.x * size.y * size.z;
	}

	aabb aabb::scale(const glm::mat4& model) const {
		glm::vec3 nmax = glm::vec3(model * glm::vec4(max, 1.0f));
		glm::vec3 nmin = glm::vec3(model * glm::vec4(min, 1.0f));
		return aabb(nmin, nmax);
	}

	aabb aabb::scale(float factor) const {
		glm::vec3 nmin = factor * min;
		glm::vec3 nmax = factor * max;
		return aabb(nmin, nmax);
	}

	bool aabb::can_fit(const aabb& target) const {
		glm::vec3 csize = size();
		glm::vec3 tsize = target.size();
		return tsize.x <= csize.x && tsize.y <= csize.y && tsize.z <= csize.z;
	}

	bool aabb::contains(const aabb& target) const {
		return 
			target.min.x >= min.x && target.max.x <= max.x &&
			target.min.y >= min.y && target.max.y <= max.y &&
			target.min.z >= min.z && target.max.z <= max.z;
	}

	bool aabb::intersects(const aabb& target) const {
		return
			max.x >= target.min.x && min.x <= target.max.x &&
			max.y >= target.min.y && min.y <= target.max.y &&
			max.z >= target.min.z && min.z <= target.max.z;
	}

}