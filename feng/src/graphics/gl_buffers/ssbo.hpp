#pragma once

#include "gl_buffer.hpp"

namespace feng {

	class ssbo : public gl_buffer {
	public:
		ssbo()
			: gl_buffer(GL_SHADER_STORAGE_BUFFER, false) { }

		void allocate(uint32_t data_size, uint32_t binding_slot) {
			generate();
			bind();
			buffer_data<uint32_t>(data_size, NULL, GL_DYNAMIC_STORAGE_BIT);
			bind_buffer_base(binding_slot);
			unbind();
		}

		void update(uint32_t offset, uint32_t size, void* data) {
			bind();
			buffer_sub_data(offset, size, data);
			unbind();
		}

	};

}