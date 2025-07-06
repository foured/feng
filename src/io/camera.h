#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace feng{

	class camera {
	public:

		enum class direction {
			FORWARD,
			BACK,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 3.0), float sensitivity = 0.4f);

		glm::vec3 position;

		glm::mat4 get_view_matrix();
		void update_vectors();
		void move();

		glm::vec3 front();

	private:
		glm::vec3 _up, _right, _front;
		bool _mouse_movement = true;

		float _sensitivity;
		float _yaw;
		float _pitch;
		float _speed;
		float _zoom;

		void controll_mouse_movement();
		void controll_keyboard_presses(direction dir);

	};

}