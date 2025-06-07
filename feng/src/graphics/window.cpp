#include "window.h"

#include <iostream>

#include "../logging/logging.h"
#include "../io/input.h"

namespace feng {

	int32_t window::win_width = 0;
	int32_t window::win_height = 0;
	window* window::current_window = nullptr;
	feng::event<int32_t, int32_t> window::on_framebuffer_size;


	window::window(const char* title, int width, int height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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
		//lfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		set_cursor_mode(false);

		window::win_width = width;
		window::win_height = height;
		window::current_window = this;

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glViewport(0, 0, width, height);

		glfwSetErrorCallback(error_callback);
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

	void window::set_cursor_mode(bool enable) {
		if (enable)
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}

	// static
	
	void window::framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
		glViewport(0, 0, width, height);
		window::win_width = width;
		window::win_height = height;
		window::on_framebuffer_size.invoke(width, height);
	}

	void window::error_callback(int error, const char* description) {
		LOG_ERROR("OpenGL error(" + std::to_string(error) + "): " + std::string(description) + ".");
	}

}