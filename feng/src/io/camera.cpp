#include "camera.h"

#include "../utilities/utilities.h"
#include "input.h"
#include "../graphics/window.h"

namespace feng {

	camera::camera(glm::vec3 pos, float sensitivity) 
		: 
		position(pos),
		_sensitivity(sensitivity),
		_yaw(-90.0f),
		_pitch(0.0f),
		_speed(8.0f),
		_zoom(45.0f) {
		update_vectors();
	}

	void camera::update_vectors() {
		glm::vec3 direction;
		direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		direction.y = sin(glm::radians(_pitch));
		direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

		_front = glm::normalize(direction);
		_right = glm::normalize(glm::cross(_front, glm::vec3(0.0, 1.0, 0.0)));
		_up = glm::normalize(glm::cross(_right, _front));
	}

	glm::mat4 camera::get_view_matrix() {
		return glm::lookAt(position, position + _front, _up);
	}

	void camera::controll_keyboard_presses(direction dir) {
		double velocity = _speed * utilities::delta_time();
		switch (dir)
		{
		case direction::FORWARD:
			position += _front * (float)velocity;
			break;
		case direction::BACK:
			position -= _front * (float)velocity;
			break;
		case direction::RIGHT:
			position += _right * (float)velocity;
			break;
		case direction::LEFT:
			position -= _right * (float)velocity;
			break;
		case direction::UP:
			position += glm::vec3(0.0, 1.0, 0.0) * (float)velocity;
			break;
		case direction::DOWN:
			position -= glm::vec3(0.0, 1.0, 0.0) * (float)velocity;
			break;
		}
		update_vectors();
	}

	void camera::controll_mouse_movement() {
		if (!input::is_cursor_enter_leave_status_changed()) {
			glm::vec2 d_pos = input::get_mouse_delta_pos() * _sensitivity;
			_yaw += d_pos.x;
			_pitch += d_pos.y;

			if (_pitch > 89.0f)
				_pitch = 89.0f;
			if (_pitch < -89.0f)
				_pitch = -89.0f;

			update_vectors();
		}
	}

	void camera::move() {
		if (input::get_key(GLFW_KEY_W)) controll_keyboard_presses(camera::direction::FORWARD);
		if (input::get_key(GLFW_KEY_S)) controll_keyboard_presses(camera::direction::BACK);
		if (input::get_key(GLFW_KEY_A)) controll_keyboard_presses(camera::direction::LEFT);
		if (input::get_key(GLFW_KEY_D)) controll_keyboard_presses(camera::direction::RIGHT);
		if (input::get_key(GLFW_KEY_SPACE)) controll_keyboard_presses(camera::direction::UP);
		if (input::get_key(GLFW_KEY_LEFT_SHIFT)) controll_keyboard_presses(camera::direction::DOWN);
		if (input::get_key_down(GLFW_KEY_TAB)) {
			_mouse_movement = !_mouse_movement;
			window::current_window->set_cursor_mode(!_mouse_movement);
		}

		if (_mouse_movement)
			controll_mouse_movement();
	}

	glm::vec3 camera::front() {
		return _front;
	}

}