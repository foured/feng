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

		static uint16_t win_width, win_height;

	private:
		GLFWwindow* _window;

	private:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	};

}