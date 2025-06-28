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


	aabb aabb::scale(const glm::mat4& model) const {
		glm::vec3 nmax = glm::vec3(model * glm::vec4(max, 1.0f));
		glm::vec3 nmin = glm::vec3(model * glm::vec4(min, 1.0f));
		return aabb(nmin, nmax);
	}

	aabb aabb::scale(const glm::vec3& v) const {
		return aabb(min * v, max * v);
	}

	aabb aabb::scale(float factor) const {
		return aabb(min * factor, max * factor);
	}

	aabb aabb::offset(const glm::vec3& p) const {
		return aabb(min + p, max + p);
	}

	aabb aabb::fit_rotation(const glm::mat3& rotation) const {
		glm::vec3 c = center();
		glm::vec3 e = 0.5f * size();

		glm::mat3 abs_rot = glm::mat3(
			glm::abs(rotation[0]),
			glm::abs(rotation[1]),
			glm::abs(rotation[2])
		);

		glm::vec3 new_e = abs_rot * e;

		return { c - new_e , c + new_e };
	}

	aabb aabb::intersect_unsafe(const aabb& target) {
		glm::vec3 rmin{
			std::max(min.x, target.min.x),
			std::max(min.y, target.min.y),
			std::max(min.z, target.min.z)
		};
		glm::vec3 rmax{
			std::min(max.x, target.max.x),
			std::min(max.y, target.max.y),
			std::min(max.z, target.max.z)
		};
		return aabb(rmin, rmax);
	}

	std::optional<aabb> aabb::intersect_safe(const aabb& target) {
		if (intersects(target)) {
			return intersect_unsafe(target);
		}
		return std::nullopt;
	}

}