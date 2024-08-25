#include "window.h"

#include <iostream>

#include "../logging/logging.h"
#include "../io/input.h"

namespace feng {

	uint16_t window::win_width = 0; 
	uint16_t window::win_height = 0; 

	window::window(const char* title, int width, int height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		_window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (_window == NULL)
		{
			glfwTerminate();
			THROW_ERROR("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(_window);
		glfwSwapInterval(0);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			THROW_ERROR("Failed to initialize GLAD");
		}
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		window::win_width = width;
		window::win_height = height;

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glViewport(0, 0, width, height);

		glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
		glfwSetKeyCallback(_window, input::key_callback);
		glfwSetMouseButtonCallback(_window, input::mouse_button_callback);
		glfwSetCursorPosCallback(_window, input::cursor_pos_callback);
		glfwSetScrollCallback(_window, input::mouse_wheel_callback);
		glfwSetCursorEnterCallback(_window, input::cursor_enter_leave_callback);
	}

	window::~window() {
		glfwTerminate();
	}

	GLFWwindow* window::get() {
		return _window;
	}

	bool window::should_close() {
		return glfwWindowShouldClose(_window);
	}

	void window::swap_buffers() {
		glfwSwapBuffers(_window);
	}

	void window::process_input() {
		input::update_inputs();

		if (input::get_key_up(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(_window, true);
	}

	void window::update() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// static

	void window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		window::win_width = width;
		window::win_height = height;
	}
}