#pragma once

#include <glm/glm.hpp>

namespace feng {

	class transform {
	public:
		transform(glm::vec3 pos = glm::vec3(0.0), glm::vec3 size = glm::vec3(1.0), glm::vec3 rotation = glm::vec3(0.0));
		
		void set_position(glm::vec3 val);
		void set_size(glm::vec3 val);
		void set_rotation(glm::vec3 val);

		glm::vec3 get_position() const;
		glm::vec3 get_size() const;
		glm::vec3 get_rotation() const;

	private:
		glm::vec3 _position;
		glm::vec3 _size;
		glm::vec3 _rotation;

	};

}