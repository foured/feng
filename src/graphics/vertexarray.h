#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace feng {
	class vertexarray {
	public:
		void generate();
		void bind();
		void set_attrib_pointer(uint32_t index, int32_t no_data_elements, GLenum element_type,
			bool normalized, int32_t data_size, uint32_t offset, int32_t devisor = 0);

		template<uint32_t columns>
		void set_atrib_pointer_matrix(uint32_t start_index, uint32_t offset, int32_t devisor = 0) {
			constexpr size_t vec_size = sizeof(float) * 4;
			constexpr size_t mat_stride = vec_size * columns;

			for (uint32_t col = 0; col < columns; ++col) {
				set_attrib_pointer(start_index + col, 4, GL_FLOAT, false, mat_stride, offset + vec_size * col, devisor);
			}
		}

		void draw_elements(int32_t mode, uint32_t no_indices);
		void draw_arrays(int32_t mode, uint32_t no_vertices);
		void draw_elements_instanced(int32_t mode, int32_t count, int32_t type, 
			const void *indices, int32_t primcount);
		void free();

		static void unbind();

	private:
		uint32_t _id;

	};
}