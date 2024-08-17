#pragma once

#include "../graphics/window.h"
#include <iostream>
#include <glm/glm.hpp>

#include "../logic/event.hpp"

namespace feng {

	class input {
	public:
		static bool get_key(uint32_t key);
		static bool get_key_down(uint32_t key);
		static bool get_key_up(uint32_t key);

		static bool get_mouse_key(uint32_t key);
		static bool get_mouse_key_down(uint32_t key);
		static bool get_mouse_key_up(uint32_t key);

		static glm::vec2 get_mouse_pos();
		static glm::vec2 get_mouse_pos_ndc();
		static glm::vec2 get_mouse_delta_pos();
		static glm::vec2 get_mouse_scroll_delta();

		static glm::vec2 get_click_start_pos();

		static bool is_cursor_in_window();
		static bool is_cursor_enter_leave_status_changed();

		static void update_inputs();

	public:
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void cursor_pos_callback(GLFWwindow* window, double _x, double _y);
		static void mouse_button_callback(GLFWwindow* window, int buttn, int action, int mods);
		static void mouse_wheel_callback(GLFWwindow* window, double dx, double dy);
		static void cursor_enter_leave_callback(GLFWwindow* window, int entered);

	private:
		static bool _keys[GLFW_KEY_LAST];
		static bool _keys_down[GLFW_KEY_LAST];
		static bool _keys_up[GLFW_KEY_LAST];

		static bool _mouse_keys[GLFW_MOUSE_BUTTON_LAST];
		static bool _mouse_keys_down[GLFW_MOUSE_BUTTON_LAST];
		static bool _mouse_keys_up[GLFW_MOUSE_BUTTON_LAST];

		static bool _keys_changed[GLFW_KEY_LAST];
		static bool _mouse_keys_changed[GLFW_MOUSE_BUTTON_LAST];

		static double x;
		static double y;

		static double last_x;
		static double last_y;

		static double dx_temp;
		static double dy_temp;

		static double dx;
		static double dy;

		static double scroll_dx_temp;
		static double scroll_dy_temp;

		static double scroll_dx;
		static double scroll_dy;

		static bool first_mouse;
		
		static bool _is_cursor_in_window; 
		static bool _is_cursor_enter_leave_status_changed;
		static bool _is_cursor_enter_leave_status_changed_flag;

		static glm::vec2 _click_start_pos;

	private:
		static bool key_changed(uint32_t key);
		static bool mouse_key_changed(uint32_t key);
	};

}