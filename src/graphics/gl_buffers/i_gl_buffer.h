#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace feng {

	struct i_gl_buffer {
		virtual ~i_gl_buffer() = default;
		virtual void generate() = 0;
		virtual void bind() = 0;
		virtual void unbind() = 0;
		virtual uint32_t id() const = 0;
		virtual int32_t type() const = 0;
	};

}