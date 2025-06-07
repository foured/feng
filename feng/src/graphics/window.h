#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../logic/event.hpp"


namespace feng {

	class window {
	public:
		static int32_t win_width, win_height;
		static window* current_window;

		static feng::event<int32_t, int32_t> on_framebuffer_size;

		window(const char* title, int width, int height);
		~window();

		GLFWwindow* get();
		bool should_close();
		void swap_buffers();

		void process_input();
		void update();

		void set_cursor_mode(bool enable);

	private:
		GLFWwindow* _window;

		static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);
		static void error_callback(int error, const char* description);
	};

}