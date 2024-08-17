#pragma once

#include "gl_buffer.hpp"

namespace feng {

	class uniformbuffer : public gl_buffer {
	public:
		uniformbuffer(bool autogen = false) 
			: gl_buffer(GL_UNIFORM_BUFFER, autogen) { }

		void fast_setup(uint32_t index, uint32_t data_size, int32_t draw_mode = GL_STATIC_DRAW, uint32_t offset = 0) {
			generate();
			bind();
			buffer_data<int32_t>(data_size, NULL, draw_mode);
			unbind();
			bind_buffer_range(index, offset, data_size);
		}
	};

}