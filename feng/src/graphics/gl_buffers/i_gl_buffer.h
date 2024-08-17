#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace feng {

	class i_gl_buffer {
	public:
		virtual void generate() = 0;
		virtual void bind() = 0;
		virtual void unbind() = 0;
		virtual uint32_t id() = 0;
		virtual int32_t type() = 0;
	};

}