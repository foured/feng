#include "input.h"


namespace feng {

	bool input::_keys[GLFW_KEY_LAST] = { 0 };
	bool input::_keys_down[GLFW_KEY_LAST] = { 0 };
	bool input::_keys_up[GLFW_KEY_LAST] = { 0 };

	bool input::_mouse_keys[GLFW_MOUSE_BUTTON_LAST] = { 0 };
	bool input::_mouse_keys_down[GLFW_MOUSE_BUTTON_LAST] = { 0 };
	bool input::_mouse_keys_up[GLFW_MOUSE_BUTTON_LAST] = { 0 };

	bool input::_keys_changed[GLFW_KEY_LAST] = { 0 };
	bool input::_mouse_keys_changed[GLFW_MOUSE_BUTTON_LAST] = { 0 };

	double input::x = 0;
	double input::y = 0;

	double input::last_x = 0;
	double input::last_y = 0;

	double input::dx_temp = 0;
	double input::dy_temp = 0;

	double input::dx = 0;
	double input::dy = 0;

	double input::scroll_dx_temp = 0;
	double input::scroll_dy_temp = 0;

	double input::scroll_dx = 0;
	double input::scroll_dy = 0;

	glm::vec2 input::_click_start_pos;

	bool input::first_mouse = true;

	bool input::_is_cursor_in_window = false;
	bool input::_is_cursor_enter_leave_status_changed = false;
	bool input::_is_cursor_enter_leave_status_changed_flag = false;

	//
	// methods
	//

	bool input::get_key(uint32_t key) {
		return _keys[key];
	}

	bool input::get_key_up(uint32_t key) {
		return _keys_up[key];
	}

	bool input::get_key_down(uint32_t key) {
		return _keys_down[key];
	}

	bool input::get_mouse_key(uint32_t key) {
		return _mouse_keys[key];
	}

	bool input::get_mouse_key_down(uint32_t key){
		return _mouse_keys_down[key];
	}

	bool input::get_mouse_key_up(uint32_t key){
		return _mouse_keys_up[key];
	}

	void input::update_inputs() {
		bool current_key_changed;
		for (size_t i = 0; i < GLFW_KEY_LAST; i++) {
			current_key_changed = key_changed(i);
			_keys_up[i] = !_keys[i] && current_key_changed;
			_keys_down[i] = _keys[i] && current_key_changed;
		}

		for (size_t i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
			current_key_changed = mouse_key_changed(i);
			_mouse_keys_up[i] = !_mouse_keys[i] && current_key_changed;
			_mouse_keys_down[i] = _mouse_keys[i] && current_key_changed;
		}

		if (_mouse_keys_down[GLFW_MOUSE_BUTTON_LEFT])
			_click_start_pos = get_mouse_pos_ndc();

		scroll_dx = scroll_dx_temp;
		scroll_dy = scroll_dy_temp;
		scroll_dx_temp = 0;
		scroll_dy_temp = 0;

		dx = dx_temp;
		dy = dy_temp;
		dx_temp = 0;
		dy_temp = 0;

		if (_is_cursor_enter_leave_status_changed_flag && _is_cursor_enter_leave_status_changed)
			_is_cursor_enter_leave_status_changed_flag = false;
		else if (!_is_cursor_enter_leave_status_changed_flag && _is_cursor_enter_leave_status_changed)
			_is_cursor_enter_leave_status_changed = false;
	}

	bool input::key_changed(uint32_t key) {
		bool ret = _keys_changed[key];
		_keys_changed[key] = false;
		return ret;
	}

	bool input::mouse_key_changed(uint32_t key) {
		bool ret = _mouse_keys_changed[key];
		_mouse_keys_changed[key] = false;
		return ret;
	}

	glm::vec2 input::get_mouse_pos(){
		return glm::vec2(x, y);
	}

	glm::vec2 input::get_mouse_pos_ndc() {
		glm::vec2 pp(x / window::win_width, y / window::win_height);
		return glm::vec2(pp.x * 2.0f - 1.0f, 1.0f - pp.y * 2.0f);
	}

	glm::vec2 input::get_mouse_delta_pos() {
		return glm::vec2(dx, dy);
	}

	glm::vec2 input::get_mouse_scroll_delta() {
		return glm::vec2(scroll_dx, scroll_dy);
	}

	bool input::is_cursor_in_window() {
		return _is_cursor_in_window;
	}

	bool input::is_cursor_enter_leave_status_changed() {
		return _is_cursor_enter_leave_status_changed;
	}

	glm::vec2 input::get_click_start_pos() {
		return _click_start_pos;
	}

	//
	// callbacks
	//

	void input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action != GLFW_RELEASE) {
			if (!_keys[key]) {
				_keys[key] = true;
			}
		}
		else {
			_keys[key] = false;
		}

		_keys_changed[key] = action != GLFW_REPEAT;
	}

	void input::cursor_pos_callback(GLFWwindow* window, double _x, double _y) {
		x = _x;
		y = _y;

		if (first_mouse) {
			last_x = x;
			last_y = y;
			first_mouse = false;
		}

		dx_temp = x - last_x;
		dy_temp = last_y - y;

		last_x = x;
		last_y = y;
	}

	void input::mouse_button_callback(GLFWwindow* window, int buttn, int action, int mods) {
		if (action != GLFW_RELEASE) {
			if (!_mouse_keys[buttn]) {
				_mouse_keys[buttn] = true;
			}
		}
		else {
			_mouse_keys[buttn] = false;
		}

		_mouse_keys_changed[buttn] = action != GLFW_REPEAT;
	}

	void input::mouse_wheel_callback(GLFWwindow* window, double dx, double dy) {
		scroll_dx_temp = dx;
		scroll_dy_temp = dy;
	}

	void input::cursor_enter_leave_callback(GLFWwindow* window, int entered) {
		_is_cursor_in_window = entered;
		_is_cursor_enter_leave_status_changed = true;
		_is_cursor_enter_leave_status_changed_flag = true;
	}

}