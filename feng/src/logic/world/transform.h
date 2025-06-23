#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace feng {

	class transform {
	public:
		transform(glm::vec3 pos = glm::vec3(0.0), glm::vec3 size = glm::vec3(1.0), glm::vec3 rotation = glm::vec3(0.0));
		
		bool changed_this_frame = false;

		void set_position(glm::vec3 val);
		void set_size(glm::vec3 val);
		void set_rotation(glm::vec3 val); 

		glm::vec3 get_position() const;
		glm::vec3 get_size() const;
		glm::vec3 get_rotation() const;

		glm::mat3 get_rotation_matrix3x3() const;

	private:
		glm::vec3 _position;
		glm::vec3 _size;
		glm::vec3 _rotation{0, 0, 0};

		glm::mat3 _rotation_matrix = glm::identity<glm::mat3>();

	};

}