#pragma once

#include <iostream>

#include "i_gl_buffer.h"

namespace feng {

	class gl_buffer : public i_gl_buffer {
	public:
		gl_buffer() = default;

		gl_buffer(uint32_t type, bool autogen)
			: _type(type) {
			if (autogen) {
				generate();
				bind();
			}
		}

		void free() {
			glDeleteBuffers(1, &_id);
		}

		void generate() override {
			glGenBuffers(1, &_id);
		}

		void bind() override {
			glBindBuffer(_type, _id);
		}

		template<typename T>
		void buffer_data(uint32_t data_size, T* data, int32_t draw_mode) {
			glBufferData(_type, data_size, data, draw_mode);
		}

		template<typename T>
		void buffer_sub_data(uint32_t offset, uint32_t data_size, T* data) {
			glBufferSubData(_type, offset, data_size, data);
		}

		void bind_buffer_range(uint32_t index, uint32_t offset, uint32_t data_size) {
			glBindBufferRange(_type, index, _id, offset, data_size);
		}

		void bind_buffer_base(uint32_t index) {
			glBindBufferBase(_type, index, _id);
		}

		void unbind() override {
			glBindBuffer(_type, 0);
		}

		uint32_t id() const override {
			return _id;
		}

		int32_t type() const override {
			return _type;
		}

	protected:
		uint32_t _id;
		uint32_t _type;
	};
}