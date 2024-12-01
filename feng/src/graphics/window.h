#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace feng {

	class window {
	public:
		window(const char* title, int width, int height);
		~window();

		GLFWwindow* get();
		bool should_close();
		void swap_buffers();

		void process_input();
		void update();

		void set_cursor_mode(bool enable);

		static uint16_t win_width, win_height;
		static window* current_window;

	private:
		GLFWwindow* _window;

	private:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void error_callback(int error, const char* description);
	};

}