#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace feng::phys {

	class base_collider_data {
	protected:
		glm::vec3 _center;
	};

	class mesh_collider_data : virtual public base_collider_data {
	protected:
		std::vector<glm::vec3> _points;
		std::vector<glm::vec3> _normals;
	};

	class sphere_collider_data : virtual public base_collider_data {
	protected:
		float _radius;

	};

}