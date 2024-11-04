#pragma once

#include <glm/glm.hpp>

namespace feng {

	class transform {
	public:
		transform(glm::vec3 pos = glm::vec3(0), glm::vec3 size = glm::vec3(0), glm::vec3 rotation = glm::vec3(0));

	private:
		glm::vec3 _position;
		glm::vec3 _size;
		glm::vec3 _rotation;

	};

}